#include <lake/a_moonlit_walk.h>

static void a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW)
{
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

#if 0
        AMW.hadal = hadal_init(
            hints->init.hadal_api,
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
        hadal_visible(AMW.hadal, true);
#endif
    }

    struct riven_tear tears[3];
    for (i = 0; i < 3; i++) {
        tears[i].argument = NULL;
    }

    /* TODO run 128 frames and exit */
    int32_t close_counter = 128;

    log_debug("A MOONLIT WALK - GAMELOOP - - - BEGIN");
    do {
        time_last = time_now;
        time_now = hadal_timer_counter();
        dt = (double)((time_now - time_last) * dt_frequency); /* deltatime in ms */

        /* execute the mainloop tears */
        riven_split_and_unchain(riven, tears, 3);

        //gpuexec_frame = rendering_frame;
        //rendering_frame = simulation_frame;
        //simulation_frame = (at_read_relaxed(&AMW.flags) & amw_flag_dont_continue_work) 
        //    == amw_flag_dont_continue_work ? NULL : &frames[(++frame_idx) % AMW_MAX_FRAMES];

        /* TODO run 128 frames and exit */
        close_counter--;
        if (close_counter < 0) // && simulation_frame)
            at_fetch_or_relaxed(&AMW.flags, amw_flag_dont_continue_work);

    } while (false);
    //} while (!(at_read_relaxed(&AMW.flags) & (amw_flag_forced_exit)) && (simulation_frame || rendering_frame || gpuexec_frame));
    log_debug("MAINLOOP - GAMELOOP - - - END %s", at_read_relaxed(&AMW.flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(&AMW);

    log_info("Application was running for a total of:  %llu min, %02llu sec, %03lu ms", 
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, ticks_ms()%AMW_MS_PER_SECOND);
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

        if (hints.riven.thread_count == 0)  hadal_cpu_count((size_t *)&hints.riven.thread_count, NULL, NULL);
        if (hints.riven.fiber_count == 0)   hints.riven.fiber_count = 128;
        if (hints.riven.stack_size == 0)    hints.riven.stack_size = 64 * 1024;
        if (hints.riven.log_2_tears == 0)   hints.riven.log_2_tears = 12;

        if (!hints.callbacks.init ||
            !hints.callbacks.simulation ||
            !hints.callbacks.rendering ||
            !hints.callbacks.gpuexec ||
            !hints.callbacks.cleanup)
        {
            log_fatal("The application should setup all callbacks (init, simulation, rendering, gpuexec, cleanup). "
                      "They can be empty procedures, just can't be NULL pointers.");
            return 1;
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
