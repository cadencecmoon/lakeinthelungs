#include <lake/a_moonlit_walk.h>

#include "framework.h"

#include <stdlib.h> /* malloc, free */

static void a_moonlit_walk_cleanup__(
    struct a_moonlit_walk *AMW)
{
    cobalt_fini(&AMW->cobalt);
    hadal_fini(&AMW->hadal);
    iafini(&AMW->ia);
}

static void a_moonlit_walk_main__(
    struct riven   *riven,
    thread_id      *threads,
    ssize           thread_count,
    void           *argument)
{
    s32 i, frame_idx = 0;
    u32 time_last = 0, time_now = bedrock_rtc_counter();
    f64 dt_frequency = 1000.0/(f64)bedrock_rtc_frequency(); /* in ms */
    f64 dt = 0;

    struct a_moonlit_walk *AMW = (struct a_moonlit_walk *)argument;

    AMW->riven = riven;
    AMW->threads = threads;
    AMW->thread_count = thread_count;

    i = hadal_init(
        AMW->hints->entry_points.hadal,
        &AMW->hadal,
        &AMW->ia,
        AMW->hints->window_width, 
        AMW->hints->window_height,
        AMW->hints->window_title
    );
    if (i != result_success) {
        log_fatal("Can't initialize the display backend.");
        a_moonlit_walk_cleanup__(AMW);
        return; 
    }
    hadal_window_visible(&AMW->hadal, true);

    i = cobalt_init(
        AMW->hints->entry_points.cobalt,
        &AMW->cobalt, 
        &AMW->ia, 
        &AMW->hadal, 
        riven, 
        AMW->hints->app_name, 
        AMW->hints->version, 
        threads,
        thread_count,
        AMW->hints->cobalt_preferred_main_device,
        AMW->hints->cobalt_max_devices,
        AMW->hints->cobalt_enable_vsync
    );
    if (i != result_success) {
        log_fatal("Can't initialize the renderer.");
        a_moonlit_walk_cleanup__(AMW);
        return;
    }

    struct amw_workload work[AMW_MAX_WORKLOAD];
    for (i = 0; i < AMW_MAX_WORKLOAD; i++) {
        work[i].idx = i;
        work[i].dt = 0.0;
        work[i].AMW = AMW;
        work[i].last_work = &work[(i - 1 + AMW_MAX_WORKLOAD) % AMW_MAX_WORKLOAD];
    }

    struct rivens_tear tears[3];
    for (i = 0; i < 3; i++) {
        tears[i].argument = NULL;
    }
    tears[AMW_SIMULATION_TEAR_IDX].procedure = (PFN_riven_tear)a_moonlit_walk_simulation_tear__;
    tears[AMW_SIMULATION_TEAR_IDX].name = "a_moonlit_walk_simulation_tear__";

    tears[AMW_RENDERING_TEAR_IDX].procedure = (PFN_riven_tear)a_moonlit_walk_rendering_tear__;
    tears[AMW_RENDERING_TEAR_IDX].name = "a_moonlit_walk_rendering_tear__";

    tears[AMW_GPUEXEC_TEAR_IDX].procedure = (PFN_riven_tear)a_moonlit_walk_gpuexec_tear__;
    tears[AMW_GPUEXEC_TEAR_IDX].name = "a_moonlit_walk_gpuexec_tear__";

    struct amw_workload *simulation_workload = &work[frame_idx % AMW_MAX_WORKLOAD];
    struct amw_workload *rendering_workload = NULL;
    struct amw_workload *gpuexec_workload = NULL;

    /* TODO run X frames and exit */
    s32 close_counter = 4096;

    log_debug("A MOONLIT WALK - GAMELOOP - - - BEGIN");
    do {
        time_last = time_now;
        time_now = bedrock_rtc_counter();
        dt = (f64)((time_now - time_last) * dt_frequency); /* deltatime in ms */

        /* GAME WORLD SIMULATION */
        tears[AMW_SIMULATION_TEAR_IDX].argument = simulation_workload;
        if (simulation_workload) {
            simulation_workload->idx = frame_idx;
            simulation_workload->dt = dt;
        }

        /* VISUAL & AUDITORY RENDERING */
        tears[AMW_RENDERING_TEAR_IDX].argument = rendering_workload;
        if (rendering_workload) {
            /* TODO */
        }

        /* GPU EXECUTION OF COMMAND BUFFERS */
        tears[AMW_GPUEXEC_TEAR_IDX].argument = gpuexec_workload;
        if (gpuexec_workload) {
            /* TODO */
        }

        /* execute the mainloop tears */
        riven_split_and_unchain(riven, tears, 3);

        /* increment the frame counter */
        gpuexec_workload = rendering_workload;
        rendering_workload = simulation_workload;
        simulation_workload = (at_read_relaxed(&AMW->flags) & amw_flag_finalize_gameloop) 
            == amw_flag_finalize_gameloop ? NULL : &work[(++frame_idx) % AMW_MAX_WORKLOAD];

        /* TODO run X frames and exit */
        close_counter--;
        if (close_counter < 0) // && simulation_frame)
            at_fetch_or_relaxed(&AMW->flags, amw_flag_finalize_gameloop);

    } while (!(at_read_relaxed(&AMW->flags) & (amw_flag_forced_exit)) && (simulation_workload || rendering_workload || gpuexec_workload));
    log_debug("A MOONLIT WALK - GAMELOOP - - - END %s", at_read_relaxed(&AMW->flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(AMW);

    log_info("%s was running for a total of: %llu:%02llu.%03lu min, %u frames", AMW->hints->app_name,
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, (ticks_ms()%AMW_MS_PER_SECOND), frame_idx-1);
}

AMWAPI s32 a_moonlit_walk(
    s32 (*main__)(struct amw_hints *, s32, char **), 
    s32 argc, char **argv)
{
    s32 res = 0;
    usize riven_bytes;

    struct a_moonlit_walk AMW;
    iazero(AMW);

    log_set_verbose(false);
    ticks_init();

    struct amw_hints hints = {
        .app_name = "A Moonlit Walk Engine",
        .version = AMW_VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION),
        .window_width = 1200,
        .window_height = 800,
        .window_title = hints.app_name,
        .riven_thread_count = 0,
        .riven_fiber_count = 128,
        .riven_stack_size = 64 * 1024,
        .riven_log_2_tears = 12,
        .cobalt_max_devices = 1, /* if 0, use all available GPUs */
        .cobalt_preferred_main_device = -1, /* select the most appropriate GPU as the main device */
        .cobalt_enable_vsync = false,
        .entry_points.hadal = hadal_entry_point,
        .entry_points.cobalt = cobalt_entry_point,
        .entry_points.silv = silver_entry_point,
        .callbacks.init = NULL,
        .callbacks.simulation = NULL,
        .callbacks.rendering = NULL,
        .callbacks.gpuexec = NULL,
        .callbacks.cleanup = NULL,
    };
    bedrock_cpu_count(&hints.riven_thread_count, NULL, NULL);

    /* parse arguments, configure the engine */
    res = main__(&hints, argc, argv);
    if (res != result_success) /* don't continue ? */
        return res;
    AMW.hints = &hints;

    riven_bytes = riven_unveil_rift(
        NULL,                           /* Null to get the required memory buffer size. */
        AMW.hints->riven_stack_size,    /* Size of stack per fiber in bytes. */
        AMW.hints->riven_fiber_count,   /* How many fibers to instance. */
        AMW.hints->riven_log_2_tears,   /* Job queue will be (1u << log2_tears) entries large. */
        AMW.hints->riven_thread_count,  /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,          /* Main riven tear, serves as the entry point. */
        &AMW);                          /* The engine context as the argument. */
    void *riven_memory = malloc(riven_bytes);

    riven_unveil_rift(
        riven_memory,
        AMW.hints->riven_stack_size,
        AMW.hints->riven_fiber_count,
        AMW.hints->riven_log_2_tears,
        AMW.hints->riven_thread_count,
        a_moonlit_walk_main__,
        &AMW);
    free(riven_memory);

    return res;
}
