#ifndef _AMW_HADOPELAGIC_H
#define _AMW_HADOPELAGIC_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/ipomoeaalba.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO maybe i'll rename and move these functions when the interface for them system-specific calls grows */
#ifndef AMW_NO_PROTOTYPES
/** Load the dynamic library into memory. */
AMWAPI void * AMWAPIENTRY hadal_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void AMWAPIENTRY hadal_close_dll(void *module);

/** Get a function address from a dynamic library. */
AMWAPI void * AMWAPIENTRY hadal_get_proc_address(void *module, const char *procedure);

/** Returns the system timer counter. */
AMWAPI uint64_t AMWAPIENTRY hadal_timer_counter(void);

/** Returns the frequency of the system timer. */
AMWAPI uint64_t AMWAPIENTRY hadal_timer_frequency(void);

/** Read some basic CPU info. */
AMWAPI void AMWAPIENTRY hadal_cpu_count(uint32_t *threads, uint32_t *cores, uint32_t *packages);
#endif /* AMW_NO_PROTOTYPES */

struct hadopelagic;

/** Entry point for the display backend. 
 *  @return True if was able to load a backend module and procedures.
 *
 *  Available display backends are:
 *  - Linux: Wayland, TODO XCB (X11), DRM/KMS
 *  - Apple: TODO Cocoa (MacOS), UIKit (iOS)
 *  - Windows TODO win32 API
 *  - Android TODO 13+, minimal ver. 10+
 *  - Headless for debug and validation */
typedef int32_t (AMWAPIENTRY *PFN_hadopelagic_entry_point)(struct hadopelagic *hadal, struct ipomoeaalba *ia);

#ifndef AMW_NO_PROTOTYPES
AMWAPI int32_t AMWAPIENTRY hadopelagic_wayland_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);
AMWAPI int32_t AMWAPIENTRY hadopelagic_headless_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);
/** Select the first appropriate display backend. */
AMWAPI int32_t AMWAPIENTRY hadopelagic_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);
#endif /* AMW_NO_PROTOTYPES */

typedef int32_t (AMWAPIENTRY *PFN_hadopelagic_init)(struct hadopelagic *hadal, struct ipomoeaalba *ia, uint32_t width, uint32_t height, const char *title);
typedef void    (AMWAPIENTRY *PFN_hadopelagic_fini)(struct hadopelagic *hadal);
typedef void    (AMWAPIENTRY *PFN_hadopelagic_get_window_size)(const struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);
typedef void    (AMWAPIENTRY *PFN_hadopelagic_get_framebuffer_size)(const struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);
typedef void    (AMWAPIENTRY *PFN_hadopelagic_show_window)(struct hadopelagic *hadal);
typedef void    (AMWAPIENTRY *PFN_hadopelagic_hide_window)(struct hadopelagic *hadal);

struct hadopelagic {
    at_uint32_t             flags;

    void                   *display;
    void                  **outputs;
    uint32_t                output_count;

    const char             *backend_name;
    struct {
        PFN_hadopelagic_init                    init;
        PFN_hadopelagic_fini                    fini;
        PFN_hadopelagic_get_window_size         get_window_size;
        PFN_hadopelagic_get_framebuffer_size    get_framebuffer_size;
        PFN_hadopelagic_show_window             show_window;
        PFN_hadopelagic_hide_window             hide_window;
    } calls;
};

/** A collection of bit flags for reading and controlling the current window state. 
 *  Internally it's an atomic uint32_t value, and is changed only using atomic operations.
 *  A read-only state of the flags can be accessed from non-local code with hadal_flags(). */
enum hadopelagic_flag {
    hadopelagic_flag_initialized    = (1u << 0), /* if false it means invalid state */
    hadopelagic_flag_visible        = (1u << 1),
    hadopelagic_flag_fullscreen     = (1u << 2),
    hadopelagic_flag_maximized      = (1u << 3),
    hadopelagic_flag_minimized      = (1u << 4),
    hadopelagic_flag_auto_minimize  = (1u << 5),
    hadopelagic_flag_is_resizing    = (1u << 6),
    hadopelagic_flag_resizable      = (1u << 7),
    hadopelagic_flag_decorated      = (1u << 8),
    hadopelagic_flag_hovered        = (1u << 9),
    hadopelagic_flag_focused        = (1u << 10),
    hadopelagic_flag_focus_on_show  = (1u << 11),
    hadopelagic_flag_activated      = (1u << 12),
    hadopelagic_flag_should_close   = (1u << 13),
};

#ifndef AMW_NO_PROTOTYPES

/** Initializes the display backend and creates a window. Only one window at a time is supported. */
AMWAPI int32_t AMWAPIENTRY 
hadopelagic_init(
        PFN_hadopelagic_entry_point entry_point__,
        struct hadopelagic *hadal,
        struct ipomoeaalba *ia,
        uint32_t window_width,
        uint32_t window_height,
        const char *window_title);

/** Closes the display backend, including all window, input or event functionality. */
AMWAPI void AMWAPIENTRY 
hadopelagic_fini(struct hadopelagic *hadal);

/** The size of a system window in screen coordinates, the framebuffer size can be different. */
AMWAPI void AMWAPIENTRY 
hadopelagic_window_size(struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);

/** Get the current framebuffer size. This value can be different from the window size,
 *  depending on monitor configuration and fractional DPI scale on some platforms. */
AMWAPI void AMWAPIENTRY 
hadopelagic_framebuffer_size(struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);

/** Control: hadal_flag_should_close. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the system window or application has requested to close,
 *  and all necessary termination or reinitialization must be done in this frame.
 *  Ofc., the flag can be ignored by setting it to false (if it was a close request 
 *  from the host windowing system, this can lead to undefined behaviour). */
AMWAPI uint32_t AMWAPIENTRY 
hadopelagic_window_should_close(struct hadopelagic *hadal, bool should);

/** Control: hadal_flag_visible. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the window surface is created and images are being
 *  rendered to the swapchain. Setting it to false will hide the window. If the 
 *  visible value is equal to the current visible flag, no operation is done. */
AMWAPI uint32_t AMWAPIENTRY 
hadopelagic_window_visible(struct hadopelagic *hadal, bool visible);

#endif /* AMW_NO_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_HADOPELAGIC_H */
