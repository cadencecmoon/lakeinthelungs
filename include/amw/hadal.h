#pragma once

#include <amw/bedrock.h>
#include <amw/input.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque handle of the display backend. Can be safely cast into 
 *  (struct hadal_interface *) if obtained from a valid display implementation. */
struct hadal;

/** Information needed to construct a display backend. */
struct hadal_create_info {
    struct riven_create_info_header header;

    u32         width, height;
    struct str  title;
};

#if defined(PLATFORM_WINDOWS)
/** The entry point to a Windows display backend. */
AMWAPI RIVEN_ENCORE(hadal, win32);
#elif defined(PLATFORM_APPLE_MACOS)
/** The entry point to an Apple MacOS Cocoa display backend. */
AMWAPI RIVEN_ENCORE(hadal, cocoa);
#elif defined(PLATFORM_APPLE_IOS)
/** The entry point to an Apple iOS display backend. */
AMWAPI RIVEN_ENCORE(hadal, ios);
#elif defined(PLATFORM_ANDROID)
/** The entry point to an Android display backend, implementing an activity. */
AMWAPI RIVEN_ENCORE(hadal, android);
#elif defined(PLATFORM_EMSCRIPTEN)
/** The entry point to a WebAssembly/Emscripten HTML5 display backend. */
AMWAPI RIVEN_ENCORE(hadal, html5);
#endif
#ifdef HADAL_WAYLAND
/** The entry point to a Linux Wayland display backend. */
AMWAPI RIVEN_ENCORE(hadal, wayland);
#endif
#ifdef HADAL_XCB
/** The entry point to a Linux X11 XCB display backend. */
AMWAPI RIVEN_ENCORE(hadal, xcb);
#endif
#ifdef HADAL_DRMKMS
/** The entry point to a Linux DRM/KMS display backend. */
AMWAPI RIVEN_ENCORE(hadal, drmkms);
#endif

/** The entry point to a headless display backend, available on all platforms.
 *  A swapchain may draw into a headless surface if it is supported by the GPU driver. */
AMWAPI RIVEN_ENCORE(hadal, headless);

/** Executes native encores until one writes to the interface:
 *  - win32, cocoa, ios, android, html5 - native builds.
 *  - wayland, xcb - Linux builds.
 *  - drkms - available on Linux and Android builds.
 *
 *  Available encores are tried in order:
 *      native builds -> wayland -> xcb -> drmkms -> headless.
 *
 *  If all options fail, the headless interface may be written as a fallback.
 *  This may be disabled by defining HADAL_DISABLE_HEADLESS_FALLBACK.
 *
 *  Other possible display backends that could be created:
 *      XBox, PlayStation 5, Nintendo Switch, XR/VR? */
AMWAPI RIVEN_ENCORE(hadal, native);

/** Acquires the framebuffer extent/resolution, may be different from the window size. */
typedef void (*PFN_acquire_framebuffer_extent)(struct hadal *hadal, u32 *width, u32 *height);

/** Procedures to be provided by an implementation. The backend must implement the 'struct hadal' and 
 *  put the hadal_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the display backend and the public interface defined below. */
struct hadal_interface {
    struct riven_interface_header   header;

    PFN_acquire_framebuffer_extent  acquire_framebuffer_extent;
};

/** Acquires the framebuffer extent/resolution, may be different from the window size. */
attr_inline attr_nonnull(1)
void hadal_acquire_framebuffer_extent(
    struct hadal *hadal,
    u32          *out_width, 
    u32          *out_height)
{
    struct hadal_interface *interface = (struct hadal_interface *)hadal;
    u32 width = 0, height = 0;

    interface->acquire_framebuffer_extent(hadal, &width, &height);
    if (out_width) *out_width = width;
    if (out_height) *out_height = height;
}

#ifdef __cplusplus
}
#endif
