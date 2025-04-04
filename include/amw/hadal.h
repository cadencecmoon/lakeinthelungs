#pragma once

#include <amw/bedrock/defines.h>
#include <amw/bedrock/keycodes.h>
#include <amw/bedrock/scancodes.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HADAL_MAX_OUTPUTS
#define HADAL_MAX_OUTPUTS 8
#endif

#ifndef HADAL_MAX_JOYSTICKS
#define HADAL_MAX_JOYSTICKS 16
#endif

/** Represents a pen device for touchscreens or drawing tablets. */
struct hadal_pen;
/** Represents a touchscreen or trackpad device. */
struct hadal_touch;
/** Represents a mouse or pointer device. */
struct hadal_mouse;
/** Represents a keyboard device. */
struct hadal_keyboard;
/** Represents a joystick device. Can be cast into (struct hadal_joystick_header *). */
struct hadal_joystick;
/** Represents a native window. Can be cast into (struct hadal_window_header *). */
struct hadal_window;
/** Represents an output monitor. Can be cast into (struct hadal_output_header *). */
struct hadal_output;
/** An opaque handle of the display backend. Can be safely cast into 
 *  (struct hadal_interface *) if obtained from a valid display implementation. */
struct hadal;

/** Information needed to construct a display backend. */
struct hadal_encore {
    struct riven_encore_header      header;
    /** A gamepad mapping database file, this is optional but necessary for joysticks to work.
     *  This can be given at initialization, or later.
     *
     *  https://github.com/mdqinc/SDL_GameControllerDB
     *
     *  While convenient, this tool has fallen out of date as SDL has amended and added new 
     *  features for gamepad support. I may use this database for now, but we'll see how 
     *  my project grows later. */
    const char                     *gamecontrollerdb;
};

#if defined(PLATFORM_WINDOWS)
/** The entry point to a Windows display backend. */
AMWAPI RIVEN_ENCORE(hadal, win32);
#elif defined(PLATFORM_APPLE_MACOS)
/** The entry point to an Apple MacOS Cocoa display backend. */
AMWAPI RIVEN_ENCORE(hadal, cocoa);
#elif defined(PLATFORM_APPLE_IOS)
/** The entry point to an Apple iOS UIKit display backend. */
AMWAPI RIVEN_ENCORE(hadal, uikit);
#elif defined(PLATFORM_ANDROID)
/** The entry point to an Android display backend, implementing an activity. */
AMWAPI RIVEN_ENCORE(hadal, android);
#elif defined(PLATFORM_EMSCRIPTEN)
/** The entry point to a WebAssembly/Emscripten HTML5 display backend. */
AMWAPI RIVEN_ENCORE(hadal, emscripten);
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

/** Describes a single output video mode. */
struct hadal_vidmode {
    s32 width;          /**< The width of the video mode, in screen coordinates. */
    s32 height;         /**< The height of the video mode, in screen coordinates. */
    s32 red_bits;       /**< The bit depth of the red channel. */
    s32 green_bits;     /**< The bit depth of the green channel. */
    s32 blue_bits;      /**< The bit depth of the blue channel. */
    s32 refresh_rate;   /**< The refresh rate of the video mode, in Hz. */
};

/** Describes the gamma ramp for a monitor output. */
struct hadal_gammaramp {
    u16 *red;           /**< An array of value describing the response of the red channel. */
    u16 *green;         /**< An array of value describing the response of the green channel. */
    u16 *blue;          /**< An array of value describing the response of the blue channel. */
    u32 size;           /**< Number of elements in each array. */
};

/** The public interface to an output monitor. An output implementation is expected 
 *  to include this header as its first struct member. */
struct hadal_output_header {
    struct hadal           *hadal;
    char                    name[128];
    /** Either NULL or a valid window handle if in fullscreen mode. */
    struct hadal_output    *window;

    u32                     pos_x, pos_y;
    u32                     width_mm, height_mm;

    struct hadal_vidmode   *modes;
    u32                     mode_count;
    struct hadal_vidmode    current_mode;

    struct hadal_gammaramp  original_ramp;
    struct hadal_gammaramp  current_ramp;
};

/** Flags that describe a window state. */
enum hadal_window_flags {
    hadal_window_flag_is_valid          = (1u << 0),
    /** Window is visible and the user can interact with it. */
    hadal_window_flag_visible           = (1u << 1),
    /** Window is in exclusive fullscreen mode. When true, some other flags may be ignored. */
    hadal_window_flag_fullscreen        = (1u << 2),
    /** Window is filling the screen space, ignored in fullscreen. */
    hadal_window_flag_maximized         = (1u << 3),
    /** Window is minimized to the task bar. */
    hadal_window_flag_minimized         = (1u << 4),
    /** Window can be resized by the user by dragging it. */
    hadal_window_flag_resizable         = (1u << 5),
    /** Window decorations are enabled, otherwise it is borderless. */
    hadal_window_flag_decorated         = (1u << 6),
    /** Window has input focus. */
    hadal_window_flag_input_focus       = (1u << 7),
    /** Window has a mouse cursor hovered over it. */
    hadal_window_flag_cursor_hovered    = (1u << 8),
    /** Capturing a mouse enables to track input outside of the window. */
    hadal_window_flag_mouse_captured    = (1u << 9),
    /** Window is modal. */
    hadal_window_flag_modal             = (1u << 10),
    /** Window is occluded by another window. */
    hadal_window_flag_occluded          = (1u << 11),
    /** Window should always be above other windows. */
    hadal_window_flag_always_on_top     = (1u << 12),
    /** Window will always minimize when leaving fullscreen. */
    hadal_window_flag_auto_minimize     = (1u << 13),
    /** Window is shell activated, available only on some platforms. */
    hadal_window_flag_shell_activated   = (1u << 14),
    /** Window has a transparent buffer. */
    hadal_window_flag_transparent       = (1u << 15),
    /** Window usable for Vulkan surface. */
    hadal_window_flag_vulkan            = (1u << 16),
    /** Window usable for Metal view. */
    hadal_window_flag_metal             = (1u << 17),
    /** Set on exit events from sources outside of the application.  */
    hadal_window_flag_should_close      = (1u << 18),
};

struct reznor_swapchain;
struct reznor_resource_header;

/** The public interface to a native window. A window implementation is expected 
 *  to include this header as its first struct member. */
struct hadal_window_header {
    struct hadal                   *hadal;
    struct str                      title;
    /** A swapchain may be attached so the window can directly control it's state (flags). */
    union {
        struct reznor_resource_header  *swapchain_header;
        struct reznor_swapchain        *swapchain;
    };
    /** Flags describe the current state of a window. */
    at_u32                          flags;
};

/** Information needed to create a native window. */
struct hadal_window_create_work {
    s32                             result;
    u32                             flags;
    struct hadal                   *hadal;
    /** Dimensions of the newly created window. */
    u32                             width, height; 
    /** The title of the newly created window. */
    struct str                      title;
    struct riven_memory             memory;
    /** Optional flags to setup a window state. Some flags may be ignored. */
    struct hadal_window           **out_window;
};

/** Creates a native window. */
typedef void (AMWCALL *PFN_hadal_window_create)(struct hadal_window_create_work *restrict work);
#define FN_HADAL_WINDOW_CREATE(encore) \
    extern void AMWCALL _hadal_##encore##_window_create(struct hadal_window_create_work *restrict work)

/** Destroys a native window. */
typedef void (AMWCALL *PFN_hadal_window_destroy)(struct hadal_window *restrict window);
#define FN_HADAL_WINDOW_DESTROY(encore) \
    extern void AMWCALL _hadal_##encore##_window_destroy(struct hadal_window *restrict window)

#define ARGS_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT \
    const struct hadal_window *window, \
    u32                       *out_width, \
    u32                       *out_height
/** Acquires the framebuffer extent/resolution, may be different from the window size. */
typedef void (AMWCALL *PFN_hadal_window_acquire_framebuffer_extent)(ARGS_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT);
#define FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(encore) \
    extern void AMWCALL _hadal_##encore##_window_acquire_framebuffer_extent(ARGS_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT)

/** Returns the window flags before visibility was changed. */
typedef u32 (AMWCALL *PFN_hadal_window_visibility)(struct hadal_window *window, b32 visible);
#define FN_HADAL_WINDOW_VISIBILITY(encore) \
    extern u32 AMWCALL _hadal_##encore##_window_visibility(struct hadal_window *window, b32 visible)

/** The header of a 'hadal_keyboard' device implemented by the backend. */
struct hadal_keyboard_header {
    struct hadal *hadal;
};

/** Key modifiers. */
enum hadal_keymod {
    hadal_keymod_shift     = 0x0001, 
    hadal_keymod_ctrl      = 0x0002, 
    hadal_keymod_alt       = 0x0004, 
    hadal_keymod_super     = 0x0008, 
    hadal_keymod_caps_lock = 0x0010,
    hadal_keymod_num_lock  = 0x0020,
    hadal_keymod_invalid   = 0,
};

/** A state of a button input. */
enum hadal_key {
    hadal_key_unknown = -1,
    hadal_key_release = 0,
    hadal_key_press   = 1,
    hadal_key_repeat  = 2,
};

/** The header of a 'hadal_mouse' device implemented by the backend. */
struct hadal_mouse_header {
    struct hadal *hadal;
};

/** An input source of the mouse wheel. */
enum hadal_mouse_wheel {
    hadal_mouse_wheel_normal,
    hadal_mouse_wheel_flipped,
};

/** A button of a mouse device. */
enum hadal_mouse_button {
    hadal_mouse_button_1       = 0x0001,
    hadal_mouse_button_2       = 0x0002,
    hadal_mouse_button_3       = 0x0004,
    hadal_mouse_button_4       = 0x0008,
    hadal_mouse_button_5       = 0x0010,
    hadal_mouse_button_6       = 0x0020,
    hadal_mouse_button_7       = 0x0040,
    hadal_mouse_button_8       = 0x0080,
    hadal_mouse_button_left    = hadal_mouse_button_1,
    hadal_mouse_button_right   = hadal_mouse_button_2,
    hadal_mouse_button_middle  = hadal_mouse_button_3,
    hadal_mouse_button_invalid = 0,
};
#define hadal_mouse_button_last hadal_mouse_button_8

/** A button of a mapped game controller. */
enum hadal_gamepad_button {
    hadal_gamepad_button_invalid      = 0,
    hadal_gamepad_button_a            = 0x0001,
    hadal_gamepad_button_b            = 0x0002,
    hadal_gamepad_button_x            = 0x0004,
    hadal_gamepad_button_y            = 0x0008,
    hadal_gamepad_button_left_bumper  = 0x0010,
    hadal_gamepad_button_right_bumper = 0x0020,
    hadal_gamepad_button_back         = 0x0040,
    hadal_gamepad_button_start        = 0x0080,
    hadal_gamepad_button_guide        = 0x0100,
    hadal_gamepad_button_left_thumb   = 0x0200,
    hadal_gamepad_button_right_thumb  = 0x0400,
    hadal_gamepad_button_dpad_up      = 0x0800,
    hadal_gamepad_button_dpad_right   = 0x1000,
    hadal_gamepad_button_dpad_down    = 0x2000,
    hadal_gamepad_button_dpad_left    = 0x4000,
    hadal_gamepad_button_cross        = hadal_gamepad_button_a,
    hadal_gamepad_button_circle       = hadal_gamepad_button_b,
    hadal_gamepad_button_square       = hadal_gamepad_button_x,
    hadal_gamepad_button_triangle     = hadal_gamepad_button_y,
};
#define hadal_gamepad_button_last hadal_gamepad_button_dpad_left

/** An axis of a mapped game controller. */
enum hadal_gamepad_axis {
    hadal_gamepad_axis_left_x         = 0x0001,
    hadal_gamepad_axis_left_y         = 0x0002,
    hadal_gamepad_axis_right_x        = 0x0004,
    hadal_gamepad_axis_right_y        = 0x0008,
    hadal_gamepad_axis_left_trigger   = 0x0010,
    hadal_gamepad_axis_right_trigger  = 0x0020,
};
#define hadal_gamepad_axis_last hadal_gamepad_axis_right_trigger

/** A hat input source of game controllers. */
enum hadal_gamepad_hat {
    hadal_gamepad_hat_centered    = 0,
    hadal_gamepad_hat_up          = 0x0001,
    hadal_gamepad_hat_right       = 0x0002,
    hadal_gamepad_hat_down        = 0x0004,
    hadal_gamepad_hat_left        = 0x0008,
    hadal_gamepad_hat_right_up    = (hadal_gamepad_hat_right | hadal_gamepad_hat_up),
    hadal_gamepad_hat_right_down  = (hadal_gamepad_hat_right | hadal_gamepad_hat_down),
    hadal_gamepad_hat_left_up     = (hadal_gamepad_hat_left  | hadal_gamepad_hat_up),
    hadal_gamepad_hat_left_down   = (hadal_gamepad_hat_left  | hadal_gamepad_hat_down),
};

/** A game controller mapping for joystick devices. */
struct hadal_gamepad_map {
    u8 type;
    u8 index;
    s8 axis_scale;
    s8 axis_offset;
};

/** The raw state of joystick input. */
struct hadal_gamepad_state {
    u8  buttons[15];
    f32 axes[6];
};

/** The joystick functions provide unlabeled axes, buttons and hats, with no indication of where they are located 
 *  on the device. Their order may also vary between platforms even when using the same device. */
struct hadal_gamepad {
    char                        name[128];
    char                        guid[33];
    struct hadal_gamepad_map    buttons[15];
    struct hadal_gamepad_map    axes[6];
};

/** The header for 'hadal_joystick' internal structure. */
struct hadal_joystick_header {
    struct hadal           *hadal;
    b32                     connected;
    f32                    *axes;
    u32                     axis_count;
    u8                     *buttons;
    u32                     button_count;
    u8                     *hats;
    u32                     hat_count;
    struct hadal_gamepad   *mapping;
};

/** A button of a pen device. */
enum hadal_pen_input {
    hadal_pen_input_down       = (1u << 0),  /**< Pen is pressed down. */
    hadal_pen_input_button_1   = (1u << 1),  /**< Button 1 is pressed. */
    hadal_pen_input_button_2   = (1u << 2),  /**< Button 2 is pressed. */
    hadal_pen_input_button_3   = (1u << 3),  /**< Button 3 is pressed. */
    hadal_pen_input_button_4   = (1u << 4),  /**< Button 4 is pressed. */
    hadal_pen_input_button_5   = (1u << 5),  /**< Button 5 is pressed. */
    hadal_pen_input_eraser_tip = (1u << 6),  /**< Eraser tip is used. */
};

/** An axis input source of a pen device. */
enum hadal_pen_axis {
    hadal_pen_axis_pressure = 0,          /**< Pen pressure. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_xtilt,                 /**< Pen horizontal tilt angle. Bidirectional: -90.0 to 90.0 (left-to-right). */
    hadal_pen_axis_ytilt,                 /**< Pen vertical tilt angle. Bidirectional: -90.0 to 90.0 (top-to-down). */
    hadal_pen_axis_distance,              /**< Pen distance to drawing surface. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_rotation,              /**< Pen barrel rotation. Bidirectional: -180.0 to 179.9 (clockwise, 0 is facing up, -180 is facing down). */
    hadal_pen_axis_slider,                /**< Pen finger wheel or slider. Unidirectional: 0.0 to 1.0. */
    hadal_pen_axis_tangential_pressure,   /**< Pressure from squeezing the pen ("barrel pressurehadal_"). */
    hadal_pen_axis_count,                 /**< Total known pen axis types. */
};

/** The header of a 'hadal_pen' device implemented by the backend. */
struct hadal_pen_header {
    struct hadal *hadal;
};

/** A type of a touch device. */
enum hadal_touch_type {
    hadal_touch_type_invalid = -1,
    hadal_touch_type_direct,            /**< Touch screen with window-relative coordinates. */
    hadal_touch_type_indirect_absolute, /**< Trackpad with absolute device coordinates. */
    hadal_touch_type_indirect_relative, /**< Trackpad with screen cursor-relative coordinates. */
};

/** The header of a 'hadal_touch' device implemented by the backend. */
struct hadal_touch_header {
    struct hadal *hadal;
};

/** Data about a single finger in a multitouch event. */
struct hadal_finger {
    u64 id;
    f32 x, y;
    f32 pressure;
};

#define ARGS_HADAL_EVENT_POLL \
    struct hadal *hadal, f64 *timeout
typedef void (AMWCALL *PFN_hadal_event_poll)(ARGS_HADAL_EVENT_POLL);
#define FN_HADAL_EVENT_POLL(encore) \
    void AMWCALL _hadal_##encore##_event_poll(ARGS_HADAL_EVENT_POLL)

#ifdef REZNOR_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, const char *))(void);

#define ARGS_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES \
    struct hadal                       *hadal,      \
    struct VkInstance_T                *instance,   \
    PFN_vkGetInstanceProcAddr           vkGetInstanceProcAddr
/** Writes instance procedures related to the window surface, specific to a display backend.
 *  Returns a non-zero value on errors or if the display does not support Vulkan. */
typedef s32 (AMWCALL *PFN_hadal_vulkan_write_instance_procedures)(ARGS_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES);
#define FN_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES(encore) \
    extern s32 AMWCALL _hadal_##encore##_vulkan_write_instance_procedures(ARGS_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES)

#define ARGS_HADAL_VULKAN_SURFACE_CREATE                \
    const struct hadal_window          *window,         \
    struct VkSurfaceKHR_T             **out_surface,    \
    const struct VkAllocationCallbacks *callbacks
/** Creates a Vulkan surface from a given window. The display backend must support 
 *  interfacing with Vulkan (then for example, an Emscripten/HTML5 backend can't).
 *  The caller must pass a valid Vulkan instance, a PFN_vkGetInstanceProcAddr and 
 *  a pointer to the surface handle, where the Vulkan surface will be created.
 *  An optional pointer to the allocation callbacks can be given too.
 *  Returns a non-zero value on errors or if the display does not support Vulkan. */
typedef s32 (AMWCALL *PFN_hadal_vulkan_surface_create)(ARGS_HADAL_VULKAN_SURFACE_CREATE);
#define FN_HADAL_VULKAN_SURFACE_CREATE(encore) \
    extern s32 AMWCALL _hadal_##encore##_vulkan_surface_create(ARGS_HADAL_VULKAN_SURFACE_CREATE)

#define ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT              \
    const struct hadal                 *hadal,              \
    struct VkPhysicalDevice_T          *physical_device,    \
    u32                                 queue_family
/** Checks whether the physical device supports presentation for a given display backend.
 *  If a display backend has no dedicated vkGetPhysicalDevice*PresentationSupport procedure,
 *  then a queue family's presentation support is checked via the Vulkan surface instead.
 *  Returns true if presentation is supported, otherwise (or on errors) returns false. */
typedef b32 (AMWCALL *PFN_hadal_vulkan_presentation_support)(ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT);
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(encore) \
    extern b32 AMWCALL _hadal_##encore##_vulkan_presentation_support(ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT)
#endif /* REZNOR_VULKAN */

/** Procedures to be provided by an implementation. The backend must implement the 'struct hadal' and 
 *  put the hadal_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the display backend and the public interface defined below. */
struct hadal_interface {
    struct riven_interface_header   header;

    struct hadal_output            *outputs[HADAL_MAX_OUTPUTS];
    u32                             output_count;

    s16                             keycodes[256];
    s16                             scancodes[hadal_keycode_last + 1];
    s8                              keynames[hadal_keycode_last + 1][5];

    s8                              keys[hadal_keycode_last + 1];
    s8                              mouse_buttons[hadal_mouse_button_last + 1];

    struct hadal_joystick          *joysticks[HADAL_MAX_JOYSTICKS];
    struct hadal_gamepad           *mappings;
    u32                             mapping_count;

    PFN_hadal_window_create                         window_create;
    PFN_hadal_window_destroy                        window_destroy;
    PFN_hadal_window_acquire_framebuffer_extent     window_acquire_framebuffer_extent;
    PFN_hadal_window_visibility                     window_visibility;
    PFN_hadal_event_poll                            event_poll;
#ifdef REZNOR_VULKAN
    PFN_hadal_vulkan_write_instance_procedures      vulkan_write_instance_procedures;
    PFN_hadal_vulkan_surface_create                 vulkan_surface_create;
    PFN_hadal_vulkan_presentation_support           vulkan_presentation_support;
#endif
};

#ifdef __cplusplus
}
#endif
