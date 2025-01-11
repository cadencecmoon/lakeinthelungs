/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_COBALT_VULKAN_H
#define _AMW_COBALT_VULKAN_H

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

#include <lake/cobalt.h>

#define VULKAN_MAX_FRAMES 3

/** Collects bits for checking used vulkan extensions. The bits are set only during initialization, 
 *  and then are read-only after. If an extension was promoted to core, the corresponding bit will be 
 *  set to indicate that this feature is available on a device. Instance extensions are represented
 *  as a uint32_t value, device extensions are represented as (for now) uint32_t values too. */
enum vulkan_extensions {
    /* instance extensions */
    vulkan_extension_surface_bit                            = (1u << 0),  /**< VK_KHR_surface */
    vulkan_extension_win32_surface_bit                      = (1u << 1),  /**< VK_KHR_win32_surface */
    vulkan_extension_metal_surface_bit                      = (1u << 2),  /**< VK_EXT_metal_surface */
    vulkan_extension_android_surface_bit                    = (1u << 3),  /**< VK_KHR_android_surface */
    vulkan_extension_wayland_surface_bit                    = (1u << 4),  /**< VK_KHR_wayland_surface */
    vulkan_extension_xcb_surface_bit                        = (1u << 5),  /**< VK_KHR_xcb_surface */
    vulkan_extension_headless_surface_bit                   = (1u << 6),  /**< VK_KHR_headless_surface */
    vulkan_extension_display_bit                            = (1u << 7),  /**< VK_KHR_display */
    /* instance extensions, debug builds only */
    vulkan_extension_debug_utils_bit                        = (1u << 10), /**< VK_EXT_debug_utils */
    vulkan_extension_layer_validation_bit                   = (1u << 11), /**< VK_LAYER_KHRONOS_validation */
    
    /* device extensions */
    vulkan_extension_swapchain_bit                          = (1ull << 0),  /**< VK_KHR_swapchain */
    vulkan_extension_device_fault_bit                       = (1ull << 1),  /**< VK_EXT_device_fault */
    vulkan_extension_memory_budget_bit                      = (1ull << 2),  /**< VK_EXT_memory_budget */
    vulkan_extension_memory_priority_bit                    = (1ull << 3),  /**< VK_EXT_memory_priority */
    vulkan_extension_deferred_host_operations_bit           = (1ull << 5),  /**< VK_KHR_deferred_host_operations */
    vulkan_extension_acceleration_structure_bit             = (1ull << 6),  /**< VK_KHR_acceleration_structure */
    vulkan_extension_ray_query_bit                          = (1ull << 7),  /**< VK_KHR_ray_query */
    /* AMD hardware */
    vulkan_extension_amd_device_coherent_memory_bit         = (1ull << 17), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    vulkan_extension_dynamic_rendering_local_read_bit       = (1ull << 20), /**< VK_KHR_dynamic_rendering_local_read */
    /* core 1.3, for backwards compatibility */
    vulkan_extension_dynamic_rendering_bit                  = (1ull << 21), /**< VK_KHR_dynamic_rendering */
    /* core 1.2, for backwards compatibility */
};
#define vulkan_extension_mask_raytracing \
    (vulkan_extension_deferred_host_operations_bit | \
     vulkan_extension_acceleration_structure_bit | \
     vulkan_extension_ray_query_bit)

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
#if defined(VK_KHR_win32_surface)
    PFN_vkCreateWin32SurfaceKHR                             vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR      vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface)
    PFN_vkCreateMetalSurfaceEXT                             vkCreateMetalSurfaceEXT;
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface)
    PFN_vkCreateWaylandSurfaceKHR                           vkCreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR    vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface)
    PFN_vkCreateXcbSurfaceKHR                               vkCreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR        vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface)
    PFN_vkCreateAndroidSurfaceKHR                           vkCreateAndroidSurfaceKHR;
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    PFN_vkCreateHeadlessSurfaceEXT                          vkCreateHeadlessSurfaceEXT;
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display)
    PFN_vkCreateDisplayModeKHR                              vkCreateDisplayModeKHR;
    PFN_vkCreateDisplayPlaneSurfaceKHR                      vkCreateDisplayPlaneSurfaceKHR;
    PFN_vkGetDisplayModePropertiesKHR                       vkGetDisplayModePropertiesKHR;
    PFN_vkGetDisplayPlaneCapabilitiesKHR                    vkGetDisplayPlaneCapabilitiesKHR;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR               vkGetDisplayPlaneSupportedDisplaysKHR;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR        vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR             vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* VK_KHR_display */
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
	PFN_vkAllocateCommandBuffers                                vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets                                vkAllocateDescriptorSets;
	PFN_vkAllocateMemory                                        vkAllocateMemory;
	PFN_vkBeginCommandBuffer                                    vkBeginCommandBuffer;
	PFN_vkBindBufferMemory                                      vkBindBufferMemory;
	PFN_vkBindImageMemory                                       vkBindImageMemory;
	PFN_vkCmdBeginQuery                                         vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass                                    vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets                                 vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer                                    vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline                                       vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers                                  vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage                                          vkCmdBlitImage;
	PFN_vkCmdClearAttachments                                   vkCmdClearAttachments;
	PFN_vkCmdClearColorImage                                    vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage                             vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer                                         vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage                                  vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage                                          vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer                                  vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults                               vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch                                           vkCmdDispatch;
	PFN_vkCmdDispatchIndirect                                   vkCmdDispatchIndirect;
	PFN_vkCmdDraw                                               vkCmdDraw;
	PFN_vkCmdDrawIndexed                                        vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect                                vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect                                       vkCmdDrawIndirect;
	PFN_vkCmdEndQuery                                           vkCmdEndQuery;
	PFN_vkCmdEndRenderPass                                      vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands                                    vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer                                         vkCmdFillBuffer;
	PFN_vkCmdNextSubpass                                        vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier                                    vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants                                      vkCmdPushConstants;
	PFN_vkCmdResetEvent                                         vkCmdResetEvent;
	PFN_vkCmdResetQueryPool                                     vkCmdResetQueryPool;
	PFN_vkCmdResolveImage                                       vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants                                  vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias                                       vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds                                     vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent                                           vkCmdSetEvent;
	PFN_vkCmdSetLineWidth                                       vkCmdSetLineWidth;
	PFN_vkCmdSetScissor                                         vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask                              vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference                                vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask                                vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport                                        vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer                                       vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents                                         vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp                                     vkCmdWriteTimestamp;
	PFN_vkCreateBuffer                                          vkCreateBuffer;
	PFN_vkCreateBufferView                                      vkCreateBufferView;
	PFN_vkCreateCommandPool                                     vkCreateCommandPool;
	PFN_vkCreateComputePipelines                                vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool                                  vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout                             vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent                                           vkCreateEvent;
	PFN_vkCreateFence                                           vkCreateFence;
	PFN_vkCreateFramebuffer                                     vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines                               vkCreateGraphicsPipelines;
	PFN_vkCreateImage                                           vkCreateImage;
	PFN_vkCreateImageView                                       vkCreateImageView;
	PFN_vkCreatePipelineCache                                   vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout                                  vkCreatePipelineLayout;
	PFN_vkCreateQueryPool                                       vkCreateQueryPool;
	PFN_vkCreateRenderPass                                      vkCreateRenderPass;
	PFN_vkCreateSampler                                         vkCreateSampler;
	PFN_vkCreateSemaphore                                       vkCreateSemaphore;
	PFN_vkCreateShaderModule                                    vkCreateShaderModule;
	PFN_vkDestroyBuffer                                         vkDestroyBuffer;
	PFN_vkDestroyBufferView                                     vkDestroyBufferView;
	PFN_vkDestroyCommandPool                                    vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool                                 vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout                            vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice                                         vkDestroyDevice;
	PFN_vkDestroyEvent                                          vkDestroyEvent;
	PFN_vkDestroyFence                                          vkDestroyFence;
	PFN_vkDestroyFramebuffer                                    vkDestroyFramebuffer;
	PFN_vkDestroyImage                                          vkDestroyImage;
	PFN_vkDestroyImageView                                      vkDestroyImageView;
	PFN_vkDestroyPipeline                                       vkDestroyPipeline;
	PFN_vkDestroyPipelineCache                                  vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout                                 vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool                                      vkDestroyQueryPool;
	PFN_vkDestroyRenderPass                                     vkDestroyRenderPass;
	PFN_vkDestroySampler                                        vkDestroySampler;
	PFN_vkDestroySemaphore                                      vkDestroySemaphore;
	PFN_vkDestroyShaderModule                                   vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle                                        vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer                                      vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges                               vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers                                    vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets                                    vkFreeDescriptorSets;
	PFN_vkFreeMemory                                            vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements                           vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment                             vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue                                        vkGetDeviceQueue;
	PFN_vkGetEventStatus                                        vkGetEventStatus;
	PFN_vkGetFenceStatus                                        vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements                            vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements                      vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout                             vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData                                  vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults                                   vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity                              vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges                          vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory                                             vkMapMemory;
	PFN_vkMergePipelineCaches                                   vkMergePipelineCaches;
	PFN_vkQueueBindSparse                                       vkQueueBindSparse;
	PFN_vkQueueSubmit                                           vkQueueSubmit;
	PFN_vkQueueWaitIdle                                         vkQueueWaitIdle;
	PFN_vkResetCommandBuffer                                    vkResetCommandBuffer;
	PFN_vkResetCommandPool                                      vkResetCommandPool;
	PFN_vkResetDescriptorPool                                   vkResetDescriptorPool;
	PFN_vkResetEvent                                            vkResetEvent;
	PFN_vkResetFences                                           vkResetFences;
	PFN_vkSetEvent                                              vkSetEvent;
	PFN_vkUnmapMemory                                           vkUnmapMemory;
	PFN_vkUpdateDescriptorSets                                  vkUpdateDescriptorSets;
	PFN_vkWaitForFences                                         vkWaitForFences;

    /* core 1.1 */
    PFN_vkBindBufferMemory2                                     vkBindBufferMemory2;
    PFN_vkBindImageMemory2                                      vkBindImageMemory2;
    PFN_vkCmdDispatchBase                                       vkCmdDispatchBase;
    PFN_vkCmdSetDeviceMask                                      vkCmdSetDeviceMask;
    PFN_vkCreateDescriptorUpdateTemplate                        vkCreateDescriptorUpdateTemplate;
    PFN_vkCreateSamplerYcbcrConversion                          vkCreateSamplerYcbcrConversion;
    PFN_vkDestroyDescriptorUpdateTemplate                       vkDestroyDescriptorUpdateTemplate;
    PFN_vkDestroySamplerYcbcrConversion                         vkDestroySamplerYcbcrConversion;
    PFN_vkGetBufferMemoryRequirements2                          vkGetBufferMemoryRequirements2;
    PFN_vkGetDescriptorSetLayoutSupport                         vkGetDescriptorSetLayoutSupport;
    PFN_vkGetDeviceGroupPeerMemoryFeatures                      vkGetDeviceGroupPeerMemoryFeatures;
    PFN_vkGetDeviceQueue2                                       vkGetDeviceQueue2;
    PFN_vkGetImageMemoryRequirements2                           vkGetImageMemoryRequirements2;
    PFN_vkGetImageSparseMemoryRequirements2                     vkGetImageSparseMemoryRequirements2;
    PFN_vkTrimCommandPool                                       vkTrimCommandPool;
    PFN_vkUpdateDescriptorSetWithTemplate                       vkUpdateDescriptorSetWithTemplate;

    /* core 1.2 */
    PFN_vkCmdDrawIndexedIndirectCount                           vkCmdDrawIndexedIndirectCount;
    PFN_vkCmdDrawIndirectCount                                  vkCmdDrawIndirectCount;
    PFN_vkGetBufferDeviceAddress                                vkGetBufferDeviceAddress;
    PFN_vkGetBufferOpaqueCaptureAddress                         vkGetBufferOpaqueCaptureAddress;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress                   vkGetDeviceMemoryOpaqueCaptureAddress;
    PFN_vkGetSemaphoreCounterValue                              vkGetSemaphoreCounterValue;
    PFN_vkResetQueryPool                                        vkResetQueryPool;
    PFN_vkSignalSemaphore                                       vkSignalSemaphore;
    PFN_vkWaitSemaphores                                        vkWaitSemaphores;

    /* core 1.3 */
    PFN_vkCmdBeginRendering                                     vkCmdBeginRendering;
    PFN_vkCmdEndRendering                                       vkCmdEndRendering;

#if defined(VK_KHR_swapchain)
    PFN_vkAcquireNextImageKHR                                   vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                                  vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                    vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                                   vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                                 vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                       vkQueuePresentKHR;
#endif /* VK_KHR_swapchain */
#if defined(VK_EXT_device_fault)
    PFN_vkGetDeviceFaultInfoEXT                                 vkGetDeviceFaultInfoEXT;
#endif /* VK_EXT_device_fault */
#if defined(VK_KHR_deferred_host_operations)
    PFN_vkCreateDeferredOperationKHR                            vkCreateDeferredOperationKHR;
    PFN_vkDeferredOperationJoinKHR                              vkDeferredOperationJoinKHR;
    PFN_vkDestroyDeferredOperationKHR                           vkDestroyDeferredOperationKHR;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR                 vkGetDeferredOperationMaxConcurrencyKHR;
    PFN_vkGetDeferredOperationResultKHR                         vkGetDeferredOperationResultKHR;
#endif /* VK_KHR_deferred_host_operations */
#if defined(VK_KHR_acceleration_structure)
    PFN_vkBuildAccelerationStructuresKHR                        vkBuildAccelerationStructuresKHR;
    PFN_vkCmdBuildAccelerationStructuresIndirectKHR             vkCmdBuildAccelerationStructuresIndirectKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR                     vkCmdBuildAccelerationStructuresKHR;
    PFN_vkCmdCopyAccelerationStructureKHR                       vkCmdCopyAccelerationStructureKHR;
    PFN_vkCmdCopyAccelerationStructureToMemoryKHR               vkCmdCopyAccelerationStructureToMemoryKHR;
    PFN_vkCmdCopyMemoryToAccelerationStructureKHR               vkCmdCopyMemoryToAccelerationStructureKHR;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR           vkCmdWriteAccelerationStructuresPropertiesKHR;
    PFN_vkCopyAccelerationStructureKHR                          vkCopyAccelerationStructureKHR;
    PFN_vkCopyAccelerationStructureToMemoryKHR                  vkCopyAccelerationStructureToMemoryKHR;
    PFN_vkCopyMemoryToAccelerationStructureKHR                  vkCopyMemoryToAccelerationStructureKHR;
    PFN_vkCreateAccelerationStructureKHR                        vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR                       vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR                 vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR              vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkGetDeviceAccelerationStructureCompatibilityKHR        vkGetDeviceAccelerationStructureCompatibilityKHR;
    PFN_vkWriteAccelerationStructuresPropertiesKHR              vkWriteAccelerationStructuresPropertiesKHR;
#endif /* VK_KHR_acceleration_structure */
};

/** Loads the Vulkan shared driver library and loads the global entry point procedures. */
extern b32 vulkan_open_driver(struct vulkan_instance_api *vk);

/** Unloads the Vulkan library, after this point none of the Vulkan backend calls are valid. */
extern void vulkan_close_driver(struct vulkan_instance_api *vk);

/** Fills the pointers of procedures defined in struct vulkan_instance_api. */
extern s32 vulkan_load_instance_api_procedures(
    struct vulkan_instance_api *vk, 
    VkInstance                  instance, 
    u32                         instance_extensions);

/** Fills the pointers of procedures defined in struct vulkan_device_api. */
extern s32 vulkan_load_device_api_procedures(
    struct vulkan_instance_api *vk, 
    struct vulkan_device_api   *api, 
    VkDevice                    device, 
    u32                         device_api_version, 
    u64                         device_extensions);

/** Get a (very helpful) message of a given Vulkan error code. */
extern const char *vulkan_result_string(VkResult result);

#if !defined(AMW_NDEBUG)
    #define VERIFY_VK(x) { \
        VkResult res__ = (x); \
        if (res__ != VK_SUCCESS) { \
            log_error("Failed to assert VK_SUCCESS for: %s", #x); \
            log_error("The error message: %s", vulkan_result_string(res__)); \
            assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

/** Holds Vulkan objects that are related to the swapchain. This includes the swapchain handle 
 *  itself, our window surface and image views for images in the swapchain. The swapchain depends
 *  on the device, but we only need one, thus the main device will be responsible for controlling
 *  the swapchain. It's changed substantially whenever the window framebuffer resolution changes. */
struct vulkan_swapchain {
    /** The swapchain created within a Hadopelagic window. NULL if the window is minimized. */
    VkSwapchainKHR              sc;
    VkSurfaceKHR                surface;        /**< A surface created within this swapchain. */
    VkFormat                    format;         /**< The format of the held swapchain images. */
    VkExtent2D                  extent;         /**< The resolution of the held swapchain images in pixels. */

    VkSurfaceCapabilitiesKHR    surface_capabilities;   /**< The surface capabilities of the associated device. */
    VkSurfaceFormatKHR         *surface_formats;        /**< List of available surface formats. */
    VkPresentModeKHR           *present_modes;          /**< List of available presentation modes. */
    u32                         surface_format_count;   /**< Number of available surface formats, for the list above. */
    u32                         present_mode_count;     /**< Number of available presentation modes, for the list above. */

    VkImage                    *images;         /**< An array of images in the swapchain. */
    VkImageView                *image_views;    /**< An image view for each image in the swapchain. */
    u32                         image_count;    /**< Number of total images held by the swapchain (internally max 3). */
    struct arena_allocator      arena;          /**< A host memory pool for allocating the image and view arrays, reset every time the swapchain is reconstructed. */
};

/** Holds the shared state of the Vulkan backend. This includes a VkInstance, instance-dependent API
 *  procedures, the swapchain and debug messengers or other non-device specific callbacks. */
struct vulkan_backend {
    /** This object makes Vulkan functions available to us. It's used for driver 
     *  calls, that are independent of any particular VkDevice. */
    VkInstance                  instance;
    VkAllocationCallbacks       allocator;      /**< TODO Our own allocation functions that use ipomoeaalba internally. */

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    /** Used to log messages given to us from the validation layers. */
    VkDebugUtilsMessengerEXT    messenger;
#endif

    struct vulkan_swapchain     swapchain;      /**< The swapchain handled by the main device. Only one window is supported. */
    u32                         extensions;     /**< Bits to check availability of used Vulkan instance extensions. */
    struct vulkan_instance_api  api;            /**< Global driver and instance procedures. */
};

/** The information needed to later perform a vkAllocateMemory call. */
struct vulkan_allocation_request {
    /**< Defines for what purpose is this allocation. */
    VkMemoryPropertyFlags   usage;
    /** An index identifying a memory type from the memoryTypes 
     *  array of the VkPhysicalDeviceMemoryProperties struct. */
    u32                     memory_type_idx;
    /** The size of the memory allocation. */
    VkDeviceSize            size;
};

/** Collects the information necessary for subdividing large memory allocations,
 *  and the offset used for binding resources like images and buffers. */
struct vulkan_allocation {
    VkDeviceMemory  memory; /**< The actual memory allocation we're subdividing. */
    VkDeviceSize    size;   /**< The size in bytes of this suballocation. */
    VkDeviceSize    offset; /**< The offset in the bound memory allocation in bytes. */
};

/** The context of a rendering device, explicitly used within backend cobalt 
 *  calls and representing an individual GPU. */
struct vulkan_device {
    at_u64                              flags;

    /** The vulkan object for the physical device below. */
    VkDevice                            logical;
    /** The physical device (GPU) that is being used in this context. */
    VkPhysicalDevice                    physical;
    /** Information about Vulkan features supported by the used physical device. */
    VkPhysicalDeviceFeatures            physical_features;
    /** Information about hardware properties of the used physical device. */
    VkPhysicalDeviceProperties          physical_properties;
#if defined(VK_KHR_acceleration_structure)
    /** Information about the support for ray tracing acceleration structures. */
    VkPhysicalDeviceAccelerationStructurePropertiesKHR  acceleration_structure_properties;
#endif
    /** Information about heap memory available from the used physical device. */
    VkPhysicalDeviceMemoryProperties                    memory_properties;
#if defined(VK_EXT_memory_budget)
    /** Information about how much heap memory can be safely allocated for the used physical device. */
    VkPhysicalDeviceMemoryBudgetPropertiesEXT           memory_budget;
#endif

    /** Command pools are shared between queue families. The total amount of allocated command pools 
     *  is equal to: max images X queue families in use X threads, and are unique to the VkDevice.
     *  The max swapchain images will be equal to 3, that's also due to the parallel gameloop workload.
     *  The individual pointers (graphics, compute, transfer) point inside the array, and the command 
     *  pools will be shared between queues of the same queue family idx. */
    VkCommandPool      *raw_command_pools;          
    u32                 raw_command_pool_count;     /**< Number of total command pools in this device. */
    u32                 queue_command_pool_count;   /**< Used to iterate through <queue>_command_pools. */

    VkCommandPool      *graphics_command_pools;     /**< Command pools used by the graphics queue family. */
    VkCommandPool      *compute_command_pools;      /**< Command pools used by the compute queue family. */
    VkCommandPool      *transfer_command_pools;     /**< Command pools used by the transfer queue family. */

    /** Command queues supporting the features we'll be using. The Vulkan spec guarantees, that if 
     *  a GPU supports draw commands (no reason why wouldn't it??), atleast one queue with graphics, 
     *  compute and transfer commands with presentation capabilities is available to use. Queues are 
     *  externally synchronized, and multiple threads can't submit to the same queue concurently.
     *  There is no real reason to use more than one queue for graphics, the hardware I know of won't 
     *  even have more than 1 real hardware queue for graphics (where the NVidia cards give us 16 queues 
     *  in one queue family, it only means that the driver does external synchronization for us). Some 
     *  GPUs will offer asynchronous hardware queues for compute, or for transfer-only commands - there 
     *  are stuff like sparse binding, video encoding/decoding queues too, but we won't use them really.
     *  We wan't to exploit the existence of such queue families with our parallel design, so we will
     *  query for them during initialization - if no async queues are available to us, the compute or 
     *  transfer queues will fallback to use the graphics queue instead, and we will be doing the 
     *  synchronization for submiting our command buffers to the queues ourselfes. */
    VkQueue                     graphics_queue;
    /** We will request an async compute queue family, or use the graphics queue instead. */
    VkQueue                    *compute_queues;
    /** We will request a transfer-only queue family, or use the graphics queue instead. */
    VkQueue                    *transfer_queues;
    u32                         compute_queue_count;  /**< If 0, the compute queue shares the graphics queue. */
    u32                         transfer_queue_count; /**< If 0, the transfer queue shares the graphics queue. */

    /** Indices of different queue families where our queues were created. For comparing if 
     *  compute or transfer are using an unique async queue family, it's enough to just compare 
     *  with the index of the graphics queue family, as compute and transfer queue families will 
     *  only ever be equal (in the context of our rendering backend) if they share the same family 
     *  as our graphics queue. It will simplify some stuff, so i'll take that as a rule of thumb. */
    u32                         graphics_queue_family_idx;
    u32                         compute_queue_family_idx;
    u32                         transfer_queue_family_idx;

    /** Bits to check support for extensions that interest us. Set during device creation, and then read-only. */
    u64                         extensions;
    struct vulkan_device_api    api;
};

/** Bundles arrays of Vulkan image objects with meta-data and image views.
 *  Handles the device memory allocations for the images. */
struct vulkan_images {
    VkImage                    *images;         /**< An array of Vulkan objects for each image. */
    VkImageView                *views;          /**< Views onto the contents of every image or NULL, if no view was requested. */
    VkDeviceSize               *offsets;        /**< The offset in the bound memory allocation in bytes, for each buffer object. Used to calculate sizes of an image. */
    struct vulkan_allocation    allocation;     /**< The memory suballocation that serves all of the images. */
    u32                         image_count;    /**< Number of images and meta-data in the arrays. */
};

/** Bundles arrays of Vulkan buffer objects with information needed to access its memory. */
struct vulkan_buffers {
    VkBuffer                   *buffers;        /**< An array of Vulkan buffer objects. */
    VkDeviceSize               *offsets;        /**< The offset in the bound memory allocation in bytes, for each buffer object. Used to calculate sizes of a buffer. */
    struct vulkan_allocation    allocation;     /**< The memory suballocation that serves all of the buffers. */
    u32                         buffer_count;   /**< Number of buffers and meta-data in the arrays. */
};

/** Handles all information needed to compile a shader into a module. */
struct vulkan_shader_request {
    char               *shader_file_path;       /**< A path to the file with the GLSL source code (relative to the current working directory). */
    char               *include_path;           /**< The director(ies) which are searched for includes. */
    char               *entry_point;            /**< The name of the function that serves as the entry point. */
    VkShaderStageFlags  stage;                  /**< A single bit from VkShaderStageFlagBits to indicate the targeted shader stage. */
    u32                 define_count;           /**< The number of defines. */
    /** A list of strings providing the defines, eitheras "IDENTIFIER" or "IDENTIFIER=VALUE". 
     *  Do not use white space, these strings go into the command line unmodified. */
    char              **defines;
};

/** Bundles a Vulkan shader module with it's SPIR-V code. */
struct vulkan_shader {
    VkShaderModule      module;                 /**< The Vulkan compiled shader module. */
    usize               spirv_size;             /**< The size of the compiled SPIR-V code in bytes. */
    u32                *spirv_code;             /**< An array of the compiled SPIR-V code. */
};

/** Holds information needed to create a Vulkan pipeline object. */
struct vulkan_pipeline_request {
    VkPipelineVertexInputStateCreateInfo    vertex_input_state;     /* graphics pipelines only */
    VkPipelineInputAssemblyStateCreateInfo  input_assembly_state;
    VkPipelineRasterizationStateCreateInfo  rasterization_state;
    VkPipelineColorBlendStateCreateInfo     color_blend_state;
    VkPipelineDepthStencilStateCreateInfo   depth_stencil_state;
    VkPipelineViewportStateCreateInfo       viewport_state;
    VkPipelineMultisampleStateCreateInfo    multisample_state;
};

struct vulkan_pipelines {
    VkDescriptorSetLayout  *descriptor_set_layouts; /**< Descriptor set layouts used by the pipelines. */
    VkPipelineLayout       *pipeline_layouts;       /**< Pipeline layouts used by the pipelines. */
    VkPipeline             *pipelines;              /**< An array of pipeline states. */
    u32                     pipeline_count;         /**< Number of pipelines. */
};

/** Returns the aspect ratio for a given swapchain. */
AMW_INLINE f32 vulkan_get_aspect_ratio(const struct vulkan_swapchain *swapchain) {
    return ((f32) swapchain->extent.width) / ((f32) swapchain->extent.height);
}

/** Returns the smallest number that is greater equal offset and a multiple of the given positive integer. */
AMW_INLINE VkDeviceSize vulkan_align_memory_offset(VkDeviceSize offset, VkDeviceSize alignment) {
    return ((offset + alignment - 1) / alignment) * alignment;
}

/** Compute the number of mipmap levels needed to get from a resource of the given size to one texel.
 *  This is the maximum number of mipmaps that can be created. */
AMW_INLINE u32 vulkan_get_mipmap_count_1d(u32 width) {
    s32 padded_width = (s32)(2 * width - 1);
    u32 mipmap_count = 0;
    while (padded_width > 0) {
        padded_width &= 0x7ffffffe;
        padded_width >>= 1;
        ++mipmap_count;
    }
    return mipmap_count;
}

/** Returns the maximum of vulkan_get_mipmap_count_1d() for all given extents. */
AMW_INLINE u32 vulkan_get_mipmap_count_3d(VkExtent3D extent) {
    u32 counts[3] = {
        vulkan_get_mipmap_count_1d(extent.width),
        vulkan_get_mipmap_count_1d(extent.height),
        vulkan_get_mipmap_count_1d(extent.depth)
    };
    u32 result = counts[0];
    result = (result < counts[1]) ? counts[1] : result;
    result = (result < counts[2]) ? counts[2] : result;
    return result;
}

extern s32 vulkan_compile_glsl_shader(void);
extern void vulkan_destroy_shader(void);

/** Goes through memory types available for the device and identifies the lowest 
 *  index that satisfies all given requirements. 
 *
 *  @param memory_type_bits     A bit mask indicating which memory type indices are 
 *                              admissible. Available from VkMemoryRequirements. 
 *  @param property_mask        A combination of VkMemoryPropertyFlagBits.
 *
 *  @return 0 if type_idx was set to a valid reply, 1 if no compatible memory is available. */
extern s32 vulkan_find_memory_type(
    u32                        *type_idx, 
    const struct vulkan_device *device, 
    u32                         memory_type_bits,
    VkMemoryPropertyFlags       property_mask);

/* vk_device.c */
AMWAPI s32 cobalt_vulkan_renderer_init(
    struct cobalt          *cobalt, 
    struct ipomoeaalba     *ia, 
    struct hadopelagic     *hadal, 
    const char             *application_name, 
    u32                     application_version, 
    struct arena_allocator *temp_arena);

/* vk_device.c */
AMWAPI void cobalt_vulkan_renderer_fini(struct cobalt *cobalt);

/* vk_device.c */
AMWAPI s32 cobalt_vulkan_construct_devices(
    struct cobalt          *cobalt,
    struct riven           *riven,
    thread_id              *threads,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    struct arena_allocator *temp_arena);

/* vk_device.c */
AMWAPI void cobalt_vulkan_destroy_devices(struct cobalt *cobalt);

/* vk_surface.c */
AMWAPI s32 cobalt_vulkan_create_swapchain_surface(struct cobalt *cobalt, struct hadopelagic *hadal);

/* vk_device.c */
AMWAPI RIVENS_TEAR(cobalt_vulkan_construct_swapchain_tear, struct cobalt_construct_swapchain_work *work);

#endif /* _AMW_COBALT_VULKAN_H */
