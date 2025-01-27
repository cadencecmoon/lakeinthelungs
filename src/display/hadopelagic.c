#include "hadopelagic.h"

s32 hadal_init(
    struct hadopelagic   *hadal, 
    struct allocation    *alloc, 
    PFN_hadal_entry_point try_entry_point,
    PFN_hadal_entry_point fallback_entry_point,
    u32                   window_width,
    u32                   window_height,
    const char           *window_title)
{

}

void hadal_fini(struct hadopelagic *hadal)
{

}

void hadal_visibility(struct hadopelagic *hadal, b32 show)
{

}

#define HADAL_ENTRY_POINT_SCAFFOLD(backend)                                              \
    HADAL_ENTRY_POINT_DECLARATION(backend) {                                             \
        (void)hadal; (void)alloc;                                                        \
        if (!quiet)                                                                      \
            log_error("Display backend '%s' is not supported in this build.", #backend); \
        return result_feature_unsupported;                                               \
    }
#ifndef PLATFORM_WINDOWS
HADAL_ENTRY_POINT_SCAFFOLD(win32)
#endif
#ifndef PLATFORM_MACOS
HADAL_ENTRY_POINT_SCAFFOLD(cocoa)
#endif
#ifndef PLATFORM_IOS
HADAL_ENTRY_POINT_SCAFFOLD(ios)
#endif
#ifndef PLATFORM_ANDROID
HADAL_ENTRY_POINT_SCAFFOLD(android)
#endif
#ifndef HADOPELAGIC_WAYLAND
HADAL_ENTRY_POINT_SCAFFOLD(wayland)
#endif
#ifndef HADOPELAGIC_XCB
HADAL_ENTRY_POINT_SCAFFOLD(xcb)
#endif
#ifndef HADOPELAGIC_DRM
HADAL_ENTRY_POINT_SCAFFOLD(drm)
#endif

s32 hadal_entry_point(struct hadopelagic *hadal, struct allocation *alloc, b32 quiet)
{
#ifdef PLATFORM_WINDOWS
    if (hadal_win32_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef PLATFORM_MACOS
    if (hadal_cocoa_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef PLATFORM_IOS
    if (hadal_ios_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef PLATFORM_ANDROID
    if (hadal_android_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef HADOPELAGIC_WAYLAND
    if (hadal_wayland_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef HADOPELAGIC_XCB
    if (hadal_xcb_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
#ifdef HADOPELAGIC_DRM
    if (hadal_drm_entry_point(hadal, alloc, quiet) == result_success) return result_success;
#endif
    return result_failure;
}
