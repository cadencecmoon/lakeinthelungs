#include <lake/a_moonlit_walk.h>

#include "framework.h"

#include <stdlib.h> /* malloc, free */

static void a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW)
{
    platinum_fini(&AMW->plat);
    hadal_fini(AMW->hadal);
    iazerop(AMW);
}

static void a_moonlit_walk_main__(
        struct riven *riven,
        thread_t *threads,
        size_t thread_count,
        void *argument)
{
    int32_t i, frame_idx = 0;
    uint32_t time_last = 0, time_now = hadal_timer_counter();
    double dt_frequency = 1000.0/(double)hadal_timer_frequency(); /* in ms */
    double dt = 0;

    struct a_moonlit_walk AMW;
    iazero(AMW);

    { /* engine initialization */
        struct amw_hints *hints = (struct amw_hints *)argument;

        AMW.riven = riven;
        AMW.threads = threads;
        AMW.thread_count = thread_count;
        AMW.callbacks = hints->callbacks;

        AMW.hadal = hadal_init(
            hints->init.hadal_backend,
            hints->window.width, 
            hints->window.height,
            hints->window.title,
            hints->init.allow_headless_display
        );
        if (!AMW.hadal) {
            log_fatal("Can't initialize the display backend.");
            a_moonlit_walk_cleanup__(&AMW);
            return; 
        }
        hadal_window_visible(AMW.hadal, true);

        i = platinum_init(&AMW.plat, AMW.hadal, hints->app_name, hints->version, thread_count);
        if (i != result_success) {
            log_fatal("Can't initialize the rendering backend.");
            a_moonlit_walk_cleanup__(&AMW);
            return;
        }
        i = platinum_construct_novas(&AMW.plat, -1, 1);
        if (i != result_success) {
            log_fatal("Creating rendering devices failed. No rendering is possible.");
            a_moonlit_walk_cleanup__(&AMW);
            return;
        }
    }

    struct amw_framedata frames[AMW_MAX_FRAMES];
    for (i = 0; i < AMW_MAX_FRAMES; i++) {
        frames[i].idx = i;
        frames[i].dt = 0.0;
        frames[i].AMW = &AMW;
        frames[i].previous = &frames[(i - 1 + AMW_MAX_FRAMES) % AMW_MAX_FRAMES];
    }

    struct riven_tear tears[3];
    for (i = 0; i < 3; i++) {
        tears[i].argument = NULL;
    }
    tears[AMW_SIMULATION_TEAR_IDX].procedure = a_moonlit_walk_simulation_tear__;
    tears[AMW_SIMULATION_TEAR_IDX].name = "a_moonlit_walk_simulation_tear__";

    tears[AMW_RENDERING_TEAR_IDX].procedure = a_moonlit_walk_rendering_tear__;
    tears[AMW_RENDERING_TEAR_IDX].name = "a_moonlit_walk_rendering_tear__";

    tears[AMW_GPUEXEC_TEAR_IDX].procedure = a_moonlit_walk_gpuexec_tear__;
    tears[AMW_GPUEXEC_TEAR_IDX].name = "a_moonlit_walk_gpuexec_tear__";

    struct amw_framedata *simulation_frame = &frames[frame_idx % AMW_MAX_FRAMES];
    struct amw_framedata *rendering_frame = NULL;
    struct amw_framedata *gpuexec_frame = NULL;

    /* TODO run X frames and exit */
    int32_t close_counter = 128;

    log_debug("A MOONLIT WALK - GAMELOOP - - - BEGIN");
    do {
        time_last = time_now;
        time_now = hadal_timer_counter();
        dt = (double)((time_now - time_last) * dt_frequency); /* deltatime in ms */

        /* GAME WORLD SIMULATION */
        tears[AMW_SIMULATION_TEAR_IDX].argument = simulation_frame;
        if (simulation_frame) {
            simulation_frame->idx = frame_idx;
            simulation_frame->dt = dt;
        }

        /* VISUAL & AUDITORY RENDERING */
        tears[AMW_RENDERING_TEAR_IDX].argument = rendering_frame;
        if (rendering_frame) {
            /* TODO */
        }

        /* GPU EXECUTION OF COMMAND BUFFERS */
        tears[AMW_GPUEXEC_TEAR_IDX].argument = gpuexec_frame;
        if (gpuexec_frame) {
            /* TODO */
        }

        /* execute the mainloop tears */
        riven_split_and_unchain(riven, tears, 3);

        gpuexec_frame = rendering_frame;
        rendering_frame = simulation_frame;
        simulation_frame = (at_read_relaxed(&AMW.flags) & amw_flag_finalize_gameloop) 
            == amw_flag_finalize_gameloop ? NULL : &frames[(++frame_idx) % AMW_MAX_FRAMES];

        /* TODO run X frames and exit */
        close_counter--;
        if (close_counter < 0) // && simulation_frame)
            at_fetch_or_relaxed(&AMW.flags, amw_flag_finalize_gameloop);

    } while (!(at_read_relaxed(&AMW.flags) & (amw_flag_forced_exit)) && (simulation_frame || rendering_frame || gpuexec_frame));
    log_debug("MAINLOOP - GAMELOOP - - - END %s", at_read_relaxed(&AMW.flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(&AMW);

    log_info("Application was running for a total of:    %llu min, %02llu sec, %03lu ms    %u frames", 
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, ticks_ms()%AMW_MS_PER_SECOND, frame_idx-1);
}

int32_t a_moonlit_walk(
        int32_t (*main__)(struct amw_hints *, int32_t, char **), 
        int32_t argc, char **argv)
{
    int res = 0;
    size_t riven_bytes;

    struct amw_hints hints;
    iazero(hints);

    /* parse arguments, configure the engine */
    res = main__(&hints, argc, argv);
    if (res != 0) /* don't continue ? */
        return res;

    { /* validate the hints, provide defaults */
        if (hints.app_name == NULL)         hints.app_name = "undefined";
        if (hints.version == 0)             hints.version = AMW_VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);

        if (hints.window.width <= 0)        hints.window.width = 1200;
        if (hints.window.height <= 0)       hints.window.height = 800;
        if (hints.window.title == NULL)     hints.window.title = "A Moonlit Walk Engine";

        if (hints.riven.thread_count == 0)  hadal_cpu_count(&hints.riven.thread_count, NULL, NULL);
        if (hints.riven.fiber_count == 0)   hints.riven.fiber_count = 128;
        if (hints.riven.stack_size == 0)    hints.riven.stack_size = 64 * 1024;
        if (hints.riven.log_2_tears == 0)   hints.riven.log_2_tears = 12;

        if (!hints.callbacks.init ||
            !hints.callbacks.simulation ||
            !hints.callbacks.rendering ||
            !hints.callbacks.gpuexec ||
            !hints.callbacks.cleanup)
        {
            log_fatal("The application should setup all callbacks (init, simulation, rendering, gpuexec, cleanup).");
            return result_error_undefined; // TODO
        }
    }

    riven_bytes = riven_unveil_rift(
        NULL,                       /* Null to get the required memory buffer size. */
        hints.riven.stack_size,     /* Size of stack per fiber in bytes. */
        hints.riven.fiber_count,    /* How many fibers to instance. */
        hints.riven.log_2_tears,    /* Job queue will be (1u << log2_tears) entries large. */
        hints.riven.thread_count,   /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,      /* Main riven tear, serves as the entry point. */
        &hints);                    /* Argument holding hints for other systems' configuration */
    void *riven = malloc(riven_bytes);

    riven_unveil_rift(
        riven,
        hints.riven.stack_size,
        hints.riven.fiber_count,
        hints.riven.log_2_tears,
        hints.riven.thread_count,
        a_moonlit_walk_main__,
        &hints);
    free(riven);

    return res;
}
