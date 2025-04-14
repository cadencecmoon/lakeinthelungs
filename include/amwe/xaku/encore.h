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
struct xaku_commands_recorder;
struct xaku_executable_commands_list;
struct xaku_encore;

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

enum xaku_queue_type {
    xaku_queue_type_main             = (1u << 0),
    xaku_queue_type_compute          = (1u << 1),
    xaku_queue_type_transfer         = (1u << 2),
    xaku_queue_type_sparse_binding   = (1u << 3),
    xaku_queue_type_video            = (1u << 4),
    xaku_queue_type_video_decode     = (xaku_queue_type_video | (1u << 5)),
    xaku_queue_type_video_encode     = (xaku_queue_type_video | (1u << 6)),
};

struct xaku_viewport {
    f32 x, y, width, height, min_depth, max_depth;
};

struct xaku_offset2d {
    s32 x, y;
};

struct xaku_offset3d {
    s32 x, y, z;
};

struct xaku_extent2d {
    u32 width, height;
};

struct xaku_extent3d {
    u32 width, height, depth;
};

struct xaku_rect2d {
    struct xaku_offset2d offset;
    struct xaku_extent2d extent;
};

enum xaku_blend_op {
    xaku_blend_op_none = 0u,
    xaku_blend_op_add,
    xaku_blend_op_subtract,
    xaku_blend_op_rev_subtract,
    xaku_blend_op_min,
    xaku_blend_op_max,
};

enum xaku_blend_factor {
    xaku_blend_factor_zero = 0u,
    xaku_blend_factor_one,
    xaku_blend_factor_src_color,
    xaku_blend_factor_inv_src_color,
    xaku_blend_factor_src_alpha,
    xaku_blend_factor_inv_src_alpha,
    xaku_blend_factor_dest_alpha,
    xaku_blend_factor_inv_dest_alpha,
    xaku_blend_factor_dest_color,
    xaku_blend_factor_inv_dest_color,
    xaku_blend_factor_src_alpha_sat,
};

enum xaku_primitive_topology {
    xaku_primitive_topology_point_list = 0,
    xaku_primitive_topology_line_list,
    xaku_primitive_topology_line_strip,
    xaku_primitive_topology_triangle_list,
    xaku_primitive_topology_triangle_strip,
    xaku_primitive_topology_triangle_fan,
    xaku_primitive_topology_patch_list,
};

enum xaku_polygon_mode {
    xaku_polygon_mode_fill = 0,
    xaku_polygon_mode_line,
    xaku_polygon_mode_point,
};

enum xaku_index_format {
    xaku_index_format_invalid = 0u,
    xaku_index_format_u16,
    xaku_index_format_u32,
};

enum xaku_compare_op {
    xaku_compare_op_never = 0u,
    xaku_compare_op_less,
    xaku_compare_op_equal,
    xaku_compare_op_less_equal,
    xaku_compare_op_not_equal,
    xaku_compare_op_greater,
    xaku_compare_op_greater_equal,
    xaku_compare_op_always,
};

enum xaku_cull_mode {
    xaku_cull_mode_none = 0u,
    xaku_cull_mode_front,
    xaku_cull_mode_back,
};

/* counter clockwise, clockwise */
enum xaku_winding_order {
    xaku_winding_order_ccw = 0u,
    xaku_winding_order_cw,
};

enum xaku_load_op {
    xaku_load_op_load = 0u,
    xaku_load_op_clear,
    xaku_load_op_dont_care,
    xaku_load_op_none,
};

enum xaku_store_op {
    xaku_store_op_store = 0u,
    xaku_store_op_dont_care,
    xaku_store_op_none,
};

enum xaku_stencil_face {
    xaku_stencil_face_front = 0u,
    xaku_stencil_face_back,
};

enum xaku_stencil_op {
    xaku_stencil_op_keep = 0u,
    xaku_stencil_op_zero,
    xaku_stencil_op_replace,
    xaku_stencil_op_increment,
    xaku_stencil_op_decrement,
    xaku_stencil_op_increment_wrap,
    xaku_stencil_op_decrement_wrap,
    xaku_stencil_op_invert,
};

/** inspired by https://github.com/Tobski/simple_vulkan_synchronization */
enum xaku_access {
    xaku_access_general = 0u,                                                             /**< Covers any access - useful for debugging, avoid for performance reasons. */
    /* read access */
    xaku_access_indirect_buffer,                                                          /**< Read as an indirect buffer for drawing or dispatch. */
    xaku_access_index_buffer,                                                             /**< Read as an index buffer for drawing. */
    xaku_access_vertex_buffer,                                                            /**< Read as a vertex buffer for drawing. */
    xaku_access_vertex_shader_read_uniform_buffer,                                        /**< Read as a uniform buffer in a vertex shader. */
    xaku_access_vertex_shader_read_sampled_image_or_uniform_texel_buffer,                 /**< Read as a sampled image/uniform texel buffer in a vertex shader. */
    xaku_access_vertex_shader_read_other,                                                 /**< Read as any other resource in a vertex shader. */
    xaku_access_tesselation_control_shader_read_uniform_buffer,                           /**< Read as a uniform buffer in a tesselation control shader. */
    xaku_access_tesselation_control_shader_read_sampled_image_or_uniform_texel_buffer,    /**< Read as a sampled image/uniform texel buffer in a tesselation control shader. */
    xaku_access_tesselation_control_shader_read_other,                                    /**< Read as any other resource in a tesselation control shader. */
    xaku_access_tesselation_evaluation_shader_read_uniform_buffer,                        /**< Read as a uniform buffer in a tesselation evaluation shader. */
    xaku_access_tesselation_evaluation_shader_read_sampled_image_or_uniform_texel_buffer, /**< Read as a sampled image/uniform texel buffer in a tesselation evaluation shader. */
    xaku_access_tesselation_evaluation_shader_read_other,                                 /**< Read as any other resource in a tesselation evaluation shader. */
    xaku_access_geometry_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a geometry shader. */
    xaku_access_geometry_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a geometry shader. */
    xaku_access_geometry_shader_read_other,                                               /**< Read as any other resource in a geometry shader. */
    xaku_access_task_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a task shader. */
    xaku_access_task_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a task shader. */
    xaku_access_task_shader_read_other,                                                   /**< Read as any other resource in a task shader. */
    xaku_access_mesh_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a mesh shader. */
    xaku_access_mesh_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a mesh shader. */
    xaku_access_mesh_shader_read_other,                                                   /**< Read as any other resource in a mesh shader. */
    xaku_access_transform_feedback_counter_read,                                          /**< Read as a transform feedback counter buffer. */
    xaku_access_fragment_density_map_read,                                                /**< Read as a fragment density map image. */
    xaku_access_shading_rate_read,                                                        /**< Read as a shading rate image. */
    xaku_access_fragment_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a fragment shader. */
    xaku_access_fragment_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a fragment shader. */
    xaku_access_fragment_shader_read_color_input_attachment,                              /**< Read as an input attachment with a color format in a fragment shader. */
    xaku_access_fragment_shader_read_depth_stencil_input_attachment,                      /**< Read as an input attachment with a depth/stencil format in a fragment shader. */
    xaku_access_fragment_shader_read_other,                                               /**< Read as any other resource in a fragment shader. */
    xaku_access_color_attachment_read,                                                    /**< Read by standard blending/logic operations or local read/subpass load operations. */
    xaku_access_color_attachment_advanced_blending,                                       /**< Read by advanced blending, standard blending, logic operations or local read/subpass load operations. */
    xaku_access_depth_stencil_attachment_read,                                            /**< Read by depth/stencil tests or local read/subpass load operations. */
    xaku_access_compute_shader_read_uniform_buffer,                                       /**< Read as a uniform buffer in a compute shader. */
    xaku_access_compute_shader_read_sampled_image_or_uniform_texel_buffer,                /**< Read as a sampled image/uniform texel buffer in a compute shader. */
    xaku_access_compute_shader_read_other,                                                /**< Read as any other resource in a compute shader. */
    xaku_access_any_shader_read_uniform_buffer,                                           /**< Read as a uniform buffer in any shader. */
    xaku_access_any_shader_read_uniform_buffer_or_vertex_buffer,                          /**< Read as a uniform buffer in any shader, or a vertex buffer. */
    xaku_access_any_shader_read_sampled_image_or_uniform_texel_buffer,                    /**< Read as a sample image/uniform texel buffer in any shader. */
    xaku_access_any_shader_read_other,                                                    /**< Read as any other resource (excluding attachments) in any shader. */
    xaku_access_transfer_read,                                                            /**< Read as the source of a transfer operation. */
    xaku_access_host_read,                                                                /**< Read on the host. */
    xaku_access_present,                                                                  /**< Read by presentation engine (i.e. vkQueuePresentKHR). */
    xaku_access_conditional_rendering_read,                                               /**< Read by conditional rendering. */
    xaku_access_raytracing_shader_acceleration_structure_read,                            /**< Read by a raytracing shader as an acceleration structure. */
    xaku_access_acceleration_structure_build_read,                                        /**< Read as an acceleration structure during a build. */
    xaku_access_end_read,                                                                 /**< Read access end. */
    /* write access */
    xaku_access_command_buffer_write,                                                     /**< Command buffer write operation. */
    xaku_access_vertex_shader_write,                                                      /**< Written as any resource in a vertex shader. */
    xaku_access_tesselation_control_shader_write,                                         /**< Written as any resource in a tesselation control shader. */
    xaku_access_tesselation_evaluation_shader_write,                                      /**< Written as any resource in a tesselation evaluation shader. */
    xaku_access_geometry_shader_write,                                                    /**< Written as any resource in a geometry shader. */
    xaku_access_task_shader_write,                                                        /**< Written as any resource in a task shader. */
    xaku_access_mesh_shader_write,                                                        /**< Written as any resource in a mesh shader. */
    xaku_access_transform_feedback_write,                                                 /**< Written as a transform feedback buffer. */
    xaku_access_transform_feedback_counter_write,                                         /**< Written as a transform feedback counter buffer. */
    xaku_access_fragment_shader_write,                                                    /**< Written as any resource in a fragment shader, */
    xaku_access_color_attachment_write,                                                   /**< Written as a color attachment during rendering, or via a subpass store op. */
    xaku_access_depth_stencil_attachment_write,                                           /**< Written as a depth/stencil attachment during rendering, or via a subpass store op. */
    xaku_access_depth_attachment_write_stencil_read_only,                                 /**< Written as a depth aspect of a depth/stencil attachment during rendering, whilst the stencil aspect is read-only. */
    xaku_access_stencil_attachment_write_depth_read_only,                                 /**< Written as a stencil aspect of a depth/stencil attachment during rendering, whilst the depth aspect is read-only. */
    xaku_access_compute_shader_write,                                                     /**< Written as any resource in a compute shader. */
    xaku_access_any_shader_write,                                                         /**< Written as any resource in any shader. */
    xaku_access_transfer_write,                                                           /**< Written as the destination of a transfer operation. */
    xaku_access_host_preinitialized,                                                      /**< Data pre-filled by host before device access starts. */
    xaku_access_host_write,                                                               /**< Written on the host. */
    xaku_access_acceleration_structure_build_write,                                       /**< Written as an acceleration structure during a build. */
    xaku_access_color_attachment_read_write,                                              /**< Read or written as a color attachment during rendering. */
    xaku_access_end_write,                                                                /**< Write access end, access types count. */
};

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
enum xaku_format {
    xaku_format_unknown = 0u,
    xaku_format_r1_unorm,        /**< 1-bit no format */
    xaku_format_a8_unorm,        /**< 8-bit alpha */
    /* depth formats */
    xaku_format_d16_unorm,       /**< 16-bit unorm depth */
    xaku_format_d16s8_unorm,     /**< 24-bit unorm depth/stencil */
    xaku_format_d24s8_unorm,     /**< 32-bit unorm depth/stencil */
    xaku_format_d32_float,       /**< 32-bit float depth */
    xaku_format_d0s8_unorm,      /**< 8-bit unorm stencil */
    /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
    xaku_format_r8_snorm,
    xaku_format_r8_unorm,
    xaku_format_r8_sint,
    xaku_format_r8_uint,
    /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r16_float,
    xaku_format_r16_snorm,
    xaku_format_r16_unorm,
    xaku_format_r16_sint,
    xaku_format_r16_uint,
    xaku_format_r8g8_snorm,
    xaku_format_r8g8_unorm,
    xaku_format_r8g8_sint,
    xaku_format_r8g8_uint,
    /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r8g8b8_snorm,
    xaku_format_r8g8b8_unorm,
    xaku_format_r8g8b8_sint,
    xaku_format_r8g8b8_uint,
    /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r32_float,
    xaku_format_r32_sint,
    xaku_format_r32_uint,
    xaku_format_r16g16_float,
    xaku_format_r16g16_snorm,
    xaku_format_r16g16_unorm,
    xaku_format_r16g16_sint,
    xaku_format_r16g16_uint,
    xaku_format_r8g8b8a8_snorm,
    xaku_format_r8g8b8a8_unorm,
    xaku_format_r8g8b8a8_sint,
    xaku_format_r8g8b8a8_uint,
    xaku_format_b8g8r8a8_snorm,
    xaku_format_b8g8r8a8_unorm,
    xaku_format_b8g8r8a8_sint,
    xaku_format_b8g8r8a8_uint,
    xaku_format_r10g10b10a2,
    xaku_format_b10g10r10a2,
    /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r16g16b16_float,
    xaku_format_r16g16b16_snorm,
    xaku_format_r16g16b16_unorm,
    xaku_format_r16g16b16_sint,
    xaku_format_r16g16b16_uint,
    /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64_float,
    xaku_format_r64_sint,
    xaku_format_r64_uint,
    xaku_format_r32g32_float,
    xaku_format_r32g32_sint,
    xaku_format_r32g32_uint,
    xaku_format_r16g16b16a16_float,
    xaku_format_r16g16b16a16_snorm,
    xaku_format_r16g16b16a16_unorm,
    xaku_format_r16g16b16a16_sint,
    xaku_format_r16g16b16a16_uint,
    /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r32g32b32_float,
    xaku_format_r32g32b32_sint,
    xaku_format_r32g32b32_uint,
    /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r32g32b32a32_float,
    xaku_format_r32g32b32a32_sint,
    xaku_format_r32g32b32a32_uint,
    xaku_format_r64g64_float,
    xaku_format_r64g64_sint,
    xaku_format_r64g64_uint,
    /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64g64b64_float,
    xaku_format_r64g64b64_sint,
    xaku_format_r64g64b64_uint,
    /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64g64b64a64_float,
    xaku_format_r64g64b64a64_sint,
    xaku_format_r64g64b64a64_uint,
    /* compressed texture formats */
    xaku_format_BC1,         /**< DXT1 r5g6b5a1 */
    xaku_format_BC2,         /**< DXT3 r5g6b6a4 */
    xaku_format_BC3,         /**< DXT5 r5g6b5a8 */
    xaku_format_BC4,         /**< LATC1/ATI1 r8 */
    xaku_format_BC5,         /**< LATC2/ATI2 r8g8 */
    xaku_format_BC6H,        /**< BC6H r16g16b16_float */
    xaku_format_BC7,         /**< BC7 rgb 4-7 bits per color channel */
    xaku_format_ETC1,        /**< ETC1 r8g8b8 */
    xaku_format_ETC2,        /**< ETC2 r8g8b8 */
    xaku_format_ETC2A,       /**< ETC2 r8g8b8a8 */
    xaku_format_ETC2A1,      /**< ETC2 r8g8b8a1 */
    xaku_format_PTC12,       /**< PVRTC1 rgb 2BPP */
    xaku_format_PTC14,       /**< PVRTC1 rgb 4BPP */
    xaku_format_PTC22,       /**< PVRTC2 rgba 2BPP */
    xaku_format_PTC24,       /**< PVRTC2 rgba 4BPP */
    xaku_format_ASTC4x4,     /**< ASTC 4x4 8.0 BPP */
    xaku_format_ASTC5x4,     /**< ASTC 5x4 6.40 BPP */
    xaku_format_ASTC5x5,     /**< ASTC 5x5 5.12 BPP */
    xaku_format_ASTC6x5,     /**< ASTC 6x5 4.27 BPP */
    xaku_format_ASTC6x6,     /**< ASTC 6x6 3.56 BPP */
    xaku_format_ASTC8x5,     /**< ASTC 8x5 3.20 BPP */
    xaku_format_ASTC8x6,     /**< ASTC 8x6 2.67 BPP */
    xaku_format_ASTC8x8,     /**< ASTC 8x8 2.0 BPP */
    xaku_format_ASTC10x5,    /**< ASTC 10x5 2.56 BPP */
    xaku_format_ASTC10x6,    /**< ASTC 10x6 2.13 BPP */
    xaku_format_ASTC10x8,    /**< ASTC 10x8 1.60 BPP */
    xaku_format_ASTC10x10,   /**< ASTC 10x10 1.28 BPP */
    xaku_format_ASTC12x10,   /**< ASTC 12x10 1.07 BPP */
    xaku_format_ASTC12x12,   /**< ASTC 12x12 0.89 BPP */
    /* format count */
    xaku_format_count,
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
