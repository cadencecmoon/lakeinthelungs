#ifndef _AMW_HADOPELAGIC_H
#define _AMW_HADOPELAGIC_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#include <lake/ipomoeaalba.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hadopelagic;

/** Entry point for the display backend. 
 *  @return True if was able to load a backend module and procedures. */
typedef s32 (*PFN_hadal_entry_point)(struct hadopelagic *hadal, struct ipomoeaalba *ia);

/** An enumeration to identify the native API used within the display backend.
 *  This information will be used mostly to either identify support or lack thereof
 *  some features, or to interact with the native window directly in places where 
 *  this direct access is needed (for example, the renderer's swapchain). */
enum hadal_backend_api {
    hadal_backend_api_win32 = 0,    // TODO reserved id
    hadal_backend_api_cocoa,        // TODO reserved id
    hadal_backend_api_ios,          // TODO reserved id
    hadal_backend_api_android,      // TODO reserved id
    hadal_backend_api_html5,        // TODO reserved id
    hadal_backend_api_wayland, 
    hadal_backend_api_xcb,          // TODO reserved id
    hadal_backend_api_drm,          // TODO reserved id
    hadal_backend_api_headless,
};

AMWAPI s32 hadal_wayland_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);
AMWAPI s32 hadal_headless_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);
/** Select the first appropriate display backend. */
AMWAPI s32 hadal_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia);

/** Initializes the internal display backends state. After this procedure is called,
 *  until PFN_hadal_display_fini is called, all other calls to the display backend 
 *  should be valid. Some functionality may be platform-specific tho, the backend 
 *  will handle these cases as appropriate. */
typedef s32 (*PFN_hadal_display_init)(struct hadopelagic *hadal, struct ipomoeaalba *ia, u32 width, u32 height, const char *title);

/** Closes the internal display backend. */
typedef void (*PFN_hadal_display_fini)(struct hadopelagic *hadal);

/** Returns the window size in pixel space. It's mostly used for cursor and mouse input. */
typedef void (*PFN_hadal_get_window_size)(struct hadopelagic *hadal, u32 *out_width, u32 *out_height);

/** Returns the size of the framebuffer in pixels. It's used by the swapchain. 
 *  Framebuffer size can be different than the window size, this depends on 
 *  some platform-specific DPI scaling, other platforms have no scaling tho. */
typedef void (*PFN_hadal_get_framebuffer_size)(struct hadopelagic *hadal, u32 *out_width, u32 *out_height);

/** The window will be visible to the user, and will accept input focus. */
typedef void (*PFN_hadal_show_window)(struct hadopelagic *hadal);

/** Hides the window. It still exists, but the user can't interact with it directly.
 *  If a window is hidden, it can't accept input focus, and can only be interacted with programatically. */
typedef void (*PFN_hadal_hide_window)(struct hadopelagic *hadal);

/** Returns platform-specific opaque handles for native access to the window system. 
 *  The handles must be casted back, this is different for every hadal_backend_api.
 *  A typical use case may be creating a swapchain surface. */
typedef void (*PFN_hadal_expose_native_window)(struct hadopelagic *hadal, void **out_display_handle, void **out_surface_handle);

/** Internal calls to be implemented by a display backend and assigned from the entry point. */
struct hadal_calls {
    PFN_hadal_display_init          display_init;
    PFN_hadal_display_fini          display_fini;
    PFN_hadal_get_window_size       get_window_size;
    PFN_hadal_get_framebuffer_size  get_framebuffer_size;
    PFN_hadal_show_window           show_window;
    PFN_hadal_hide_window           hide_window;
    PFN_hadal_expose_native_window  expose_native_window;
};

/** A collection of bit flags for reading and controlling the current window state.
 *  Values up to hadal_flag_reserved_bits are shared and reserved for future use,
 *  any bits above that can be used internally. */
enum hadal_flag {
    hadal_flag_initialized          = (1u << 0),  /**< True for a valid display backend. */
    hadal_flag_visible              = (1u << 1),  /**< Set if the window is visible to the user, or totally hidden. */
    hadal_flag_fullscreen           = (1u << 2),  /**< Set if the window is running in fullscreen mode. */
    hadal_flag_maximized            = (1u << 3),  /**< Set if the window is filling the screen space, ignored in fullscreen. */
    hadal_flag_minimized            = (1u << 4), 
    hadal_flag_auto_minimize        = (1u << 5),
    hadal_flag_is_resizing          = (1u << 6),
    hadal_flag_resizable            = (1u << 7),
    hadal_flag_decorated            = (1u << 8),
    hadal_flag_hovered              = (1u << 9),
    hadal_flag_focused              = (1u << 10),
    hadal_flag_focus_on_show        = (1u << 11),
    hadal_flag_activated            = (1u << 12),
    hadal_flag_should_close         = (1u << 13),
    hadal_flag_recreate_swapchain   = (1u << 14), /**< Set internally on framebuffer resizes, or when hiding the window state. Informs the renderer to update the swapchain. */
    hadal_flag_reserved_bits        = 20,
};
/** A bitmask for the reserved bits of a hadal_flag value. */
#define hadal_flag_mask_reserved ((1u << hadal_flag_reserved_bits) - 1)

/** Holds state responsible for interacting with the display, input devices and the window system. */
struct hadopelagic {
    at_u32              flags;          /**< Any write-modify operations outside the backend must be externally synchronized. */

    void               *display;        /**< Holds internal state for the display and for one window. Only one window is supported. */
    void              **outputs;        /**< A list of handles for available output monitors. */
    at_u32              output_count;   /**< The count of available outputs can dynamically change, as they can be connected/disconnected. */

    u32                 backend_api;    /**< Either enum hadal_backend_api or a custom identifier. */
    const char         *backend_name;   /**< A readable name of the running display backend, for logging purposes. */
    struct hadal_calls  calls;
};

/** Initializes the display backend and creates a window. Only one window at a time is supported. */
AMWAPI s32 hadal_init(
    PFN_hadal_entry_point entry_point__, 
    struct hadopelagic   *hadal, 
    struct ipomoeaalba   *ia,
    u32                   window_width, 
    u32                   window_height, 
    const char           *window_title);

/** Closes the display backend, including all window, input or event functionality. */
AMWAPI void hadal_fini(struct hadopelagic *hadal);

/** The size of a system window in screen coordinates, the framebuffer size can be different. */
AMWAPI void hadal_window_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height);

/** Get the current framebuffer size. This value can be different from the window size,
 *  depending on monitor configuration and fractional DPI scale on some platforms. */
AMWAPI void hadal_framebuffer_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height);

/** Control: hadal_flag_should_close. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the system window or application has requested to close,
 *  and all necessary termination or reinitialization must be done in this frame.
 *  Ofc., the flag can be ignored by setting it to false (if it was a close request 
 *  from the host windowing system, this can lead to undefined behaviour). */
AMWAPI u32 hadal_window_should_close(struct hadopelagic *hadal, b32 should);

/** Control: hadal_flag_visible. Returns old context flags, before the atomic operation.
 *
 *  If the flag is true, the window surface is created and images are being
 *  rendered to the swapchain. Setting it to false will hide the window. If the 
 *  visible value is equal to the current visible flag, no operation is done. */
AMWAPI u32 hadal_window_visible(struct hadopelagic *hadal, b32 visible);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_HADOPELAGIC_H */
