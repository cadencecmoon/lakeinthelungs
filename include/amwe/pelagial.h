#pragma once

#include <amwe/riven.h>

/** An interface for game applications. */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Implemented by the application. */
struct pelagial_encore;

union pelagial_encore_view {
    struct riven_interface_header  *header;
    struct pelagia_interface       *interface;
    struct pelagia_encore          *encore;
    void                           *data;
};

struct pelagial_metadata {
    const char                     *engine_name;
    const char                     *app_name;
    u32                             engine_build_ver;
    u32                             app_build_ver;
    u64                             app_timer_start;
#if defined(LAKE_PLATFORM_WINDOWS)
#elif defined(LAKE_PLATFORM_APPLE_MACOS)
#elif defined(LAKE_PLATFORM_APPLE_IOS)
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
#elif defined(LAKE_PLATFORM_ANDROID)
#else /* unix/linux */
#endif /* LAKE_PLATFORM_WINDOWS */
    s32                             argc;
    char                          **argv;
};

/** Userdata is 'struct a_moonlit_walk_engine'. */
struct pelagial_interface {
    struct riven_interface_header   header;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
