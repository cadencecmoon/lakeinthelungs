#include "vk_xaku.h"
#ifdef XAKU_VULKAN
    
enum extension_bits {
    extension_swapchain_bit                     = (1ull << 0),  /**< VK_KHR_swapchain */
    extension_device_fault_bit                  = (1ull << 1),  /**< VK_EXT_device_fault */
    extension_memory_budget_bit                 = (1ull << 2),  /**< VK_EXT_memory_budget */
    extension_memory_priority_bit               = (1ull << 3),  /**< VK_EXT_memory_priority */
    extension_mesh_shader_bit                   = (1ull << 4),  /**< VK_EXT_mesh_shader */
    extension_fragment_shading_rate_bit         = (1ull << 5),  /**< VK_KHR_fragment_shading_rate */
    extension_deferred_host_operations_bit      = (1ull << 6),  /**< VK_KHR_deferred_host_operations */
    extension_acceleration_structure_bit        = (1ull << 7),  /**< VK_KHR_acceleration_structure */
    extension_pipeline_library_bit              = (1ull << 8),  /**< VK_KHR_pipeline_library */
    extension_ray_query_bit                     = (1ull << 9),  /**< VK_KHR_ray_query */
    extension_ray_tracing_pipeline_bit          = (1ull << 10), /**< VK_KHR_ray_tracing_pipeline */
    extension_ray_tracing_maintenance1_bit      = (1ull << 11), /**< VK_KHR_ray_tracing_maintenance1 */
    extension_video_queue_bit                   = (1ull << 12), /**< VK_KHR_video_queue */
    extension_video_decode_queue_bit            = (1ull << 13), /**< VK_KHR_video_decode_queue */
    extension_video_decode_av1_bit              = (1ull << 14), /**< VK_KHR_video_decode_av1 */
    extension_video_decode_h264_bit             = (1ull << 15), /**< VK_KHR_video_decode_h264 */
    extension_video_maintenance1_bit            = (1ull << 16), /**< VK_KHR_video_maintenance1 */
    /* AMD hardware */
    extension_amd_device_coherent_memory_bit    = (1ull << 17), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    extension_dynamic_rendering_local_read_bit  = (1ull << 18), /**< VK_KHR_dynamic_rendering_local_read */
    extension_maintenance5_bit                  = (1ull << 19), /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    extension_dynamic_rendering_bit             = (1ull << 20), /**< VK_KHR_dynamic_rendering */
    extension_synchronization2_bit              = (1ull << 21), /**< VK_KHR_synchronization2 */
    extension_maintenance4_bit                  = (1ull << 22), /**< VK_KHR_maintenance4 */
};

#endif /* XAKU_VULKAN */
