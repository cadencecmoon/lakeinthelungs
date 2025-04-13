#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* opaque handles, implemented by the backend */
struct hadal_pen;
struct hadal_touch;
struct hadal_mouse;
struct hadal_keyboard;
struct hadal_joystick;
struct hadal_window;
struct hadal_monitor;
struct hadal_encore;

#if defined(LAKE_PLATFORM_WINDOWS)
LAKEAPI FN_RIVEN_ENCORE(hadal, win32);
#elif defined(LAKE_PLATFORM_APPLE_MACOS)
LAKEAPI FN_RIVEN_ENCORE(hadal, cocoa);
#elif defined(LAKE_PLATFORM_APPLE_IOS)
LAKEAPI FN_RIVEN_ENCORE(hadal, uikit);
#elif defined(LAKE_PLATFORM_ANDROID)
LAKEAPI FN_RIVEN_ENCORE(hadal, android);
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
LAKEAPI FN_RIVEN_ENCORE(hadal, html5);
#endif /* LAKE_PLATFORM_WINDOWS */
#ifdef HADAL_WAYLAND
LAKEAPI FN_RIVEN_ENCORE(hadal, wayland);
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
LAKEAPI FN_RIVEN_ENCORE(hadal, xcb);
#endif /* HADAL_XCB */
#ifdef HADAL_DRMKMS
LAKEAPI FN_RIVEN_ENCORE(hadal, drmkms);
#endif /* HADAL_DRMKMS */

/* A headless display backend, used for testing. */
LAKEAPI FN_RIVEN_ENCORE(hadal, null);

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

struct hadal_window_header {
    struct hadal_encore    *encore;
    const char             *title;
    atomic_u32              flags;
};

/** Returns an array of native encores, with a predefined priority. */
LAKEAPI lake_nonnull(2) const PFN_riven_encore *LAKECALL 
hadal_native_encores(bool null_fallback, u32 *out_encore_count);

typedef struct hadal_window *(LAKECALL *PFN_hadal_window_create)(struct hadal_encore *ENCORE);
#define FN_HADAL_WINDOW_CREATE(ENCORE) \
    struct hadal_window *LAKECALL _hadal_##ENCORE##_window_create(struct hadal_encore *ENCORE)

typedef void (LAKECALL *PFN_hadal_window_destroy)(struct hadal_window *window);
#define FN_HADAL_WINDOW_DESTROY(ENCORE) \
    void LAKECALL _hadal_##ENCORE##_window_destroy(struct hadal_window *window)

typedef u32 (LAKECALL *PFN_hadal_window_visibility)(struct hadal_window *window, bool visible);
#define FN_HADAL_WINDOW_VISIBILITY(ENCORE) \
    u32 LAKECALL _hadal_##ENCORE##_window_visibility(struct hadal_window *window, bool visible)

#include <amwe/input/keycodes.h>
#include <amwe/input/scancodes.h>

#ifdef XAKU_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, const char *))(void);

#define ARGS_HADAL_VULKAN_WRITE_INSTANCE(ENCORE) \
    struct hadal_encore                *ENCORE, \
    struct VkInstance_T                *instance, \
    PFN_vkGetInstanceProcAddr           vkGetInstanceProcAddr
typedef bool (LAKECALL *PFN_hadal_vulkan_write_instance)(ARGS_HADAL_VULKAN_WRITE_INSTANCE(encore));
#define FN_HADAL_VULKAN_WRITE_INSTANCE(ENCORE) \
    bool LAKECALL _hadal_##ENCORE##_vulkan_write_instance(ARGS_HADAL_VULKAN_WRITE_INSTANCE(ENCORE))

#define ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT(ENCORE) \
    const struct hadal_encore          *ENCORE, \
    struct VkPhysicalDevice_T          *physical_device, \
    u32                                 queue_family_index
typedef bool (LAKECALL *PFN_hadal_vulkan_presentation_support)(ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT(encore));
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(ENCORE) \
    bool LAKECALL _hadal_##ENCORE##_vulkan_presentation_support(ARGS_HADAL_VULKAN_PRESENTATION_SUPPORT(ENCORE))

#define ARGS_HADAL_VULKAN_SURFACE_CREATE(ENCORE) \
    const struct hadal_window          *window, \
    const struct VkAllocationCallbacks *callbacks, \
    struct VkSurfaceKHR_T             **out_surface
typedef s32 (LAKECALL *PFN_hadal_vulkan_surface_create)(ARGS_HADAL_VULKAN_SURFACE_CREATE(encore));
#define FN_HADAL_VULKAN_SURFACE_CREATE(ENCORE) \
    s32 LAKECALL _hadal_##ENCORE##_vulkan_surface_create(ARGS_HADAL_VULKAN_SURFACE_CREATE(ENCORE))
#endif /* XAKU_VULKAN */

/** A public interface of the display engine, implemented by a backend 'hadal_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct hadal_interface {
    struct riven_interface_header           header;

    s16                                     keycodes[256];
    s16                                     scancodes[hadal_keycode_last + 1];
    s8                                      keynames[hadal_keycode_last + 1][5];
    s8                                      keys[hadal_keycode_last + 1];

    PFN_hadal_window_create                 window_create;
    PFN_hadal_window_destroy                window_destroy;
    PFN_hadal_window_visibility             window_visibility;
#ifdef XAKU_VULKAN
    PFN_hadal_vulkan_write_instance         vulkan_write_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_surface_create         vulkan_surface_create;
#endif /* XAKU_VULKAN */
};

/** A view into the backend. */
union hadal_encore_view {
    struct riven_interface_header          *header;
    struct hadal_interface                 *interface;
    struct hadal_encore                    *encore;
};

/** A view into a window. */
union hadal_window_view {
    struct hadal_window_header             *header;
    struct hadal_window                    *data; 
};

/** Implies different strategies for the display engine. */
enum hadal_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    hadal_strategy_auto = 0,
    /** Only the main window may be present, no modals, popups or multiple windows allowed. */
    hadal_strategy_optimal_one_viewport = 0,
    /** One backend, but it is allowed and expected to run more than one system window */
    hadal_strategy_optimal_multi_viewport,
    /** Allows to run multiple backends, may be used for testing. */
    hadal_strategy_debug,
};

#ifndef HADAL_MAX_ENCORE_COUNT
    #define HADAL_MAX_ENCORE_COUNT 1
#endif /* HADAL_MAX_ENCORE_COUNT */

/** An engine structure for display communication and windowing. */
struct hadal {
    enum hadal_strategy                     strategy;
    atomic_u32                              flags;
    union hadal_encore_view                 backends[HADAL_MAX_ENCORE_COUNT];

    union hadal_window_view                *windows;
    u32                                     window_count;
    u32                                     window_capacity;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
