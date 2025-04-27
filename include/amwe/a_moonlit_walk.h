#pragma once

#include <amwe/lake_defines.h>
#include <amwe/bedrock.h>
#include <amwe/riven.h>

#include <amwe/hadal.h>
#include <amwe/hash_table.h>
#include <amwe/render_graph.h>
#include <amwe/soma.h>
#include <amwe/xaku.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AMWE_VERSION_MAJOR 0
#define AMWE_VERSION_MINOR 1
#define AMWE_VERSION_REVISION 6

#define AMWE_VERSION (LAKE_VERSION_NUM(AMWE_VERSION_MAJOR, AMWE_VERSION_MINOR, AMWE_VERSION_REVISION))
#define AMWE_CSTR_NAME "A Moonlit Walk Engine"

/** Implemented by the application. The pointer 'engine' inside 'struct riven_app' can be safely cast 
 *  into 'struct a_moonlit_walk_engine *' from within the encore. When the framework calls a lake encore, 
 *  it is assumed that other systems encores (e.g. display, renderer, audio engine, etc.) are not yet created. */
struct lake_encore;

/** Interface of the application. */
struct lake_interface {
    struct riven_interface          riven;
    struct a_moonlit_walk_engine   *amwe;
};
union lake_app {
    struct riven_interface         *riven;
    struct lake_interface          *interface;
    struct lake_encore             *encore;
    void                           *data;
};

enum amwe_hint_pipeline_setting {
    amwe_hint_pipeline_setting_auto = 0u,
    amwe_hint_pipeline_setting_parallel,
    amwe_hint_pipeline_setting_sequential,
    amwe_hint_pipeline_setting_max_enum,
};

#define AMWE_HINT_PIPELINE_SETTING 1
#define AMWE_HINT_NULL_FALLBACK_NATIVE_ENCORES 2

/** Overwrite the default hint for framework initialization. */
LAKEAPI void LAKECALL 
amwe_hint_framework(u32 hint, u32 value);

#define AMWE_HINT_ENCORE_HADAL     10000
#define AMWE_HINT_ENCORE_XAKU      10001
#define AMWE_HINT_ENCORE_SOMA      10002
#define AMWE_HINT_ENCORE_LAKE      10003

/** The following encores (in array order) will be tried for the framework initialization.
 *  If an encore fails or no hint was given, native encores will be tried with the 
 *  AMWE_HINT_NULL_FALLBACK_NATIVE_ENCORES 'null_fallback' argument, false by default. */
LAKEAPI void LAKECALL
amwe_hint_encore(u32 hint, PFN_riven_encore encore);

enum amwe_pipeline_stage_result {
    amwe_pipeline_stage_result_continue = 0,
    amwe_pipeline_stage_result_save_and_exit,
    amwe_pipeline_stage_result_fatal_error,
    amwe_pipeline_stage_result_reload_settings,
    amwe_pipeline_stage_result_restart_application,
};

struct amwe_pipeline_work {
    u64                             frame_index;
    f64                             delta_time;
    struct {
        u64                         simulation_begin_time;
        u64                         simulation_end_time;
        u64                         rendering_begin_time;
        u64                         rendering_end_time;
        u64                         gpuexec_begin_time;
        u64                         gpuexec_end_time;
    } profiling;
    enum amwe_pipeline_stage_result stage_result;

    struct a_moonlit_walk_engine   *amwe;
    struct amwe_pipeline_work      *last_work;
    struct amwe_pipeline_work      *next_work;
    riven_chain_t                   chain;
};

struct a_moonlit_walk_engine {
    struct riven_context            riven;
    union lake_app                  lake;
    union hadal_display             hadal;
    union soma_audio                soma;
    union xaku_renderer             xaku;
};

LAKEAPI s32 LAKECALL 
a_moonlit_walk(
    struct riven             *riven,
    const struct riven_hints *riven_hints,
    struct riven_app         *riven_app);

LAKEAPI s32 LAKECALL 
a_moonlit_walk_entry_point__(
    PFN_riven_framework (LAKECALL *app_main)(
        struct riven_hints *, struct riven_app *),
    s32 argc, char **argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef A_MOONLIT_WALK_FRAMEWORK
#undef A_MOONLIT_WALK_FRAMEWORK

extern PFN_riven_framework LAKECALL
lake_main(
    struct riven_hints *hints,
    struct riven_app   *app);

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>

s32 WINAPI WinMain(
    _In_ HINSTANCE     hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPSTR         lpCmdLine, 
    _In_ int           nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    /* TODO */
    return a_moonlit_walk_entry_point__(lake_main, 0, NULL);
}

#elif defined(LAKE_PLATFORM_APPLE_MACOS)
    /* TODO */

#elif defined(LAKE_PLATFORM_APPLE_IOS)
    /* TODO */

#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
    /* TODO */

#elif defined(LAKE_PLATFORM_ANDROID)
#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

JNIEXPORT void ANativeActivity_onCreate(
    ANativeActivity *activity, 
    void            *saved_state, 
    usize            saved_state_size) 
{
    (void)activity;
    (void)saved_state;
    (void)saved_state_size;
    /* TODO */
    return a_moonlit_walk_entry_point__(lake_main, 0, NULL);
}
#else /* unix/linux */
s32 main(s32 argc, char **argv)
{
    return a_moonlit_walk_entry_point__(lake_main, argc, argv);
}
#endif /* LAKE_PLATFORM_WINDOWS */
#endif /* A_MOONLIT_WALK_FRAMEWORK */
