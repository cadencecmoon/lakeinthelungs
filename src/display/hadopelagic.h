#pragma once

#include "../bedrock.h"
#include "../memory/allocator.h"

#include "input/gamepad.h"
#include "input/joystick.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/pen.h"
#include "input/touch.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hadopelagic;

/** Codes to identify the current display backend. */
enum hadal_backend {
    hadal_backend_invalid = 0,
    hadal_backend_win32,
    hadal_backend_cocoa,
    hadal_backend_ios,
    hadal_backend_android,
    hadal_backend_wayland,
    hadal_backend_xcb,
    hadal_backend_drm,
    hadal_backend_headless,
};

/** Entry point for the display backend. Set quiet to disable error messages. */
typedef s32 (*PFN_hadal_entry_point)(struct hadopelagic *hadal, struct allocation *alloc, b32 quiet);

#define HADAL_ENTRY_POINT_DECLARATION(backend) \
    attr_unused LAKEAPI s32 hadal_##backend##_entry_point(struct hadopelagic *hadal, struct allocation *alloc, b32 quiet)
HADAL_ENTRY_POINT_DECLARATION(win32);
HADAL_ENTRY_POINT_DECLARATION(cocoa);
HADAL_ENTRY_POINT_DECLARATION(ios);
HADAL_ENTRY_POINT_DECLARATION(android);
HADAL_ENTRY_POINT_DECLARATION(wayland);
HADAL_ENTRY_POINT_DECLARATION(xcb);
HADAL_ENTRY_POINT_DECLARATION(drm);
HADAL_ENTRY_POINT_DECLARATION(headless);

/** Commit to the first available display backend (not headless). */
LAKEAPI s32 hadal_entry_point(struct hadopelagic *hadal, struct allocation *alloc, b32 quiet);

/** Initializes the internal display backends state. After this procedure is called,
 *  until PFN_hadal_display_fini is called, all other calls to the display backend 
 *  should be valid. Some functionality may be platform-specific tho, the backend 
 *  will handle these cases as appropriate. */
typedef s32 (*PFN_hadal_display_init)(struct hadopelagic *hadal, struct allocation *alloc, u32 width, u32 height, const char *title);

/** Closes the internal display backend. */
typedef void (*PFN_hadal_display_fini)(struct hadopelagic *hadal);

/** Returns the window size in pixel space. It's mostly used for cursor and mouse input. */
typedef void (*PFN_hadal_get_window_size)(struct hadopelagic *hadal, u32 *width, u32 *height);

/** Returns the size of the framebuffer in pixels. It's used by the swapchain. 
 *  Framebuffer size can be different than the window size, this depends on 
 *  some platform-specific DPI scaling, other platforms have no scaling tho. */
typedef void (*PFN_hadal_get_framebuffer_size)(struct hadopelagic *hadal, u32 *width, u32 *height);

/** The window will be visible to the user, and will accept input focus. */
typedef void (*PFN_hadal_show_window)(struct hadopelagic *hadal);

/** Hides the window. It still exists, but the user can't interact with it directly.
 *  If a window is hidden, it can't accept input focus, and can only be interacted with programatically. */
typedef void (*PFN_hadal_hide_window)(struct hadopelagic *hadal);

/** Returns platform-specific opaque handles for native access to the window system. 
 *  The handles must be casted back, this is different for every native backend.
 *  A typical use case is creating a swapchain surface. */
typedef void (*PFN_hadal_expose_native_window)(struct hadopelagic *hadal, void **display_handle, void **surface_handle);

/** Procedures to be implemented by a display backend, pointers are retrieved 
 *  from an entry point function. */
struct hadal_interface {
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
enum hadal_flags {
    hadal_flag_display_is_valid     = (1u << 0),  /**< True for a valid display backend. */
    hadal_flag_window_visible       = (1u << 1),  /**< Set if the window is visible for user interaction, or totally hidden. */
    hadal_flag_fullscreen           = (1u << 2),  /**< Set if the window is running in fullscreen mode. */
    hadal_flag_maximized            = (1u << 3),  /**< Set if the window is filling the screen space, ignored in fullscreen. */
    hadal_flag_minimized            = (1u << 4),  /**< Set if the window is minimized to the task bar. */
    hadal_flag_auto_minimize        = (1u << 5),  /**< If true, exiting fullscreen will minimize the window. */
    hadal_flag_is_resizing          = (1u << 6),  /**< This flag is set per-frame if the window content area is currenly changing. */
    hadal_flag_resizable            = (1u << 7),  /**< True if the user can resize the window by dragging its borders. */
    hadal_flag_decorated            = (1u << 8),  /**< True if window decorations are being used. */
    hadal_flag_cursor_hovered       = (1u << 9),  /**< True if a mouse cursor is hovered over the window space. */
    hadal_flag_input_focus          = (1u << 10), /**< True if the window has input focus. */
    hadal_flag_focus_on_show        = (1u << 11), /**< If true, the window will hijack input focus when visible changes to true. */
    hadal_flag_activated            = (1u << 12), /**< Shell communication with the compositor, available on some platforms. */
    hadal_flag_should_close         = (1u << 13), /**< True if the window was requested to close by means outside of the application. */
    hadal_flag_framebuffer_resized  = (1u << 14), /**< Set on framebuffer resizes, or when hiding the window. Informs the renderer to update the swapchain. */
};

/** Holds state responsible for interacting with the display, input devices and the window system. */
struct hadopelagic {
    at_u32                  flags;          /**< Any write-modify operations must be synchronized. */

    void                   *display;        /**< Holds internal state for the display and for one window. Only one window is supported. */

    s32                     backend_id;     /**< A numerical identifier for the display backend. */
    const char             *backend_name;   /**< A readable name of the running display backend, for logging purposes. */
    struct hadal_interface  interface;      /**< Calls to the display backend. */
};

/** Initializes the display backend and a window state from the given configuration. */
LAKEAPI s32 hadal_init(
    struct hadopelagic   *hadal, 
    struct allocation    *alloc, 
    PFN_hadal_entry_point try_entry_point,
    PFN_hadal_entry_point fallback_entry_point,
    u32                   window_width,
    u32                   window_height,
    const char           *window_title)
    attr_nonnull(1,2,3,7);

/** Terminates the display backend. */
LAKEAPI void hadal_fini(struct hadopelagic *hadal);

/** Control the visibility state of a window. The window can be either shown or hidden from user interaction. */
LAKEAPI void hadal_visibility(struct hadopelagic *hadal, b32 show) attr_nonnull(1);

#ifdef __cplusplus
}
#endif
