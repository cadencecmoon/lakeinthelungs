#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define XAKU_MAX_PUSH_CONSTANT_WORD_SIZE (32u)
#define XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE * 4)
#define XAKU_PIPELINE_LAYOUT_COUNT       (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE + 1)

/* opaque handles, implemented by the backend */
struct xaku_device;
struct xaku_memory;
struct xaku_query_pool;
struct xaku_swapchain;
struct xaku_compute_pipeline;
struct xaku_raster_pipeline;
struct xaku_ray_tracing_pipeline;
struct xaku_command_recorder;
struct xaku_executable_command_list;
struct xaku_encore;

/** Userdata argument for any Xaku encore. */
struct xaku_encore_assembly {
    u32 enable_debug_utils;
};

#ifdef XAKU_D3D12
LAKEAPI FN_RIVEN_ENCORE(xaku, d3d12);
#endif /* XAKU_D3D12 */
#ifdef XAKU_METAL
LAKEAPI FN_RIVEN_ENCORE(xaku, metal);
#endif /* XAKU_METAL */
#ifdef XAKU_WEBGPU
LAKEAPI FN_RIVEN_ENCORE(xaku, webgpu);
#endif /* XAKU_WEBGPU */
#ifdef XAKU_VULKAN
LAKEAPI FN_RIVEN_ENCORE(xaku, vulkan);
#endif /* XAKU_VULKAN */

/* A stub rendering backend, used for testing. */
LAKEAPI FN_RIVEN_ENCORE(xaku, null);

/** Returns an array of native encores, with a predefined priority. */
LAKEAPI lake_nonnull(2) const PFN_riven_encore *LAKECALL 
xaku_native_encores(bool null_fallback, u32 *out_encore_count);

/** Used to assemble an object that uses an internal reference counter: devices, swapchains, pipelines, etc. */
#define ARGS_XAKU_ASSEMBLY(SRC, T) \
    struct xaku_##SRC                *SRC, \
    const struct xaku_##T##_assembly *assembly, \
    struct xaku_##T                 **out_##T
#define PFN_XAKU_ASSEMBLY(SRC, T) \
    typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_##T##_assembly)(ARGS_XAKU_ASSEMBLY(SRC, T))
#define FN_XAKU_ASSEMBLY(ENCORE, SRC, T) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_##T##_assembly(ARGS_XAKU_ASSEMBLY(SRC, T))

/** Used as a callback for zero reference count, or to explicitly destroy an object.
 *  Can be run as a job with Riven, cast into PFN_riven_work. */
#define PFN_XAKU_DISASSEMBLY(T) \
    typedef void (LAKECALL *PFN_xaku_##T##_disassembly)(struct xaku_##T *T)
#define FN_XAKU_DISASSEMBLY(ENCORE, T) \
    void LAKECALL _xaku_##ENCORE##_##T##_disassembly(struct xaku_##T *T)

/** Must be implemented as the first member of a 'xaku_device' implementation. */
#define XAKU_INTERFACE_DEVICE_HEADER \
    /** The encore used to create this device. Does not modify the refcnt, as it's owned by Riven. */ \
    struct xaku_encore                     *encore; \
    /** The device may be destroyed when the reference count reaches zero. */ \
    atomic_u64                              refcnt; \
    /** A pointer to Riven for convenience. */ \
    struct riven                           *riven; \
    /** The lifetime of this device, used for non scratch allocations. */ \
    riven_tag_t                             riven_tag; \
    /** Index of the device, used within a mGPU context. */ \
    u32                                     index; \
    /** Acquired from an internal physical device, tied to the encore and not a device. */ \
    const struct xaku_device_properties    *properties; \
    /** The assembly details used to create this device. */ \
    struct xaku_device_assembly             assembly;


/** Must be implemented as the first member by the following opaque structures:
 *      xaku_device_memory, xaku_query_pool, xaku_swapchain, xaku_compute_pipeline, 
 *      xaku_raster_pipeline, xaku_ray_tracing_pipeline, xaku_command_recorder. */
#define XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(T) \
    /** Device that owns this resource. */ \
    struct xaku_device         *device; \
    /** The resource may be released when the reference count reaches zero. */ \
    atomic_u64                  refcnt; \
    /** The assembly details used to create this resource. */ \
    struct xaku_##T##_assembly  assembly;
    
/** Must be implemented as the first member by the following internal structures,
 *  that are accessed via a render handle:
 *      buffer, texture, texture_view, sampler, tlas, blas. */
#define XAKU_INTERFACE_GPU_RESOURCE_HEADER(T) \
    /** The assembly details used to create this resource. */ \
    struct xaku_##T##_assembly  assembly;

enum xaku_result {
    xaku_result_success                                                 = 0,
    xaku_result_not_ready                                               = 1,
    xaku_result_timeout                                                 = 2,
    xaku_result_event_set                                               = 3,
    xaku_result_event_reset                                             = 4,
    xaku_result_incomplete                                              = 5,
    xaku_result_error_out_of_host_memory                                = -1,
    xaku_result_error_out_of_device_memory                              = -2,
    xaku_result_error_initialization_failed                             = -3,
    xaku_result_error_device_lost                                       = -4,
    xaku_result_error_memory_map_failed                                 = -5,
    xaku_result_error_layer_not_present                                 = -6,
    xaku_result_error_extension_not_present                             = -7,
    xaku_result_error_feature_not_present                               = -8,
    xaku_result_error_incompatible_driver                               = -9,
    xaku_result_error_too_many_objects                                  = -10,
    xaku_result_error_format_not_supported                              = -11,
    xaku_result_error_fragmented_pool                                   = -12,
    xaku_result_error_unknown                                           = -13,
    xaku_result_error_out_of_pool_memory                                = -1000069000,
    xaku_result_error_invalid_external_handle                           = -1000072003,
    xaku_result_error_fragmentation                                     = -1000161000,
    xaku_result_error_invalid_opaque_capture_address                    = -1000257000,
    xaku_result_pipeline_compile_required                               = 1000297000,
    xaku_result_error_surface_lost_khr                                  = -1000000000,
    xaku_result_error_native_window_in_use_khr                          = -1000000001,
    xaku_result_suboptimal_khr                                          = 1000001003,
    xaku_result_error_out_of_date_khr                                   = -1000001004,
    xaku_result_error_incompatible_display_khr                          = -1000003001,
    xaku_result_error_validation_failed_ext                             = -1000011001,
    xaku_result_error_invalid_shader_nv                                 = -1000012000,
    xaku_result_error_image_usage_not_supported_khr                     = -1000023000,
    xaku_result_error_video_picture_layout_not_supported_khr            = -1000023001,
    xaku_result_error_video_profile_operation_not_supported_khr         = -1000023002,
    xaku_result_error_video_profile_format_not_supported_khr            = -1000023003,
    xaku_result_error_video_profile_codec_not_supported_khr             = -1000023004,
    xaku_result_error_video_std_version_not_supported_khr               = -1000023005,
    xaku_result_error_invalid_drm_format_modifier_plane_layout_ext      = -1000158000,
    xaku_result_error_not_permitted_khr                                 = -1000174001,
    xaku_result_error_full_screen_exclusive_mode_lost_ext               = -1000255000,
    xaku_result_thread_idle_khr                                         = 1000268000,
    xaku_result_thread_done_khr                                         = 1000268001,
    xaku_result_operation_deferred_khr                                  = 1000268002,
    xaku_result_operation_not_deferred_khr                              = 1000268003,
    xaku_result_missing_extension                                       = (1 << 30) + 0,
    xaku_result_invalid_buffer_id                                       = (1 << 30) + 1,
    xaku_result_invalid_image_id                                        = (1 << 30) + 2,
    xaku_result_invalid_image_view_id                                   = (1 << 30) + 3,
    xaku_result_invalid_sampler_id                                      = (1 << 30) + 4,
    xaku_result_buffer_double_free                                      = (1 << 30) + 5,
    xaku_result_image_double_free                                       = (1 << 30) + 6,
    xaku_result_image_view_double_free                                  = (1 << 30) + 7,
    xaku_result_sampler_double_free                                     = (1 << 30) + 8,
    xaku_result_invalid_buffer_info                                     = (1 << 30) + 9,
    xaku_result_invalid_image_info                                      = (1 << 30) + 10,
    xaku_result_invalid_image_view_info                                 = (1 << 30) + 11,
    xaku_result_invalid_sampler_info                                    = (1 << 30) + 12,
    xaku_result_command_list_completed                                  = (1 << 30) + 13,
    xaku_result_command_list_not_completed                              = (1 << 30) + 14,
    xaku_result_invalid_clear_value                                     = (1 << 30) + 15,
    xaku_result_buffer_not_host_visible                                 = (1 << 30) + 16,
    xaku_result_buffer_not_device_visible                               = (1 << 30) + 17,
    xaku_result_incomplete_command_list                                 = (1 << 30) + 18,
    xaku_result_device_does_not_support_buffer_count                    = (1 << 30) + 19,
    xaku_result_device_does_not_support_image_count                     = (1 << 30) + 20,
    xaku_result_device_does_not_support_sampler_count                   = (1 << 30) + 21,
    xaku_result_failed_to_create_null_buffer                            = (1 << 30) + 22,
    xaku_result_failed_to_create_null_image                             = (1 << 30) + 23,
    xaku_result_failed_to_create_null_image_view                        = (1 << 30) + 24,
    xaku_result_failed_to_create_null_sampler                           = (1 << 30) + 25,
    xaku_result_failed_to_create_buffer                                 = (1 << 30) + 26,
    xaku_result_failed_to_create_image                                  = (1 << 30) + 27,
    xaku_result_failed_to_create_image_view                             = (1 << 30) + 28,
    xaku_result_failed_to_create_default_image_view                     = (1 << 30) + 29,
    xaku_result_failed_to_create_sampler                                = (1 << 30) + 30,
    xaku_result_failed_to_create_bda_buffer                             = (1 << 30) + 31,
    xaku_result_failed_to_submit_device_init_commands                   = (1 << 30) + 32,
    xaku_result_invalid_buffer_range                                    = (1 << 30) + 33,
    xaku_result_invalid_buffer_offset                                   = (1 << 30) + 34,
    xaku_result_no_suitable_format_found                                = (1 << 30) + 36,
    xaku_result_range_out_of_bounds                                     = (1 << 30) + 37,
    xaku_result_no_suitable_device_found                                = (1 << 30) + 38,
    xaku_result_exceeded_max_buffers                                    = (1 << 30) + 39,
    xaku_result_exceeded_max_images                                     = (1 << 30) + 40,
    xaku_result_exceeded_max_image_views                                = (1 << 30) + 41,
    xaku_result_exceeded_max_samplers                                   = (1 << 30) + 42,
    xaku_result_device_surface_unsupported_present_mode                 = (1 << 30) + 43,
    xaku_result_command_references_invalid_buffer_id                    = (1 << 30) + 44,
    xaku_result_command_references_invalid_image_id                     = (1 << 30) + 45,
    xaku_result_command_references_invalid_image_view_id                = (1 << 30) + 46,
    xaku_result_command_references_invalid_sampler_id                   = (1 << 30) + 47,
    xaku_result_invalid_acceleration_structure_id                       = (1 << 30) + 48,
    xaku_result_exceeded_max_acceleration_structures                    = (1 << 30) + 49,
    xaku_result_device_does_not_support_raytracing                      = (1 << 30) + 50,
    xaku_result_device_does_not_support_mesh_shader                     = (1 << 30) + 51,
    xaku_result_invalid_tlas_id                                         = (1 << 30) + 52,
    xaku_result_invalid_blas_id                                         = (1 << 30) + 53,
    xaku_result_invalid_without_enabling_ray_tracing                    = (1 << 30) + 54,
    xaku_result_no_compute_pipeline_bound                               = (1 << 30) + 55,
    xaku_result_no_raster_pipeline_bound                                = (1 << 30) + 56,
    xaku_result_no_raytracing_pipeline_bound                            = (1 << 30) + 57,
    xaku_result_no_pipeline_bound                                       = (1 << 30) + 58,
    xaku_result_pushconstant_range_exceeded                             = (1 << 30) + 59,
    xaku_result_mesh_shader_not_device_enabled                          = (1 << 30) + 60,
    xaku_result_error_copy_out_of_bounds                                = (1 << 30) + 61,
    xaku_result_error_no_graphics_queue_found                           = (1 << 30) + 62,
    xaku_result_error_could_not_query_queue                             = (1 << 30) + 63,
    xaku_result_error_invalid_queue                                     = (1 << 30) + 64,
    xaku_result_error_cmd_list_submit_queue_family_mismatch             = (1 << 30) + 65,
    xaku_result_error_present_queue_family_mismatch                     = (1 << 30) + 66,
    xaku_result_error_invalid_queue_family                              = (1 << 30) + 67,
    xaku_result_error_invalid_device_index                              = (1 << 30) + 68,
    xaku_result_error_device_not_supported                              = (1 << 30) + 69,
    xaku_result_device_does_not_support_acceleration_structure_count    = (1 << 30) + 70,
    xaku_result_error_no_suitable_device_found                          = (1 << 30) + 71,
    xaku_result_error_compute_family_cmd_on_transfer_queue_recorder     = (1 << 30) + 72,
    xaku_result_error_main_family_cmd_on_transfer_queue_recorder        = (1 << 30) + 73,
    xaku_result_error_main_family_cmd_on_compute_queue_recorder         = (1 << 30) + 74,
    xaku_result_max_enum                                                = 0x7FFFFFFF,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
