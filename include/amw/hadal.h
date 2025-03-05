#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>
#include <amw/input.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HADAL_MAX_OUTPUTS
#define HADAL_MAX_OUTPUTS 8
#endif

struct hadopelagic;
struct hadopelagic_interface;
struct hadopelagic_create_info;

/** 2D raw image data. */
struct image {
    u32 width;          /**< The width of this image, in pixels. */
    u32 height;         /**< The height of this image, in pixels. */
    u8 *pixels;         /**< Pixel data of this image, arranged left-to-right, top-to-bottom. */
};

/** Describes a single output video mode. */
struct vidmode {
    s32 width;          /**< The width of the video mode, in screen coordinates. */
    s32 height;         /**< The height of the video mode, in screen coordinates. */
    s32 red_bits;       /**< The bit depth of the red channel. */
    s32 green_bits;     /**< The bit depth of the green channel. */
    s32 blue_bits;      /**< The bit depth of the blue channel. */
    s32 refresh_rate;   /**< The refresh rate of the video mode, in Hz. */
};

/** Describes the gamma ramp for a monitor output. */
struct gammaramp {
    u16 *red;           /**< An array of value describing the response of the red channel. */
    u16 *green;         /**< An array of value describing the response of the green channel. */
    u16 *blue;          /**< An array of value describing the response of the blue channel. */
    u32 size;           /**< Number of elements in each array. */
};

/** Shared data of a single monitor output, with a pointer to the internal state. */
struct hadal_output {
    char                    name[128];
    struct hadopelagic     *hadal;
    void                   *internal;

    u32                     width_mm, height_mm;

    struct vidmode         *modes;
    u32                     mode_count;
    struct vidmode          current_mode;

    struct gammaramp        original_ramp;
    struct gammaramp        current_ramp;
};

/** A state of an button input, can be either pressed, released, repeated or invalid. */
enum hadal_key {
    hadal_key_unknown = -1,
    hadal_key_release = 0,
    hadal_key_press   = 1,
    hadal_key_repeat  = 2,
};

/** Arguments of the entry point to the display backend. */
#define _HADAL_ENTRY_POINT_ARGS                                 \
    struct hadopelagic_interface         *interface,   \
    const struct hadopelagic_create_info *create_info

/** Defines an entry point for the display backend. */
typedef enum result attr_nonnull_all (*PFN_hadal_entry_point)(_HADAL_ENTRY_POINT_ARGS);

/** Declares a hadal entry point implementation. */
#define HADAL_ENTRY_POINT(name) \
    s32 attr_nonnull_all hadal_##name##_entry_point(_HADAL_ENTRY_POINT_ARGS)

/* Predefined display backends available. */
AMWAPI HADAL_ENTRY_POINT(win32);    /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(cocoa);    /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(ios);      /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(android);  /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(html5);    /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(wayland);
AMWAPI HADAL_ENTRY_POINT(xcb);      /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(drmkms);   /* TODO reserved */
AMWAPI HADAL_ENTRY_POINT(headless);

/** Picks the first valid entry point from the available above, except headless.
 *  A headless display backend must be explicitly entered. */
AMWAPI attr_nonnull_all 
enum result hadal_entry_point(_HADAL_ENTRY_POINT_ARGS);

/** Initializes the display backend. */
typedef enum result (*PFN_hadal_display_init)(
    struct hadopelagic                   *hadal,
    const struct hadopelagic_create_info *create_info);

/** Cleanups the display backend. */
typedef void (*PFN_hadal_display_fini)(void *internal);

/** Returns the dimensions of a surface framebuffer, this may be different from the window size. */
typedef void (*PFN_hadal_get_framebuffer_extent)(
    struct hadopelagic *hadal,
    u32                *out_width,
    u32                *out_height);

struct pelagic_ocean;

/** Creates a surface that a rendering device can draw to. In the interface there 
 *  are Vulkan and other platform-specific variants (D3D12, WebGPU, Metal, etc.). */
typedef enum result (*PFN_hadal_create_surface)(struct hadopelagic *hadal, struct pelagic_ocean *pelagial);

/** Flags describing the state of a display backend. This includes all state related
 *  to a window, implying that only one window is supported at a time. */
enum hadal_flags {
    hadal_flag_display_is_valid     = (1u << 0),    /**< Always true for a valid display backend. */
    hadal_flag_window_visible       = (1u << 1),    /**< Whether the window is visible for user interaction or hidden. */
    hadal_flag_fullscreen           = (1u << 2),    /**< True in fullscreen mode. */
    hadal_flag_maximized            = (1u << 3),    /**< The window's content area is filling the screen space, ignored in fullscreen. */
    hadal_flag_minimized            = (1u << 4),    /**< The window is minimized to the task bar. */
    hadal_flag_resizable            = (1u << 5),    /**< The user can resize the window by dragging it's borders. */
    hadal_flag_decorated            = (1u << 6),    /**< Whether window decorations are in use, platform dependent. */
    hadal_flag_cursor_hovered       = (1u << 7),    /**< Whether a cursor hovers over the window space, does not imply key input focus. */
    hadal_flag_has_input_focus      = (1u << 8),    /**< If the window will accept input from keyboard, joystick, pen, touch devices, etc. */
    hadal_flag_locked_key_mods      = (1u << 9),    /**< Key modifiers (ctrl, shift, alt, etc.) are in a locked state. */
    hadal_flag_sticky_keys          = (1u << 10),   /**< Enables accessibility features regarding sticky keys. */
    hadal_flag_sticky_mouse_buttons = (1u << 11),   /**< Enables accessibility features regarding sticky mouse buttons. */
    hadal_flag_mouse_passthrough    = (1u << 12),   /**< The window will passthrough mouse button input to other windows beneath. */
    hadal_flag_framebuffer_resized  = (1u << 13),   /**< Will be set whenever the window size or framebuffer scaling changes, signals the need to recreate the swapchain. */
    hadal_flag_shell_activated      = (1u << 14),   /**< Shell communication with the compositor, available on some platforms. */
    hadal_flag_should_close         = (1u << 15),   /**< Whether the window was requested to close by means outside of the application. */
};

/** Procedures implemented by the display backend. */
struct hadopelagic_interface {
    const char                         *name;       /**< A readable name of the display backend, for logging. */
    void                               *internal;   /**< The display backend. */

    PFN_hadal_display_init              display_init;
    PFN_hadal_display_fini              display_fini;
    PFN_hadal_create_surface            create_vulkan_surface;
    PFN_hadal_create_surface            create_native_surface;
    PFN_hadal_get_framebuffer_extent    get_framebuffer_extent;
};

/** Holds shared state responsible for interacting with the display backend, 
 *  different input devices and the window system. Only one window is supported. */
struct hadopelagic {
    at_u32                          flags;
    struct hadopelagic_interface    interface;

    rivens_tag_t                    tag;    /**< The lifetime of this system. */
    struct rivens                  *riven;

    struct hadal_output            *outputs[HADAL_MAX_OUTPUTS];
    u32                             output_count;

    const char                     *window_title;
    at_u32                          window_width, window_height;

    u32                             minwidth, minheight;
    u32                             maxwidth, maxheight;
    u32                             numer, denom;

    s16                             keycodes[256];
    s16                             scancodes[keycode_last + 1];
    s8                              keynames[keycode_last + 1][5];

    struct joystick                 joysticks[16];
    struct gamepad                 *mappings;
    at_u32                          mapping_count;

    s8                              keys[keycode_last + 1];
    s8                              mouse_buttons[mouse_button_last + 1];
};

/** Information needed to construct a display backend. */
struct hadopelagic_create_info {
    struct rivens *riven;
    rivens_tag_t   tag;
    u32            window_width;
    u32            window_height;
    const char    *window_title;
};

/** Initialzies the display backend from a given configuration and entry point. If an entry points 
 *  fails, the next entry point from the given array is used as a fallback, until initialized 
 *  successfully or until there are no more entry points requested to try. */
AMWAPI attr_nonnull_all
struct hadopelagic *hadal_init(
    const PFN_hadal_entry_point          *entries,
    u32                                   entry_count,
    const struct hadopelagic_create_info *create_info);

/** Closes the display backend and zeroes the state of hadal. */
AMWAPI attr_nonnull_all
void hadal_fini(struct hadopelagic *hadal);

#ifdef __cplusplus
}
#endif
