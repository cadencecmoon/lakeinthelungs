#include <amwe/a_moonlit_walk.h>

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 128

#define ENCORE_COUNT 3
#define PIPELINE_WORK_COUNT 16
bedrock_static_assert(((PIPELINE_WORK_COUNT != 0) && ((PIPELINE_WORK_COUNT & (PIPELINE_WORK_COUNT - 1)) == 0)), 
        "pipeline work count must be a power of 2");
#define PIPELINE_WORK_MASK (PIPELINE_WORK_COUNT-1)

#define SIMULATION_WORK_INDEX 0
#define RENDERING_WORK_INDEX  1
#define GPUEXEC_WORK_INDEX    2

extern FN_RIVEN_INTERFACE_VALIDATION(hadal);
extern FN_RIVEN_INTERFACE_VALIDATION(soma);
extern FN_RIVEN_INTERFACE_VALIDATION(xaku);

static struct framework_hints {
    enum amwe_hint_pipeline_setting pipeline_setting;

    bool                            null_fallback_native_encores;

    PFN_riven_encore                encore_hadal;
    PFN_riven_encore                encore_xaku;
    PFN_riven_encore                encore_soma;
    PFN_riven_encore                encore_pelagial;

    struct hadal_display_assembly   display_assembly;
    struct xaku_renderer_assembly   renderer_assembly;
    struct soma_audio_assembly      audio_assembly;
} g_hints = {
    .pipeline_setting = amwe_hint_pipeline_setting_auto,
    .null_fallback_native_encores = false,
    .encore_hadal = NULL,
    .encore_xaku = NULL,
    .encore_soma = NULL,
    .encore_pelagial = NULL,
    .display_assembly = {
        .strategy = hadal_strategy_auto,
    },
    .renderer_assembly = {
        .strategy = xaku_strategy_auto,
    },
    .audio_assembly = {
        .strategy = soma_strategy_auto,
    },
};

void amwe_hint_framework(u32 hint, u32 value)
{
    switch (hint) {
    case AMWE_HINT_PIPELINE_SETTING:
        bedrock_assert_debug(value < amwe_hint_pipeline_setting_max_enum);
        g_hints.pipeline_setting = value; break;
    case AMWE_HINT_NULL_FALLBACK_NATIVE_ENCORES:
        g_hints.null_fallback_native_encores = value ? true : false; break;
    default: break;
    }
}

void amwe_hint_encore(u32 hint, PFN_riven_encore encore)
{
    switch (hint) {
    case AMWE_HINT_ENCORE_HADAL:
        g_hints.encore_hadal = encore; break;
    case AMWE_HINT_ENCORE_XAKU:
        g_hints.encore_xaku = encore; break;
    case AMWE_HINT_ENCORE_SOMA:
        g_hints.encore_soma = encore; break;
    case AMWE_HINT_ENCORE_PELAGIAL:
        g_hints.encore_pelagial = encore; break;
    default: break;
    }
}

static lake_nonnull_all 
bool engine_init_encores(
    struct riven                   *riven,
    const struct riven_hints       *riven_hints,
    const struct pelagial_metadata *metadata)
{
    const riven_tag_t tag = riven_tag_roots; /* later i may give different backends different tags */
    union hadal_encore_view hadal = {0};
    union xaku_encore_view xaku = {0};
    union soma_encore_view soma = {0};
    struct hadal_encore_assembly hadal_assembly = {0};
    struct xaku_encore_assembly xaku_assembly = {0};
    struct soma_encore_assembly soma_assembly = {0};

    struct riven_encore_work encores[ENCORE_COUNT] = {
#define ENCORE_ARRAY_SETUP(T)                                                                   \
        (struct riven_encore_work){                                                             \
            .encores = &g_hints.encore_##T,                                                     \
            .encore_userdata = &T##_assembly,                                                   \
            .interface_validation = (PFN_riven_interface_validation)T##_interface_validation,   \
            .out_interface = &T.data,                                                           \
        }
        ENCORE_ARRAY_SETUP(hadal),
        ENCORE_ARRAY_SETUP(xaku),
        ENCORE_ARRAY_SETUP(soma),
#undef ENCORE_ARRAY_SETUP
    };
    struct riven_work begins[ENCORE_COUNT];

    for (u32 i = 0; i < ENCORE_COUNT; i++) {
        encores[i].encore_count = 1,
        encores[i].riven = riven;
        encores[i].tag = tag;
        encores[i].metadata = metadata;
        begins[i].procedure = (PFN_riven_work)riven_encore;
        begins[i].userdata = &encores[i];
        begins[i].name = "amwe:init:riven_encore";
    }

    /* fallback native encores */
#define ENCORE_EARLY_FALLBACK(IDX, T) \
    if (*encores[IDX].encores == NULL) \
        encores[IDX].encores = T##_native_encores(g_hints.null_fallback_native_encores, &encores[IDX].encore_count);
    ENCORE_EARLY_FALLBACK(0, hadal);
    ENCORE_EARLY_FALLBACK(1, xaku);
    ENCORE_EARLY_FALLBACK(2, soma);
#undef ENCORE_EARLY_FALLBACK

    /* run encores */
    riven_split_work_and_unchain(riven, begins, ENCORE_COUNT);

    bool valid = true;
    if (!hadal.data || !xaku.data || !soma.data) {
        const char *err = "Failed to encore '%s' at engine init.";
        const char *flbk = "Trying a NULL '%s' fallback...";
        const char *done = "'%s: %s' interface write.";
#define ENCORE_FALLBACK(T)                                                                  \
        if (!T.data) {                                                                      \
            if (g_hints.null_fallback_native_encores) {                                     \
                bedrock_log_warn(flbk, #T);                                                 \
                T.data = T##_encore_null(riven, riven_hints, metadata, &T##_assembly, tag); \
            }                                                                               \
            if (!T.data) {                                                                  \
                bedrock_log_error(err, #T);                                                 \
                valid = false;                                                              \
            } else {                                                                        \
                bedrock_log_verbose(done, T.header->name, T.header->backend);               \
            }                                                                               \
        }
        ENCORE_FALLBACK(hadal);
        ENCORE_FALLBACK(xaku);
        ENCORE_FALLBACK(soma);
#undef ENCORE_FALLBACK
    }
    /* exit early if we couldn't resolve the encores */
    if (!valid) {
        riven_destroy_interface(soma.data);
        riven_destroy_interface(xaku.data);
        riven_destroy_interface(hadal.data);
    }
    g_hints.display_assembly.hadal = hadal;
    g_hints.renderer_assembly.xaku = xaku;
    g_hints.audio_assembly.soma = soma;
    return valid;
}

static void engine_fini(struct a_moonlit_walk_engine *amwe)
{
    /* destroy the game interface first, before anything else */
    if (amwe->application.encore)
        amwe->application.header->zero_ref_callback(amwe->application.encore);
    amwe->application.encore = NULL;

    /* destroy in order: audio engine, renderer, display */
    struct riven_work ends[ENCORE_COUNT];
    u32 end_index = 0;
    /* 0 means uninitialized, 1 means we can destroy it,
     * anything else means we misused the encores at some point */
    u32 prev_refcnt = 0;

#define ENCORE_ENDS(T, MEMBER) \
    prev_refcnt = T##_##MEMBER##_fini(&amwe->MEMBER, &ends[end_index]); \
    bedrock_assert_debug(prev_refcnt < 2); \
    end_index += prev_refcnt;

    ENCORE_ENDS(soma, audio);
    ENCORE_ENDS(xaku, renderer);
    ENCORE_ENDS(hadal, display);
#undef ENCORE_ENDS

    /* destroy the encores */
    if (end_index) 
        riven_split_work_and_unchain(amwe->riven, ends, end_index);
    bedrock_zerop(amwe);
}

lake_force_inline void prepare_pipeline_work(struct amwe_pipeline_work *work, u64 index, f64 dt)
{
    work->frame_index = index;
    work->delta_time = dt;
    work->profiling.simulation_begin_time = 0;
    work->profiling.simulation_end_time = 0;
    work->profiling.rendering_begin_time = 0;
    work->profiling.rendering_end_time = 0;
    work->profiling.gpuexec_begin_time = 0;
    work->profiling.gpuexec_end_time = 0;
    work->stage_result = amwe_pipeline_stage_result_continue;
}

s32 a_moonlit_walk(
    struct riven                   *riven,
    const struct riven_hints       *riven_hints,
    bedrock_thread_t               *threads,
    struct pelagial_metadata       *metadata)
{
    struct a_moonlit_walk_engine amwe = {
        .riven = riven,
        .riven_hints = riven_hints,
        .threads = threads,
        .metadata = metadata,
    };
    enum amwe_pipeline_stage_result stage_result = amwe_pipeline_stage_result_fatal_error;
    s32 exitcode = 0;
    u64 frame_index = 0;
    u64 time_last = 0, time_now = 0;
 
    /* precompute reciprocal, in seconds */
    const f64 delta_time_frequency = 1.0/(f64)bedrock_rtc_frequency();
    f64 delta_time = 0.0;

    const char *stage_names[] = { "simulation", "rendering", "gpuexec" };

    struct amwe_pipeline_work frames[PIPELINE_WORK_COUNT];
    bedrock_zeroa(frames);
    for (s32 i = 0; i < PIPELINE_WORK_COUNT; i++) {
        frames[i].amwe = &amwe;
        frames[i].last_work = &frames[(i - 1 + PIPELINE_WORK_COUNT) & (PIPELINE_WORK_MASK)];
        frames[i].next_work = &frames[(i + 1 + PIPELINE_WORK_COUNT) & (PIPELINE_WORK_MASK)];
    }

    {/* early engine initialization */
        if (!engine_init_encores(riven, riven_hints, metadata))
            goto fini;

        if (hadal_display_init(&g_hints.display_assembly, &amwe.display) != hadal_result_success) 
            goto fini;
        if (xaku_renderer_init(&g_hints.renderer_assembly, &amwe.renderer) != xaku_result_success)
            goto fini;
        if (soma_audio_init(&g_hints.audio_assembly, &amwe.audio) != soma_result_success)
            goto fini;

        /* run pelagial game encore */
        bedrock_assert_debug(g_hints.encore_pelagial);
        amwe.application.data = g_hints.encore_pelagial(riven, riven_hints, metadata, &amwe, riven_tag_roots);
    }

    /* this additional loop controls engine state updates */
    do {time_now = bedrock_rtc_counter();
        stage_result = amwe_pipeline_stage_result_continue;

        /* TODO commit updates to the engine state */

        enum amwe_hint_pipeline_setting pipeline = g_hints.pipeline_setting;
        if (pipeline == amwe_hint_pipeline_setting_auto) {
            pipeline = (riven_hints->thread_count > 3) 
                ? amwe_hint_pipeline_setting_parallel
                : amwe_hint_pipeline_setting_sequential;
        }

        /* XXX debug, delete later */
        s32 close_counter = DEBUG_CLOSE_COUNTER;

        /* ENTER GAMELOOP */
        if (pipeline == amwe_hint_pipeline_setting_parallel) {
            /** Runs the gameloop as a parallel pipeline, where three frames may be processed concurrently at a time.
             *  Frame N runs the game simulation stage, frame N-1 runs rendering and frame N-2 runs GPU execution.
             *  This gameloop greatly increases bandwith at cost of increased input latency and synchronization 
             *  overhead, not recommended on systems with less than 4 CPU threads or when the GPU is the bottleneck. */
            struct amwe_pipeline_work *simulation = frames;
            struct amwe_pipeline_work *rendering = NULL;
            struct amwe_pipeline_work *gpuexec = NULL;
            struct amwe_pipeline_work *cleanup = NULL;

            struct riven_work pipeline_work[3];
            pipeline_work[SIMULATION_WORK_INDEX].procedure = (PFN_riven_work)a_moonlit_walk_simulation;
            pipeline_work[SIMULATION_WORK_INDEX].name = stage_names[SIMULATION_WORK_INDEX];
            pipeline_work[RENDERING_WORK_INDEX].procedure = (PFN_riven_work)a_moonlit_walk_rendering;
            pipeline_work[RENDERING_WORK_INDEX].name = stage_names[RENDERING_WORK_INDEX];
            pipeline_work[GPUEXEC_WORK_INDEX].procedure = (PFN_riven_work)a_moonlit_walk_gpuexec;
            pipeline_work[GPUEXEC_WORK_INDEX].name = stage_names[GPUEXEC_WORK_INDEX];
            bool continue_gameloop = true;

            bedrock_log_verbose("GAMELOOP ENTER: parallel");
            while (simulation || rendering || gpuexec || cleanup) {
                time_last = time_now;
                time_now = bedrock_rtc_counter();
                delta_time = ((f64)(time_now - time_last) * delta_time_frequency);

                bedrock_frame_time_record(metadata->app_timer_start, time_now, delta_time_frequency);
                bedrock_frame_time_print(1000.f);

                if (cleanup) { /* frame N-3 */
                    /* chain points to the frame that may currently be in the gpuexec stage */
                    riven_unchain(riven, cleanup->chain);
                    cleanup->chain = NULL;

                    if (cleanup->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = rendering->stage_result;
                    /* TODO cleanup in flight state */
                }

                if (gpuexec) { /* frame N-2 */
                    /* chain points to the frame that may currently be in the rendering stage */
                    riven_unchain(riven, gpuexec->chain);
                    gpuexec->chain = NULL;

                    if (gpuexec->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = rendering->stage_result;

                    pipeline_work[GPUEXEC_WORK_INDEX].userdata = gpuexec;
                    riven_split_work(riven, &pipeline_work[GPUEXEC_WORK_INDEX], 1, &gpuexec->chain);
                }

                if (rendering) { /* frame N-1 */
                    /* chain points to the frame that may currently be in the simulation stage */
                    riven_unchain(riven, rendering->chain);
                    rendering->chain = NULL;

                    if (rendering->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = rendering->stage_result;

                    pipeline_work[RENDERING_WORK_INDEX].userdata = rendering;
                    riven_split_work(riven, &pipeline_work[RENDERING_WORK_INDEX], 1, &rendering->chain);
                }

                if (simulation) { /* frame N */
                    prepare_pipeline_work(simulation, frame_index, delta_time);
                    pipeline_work[SIMULATION_WORK_INDEX].userdata = simulation;
                    riven_split_work(riven, &pipeline_work[SIMULATION_WORK_INDEX], 1, &simulation->chain);
                }
                continue_gameloop &= stage_result == amwe_pipeline_stage_result_continue;

                /* XXX debug, delete later */
                close_counter--;
                if (close_counter <= 0)
                    continue_gameloop = false;

                cleanup = gpuexec;
                gpuexec = rendering;
                rendering = simulation;
                simulation = continue_gameloop ? &frames[(++frame_index) & PIPELINE_WORK_MASK] : NULL;
            }
            /* wait for all work to finish before leaving the gameloop */
            for (u32 i = 0; i < 3; i++)
                riven_unchain(riven, frames[i].chain);

        } else {
            /** Runs the pipeline one frame at a time. This setting will lesser synchronization overhead and 
             *  decrease input latency, at potential cost of performance or CPU bottleneck on multicore CPUs. */
            struct amwe_pipeline_work *frame = frames;

            PFN_riven_work stages[] = {
                (PFN_riven_work)a_moonlit_walk_simulation,
                (PFN_riven_work)a_moonlit_walk_rendering,
                (PFN_riven_work)a_moonlit_walk_gpuexec,
            };

            bedrock_log_verbose("GAMELOOP ENTER: sequential");
            while (frame) {
                time_last = time_now;
                time_now = bedrock_rtc_counter();
                delta_time = ((f64)(time_now - time_last) * delta_time_frequency);

                bedrock_frame_time_record(metadata->app_timer_start, time_now, delta_time_frequency);
                bedrock_frame_time_print(1000.f);

                prepare_pipeline_work(frame, frame_index, delta_time);
                for (u32 i = 0; i < 3; i++) {
                    stages[i](frame);            

                    if (frame->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = frame->stage_result;
                }
                /* XXX debug, delete later */
                close_counter--;
                if (close_counter <= 0) {
                    frame = NULL;
                    continue;
                }

                frame = stage_result == amwe_pipeline_stage_result_continue 
                    ? &frames[(++frame_index) & PIPELINE_WORK_MASK] : NULL;
            }
        } /* LEAVE GAMELOOP */
        bedrock_log_verbose("GAMELOOP LEAVE: at frame %lu", frame_index);

#ifdef RIVEN_ENABLE_PROFILER
        if (stage_result == amwe_pipeline_stage_result_fatal_error) {
            /* run a modal window with error message? flush log file? */
            struct amwe_pipeline_work *last_frame = &frames[frame_index];
            bedrock_log_fatal("Gameloop fail at frame '%lu':\n"
                "    SIMULATION   %.3f ms\n    RENDERING    %.3f ms\n    GPUEXEC      %.3f ms", frame_index,
                ((f64)(last_frame->profiling.simulation_begin_time - last_frame->profiling.simulation_end_time) * delta_time_frequency),
                ((f64)(last_frame->profiling.rendering_begin_time - last_frame->profiling.rendering_end_time) * delta_time_frequency),
                ((f64)(last_frame->profiling.gpuexec_begin_time - last_frame->profiling.gpuexec_end_time) * delta_time_frequency));
        }
        delta_time = bedrock_frame_time_median();
        bedrock_log_verbose("Last recorded frame time: %.3f ms (%.0f FPS), at frame %lu", 1000.f*delta_time, 1.f/delta_time, frame_index);
#endif /* RIVEN_ENABLE_PROFILER */
    } while (stage_result == amwe_pipeline_stage_result_reload_settings);

fini:
    engine_fini(&amwe);
    if (stage_result == amwe_pipeline_stage_result_restart_application)
        exitcode = INT32_MAX;
    return exitcode;
}

s32 a_moonlit_walk_entry_point__(
    PFN_riven_framework (LAKECALL *app_main)(
        struct riven_hints *, struct pelagial_metadata *),
    s32 argc, char **argv)
{
    /* provide defaults */
    struct riven_hints riven_hints = {
        .memory_budget = 1lu*1024*1024*1024, /* 1GB */
        .fiber_stack_size = 64lu*1024, /* 64KB */
        .fiber_count = 0,
        .tagged_heap_count = 0,
        .thread_count = 0,
        .log2_job_count = 0,
        .log2_arena_count = 0,
    };
    struct pelagial_metadata metadata = {
        .engine_name = "A Moonlit Walk Engine",
        .engine_build_ver = AMWE_VERSION,
        .app_timer_start = bedrock_rtc_counter(),
        .argc = argc,
        .argv = argv,
    };

    s32 res = 0;
    do {PFN_riven_framework framework = app_main(&riven_hints, &metadata);
        if (!framework) return 0;

        if (g_hints.encore_pelagial == NULL) {
            bedrock_log_fatal("The application must set 'pelagial' encore via 'amwe_hint_encore()' in main.");
            return -1;
        }
        res = riven_moonlit_walk(&riven_hints, framework, &metadata);
    } while (res == INT32_MAX);

    return res;
}
