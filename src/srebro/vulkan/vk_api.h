#ifndef _AMW_SREBRO_VULKAN_API_H
#define _AMW_SREBRO_VULKAN_API_H

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    #include <vulkan/vk_platform.h>
    #include <vulkan/vulkan_core.h>

    /* to avoid including <windows.h> */
    typedef unsigned long                DWORD;
    typedef const wchar_t               *LPCWSTR;
    typedef void                        *HANDLE;
    typedef struct HINSTANCE__          *HINSTANCE;
    typedef struct HWND__               *HWND;
    typedef struct HMONITOR__           *HMONITOR;
    typedef struct _SECURITY_ATTRIBUTES  _SECURITY_ATTRIBUTES;
    
    #include <vulkan/vulkan_win32.h>
    #if defined(VK_ENABLE_BETA_EXTENSIONS)
        #include <vulkan/vulkan_beta.h>
    #endif
#else
    #include <vulkan/vulkan.h>
#endif

#include <lake/defines.h>

/** Collects used Vulkan extensions and core features for backwards compatibility as bit flags. */
enum vulkan_extensions {
    /* instance extensions */
    vulkan_extension_surface_bit                        = (1ull << 0),  /**< VK_KHR_surface */
    vulkan_extension_win32_surface_bit                  = (1ull << 1),  /**< VK_KHR_win32_surface */
    vulkan_extension_metal_surface_bit                  = (1ull << 2),  /**< VK_EXT_metal_surface */
    vulkan_extension_android_surface_bit                = (1ull << 3),  /**< VK_KHR_android_surface */
    vulkan_extension_wayland_surface_bit                = (1ull << 4),  /**< VK_KHR_wayland_surface */
    vulkan_extension_xcb_surface_bit                    = (1ull << 5),  /**< VK_KHR_xcb_surface */
    vulkan_extension_headless_surface_bit               = (1ull << 6),  /**< VK_KHR_headless_surface */
    vulkan_extension_display_bit                        = (1ull << 7),  /**< VK_KHR_display */
    vulkan_extension_acquire_drm_display_bit            = (1ull << 8),  /**< VK_EXT_acquire_drm_display */

    /* debug builds only */
    vulkan_extension_debug_utils_bit                    = (1ull << 9),  /**< VK_EXT_debug_utils */
    vulkan_extension_layer_validation_bit               = (1ull << 10), /**< VK_LAYER_KHRONOS_validation */

    /* device extensions */
    vulkan_extension_swapchain_bit                      = (1ull << 11), /**< VK_KHR_swapchain */
    vulkan_extension_display_swapchain_bit              = (1ull << 12), /**< VK_KHR_display_swapchain */
    vulkan_extension_device_fault_bit                   = (1ull << 13), /**< VK_EXT_device_fault */
    vulkan_extension_memory_budget_bit                  = (1ull << 14), /**< VK_EXT_memory_budget */
    vulkan_extension_memory_priority_bit                = (1ull << 15), /**< VK_EXT_memory_priority */
    vulkan_extension_amd_buffer_marker_bit              = (1ull << 16), /**< VK_AMD_buffer_marker */
    vulkan_extension_amd_shader_info_bit                = (1ull << 17), /**< VK_AMD_shader_info */

    /* hardware raytracing */
    vulkan_extension_deferred_host_operations_bit       = (1ull << 20), /**< VK_KHR_deferred_host_operations */
    vulkan_extension_acceleration_structure_bit         = (1ull << 21), /**< VK_KHR_acceleration_structure */
    vulkan_extension_ray_query_bit                      = (1ull << 22), /**< VK_KHR_ray_query */

    /* core 1.4, for backwards compatibility */
    vulkan_extension_dynamic_rendering_local_read_bit   = (1ull << 32), /**< VK_KHR_dynamic_rendering_local_read */

    /* core 1.3, for backwards compatibility */
    vulkan_extension_dynamic_rendering_bit              = (1ull << 33), /**< VK_KHR_dynamic_rendering */
};
#define vulkan_extension_mask_linux_drm \
    (vulkan_extension_display_bit | \
     vulkan_extension_acquire_drm_display_bit | \
     vulkan_extension_display_swapchain_bit)
#define vulkan_extension_mask_amd \
    (vulkan_extension_amd_buffer_marker_bit | \
     vulkan_extension_amd_shader_info_bit)
#define vulkan_extension_mask_raytracing \
    (vulkan_extension_deferred_host_operations_bit | \
     vulkan_extension_acceleration_structure_bit | \
     vulkan_extension_ray_query_bit)
#define vulkan_extension_mask_1_4_compatibility \
    (vulkan_extension_dynamic_rendering_local_read_bit)
#define vulkan_extension_mask_1_3_compatibility \
    (vulkan_extension_dynamic_rendering_bit)

#endif /* _AMW_SREBRO_VULKAN_API_H */
