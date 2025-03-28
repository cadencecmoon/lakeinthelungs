#include <amw/a_moonlit_walk.h>

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER_MS 20

#define FRAME_TIME_PRINT_INTERVAL_MS 1000

static_assert(((AMW_MAX_FRAMEDATA != 0) && ((AMW_MAX_FRAMEDATA & (AMW_MAX_FRAMEDATA - 1)) == 0)), 
    "work count must be a power of 2");

s32 amw_run_framework(PFN_riven_work (AMWCALL *entry)(struct amw_config *, s32, char **), s32 argc, char **argv)
{
    struct amw_config config = {
        .metadata = {
            .engine_name = str_init("A Moonlit Walk Engine"),
            .engine_build_version = AMW_VERSION,
            .game_name = str_null,
            .game_build_version = 0,
        },
        .frames_in_flight = 3,
        .hadal_main_width = 1200,
        .hadal_main_height = 900,
        .reznor_force_primary_device_index = -1,
        .reznor_target_device_count = AMW_MAX_RENDERING_DEVICE_COUNT,
        .reznor_prefer_only_discrete_devices = false,
        .riven_memory_budget_size = 1lu*1024*1024*1024, /* 1 GB */
        .riven_fiber_stack_size = 64lu*1024,
        .riven_thread_count = 0,
        .riven_tagged_heap_count = 0,
        .riven_log2_work_count = 11,
        .riven_log2_memory_count = 9,
        .encores = {
            .hadal = (PFN_riven_work)hadal_encore_native,
            .reznor = (PFN_riven_work)reznor_encore_native,
            .soma = (PFN_riven_work)soma_encore_native,
        },
#ifdef DEBUG
        .reznor_virtual_device_count = 0,
        .debug_utilities = true,
#endif
    };

    config.encores.moonlit = entry(&config, argc, argv);
    if (config.encores.moonlit == NULL) 
        return result_continue;

    /* check defaults and limits */
    config.frames_in_flight = max(2, min(config.frames_in_flight, REZNOR_MAX_FRAMES_IN_FLIGHT)) ;
    if (!config.metadata.game_name.ptr)
        config.metadata.game_name = str_init("undefined");

    s32 res = riven_moonlit_walk(
        config.riven_memory_budget_size,
        config.riven_fiber_stack_size,
        config.riven_fiber_count,
        config.riven_thread_count,
        config.riven_tagged_heap_count,
        config.riven_log2_work_count,
        config.riven_log2_memory_count,
        (PFN_riven_heart)a_moonlit_walk,
        (void *)&config);
    return res;
}

static void AMWCALL engine_fini(struct a_moonlit_walk_engine *amw)
{
    struct riven *riven = amw->riven;
    struct reznor_interface *reznor = (struct reznor_interface *)amw->reznor;

    /* destroy the game application */
    if (amw->moonlit) {
        PFN_riven_work moonlit_fini = riven_interface_fini(amw->moonlit);
        moonlit_fini(amw->moonlit);
        amw->moonlit = NULL;
    }

    /* destroy windows and swapchains */
    u32 window_count = atomic_load_explicit(&amw->window_count, memory_order_relaxed);
    u32 swapchain_count = 0;
    struct reznor_swapchain *swapchains[AMW_MAX_WINDOW_COUNT];

    for (s32 i = AMW_MAX_WINDOW_COUNT - 1u; i >= 0 && window_count; i--) {
        struct hadal_window_header *window = (struct hadal_window_header *)amw->windows[i];
        if (!window) continue;

        struct hadal_interface *hadal = (struct hadal_interface *)window->hadal;
        struct reznor_swapchain *swapchain = window->swapchain;
        if (swapchain) {
            hadal->window_attach_swapchain(amw->windows[i], NULL);
            swapchains[swapchain_count++] = swapchain;
        }
        hadal->window_destroy((struct hadal_window *)window);
        amw->windows[i] = NULL;
    }

    for (u32 i = 0; i < swapchain_count; i++)
        reznor->disassembly_table[reznor_resource_type_swapchain](swapchains[i]);

    atomic_store_explicit(&amw->window_count, 0, memory_order_relaxed);
    amw->active_window_count = 0;

    /* destroy rendering devices */
    if (amw->render_device_count) {
        struct riven_work render_device_work[AMW_MAX_RENDERING_DEVICE_COUNT];
        struct reznor_interface *reznor = (struct reznor_interface *)amw->reznor;

        assert_debug(reznor != NULL);

        for (u32 i = 0; i < amw->render_device_count; i++) {
            render_device_work[i].procedure = (PFN_riven_work)reznor->device_destroy;
            render_device_work[i].argument = amw->render_devices[i];
            render_device_work[i].name = str_init("a_moonlit_walk:fini:device_destroy");
            amw->render_devices[i] = NULL;
        }
        riven_split_work_and_unchain(riven, render_device_work, amw->render_device_count);
    }
    amw->render_device_count = 0;

    /* destroy core interfaces */
    u32 o = 0;
    void *interfaces[] = {
        amw->soma, amw->reznor, amw->hadal,
    };
    PFN_riven_work finish[] = {
        riven_interface_fini(amw->soma),
        riven_interface_fini(amw->reznor),
        riven_interface_fini(amw->hadal),
    };
    struct riven_work finish_work[arraysize(interfaces)];
    for (u32 i = 0; i < arraysize(interfaces); i++) {
        if (interfaces[i] == NULL) continue;

        finish_work[o].procedure = finish[i];
        finish_work[o].argument = interfaces[i];
        finish_work[o].name = str_init("a_moonlit_walk:fini:interfaces");
        o++;
    }
    riven_split_work_and_unchain(riven, finish_work, o);
}

static s32 AMWCALL engine_init(
    struct a_moonlit_walk_engine *amw, 
    const struct amw_config      *config)
{
    s32 res = result_success;
    struct riven *riven = amw->riven;
    const riven_tag_t tag = riven_tag_roots;
    { /* create core interfaces */
        struct soma_encore soma = {
            /* TODO */
        };
        struct reznor_encore reznor = {
            .thread_count = amw->thread_count,
            .debug_utils = config->debug_utilities,
        };
        struct hadal_encore hadal = {
            .gamecontrollerdb = NULL,
            /* TODO */
        };

        struct {
            void *argument, **interface;
            PFN_riven_work encore;
        } interfaces[] = {
            { (void *)&soma,   (void **)&amw->soma,   config->encores.soma },
            { (void *)&reznor, (void **)&amw->reznor, config->encores.reznor },
            { (void *)&hadal,  (void **)&amw->hadal,  config->encores.hadal },
        };
        struct riven_work init_work[arraysize(interfaces)];

        for (u32 i = 0; i < arraysize(interfaces); i++) {
            struct riven_encore_header *header = interfaces[i].argument;
            header->interface = interfaces[i].interface;
            header->metadata = &config->metadata;
            header->riven = riven;
            header->tag = tag;
            init_work[i].procedure = interfaces[i].encore;
            init_work[i].argument = interfaces[i].argument;
            init_work[i].name = str_init("a_moonlit_walk:init:interfaces");
        }
        riven_split_work_and_unchain(riven, init_work, arraysize(interfaces));

        for (u32 i = 0; i < arraysize(interfaces); i++) {
            struct riven_encore_header *header = interfaces[i].argument;

            /* we'll exit on errors */
            if (*header->interface == NULL)
                res = result_error;
        }
        if (res != result_success)
            return res;
    }

    res = reznor_mgpu_create_devices(
        amw->reznor, amw->hadal, tag,
        config->frames_in_flight,
        config->reznor_target_device_count,
        config->reznor_virtual_device_count,
        config->reznor_prefer_only_discrete_devices,
        config->reznor_force_primary_device_index,
        &amw->render_device_count,
        amw->render_devices);

    if (res != result_success) {
        log_fatal("AMW Engine early initialization failure.");
        return res;
    }

    return result_success;
}

static void AMWCALL deferred_window_destroy(struct amw_deferred_work *work)
{
    struct a_moonlit_walk_engine *amw = work->frame->amw;
    assert_debug(amw->windows[work->context] != NULL);

    struct hadal_window_header *window = (struct hadal_window_header *)amw->windows[work->context];
    struct hadal_interface *hadal = (struct hadal_interface *)window->hadal;
    struct reznor_swapchain *swapchain = window->swapchain;
    if (swapchain) {
        struct reznor_interface *reznor = (struct reznor_interface *)amw->reznor;

        hadal->window_attach_swapchain((struct hadal_window *)window, NULL);
        reznor->disassembly_table[reznor_resource_type_swapchain](swapchain);
    }
    hadal->window_destroy((struct hadal_window *)window);
    atomic_fetch_sub_explicit(&amw->window_count, 1, memory_order_release);
};

static void AMWCALL framedata_prepare_swapchains(struct amw_framedata *frame) 
{
    struct a_moonlit_walk_engine *amw = frame->amw;

    u32 index = 0, sc = 0;
    u32 window_count = amw->active_window_count;
    while (index < window_count) {
        struct hadal_window_header *window = (struct hadal_window_header *)amw->windows[index];

        u32 flags = atomic_load_explicit(&window->flags, memory_order_acquire);
        if (flags & hadal_window_flag_should_close) {
            if (index == 0) {
                /* We defer the destruction of the main window until the engine exit.
                 * In this case we still wan't to save the swapchain. */
                amw->finalize_gameloop = true;
            } else {
                window_count--;
                /* bring forward last window handle */
                if (index != window_count) {
                    amw->windows[index] = amw->windows[window_count];
                    amw->windows[window_count] = NULL;
                }
                /* find an index for the inactive handle */
                u32 inactive = 0;
                for (u32 j = AMW_MAX_WINDOW_COUNT - 1; inactive == 0 && j >= window_count; j++) {
                    if (amw->windows[j] == NULL) {
                        amw->windows[j] = (struct hadal_window *)window; 
                        inactive = j;
                    }
                }
                assert_debug(inactive);

                /* the last frame will destroy the window */
                u32 defer = frame->deferred_count; 
                frame->deferred_count++;

                assert_debug(frame->deferred_count <= AMW_MAX_DEFERRED_COUNT);

                frame->deferred_work[defer].procedure = (PFN_riven_work)deferred_window_destroy;
                frame->deferred_work[defer].context = inactive;
                continue; /* don't increment the index, don't grab the swapchain */
            }
        }
        index++;

        /* this window is active, so let's try to grab the swapchain handle */
        if (flags & hadal_window_flag_visible || window->swapchain)
            frame->swapchains[sc++] = window->swapchain;
    }
    frame->swapchain_count = sc;
}

s32 a_moonlit_walk(
    struct riven            *riven,
    thread_t                *threads,
    u32                      thread_count,
    const struct amw_config *config)
{
    u64 frame_index = 0;
    u64 time_last = 0, time_now = 0;

    const f64 delta_time_frequency = 1000.0/(f64)rtc_frequency(); /* ms */
    f64 delta_time = 0.0;

    struct a_moonlit_walk_engine amw = {
        .riven = riven,
        .threads = threads,
        .thread_count = thread_count,
    };

    /* don't continue on early initialization errors */
    s32 res = engine_init(&amw, config);
    if (res != result_success) {
        engine_fini(&amw);
        return res;
    }

    struct amw_framedata frames[AMW_MAX_FRAMEDATA];
    zeroa(frames);

    for (u32 i = 0; i < AMW_MAX_FRAMEDATA; i++) {
        frames[i].amw = &amw;
        frames[i].last_work = &frames[(i - 1 + AMW_MAX_FRAMEDATA) % AMW_MAX_FRAMEDATA];
        frames[i].next_work = &frames[(i + 1 + AMW_MAX_FRAMEDATA) % AMW_MAX_FRAMEDATA];
        for (u32 j = 0; j < AMW_MAX_DEFERRED_COUNT; j++)
            frames[i].deferred_work[j].frame = &frames[i];
    }

    struct riven_work work[3];
    work[AMW_GAME_WORK_IDX].procedure = (PFN_riven_work)a_moonlit_walk_game;
    work[AMW_GAME_WORK_IDX].name = str_init("a_moonlit_walk:game");
    work[AMW_RENDERING_WORK_IDX].procedure = (PFN_riven_work)a_moonlit_walk_rendering;
    work[AMW_RENDERING_WORK_IDX].name = str_init("a_moonlit_walk:rendering");
    work[AMW_GPUEXEC_WORK_IDX].procedure = (PFN_riven_work)a_moonlit_walk_gpuexec;
    work[AMW_GPUEXEC_WORK_IDX].name = str_init("a_moonlit_walk:gpuexec");

    struct riven_work deferred_work[AMW_MAX_DEFERRED_COUNT];
    for (u32 i = 0; i < AMW_MAX_DEFERRED_COUNT; i++)
        deferred_work[i].name = str_init("a_moonlit_walk:deferred");

    /* XXX debug, delete later */
    f64 close_counter = DEBUG_CLOSE_COUNTER_MS;

    do { /* this additional loop will be responsible for restarting core engine state and backends at runtime */
        time_now = rtc_counter();
        amw.restart_engine = amw.finalize_gameloop = false;

        /* XXX updates can be made here, e.g. changing backends, reinitializing devices, main window, etc. */

        struct amw_framedata *game = frames;
        struct amw_framedata *rendering = NULL;
        struct amw_framedata *gpuexec = NULL;
        struct amw_framedata *deferred = NULL;
        struct amw_framedata *cleanup = NULL;

        while (game || rendering || gpuexec || deferred) {
            time_last = time_now;
            time_now = rtc_counter();
            delta_time = ((f64)(time_now - time_last) * delta_time_frequency);
            record_frame_time(time_now, delta_time_frequency);

            /* frame N-4, cleanup */
            if (cleanup) {
                /* wait for deferred work to finish */
                if (cleanup->deferred_count)
                    riven_unchain(riven, cleanup->chain);
                cleanup->chain = NULL; 
                zeroa(cleanup->deferred_work);
                zeroa(cleanup->render_frames);
                zeroa(cleanup->swapchains);
                cleanup->swapchain_count = 0;
            }

            /* frame N-3, deferred */
            if (deferred) {
                /* wait for last gpuexec to finish */
                riven_unchain(riven, deferred->chain);
                deferred->chain = NULL;
#ifndef NDEBUG
                /* check fatal results of last stage (gpuexec) */
                if (deferred->result == result_error) {
                    log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                              frame_index, work[AMW_GPUEXEC_WORK_IDX].name.ptr);
                    res = deferred->result;
                    amw.exit_game = true;
                }
#endif /* NDEBUG */
                /* process deferred work */
                if (deferred->deferred_count) {
                    for (u32 i = 0; i < deferred->deferred_count; i++) {
                        struct amw_deferred_work *context = &deferred->deferred_work[i];
                        deferred_work[i].procedure = context->procedure;
                        deferred_work[i].argument = context;
                    }
                    riven_split_work(riven, deferred_work, deferred->deferred_count, &deferred->chain);
                }
            }

            /* frame N-2, gpuexec */
            if (gpuexec) {
                /* gpuexec points to the frame that may currently be in the rendering stage */
                riven_unchain(riven, gpuexec->chain);
                gpuexec->chain = NULL;
#ifndef NDEBUG
                /* check fatal results of last stage (rendering) */
                if (gpuexec->result == result_error) {
                    log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                              frame_index, work[AMW_RENDERING_WORK_IDX].name.ptr);
                    res = gpuexec->result;
                    amw.exit_game = true;
                }
#endif /* NDEBUG */
                work[AMW_GPUEXEC_WORK_IDX].argument = gpuexec;
                riven_split_work(riven, &work[AMW_GPUEXEC_WORK_IDX], !amw.exit_game, &gpuexec->chain);
            }

            /* frame N-1, rendering */
            if (rendering) {
                /* rendering points to the frame that may currently be in the game stage */
                riven_unchain(riven, rendering->chain);
                rendering->chain = NULL;
#ifndef NDEBUG
                /* check fatal results of last stage (game) */
                if (rendering->result == result_error) {
                    log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                              frame_index, work[AMW_GAME_WORK_IDX].name.ptr);
                    res = rendering->result;
                    amw.exit_game = true;
                }
#endif /* NDEBUG */
                work[AMW_RENDERING_WORK_IDX].argument = rendering;
                riven_split_work(riven, &work[AMW_RENDERING_WORK_IDX], !amw.exit_game, &rendering->chain);
            }

            /* frame N, game */
            if (game) {
                /* game stage has no dependency but on itself, which is resolved at frame N-1 */
                game->chain = NULL;
                game->frame_index = frame_index;
                game->delta_time = delta_time;
                game->game_to_rendering_tag = riven_tag_game_to_rendering + (frame_index % riven_tag_game_to_rendering_cycle);
                game->game_to_gpuexec_tag = riven_tag_game_to_gpuexec + (frame_index % riven_tag_game_to_gpuexec_cycle);
                game->rendering_to_gpuexec_tag = riven_tag_rendering_to_gpuexec + (frame_index % riven_tag_rendering_to_gpuexec_cycle);

                for (u32 i = 0; i < amw.render_device_count; i++)
                    game->render_frames[i] = reznor_frame_acquire(frame_index, amw.render_devices[i]);
                /* resolve the visible windows for this frame */
                framedata_prepare_swapchains(game);

                work[AMW_GAME_WORK_IDX].argument = game;
                riven_split_work(riven, &work[AMW_GAME_WORK_IDX], !amw.exit_game, &game->chain);
            }

            /* XXX debug, delete later */
            close_counter -= delta_time;
            if ((amw.exit_game || close_counter <= 0))
                amw.finalize_gameloop = true;

            /* cycle the framedata */
            cleanup = deferred;
            deferred = gpuexec;
            gpuexec = rendering;
            rendering = game;
            game = amw.finalize_gameloop ? NULL : &frames[(frame_index++) & (AMW_MAX_FRAMEDATA-1)];

            print_frame_time(FRAME_TIME_PRINT_INTERVAL_MS);
            riven_rotate_deferred(riven);
        }

        /* wait for all work to finish */
        for (u32 i = 0; i < AMW_MAX_FRAMEDATA; i++)
            riven_unchain(riven, frames[i].chain);

    } while (amw.restart_engine && !amw.exit_game);

    delta_time = median_frame_time();
    log_info("Last recorded frame time: %.3f ms (%.0f FPS), at frame %lu", delta_time, 1000/delta_time, frame_index);

    engine_fini(&amw);
    return res;
}
