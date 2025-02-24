#pragma once

#include <amw/bedrock.h>
#include <amw/log.h>
#include <amw/riven.h>
#include <amw/string.h>

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    #ifndef UNICODE
        #define UNICODE 1
    #endif
    #ifndef _UNICODE
        #define _UNICODE 1
    #endif
    #ifndef VC_EXTRALEAN
        #define VC_EXTRALEAN
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <vulkan/vk_platform.h>
    #include <vulkan/vulkan_core.h>

    typedef unsigned long                DWORD;
    typedef const wchar_t               *LPCWSTR;
    typedef void                        *HANDLE;
    typedef struct HINSTANCE__          *HINSTANCE;
    typedef struct HWND__               *HWND;
    typedef struct HMONITOR__           *HMONITOR;
    typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES;
    
    #include <vulkan/vulkan_win32.h>
    #if defined(VK_ENABLE_BETA_EXTENSIONS)
        #include <vulkan/vulkan_beta.h>
    #endif
#else
    #include <vulkan/vulkan.h>
#endif

struct hadopelagic;
struct harridan;

/** Collects bits for checking availability of Vulkan extensions and features that interest us. 
 *  These bits are only set once at initialization, represented as a 64-bit integer value.
 *  With our Vulkan renderer we target a minimum of Vulkan core 1.2 with dynamic rendering. */
enum vk_extensions {
    /* instance extensions */
    vk_extension_surface_bit                            = (1ull << 0),  /**< VK_KHR_surface */
    vk_extension_win32_surface_bit                      = (1ull << 1),  /**< VK_KHR_win32_surface */
    vk_extension_metal_surface_bit                      = (1ull << 2),  /**< VK_EXT_metal_surface */
    vk_extension_android_surface_bit                    = (1ull << 3),  /**< VK_KHR_android_surface */
    vk_extension_wayland_surface_bit                    = (1ull << 4),  /**< VK_KHR_wayland_surface */
    vk_extension_xcb_surface_bit                        = (1ull << 5),  /**< VK_KHR_xcb_surface */
    vk_extension_headless_surface_bit                   = (1ull << 6),  /**< VK_KHR_headless_surface */
    vk_extension_display_bit                            = (1ull << 7),  /**< VK_KHR_display */
    /* instance extensions, debug builds only */
    vk_extension_debug_utils_bit                        = (1ull << 8),  /**< VK_EXT_debug_utils */
    vk_extension_layer_validation_bit                   = (1ull << 9),  /**< VK_LAYER_KHRONOS_validation */
    
    /* device extensions */
    vk_extension_swapchain_bit                          = (1ull << 10), /**< VK_KHR_swapchain */
    vk_extension_device_fault_bit                       = (1ull << 11), /**< VK_EXT_device_fault */
    vk_extension_memory_budget_bit                      = (1ull << 12), /**< VK_EXT_memory_budget */
    vk_extension_memory_priority_bit                    = (1ull << 13), /**< VK_EXT_memory_priority */
    vk_extension_mesh_shader_bit                        = (1ull << 14), /**< VK_EXT_mesh_shader */
    vk_extension_fragment_shading_rate_bit              = (1ull << 15), /**< VK_KHR_fragment_shading_rate */
    vk_extension_deferred_host_operations_bit           = (1ull << 16), /**< VK_KHR_deferred_host_operations */
    vk_extension_acceleration_structure_bit             = (1ull << 17), /**< VK_KHR_acceleration_structure */
    vk_extension_pipeline_library_bit                   = (1ull << 18), /**< VK_KHR_pipeline_library */
    vk_extension_ray_query_bit                          = (1ull << 19), /**< VK_KHR_ray_query */
    vk_extension_ray_tracing_pipeline_bit               = (1ull << 20), /**< VK_KHR_ray_tracing_pipeline */
    vk_extension_ray_tracing_maintenance1_bit           = (1ull << 21), /**< VK_KHR_ray_tracing_maintenance1 */
    vk_extension_video_queue_bit                        = (1ull << 22), /**< VK_KHR_video_queue */
    vk_extension_video_encode_queue_bit                 = (1ull << 23), /**< VK_KHR_video_encode_queue */
    vk_extension_video_decode_queue_bit                 = (1ull << 24), /**< VK_KHR_video_decode_queue */
    /* AMD hardware */
    vk_extension_amd_device_coherent_memory_bit         = (1ull << 25), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    vk_extension_dynamic_rendering_local_read_bit       = (1ull << 26), /**< VK_KHR_dynamic_rendering_local_read */
    vk_extension_maintenance5_bit                       = (1ull << 27), /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    vk_extension_dynamic_rendering_bit                  = (1ull << 28), /**< VK_KHR_dynamic_rendering */
    vk_extension_synchronization2_bit                   = (1ull << 29), /**< VK_KHR_synchronization2 */
    vk_extension_maintenance4_bit                       = (1ull << 30), /**< VK_KHR_maintenance4 */
};
#define vk_extension_mask_raytracing \
    (vk_extension_deferred_host_operations_bit | \
     vk_extension_acceleration_structure_bit | \
     vk_extension_ray_query_bit)
#define vk_extension_mask_raytracing_pipeline \
    (vk_extension_mask_raytracing | \
     vk_extension_pipeline_library_bit | \
     vk_extension_ray_tracing_pipeline_bit)
#define vk_extension_mask_api14 \
    (vk_extension_dynamic_rendering_local_read_bit | \
     vk_extension_maintenance5_bit)
#define vk_extension_mask_api13 \
    (vk_extension_dynamic_rendering_bit | \
     vk_extension_synchronization2_bit | \
     vk_extension_maintenance4_bit)
#define vk_extension_mask_video \
    (vk_extension_synchronization2_bit | \
     vk_extension_video_queue_bit | \
     vk_extension_video_encode_queue_bit | \
     vk_extension_video_decode_queue_bit)
#define vk_extension_mask_required_device \
    (vk_extension_swapchain_bit | \
     vk_extension_dynamic_rendering_local_read_bit | \
     vk_extension_dynamic_rendering_bit | \
     vk_extension_synchronization2_bit)

/** Vulkan API of globa, instance and device procedures. */
struct vk_api {
    void                                                       *module;
    u64                                                         extensions;
    PFN_vkGetInstanceProcAddr                                   vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr                                     vkGetDeviceProcAddr;

    /* global procedures */
    PFN_vkCreateInstance                                        vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties                  vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties                      vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceVersion                              vkEnumerateInstanceVersion;

    /* instance procedures */
    PFN_vkCreateDevice                                          vkCreateDevice;
    PFN_vkDestroyInstance                                       vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties                    vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices                              vkEnumeratePhysicalDevices;
    PFN_vkEnumeratePhysicalDeviceGroups                         vkEnumeratePhysicalDeviceGroups;
    PFN_vkGetPhysicalDeviceFeatures                             vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFeatures2                            vkGetPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceFormatProperties                     vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceFormatProperties2                    vkGetPhysicalDeviceFormatProperties2;
    PFN_vkGetPhysicalDeviceImageFormatProperties                vkGetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties2               vkGetPhysicalDeviceImageFormatProperties2;
    PFN_vkGetPhysicalDeviceMemoryProperties                     vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties2                    vkGetPhysicalDeviceMemoryProperties2;
    PFN_vkGetPhysicalDeviceProperties                           vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceProperties2                          vkGetPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties                vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2               vkGetPhysicalDeviceQueueFamilyProperties2;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties          vkGetPhysicalDeviceSparseImageFormatProperties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2         vkGetPhysicalDeviceSparseImageFormatProperties2;
    PFN_vkGetPhysicalDeviceToolProperties                       vkGetPhysicalDeviceToolProperties;

    /* surface */
    PFN_vkDestroySurfaceKHR                                     vkDestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR                    vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR               vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR                    vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR               vkGetPhysicalDeviceSurfacePresentModesKHR;

#if defined(VK_KHR_win32_surface)
    PFN_vkCreateWin32SurfaceKHR                                 vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR          vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface)
    PFN_vkCreateMetalSurfaceEXT                                 vkCreateMetalSurfaceEXT;
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface)
    PFN_vkCreateWaylandSurfaceKHR                               vkCreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR        vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface)
    PFN_vkCreateXcbSurfaceKHR                                   vkCreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR            vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface)
    PFN_vkCreateAndroidSurfaceKHR                               vkCreateAndroidSurfaceKHR;
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    PFN_vkCreateHeadlessSurfaceEXT                              vkCreateHeadlessSurfaceEXT;
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display)
    PFN_vkCreateDisplayModeKHR                                  vkCreateDisplayModeKHR;
    PFN_vkCreateDisplayPlaneSurfaceKHR                          vkCreateDisplayPlaneSurfaceKHR;
    PFN_vkGetDisplayModePropertiesKHR                           vkGetDisplayModePropertiesKHR;
    PFN_vkGetDisplayPlaneCapabilitiesKHR                        vkGetDisplayPlaneCapabilitiesKHR;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR                   vkGetDisplayPlaneSupportedDisplaysKHR;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR            vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR                 vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* VK_KHR_display */

    /* debug utils */
    PFN_vkSetDebugUtilsObjectNameEXT                            vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT                             vkSetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT                          vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT                            vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT                         vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT                            vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT                              vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT                           vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT                          vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                         vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT                            vkSubmitDebugUtilsMessageEXT;

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
    PFN_vkCmdPipelineBarrier2                                   vkCmdPipelineBarrier2;
    PFN_vkCmdResetEvent2                                        vkCmdResetEvent2;
    PFN_vkCmdSetEvent2                                          vkCmdSetEvent2;
    PFN_vkCmdWaitEvents2                                        vkCmdWaitEvents2;
    PFN_vkCmdWriteTimestamp2                                    vkCmdWriteTimestamp2;
    PFN_vkQueueSubmit2                                          vkQueueSubmit2;
    PFN_vkGetDeviceBufferMemoryRequirements                     vkGetDeviceBufferMemoryRequirements;
    PFN_vkGetDeviceImageMemoryRequirements                      vkGetDeviceImageMemoryRequirements;
    PFN_vkGetDeviceImageSparseMemoryRequirements                vkGetDeviceImageSparseMemoryRequirements;

    /* core 1.4 */
    PFN_vkCmdBindIndexBuffer2                                   vkCmdBindIndexBuffer2;
    PFN_vkCmdSetRenderingAttachmentLocations                    vkCmdSetRenderingAttachmentLocations;
    PFN_vkCmdSetRenderingInputAttachmentIndices                 vkCmdSetRenderingInputAttachmentIndices;
    PFN_vkGetDeviceImageSubresourceLayout                       vkGetDeviceImageSubresourceLayout;
    PFN_vkGetImageSubresourceLayout2                            vkGetImageSubresourceLayout2;
    PFN_vkGetRenderingAreaGranularity                           vkGetRenderingAreaGranularity;

    /* swapchain */
    PFN_vkAcquireNextImageKHR                                   vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                                  vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                    vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                                   vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                                 vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                       vkQueuePresentKHR;

    /* device fault */
    PFN_vkGetDeviceFaultInfoEXT                                 vkGetDeviceFaultInfoEXT;

    /* mesh shader */
    PFN_vkCmdDrawMeshTasksEXT                                   vkCmdDrawMeshTasksEXT;
    PFN_vkCmdDrawMeshTasksIndirectEXT                           vkCmdDrawMeshTasksIndirectEXT;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT                      vkCmdDrawMeshTasksIndirectCountEXT;

    /* deferred host operations */
    PFN_vkCreateDeferredOperationKHR                            vkCreateDeferredOperationKHR;
    PFN_vkDeferredOperationJoinKHR                              vkDeferredOperationJoinKHR;
    PFN_vkDestroyDeferredOperationKHR                           vkDestroyDeferredOperationKHR;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR                 vkGetDeferredOperationMaxConcurrencyKHR;
    PFN_vkGetDeferredOperationResultKHR                         vkGetDeferredOperationResultKHR;

    /* acceleration structure */
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

    /* ray tracing pipeline */
    PFN_vkCmdSetRayTracingPipelineStackSizeKHR                  vkCmdSetRayTracingPipelineStackSizeKHR;
    PFN_vkCmdTraceRaysIndirectKHR                               vkCmdTraceRaysIndirectKHR;
    PFN_vkCmdTraceRaysIndirect2KHR                              vkCmdTraceRaysIndirect2KHR; /* ray_tracing_maintenance1 */
    PFN_vkCmdTraceRaysKHR                                       vkCmdTraceRaysKHR;
    PFN_vkCreateRayTracingPipelinesKHR                          vkCreateRayTracingPipelinesKHR;
    PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR       vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupHandlesKHR                    vkGetRayTracingShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupStackSizeKHR                  vkGetRayTracingShaderGroupStackSizeKHR;
};

/** Loads the Vulkan driver library and saves global entry point procedures. */
AMWAPI b32 open_vulkan_driver(struct vk_api *api, b32 verbose);

/** Unloads the Vulkan driver library, after this point none of the Vulkan calls are valid. */
AMWAPI void close_vulkan_driver(struct vk_api *api);

/** Fills the pointers to procedures defined in vk_instance_api, after instance creation. */
AMWAPI attr_nonnull(1,2)
b32 load_vulkan_instance_procedures(struct vk_api *api, VkInstance instance);

/** Fills the pointers to procedures defined in vk_device_api, after device creation. */
AMWAPI attr_nonnull(1,2)
b32 load_vulkan_device_procedures(struct vk_api *api, VkDevice device, u32 api_version);

/** Get a (very helpful I guess) message of a given Vulkan error code. */
AMWAPI attr_const
const char *vulkan_result_string(VkResult result);

#if !defined(NDEBUG)
    #define VERIFY_VK(x) { \
        VkResult res__ = (x); \
        if (res__ != VK_SUCCESS) { \
            log_error("Failed to assert VK_SUCCESS for: %s", #x); \
            log_error("The Vulkan error message: %s", vulkan_result_string(res__)); \
            assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

/** We use the swapchain as a way to present to the window surface. */
struct swapchain {
    /** A swapchain created by the means of our display backend. */
    VkSwapchainKHR              sc;
    /** A surface created within the context of a window. */
    VkSurfaceKHR                surface;
    /** The format of images held in the swapchain. */
    VkFormat                    format;
    /** The resolution of images, in pixels. */
    VkExtent2D                  extent;
    /** Surface capabilities of the physical device in use. */
    VkSurfaceCapabilitiesKHR    surface_capabilities;
    /** Selected surface format. */
    VkSurfaceFormatKHR          surface_format;
    /** Selected presentation mode. */
    VkPresentModeKHR            present_mode;
    /** Semaphores for synchronizing presentation with the graphics queue.
     *  One semaphore per swapchain image. */
    VkSemaphore                *image_available_sem;
    /** An array of images held in the swapchain. */
    VkImage                    *images;
    /** An image view for each image in the swapchain. */
    VkImageView                *image_views;
    /** Number of swapchain images. */
    u32                         image_count;
};

/** Controls the rendering device overall state. */
enum harridan_flags {
    harridan_flag_device_is_valid       = (1u << 0), /**< True for a valid rendering device. */
    harridan_flag_debug_utils           = (1u << 1), /**< Set when debug labeling and validation layers are enabled. */
    harridan_flag_async_compute         = (1u << 2), /**< When the compute queue is not overloaded with the graphics queue. */
    harridan_flag_async_transfer        = (1u << 3), /**< When the transfer queue is not overloaded with the graphics queue. */
    harridan_flag_async_sparse_binding  = (1u << 4), /**< When the sparse binding queue is not overloaded with the graphics queue. */
    harridan_flag_video_coding          = (1u << 5), /**< Whether accelerated video coding is available. */
};

struct harridan {
    at_u32                      flags;

    rivens_tag_t                device_tag;
    rivens_tag_t                render_state_tag;
    struct rivens              *riven;

    /** An instance makes Vulkan functions available to us. It is used for driver calls,
     *  and holds information about the application before passing it to a rendering device. */
    VkInstance                  instance;
    /** The Vulkan object of a logical rendering device, using the given physical device. */
    VkDevice                    logical;
    /** A handle of the physical device used to create this rendering device. */
    VkPhysicalDevice            physical;
    /** Used for logging messages and profiling with debug mode and validation layers enabled. */
    VkDebugUtilsMessengerEXT    debug_messenger;
    /** TODO Host allocation callbacks implemented with riven. */
    VkAllocationCallbacks       allocation_callbacks;

    u32                         graphics_queue_family_index;
    u32                         compute_queue_family_index;
    u32                         transfer_queue_family_index;
    u32                         sparse_queue_family_index;
    u32                         decode_queue_family_index;
    u32                         encode_queue_family_index;
    /** For convenience we put unique indices in an array. */
    u32                        *queue_family_indices;
    /** How many unique queue families are currently in use, very much device dependent. */
    u32                         queue_family_count;

    VkQueue                     graphics_queue;
    VkQueue                     compute_queue;
    VkQueue                     transfer_queue;
    VkQueue                     sparse_queue;
    VkQueue                     decode_queue;
    VkQueue                     encode_queue;

    /** Our means of presenting rendered images. */
    struct swapchain            swapchain;
    /** Vulkan procedures loaded from the drivers. */
    struct vk_api               api;

    /** Information about hardware properties of the physical device. */
    VkPhysicalDeviceProperties2                         properties2;
    VkPhysicalDeviceVulkan11Properties                  properties_11;
    VkPhysicalDeviceVulkan12Properties                  properties_12;
    VkPhysicalDeviceVulkan13Properties                  properties_13;
    VkPhysicalDeviceVulkan14Properties                  properties_14;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR  acceleration_structure_properties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR     raytracing_pipeline_properties;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR    fragment_shading_rate_properties;
    VkPhysicalDeviceMeshShaderPropertiesEXT             mesh_shader_properties;
    VkPhysicalDeviceMemoryProperties2                   memory_properties2;
    VkPhysicalDeviceMemoryBudgetPropertiesEXT           memory_budget;

    /** Information about optional features supported by the physical device. */
    VkPhysicalDeviceFeatures2                           features2;
    VkPhysicalDeviceVulkan11Features                    features_11;
    VkPhysicalDeviceVulkan12Features                    features_12;
    VkPhysicalDeviceVulkan13Features                    features_13;
    VkPhysicalDeviceVulkan14Features                    features_14;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR    acceleration_structure_features;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR       raytracing_pipeline_features;
    VkPhysicalDeviceRayQueryFeaturesKHR                 raytracing_query_features;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR      fragment_shading_rate_features;
    VkPhysicalDeviceMeshShaderFeaturesEXT               mesh_shader_features;

    /** Information about capabilities of accelerated video coding. */
    VkVideoDecodeH264ProfileInfoKHR                     decode_h264_profile;
    VkVideoDecodeH264CapabilitiesKHR                    decode_h264_capabilities;
    VkVideoEncodeH264ProfileInfoKHR                     encode_h264_profile;
    VkVideoEncodeH264CapabilitiesKHR                    encode_h264_capabilities;
    VkVideoProfileInfoKHR                               video_profile;
    VkVideoDecodeCapabilitiesKHR                        video_decode_capabilities;
    VkVideoCapabilitiesKHR                              video_capabilities;
};

/** Creates a rendering device and prepares the swapchain from given information. */
AMWAPI attr_nonnull_all
s32 harridan_init(
    struct harridan        *harridan,
    struct hadopelagic     *hadal,
    struct rivens          *riven,
    rivens_tag_t            tag,
    const char             *app_name,
    u32                     app_version,
    const char             *engine_name,
    u32                     engine_version,
    s32                     select_device_index,
    b32                     verbose,
    b32                     debug_utilities);

/** Destroys the rendering device. */
AMWAPI attr_nonnull_all
void harridan_fini(struct harridan *harridan);
