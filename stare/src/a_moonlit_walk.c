/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/a_moonlit_walk.h>

#include "framework.h"

#include <stdlib.h> /* malloc, free */

static void a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW, s32 frame_idx, f64 dt)
{
    struct amw_workload cleanup_work = {
        .context = amw_workload_context_exit,
        .idx = frame_idx,
        .dt = dt,
        .AMW = AMW,
        .last_work = NULL,
    };
    a_moonlit_walk_gpuexec_tear__(&cleanup_work);
    a_moonlit_walk_rendering_tear__(&cleanup_work);
    a_moonlit_walk_simulation_tear__(&cleanup_work);

    struct pelagia_renderer_fini_work renderer_fini = { .pelagia = &AMW->pelagia };
    pelagia_renderer_fini(&renderer_fini);

    hadal_fini(&AMW->hadal);
}

static void a_moonlit_walk_main__(
    struct riven   *riven,
    thread_t       *threads,
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
    
    { /* initialization */
        i = hadal_init(
            AMW->hints->entry_points.hadal,
            &AMW->hadal,
            AMW->ia,
            AMW->hints->window_width, 
            AMW->hints->window_height,
            AMW->hints->window_title
        );
        if (i != result_success) {
            log_fatal("Can't initialize the display backend.");
            a_moonlit_walk_cleanup__(AMW, frame_idx, dt);
            return; 
        }
        hadal_window_visible(&AMW->hadal, true);

        struct pelagia_renderer_init_work renderer_init_work = {
            .pelagia = &AMW->pelagia,
            .ia = AMW->ia,
            .hadal = &AMW->hadal,
            .application_name = AMW->hints->app_name,
            .application_version = AMW->hints->version,
            .riven = riven,
            .threads = threads,
            .thread_count = thread_count,
            .target_workload_buffering = AMW->hints->pelagia_target_workload_buffering,
            .preferred_device_idx = AMW->hints->pelagia_preferred_device_idx,
            .enable_vsync = AMW->hints->pelagia_enable_vsync,
            .out_result = result_success,
        };

        pelagia_renderer_init(&renderer_init_work);
        if (renderer_init_work.out_result != result_success) {
            log_fatal("Can't initialize the renderer.");
            a_moonlit_walk_cleanup__(AMW, frame_idx, dt);
            return;
        }
    }

    struct amw_workload work[AMW_MAX_WORKLOAD];
    for (i = 0; i < AMW_MAX_WORKLOAD; i++) {
        work[i].context = amw_workload_context_continue;
        work[i].idx = i;
        work[i].dt = 0.0;
        work[i].AMW = AMW;
        work[i].last_work = &work[(i - 1 + AMW_MAX_WORKLOAD) % AMW_MAX_WORKLOAD];
    }
    work[frame_idx].context = amw_workload_context_startup;

    struct rivens_tear tears[3];
    tears[AMW_SIMULATION_TEAR_IDX].procedure = (PFN_riven_work)a_moonlit_walk_simulation_tear__;
    tears[AMW_SIMULATION_TEAR_IDX].name = "a_moonlit_walk_simulation_tear__";
    tears[AMW_RENDERING_TEAR_IDX].procedure = (PFN_riven_work)a_moonlit_walk_rendering_tear__;
    tears[AMW_RENDERING_TEAR_IDX].name = "a_moonlit_walk_rendering_tear__";
    tears[AMW_GPUEXEC_TEAR_IDX].procedure = (PFN_riven_work)a_moonlit_walk_gpuexec_tear__;
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

        tears[AMW_SIMULATION_TEAR_IDX].argument = simulation_workload;
        tears[AMW_RENDERING_TEAR_IDX].argument = rendering_workload;
        tears[AMW_GPUEXEC_TEAR_IDX].argument = gpuexec_workload;

        /* prepare the next frame */
        if (simulation_workload) {
            simulation_workload->idx = frame_idx;
            simulation_workload->dt = dt;
        }

        /* execute the mainloop stages */
        riven_split_and_unchain(riven, tears, 3);

        /* increment the frame counter */
        gpuexec_workload = rendering_workload;
        rendering_workload = simulation_workload;
        simulation_workload = (at_read_relaxed(&AMW->flags) & amw_flag_finalize_gameloop) != 0 ? NULL : &work[(++frame_idx) % AMW_MAX_WORKLOAD];

        /* cleanup the last workload */
        if (gpuexec_workload) {
            gpuexec_workload->context = amw_workload_context_continue;
            /* TODO ia reset tags owned by this workload */
        }

        /* TODO run X frames and exit */
        close_counter--;
        if (close_counter < 0) // && simulation_frame)
            at_fetch_or_relaxed(&AMW->flags, amw_flag_finalize_gameloop);

    } while (!(at_read_relaxed(&AMW->flags) & (amw_flag_forced_exit)) && (simulation_workload || rendering_workload || gpuexec_workload));
    log_debug("A MOONLIT WALK - GAMELOOP - - - END %s", at_read_relaxed(&AMW->flags) & amw_flag_forced_exit ? "!! FORCED EXIT" : "");

    a_moonlit_walk_cleanup__(AMW, frame_idx, dt);

    log_info("%s was running for a total of: %llu:%02llu.%03lu min, %u frames", AMW->hints->app_name,
        (ticks_ns()/AMW_NS_PER_SECOND)/60, (ticks_ns()/AMW_NS_PER_SECOND)%60, (ticks_ms()%AMW_MS_PER_SECOND), frame_idx-1);
}

AMWAPI s32 a_moonlit_walk(
    s32 (*main__)(struct amw_hints *, s32, char **), 
    s32 argc, char **argv)
{
    s32 res = 0;
    usize riven_bytes;
    struct ipomoea *ia;

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
        .riven_log_2_tears = 12, /* (1u << log_2_tears) work queue size */
        .ipomoea_root_budget_estimate = 0,
        .ipomoea_max_memory_budget = 0,
        .pelagia_preferred_device_idx = -1, /* select the most appropriate GPU as the rendering device */
        .pelagia_target_workload_buffering = pelagia_workload_buffering_triple, /* max is 4, depends on the swapchain */
        .pelagia_enable_vsync = false,
        .entry_points.hadal = hadal_entry_point,
        .entry_points.silv = silver_entry_point,
        .callbacks.init = NULL,
        .callbacks.simulation = NULL,
        .callbacks.rendering = NULL,
        .callbacks.gpuexec = NULL,
        .callbacks.fini = NULL,
    };
    bedrock_cpuinfo(&hints.riven_thread_count, NULL, NULL);

    usize hugepage = 0;
    bedrock_meminfo(NULL, NULL, &hugepage);

    /* TODO system memory */
    /* TODO init ipomoea */

    /* parse arguments, configure the engine */
    res = main__(&hints, argc, argv);
    if (res != result_success) /* don't continue ? */
        return res;

    /* estimate the core memory requirements */
    riven_bytes = riven_unveil_rift(
        NULL,                       /* Null to get the required memory buffer size. */
        hints.riven_stack_size,     /* Size of stack per fiber in bytes. */
        hints.riven_fiber_count,    /* How many fibers to instance. */
        hints.riven_log_2_tears,    /* Job queue will be (1u << log2_tears) entries large. */
        hints.riven_thread_count,   /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,      /* Main riven tear, serves as the entry point. */
        NULL);                      /* The engine context as the argument. */
    hints.ipomoea_root_budget_estimate += riven_bytes;

    ia = iainit(hints.ipomoea_max_memory_budget, hints.ipomoea_root_budget_estimate);
    if (ia == NULL) {
        log_fatal("Can't create the tagged heap allocator.");
        return result_error_undefined; /* TODO */
    }
    log_debug("The tagged heap root memory pool: %lu bytes (%luMiB, %u large pages).", 
        ia->page_size * ia->root_heap.page_count, (ia->page_size * ia->root_heap.page_count)/(1024*1024), ia->root_heap.page_count);
    log_debug("Constructing the Riven structure for a parallel workflow:");
    log_debug("      memory buffer - %lu bytes (%luKiB)", riven_bytes, riven_bytes/1024);
    log_debug("    stack per fiber - %u bytes (%uKiB)", hints.riven_stack_size, hints.riven_stack_size/1024);
    log_debug("    tear queue size - %u tears, %u log2", (1 << hints.riven_log_2_tears), hints.riven_log_2_tears);
    log_debug("        fiber count - %u fibers", hints.riven_fiber_count);
    log_debug("       thread count - %d threads", hints.riven_thread_count);

    struct a_moonlit_walk *AMW = (struct a_moonlit_walk *)ialloc(ia, sizeof(struct a_moonlit_walk), 16, ipomoea_tag_root);
    void *riven_memory = ialloc(ia, riven_bytes, 16, ipomoea_tag_root);

    AMW->ia = ia;
    AMW->hints = &hints;
    
    riven_unveil_rift(
        riven_memory,
        hints.riven_stack_size,
        hints.riven_fiber_count,
        hints.riven_log_2_tears,
        hints.riven_thread_count,
        a_moonlit_walk_main__, AMW);
    iafini(ia);

    return AMW->out_result;
}
