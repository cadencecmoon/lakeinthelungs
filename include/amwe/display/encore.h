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

/** Returns an array of native encores, with a predefined priority. */
LAKEAPI lake_nonnull(2) const PFN_riven_encore *LAKECALL 
hadal_native_encores(bool null_fallback, u32 *out_encore_count);

#define HADAL_INTERFACE_MONITOR_HEADER \
    /**< The encore that owns this output monitor. */ \
    struct hadal_encore *encore;

#define HADAL_INTERFACE_WINDOW_HEADER \
    /**< The encore that was used to create this native window. */ \
    struct hadal_encore *encore;

enum hadal_result {
    hadal_result_success = 0,
};

/* Connection to graphics APIs: */

#ifdef XAKU_VULKAN
enum xaku_result;

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

#define ARGS_HADAL_VULKAN_SURFACE_CREATE \
    const struct hadal_window          *window, \
    const struct VkAllocationCallbacks *callbacks, \
    struct VkSurfaceKHR_T             **out_surface
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_hadal_vulkan_surface_create)(ARGS_HADAL_VULKAN_SURFACE_CREATE);
#define FN_HADAL_VULKAN_SURFACE_CREATE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _hadal_##ENCORE##_vulkan_surface_create(ARGS_HADAL_VULKAN_SURFACE_CREATE)
#endif /* XAKU_VULKAN */

#ifdef __cplusplus
}
#endif /* __cplusplus */
