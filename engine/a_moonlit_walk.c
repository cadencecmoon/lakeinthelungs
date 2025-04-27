#include <amwe/a_moonlit_walk.h>

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER 1024

#define ENCORE_COUNT 4
#define PIPELINE_WORK_COUNT 16
bedrock_static_assert(((PIPELINE_WORK_COUNT != 0) && ((PIPELINE_WORK_COUNT & (PIPELINE_WORK_COUNT - 1)) == 0)), 
        "pipeline work count must be a power of 2");
#define PIPELINE_WORK_MASK (PIPELINE_WORK_COUNT-1)

#define SIMULATION_WORK_INDEX 0
#define RENDERING_WORK_INDEX  1
#define GPUEXEC_WORK_INDEX    2

static struct framework_hints {
    enum amwe_hint_pipeline_setting pipeline_setting;

    bool                            null_fallback_native_encores;

    PFN_riven_encore                encore_hadal;
    PFN_riven_encore                encore_xaku;
    PFN_riven_encore                encore_soma;
    PFN_riven_encore                encore_lake;

} g_hints = {
    .pipeline_setting = amwe_hint_pipeline_setting_auto,
    .null_fallback_native_encores = false,
    .encore_hadal = NULL,
    .encore_xaku = NULL,
    .encore_soma = NULL,
    .encore_lake = NULL,
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
    case AMWE_HINT_ENCORE_LAKE:
        g_hints.encore_lake = encore; break;
    default: break;
    }
}

extern FN_RIVEN_INTERFACE_VALIDATION(hadal);
extern FN_RIVEN_INTERFACE_VALIDATION(xaku);
extern FN_RIVEN_INTERFACE_VALIDATION(soma);

static s32 engine_init(struct a_moonlit_walk_engine *amwe)
{
    s32 result = lake_result_success;
    struct riven_work begins[ENCORE_COUNT];
    struct riven_encore_assembly_work encore_assembly_work[ENCORE_COUNT]; 
    bedrock_log_verbose("- - - engine initialize - - -");

    bedrock_zeroa(encore_assembly_work);
    for (u32 i = 0; i < ENCORE_COUNT; i++) {
        encore_assembly_work[i].riven = &amwe->riven;
        /* for now don't own the tag */
        encore_assembly_work[i].flags = 0;
        encore_assembly_work[i].tag = riven_tag_roots;
        begins[i].procedure = (PFN_riven_work)riven_encore_assembly;
        begins[i].argument = &encore_assembly_work[i];
        begins[i].name = "amwe:init:encore_assembly";
    }
#define ENGINE_INIT_ENCORE(IDX, T) \
    encore_assembly_work[IDX].name = #T; \
    encore_assembly_work[IDX].preferred_encore = g_hints.encore_##T; \
    encore_assembly_work[IDX].native_encores = T##_native_encores(true, &encore_assembly_work[IDX].native_encore_count); \
    encore_assembly_work[IDX].interface_validation = (PFN_riven_interface_validation)T##_interface_validation; \
    encore_assembly_work[IDX].out_interface = &amwe->T.data;
    ENGINE_INIT_ENCORE(0, hadal)
    ENGINE_INIT_ENCORE(1, xaku)
    ENGINE_INIT_ENCORE(2, soma)
#undef ENGINE_INIT_ENCORE
    encore_assembly_work[3].name = "lake";
    encore_assembly_work[3].preferred_encore = g_hints.encore_lake;
    encore_assembly_work[3].out_interface = &amwe->lake.data;
    riven_split_work_and_unchain(amwe->riven.self, begins, ENCORE_COUNT);

    /* check results and exit on errors */
    for (u32 i = 0; i < ENCORE_COUNT; i++)
        if (*encore_assembly_work[i].out_interface == NULL) result = lake_result_error_initialization_failed;
    if (result != lake_result_success) return result;

    /* TODO continue */

    return result;
}

static void engine_fini(struct a_moonlit_walk_engine *amwe)
{
    bedrock_log_verbose("- - - engine finalize - - -");
    /* destroy child objects to decouple dependencies in core systems */
    /* TODO windows, swapchains, pipelines, devices, etc. */

    /* destroy encores */
    struct riven_work ends[ENCORE_COUNT];
    for (u32 i = 0; i < ENCORE_COUNT; i++) {
        ends[i].procedure = (PFN_riven_work)riven_encore_disassembly;
        ends[i].name = "amwe:fini:encore_disassembly";
    }
    ends[0].argument = amwe->hadal.data;
    ends[1].argument = amwe->xaku.data;
    ends[2].argument = amwe->soma.data;
    ends[3].argument = amwe->lake.data;
    riven_split_work_and_unchain(amwe->riven.self, ends, ENCORE_COUNT);
    bedrock_zerop(amwe);
}

static lake_hot void LAKECALL prepare_pipeline_work(struct amwe_pipeline_work *work, u64 index, f64 dt)
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

static void LAKECALL a_moonlit_walk_simulation(struct amwe_pipeline_work *work);
static void LAKECALL a_moonlit_walk_rendering(struct amwe_pipeline_work *work);
static void LAKECALL a_moonlit_walk_gpuexec(struct amwe_pipeline_work *work);

s32 a_moonlit_walk(
    struct riven             *riven,
    const struct riven_hints *riven_hints,
    struct riven_app         *riven_app)
{
    struct a_moonlit_walk_engine amwe = {
        .riven = {
            .self = riven,
            .hints = riven_hints,
            .app = riven_app,
        },
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
    riven_app->engine = &amwe;

    /* early initialization */
    exitcode = engine_init(&amwe);
    if (exitcode != 0) {
        engine_fini(&amwe);
        return exitcode;
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
        /* reset deferred scratch thread-local heaps */
        riven_thfree(riven, riven_tag_deferred);

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

            bedrock_log_verbose("- - - gameloop entry - - - parallel - - -");
            while (simulation || rendering || gpuexec || cleanup) {
                time_last = time_now;
                time_now = bedrock_rtc_counter();
                delta_time = ((f64)(time_now - time_last) * delta_time_frequency);

                bedrock_frame_time_record(riven_app->app_timer_start, time_now, delta_time_frequency);
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

                    pipeline_work[GPUEXEC_WORK_INDEX].argument = gpuexec;
                    riven_split_work(riven, &pipeline_work[GPUEXEC_WORK_INDEX], 1, &gpuexec->chain);
                }

                if (rendering) { /* frame N-1 */
                    /* chain points to the frame that may currently be in the simulation stage */
                    riven_unchain(riven, rendering->chain);
                    rendering->chain = NULL;

                    if (rendering->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = rendering->stage_result;

                    pipeline_work[RENDERING_WORK_INDEX].argument = rendering;
                    riven_split_work(riven, &pipeline_work[RENDERING_WORK_INDEX], 1, &rendering->chain);
                }

                if (simulation) { /* frame N */
                    prepare_pipeline_work(simulation, frame_index, delta_time);
                    pipeline_work[SIMULATION_WORK_INDEX].argument = simulation;
                    riven_split_work(riven, &pipeline_work[SIMULATION_WORK_INDEX], 1, &simulation->chain);
                }

                /* XXX debug, delete later */
                if (--close_counter <= 0) stage_result = amwe_pipeline_stage_result_save_and_exit;

                cleanup = gpuexec;
                gpuexec = rendering;
                rendering = simulation;
                simulation = stage_result == amwe_pipeline_stage_result_continue 
                    ? &frames[(++frame_index) & PIPELINE_WORK_MASK] : NULL;
                riven_rotate_deferred(riven);
            }
        } else {
            /** Runs the pipeline one frame at a time. This setting will lesser synchronization overhead and 
             *  decrease input latency, at potential cost of performance or CPU bottleneck on multicore CPUs. */
            struct amwe_pipeline_work *frame = frames;

            PFN_riven_work stages[] = {
                (PFN_riven_work)a_moonlit_walk_simulation,
                (PFN_riven_work)a_moonlit_walk_rendering,
                (PFN_riven_work)a_moonlit_walk_gpuexec,
            };

            bedrock_log_verbose("- - - gameloop entry - - - sequential - - -");
            while (frame) {
                time_last = time_now;
                time_now = bedrock_rtc_counter();
                delta_time = ((f64)(time_now - time_last) * delta_time_frequency);

                bedrock_frame_time_record(riven_app->app_timer_start, time_now, delta_time_frequency);
                bedrock_frame_time_print(1000.f);

                prepare_pipeline_work(frame, frame_index, delta_time);
                for (u32 i = 0; i < 3; i++) {
                    stages[i](frame);            

                    if (frame->stage_result != amwe_pipeline_stage_result_continue)
                        stage_result = frame->stage_result;
                }

                /* XXX debug, delete later */
                if (--close_counter <= 0) stage_result = amwe_pipeline_stage_result_save_and_exit;

                frame = stage_result == amwe_pipeline_stage_result_continue 
                    ? &frames[(++frame_index) & PIPELINE_WORK_MASK] : NULL;
                riven_rotate_deferred(riven);
            }
        } /* LEAVE GAMELOOP */
        bedrock_log_verbose("- - - gameloop leave - - - at frame %lu - - -", frame_index);

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

    engine_fini(&amwe);
    if (stage_result == amwe_pipeline_stage_result_restart_application)
        exitcode = lake_result_retry;
    return exitcode;
}

void a_moonlit_walk_simulation(struct amwe_pipeline_work *work)
{
    bedrock_assert_debug(work);
#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.simulation_begin_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */

#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.simulation_end_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */
}

void a_moonlit_walk_rendering(struct amwe_pipeline_work *work)
{
    bedrock_assert_debug(work);
#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.rendering_begin_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */

#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.rendering_end_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */
}

void a_moonlit_walk_gpuexec(struct amwe_pipeline_work *work)
{
    bedrock_assert_debug(work);
#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.gpuexec_begin_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */

#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.gpuexec_end_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */
}

s32 a_moonlit_walk_entry_point__(
    PFN_riven_framework (LAKECALL *app_main)(
        struct riven_hints *, struct riven_app *),
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
    struct riven_app riven_app = {
        .engine_name = AMWE_CSTR_NAME,
        .engine_build_ver = AMWE_VERSION,
        .app_timer_start = bedrock_rtc_counter(),
        .argc = argc,
        .argv = argv,
    };

    s32 res = 0;
    do {PFN_riven_framework framework = app_main(&riven_hints, &riven_app);
        if (!framework) return 0;

        if (g_hints.encore_lake == NULL) {
            bedrock_log_fatal("The application must set 'lake' encore via 'amwe_hint_encore(AMWE_HINT_ENCORE_LAKE, fn)' from entry point.");
            return -1;
        }
        res = riven_moonlit_walk(&riven_hints, framework, &riven_app);
        riven_app.engine = NULL;
    } while (res == lake_result_retry);

    return res;
}
