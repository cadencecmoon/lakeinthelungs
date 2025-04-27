#pragma once

#include <amwe/xaku.h>
#ifdef XAKU_VULKAN

FN_XAKU_LIST_DEVICES_PROPERTIES(vulkan);
FN_XAKU_DEVICE_ASSEMBLY(vulkan);
FN_XAKU_DEVICE_DISASSEMBLY(vulkan);
FN_XAKU_DEVICE_QUEUE_COUNT(vulkan);
FN_XAKU_DEVICE_QUEUE_WAIT_IDLE(vulkan);
FN_XAKU_DEVICE_WAIT_IDLE(vulkan);
FN_XAKU_DEVICE_SUBMIT(vulkan);
FN_XAKU_DEVICE_PRESENT(vulkan);
FN_XAKU_DEFERRED_RESOURCE_DISASSEMBLY(vulkan);

FN_XAKU_MEMORY_ASSEMBLY(vulkan);
FN_XAKU_MEMORY_DISASSEMBLY(vulkan);
FN_XAKU_MEMORY_BUFFER_REQUIREMENTS(vulkan);
FN_XAKU_MEMORY_TEXTURE_REQUIREMENTS(vulkan);
FN_XAKU_QUERY_POOL_ASSEMBLY(vulkan);
FN_XAKU_QUERY_POOL_DISASSEMBLY(vulkan);
FN_XAKU_SWAPCHAIN_ASSEMBLY(vulkan);
FN_XAKU_SWAPCHAIN_DISASSEMBLY(vulkan);
FN_XAKU_COMPUTE_PIPELINE_ASSEMBLY(vulkan);
FN_XAKU_COMPUTE_PIPELINE_DISASSEMBLY(vulkan);
FN_XAKU_RASTER_PIPELINE_ASSEMBLY(vulkan);
FN_XAKU_RASTER_PIPELINE_DISASSEMBLY(vulkan);
FN_XAKU_RAY_TRACING_PIPELINE_ASSEMBLY(vulkan);
FN_XAKU_RAY_TRACING_PIPELINE_DISASSEMBLY(vulkan);
FN_XAKU_WORK_GRAPH_PIPELINE_ASSEMBLY(vulkan);
FN_XAKU_WORK_GRAPH_PIPELINE_DISASSEMBLY(vulkan);
FN_XAKU_COMMAND_RECORDER_ASSEMBLY(vulkan);
FN_XAKU_COMMAND_RECORDER_DISASSEMBLY(vulkan);

FN_XAKU_CREATE_BUFFER(vulkan);
FN_XAKU_CREATE_BUFFER_FROM_MEMORY(vulkan);
FN_XAKU_CREATE_TEXTURE(vulkan);
FN_XAKU_CREATE_TEXTURE_FROM_MEMORY(vulkan);
FN_XAKU_CREATE_TEXTURE_VIEW(vulkan);
FN_XAKU_CREATE_SAMPLER(vulkan);
FN_XAKU_CREATE_TLAS(vulkan);
FN_XAKU_CREATE_TLAS_FROM_BUFFER(vulkan);
FN_XAKU_CREATE_BLAS(vulkan);
FN_XAKU_CREATE_BLAS_FROM_BUFFER(vulkan);

FN_XAKU_GET_BUFFER_ASSEMBLY(vulkan);
FN_XAKU_GET_TEXTURE_ASSEMBLY(vulkan);
FN_XAKU_GET_TEXTURE_VIEW_ASSEMBLY(vulkan);
FN_XAKU_GET_SAMPLER_ASSEMBLY(vulkan);
FN_XAKU_GET_TLAS_BUILD_SIZES(vulkan);
FN_XAKU_GET_TLAS_ASSEMBLY(vulkan);
FN_XAKU_GET_BLAS_BUILD_SIZES(vulkan);
FN_XAKU_GET_BLAS_ASSEMBLY(vulkan);

FN_XAKU_DESTROY_BUFFER(vulkan);
FN_XAKU_DESTROY_TEXTURE(vulkan);
FN_XAKU_DESTROY_TEXTURE_VIEW(vulkan);
FN_XAKU_DESTROY_SAMPLER(vulkan);
FN_XAKU_DESTROY_TLAS(vulkan);
FN_XAKU_DESTROY_BLAS(vulkan);

FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(vulkan);
FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(vulkan);
FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(vulkan);
FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(vulkan);
FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(vulkan);
FN_XAKU_CMD_CLEAR_BUFFER(vulkan);
FN_XAKU_CMD_CLEAR_TEXTURE(vulkan);
FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(vulkan);
FN_XAKU_CMD_DESTROY_BUFFER_DEFERRED(vulkan);
FN_XAKU_CMD_DESTROY_TEXTURE_DEFERRED(vulkan);
FN_XAKU_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(vulkan);
FN_XAKU_CMD_DESTROY_SAMPLER_DEFERRED(vulkan);
FN_XAKU_CMD_DESTROY_TLAS_DEFERRED(vulkan);
FN_XAKU_CMD_DESTROY_BLAS_DEFERRED(vulkan);
FN_XAKU_CMD_PUSH_CONSTANT(vulkan);
FN_XAKU_CMD_SET_COMPUTE_PIPELINE(vulkan);
FN_XAKU_CMD_SET_RASTER_PIPELINE(vulkan);
FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(vulkan);
FN_XAKU_CMD_SET_WORK_GRAPH_PIPELINE(vulkan);
FN_XAKU_CMD_SET_VIEWPORT(vulkan);
FN_XAKU_CMD_SET_SCISSOR(vulkan);
FN_XAKU_CMD_SET_DEPTH_BIAS(vulkan);
FN_XAKU_CMD_SET_INDEX_BUFFER(vulkan);
FN_XAKU_CMD_BEGIN_RENDERING(vulkan);
FN_XAKU_CMD_END_RENDERING(vulkan);
FN_XAKU_CMD_DISPATCH(vulkan);
FN_XAKU_CMD_DISPATCH_INDIRECT(vulkan);
FN_XAKU_CMD_DISPATCH_GRAPH(vulkan);
FN_XAKU_CMD_DISPATCH_GRAPH_INDIRECT(vulkan);
FN_XAKU_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(vulkan);
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
FN_XAKU_COMPILE_COMMAND_LIST(vulkan);

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif
#ifndef VK_ENABLE_BETA_EXTENSIONS
    #define VK_ENABLE_BETA_EXTENSIONS
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    /* Pto avoid including windows.h */
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
#else
    #include <vulkan/vulkan.h>
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    #include <vulkan/vulkan_beta.h>
#endif

#include <vk_video/vulkan_video_codec_av1std_decode.h>
#include <vk_video/vulkan_video_codec_av1std_encode.h>
#include <vk_video/vulkan_video_codec_h264std_decode.h>
#include <vk_video/vulkan_video_codec_h264std_encode.h>
#include <vk_mem_alloc.h>

struct vulkan_queue_family {
    u32 queue_count;
    u32 vk_index;
};
#define VULKAN_QUEUE_FAMILY_INIT { .queue_count = 0, .index = ~0u }

/** Information about capabilities of accelerated video coding. */
struct vulkan_video_capability {
    VkVideoProfileInfoKHR                                           profile;
    VkVideoDecodeCapabilitiesKHR                                    decode_capabilities;
    VkVideoEncodeCapabilitiesKHR                                    encode_capabilities;
    VkVideoCapabilitiesKHR                                          capabilities;
};

/** Information about the physical device AV1 video codec. */
struct vulkan_video_av1 {
    VkVideoDecodeAV1ProfileInfoKHR                                  decode_profile;
    VkVideoDecodeAV1CapabilitiesKHR                                 decode_capabilities;
    VkVideoEncodeAV1ProfileInfoKHR                                  encode_profile;
    VkVideoEncodeAV1CapabilitiesKHR                                 encode_capabilities;
    struct vulkan_video_capability                                  video_capability;
};

/** Information about the physical device H.264 video codec. */
struct vulkan_video_h264 {
    VkVideoDecodeH264ProfileInfoKHR                                 decode_profile;
    VkVideoDecodeH264CapabilitiesKHR                                decode_capabilities;
    VkVideoEncodeH264ProfileInfoKHR                                 encode_profile;
    VkVideoEncodeH264CapabilitiesKHR                                encode_capabilities;
    struct vulkan_video_capability                                  video_capability;
};

/** Information about hardware properties of the physical device. */
struct vulkan_physical_device_properties {
    VkPhysicalDeviceShaderEnqueuePropertiesAMDX                     shader_enqueue;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR              acceleration_structure;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR                 ray_tracing_pipeline;
    VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV         ray_tracing_invocation_reorder;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR                fragment_shading_rate;
    VkPhysicalDeviceMeshShaderPropertiesEXT                         mesh_shader;
    VkPhysicalDeviceRobustness2PropertiesEXT                        robustness2;
    VkPhysicalDeviceDescriptorIndexingProperties                    descriptor_indexing;
    VkPhysicalDeviceSubgroupSizeControlProperties                   subgroup_size_control;
    VkPhysicalDeviceTimelineSemaphoreProperties                     timeline_semaphore;
    VkPhysicalDeviceMaintenance6Properties                          maintenance6;
    VkPhysicalDeviceMaintenance5Properties                          maintenance5;
    VkPhysicalDeviceMaintenance4Properties                          maintenance4;
    VkPhysicalDeviceProperties2                                     properties2;
    /* memory properties */
    VkPhysicalDeviceMemoryBudgetPropertiesEXT                       memory_budget;
    VkPhysicalDeviceMemoryProperties2                               memory_properties2;
};

/** Information about features supported by the physical device. */
struct vulkan_physical_device_features {
    VkPhysicalDeviceShaderEnqueueFeaturesAMDX                       shader_enqueue;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR                acceleration_structure;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR                   ray_tracing_pipeline;
    VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR              ray_tracing_position_fetch;
    VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV           ray_tracing_invocation_reorder;
    VkPhysicalDeviceRayQueryFeaturesKHR                             ray_query;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR                  fragment_shading_rate;
    VkPhysicalDeviceMeshShaderFeaturesEXT                           mesh_shader;
    VkPhysicalDeviceRobustness2FeaturesEXT                          robustness2;
    VkPhysicalDeviceDescriptorIndexingFeatures                      descriptor_indexing;
    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT               shader_image_atomic_int64;
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT                    shader_atomic_float;
    VkPhysicalDeviceShaderAtomicInt64Features                       shader_atomic_int64;
    VkPhysicalDeviceShaderFloat16Int8Features                       shader_float16_int8;
    VkPhysicalDeviceVulkanMemoryModelFeatures                       vulkan_memory_model;
    VkPhysicalDevice16BitStorageFeatures                            bit16_storage;
    VkPhysicalDevice8BitStorageFeatures                             bit8_storage;
    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT                extended_dynamic_state3;
    VkPhysicalDeviceVariablePointerFeatures                         variable_pointer;
    VkPhysicalDeviceHostQueryResetFeatures                          host_query_reset;
    VkPhysicalDeviceBufferDeviceAddressFeatures                     buffer_device_address;
    VkPhysicalDeviceScalarBlockLayoutFeatures                       scalar_block_layout;
    VkPhysicalDeviceSubgroupSizeControlFeatures                     subgroup_size_control;
    VkPhysicalDeviceMultiviewFeatures                               multiview;
    VkPhysicalDeviceTimelineSemaphoreFeatures                       timeline_semaphore;
    VkPhysicalDeviceSynchronization2Features                        synchronization2;
    VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT    dynamic_rendering_unused_attachments;
    VkPhysicalDeviceDynamicRenderingLocalReadFeatures               dynamic_rendering_local_read;
    VkPhysicalDeviceDynamicRenderingFeatures                        dynamic_rendering;
    VkPhysicalDeviceMaintenance6Features                            maintenance6;
    VkPhysicalDeviceMaintenance5Features                            maintenance5;
    VkPhysicalDeviceMaintenance4Features                            maintenance4;
    VkPhysicalDeviceFeatures2                                       features2;
    u8 has_swapchain, has_conservative_rasterization, has_video_capabilities, has_video_decode_queue, has_video_encode_queue;
};

/** Holds information about physical devices available. We query this once,
 *  and reference them whenever we create a rendering device. */
struct vulkan_physical_device {
    struct xaku_device_properties                                   xaku_properties;
    VkPhysicalDevice                                                vk_physical_device;
    u64                                                             vk_extension_bits;

    struct vulkan_queue_family                                      queue_families[xaku_queue_type_max_enum];
    u32                                                             valid_queue_family_indices[xaku_queue_type_max_enum];
    u32                                                             valid_queue_family_count;

    struct vulkan_physical_device_properties                        properties;
    struct vulkan_physical_device_features                          features;
    struct vulkan_video_av1                                         video_av1;
    struct vulkan_video_h264                                        video_h264;
};

struct xaku_device_memory { XAKU_INTERFACE_MEMORY_HEADER
    const char                         *name;
    VmaAllocation                       vma_allocation;
};

struct xaku_query_pool { XAKU_INTERFACE_QUERY_POOL_HEADER
    const char                         *name;
    VkQueryPool                         vk_query_pool;
};

/** Every frame we get a swapchain image index. This index can be non sequential in the case 
 *  of mail box presentation and other modes. This means we need to acquire a new index every 
 *  frame to know what swapchain image to use. Semaphores in acquire MUST be un-signaled when 
 *  recording actions ON THE CPU. Because of this, we need frames_in_flight+1 semaphores for 
 *  the acquire. And so, we use two types of semaphores here: acquire and present. 
 *
 *  The present semaphore is signaled in the last submission that uses the swapchain image 
 *  and waited on in the present. The acquire semaphore is signaled when the swapchain 
 *  image is ready to be used. This also means that the previous presentation of the image 
 *  is finished and the semaphore used in the present is un-signaled.
 *
 *  I don't know of any other way to know when a present finishes (or the corresponding 
 *  semaphore is un-signaled). In order to be able to reuse semaphores used in presentation,
 *  one MUST pair them with the image they are used to present.
 *
 *  One can then rely on the acquire semaphore of the image being signaled to indicate that 
 *  the present semaphore is able to be reused, as a swapchain images acquire semaphore is 
 *  signaled when it is available and its previous present has completed.
 *
 *  In order to reuse the acquire semaphore, we must set a limit in frames in flight and 
 *  wait on the cpu to limit the frames in flight. When we have this wait in place, we can 
 *  safely reuse the acquire semaphores with a linearly increasing index corresponding to 
 *  the frame. This means the acquire semaphores are not tied to the number of swapchain 
 *  images like present semaphores, but to the number of frames in flight.
 *
 *  To limit the frames in flight we may employ a timeline semaphore that must be signaled 
 *  in a submission that uses or after one that uses the swapchain image.
 *
 *  DISCLAIMER: The swapchain only works from the main queue. */
struct xaku_swapchain { XAKU_INTERFACE_SWAPCHAIN_HEADER
    const char                                 *name;
    VkSwapchainKHR                              vk_swapchain;
    /** A Vulkan surface created from a native window. */
    VkSurfaceKHR                                vk_surface;
    /** The selected surface format. */
    VkSurfaceFormatKHR                          surface_format;
    /** The window framebuffer resolution. */
    VkExtent2D                                  surface_extent;
    /** Abstracted presentation modes, acquired from the Vulkan surface. */
    lake_dynamic_array(enum xaku_present_mode)  supported_present_modes;
    /** Swapchain holds strong references to these objects as it owns them. */
    lake_dynamic_array(xaku_texture_id)         images;
    lake_dynamic_array(VkSemaphore)             acquire_semaphores;
    lake_dynamic_array(VkSemaphore)             present_semaphores;
    /** cpu_frame_timeline % frames in flight, used to index the acquire semaphores. */
    usize                                       acquire_semaphore_index;
    /** Monotonically increasing frame index. */
    usize                                       cpu_frame_timeline;
    /** Timeline semaphore used to track how far behind the GPU is. */
    VkSemaphore                                 gpu_frame_timeline;
    /** This presentation mode will be used if V-SYNC is disabled. */
    VkPresentModeKHR                            no_vsync_present_mode;
    /** This is the swapchain image index that acquire returns. This is not necessarily linear.
     *  This index must be used for present semaphores as they are paired to the images. */
    u32                                         current_image_index;
};

struct xaku_compute_pipeline { XAKU_INTERFACE_COMPUTE_PIPELINE_HEADER
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
};

struct xaku_raster_pipeline { XAKU_INTERFACE_RASTER_PIPELINE_HEADER
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
};

struct xaku_ray_tracing_pipeline { XAKU_INTERFACE_RAY_TRACING_PIPELINE_HEADER
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
    lake_span_to_const(struct xaku_ray_tracing_shader_group_params) shader_groups;
};

struct xaku_work_graph_pipeline { XAKU_INTERFACE_WORK_GRAPH_PIPELINE_HEADER
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
    lake_span_to_const(struct xaku_work_graph_node) nodes;
};

/** Used per queue family, to recycle command pools. */
struct vulkan_command_pool_arena {
    /* TODO */
};

struct vulkan_command_recorder_zombie {
    enum xaku_queue_type                        queue_family;
    VkCommandPool                               vk_cmd_pool;
};

struct vulkan_executable_command_list_data {
    VkCommandBuffer                                     vk_cmd_buffer;
    lake_dynamic_array(lake_pair(xaku_resource_id, u8)) deferred_disassembly;
    lake_dynamic_array(xaku_buffer_id)                  used_buffers;
    lake_dynamic_array(xaku_texture_id)                 used_textures;
    lake_dynamic_array(xaku_texture_view_id)            used_texture_views;
    lake_dynamic_array(xaku_sampler_id)                 used_samplers;
    lake_dynamic_array(xaku_tlas_id)                    used_tlass;
    lake_dynamic_array(xaku_blas_id)                    used_blass;
};

#define COMMAND_LIST_BARRIER_MAX_BATCH_SIZE 16
#define COMMAND_LIST_COLOR_ATTACHMENT_MAX 16

struct xaku_command_recorder { XAKU_INTERFACE_COMMAND_RECORDER_HEADER
    bool                                        in_renderpass;
    VkCommandPool                               vk_cmd_pool;
    lake_dynamic_array(VkCommandBuffer)         allocated_command_buffers;
    VkMemoryBarrier2                            memory_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    VkImageMemoryBarrier2                       image_barrier_batch[COMMAND_LIST_BARRIER_MAX_BATCH_SIZE];
    usize                                       memory_barrier_batch_count;
    usize                                       image_barrier_batch_count;
    usize                                       split_barrier_batch_count;
    union {
        struct {}                              *no_pipeline;
        struct xaku_compute_pipeline           *compute_pipeline;
        struct xaku_raster_pipeline            *raster_pipeline;
        struct xaku_ray_tracing_pipeline       *ray_tracing_pipeline;
    } current_pipeline;
    struct vulkan_executable_command_list_data  current_command_data;
};

struct xaku_executable_command_list {
    //XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(executable_command_list)
    u64                                         refcnt;
    struct xaku_command_recorder               *cmd;
    struct vulkan_executable_command_list_data  data;
};

/**< xaku_render_handle -> buffer */
struct vulkan_buffer_slot { XAKU_INTERFACE_BUFFER_HEADER
    VkBuffer                            vk_buffer;
    struct xaku_allocation              allocation;
    VkDeviceAddress                     device_address;
    void                               *host_address;
};

/**< xaku_render_handle -> texture_view */
struct vulkan_texture_view_slot { XAKU_INTERFACE_TEXTURE_VIEW_HEADER
    VkImageView                         vk_image_view;
};

/**< xaku_render_handle -> texture */
struct vulkan_texture_slot { XAKU_INTERFACE_TEXTURE_HEADER
    VkImage                             vk_image;
    struct vulkan_texture_view_slot     view_slot;
    struct xaku_allocation              allocation;
    s32                                 swapchain_image_index; /* default: VULKAN_IMAGE_NOT_OWNED_BY_SWAPCHAIN */
    VkImageAspectFlags                  aspect_flags;
};
#define VULKAN_IMAGE_NOT_OWNED_BY_SWAPCHAIN -1

/**< xaku_render_handle -> sampler */
struct vulkan_sampler_slot { XAKU_INTERFACE_SAMPLER_HEADER
    VkSampler                           vk_sampler;
};

/**< xaku_render_handle -> tlas */
struct vulkan_tlas_slot { XAKU_INTERFACE_TLAS_HEADER
    VkAccelerationStructureKHR          vk_acceleration_structure;
    VkBuffer                            vk_buffer;
    xaku_buffer_id                      buffer_handle;
    u64                                 offset;
    VkDeviceAddress                     device_address;
    bool                                owns_buffer;
};

/**< xaku_render_handle -> blas */
struct vulkan_blas_slot { XAKU_INTERFACE_BLAS_HEADER
    VkAccelerationStructureKHR          vk_acceleration_structure;
    VkBuffer                            vk_buffer;
    xaku_buffer_id                      buffer_handle;
    u64                                 offset;
    VkDeviceAddress                     device_address;
    bool                                owns_buffer;
};

struct vulkan_shader_resource_table {
    atomic_spinlock                 lifetime_lock;
    /* TODO
    struct vulkan_buffer_slot_pool  buffer_slots;
    struct vulkan_texture_slot_pool texture_slots;
    struct vulkan_sampler_slot_pool sampler_slots;
    struct vulkan_tlas_slot_pool    tlas_slots;
    struct vulkan_blas_slot_pool    blas_slots;
    */

    VkDescriptorSetLayout           descriptor_set_layout;
    VkDescriptorSet                 descriptor_set;
    VkDescriptorPool                descriptor_pool;

    /** Contains pipeline layouts with varying push constant range size.
     *  The first size is 0 word, second is 1 word, all other are a power of two.
     *  Maximum is XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE. */
    VkPipelineLayout                pipeline_layouts[XAKU_PIPELINE_LAYOUT_COUNT];
};

struct vulkan_queue {
    enum xaku_queue_type    family;
    u32                     queue_index;
    u32                     vk_queue_family_index;
    VkQueue                 vk_queue;
    VkSemaphore             local_timeline;
    atomic_u64              latest_pending_submit_timeline_value;
};

struct xaku_device {
    XAKU_INTERFACE_DEVICE_HEADER
    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                                    vk_device;
    /** The Vulkan physical device used to construct this rendering device. */
    const struct vulkan_physical_device                        *physical_details;
    /** Points into the allocation callbacks in encore. */
    const VkAllocationCallbacks                                *host_allocator;
    /** GPU allocator from VulkanMemoryAllocator library. */
    VmaAllocator                                                gpu_allocator;

    /** Device address buffer for a bindless descriptor set design. */
    VkBuffer                                                    device_address_buffer;
    u64                                                        *device_address_buffer_host;
    VmaAllocation                                               device_address_buffer_allocation;

    /** 'NULL' resources, used to fill empty slots in the resource table after a resource is destroyed.
     *  This is not necessary, as it is valid to have "garbage" in the descriptor slots given our 
     *  enabled Vulkan features. BUT, accessing garbage descriptors normally causes a device lost 
     *  immediately, making debugging much harder. So instead of leaving dead descriptors dangle, 
     *  we overwrite them with 'NULL' descriptors that just contain some debug value (e.g. pink 
     *  0xFF00FFFF). This in particular prevents device hang in the case of a use after free if 
     *  the device does not encounter a race condition on the descriptor update before. */
    VkBuffer                                                    null_buffer;
    VkImage                                                     null_image;
    VkImageView                                                 null_image_view;
    VkSampler                                                   null_sampler;
    VmaAllocation                                               null_buffer_allocation;
    VmaAllocation                                               null_image_allocation;

    /** Every submit to any queue increments the global submit timeline.
     *  Each queue stores a mapping between local submit index and global submit index for each 
     *  of their in flight submits. When destroying a resource it becomes a zombie, the zombie 
     *  remembers the current global timeline value. When deferred disassembly is called, the 
     *  zombies timeline values are compared against submits running in all queues. If the 
     *  zombies global submit index is smaller than the global index of all submits currently 
     *  in flight (on all queues), we can safely clean the resource up. */
    atomic_u64                                                  global_submit_timeline;
    atomic_spinlock                                             zombies_lock;
    /* TODO double ended queue of lake_pair ?? */
    lake_pair(u64, struct vulkan_command_recorder_zombie)       command_recorder_zombies;
    lake_pair(u64, xaku_buffer_id)                              buffer_zombies;
    lake_pair(u64, xaku_texture_id)                             texture_zombies;
    lake_pair(u64, xaku_texture_view_id)                        texture_view_zombies;
    lake_pair(u64, xaku_sampler_id)                             sampler_zombies;
    lake_pair(u64, xaku_tlas_id)                                tlas_zombies;
    lake_pair(u64, xaku_blas_id)                                blas_zombies;
    lake_pair(u64, VkEvent)                                     event_zombies;
    lake_pair(u64, VkSemaphore)                                 semaphore_zombies;
    lake_pair(u64, VkPipeline)                                  pipeline_zombies;
    lake_pair(u64, VkQueryPool)                                 query_pool_zombies;
    lake_pair(u64, VkDeviceMemory)                              memory_zombies;

    /** Command buffer/pool recycling, accessed via 'enum xaku_queue_type'. */
    struct vulkan_command_pool_arena command_pool_arenas[xaku_queue_type_max_enum];

    /** Command queues in order: main, compute (8), transfer (2), sparse binding, video decode, video encode */
    struct vulkan_queue queues[1 + XAKU_MAX_COMPUTE_QUEUE_COUNT + XAKU_MAX_TRANSFER_QUEUE_COUNT + 3];

    /** GPU shader resource object table. */
    struct vulkan_shader_resource_table shader_resource_table;

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
    PFN_vkCmdBeginRendering                                     vkCmdBeginRendering;        /* dynamic rendering */
    PFN_vkCmdEndRendering                                       vkCmdEndRendering;          /* dynamic rendering */
    PFN_vkCmdPipelineBarrier2                                   vkCmdPipelineBarrier2;      /* synchronization2 */
    PFN_vkCmdResetEvent2                                        vkCmdResetEvent2;           /* synchronization2 */
    PFN_vkCmdSetEvent2                                          vkCmdSetEvent2;             /* synchronization2 */
    PFN_vkCmdWaitEvents2                                        vkCmdWaitEvents2;           /* synchronization2 */
    PFN_vkCmdWriteTimestamp2                                    vkCmdWriteTimestamp2;       /* synchronization2 */
    PFN_vkQueueSubmit2                                          vkQueueSubmit2;             /* synchronization2 */
    PFN_vkGetDeviceBufferMemoryRequirements                     vkGetDeviceBufferMemoryRequirements;        /* maintenance4 */
    PFN_vkGetDeviceImageMemoryRequirements                      vkGetDeviceImageMemoryRequirements;         /* maintenance4 */
    PFN_vkGetDeviceImageSparseMemoryRequirements                vkGetDeviceImageSparseMemoryRequirements;   /* maintenance4 */

    /* core 1.4 */
    PFN_vkCmdBindDescriptorSets2                                vkCmdBindDescriptorSets2;                   /* maintenance6 */
    PFN_vkCmdBindIndexBuffer2                                   vkCmdBindIndexBuffer2;                      /* maintenance5 */
    PFN_vkCmdPushConstants2                                     vkCmdPushConstants2;                        /* maintenance6 */
    PFN_vkCmdSetRenderingAttachmentLocations                    vkCmdSetRenderingAttachmentLocations;       /* local read */
    PFN_vkCmdSetRenderingInputAttachmentIndices                 vkCmdSetRenderingInputAttachmentIndices;    /* local read */
    PFN_vkGetDeviceImageSubresourceLayout                       vkGetDeviceImageSubresourceLayout;          /* maintenance5 */
    PFN_vkGetImageSubresourceLayout2                            vkGetImageSubresourceLayout2;               /* maintenance5 */
    PFN_vkGetRenderingAreaGranularity                           vkGetRenderingAreaGranularity;              /* maintenance5 */

    /* swapchain */
    PFN_vkAcquireNextImageKHR                                   vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                                  vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                    vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                                   vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                                 vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                       vkQueuePresentKHR;

    /* device fault */
    PFN_vkGetDeviceFaultInfoEXT                                 vkGetDeviceFaultInfoEXT;

    /* dynamic state 3 */
    PFN_vkCmdSetRasterizationSamplesEXT                         vkCmdSetRasterizationSamplesEXT;

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
    PFN_vkCmdEncodeVideoKHR                                     vkCmdEncodeVideoKHR;
    PFN_vkCmdEndVideoCodingKHR                                  vkCmdEndVideoCodingKHR;
    PFN_vkCreateVideoSessionKHR                                 vkCreateVideoSessionKHR;
    PFN_vkCreateVideoSessionParametersKHR                       vkCreateVideoSessionParametersKHR;
    PFN_vkDestroyVideoSessionKHR                                vkDestroyVideoSessionKHR;
    PFN_vkDestroyVideoSessionParametersKHR                      vkDestroyVideoSessionParametersKHR;
    PFN_vkGetEncodedVideoSessionParametersKHR                   vkGetEncodedVideoSessionParametersKHR;
    PFN_vkGetVideoSessionMemoryRequirementsKHR                  vkGetVideoSessionMemoryRequirementsKHR;
    PFN_vkUpdateVideoSessionParametersKHR                       vkUpdateVideoSessionParametersKHR;

    /* work graph */
    PFN_vkCmdDispatchGraphAMDX                                  vkCmdDispatchGraphAMDX;
    PFN_vkCmdDispatchGraphIndirectAMDX                          vkCmdDispatchGraphIndirectAMDX;
    PFN_vkCmdDispatchGraphIndirectCountAMDX                     vkCmdDispatchGraphIndirectCountAMDX;
    PFN_vkCmdInitializeGraphScratchMemoryAMDX                   vkCmdInitializeGraphScratchMemoryAMDX;
    PFN_vkCreateExecutionGraphPipelinesAMDX                     vkCreateExecutionGraphPipelinesAMDX;
    PFN_vkGetExecutionGraphPipelineNodeIndexAMDX                vkGetExecutionGraphPipelineNodeIndexAMDX;
    PFN_vkGetExecutionGraphPipelineScratchSizeAMDX              vkGetExecutionGraphPipelineScratchSizeAMDX;
};

extern enum lake_result LAKECALL 
vulkan_device_assembly_helper(struct xaku_device *device, const struct xaku_device_assembly *assembly);

lake_force_inline bool vulkan_device_valid_queue(const struct xaku_device *device, struct xaku_queue queue) {
    return queue.type < xaku_queue_type_max_enum && queue.index < device->physical_details->queue_families[queue.type].queue_count;
}

extern struct vulkan_queue *LAKECALL vulkan_device_get_queue(struct xaku_device *device, struct xaku_queue queue);

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
    u32                                                         api_version;
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
    PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
};

extern lake_const const char *LAKECALL vulkan_result_string(VkResult result);
extern lake_const lake_hot enum lake_result LAKECALL vulkan_lake_result(VkResult result);

#if !defined(NDEBUG)
    #define VERIFY_VK(x) { \
        VkResult res__ = (x); \
        if (res__ != VK_SUCCESS) { \
            bedrock_log_error("Failed to assert VK_SUCCESS for: %s, Vulkan message: %s", #x, vulkan_result_string(res__)); \
            bedrock_assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

extern void LAKECALL 
vulkan_write_descriptor_set_sampler(
    VkDevice                    device, 
    VkDescriptorSet             descriptor_set, 
    VkSampler                   sampler, 
    u32                         index);

extern void LAKECALL 
vulkan_write_descriptor_set_buffer(
    VkDevice                    device, 
    VkDescriptorSet             descriptor_set, 
    VkBuffer                    buffer, 
    VkDeviceSize                offset, 
    VkDeviceSize                range, 
    u32                         index);

extern void LAKECALL 
vulkan_write_descriptor_set_image(
    VkDevice                    device, 
    VkDescriptorSet             descriptor_set, 
    VkImageView                 image_view, 
    VkImageUsageFlags           usage, 
    u32                         index);

extern void LAKECALL 
vulkan_write_descriptor_set_acceleration_structure(
    VkDevice                    device, 
    VkDescriptorSet             descriptor_set, 
    VkAccelerationStructureKHR  acceleration_structure, 
    u32                         index);

#endif /* XAKU_VULKAN */
