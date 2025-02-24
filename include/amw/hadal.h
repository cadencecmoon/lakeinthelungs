#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>
#include <amw/input.h>
#include <amw/string.h>

#define HADAL_MAX_OUTPUTS 8

struct hadopelagic;
struct harridan;
struct swapchain;

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
    struct hadopelagic     *display;
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

/** Numeric values to distinguish between the available display backends. */
enum hadal_backend_id {
    hadal_backend_invalid = 0,
    hadal_backend_win32,
    hadal_backend_cocoa,
    hadal_backend_ios,
    hadal_backend_android,
    hadal_backend_wayland,
    hadal_backend_xcb,
    hadal_backend_drmkms,
    hadal_backend_headless,
};

/** Defines an entry point for the display backend. Verbose will enable log messages, including errors. */
typedef s32 (*PFN_hadal_entry_point)(struct hadopelagic *hadal, b32 verbose);

AMWAPI s32 hadal_wayland_entry_point(struct hadopelagic *hadal, b32 verbose);

/** Picks the first valid entry point from the available above. */
AMWAPI s32 hadal_entry_point(struct hadopelagic *hadal, b32 verbose);

/** Initializes the display backend. */
typedef s32 (*PFN_hadal_display_init)(struct hadopelagic *hadal);

/** Cleanups the display backend. */
typedef void (*PFN_hadal_display_fini)(struct hadopelagic *hadal);

/** Creates a Vulkan surface for the display backend in use and a given swapchain. */
typedef s32 (*PFN_hadal_create_vulkan_surface)(
    struct hadopelagic *hadal, 
    struct harridan    *harridan, 
    struct swapchain   *swapchain);

/** Implements the display backend. */
struct hadal_interface {
    s32                     id;     /**< The numeric identifier of the display backend. */
    struct string           name;   /**< A readable name of the display backend, for logging. */

    PFN_hadal_display_init          display_init;
    PFN_hadal_display_fini          display_fini;
    PFN_hadal_create_vulkan_surface create_vulkan_surface;
};

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

/** Holds shared state responsible for interacting with the display backend, 
 *  different input devices and the window system. Only one window is supported. */
struct hadopelagic {
    at_u32                  flags;
    rivens_tag_t            tag;
    struct rivens          *riven;

    void                   *display;
    struct hadal_interface  interface;

    struct hadal_output    *outputs[HADAL_MAX_OUTPUTS];
    at_u32                  output_count;

    struct string           window_title;
    at_u32                  window_width, window_height;
    at_u32                  fb_width, fb_height;

    u32                     minwidth, minheight;
    u32                     maxwidth, maxheight;
    u32                     numer, denom;

    s16                     keycodes[256];
    s16                     scancodes[keycode_last + 1];
    s8                      keynames[keycode_last + 1][5];

    struct joystick         joysticks[16];
    struct gamepad         *mappings;
    at_u32                  mapping_count;

    s8                      keys[keycode_last + 1];
    s8                      mouse_buttons[mouse_button_last + 1];
};

/** Initialzies the display backend from a given configuration and entry point. */
AMWAPI attr_nonnull_all
s32 hadal_init(
    struct hadopelagic     *hadal,
    PFN_hadal_entry_point   entry,
    struct rivens          *riven,
    rivens_tag_t            tag,
    u32                     width,
    u32                     height,
    const struct string    *title,
    b32                     verbose_backend);

/** Closes the display backend and zeroes the state of hadal. */
AMWAPI attr_nonnull_all
void hadal_fini(struct hadopelagic *hadal);
