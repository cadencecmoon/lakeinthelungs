#include <lake/a_moonlit_walk.h>

#include "framework.h"

#include <stdlib.h> /* malloc, free */

static void AMWAPIENTRY a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW)
{
    platinum_fini(&AMW->plat);
    hadopelagic_fini(&AMW->hadal);
    iafini(&AMW->ia);
    iazerop(AMW);
}

static void AMWAPIENTRY a_moonlit_walk_main__(
        struct riven *riven,
        thread_t *threads,
        size_t thread_count,
        void *argument)
{
    int32_t i, frame_idx = 0;
    uint32_t time_last = 0, time_now = hadal_timer_counter();
    double dt_frequency = 1000.0/(double)hadal_timer_frequency(); /* in ms */
    double dt = 0;

    struct a_moonlit_walk *AMW = (struct a_moonlit_walk *)argument;

    AMW->riven = riven;
    AMW->threads = threads;
    AMW->thread_count = thread_count;

    i = hadopelagic_init(
        AMW->hints.entry_points.hadopelagic,
        &AMW->hadal,
        &AMW->ia,
        AMW->hints.window.width, 
        AMW->hints.window.height,
        AMW->hints.window.title
    );
    if (i != result_success) {
        log_fatal("Can't initialize the display backend.");
        a_moonlit_walk_cleanup__(AMW);
        return; 
    }
    hadopelagic_window_visible(&AMW->hadal, true);

    i = platinum_init(AMW->hints.entry_points.platinum, &AMW->plat, &AMW->ia, &AMW->hadal, AMW->hints.app_name, AMW->hints.version, thread_count);
    if (i != result_success) {
        log_fatal("Can't initialize the rendering backend.");
        a_moonlit_walk_cleanup__(AMW);
        return;
    }
    i = platinum_construct_novas(&AMW->plat, riven, -1, 1, true);
    if (i != result_success) {
        log_fatal("Creating rendering devices failed. No rendering is possible.");
        a_moonlit_walk_cleanup__(AMW);
        return;
    }

    struct amw_workload work[AMW_MAX_WORKLOAD];
    for (i = 0; i < AMW_MAX_WORKLOAD; i++) {
        work[i].idx = i;
        work[i].dt = 0.0;
        work[i].AMW = AMW;
        work[i].previous = &work[(i - 1 + AMW_MAX_WORKLOAD) % AMW_MAX_WORKLOAD];
    }

    struct riven_tear tears[3];
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
    int32_t close_counter = 128;

    log_debug("A MOONLIT WALK - GAMELOOP - - - BEGIN");
    do {
        time_last = time_now;
        time_now = hadal_timer_counter();
        dt = (double)((time_now - time_last) * dt_frequency); /* deltatime in ms */

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

        gpuexec_workload = rendering_workload;
        rendering_workload = simulation_workload;
        simulation_workload = (at_read_relaxed(&AMW->flags) & amw_flag_finalize_gameloop) 
            == amw_flag_finalize_gameloop ? NULL : &work[(++frame_idx) % AMW_MAX_WORKLOAD];

        /* TODO run X frames and exit */
        close_counter--;
        if (close_counter < 0) // && simulation_frame)
            at_fetch_or_relaxed(&AMW->flags, amw_flag_finalize_gameloop);

    } while (!(at_read_relaxed(&AMW->flags) & (amw_flag_forced_exit)) && (simulation_workload || rendering_workload || gpuexec_workload));
    log_debug("MAINLOOP - GAMELOOP - - - END %s", at_read_relaxed(&AMW->flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(AMW);

    log_info("%s was running for a total of:\n"
             "    %llu min, %02llu sec, %03lu ms\n"
             "    %u frames", ((struct amw_hints *)argument)->app_name,
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, ticks_ms()%AMW_MS_PER_SECOND, frame_idx-1);
}

AMWAPI int32_t AMWAPIENTRY 
a_moonlit_walk(int32_t (*main__)(struct amw_hints *, int32_t, char **), int32_t argc, char **argv)
{
    int res = 0;
    size_t riven_bytes;

    struct a_moonlit_walk AMW;
    iazero(AMW);

    /* parse arguments, configure the engine */
    res = main__(&AMW.hints, argc, argv);
    if (res != result_success) /* don't continue ? */
        return res;

    { /* provide defaults and validate state if necessary */
        if (AMW.hints.app_name == NULL)         AMW.hints.app_name = "undefined";
        if (AMW.hints.version == 0)             AMW.hints.version = AMW_VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);

        if (AMW.hints.window.width <= 0)        AMW.hints.window.width = 1200;
        if (AMW.hints.window.height <= 0)       AMW.hints.window.height = 800;
        if (AMW.hints.window.title == NULL)     AMW.hints.window.title = "A Moonlit Walk Engine";

        if (AMW.hints.riven.thread_count == 0)  hadal_cpu_count(&AMW.hints.riven.thread_count, NULL, NULL);
        if (AMW.hints.riven.fiber_count == 0)   AMW.hints.riven.fiber_count = 128;
        if (AMW.hints.riven.stack_size == 0)    AMW.hints.riven.stack_size = 64 * 1024;
        if (AMW.hints.riven.log_2_tears == 0)   AMW.hints.riven.log_2_tears = 12;

        if (AMW.hints.entry_points.hadopelagic == NULL) AMW.hints.entry_points.hadopelagic = hadopelagic_entry_point;
        if (AMW.hints.entry_points.platinum == NULL)    AMW.hints.entry_points.platinum = platinum_entry_point;
        if (AMW.hints.entry_points.silver == NULL)      AMW.hints.entry_points.silver = silver_entry_point;

        if (!AMW.hints.callbacks.init ||
            !AMW.hints.callbacks.simulation ||
            !AMW.hints.callbacks.rendering ||
            !AMW.hints.callbacks.gpuexec ||
            !AMW.hints.callbacks.cleanup)
        {
            log_fatal("The application should setup all callbacks (init, simulation, rendering, gpuexec, cleanup).");
            return result_error_undefined; // TODO
        }
    }

    riven_bytes = riven_unveil_rift(
        NULL,                       /* Null to get the required memory buffer size. */
        AMW.hints.riven.stack_size,     /* Size of stack per fiber in bytes. */
        AMW.hints.riven.fiber_count,    /* How many fibers to instance. */
        AMW.hints.riven.log_2_tears,    /* Job queue will be (1u << log2_tears) entries large. */
        AMW.hints.riven.thread_count,   /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,      /* Main riven tear, serves as the entry point. */
        &AMW);                    /* Argument holding hints for other systems' configuration */
    void *riven_memory = malloc(riven_bytes);

    riven_unveil_rift(
        riven_memory,
        AMW.hints.riven.stack_size,
        AMW.hints.riven.fiber_count,
        AMW.hints.riven.log_2_tears,
        AMW.hints.riven.thread_count,
        a_moonlit_walk_main__,
        &AMW);
    free(riven_memory);

    return res;
}
