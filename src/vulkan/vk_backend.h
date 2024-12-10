/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_VULKAN_BACKEND_H
#define _AMW_VULKAN_BACKEND_H

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

#include <amw/common/defines.h>

/** Collects used Vulkan extensions and core features for backwards compatibility as bit flags. */
enum vulkan_extensions {
    /* instance extensions */
    vulkan_extension_surface_bit                        = (1u << 0),  /**< VK_KHR_surface */
    vulkan_extension_win32_surface_bit                  = (1u << 1),  /**< VK_KHR_win32_surface */
    vulkan_extension_metal_surface_bit                  = (1u << 2),  /**< VK_EXT_metal_surface */
    vulkan_extension_android_surface_bit                = (1u << 3),  /**< VK_KHR_android_surface */
    vulkan_extension_wayland_surface_bit                = (1u << 4),  /**< VK_KHR_wayland_surface */
    vulkan_extension_xcb_surface_bit                    = (1u << 5),  /**< VK_KHR_xcb_surface */
    vulkan_extension_headless_surface_bit               = (1u << 6),  /**< VK_KHR_headless_surface */
    vulkan_extension_display_bit                        = (1u << 7),  /**< VK_KHR_display */
    vulkan_extension_acquire_drm_display_bit            = (1u << 8),  /**< VK_EXT_acquire_drm_display */

    /* debug builds only */
    vulkan_extension_debug_utils_bit                    = (1u << 9),  /**< VK_EXT_debug_utils */
    vulkan_extension_layer_validation_bit               = (1u << 10), /**< VK_LAYER_KHRONOS_validation */

    /* device extensions */
    vulkan_extension_swapchain_bit                      = (1u << 11), /**< VK_KHR_swapchain */
    vulkan_extension_display_swapchain_bit              = (1u << 12), /**< VK_KHR_display_swapchain */
    vulkan_extension_device_fault_bit                   = (1u << 13), /**< VK_EXT_device_fault */
    vulkan_extension_memory_budget_bit                  = (1u << 14), /**< VK_EXT_memory_budget */
    vulkan_extension_memory_priority_bit                = (1u << 15), /**< VK_EXT_memory_priority */
    vulkan_extension_amd_buffer_marker_bit              = (1u << 16), /**< VK_AMD_buffer_marker */
    vulkan_extension_amd_shader_info_bit                = (1u << 17), /**< VK_AMD_shader_info */

    /* hardware raytracing */
    vulkan_extension_deferred_host_operations_bit       = (1u << 20), /**< VK_KHR_deferred_host_operations */
    vulkan_extension_acceleration_structure_bit         = (1u << 21), /**< VK_KHR_acceleration_structure */
    vulkan_extension_ray_query_bit                      = (1u << 22), /**< VK_KHR_ray_query */

    /* core 1.4 backwards compatibility */
    vulkan_extension_dynamic_rendering_local_read_bit   = (1u << 32), /**< VK_KHR_dynamic_rendering_local_read */

    /* core 1.3 backwards compatibility */
    vulkan_extension_dynamic_rendering_bit              = (1u << 33), /**< VK_KHR_dynamic_rendering */
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

/* TODO delete pointers that happen to be unnecessary, later */
struct vulkan_api {
    VkPhysicalDevice                                        physical_device;
    VkDevice                                                logical_device;

    u64                                                     extensions;

    void                                                   *module;
    PFN_vkGetInstanceProcAddr                               vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr                                 vkGetDeviceProcAddr;

    /* global functions */
    PFN_vkCreateInstance                                    vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties              vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties                  vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceVersion                          vkEnumerateInstanceVersion;

    /* instance functions */
    PFN_vkCreateDevice                                      vkCreateDevice;
    PFN_vkDestroyInstance                                   vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties                vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices                          vkEnumeratePhysicalDevices;
    PFN_vkEnumeratePhysicalDeviceGroups                     vkEnumeratePhysicalDeviceGroups;
    PFN_vkGetPhysicalDeviceFeatures                         vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFeatures2                        vkGetPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceFormatProperties                 vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceFormatProperties2                vkGetPhysicalDeviceFormatProperties2;
    PFN_vkGetPhysicalDeviceImageFormatProperties            vkGetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties2           vkGetPhysicalDeviceImageFormatProperties2;
    PFN_vkGetPhysicalDeviceMemoryProperties                 vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties2                vkGetPhysicalDeviceMemoryProperties2;
    PFN_vkGetPhysicalDeviceProperties                       vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceProperties2                      vkGetPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties            vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2           vkGetPhysicalDeviceQueueFamilyProperties2;

    /* device version 1.0 */
    /* device version 1.1 */
    /* device version 1.2 */
    /* device version 1.3 */
    /* device version 1.4 */

#if defined(VK_KHR_surface)
    PFN_vkDestroySurfaceKHR                                 vkDestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR                vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR           vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR                vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR           vkGetPhysicalDeviceSurfacePresentModesKHR;
#endif /* VK_KHR_surface */

#if defined(VK_KHR_win32_surface)
    PFN_vkCreateWin32SurfaceKHR                             vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR      vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* VK_KHR_win32_surface */

#if defined(VK_KHR_metal_surface)
    PFN_vkCreateMetalSurfaceEXT                             vkCreateMetalSurfaceEXT;
#endif /* VK_KHR_metal_surface */

#if defined(VK_KHR_wayland_surface) && defined(AMW_NATIVE_WAYLAND)
    PFN_vkCreateWaylandSurfaceKHR                           vkCreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR    vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* VK_KHR_wayland_surface */

#if defined(VK_KHR_xcb_surface) && defined(AMW_NATIVE_XCB)
    PFN_vkCreateXcbSurfaceKHR                               vkCreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR        vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* VK_KHR_xcb_surface */

#if defined(VK_KHR_android_surface)
    PFN_vkCreateAndroidSurfaceKHR                           vkCreateAndroidSurfaceKHR;
#endif /* VK_KHR_android_surface */

#if defined(VK_EXT_headless_surface)
    PFN_vkCreateHeadlessSurfaceEXT                          vkCreateHeadlessSurfaceEXT;
#endif /* VK_EXT_headless_surface */

#if defined(VK_KHR_display) && defined(AMW_NATIVE_KMS)
    PFN_vkCreateDisplayModeKHR                              vkCreateDisplayModeKHR;
    PFN_vkCreateDisplayPlaneSurfaceKHR                      vkCreateDisplayPlaneSurfaceKHR;
    PFN_vkGetDisplayModePropertiesKHR                       vkGetDisplayModePropertiesKHR;
    PFN_vkGetDisplayPlaneCapabilitiesKHR                    vkGetDisplayPlaneCapabilitiesKHR;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR               vkGetDisplayPlaneSupportedDisplaysKHR;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR        vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR             vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* VK_KHR_display */

#if defined(VK_EXT_acquire_drm_display) && defined(AMW_NATIVE_KMS)
    PFN_vkAcquireDrmDisplayEXT                              vkAcquireDrmDisplayEXT;
    PFN_vkGetDrmDisplayEXT                                  vkGetDrmDisplayEXT;
#endif /* VK_EXT_acquire_drm_display */

#if defined(VK_EXT_debug_utils)
    PFN_vkSetDebugUtilsObjectNameEXT                        vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT                         vkSetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT                      vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT                        vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT                     vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT                        vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT                          vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT                       vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT                      vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                     vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT                        vkSubmitDebugUtilsMessageEXT;
#endif /* VK_EXT_debug_utils */

#if defined(VK_KHR_swapchain)
    PFN_vkAcquireNextImageKHR                               vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                              vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                               vkDestroySwapchainKHR;
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR              vkGetDeviceGroupPresentCapabilitiesKHR;
    PFN_vkGetDeviceGroupSurfacePresentModesKHR              vkGetDeviceGroupSurfacePresentModesKHR;
    PFN_vkGetSwapchainImagesKHR                             vkGetSwapchainImagesKHR;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR             vkGetPhysicalDevicePresentRectanglesKHR;
    PFN_vkQueuePresentKHR                                   vkQueuePresentKHR;
#endif /* VK_KHR_swapchain */

#if defined(VK_KHR_display_swapchain) && defined(AMW_NATIVE_KMS)
    PFN_vkCreateSharedSwapchainsKHR                         PFN_vkCreateSharedSwapchainsKHR;
#endif /* VK_KHR_display_swapchain */

#if defined(VK_EXT_device_fault)
    PFN_vkGetDeviceFaultInfoEXT                             vkGetDeviceFaultInfoEXT;
#endif /* VK_EXT_device_fault */

#if defined(VK_AMD_buffer_marker)
    PFN_vkCmdWriteBufferMarkerAMD                           vkCmdWriteBufferMarkerAMD;
#endif /* VK_AMD_buffer_marker */

#if defined(VK_AMD_shader_info)
    PFN_vkGetShaderInfoAMD                                  vkGetShaderInfoAMD;
#endif /* VK_AMD_shader_info */

#if defined(VK_KHR_deferred_host_operations)
    PFN_vkCreateDeferredOperationKHR                        vkCreateDeferredOperationKHR;
    PFN_vkDeferredOperationJoinKHR                          vkDeferredOperationJoinKHR;
    PFN_vkDestroyDeferredOperationKHR                       vkDestroyDeferredOperationKHR;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR             vkGetDeferredOperationMaxConcurrencyKHR;
    PFN_vkGetDeferredOperationResultKHR                     vkGetDeferredOperationResultKHR;
#endif /* VK_KHR_deferred_host_operations */

#if defined(VK_KHR_acceleration_structure)
    PFN_vkBuildAccelerationStructuresKHR                    vkBuildAccelerationStructuresKHR;
    PFN_vkCmdBuildAccelerationStructuresIndirectKHR         vkCmdBuildAccelerationStructuresIndirectKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR                 vkCmdBuildAccelerationStructuresKHR;
    PFN_vkCmdCopyAccelerationStructureKHR                   vkCmdCopyAccelerationStructureKHR;
    PFN_vkCmdCopyAccelerationStructureToMemoryKHR           vkCmdCopyAccelerationStructureToMemoryKHR;
    PFN_vkCmdCopyMemoryToAccelerationStructureKHR           vkCmdCopyMemoryToAccelerationStructureKHR;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR       vkCmdWriteAccelerationStructuresPropertiesKHR;
    PFN_vkCopyAccelerationStructureKHR                      vkCopyAccelerationStructureKHR;
    PFN_vkCopyAccelerationStructureToMemoryKHR              vkCopyAccelerationStructureToMemoryKHR;
    PFN_vkCopyMemoryToAccelerationStructureKHR              vkCopyMemoryToAccelerationStructureKHR;
    PFN_vkCreateAccelerationStructureKHR                    vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR                   vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR             vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR          vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkGetDeviceAccelerationStructureCompatibilityKHR    vkGetDeviceAccelerationStructureCompatibilityKHR;
    PFN_vkWriteAccelerationStructuresPropertiesKHR          vkWriteAccelerationStructuresPropertiesKHR;
#endif /* VK_KHR_acceleration_structure */
};

extern bool vulkan_open_driver(struct vulkan_api *vulkan);
extern void vulkan_close_driver(struct vulkan_api *vulkan);

extern bool vulkan_has_extension(const VkExtensionProperties *ext, u32 count, const char *name);
extern i32 vulkan_load_instance_pointers(struct vulkan_api *vulkan, VkInstance instance);
extern i32 vulkan_load_device_pointers(struct vulkan_api *vulkan, VkDevice device);

#endif /* _AMW_VULKAN_BACKEND_H */
