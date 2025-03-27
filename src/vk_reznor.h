#pragma once

#include <amw/bedrock.h>
#include <amw/reznor.h>

FN_REZNOR_DEVICE_QUERY(vulkan);
FN_REZNOR_DEVICE_ASSEMBLY(vulkan);
FN_REZNOR_DEVICE_DESTROY(vulkan);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, device_memory);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, buffer);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, texture);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, sampler);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, descriptor_set_layout);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, descriptor_set);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, pipeline_layout);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, graphics_pipeline);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, compute_pipeline);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, raytracing_pipeline);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, shader_binding_table);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, bottom_level);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, top_level);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, query_pool);
FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, swapchain);
FN_REZNOR_SWAPCHAIN_TRY_RECREATE(vulkan);
FN_REZNOR_FRAME_BEGIN(vulkan);
FN_REZNOR_FRAME_NEXT_IMAGES(vulkan);
FN_REZNOR_FRAME_SUBMIT(vulkan);
FN_REZNOR_DISASSEMBLY(vulkan);
FN_REZNOR_COMMAND_DRAW(vulkan);
FN_REZNOR_COMMAND_DRAW_INDEXED(vulkan);
FN_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT(vulkan);
FN_REZNOR_COMMAND_DRAW_INDIRECT(vulkan);
FN_REZNOR_COMMAND_DISPATCH(vulkan);
FN_REZNOR_COMMAND_DISPATCH_INDIRECT(vulkan);
FN_REZNOR_COMMAND_COPY_BUFFER(vulkan);
FN_REZNOR_COMMAND_COPY_TEXTURE(vulkan);
FN_REZNOR_COMMAND_BEGIN_RENDER_PASS(vulkan);
FN_REZNOR_COMMAND_END_RENDER_PASS(vulkan);

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

/** Holds information about physical devices available. We query this once,
 *  and reference them whenever we create a rendering device. */
struct vulkan_physical_device {
    /** A handle of the physical device this structure holds information for. */
    VkPhysicalDevice            device;

    u32                         score;
    u32                         feats;
    u64                         extension_bits;
    usize                       total_vram;

    u32                         graphics_queue_family_index;
    u32                         compute_queue_family_index;
    u32                         transfer_queue_family_index;
    u32                         sparse_binding_queue_family_index;
    u32                         video_decode_queue_family_index;
    u32                         presentation_queue_family_index;

    /** Unique queue family indices present in this device. */
    u32                         queue_family_indices[5];
    /** How many unique queue families are present. */
    u32                         queue_family_count;

    u32                         graphics_queue_family_queue_count;
    u32                         async_compute_queue_family_queue_count;

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

struct reznor {
    struct reznor_interface                                     interface;
    /** An instance makes Vulkan functions available to us. It is used for calls to the driver,
     *  and holds information about the application. Afterwards it is passed to the logical device. */
    VkInstance                                                  instance;
    /** Used for logging messages and profiling with validation layers enabled. */
    VkDebugUtilsMessengerEXT                                    debug_messenger;
    /** Allocation callbacks passed into API calls. */
    VkAllocationCallbacks                                       allocator;
    /** The loaded driver library. */
    void                                                       *module;

    /** We query available physical devices at initialization. */
    const struct vulkan_physical_device                        *physical_devices;
    /** Available physical devices, this value won't change at runtime. */
    u32                                                         physical_device_count;
    /** Bits from instance-level extension query. */
    u32                                                         extension_bits;
    
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

struct vulkan_memory_allocator {
    u32 todo;
};

struct reznor_device {
    /** The public header of the rendering device. */
    struct reznor_device_header                                 header;
    /** The physical device used to create this rendering device. */
    const struct vulkan_physical_device                        *physical;
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                                    logical;
    /** Host allocation callbacks of this device. */
    VkAllocationCallbacks                                       host_allocator;
    /** GPU allocator. */
    struct vulkan_memory_allocator                              gpu_allocator;

    /* Command queues, they are created from the information about queue families stored in the physical device structure. */
    VkQueue                                                     graphics_queue;
    VkQueue                                                     compute_queue;
    VkQueue                                                     transfer_queue;
    VkQueue                                                     sparse_binding_queue;
    VkQueue                                                     video_decode_queue;
    VkQueue                                                     present_queue;

#if REZNOR_ENABLE_GPU_PROFILER
    at_u32                                                      device_memory_count;
    at_u32                                                      buffer_count;
    at_u32                                                      texture_count;
    at_u32                                                      sampler_count;
    at_u32                                                      pipeline_layout_count;
    at_u32                                                      graphics_pipeline_count;
    at_u32                                                      compute_pipeline_count;
    at_u32                                                      raytracing_pipeline_count;
    at_u32                                                      descriptor_set_layout_count;
    at_u32                                                      descriptor_pool_count;
    at_u32                                                      query_pool_count;
    at_u32                                                      swapchain_count;
#endif

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

struct reznor_device_memory {
    struct reznor_resource_header       header;
    u32                                 memory_type_index;
    VkDeviceMemory                      memory;
};

struct reznor_command_buffer {
    struct reznor_resource_header       header;
    VkCommandBuffer                     command_buffer;
};

struct reznor_buffer {
    struct reznor_resource_header       header;
    VkBuffer                            buffer;
    struct reznor_device_memory        *allocation;
};

struct reznor_texture {
    struct reznor_resource_header       header;
    VkImage                             image;
    VkImageView                         image_view;
    struct reznor_device_memory        *allocation;
};

struct reznor_sampler {
    struct reznor_resource_header       header;
    VkSampler                           sampler;
};

enum vulkan_descriptor_pool_type {
    vulkan_descriptor_pool_type_dynamic,
    vulkan_descriptor_pool_type_static,
};

struct vulkan_descriptor_pool {
    struct vulkan_descriptor_pool      *next;
    VkDescriptorPool                    pool;
    enum vulkan_descriptor_pool_type    type;
};

struct reznor_descriptor_set_layout {
    struct reznor_resource_header       header;
    VkDescriptorSetLayout               layout;
    struct reznor_sampler             **static_samplers;
    u32                                 static_samplers_size;
    u32                                 static_sampler_count;
};

struct reznor_descriptor_set {
    struct reznor_resource_header       header;
    VkDescriptorSet                     set;
};

struct reznor_pipeline_layout {
    struct reznor_resource_header       header;
    VkPipelineLayout                    pipeline_layout;
    b32                                 use_bindless_descriptors;
};

struct reznor_graphics_pipeline {
    struct reznor_resource_header       header;
    struct reznor_pipeline_layout      *layout;
    VkPipeline                          pipeline;
    b32                                 scissor_test_enabled;
};

struct reznor_compute_pipeline {
    struct reznor_resource_header       header;
    struct reznor_pipeline_layout      *layout;
    VkPipeline                          pipeline;
};

struct reznor_raytracing_pipeline {
    struct reznor_resource_header       header;
    struct reznor_pipeline_layout      *layout;
    VkPipeline                          pipeline;
};

struct reznor_shader_binding_table {
    struct reznor_resource_header       header;
    struct reznor_buffer_range          raygen_shader;
    struct reznor_buffer_range          miss_shader;
    struct reznor_buffer_range          hit_shader;
    struct reznor_buffer_range          callable_shader;
};

struct reznor_bottom_level {
    struct reznor_resource_header       header;
    /** The bottom-level acceleration structure (raytracing mesh). */
    VkAccelerationStructureKHR          acceleration_structure;
    /** The buffer that holds the acceleration structure. */
    struct reznor_buffer_range          buffer;
};

struct reznor_top_level {
    struct reznor_resource_header       header;
    /** The top-level acceleration structure (raytracing scene). */
    VkAccelerationStructureKHR          acceleration_structure;
    /** The buffer that holds the acceleration structure. */
    struct reznor_buffer_range          buffer;
};

struct reznor_query_pool {
    struct reznor_resource_header       header;
    VkQueryPool                         query_pool;
};

struct reznor_swapchain {
    struct reznor_resource_header       header;
    /** The window & display backend used to create a surface. */
    struct hadal_window                *window;
    /** The Vulkan object for a swapchain, created using the surface below. */
    VkSwapchainKHR                      swapchain;
    /** A surface created by interfacing with a display backend. */
    VkSurfaceKHR                        surface;
    /** Capabilities of the native surface and the physical device. */
    VkSurfaceCapabilitiesKHR            surface_capabilities;
    /** Selected mode of image presentation, when V-SYNC is disabled. */
    VkPresentModeKHR                    no_vsync_present_mode;
    /** The bit flag of the composite alpha. */
    VkCompositeAlphaFlagsKHR            composite_alpha;
    /** The resolution of images, in pixels. */
    VkExtent2D                          extent;
    /** Sharing mode that results from device queue families. */
    VkSharingMode                       sharing_mode;
    /** The intended use of the images. If transfer is set, screenshots are supported. */
    VkImageUsageFlags                   image_usage;
    /** The format of images held in the swapchain. */
    VkFormat                            image_format;
    /** An array of images held in the swapchain. */
    VkImage                             images[REZNOR_MAX_SWAPCHAIN_IMAGES];
    /** An image view for each image in the swapchain. */
    VkImageView                         image_views[REZNOR_MAX_SWAPCHAIN_IMAGES];
    /** Synchronization semaphores used for presentation, one per image. */
    VkSemaphore                         image_available_semaphores[REZNOR_MAX_FRAMES_IN_FLIGHT];
    /** The current index to the semaphores, cycled by frames_in_flight. */
    u32                                 image_available_semaphore_index;
    /** Number of images in the swapchain. */
    u32                                 image_count;
    /** If a non-zero value, will limit the framerate to a given interval. */
    u32                                 presentation_interval;
    /** Counts the last time an image was presented to the surface. */
    f64                                 last_presented_time;
};

struct vulkan_command_pool {
    VkCommandPool                       command_pool;
    VkCommandBuffer                    *command_buffers;
    u32                                 command_buffers_capacity;
    u32                                 command_buffers_count;
};

struct reznor_device_frame {
    struct reznor_device               *device;
    VkFence                             fence;
    /* one per thread */
    struct vulkan_command_pool         *graphics_command_pools;
    VkCommandBuffer                     graphics_command_buffer;
    VkSemaphore                         rendering_finished_semaphore;
    struct vulkan_descriptor_pool      *descriptor_pool;
    b32                                 is_running;
};

struct vulkan_memory_barrier {
    VkPipelineStageFlags                src_stage_mask;
    VkPipelineStageFlags                dst_stage_mask;
    VkMemoryBarrier                     barrier;
};

struct vulkan_image_memory_barrier {
    VkPipelineStageFlags                src_stage_mask;
    VkPipelineStageFlags                dst_stage_mask;
    VkImageMemoryBarrier                barrier;
};

extern void AMWCALL vulkan_write_allocation_callbacks(VkAllocationCallbacks *allocator, const char *userdata);
extern attr_const const char *AMWCALL vulkan_result_string(VkResult result);

#if !defined(NDEBUG)
    #define VERIFY_VK(x) { \
        VkResult res__ = (x); \
        if (res__ != VK_SUCCESS) { \
            log_error("Failed to assert VK_SUCCESS for: %s, Vulkan message: %s", #x, vulkan_result_string(res__)); \
            assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

extern b32 AMWCALL vulkan_query_extension(VkExtensionProperties *properties, u32 count, const char *ext);
extern b32 AMWCALL vulkan_load_instance_procedures(struct reznor *reznor, u32 api_version, u32 extension_bits);
extern b32 AMWCALL vulkan_load_device_procedures(struct reznor_device *device, u32 api_version, u64 extension_bits);

extern void AMWCALL vulkan_device_memory_destroy(struct reznor_device_memory *restrict device_memory);
extern void AMWCALL vulkan_buffer_destroy(struct reznor_buffer *restrict buffer);
extern void AMWCALL vulkan_texture_destroy(struct reznor_texture *restrict texture);
extern void AMWCALL vulkan_sampler_destroy(struct reznor_sampler *restrict sampler);
extern void AMWCALL vulkan_descriptor_set_layout_destroy(struct reznor_descriptor_set_layout *restrict descriptor_set_layout);
extern void AMWCALL vulkan_descriptor_set_destroy(struct reznor_descriptor_set *restrict descriptor_set);
extern void AMWCALL vulkan_pipeline_layout_destroy(struct reznor_pipeline_layout *restrict pipeline_layout);
extern void AMWCALL vulkan_graphics_pipeline_destroy(struct reznor_graphics_pipeline *restrict pipeline);
extern void AMWCALL vulkan_compute_pipeline_destroy(struct reznor_compute_pipeline *restrict pipeline);
extern void AMWCALL vulkan_raytracing_pipeline_destroy(struct reznor_raytracing_pipeline *restrict pipeline);
extern void AMWCALL vulkan_shader_binding_table_destroy(struct reznor_shader_binding_table *restrict shader_binding_table);
extern void AMWCALL vulkan_bottom_level_destroy(struct reznor_bottom_level *restrict bottom_level);
extern void AMWCALL vulkan_top_level_destroy(struct reznor_top_level *restrict top_level);
extern void AMWCALL vulkan_query_pool_destroy(struct reznor_query_pool *restrict query_pool);
extern void AMWCALL vulkan_swapchain_destroy(struct reznor_swapchain *restrict swapchain);

extern attr_const VkFormat AMWCALL vulkan_texture_format_translate(enum reznor_texture_format format);

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
