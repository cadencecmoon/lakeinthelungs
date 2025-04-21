#pragma once

#include <amwe/renderer/xaku.h>
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

FN_XAKU_MEMORY_BUFFER_REQUIREMENTS(vulkan);
FN_XAKU_MEMORY_TEXTURE_REQUIREMENTS(vulkan);
FN_XAKU_MEMORY_ASSEMBLY(vulkan);
FN_XAKU_MEMORY_DISASSEMBLY(vulkan);

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

FN_XAKU_COMMAND_RECORDER_ASSEMBLY(vulkan);
FN_XAKU_COMMAND_RECORDER_DISASSEMBLY(vulkan);

FN_XAKU_CREATE_BUFFER(vulkan);
FN_XAKU_CREATE_BUFFER_FROM_MEMORY(vulkan);
FN_XAKU_CREATE_TEXTURE(vulkan);
FN_XAKU_CREATE_TEXTURE_FROM_MEMORY(vulkan);
FN_XAKU_CREATE_TEXTURE_VIEW(vulkan);
FN_XAKU_CREATE_SAMPLER(vulkan);
FN_XAKU_CREATE_TLAS(vulkan);
FN_XAKU_CREATE_BLAS(vulkan);
FN_XAKU_CREATE_TLAS_FROM_BUFFER(vulkan);
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
FN_XAKU_CMD_MESH_TASKS(vulkan);
FN_XAKU_CMD_MESH_TASKS_INDIRECT(vulkan);
FN_XAKU_CMD_MESH_TASKS_INDIRECT_COUNT(vulkan);
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
    #if defined(VK_ENABLE_BETA_EXTENSIONS)
        #include <vulkan/vulkan_beta.h>
    #endif
#else
    #include <vulkan/vulkan.h>
#endif

#include <vk_mem_alloc.h>

enum physical_device_extension_bits {
    physical_device_extension_khr_swapchain = 0llu,                 /**< VK_KHR_swapchain */
    physical_device_extension_ext_device_fault,                     /**< VK_EXT_device_fault */
    physical_device_extension_ext_memory_budget,                    /**< VK_EXT_memory_budget */
    physical_device_extension_ext_memory_priority,                  /**< VK_EXT_memory_priority */
    physical_device_extension_ext_mesh_shader,                      /**< VK_EXT_mesh_shader */
    physical_device_extension_khr_fragment_shading_rate,            /**< VK_KHR_fragment_shading_rate */
    physical_device_extension_khr_deferred_host_operations,         /**< VK_KHR_deferred_host_operations */
    physical_device_extension_khr_acceleration_structure,           /**< VK_KHR_acceleration_structure */
    physical_device_extension_khr_pipeline_library,                 /**< VK_KHR_pipeline_library */
    physical_device_extension_khr_ray_query,                        /**< VK_KHR_ray_query */
    physical_device_extension_khr_ray_tracing_pipeline,             /**< VK_KHR_ray_tracing_pipeline */
    physical_device_extension_khr_ray_tracing_maintenance1,         /**< VK_KHR_ray_tracing_maintenance1 */
    physical_device_extension_khr_ray_tracing_position_fetch,       /**< VK_KHR_ray_tracing_position_fetch */
    physical_device_extension_khr_video_queue,                      /**< VK_KHR_video_queue */
    physical_device_extension_khr_video_decode_queue,               /**< VK_KHR_video_decode_queue */
    physical_device_extension_khr_video_decode_av1,                 /**< VK_KHR_video_decode_av1 */
    physical_device_extension_khr_video_decode_h264,                /**< VK_KHR_video_decode_h264 */
    physical_device_extension_khr_video_maintenance1,               /**< VK_KHR_video_maintenance1 */
    physical_device_extension_ext_extended_dynamic_state3,          /**< VK_EXT_extended_dynamic_state3 */
    physical_device_extension_ext_robustness2,                      /**< VK_EXT_robustness2 */
    physical_device_extension_ext_shader_image_atomic_int64,        /**< VK_EXT_shader_image_atomic_int64 */
    physical_device_extension_ext_shader_atomic_float,              /**< VK_EXT_shader_atomic_float */
    physical_device_extension_ext_conservative_rasterization,       /**< VK_EXT_conservative_rasterization */
    /* NVIDIA hardware */
    physical_device_extension_nv_ray_tracing_invocation_reorder,    /**< VK_NV_ray_tracing_invocation_reorder */
    /* AMD hardware */
    physical_device_extension_amd_device_coherent_memory,           /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    physical_device_extension_khr_dynamic_rendering_local_read,     /**< VK_KHR_dynamic_rendering_local_read */
    physical_device_extension_khr_maintenance5,                     /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    physical_device_extension_khr_dynamic_rendering,                /**< VK_KHR_dynamic_rendering */
    physical_device_extension_khr_synchronization2,                 /**< VK_KHR_synchronization2 */
    physical_device_extension_khr_maintenance4,                     /**< VK_KHR_maintenance4 */
    /** if this exceeds 64, we may need to split the values holding extension bits */
    physical_device_extension_count,
};

struct vulkan_queue_family {
    u32 queue_count;
    u32 vk_index;
};
#define VULKAN_QUEUE_FAMILY_INIT { .queue_count = 0, .index = ~0u }

/** Holds information about physical devices available. We query this once,
 *  and reference them whenever we create a rendering device. */
struct vulkan_physical_device {
    struct xaku_device_properties                               xaku_properties;

    /** A handle of the physical device this structure holds information for. */
    VkPhysicalDevice                                            vk_physical_device;
    u64                                                         extension_bits;

    struct vulkan_queue_family                                  queue_families[xaku_queue_type_max_enum];
    /** Unique queue family indices present in this device. */
    u32                                                         valid_queue_family_indices[xaku_queue_type_max_enum];
    /** How many unique queue families are present. */
    u32                                                         valid_queue_family_count;

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

struct xaku_device_memory {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(memory)
    const char                         *name;
    VmaAllocation                       allocation;
};

struct xaku_query_pool {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(query_pool)
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
struct xaku_swapchain {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(swapchain)
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

struct xaku_compute_pipeline {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(compute_pipeline)
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
};

struct xaku_raster_pipeline {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(raster_pipeline)
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
};

struct xaku_ray_tracing_pipeline {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(ray_tracing_pipeline)
    VkPipeline                                  vk_pipeline;
    VkPipelineLayout                            vk_pipeline_layout;
    lake_dynamic_array(struct xaku_ray_tracing_shader_group_params) shader_groups;
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

struct xaku_command_recorder {
    XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(command_recorder)
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
struct vulkan_buffer_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(buffer)
    VkBuffer                            vk_buffer;
    struct xaku_allocation              allocation;
    VkDeviceAddress                     device_address;
    void                               *host_address;
};

/**< xaku_render_handle -> texture_view */
struct vulkan_texture_view_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(texture_view)
    VkImageView                         vk_image_view;
};

/**< xaku_render_handle -> texture */
struct vulkan_texture_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(texture)
    VkImage                             vk_image;
    struct vulkan_texture_view_slot     view_slot;
    struct xaku_allocation              allocation;
    s32                                 swapchain_image_index; /* default: VULKAN_IMAGE_NOT_OWNED_BY_SWAPCHAIN */
    VkImageAspectFlags                  aspect_flags;
};
#define VULKAN_IMAGE_NOT_OWNED_BY_SWAPCHAIN -1

/**< xaku_render_handle -> sampler */
struct vulkan_sampler_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(sampler)
    VkSampler                           vk_sampler;
};

/**< xaku_render_handle -> tlas */
struct vulkan_tlas_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(tlas)
    VkAccelerationStructureKHR          vk_acceleration_structure;
    VkBuffer                            vk_buffer;
    xaku_buffer_id                      buffer_handle;
    u64                                 offset;
    VkDeviceAddress                     device_address;
    bool                                owns_buffer;
};

/**< xaku_render_handle -> blas */
struct vulkan_blas_slot {
    XAKU_INTERFACE_GPU_RESOURCE_HEADER(blas)
    VkAccelerationStructureKHR          vk_acceleration_structure;
    VkBuffer                            vk_buffer;
    xaku_buffer_id                      buffer_handle;
    u64                                 offset;
    VkDeviceAddress                     device_address;
    bool                                owns_buffer;
};

XAKU_TEMPLATE_GPU_RESOURCE_POOL(vulkan, buffer_slot)
XAKU_TEMPLATE_GPU_RESOURCE_POOL(vulkan, texture_slot)
XAKU_TEMPLATE_GPU_RESOURCE_POOL(vulkan, sampler_slot)
XAKU_TEMPLATE_GPU_RESOURCE_POOL(vulkan, tlas_slot)
XAKU_TEMPLATE_GPU_RESOURCE_POOL(vulkan, blas_slot)

struct vulkan_shader_resource_table {
    atomic_spinlock                 lifetime_lock;
    struct vulkan_buffer_slot_pool  buffer_slots;
    struct vulkan_texture_slot_pool texture_slots;
    struct vulkan_sampler_slot_pool sampler_slots;
    struct vulkan_tlas_slot_pool    tlas_slots;
    struct vulkan_blas_slot_pool    blas_slots;

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
    VkDevice                                                    logical;
    /** The physical device for convenience. */
    VkPhysicalDevice                                            physical;
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

    /* dynamic state */
    PFN_vkCmdSetRasterizationSamplesEXT                         vkCmdSetRasterizationSamplesEXT;

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

extern void LAKECALL vulkan_device_zero_ref_callback(struct xaku_device *device);
extern void LAKECALL vulkan_device_memory_zero_ref_callback(struct xaku_device_memory *memory);
extern void LAKECALL vulkan_query_pool_zero_ref_callback(struct xaku_query_pool *query_pool);
extern void LAKECALL vulkan_swapchain_zero_ref_callback(struct xaku_swapchain *swapchain);
extern void LAKECALL vulkan_compute_pipeline_zero_ref_callback(struct xaku_compute_pipeline *pipeline);
extern void LAKECALL vulkan_raster_pipeline_zero_ref_callback(struct xaku_raster_pipeline *pipeline);
extern void LAKECALL vulkan_ray_tracing_pipeline_zero_ref_callback(struct xaku_ray_tracing_pipeline *pipeline);
extern void LAKECALL vulkan_command_recorder_zero_ref_callback(struct xaku_command_recorder *cmd);
extern void LAKECALL vulkan_executable_command_list_zero_ref_callback(struct xaku_executable_command_list *cmd_list);

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
