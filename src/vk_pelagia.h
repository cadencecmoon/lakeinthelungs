#pragma once

#include <amw/bedrock.h>
#include <amw/pelagia.h>

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    /* to avoid including windows.h */
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

/** Collects bits for checking availability of Vulkan extensions and features that interest us. 
 *  These bits are only set once at initialization, represented as a 64-bit integer value.
 *  With our Vulkan renderer we target a minimum of Vulkan core 1.2 with dynamic rendering. */
enum vulkan_extensions {
    /* instance extensions */
    vulkan_ext_surface_bit                      = (1u << 0),    /**< VK_KHR_surface */
    vulkan_ext_win32_surface_bit                = (1u << 1),    /**< VK_KHR_win32_surface */
    vulkan_ext_metal_surface_bit                = (1u << 2),    /**< VK_EXT_metal_surface */
    vulkan_ext_android_surface_bit              = (1u << 3),    /**< VK_KHR_android_surface */
    vulkan_ext_wayland_surface_bit              = (1u << 4),    /**< VK_KHR_wayland_surface */
    vulkan_ext_xcb_surface_bit                  = (1u << 5),    /**< VK_KHR_xcb_surface */
    vulkan_ext_headless_surface_bit             = (1u << 6),    /**< VK_KHR_headless_surface */
    vulkan_ext_display_bit                      = (1u << 7),    /**< VK_KHR_display */
    /* instance extensions, debug builds only */
    vulkan_ext_debug_utils_bit                  = (1u << 8),    /**< VK_EXT_debug_utils */
    vulkan_ext_layer_validation_bit             = (1u << 9),    /**< VK_LAYER_KHRONOS_validation */
    
    /* device extensions */
    vulkan_ext_swapchain_bit                    = (1ull << 0),  /**< VK_KHR_swapchain */
    vulkan_ext_device_fault_bit                 = (1ull << 1),  /**< VK_EXT_device_fault */
    vulkan_ext_memory_budget_bit                = (1ull << 2),  /**< VK_EXT_memory_budget */
    vulkan_ext_memory_priority_bit              = (1ull << 3),  /**< VK_EXT_memory_priority */
    vulkan_ext_mesh_shader_bit                  = (1ull << 4),  /**< VK_EXT_mesh_shader */
    vulkan_ext_fragment_shading_rate_bit        = (1ull << 5),  /**< VK_KHR_fragment_shading_rate */
    vulkan_ext_deferred_host_operations_bit     = (1ull << 6),  /**< VK_KHR_deferred_host_operations */
    vulkan_ext_acceleration_structure_bit       = (1ull << 7),  /**< VK_KHR_acceleration_structure */
    vulkan_ext_pipeline_library_bit             = (1ull << 8),  /**< VK_KHR_pipeline_library */
    vulkan_ext_ray_query_bit                    = (1ull << 9),  /**< VK_KHR_ray_query */
    vulkan_ext_raytracing_pipeline_bit          = (1ull << 10), /**< VK_KHR_ray_tracing_pipeline */
    vulkan_ext_raytracing_maintenance1_bit      = (1ull << 11), /**< VK_KHR_ray_tracing_maintenance1 */
    vulkan_ext_video_queue_bit                  = (1ull << 12), /**< VK_KHR_video_queue */
    vulkan_ext_video_decode_queue_bit           = (1ull << 13), /**< VK_KHR_video_decode_queue */
    vulkan_ext_video_decode_av1_bit             = (1ull << 14), /**< VK_KHR_video_decode_av1 */
    vulkan_ext_video_decode_h264_bit            = (1ull << 15), /**< VK_KHR_video_decode_h264 */
    vulkan_ext_video_maintenance1_bit           = (1ull << 16), /**< VK_KHR_video_maintenance1 */
    /* AMD hardware */
    vulkan_ext_amd_device_coherent_memory_bit   = (1ull << 17), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    vulkan_ext_dynamic_rendering_local_read_bit = (1ull << 18), /**< VK_KHR_dynamic_rendering_local_read */
    vulkan_ext_maintenance5_bit                 = (1ull << 19), /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    vulkan_ext_dynamic_rendering_bit            = (1ull << 20), /**< VK_KHR_dynamic_rendering */
    vulkan_ext_synchronization2_bit             = (1ull << 21), /**< VK_KHR_synchronization2 */
    vulkan_ext_maintenance4_bit                 = (1ull << 22), /**< VK_KHR_maintenance4 */
};

#define VULKAN_QUEUE_FAMILY_INDICES_COUNT 5
/** Holds information about physical devices available. We query this once,
 *  and reference them whenever we create a rendering device. */
struct vulkan_physical_device {
    /** A handle of the physical device this structure holds information for. */
    VkPhysicalDevice            device;

    u32                         score;
    u32                         pelagia_features;
    u64                         extension_bits;
    usize                       total_vram;

    u32                         graphics_queue_family_index;
    u32                         compute_queue_family_index;
    u32                         transfer_queue_family_index;
    u32                         sparse_binding_queue_family_index;
    u32                         video_decode_queue_family_index;
    u32                         presentation_queue_family_index;

    /** Unique queue family indices present in this device. */
    u32                         queue_family_indices[VULKAN_QUEUE_FAMILY_INDICES_COUNT];
    /** How many unique queue families are present. */
    u32                         queue_family_count;

    u32                         graphics_queue_family_queue_count;
    u32                         async_compute_queue_family_queue_count;

    VkBool32                    presentation_support;
    b32                         has_async_compute;
    b32                         has_async_transfer;
    b32                         has_async_sparse_binding;
    b32                         has_async_video;
    b32                         has_async_present;

    /** Information about hardware properties of the physical device. */
    VkPhysicalDeviceProperties2                                 properties2;
    VkPhysicalDeviceVulkan14Properties                          properties_14;
    VkPhysicalDeviceVulkan13Properties                          properties_13;
    VkPhysicalDeviceVulkan12Properties                          properties_12;
    VkPhysicalDeviceVulkan11Properties                          properties_11;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR          acceleration_structure_properties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR             raytracing_pipeline_properties;
    VkPhysicalDeviceDescriptorIndexingProperties                descriptor_indexing_properties;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR            fragment_shading_rate_properties;
    VkPhysicalDeviceMeshShaderPropertiesEXT                     mesh_shader_properties;

    /* Information about device memory properties. */
    VkPhysicalDeviceMemoryProperties2                           memory_properties2;
    VkPhysicalDeviceMemoryBudgetPropertiesEXT                   memory_budget;

    /** Information about features supported by the physical device. */
    VkPhysicalDeviceFeatures2                                   features2;
    VkPhysicalDeviceVulkan14Features                            features_14;
    VkPhysicalDeviceVulkan13Features                            features_13;
    VkPhysicalDeviceVulkan12Features                            features_12;
    VkPhysicalDeviceVulkan11Features                            features_11;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR            acceleration_structure_features;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR               raytracing_pipeline_features;
    VkPhysicalDeviceRayQueryFeaturesKHR                         ray_query_features;
    VkPhysicalDeviceDescriptorIndexingFeatures                  descriptor_indexing_features;
    VkPhysicalDeviceDynamicRenderingFeatures                    dynamic_rendering_features;
    VkPhysicalDeviceDynamicRenderingLocalReadFeatures           dynamic_rendering_local_read_features;
    VkPhysicalDeviceSynchronization2Features                    synchronization2_features;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR              fragment_shading_rate_features;
    VkPhysicalDeviceMeshShaderFeaturesEXT                       mesh_shader_features;

    /** Information about capabilities of accelerated video coding. */
    VkVideoDecodeH264ProfileInfoKHR                             decode_h264_profile;
    VkVideoDecodeH264CapabilitiesKHR                            decode_h264_capabilities;
    VkVideoDecodeAV1ProfileInfoKHR                              decode_av1_profile;
    VkVideoDecodeAV1CapabilitiesKHR                             decode_av1_capabilities;

    struct vulkan_video_capability {
        VkVideoProfileInfoKHR                                   profile;
        VkVideoDecodeCapabilitiesKHR                            decode_capabilities;
        VkVideoCapabilitiesKHR                                  video_capabilities;
    } video_h264_capability, video_av1_capability;
};

struct pelagia {
    struct pelagia_interface                interface;
    /** An instance makes Vulkan functions available to us. It is used for calls to the driver,
     *  and holds information about the application. Afterwards it is passed to the logical device. */
    VkInstance                              instance;
    /** Used for logging messages and profiling with validation layers enabled. */
    VkDebugUtilsMessengerEXT                debug_messenger;
    /** The loaded driver library. */
    void                                   *module;

    /** We query available physical devices at initialization. */
    const struct vulkan_physical_device    *physical_devices;
    /** Available physical devices, this value won't change at runtime. */
    u32                                     physical_device_count;

    /* access points */
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

    /* video coding */
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR                 vkGetPhysicalDeviceVideoCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR             vkGetPhysicalDeviceVideoFormatPropertiesKHR;
};

struct pelagia_device {
    /** We can cast struct pelagia_device* into struct pelagia**. */
    const struct pelagia                   *pelagia;
    /** The physical device used to create this rendering device. */
    const struct vulkan_physical_device    *physical;
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                logical;

    /* Command queues, they are created from the information about queue families stored in the physical device structure. */
    VkQueue                                 graphics_queue;
    VkQueue                                 compute_queue;
    VkQueue                                 transfer_queue;
    VkQueue                                 sparse_binding_queue;
    VkQueue                                 video_decode_queue;
    VkQueue                                 presentation_queue;

#if PELAGIA_ENABLE_GPU_PROFILER
    at_u32                                  device_memory_count;
    at_u32                                  buffer_count;
    at_u32                                  texture_count;
    at_u32                                  sampler_count;
    at_u32                                  pipeline_layout_count;
    at_u32                                  graphics_pipeline_count;
    at_u32                                  compute_pipeline_count;
    at_u32                                  raytracing_pipeline_count;
    at_u32                                  descriptor_set_layout_count;
    at_u32                                  descriptor_pool_count;
    at_u32                                  swapchain_count;
    at_u32                                  query_pool_count;
#endif
    u32                                     frames_in_flight;

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

    /* video coding */
    PFN_vkBindVideoSessionMemoryKHR                             vkBindVideoSessionMemoryKHR;
    PFN_vkCmdBeginVideoCodingKHR                                vkCmdBeginVideoCodingKHR;
    PFN_vkCmdControlVideoCodingKHR                              vkCmdControlVideoCodingKHR;
    PFN_vkCmdDecodeVideoKHR                                     vkCmdDecodeVideoKHR;
    PFN_vkCmdEndVideoCodingKHR                                  vkCmdEndVideoCodingKHR;
    PFN_vkCreateVideoSessionKHR                                 vkCreateVideoSessionKHR;
    PFN_vkCreateVideoSessionParametersKHR                       vkCreateVideoSessionParametersKHR;
    PFN_vkDestroyVideoSessionKHR                                vkDestroyVideoSessionKHR;
    PFN_vkDestroyVideoSessionParametersKHR                      vkDestroyVideoSessionParametersKHR;
    PFN_vkGetVideoSessionMemoryRequirementsKHR                  vkGetVideoSessionMemoryRequirementsKHR;
    PFN_vkUpdateVideoSessionParametersKHR                       vkUpdateVideoSessionParametersKHR;
};

struct pelagia_device_memory {
    /** pelagia_resource_type_device_memory */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_buffer {
    /** pelagia_resource_type_buffer */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_texture {
    /** pelagia_resource_type_texture */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_sampler {
    /** pelagia_resource_type_sampler */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_shader {
    /** pelagia_resource_type_shader */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_pipeline_layout {
    /** pelagia_resource_type_pipeline_layout */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_graphics_pipeline {
    /** pelagia_resource_type_graphics_pipeline */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_compute_pipeline {
    /** pelagia_resource_type_compute_pipeline */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_raytracing_pipeline {
    /** pelagia_resource_type_raytracing_pipeline */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_shader_binding_table {
    /** pelagia_resource_type_shader_binding_table */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_descriptor_set {
    /** pelagia_resource_type_descriptor_set */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_query_pool {
    /** pelagia_resource_type_query_pool */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_swapchain {
    /** pelagia_resource_type_swapchain */
    struct pelagia_resource_header  header;
    /** The display used to create a surface. */
    struct hadal                   *hadal;
    /** The Vulkan object for a swapchain, created using the surface below. */
    VkSwapchainKHR                  sc;
    /** A surface created by interfacing with a display backend. */
    VkSurfaceKHR                    surface;
    /** Selected mode of image presentation, when V-SYNC is disabled. */
    VkPresentModeKHR                no_vsync_present_mode;
    /** The bit flag of the composite alpha. */
    VkCompositeAlphaFlagsKHR        composite_alpha;
    /** The resolution of images, in pixels. */
    VkExtent2D                      extent;
    VkSharingMode                   sharing_mode;
    /** The intended use of the images. If transfer is set, screenshots are supported. */
    VkImageUsageFlags               image_usage;
    /** The format of images held in the swapchain. */
    VkFormat                        image_format;
    /** An array of images held in the swapchain. */
    VkImage                         images[PELAGIA_MAX_SWAPCHAIN_IMAGES];
    /** An image view for each image in the swapchain. */
    VkImageView                     image_views[PELAGIA_MAX_SWAPCHAIN_IMAGES];
    /** Synchronization semaphores used for presentation. */
    VkSemaphore                     image_available_sem[PELAGIA_MAX_FRAMES_IN_FLIGHT];
    /** Number of images in the swapchain. */
    u32                             image_count;
};

struct pelagia_bottom_level {
    /** pelagia_resource_type_bottom_level */
    struct pelagia_resource_header  header;
    /* TODO */
};

struct pelagia_top_level {
    /** pelagia_resource_type_top_level */
    struct pelagia_resource_header  header;
    /* TODO */
};

/** Get a (very helpful I guess) message of a given Vulkan error code. */
extern attr_const const char *AMWCALL vulkan_result_string(VkResult result);

/** Return true if an extension is present in the properties array. */
extern b32 AMWCALL vulkan_query_extension(VkExtensionProperties *properties, u32 count, const char *ext);

/** Translates a pelagia_texture_format into a Vulkan image format. */
extern attr_const VkFormat AMWCALL vulkan_translate_texture_format(enum pelagia_texture_format format);

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

/** Computes the number of mipmap levels needed to get from a resource of the 
 *  given size to one texel. This is the max number of mipmapc that can be created. */
attr_inline u32 vulkan_get_mipmap_count_1d(u32 width) {
	s32 padded_width = (s32)(2 * width - 1);
	u32 mipmap_count = 0;
	while (padded_width > 0) {
		padded_width &= 0x7ffffffe;
		padded_width >>= 1;
		++mipmap_count;
	}
	return mipmap_count;
}

/** The maximum of 'get_mipmap_count_1d' for all given extents. */
attr_inline u32 vulkan_get_mipmap_count_3d(VkExtent3D *extent) {
	u32 counts[3] = {
		vulkan_get_mipmap_count_1d(extent->width),
		vulkan_get_mipmap_count_1d(extent->height),
		vulkan_get_mipmap_count_1d(extent->depth)
	};
	u32 result = counts[0];
	result = (result < counts[1]) ? counts[1] : result;
	result = (result < counts[2]) ? counts[2] : result;
	return result;
}

/* calls for '_pelagia_vulkan_destroy_resources()'. */
extern void AMWCALL vulkan_destroy_device_memory(struct pelagia_device_memory *device_memory);
extern void AMWCALL vulkan_destroy_buffer(struct pelagia_buffer *buffer);
extern void AMWCALL vulkan_destroy_texture(struct pelagia_texture *texture);
extern void AMWCALL vulkan_destroy_sampler(struct pelagia_sampler *sampler);
extern void AMWCALL vulkan_destroy_shader(struct pelagia_shader *shader);
extern void AMWCALL vulkan_destroy_pipeline_layout(struct pelagia_pipeline_layout *pipeline_layout);
extern void AMWCALL vulkan_destroy_graphics_pipeline(struct pelagia_graphics_pipeline *graphics_pipeline);
extern void AMWCALL vulkan_destroy_compute_pipeline(struct pelagia_compute_pipeline *compute_pipeline);
extern void AMWCALL vulkan_destroy_raytracing_pipeline(struct pelagia_raytracing_pipeline *raytracing_pipeline);
extern void AMWCALL vulkan_destroy_shader_binding_table(struct pelagia_shader_binding_table *shader_binding_table);
extern void AMWCALL vulkan_destroy_descriptor_set(struct pelagia_descriptor_set *descriptor_set);
extern void AMWCALL vulkan_destroy_query_pool(struct pelagia_query_pool *query_pool);
extern void AMWCALL vulkan_destroy_swapchain(struct pelagia_swapchain *swapchain);
extern void AMWCALL vulkan_destroy_bottom_level(struct pelagia_bottom_level *bottom_level);
extern void AMWCALL vulkan_destroy_top_level(struct pelagia_top_level *top_level);

/* pelagia interface implementation */

extern s32 AMWCALL _pelagia_vulkan_query_physical_devices(
    struct pelagia                      *pelagia, 
    const struct hadal                  *hadal,
    u32                                 *out_device_count, 
    struct pelagia_physical_device_info *out_devices);

extern void AMWCALL _pelagia_vulkan_create_device(struct pelagia_device_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_destroy_device(struct pelagia_device *device);

extern void AMWCALL _pelagia_vulkan_create_device_memory(struct pelagia_device_memory_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_buffers(struct pelagia_buffers_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_textures(struct pelagia_textures_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_samplers(struct pelagia_samplers_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_shaders(struct pelagia_shaders_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_pipeline_layouts(struct pelagia_pipeline_layouts_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_graphics_pipelines(struct pelagia_graphics_pipelines_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_compute_pipelines(struct pelagia_compute_pipelines_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_raytracing_pipelines(struct pelagia_raytracing_pipelines_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_shader_binding_tables(struct pelagia_shader_binding_tables_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_descriptor_sets(struct pelagia_descriptor_sets_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_query_pools(struct pelagia_query_pools_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_swapchain(struct pelagia_swapchain_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_bottom_levels(struct pelagia_bottom_levels_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_create_top_levels(struct pelagia_top_levels_create_info *create_info);
extern void AMWCALL _pelagia_vulkan_destroy_resources(struct pelagia_destroy_resources_work *work);
