#ifndef _AMW_HADAL_H
#define _AMW_HADAL_H

#include <lake/defines.h>
#include <lake/atomic.h>

/** @file hadal.h
 *
 *  Hadal, or hadopelagic, is the deepest region of the ocean, lying within oceanic trenches. 
 *  In this engine, it serves as a direct abstraction to the operating system resources
 *  and the display backend, like windowing or handling input devices.
 *
 *  The hadal struct holds the opaque context state of a display backend. Functions that don't accept
 *  a pointer to this context are considered stateless, or have their internal state hidden. */
struct hadal;

/** Load the dynamic library into memory. */
AMWAPI void *hadal_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void hadal_close_dll(void *module);

/** Get a function address from a dynamic library. */
AMWAPI void *hadal_get_proc_address(void *module, const char *procedure);

/** Returns the system timer counter. */
AMWAPI uint64_t hadal_timer_counter(void);

/** Returns the frequency of the system timer. */
AMWAPI uint64_t hadal_timer_frequency(void);

AMWAPI void hadal_cpu_count(size_t *threads, size_t *cores, size_t *packages);

/** Specifies an ID for a display backend. Not all display backends are 
 *  available, they are different on every platform. */
enum hadal_api {
    hadal_api_auto = 0u,
    hadal_api_win32,
    hadal_api_cocoa,
    hadal_api_ios,
    hadal_api_android,
    hadal_api_wayland,
    hadal_api_xcb,
    hadal_api_kms,
    hadal_api_headless,
    hadal_api_count,
};

/** True if the given backend is available on the host system. */
AMWAPI bool hadal_api_is_supported(const uint32_t api);

/** Initializes the display backend. */
AMWAPI struct hadal *hadal_init(
        enum hadal_api api, 
        uint32_t window_width,
        uint32_t window_height,
        const char *window_title,
        bool allow_headless);

/** Reinitialize the display with a fallback backend. It first tries to open any other
 *  supported backend (that is the case for linux: DRM/KMS, XCB or Wayland), if they don't 
 *  work then, if requested, the display will enter headless mode and NULL out some stuff.
 *  In headless mode, the display api will work correctly with lack of some functionality,
 *  and there is no real graphical window to present - screenshots will still work tho. */
AMWAPI int32_t hadal_fallback(struct hadal *hadal, bool allow_headless);

/** Closes the display backend, including all window, input or event functionality. */
AMWAPI void hadal_fini(struct hadal *hadal);

/** The size of a system window in screen coordinates, the framebuffer size can be different. */
AMWAPI void hadal_window_size(
        struct hadal *hadal,
        uint32_t *out_width, 
        uint32_t *out_height);

/** Get the current framebuffer size. This value can be different from the window size,
 *  depending on monitor configuration and fractional DPI scale on some platforms. */
AMWAPI void hadal_framebuffer_size(
        struct hadal *hadal,
        uint32_t *out_width, 
        uint32_t *out_height);

/** A collection of bit flags for reading and controlling the current window state. 
 *  Internally it's an atomic uint32_t value, and is changed only using atomic operations.
 *  A read-only state of the flags can be accessed from non-local code with hadal_flags(). */
enum hadal_flag {
    hadal_flag_should_close   = (1u << 0),
    hadal_flag_visible        = (1u << 1),
    hadal_flag_fullscreen     = (1u << 2),
    hadal_flag_maximized      = (1u << 3),
    hadal_flag_minimized      = (1u << 4),
    hadal_flag_auto_minimize  = (1u << 5),
    hadal_flag_is_resizing    = (1u << 6),
    hadal_flag_resizable      = (1u << 7),
    hadal_flag_decorated      = (1u << 8),
    hadal_flag_hovered        = (1u << 9),
    hadal_flag_focused        = (1u << 10),
    hadal_flag_focus_on_show  = (1u << 11),
    hadal_flag_activated      = (1u << 12),
    hadal_flag_initialized    = (1u << 13), /* if false it means invalid state */
};

/** Atomically retrieve current hadal context flags from non-local code, a read only copy. */
AMWAPI uint32_t hadal_flags(struct hadal *hadal, enum memory_model model);

/** Retrieve a pointer to the hadal context flags from non-local code, this can be used in atomic
 *  read/modify/write operations. The calling code must ensure proper handling of this atomic type.
 *  If the context is invalid, NULL is returned instead. */
AMWAPI at_uint32_t *hadal_at_flags(struct hadal *hadal);

/** Control: hadal_flag_should_close. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the system window or application has requested to close,
 *  and all necessary termination or reinitialization must be done in this frame.
 *  Ofc., the flag can be ignored by setting it to false (if it was a close request 
 *  from the host windowing system, this can lead to undefined behaviour). */
AMWAPI uint32_t hadal_should_close(struct hadal *hadal, bool should);

/** Control: hadal_flag_visible. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the window surface is created and images are being
 *  rendered to the swapchain. Setting it to false will hide the window. If the 
 *  visible value is equal to the current visible flag, no operation is done. */
AMWAPI uint32_t hadal_visible(struct hadal *hadal, bool visible);

#endif /* _AMW_HADAL_H */