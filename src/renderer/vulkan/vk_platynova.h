#ifndef _PLATYNOVA_VULKAN_H
#define _PLATYNOVA_VULKAN_H

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

#include <lake/platynova.h>

/** Collects Vulkan extensions in use as bit flags for checking their availability.
  * We use different uint32_t variables to represent extensions per instance or per device. */
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
    vulkan_extension_direct_mode_display_bit            = (1u << 9),  /**< VK_EXT_direct_mode_display */
    /* instance extensions, debug builds only */
    vulkan_extension_debug_utils_bit                    = (1u << 10), /**< VK_EXT_debug_utils */
    vulkan_extension_layer_validation_bit               = (1u << 11), /**< VK_LAYER_KHRONOS_validation */

    /* device extensions */
    vulkan_extension_swapchain_bit                      = (1u << 0),  /**< VK_KHR_swapchain */
    vulkan_extension_display_swapchain_bit              = (1u << 1),  /**< VK_KHR_display_swapchain */
    vulkan_extension_device_fault_bit                   = (1u << 2),  /**< VK_EXT_device_fault */
    vulkan_extension_memory_budget_bit                  = (1u << 3),  /**< VK_EXT_memory_budget */
    vulkan_extension_memory_priority_bit                = (1u << 4),  /**< VK_EXT_memory_priority */
    /* device extensions, vendor AMD */
    vulkan_extension_amd_shader_info_bit                = (1u << 5),  /**< VK_AMD_shader_info */
    /* device extensions, raytracing */
    vulkan_extension_deferred_host_operations_bit       = (1u << 6), /**< VK_KHR_deferred_host_operations */
    vulkan_extension_acceleration_structure_bit         = (1u << 7), /**< VK_KHR_acceleration_structure */
    vulkan_extension_ray_query_bit                      = (1u << 8), /**< VK_KHR_ray_query */
    /* core 1.4, for backwards compatibility */
    vulkan_extension_dynamic_rendering_local_read_bit   = (1u << 20), /**< VK_KHR_dynamic_rendering_local_read */
    /* core 1.3, for backwards compatibility */
    vulkan_extension_dynamic_rendering_bit              = (1u << 21), /**< VK_KHR_dynamic_rendering */
    /* core 1.2, for backwards compatibility */
};
#define vulkan_extension_mask_raytracing \
    (vulkan_extension_deferred_host_operations_bit | \
     vulkan_extension_acceleration_structure_bit | \
     vulkan_extension_ray_query_bit)

/** Collects vulkan instance core 1.0 and 1.1 procedures, and optional instance extensions. */
struct vulkan_instance_api {
    void                                                   *module;
    PFN_vkGetInstanceProcAddr                               vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr                                 vkGetDeviceProcAddr;

    /* global procedures */
    PFN_vkCreateInstance                                    vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties              vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties                  vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceVersion                          vkEnumerateInstanceVersion;

    /* instance procedures */
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
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties      vkGetPhysicalDeviceSparseImageFormatProperties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2     vkGetPhysicalDeviceSparseImageFormatProperties2;
    PFN_vkGetPhysicalDeviceToolProperties                   vkGetPhysicalDeviceToolProperties;
#if defined(VK_KHR_surface)
    PFN_vkDestroySurfaceKHR                                 vkDestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR                vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR           vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR                vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR           vkGetPhysicalDeviceSurfacePresentModesKHR;
#endif /* VK_KHR_surface */
#if defined(VK_KHR_win32_surface) && defined(AMW_PLATFORM_WINDOWS)
    PFN_vkCreateWin32SurfaceKHR                             vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR      vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface) && defined(AMW_PLATFORM_APPLE)
    PFN_vkCreateMetalSurfaceEXT                             vkCreateMetalSurfaceEXT;
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface) && defined(AMW_NATIVE_WAYLAND)
    PFN_vkCreateWaylandSurfaceKHR                           vkCreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR    vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface) && defined(AMW_NATIVE_XCB)
    PFN_vkCreateXcbSurfaceKHR                               vkCreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR        vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface) && defined(AMW_PLATFORM_ANDROID)
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
#if defined(VK_EXT_direct_mode_display) && defined(AMW_NATIVE_KMS)
    PFN_vkReleaseDisplayEXT                                 vkReleaseDisplayEXT;
#endif /* VK_EXT_direct_mode_display */
#if defined(VK_EXT_acquire_drm_display) && defined(AMW_NATIVE_KMS)
    PFN_vkAcquireDrmDisplayEXT                              vkAcquireDrmDisplayEXT;
    PFN_vkGetDrmDisplayEXT                                  vkGetDrmDisplayEXT;
#endif /* VK_EXT_acquire_drm_display */
#if defined(VK_EXT_debug_utils) && !defined(AMW_NDEBUG)
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
};

/** Collects Vulkan device procedures and extensions. A minimal Vulkan target is version 1.2 with 
 *  dynamic_rendering and dynamic_rendering_local_read extensions. */
struct vulkan_device_api {
    /* core 1.0 */
	PFN_vkAllocateCommandBuffers                            vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets                            vkAllocateDescriptorSets;
	PFN_vkAllocateMemory                                    vkAllocateMemory;
	PFN_vkBeginCommandBuffer                                vkBeginCommandBuffer;
	PFN_vkBindBufferMemory                                  vkBindBufferMemory;
	PFN_vkBindImageMemory                                   vkBindImageMemory;
	PFN_vkCmdBeginQuery                                     vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass                                vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets                             vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer                                vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline                                   vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers                              vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage                                      vkCmdBlitImage;
	PFN_vkCmdClearAttachments                               vkCmdClearAttachments;
	PFN_vkCmdClearColorImage                                vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage                         vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer                                     vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage                              vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage                                      vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer                              vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults                           vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch                                       vkCmdDispatch;
	PFN_vkCmdDispatchIndirect                               vkCmdDispatchIndirect;
	PFN_vkCmdDraw                                           vkCmdDraw;
	PFN_vkCmdDrawIndexed                                    vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect                            vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect                                   vkCmdDrawIndirect;
	PFN_vkCmdEndQuery                                       vkCmdEndQuery;
	PFN_vkCmdEndRenderPass                                  vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands                                vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer                                     vkCmdFillBuffer;
	PFN_vkCmdNextSubpass                                    vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier                                vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants                                  vkCmdPushConstants;
	PFN_vkCmdResetEvent                                     vkCmdResetEvent;
	PFN_vkCmdResetQueryPool                                 vkCmdResetQueryPool;
	PFN_vkCmdResolveImage                                   vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants                              vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias                                   vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds                                 vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent                                       vkCmdSetEvent;
	PFN_vkCmdSetLineWidth                                   vkCmdSetLineWidth;
	PFN_vkCmdSetScissor                                     vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask                          vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference                            vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask                            vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport                                    vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer                                   vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents                                     vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp                                 vkCmdWriteTimestamp;
	PFN_vkCreateBuffer                                      vkCreateBuffer;
	PFN_vkCreateBufferView                                  vkCreateBufferView;
	PFN_vkCreateCommandPool                                 vkCreateCommandPool;
	PFN_vkCreateComputePipelines                            vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool                              vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout                         vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent                                       vkCreateEvent;
	PFN_vkCreateFence                                       vkCreateFence;
	PFN_vkCreateFramebuffer                                 vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines                           vkCreateGraphicsPipelines;
	PFN_vkCreateImage                                       vkCreateImage;
	PFN_vkCreateImageView                                   vkCreateImageView;
	PFN_vkCreatePipelineCache                               vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout                              vkCreatePipelineLayout;
	PFN_vkCreateQueryPool                                   vkCreateQueryPool;
	PFN_vkCreateRenderPass                                  vkCreateRenderPass;
	PFN_vkCreateSampler                                     vkCreateSampler;
	PFN_vkCreateSemaphore                                   vkCreateSemaphore;
	PFN_vkCreateShaderModule                                vkCreateShaderModule;
	PFN_vkDestroyBuffer                                     vkDestroyBuffer;
	PFN_vkDestroyBufferView                                 vkDestroyBufferView;
	PFN_vkDestroyCommandPool                                vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool                             vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout                        vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice                                     vkDestroyDevice;
	PFN_vkDestroyEvent                                      vkDestroyEvent;
	PFN_vkDestroyFence                                      vkDestroyFence;
	PFN_vkDestroyFramebuffer                                vkDestroyFramebuffer;
	PFN_vkDestroyImage                                      vkDestroyImage;
	PFN_vkDestroyImageView                                  vkDestroyImageView;
	PFN_vkDestroyPipeline                                   vkDestroyPipeline;
	PFN_vkDestroyPipelineCache                              vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout                             vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool                                  vkDestroyQueryPool;
	PFN_vkDestroyRenderPass                                 vkDestroyRenderPass;
	PFN_vkDestroySampler                                    vkDestroySampler;
	PFN_vkDestroySemaphore                                  vkDestroySemaphore;
	PFN_vkDestroyShaderModule                               vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle                                    vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer                                  vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges                           vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers                                vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets                                vkFreeDescriptorSets;
	PFN_vkFreeMemory                                        vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements                       vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment                         vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue                                    vkGetDeviceQueue;
	PFN_vkGetEventStatus                                    vkGetEventStatus;
	PFN_vkGetFenceStatus                                    vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements                        vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements                  vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout                         vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData                              vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults                               vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity                          vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges                      vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory                                         vkMapMemory;
	PFN_vkMergePipelineCaches                               vkMergePipelineCaches;
	PFN_vkQueueBindSparse                                   vkQueueBindSparse;
	PFN_vkQueueSubmit                                       vkQueueSubmit;
	PFN_vkQueueWaitIdle                                     vkQueueWaitIdle;
	PFN_vkResetCommandBuffer                                vkResetCommandBuffer;
	PFN_vkResetCommandPool                                  vkResetCommandPool;
	PFN_vkResetDescriptorPool                               vkResetDescriptorPool;
	PFN_vkResetEvent                                        vkResetEvent;
	PFN_vkResetFences                                       vkResetFences;
	PFN_vkSetEvent                                          vkSetEvent;
	PFN_vkUnmapMemory                                       vkUnmapMemory;
	PFN_vkUpdateDescriptorSets                              vkUpdateDescriptorSets;
	PFN_vkWaitForFences                                     vkWaitForFences;

    /* core 1.1 */
    PFN_vkBindBufferMemory2                                 vkBindBufferMemory2;
    PFN_vkBindImageMemory2                                  vkBindImageMemory2;
    PFN_vkCmdDispatchBase                                   vkCmdDispatchBase;
    PFN_vkCmdSetDeviceMask                                  vkCmdSetDeviceMask;
    PFN_vkCreateDescriptorUpdateTemplate                    vkCreateDescriptorUpdateTemplate;
    PFN_vkCreateSamplerYcbcrConversion                      vkCreateSamplerYcbcrConversion;
    PFN_vkDestroyDescriptorUpdateTemplate                   vkDestroyDescriptorUpdateTemplate;
    PFN_vkDestroySamplerYcbcrConversion                     vkDestroySamplerYcbcrConversion;
    PFN_vkGetBufferMemoryRequirements2                      vkGetBufferMemoryRequirements2;
    PFN_vkGetDescriptorSetLayoutSupport                     vkGetDescriptorSetLayoutSupport;
    PFN_vkGetDeviceGroupPeerMemoryFeatures                  vkGetDeviceGroupPeerMemoryFeatures;
    PFN_vkGetDeviceQueue2                                   vkGetDeviceQueue2;
    PFN_vkGetImageMemoryRequirements2                       vkGetImageMemoryRequirements2;
    PFN_vkGetImageSparseMemoryRequirements2                 vkGetImageSparseMemoryRequirements2;
    PFN_vkTrimCommandPool                                   vkTrimCommandPool;
    PFN_vkUpdateDescriptorSetWithTemplate                   vkUpdateDescriptorSetWithTemplate;

    /* core 1.2 */
    PFN_vkCmdDrawIndexedIndirectCount                       vkCmdDrawIndexedIndirectCount;
    PFN_vkCmdDrawIndirectCount                              vkCmdDrawIndirectCount;
    PFN_vkGetBufferDeviceAddress                            vkGetBufferDeviceAddress;
    PFN_vkGetBufferOpaqueCaptureAddress                     vkGetBufferOpaqueCaptureAddress;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress               vkGetDeviceMemoryOpaqueCaptureAddress;
    PFN_vkGetSemaphoreCounterValue                          vkGetSemaphoreCounterValue;
    PFN_vkResetQueryPool                                    vkResetQueryPool;
    PFN_vkSignalSemaphore                                   vkSignalSemaphore;
    PFN_vkWaitSemaphores                                    vkWaitSemaphores;

#if defined(VK_VERSION_1_3)
    PFN_vkCmdBeginRendering                                 vkCmdBeginRendering;
    PFN_vkCmdEndRendering                                   vkCmdEndRendering;
#else
    #if defined(VK_KHR_dynamic_rendering)
        PFN_vkCmdBeginRenderingKHR                          vkCmdBeginRendering;
        PFN_vkCmdEndRenderingKHR                            vkCmdEndRendering;
    #endif
#endif /* 1.3 */

#if defined(VK_VERSION_1_4)
	PFN_vkCmdSetRenderingAttachmentLocations                vkCmdSetRenderingAttachmentLocations;
	PFN_vkCmdSetRenderingInputAttachmentIndices             vkCmdSetRenderingInputAttachmentIndices;
#else
    #if defined(VK_KHR_dynamic_rendering_local_read)
	    PFN_vkCmdSetRenderingAttachmentLocationsKHR         vkCmdSetRenderingAttachmentLocations;
	    PFN_vkCmdSetRenderingInputAttachmentIndicesKHR      vkCmdSetRenderingInputAttachmentIndices;
    #endif
#endif /* 1.4 */

#if defined(VK_KHR_swapchain)
    PFN_vkAcquireNextImageKHR                               vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                              vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                               vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                             vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                   vkQueuePresentKHR;
#endif /* VK_KHR_swapchain */
#if defined(VK_KHR_display_swapchain) && defined(AMW_NATIVE_KMS)
    PFN_vkCreateSharedSwapchainsKHR                         vkCreateSharedSwapchainsKHR;
#endif /* VK_KHR_display_swapchain */
#if defined(VK_EXT_device_fault)
    PFN_vkGetDeviceFaultInfoEXT                             vkGetDeviceFaultInfoEXT;
#endif /* VK_EXT_device_fault */
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

bool vulkan_open_driver(struct vulkan_instance_api *vk);
void vulkan_close_driver(struct vulkan_instance_api *vk);

int32_t vulkan_instance_api_load_procedures(
        struct vulkan_instance_api *api, 
        VkInstance instance, 
        uint32_t *instance_extension_bits);

int32_t vulkan_device_api_load_procedures(
        struct vulkan_instance_api *instance_api, 
        struct vulkan_device_api *api, 
        VkDevice device, 
        uint32_t drivers_version,
        uint32_t *device_extension_bits);

struct vulkan_swapchain {
    VkSwapchainKHR              sc;
    VkSurfaceKHR                surface;
    VkFormat                    format;
    VkExtent2D                  extent;

    VkSurfaceFormatKHR         *surface_formats;
    VkPresentModeKHR           *present_modes;
    VkImage                    *images;
    VkImageView                *image_views;

    uint32_t                    surface_format_count;
    uint32_t                    present_mode_count;
    uint32_t                    image_count;
};

VkResult vulkan_create_surface(
        struct vulkan_instance_api *api, 
        VkInstance instance, 
        struct hadal *hadal, 
        const VkAllocationCallbacks *allocator, 
        VkSurfaceKHR *surface);

RIVENS_TEAR(_plat_vulkan_construct_swapchain_tear__, struct platinum);

/* struct platinum : backend */
struct platinum_vulkan {
    VkInstance                  instance;
    struct vulkan_swapchain     swapchain;

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    VkDebugUtilsMessengerEXT    messenger;
#endif

    uint32_t                    extensions;
    struct vulkan_instance_api  api;

    VkAllocationCallbacks       allocator;
};

int32_t _plat_vulkan_init(
        void *internal_plat, 
        struct hadal *hadal, 
        const char *application_name, 
        uint32_t application_version,
        struct arena_allocator *temp_arena);

void _plat_vulkan_fini(void *internal_plat);

void _plat_vulkan_select_physical_devices(
        void *internal_plat,
        void **out_internal_novas, 
        struct arena_allocator *temp_arena,
        uint32_t *out_gpu_count, 
        uint32_t max_gpu_count, 
        int32_t preferred_main_gpu);

int32_t _plat_vulkan_create_devices(
        void *internal_plat,
        void *internal_novas,
        struct arena_allocator *temp_arena,
        uint32_t nova_count,
        uint32_t thread_count);

void _plat_vulkan_destroy_devices(
        void *internal_novas,
        uint32_t nova_count,
        uint32_t thread_count);

/* struct platynova : GPU device */
struct platynova_vulkan { 
    VkDevice                            logical;
    VkPhysicalDevice                    physical;
    VkPhysicalDeviceProperties          physical_properties;
    VkPhysicalDeviceFeatures            physical_features;
    VkPhysicalDeviceMemoryProperties    memory_properties;

#if defined(VK_KHR_acceleration_structure)
    VkPhysicalDeviceAccelerationStructurePropertiesKHR  acceleration_structure_properties;
#endif
    VkCommandPool              *graphics_command_pools; /**< 3 command pools per worker thread for the single graphics queue. */
    VkCommandPool              *compute_command_pools;  /**< 3 command pools per worker thread per compute queue. */
    VkCommandPool              *transfer_command_pools; /**< 1 command pool per worker thread per transfer queue. */

    VkQueue                    *graphics_queues;        /**< For now we'll use just one graphics queue, but to keep stuff clean lets define it like the other queues. */
    VkQueue                    *compute_queues;         /**< Compute workload can be partitioned and submitted per queue, on compute & transfer only families they are independent. */
    VkQueue                    *transfer_queues;        /**< A separate transfer family is optimal, some AMD cards offer two DMA cores we can use. */
    uint32_t                    graphics_queue_count;
    uint32_t                    compute_queue_count;
    uint32_t                    transfer_queue_count;

    uint32_t                    graphics_queue_family_idx;
    uint32_t                    compute_queue_family_idx;
    uint32_t                    transfer_queue_family_idx;

    uint32_t                    extensions;
    struct vulkan_device_api    api;
};

void _nova_vulkan_setup_internal_device(
        struct platynova *nova,
        void *internal_novas,
        uint32_t nova_idx,
        bool use_raytracing);

#if !defined(AMW_NDEBUG)
    #define VERIFY_VK(x) { \
        if ((x) != VK_SUCCESS) { \
            log_error("Failed to assert VK_SUCCESS for: %s", #x); \
            assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

#endif /* _PLATYNOVA_VULKAN_H */
