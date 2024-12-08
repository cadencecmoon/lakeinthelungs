/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/a_moonlit_walk.h>

static void a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW)
{
    hadal_fini(AMW->hadal);
}

#define NEXT_FRAME(idx)     (((idx) + 1) % AMW_MAX_FRAMES)
#define CURRENT_FRAME(idx)  ((idx) % AMW_MAX_FRAMES)
#define PREV_FRAME(idx)     (((idx) - 1 + AMW_MAX_FRAMES) % AMW_MAX_FRAMES)
#define LAST_FRAME(idx)     (((idx) - 2 + AMW_MAX_FRAMES) % AMW_MAX_FRAMES)

static void a_moonlit_walk_main__(
        struct riven *riven,
        thread_t *threads,
        size_t thread_count,
        riven_argument_t argument)
{
    struct a_moonlit_walk AMW;
    iazero(AMW);

    /* TODO */
    (void)threads;
    (void)thread_count;

    AMW.riven = riven;

    { /* engine initialization */
        struct application_config *config = (struct application_config *)argument;

        AMW.hadal = hadal_init(
            config->hadal.api,
            config->hadal.width, 
            config->hadal.height,
            config->hadal.title,
            config->hadal.allow_headless
        );
        if (!AMW.hadal) {
            log_fatal("Can't initialize the display backend.");
            a_moonlit_walk_cleanup__(&AMW);
            return; 
        }
    }

    i32 i, frame_idx = 0;
    u32 time_last = 0, time_now = ticks_counter();
    f64 dt_frequency = 1000.0/(f64)ticks_frequency(); /* in ms */
    f64 dt = 0;

    /* TODO run 120 frames and exit */
    i32 close_counter = 120;

    struct framedata frames[AMW_MAX_FRAMES];
    for (i = 0; i < AMW_MAX_FRAMES; i++) {
        frames[i].frame_idx = i;
        frames[i].delta_time = 0.0;
        frames[i].AMW = &AMW;
        frames[i].previous_frame = &frames[PREV_FRAME(i)];
    }

    struct riven_tear tears[3];

    tears[AMW_SIMULATION_TEAR_IDX].procedure = a_moonlit_walk_simulation_tear__;
    tears[AMW_SIMULATION_TEAR_IDX].name = "a_moonlit_walk_simulation_tear__";

    tears[AMW_RENDERING_TEAR_IDX].procedure = a_moonlit_walk_rendering_tear__;
    tears[AMW_RENDERING_TEAR_IDX].name = "a_moonlit_walk_rendering_tear__";

    tears[AMW_GPUEXEC_TEAR_IDX].procedure = a_moonlit_walk_gpuexec_tear__;
    tears[AMW_GPUEXEC_TEAR_IDX].name = "a_moonlit_walk_gpuexec_tear__";

    for (i = 0; i < 3; i++) {
        tears[i].argument = NULL;
    }

    struct framedata *simulation_frame = &frames[CURRENT_FRAME(frame_idx)];
    struct framedata *rendering_frame = NULL;
    struct framedata *gpuexec_frame = NULL;

    log_debug("MAINLOOP - - - BEGIN");
    do {
        time_last = time_now;
        time_now = ticks_counter();
        dt = (f64)((time_now - time_last) * dt_frequency); /* deltatime in ms */

        { /* GAME WORLD SIMULATION */
            simulation_frame->delta_time = dt;
            tears[AMW_SIMULATION_TEAR_IDX].argument = simulation_frame;
        }
        
        { /* VISUAL & AUDITORY RENDERING */
            tears[AMW_RENDERING_TEAR_IDX].argument = rendering_frame;
        }

        { /* GPU EXECUTION OF COMMAND BUFFERS */
            tears[AMW_GPUEXEC_TEAR_IDX].argument = gpuexec_frame;
        }

        /* execute the mainloop tears */
        riven_split_and_unchain(riven, tears, 3);

        gpuexec_frame = rendering_frame;
        rendering_frame = simulation_frame;
        simulation_frame = (at_read_explicit(&AMW.flags, memory_model_acq_rel) & amw_flag_dont_continue_work) ? NULL : &frames[NEXT_FRAME(frame_idx++)];

        /* TODO run 120 frames and exit */
        if (--close_counter < 0)
            at_fetch_or_explicit(&AMW.flags, amw_flag_dont_continue_work, memory_model_release);

    } while (((at_read_explicit(&AMW.flags, memory_model_seq_cst) & (amw_flag_run_gameloop | amw_flag_forced_exit)) == amw_flag_run_gameloop));
    log_debug("MAINLOOP - - - END %s", at_read_relaxed(&AMW.flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(&AMW);

    log_info("Application was running for a total of:  %llu min, %02llu sec, %03lu ms", 
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, ticks_ms()%AMW_MS_PER_SECOND);
}

int a_moonlit_walk(
        int (*main__)(struct application_config *, int, char **), 
        int argc, char **argv)
{
    int res = result_success;
    size_t riven_bytes;
    void *riven;

    struct application_config config;
    iazero(config);

    /* parse arguments, configure the engine */
    res = main__(&config, argc, argv);
    if (res != result_success)
        return res;

    { /* validate the configuration, provide defaults */
        if (config.name == NULL)            config.name = "Undefined";
        if (config.version == 0)            config.version = VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);

        if (config.hadal.api <= 0)          config.hadal.api = hadal_api_auto;
        if (config.hadal.width <= 0)        config.hadal.width = 1200;
        if (config.hadal.height <= 0)       config.hadal.height = 800;
        if (config.hadal.title == NULL)     config.hadal.title = "A Moonlit Walk Engine";

        if (config.riven.threads == 0)      cpu_count(&config.riven.threads, NULL, NULL);
        if (config.riven.fibers == 0)       config.riven.fibers = 128;
        if (config.riven.stack_size == 0)   config.riven.stack_size = 64 * 1024;
        if (config.riven.log2_tears == 0)   config.riven.log2_tears = 12;
    }

    riven_bytes = riven_unveil_rift(
        NULL,                       /* Null to get the required memory buffer size. */
        config.riven.stack_size,    /* Size of stack per fiber in bytes. */
        config.riven.fibers,        /* How many fibers to instance. */
        config.riven.log2_tears,    /* Job queue will be (1u << log2_tears) entries large. */
        config.riven.threads,       /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,      /* Main riven tear, serves as the entry point. */
        &config);                   /* Argument holding hints for other systems' configuration */
    riven = malloc(riven_bytes);

    riven_unveil_rift(
        riven,
        config.riven.stack_size,
        config.riven.fibers,
        config.riven.log2_tears,
        config.riven.threads,
        a_moonlit_walk_main__,
        &config);
    free(riven);

    return res;
}
