#define A_MOONLIT_WALK_MAIN
#include "lake.h"

static_assert(AMW_MAX_FRAMES_IN_FLIGHT >= 3, "AMW_MAX_FRAMES_IN_FLIGHT must be 3 or more.");

#define DEBUG_CLOSE_COUNTER_MS 100

#define FRAME_TIME_PRINT_INTERVAL_MS 1000

struct engine_hints {
    const char *engine_name;
    const char *game_name;
    u32         build_version;
    u32         frames_in_flight;

    usize       riven_memory_budget_size;
    usize       riven_fiber_stack_size;
    u32         riven_fiber_count;
    u32         riven_thread_count;
    u32         riven_tagged_heap_count;
    u32         riven_log2_work_count;
    u32         riven_log2_memory_count;
};

static void lake_fini(struct lake *lake)
{
    (void)lake;
}

static s32 lake_init(
    struct lake         *lake,
    struct engine_hints *hints)
{
    (void)lake;
    (void)hints;
    return result_success;
}

static s32 lake_in_the_lungs(
    struct rivens       *riven, 
    const u32            thread_count, 
    struct engine_hints *hints)
{
    s32 res = result_success;
    u64 frame_index = 0;
    u64 time_last = 0, time_now = rtc_counter();
    f64 dt = 0, dt_frequency = 1000/(f64)rtc_frequency();

    struct lake *lake = (struct lake *)riven_alloc(riven, rivens_tag_roots, sizeof(struct lake), 16);
    zerop(lake);

    lake->riven = riven;
    lake->thread_count = thread_count;

    res = lake_init(lake, hints);
    if (res != result_success) return res;

    struct framedata frames[AMW_MAX_FRAMES_IN_FLIGHT];
    zeroa(frames);

    for (s32 i = 0; i < AMW_MAX_FRAMES_IN_FLIGHT; i++) {
        frames[i].result = result_success;
        frames[i].type = (i == 0) ? work_type_assembly : work_type_continue;
        frames[i].lake = lake;
        frames[i].last_frame = &frames[(i - 1 + AMW_MAX_FRAMES_IN_FLIGHT) % AMW_MAX_FRAMES_IN_FLIGHT];
        frames[i].next_frame = &frames[(i + 1 + AMW_MAX_FRAMES_IN_FLIGHT) % AMW_MAX_FRAMES_IN_FLIGHT];
    }
    struct rivens_work work[3];
    work[LAKE_SIMULATION_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_simulation;
    work[LAKE_SIMULATION_WORK_IDX].name = "lake_in_the_lungs:simulation";
    work[LAKE_RENDERING_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_rendering;
    work[LAKE_RENDERING_WORK_IDX].name = "lake_in_the_lungs:rendering";
    work[LAKE_GPUEXEC_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_gpuexec;
    work[LAKE_GPUEXEC_WORK_IDX].name = "lake_in_the_lungs:gpuexec";

    struct framedata *simulation = frames;
    struct framedata *rendering = NULL;
    struct framedata *gpuexec = NULL;

    /* XXX delete later */
    f64 close_counter = DEBUG_CLOSE_COUNTER_MS;

    while (simulation || rendering || gpuexec) {
        time_last = time_now;
        time_now = rtc_counter();
        dt = (f64)((time_now - time_last) * dt_frequency);
        record_frame_time(time_now, dt_frequency);

        /* GPU EXECUTION for frame N-2 */
        if (gpuexec) {
            /* gpuexec points to the frame that may currently be in the rendering stage */
            riven_unchain(riven, gpuexec->chain);
            gpuexec->chain = NULL;

            /* check results of last stage (rendering) */
            if (gpuexec->result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_RENDERING_WORK_IDX].name);
                lake->exit_game = true;
            }

            /* feed forward to GPU execution */
            work[LAKE_GPUEXEC_WORK_IDX].argument = gpuexec;
            riven_split_work(riven, &work[LAKE_GPUEXEC_WORK_IDX], 1, &gpuexec->chain);
        }

        /* RENDERING for frame N-1 */
        if (rendering) {
            /* rendering points to the frame that may currently be in the simulation stage */
            riven_unchain(riven, rendering->chain);
            rendering->chain = NULL;

            /* check results of last stage (simulation) */
            if (rendering->result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_SIMULATION_WORK_IDX].name);
                lake->exit_game = true;
            }

            /* feed forward to rendering */
            work[LAKE_RENDERING_WORK_IDX].argument = rendering;
            riven_split_work(riven, &work[LAKE_RENDERING_WORK_IDX], 1, &rendering->chain);
        }

        /* SIMULATION for current frame */
        if (simulation) {
            /* simulation points to the frame that may not yet completed the pipeline */
            riven_unchain(riven, simulation->chain);
            simulation->chain = NULL;

            /* check results of last stage (gpuexec) */
            if (simulation->result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_GPUEXEC_WORK_IDX].name);
                lake->exit_game = true;
            }

            /* prepare the next framedata */
            simulation->index = frame_index;
            simulation->dt = dt;
            
            /* begin the next frame */
            work[LAKE_SIMULATION_WORK_IDX].argument = simulation;
            riven_split_work(riven, &work[LAKE_SIMULATION_WORK_IDX], 1, &simulation->chain);

            if (simulation->type == work_type_disassembly)
                lake->finalize_gameloop = true;
        }

        /* rotate the framedata */
        gpuexec = rendering;
        rendering = simulation;
        simulation = !lake->finalize_gameloop ? &frames[(frame_index++) % AMW_MAX_FRAMES_IN_FLIGHT] : NULL;

        /* XXX delete the close_counter later */
        close_counter -= dt;
        if (simulation && (lake->exit_game || close_counter <= 0))
            simulation->type = work_type_disassembly;

        print_frame_time(FRAME_TIME_PRINT_INTERVAL_MS);
    }
    dt = median_frame_time();
    log_info("Last recorded frame time: %.3f ms (%.0f FPS)", dt, 1000/dt);
    lake_fini(lake);

    if (lake->restart_engine && res == result_success)
        res = result_reiterate;
    return res;
}

s32 amw_main(s32 argc, char **argv)
{
    s32 res = result_success;
    struct engine_hints hints = {
        .engine_name = "A Moonlit Walk Engine",
        .game_name = "Lake in the Lungs", /* XXX localize the name? */
        .build_version = LAKE_VERSION,
        .frames_in_flight = AMW_MAX_FRAMES_IN_FLIGHT,
        .riven_memory_budget_size = 0,
        .riven_fiber_stack_size = 96*1024,
        .riven_fiber_count = 0,
        .riven_thread_count = 0,
        .riven_tagged_heap_count = 0,
        .riven_log2_work_count = 11,
        .riven_log2_memory_count = 9,
    };
    log_set_verbose(argc-1);

    /* TODO parse the command line */
    (void)argc;
    (void)argv;

    assert_debug(hints.frames_in_flight <= AMW_MAX_FRAMES_IN_FLIGHT && hints.frames_in_flight > 0);

    /* go for a walk */
    do {res = riven_moonlit_walk(
            hints.riven_memory_budget_size,
            hints.riven_fiber_stack_size,
            hints.riven_fiber_count,
            hints.riven_thread_count,
            hints.riven_tagged_heap_count,
            hints.riven_log2_work_count,
            hints.riven_log2_memory_count,
            (PFN_rivens_heart)lake_in_the_lungs,
            (rivens_arg_t)&hints);
    } while (res == result_reiterate);
    return res;
}
