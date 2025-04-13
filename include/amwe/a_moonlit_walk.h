#pragma once

#include <amwe/lake_defines.h>
#include <amwe/bedrock.h>
#include <amwe/riven.h>

#include <amwe/hash_table.h>

#include <amwe/hadal.h>
#include <amwe/pelagial.h>
#include <amwe/soma.h>
#include <amwe/xaku.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AMWE_VERSION_MAJOR 0
#define AMWE_VERSION_MINOR 1
#define AMWE_VERSION_REVISION 5

#define AMWE_VERSION (LAKE_VERSION_NUM(AMWE_VERSION_MAJOR, AMWE_VERSION_MINOR, AMWE_VERSION_REVISION))

struct amwe_pipeline_work {
    u64                             frame_index;
    f64                             delta_time;

    struct a_moonlit_walk_engine   *amwe;
    struct amwe_pipeline_work      *last_work;
    struct amwe_pipeline_work      *next_work;
    riven_chain_t                   chain;
};

struct a_moonlit_walk_engine {
    struct riven                   *riven;
    const struct riven_hints       *riven_hints;
    const struct pelagial_metadata *metadata;
    bedrock_thread_t               *threads;

    union pelagial_encore_view      pelagial;
    struct hadal                    hadal;
    struct soma                     soma;
    struct xaku                     xaku;
};


LAKEAPI s32 LAKECALL 
a_moonlit_walk(
    struct riven                   *riven,
    const struct riven_hints       *riven_hints,
    bedrock_thread_t               *threads,
    struct pelagial_metadata       *metadata);

LAKEAPI s32 LAKECALL 
a_moonlit_walk_entry_point__(
    PFN_riven_framework (LAKECALL *app_main)(
        struct pelagial_metadata *, struct riven_hints *, s32 argc, char **argv),
    struct pelagial_metadata *metadata, s32 argc, char **argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef A_MOONLIT_WALK_FRAMEWORK
#undef A_MOONLIT_WALK_FRAMEWORK

extern PFN_riven_framework LAKECALL
pelagial_main(
    struct pelagial_metadata   *metadata,
    struct riven_hints         *hints, 
    s32                         argc, 
    char                      **argv);

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/arch/windows.h>

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
    struct pelagial_metadata metadata = {0};
    return a_moonlit_walk_entry_point__(pelagial_main, &metadata, 0, NULL);
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
    struct pelagial_metadata metadata = {0};
    return a_moonlit_walk_entry_point__(pelagial_main, &metadata, 0, NULL);
}
#else /* unix/linux */
s32 main(s32 argc, char **argv)
{
    struct pelagial_metadata metadata = {0};
    return a_moonlit_walk_entry_point__(pelagial_main, &metadata, argc, argv);
}
#endif /* LAKE_PLATFORM_WINDOWS */
#endif /* A_MOONLIT_WALK_FRAMEWORK */
