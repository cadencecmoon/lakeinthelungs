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
struct hadal_overture {
    struct rivens_overture_header   header;
    u32                             width, height;
    struct str                      title;
};

#if defined(PLATFORM_WINDOWS)
/** The entry point to a Windows display backend. */
AMWAPI RIVENS_ENCORE(hadal, win32);
#elif defined(PLATFORM_APPLE_MACOS)
/** The entry point to an Apple MacOS Cocoa display backend. */
AMWAPI RIVENS_ENCORE(hadal, cocoa);
#elif defined(PLATFORM_APPLE_IOS)
/** The entry point to an Apple iOS display backend. */
AMWAPI RIVENS_ENCORE(hadal, ios);
#elif defined(PLATFORM_ANDROID)
/** The entry point to an Android display backend, implementing an activity. */
AMWAPI RIVENS_ENCORE(hadal, android);
#elif defined(PLATFORM_EMSCRIPTEN)
/** The entry point to a WebAssembly/Emscripten HTML5 display backend. */
AMWAPI RIVENS_ENCORE(hadal, html5);
#endif
#ifdef HADAL_WAYLAND
/** The entry point to a Linux Wayland display backend. */
AMWAPI RIVENS_ENCORE(hadal, wayland);
#endif
#ifdef HADAL_XCB
/** The entry point to a Linux X11 XCB display backend. */
AMWAPI RIVENS_ENCORE(hadal, xcb);
#endif
#ifdef HADAL_DRMKMS
/** The entry point to a Linux DRM/KMS display backend. */
AMWAPI RIVENS_ENCORE(hadal, drmkms);
#endif

/** The entry point to a headless display backend, available on all platforms.
 *  A swapchain may draw into a headless surface if it is supported by the GPU driver. */
AMWAPI RIVENS_ENCORE(hadal, headless);

/** Writes a list of encores native to the host system. May write a headless encore if requested,
 *  but it will be written if there is no other display backend available.
 *
 *  The following encores may be written:
 *  - win32, cocoa, ios, android, html5 - native builds.
 *  - drkms - available on Linux and Android builds.
 *  - wayland, xcb - Linux builds.
 *  - headless - if requested, or as a fallback.
 *
 *  Available encores are written in the order they are declared.
 *
 *  Other possible display backends that could be created:
 *      XBox, PlayStation 5, Nintendo Switch, XR/VR? */
AMWAPI attr_nonnull_all
const PFN_rivens_encore *hadal_acquire_native_encores(u32 *out_count, b32 fallback);

/** Acquires the framebuffer extent/resolution, may be different from the window size. */
typedef void (*PFN_acquire_framebuffer_extent)(struct hadal *hadal, u32 *width, u32 *height);

/** Flags describing the state or running configuration of the display backend. */
enum hadal_flags {
    /** Set whenever an event from external means (outside of the application, the user) 
     *  has requested the display or system window to quit. We can assume that if such 
     *  thing happens, the application should save the game, cleanup and exit. */
    hadal_flag_window_should_close = (1u << 0),
};

/** Procedures to be provided by an implementation. The backend must implement the 'struct hadal' and 
 *  put the hadal_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the display backend and the public interface defined below. */
struct hadal_interface {
    struct rivens_interface_header  header;
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
