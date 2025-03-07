#define A_MOONLIT_WALK_MAIN
#include "lake.h"

static_assert(MAX_FRAMES_IN_FLIGHT >= 3, "AMW_MAX_FRAMES_IN_FLIGHT must be 3 or more.");

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER_MS 100

#define FRAME_TIME_PRINT_INTERVAL_MS 1000

struct engine_hints {
    u32                 frames_in_flight;

    u32                 window_width, window_height;
    const struct str    window_title;

    usize               riven_memory_budget_size;
    usize               riven_fiber_stack_size;
    u32                 riven_fiber_count;
    u32                 riven_thread_count;
    u32                 riven_tagged_heap_count;
    u32                 riven_log2_work_count;
    u32                 riven_log2_memory_count;

    struct {
        const PFN_rivens_encore    *octavia;
        const PFN_rivens_encore    *pelagia;
        const PFN_rivens_encore    *hadal;

        u32                         octavia_count;
        u32                         pelagia_count;
        u32                         hadal_count;
    } encores;

    b32                 verbose;
    b32                 debug_utilities;
};

static void lake_fini(struct lake *lake)
{
    struct str name = str_init("lake_fini");
    rivens_song_t finales[] = {
        lake->octavia,
        lake->pelagia,
        lake->hadal,
    };
    struct rivens_work init_work[3] = {
        { .procedure = riven_finale, .argument = finales[0], .name = str_init("riven_finale:hadal") },
        { .procedure = riven_finale, .argument = finales[1], .name = str_init("riven_finale:octavia") },
        { .procedure = riven_finale, .argument = finales[2], .name = str_init("riven_finale:pelagia") }
    };
    riven_split_work_and_unchain(lake->riven, init_work, 3);
}

static s32 lake_init(
    struct lake         *lake,
    struct engine_hints *hints)
{
    struct str           name = str_init("lake_init");
    struct rivens      *riven = lake->riven;
    const  rivens_tag_t   tag = rivens_tag_roots;
    const  b32          debug = hints->debug_utilities;
    {
        /* the display backend */
        struct hadal_overture hadal_info = {
            .header = riven_write_overture_header(
                riven, tag, str_init("hadal"), 
                (rivens_song_t *)&lake->hadal, 
                hints->encores.hadal, 
                hints->encores.hadal_count),
            /* display properties */
            .width = hints->window_width,
            .height = hints->window_height,
            .title = hints->window_title,
        };

        /* the rendering backend */
        struct pelagia_overture pelagia_info = {
            .header = riven_write_overture_header(
                riven, tag, str_init("pelagia"),
                (rivens_song_t *)&lake->pelagia,
                hints->encores.pelagia,
                hints->encores.pelagia_count),
            /* renderer properties */
            .debug_utilities = debug,
        };

        /* the audio backend */
        struct octavia_overture octavia_info = {
            .header = riven_write_overture_header(
                riven, tag, str_init("octavia"),
                (rivens_song_t *)&lake->octavia,
                hints->encores.octavia,
                hints->encores.octavia_count),
            /* audio engine properties */
        };

        rivens_song_t overtures[] = {
            &hadal_info,
            &octavia_info,
            &pelagia_info,
        };
        struct rivens_work init_work[3] = {
            { .procedure = riven_encore, .argument = overtures[0], .name = str_init("riven_encore:hadal") },
            { .procedure = riven_encore, .argument = overtures[1], .name = str_init("riven_encore:octavia") },
            { .procedure = riven_encore, .argument = overtures[2], .name = str_init("riven_encore:pelagia") }
        };
        riven_split_work_and_unchain(riven, init_work, 3);

        /* check the interfaces */
        for (u32 i = 0; i < arraysize(overtures); i++) {
            struct rivens_overture_header *header = (struct rivens_overture_header *)overtures[i];
            if (header->interface) continue;

            log_fatal("Can't create an interface '%s', aborting.", header->name.ptr);
            lake->exit_game = true;
        }
        if (lake->exit_game) return result_error;
    }

    /* TODO create a rendering device, create a window, etc. */

    return result_success;
}

static s32 lake_in_the_lungs(
    struct rivens       *riven,
    thread_t            *threads,
    u32                  thread_count,
    struct engine_hints *hints)
{
    s32 res = result_success;
    u64 frame_index = 0;
    u64 time_last = 0, time_now = rtc_counter();
    f64 dt = 0, dt_frequency = 1000/(f64)rtc_frequency();

    struct lake lake = {0};
    lake.riven = riven;
    lake.threads = threads;
    lake.thread_count = thread_count;

    res = lake_init(&lake, hints);
    if (res != result_success) {
        lake_fini(&lake);
        return res;
    }

    struct framedata frames[MAX_FRAMES_IN_FLIGHT];
    zeroa(frames);

    for (s32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        frames[i].result = result_success;
        frames[i].lake = &lake;
        frames[i].last_frame = &frames[(i - 1 + MAX_FRAMES_IN_FLIGHT) % MAX_FRAMES_IN_FLIGHT];
        frames[i].next_frame = &frames[(i + 1 + MAX_FRAMES_IN_FLIGHT) % MAX_FRAMES_IN_FLIGHT];
    }
    struct rivens_work work[3];
    work[LAKE_SIMULATION_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_simulation;
    work[LAKE_SIMULATION_WORK_IDX].name = str_init("lake_in_the_lungs:simulation");
    work[LAKE_RENDERING_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_rendering;
    work[LAKE_RENDERING_WORK_IDX].name = str_init("lake_in_the_lungs:rendering");
    work[LAKE_GPUEXEC_WORK_IDX].procedure = (PFN_rivens_job)lake_in_the_lungs_gpuexec;
    work[LAKE_GPUEXEC_WORK_IDX].name = str_init("lake_in_the_lungs:gpuexec");

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
                          frame_index, work[LAKE_RENDERING_WORK_IDX].name.ptr);
                lake.exit_game = true;
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
                          frame_index, work[LAKE_SIMULATION_WORK_IDX].name.ptr);
                lake.exit_game = true;
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
                          frame_index, work[LAKE_GPUEXEC_WORK_IDX].name.ptr);
                lake.exit_game = true;
            }

            /* prepare the next framedata */
            simulation->index = frame_index;
            simulation->dt = dt;
            
            /* begin the next frame */
            work[LAKE_SIMULATION_WORK_IDX].argument = simulation;
            riven_split_work(riven, &work[LAKE_SIMULATION_WORK_IDX], 1, &simulation->chain);
        }

        /* XXX delete the close_counter later */
        close_counter -= dt;
        if ((lake.exit_game || close_counter <= 0))
            lake.finalize_gameloop = true;

        /* rotate the framedata */
        gpuexec = rendering;
        rendering = simulation;
        simulation = lake.finalize_gameloop ? NULL : &frames[(frame_index++) % MAX_FRAMES_IN_FLIGHT];

        print_frame_time(FRAME_TIME_PRINT_INTERVAL_MS);
    }

    /* wait for any existing work to finish */
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        riven_unchain(riven, frames[i].chain);

    dt = median_frame_time();
    log_info("Last recorded frame time: %.3f ms (%.0f FPS)", dt, 1000/dt);

    if (lake.restart_engine && res == result_success)
        res = result_continue;
    lake_fini(&lake);
    return res;
}

s32 amw_main(s32 argc, char **argv)
{
    s32 res = result_success;
    struct engine_hints hints = {
        .frames_in_flight = MAX_FRAMES_IN_FLIGHT,
        .window_width = 1200,
        .window_height = 900,
        .window_title = str_init("UwU miau mlem"),
        .riven_memory_budget_size = 0,
        .riven_fiber_stack_size = 96*1024,
        .riven_fiber_count = 0,
        .riven_thread_count = 1,
        .riven_tagged_heap_count = 0,
        .riven_log2_work_count = 11,
        .riven_log2_memory_count = 9,

        .encores = {
            .octavia = octavia_acquire_native_encores(&hints.encores.octavia_count, true),
            .pelagia = pelagia_acquire_native_encores(&hints.encores.pelagia_count, true),
            .hadal = hadal_acquire_native_encores(&hints.encores.hadal_count, true),
        },
#ifndef NDEBUG
        .verbose = true,
#endif
#ifdef DEBUG
        .debug_utilities = true,
#endif
    };
    log_set_verbose(argc-1);

    /* TODO parse the command line */
    (void)argc;
    (void)argv;

    assert_debug(hints.frames_in_flight <= MAX_FRAMES_IN_FLIGHT && hints.frames_in_flight > 0);

    struct rivens_metadata metadata = {
        .engine_name = str_init("A Moonlit Walk Engine"),
        .engine_build_version = LAKE_VERSION, /* XXX */
        .game_name = str_init("Lake in the Lungs"),
        .game_build_version = LAKE_VERSION,
    };

    /* go for a walk */
    do {res = riven_moonlit_walk(
            hints.riven_memory_budget_size,
            hints.riven_fiber_stack_size,
            hints.riven_fiber_count,
            hints.riven_thread_count,
            hints.riven_tagged_heap_count,
            hints.riven_log2_work_count,
            hints.riven_log2_memory_count,
            &metadata,
            (PFN_rivens_heart)lake_in_the_lungs,
            (rivens_song_t)&hints);
    } while (res == result_continue);
    return res;
}
