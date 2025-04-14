#pragma once

#include <amwe/xaku.h>
#ifdef XAKU_VULKAN

FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(vulkan);
FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(vulkan);
FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(vulkan);
FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(vulkan);
FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(vulkan);
FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan);
FN_XAKU_CMD_CLEAR_BUFFER(vulkan);
FN_XAKU_CMD_CLEAR_TEXTURE(vulkan);
FN_XAKU_CMD_DESTROY_DEFERRED(vulkan);
FN_XAKU_CMD_PUSH_CONSTANT(vulkan);
FN_XAKU_CMD_BARRIERS(vulkan);
FN_XAKU_CMD_TRANSITIONS(vulkan);
FN_XAKU_CMD_SET_COMPUTE_PIPELINE(vulkan);
FN_XAKU_CMD_SET_RASTER_PIPELINE(vulkan);
FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(vulkan);
FN_XAKU_CMD_SET_VIEWPORT(vulkan);
FN_XAKU_CMD_SET_SCISSOR(vulkan);
FN_XAKU_CMD_SET_DEPTH_BIAS(vulkan);
FN_XAKU_CMD_SET_INDEX_BUFFER(vulkan);
FN_XAKU_CMD_BEGIN_RENDERING(vulkan);
FN_XAKU_CMD_END_RENDERING(vulkan);
FN_XAKU_CMD_DISPATCH(vulkan);
FN_XAKU_CMD_DISPATCH_INDIRECT(vulkan);
FN_XAKU_CMD_DRAW(vulkan);
FN_XAKU_CMD_DRAW_INDEXED(vulkan);
FN_XAKU_CMD_DRAW_INDIRECT(vulkan);
FN_XAKU_CMD_DRAW_INDIRECT_COUNT(vulkan);
FN_XAKU_CMD_DRAW_MESH_TASKS(vulkan);
FN_XAKU_CMD_DRAW_MESH_TASKS_INDIRECT(vulkan);
FN_XAKU_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(vulkan);
FN_XAKU_CMD_TRACE_RAYS(vulkan);
FN_XAKU_CMD_TRACE_RAYS_INDIRECT(vulkan);
FN_XAKU_CMD_WRITE_TIMESTAMPS(vulkan);
FN_XAKU_CMD_RESET_TIMESTAMPS(vulkan);
FN_XAKU_CMD_BEGIN_LABEL(vulkan);
FN_XAKU_CMD_END_LABEL(vulkan);

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

/** Holds information about physical devices available. We query this once,
 *  and reference them whenever we create a rendering device. */
struct vulkan_physical_device {
    /** A handle of the physical device this structure holds information for. */
    VkPhysicalDevice            device;

    u32                         score;
    u32                         feats;
    u64                         extension_bits;

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

    bool                        has_async_compute;
    bool                        has_async_transfer;
    bool                        has_async_sparse_binding;
    bool                        has_async_video;
    bool                        has_async_present;

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

struct vulkan_allocation {
    VkDeviceMemory                                              memory;
    VkDeviceSize                                                offset;
    VkDeviceSize                                                size;
};

struct vulkan_gpu_allocator {

};

struct xaku_device {
    //struct xaku_device_header                                   header;
    /** A pointer to Riven for convenience. */
    struct riven                                               *riven;
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                                    logical;
    /** The Vulkan physical device used to construct this rendering device. */
    const struct vulkan_physical_device                        *physical_device;
    /** Points into the allocation callbacks in encore. */
    const VkAllocationCallbacks                                *host_allocator;
    /** A GPU memory allocator. */
    struct vulkan_gpu_allocator                                 gpu_allocator;

    /* Command queues, they are created from the information about queue families stored in the physical device structure. */
    VkQueue                                                     graphics_queue;
    VkQueue                                                     compute_queue;
    VkQueue                                                     transfer_queue;
    VkQueue                                                     sparse_binding_queue;
    VkQueue                                                     video_decode_queue;
    VkQueue                                                     present_queue;

#if XAKU_ENABLE_GPU_PROFILER
    atomic_u32                                                  device_memory_count;
    atomic_u32                                                  buffer_count;
    atomic_u32                                                  texture_count;
    atomic_u32                                                  sampler_count;
    atomic_u32                                                  pipeline_layout_count;
    atomic_u32                                                  graphics_pipeline_count;
    atomic_u32                                                  compute_pipeline_count;
    atomic_u32                                                  raytracing_pipeline_count;
    atomic_u32                                                  descriptor_set_layout_count;
    atomic_u32                                                  descriptor_pool_count;
    atomic_u32                                                  query_pool_count;
    atomic_u32                                                  swapchain_count;
#endif /* XAKU_ENABLE_GPU_PROFILER */

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

struct xaku_encore {
    struct xaku_interface                                       interface;
    /** An instance makes Vulkan functions available to us. It is used for calls to the driver,
     *  and holds information about the application. Afterwards it is passed to the logical device. */
    VkInstance                                                  instance;
    /** Used for logging messages and profiling with validation layers enabled. */
    VkDebugUtilsMessengerEXT                                    debug_messenger;
    /** Allocation callbacks passed into API calls. */
    VkAllocationCallbacks                                       allocator;

    struct vulkan_physical_device                              *physical_devices;
    u32                                                         physical_device_count;

    /** The loaded driver library. */
    void                                                       *module_vulkan;

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

#endif /* XAKU_VULKAN */
