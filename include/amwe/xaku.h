#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define XAKU_MAX_PUSH_CONSTANT_WORD_SIZE (32u)
#define XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE * 4)
#define XAKU_PIPELINE_LAYOUT_COUNT       (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE + 1)
#define XAKU_SHADER_UNUSED (~0u)

/** A rendering device is the context of execution, used to create resources and run commands on the GPU. */
struct xaku_device;
/** Represents a block of device memory (most often VRAM). It can be passed into creation of 
 *  different GPU resources to reuse allocated memory. */
struct xaku_memory;
/** Query pools are used to gather GPU execution statistics, like timestamps, occlusion queries,
 *  or pipelines stats - counting how many vertices, primitves, or fragments were processed, etc. 
 *  They are important for profiling, visibility queries and conditional rendering. */
struct xaku_query_pool;
/** The swapchain is a way to present rendered images into a window surface. This surface is owned by 
 *  the display backend, thus Hadal is interfaced to create a surface we can draw into. */
struct xaku_swapchain;
/** Defines the configuration of a pipeline for compute work. */
struct xaku_compute_pipeline;
/** Defines the configuration of a pipeline for rasterization. */
struct xaku_raster_pipeline;
/** Defines the configuration of a pipeline for accelerated ray tracing. */
struct xaku_ray_tracing_pipeline;
/** The context in which GPU commands are recorded for submission, externally synchronized. */
struct xaku_command_recorder;
/** A list of recorded GPU commands that can be submited for work. */
struct xaku_executable_command_list;
/* The interface implementation - backends like Vulkan, D3D12, Metal, etc. */
struct xaku_encore;

/** We represent GPU resources (buffers, textures, views, samplers and acceleration structures) as IDs.
 *  They are all accessed via a bindless API, and can represent the same resource on multiple devices. */
typedef struct { u64 value; } xaku_resource_id;
/* for static type checking */
typedef struct { u64 value; } xaku_buffer_id;
typedef struct { u64 value; } xaku_texture_id;
typedef struct { u64 value; } xaku_texture_view_id;
typedef struct { u64 value; } xaku_sampler_id;
typedef struct { u64 value; } xaku_tlas_id;
typedef struct { u64 value; } xaku_blas_id;

#define XAKU_ID_INDEX_MASK      ((1ull << 20) - 1)
#define XAKU_ID_VERSION_SHIFT   20

#define xaku_id_make(T, IDX, VER) \
    ((T){ .value = ((u64)(VER) << XAKU_ID_VERSION_SHIFT) | ((IDX) & XAKU_ID_INDEX_MASK) })
#define xaku_id_get_index(ID) \
    ((u32)((ID).value & XAKU_ID_INDEX_MASK))
#define xaku_id_get_version(ID) \
    ((u64)(((ID).value) >> XAKU_ID_VERSION_SHIFT))
#define xaku_id_set_index(ID, IDX) \
    ((ID).value = ((ID).value & ~XAKU_ID_INDEX_MASK) | ((IDX) & XAKU_ID_INDEX_MASK))
#define xaku_id_set_version(ID, VER) \
    ((ID).value = ((ID).value & XAKU_ID_INDEX_MASK) | ((u64)(VER) << XAKU_ID_VERSION_SHIFT))

#define XAKU_MAX_COMPUTE_QUEUE_COUNT            8
#define XAKU_MAX_TRANSFER_QUEUE_COUNT           2

#define XAKU_QUEUE_MAIN_INDEX                   0
#define XAKU_QUEUE_COMPUTE_BEGIN_INDEX          1
#define XAKU_QUEUE_TRANSFER_BEGIN_INDEX         (XAKU_MAX_COMPUTE_QUEUE_COUNT)
#define XAKU_QUEUE_SPARSE_BINDING_INDEX         (XAKU_QUEUE_TRANSFER_BEGIN_INDEX + XAKU_MAX_TRANSFER_QUEUE_COUNT)
#define XAKU_QUEUE_VIDEO_DECODE_INDEX           (XAKU_QUEUE_SPARSE_BINDING_INDEX + 1)
#define XAKU_QUEUE_VIDEO_ENCODE_INDEX           (XAKU_QUEUE_VIDEO_DECODE_INDEX + 1)

enum xaku_device_type {
    xaku_device_type_other = 0u,
    xaku_device_type_integrated_gpu,
    xaku_device_type_discrete_gpu,
    xaku_device_type_virtual_gpu,
    xaku_device_type_cpu,
};

enum xaku_queue_type {
    xaku_queue_type_main = 0u,
    xaku_queue_type_compute,
    xaku_queue_type_transfer,
    xaku_queue_type_sparse_binding,
    xaku_queue_type_video_decode,
    xaku_queue_type_video_encode,
    xaku_queue_type_max_enum,
};

#define XAKU_QUEUE_MAIN             {xaku_queue_type_main, 0}
#define XAKU_QUEUE_COMPUTE_0        {xaku_queue_type_compute, 0}
#define XAKU_QUEUE_COMPUTE_1        {xaku_queue_type_compute, 1}
#define XAKU_QUEUE_COMPUTE_2        {xaku_queue_type_compute, 2}
#define XAKU_QUEUE_COMPUTE_3        {xaku_queue_type_compute, 3}
#define XAKU_QUEUE_COMPUTE_4        {xaku_queue_type_compute, 4}
#define XAKU_QUEUE_COMPUTE_5        {xaku_queue_type_compute, 5}
#define XAKU_QUEUE_COMPUTE_6        {xaku_queue_type_compute, 6}
#define XAKU_QUEUE_COMPUTE_7        {xaku_queue_type_compute, 7}
#define XAKU_QUEUE_TRANSFER_0       {xaku_queue_type_transfer, 0}
#define XAKU_QUEUE_TRANSFER_1       {xaku_queue_type_transfer, 1}
#define XAKU_QUEUE_SPARSE_BINDING   {xaku_queue_type_sparse_binding, 0}
#define XAKU_QUEUE_VIDEO_DECODE     {xaku_queue_type_video_decode, 0}
#define XAKU_QUEUE_VIDEO_ENCODE     {xaku_queue_type_video_encode, 0}

struct xaku_queue {
    enum xaku_queue_type    type;
    u32                     index;
};

struct xaku_device_limits {
    u32 max_image_dimension1d;
    u32 max_image_dimension2d;
    u32 max_image_dimension3d;
    u32 max_image_dimension_cube;
    u32 max_image_array_layers;
    u32 max_texel_buffer_elements;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_push_constants_size;
    u32 max_memory_allocation_count;
    u32 max_sampler_allocation_count;
    u64 buffer_image_granularity;
    u64 sparse_address_space_size;
    u32 max_bound_descriptor_sets;
    u32 max_per_stage_descriptor_samplers;
    u32 max_per_stage_descriptor_uniform_buffers;
    u32 max_per_stage_descriptor_storage_buffers;
    u32 max_per_stage_descriptor_sampled_images;
    u32 max_per_stage_descriptor_storage_images;
    u32 max_per_stage_descriptor_input_attachments;
    u32 max_per_stage_resources;
    u32 max_descriptor_set_samplers;
    u32 max_descriptor_set_uniform_buffers;
    u32 max_descriptor_set_uniform_buffers_dynamic;
    u32 max_descriptor_set_storage_buffers;
    u32 max_descriptor_set_storage_buffers_dynamic;
    u32 max_descriptor_set_sampled_images;
    u32 max_descriptor_set_storage_images;
    u32 max_descriptor_set_input_attachments;
    u32 max_vertex_input_attributes;
    u32 max_vertex_input_bindings;
    u32 max_vertex_input_attribute_offset;
    u32 max_vertex_input_binding_stride;
    u32 max_vertex_output_components;
    u32 max_tessellation_generation_level;
    u32 max_tessellation_patch_size;
    u32 max_tessellation_control_per_vertex_input_components;
    u32 max_tessellation_control_per_vertex_output_components;
    u32 max_tessellation_control_per_patch_output_components;
    u32 max_tessellation_control_total_output_components;
    u32 max_tessellation_evaluation_input_components;
    u32 max_tessellation_evaluation_output_components;
    u32 max_geometry_shader_invocations;
    u32 max_geometry_input_components;
    u32 max_geometry_output_components;
    u32 max_geometry_output_vertices;
    u32 max_geometry_total_output_components;
    u32 max_fragment_input_components;
    u32 max_fragment_output_attachments;
    u32 max_fragment_dual_src_attachments;
    u32 max_fragment_combined_output_resources;
    u32 max_compute_shared_memory_size;
    u32 max_compute_work_group_count[3];
    u32 max_compute_work_group_invocations;
    u32 max_compute_work_group_size[3];
    u32 sub_pixel_precision_bits;
    u32 sub_texel_precision_bits;
    u32 mipmap_precision_bits;
    u32 max_draw_indexed_index_value;
    u32 max_draw_indirect_count;
    f32 max_sampler_lod_bias;
    f32 max_sampler_anisotropy;
    u32 max_viewports;
    u32 max_viewport_dimensions[2];
    f32 viewport_bounds_range[2];
    u32 viewport_sub_pixel_bits;
    usize min_memory_map_alignment;
    u64 min_texel_buffer_offset_alignment;
    u64 min_uniform_buffer_offset_alignment;
    u64 min_storage_buffer_offset_alignment;
    s32 min_texel_offset;
    u32 max_texel_offset;
    s32 min_texel_gather_offset;
    u32 max_texel_gather_offset;
    f32 min_interpolation_offset;
    f32 max_interpolation_offset;
    u32 sub_pixel_interpolation_offset_bits;
    u32 max_framebuffer_width;
    u32 max_framebuffer_height;
    u32 max_framebuffer_layers;
    u32 framebuffer_color_sample_counts;
    u32 framebuffer_depth_sample_counts;
    u32 framebuffer_stencil_sample_counts;
    u32 framebuffer_no_attachments_sample_counts;
    u32 max_color_attachments;
    u32 sampled_image_color_sample_counts;
    u32 sampled_image_integer_sample_counts;
    u32 sampled_image_depth_sample_counts;
    u32 sampled_image_stencil_sample_counts;
    u32 storage_image_sample_counts;
    u32 max_sample_mask_words;
    u32 timestamp_compute_and_graphics;
    f32 timestamp_period;
    u32 max_clip_distances;
    u32 max_cull_distances;
    u32 max_combined_clip_and_cull_distances;
    u32 discrete_queue_priorities;
    f32 point_size_range[2];
    f32 line_width_range[2];
    f32 point_size_granularity;
    f32 line_width_granularity;
    u32 strict_lines;
    u32 standard_sample_locations;
    u64 optimal_buffer_copy_offset_alignment;
    u64 optimal_buffer_copy_row_pitch_alignment;
    u64 non_coherent_atom_size;
};

struct xaku_ray_tracing_pipeline_properties {
    u32 shader_group_handle_size;
    u32 max_ray_recursion_depth;
    u32 max_shader_group_stride;
    u32 shader_group_base_alignment;
    u32 shader_group_handle_capture_replay_size;
    u32 max_ray_dispatch_invocation_count;
    u32 shader_group_handle_alignment;
    u32 max_ray_hit_attribute_size;
};

struct xaku_ray_tracing_invocation_reorder_properties {
    u32 invocation_reorder_mode;
}; 

struct xaku_acceleration_structure_properties {
    u64 max_geometry_count;
    u64 max_instance_count;
    u64 max_primitive_count;
    u32 max_per_stage_descriptor_acceleration_structures;
    u32 max_per_stage_descriptor_update_after_bind_acceleration_structures;
    u32 max_descriptor_set_acceleration_structures;
    u32 max_descriptor_set_update_after_bind_acceleration_structures;
    u32 min_acceleration_structure_scratch_offset_alignment;
};

struct xaku_mesh_shader_properties {
    u32 max_task_work_group_total_count;
    u32 max_task_work_group_count[3];
    u32 max_task_work_group_invocations;
    u32 max_task_work_group_size[3];
    u32 max_task_payload_size;
    u32 max_task_shared_memory_size;
    u32 max_task_payload_and_shared_memory_size;
    u32 max_mesh_work_group_total_count;
    u32 max_mesh_work_group_count[3];
    u32 max_mesh_work_group_invocations;
    u32 max_mesh_work_group_size[3];
    u32 max_mesh_shared_memory_size;
    u32 max_mesh_payload_and_shared_memory_size;
    u32 max_mesh_output_memory_size;
    u32 max_mesh_payload_and_output_memory_size;
    u32 max_mesh_output_components;
    u32 max_mesh_output_vertices;
    u32 max_mesh_output_primitives;
    u32 max_mesh_output_layers;
    u32 max_mesh_multiview_view_count;
    u32 mesh_output_per_vertex_granularity;
    u32 mesh_output_per_primitive_granularity;
    u32 max_preferred_task_work_group_invocations;
    u32 max_preferred_mesh_work_group_invocations;
    bool prefers_local_invocation_vertex_output;
    bool prefers_local_invocation_primitive_output;
    bool prefers_compact_vertex_output;
    bool prefers_compact_primitive_output;
};

enum xaku_explicit_feature_bits {
    xaku_explicit_feature_none                                                          = 0u,
    xaku_explicit_feature_buffer_device_address_capture_replay                          = (1u << 0),
    xaku_explicit_feature_acceleration_structure_capture_replay                         = (1u << 1),
    xaku_explicit_feature_vulkan_memory_model                                           = (1u << 2),
    xaku_explicit_feature_robustness2                                                   = (1u << 3),
    xaku_explicit_feature_video_decode_av1                                              = (1u << 4),
    xaku_explicit_feature_video_decode_h264                                             = (1u << 5),
};
typedef u32 xaku_explicit_features;

enum xaku_implicit_feature_bits {
    xaku_implicit_feature_none                                                          = 0u,
    xaku_implicit_feature_mesh_shader                                                   = (1u << 0),
    xaku_implicit_feature_basic_ray_tracing                                             = (1u << 1),
    xaku_implicit_feature_ray_tracing_pipeline                                          = (1u << 2),
    xaku_implicit_feature_ray_tracing_invocation_reorder                                = (1u << 3),
    xaku_implicit_feature_ray_tracing_position_fetch                                    = (1u << 4),
    xaku_implicit_feature_conservative_rasterization                                    = (1u << 5),
    xaku_implicit_feature_image_atomic64                                                = (1u << 6),
    xaku_implicit_feature_shader_atomic_float                                           = (1u << 7),
    xaku_implicit_feature_shader_atomic_int64                                           = (1u << 8),
    xaku_implicit_feature_shader_float16                                                = (1u << 9),
    xaku_implicit_feature_shader_int16                                                  = (1u << 10),
    xaku_implicit_feature_shader_int8                                                   = (1u << 11),
    xaku_implicit_feature_swapchain                                                     = (1u << 12),
    xaku_implicit_feature_khr_dynamic_state3                                            = (1u << 13),
};
typedef u32 xaku_implicit_features;

struct xaku_device_properties {
    u32                                                     api_version;
    u32                                                     driver_version;
    u32                                                     vendor_id;
    u32                                                     device_id;
    enum xaku_device_type                                   device_type;
    char                                                    device_name[256u];
    char                                                    pipeline_cache_uuid[16u];
    bool                                                    has_mesh_shader_properties;
    bool                                                    has_ray_tracing_pipeline_properties;
    bool                                                    has_ray_tracing_invocation_reoder_properties;
    bool                                                    has_acceleration_structure_properties;
    struct xaku_mesh_shader_properties                      mesh_shader_properties;
    struct xaku_ray_tracing_pipeline_properties             ray_tracing_pipeline_properties;
    struct xaku_ray_tracing_invocation_reorder_properties   ray_tracing_invocation_reorder_properties;
    struct xaku_acceleration_structure_properties           acceleration_structure_properties;
    struct xaku_device_limits                               limits;
    u32                                                     compute_queue_count;
    u32                                                     transfer_queue_count;
    xaku_implicit_features                                  implicit_features;
    xaku_explicit_features                                  explicit_features;
    u64                                                     total_score;
}; 

struct xaku_device_assembly {
    /** Index into a list of devices returned from PFN_xaku_encore_list_devices_properties. */
    u32                                                     physical_device_index;
    /** Explicit features must be manually enabled. */
    xaku_explicit_features                                  explicit_features;
    u32                                                     max_allowed_textures;
    u32                                                     max_allowed_buffers;
    u32                                                     max_allowed_samplers;
    u32                                                     max_allowed_acceleration_structures;
    struct lake_allocation                                  host_allocation;
    lake_small_string                                       name;
};
#define XAKU_DEVICE_ASSEMBLY_INIT { \
    .physical_device_index = ~0u, \
    .explicit_features = xaku_explicit_feature_buffer_device_address_capture_replay, \
    .max_allowed_textures = 10000, \
    .max_allowed_buffers = 10000, \
    .max_allowed_samplers = 400, \
    .max_allowed_acceleration_structures = 10000, \
    .name = LAKE_ZERO_INIT, \
}

enum xaku_memory_flag_bits {
    xaku_memory_flag_none                           = 0,
    xaku_memory_flag_dedicated_memory               = (1u << 0),
    xaku_memory_flag_can_alias                      = (1u << 1),
    xaku_memory_flag_host_access_sequential_write   = (1u << 2),
    xaku_memory_flag_host_access_random             = (1u << 3),
    xaku_memory_flag_strategy_min_memory            = (1u << 4),
    xaku_memory_flag_strategy_min_time              = (1u << 5),
};
typedef u32 xaku_memory_flags;

struct xaku_memory_requirements {
    u64 size;
    u64 alignment;
    u32 memory_type_bits; /* TODO */
};

struct xaku_memory_assembly {
    struct xaku_memory_requirements requirements;
    xaku_memory_flags               flags;
};
#define XAKU_MEMORY_ASSEMBLY_INIT LAKE_ZERO_INIT

struct xaku_allocation {
    struct xaku_memory             *memory;
    u64                             offset;
    u64                             range;
};

struct xaku_query_pool_assembly {
    u32                             query_count;
    lake_small_string               name;
};
#define XAKU_QUERY_POOL_ASSEMBLY_INIT LAKE_ZERO_INIT

enum xaku_color_component_bits {
    xaku_color_component_r = (1u << 0),
    xaku_color_component_g = (1u << 1),
    xaku_color_component_b = (1u << 2),
    xaku_color_component_a = (1u << 3),
};
typedef u32 xaku_color_components;

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

enum xaku_sharing_mode {
    xaku_sharing_mode_exclusive = 0,
    xaku_sharing_mode_concurrent,
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

union xaku_clear_color_value {
    f32 vec[4];
    s32 sint[4];
    u32 uint[4];
};

struct xaku_clear_depth_stencil_value {
    f32 depth;
    u32 stencil;
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
    xaku_format_r8g8b8a8_srgb,
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

struct xaku_memory_barrier {
    enum xaku_access                            src_access;
    enum xaku_access                            dst_access;
};

struct xaku_buffer_assembly {
    usize                                       size;
    /* ignored when allocating with a memory block */
    xaku_memory_flags                           memory_flags;
    lake_small_string                           name;
};
#define XAKU_BUFFER_ASSEMBLY_INIT { \
    .size = 0lu, \
    .allocate_flags = xaku_memory_flag_none, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_buffer_memory_assembly {
    struct xaku_buffer_assembly                 buffer_assembly;
    struct xaku_device_memory                  *memory;
    usize                                       offset;
};
#define XAKU_BUFFER_MEMORY_ASSEMBLY_INIT LAKE_ZERO_INIT

enum xaku_texture_flag_bits {
    xaku_texture_flag_none                              = 0,
    xaku_texture_flag_allow_mutable_format              = (1u << 0),
    xaku_texture_flag_compatible_cube                   = (1u << 1),
    xaku_texture_flag_compatible_2d_array               = (1u << 2),
    xaku_texture_flag_allow_alias                       = (1u << 3),
};
typedef u32 xaku_texture_flags;

enum xaku_texture_usage_bits {
    xaku_texture_usage_none                             = 0,
    xaku_texture_usage_transfer_src                     = (1u << 0),
    xaku_texture_usage_transfer_dst                     = (1u << 1),
    xaku_texture_usage_shader_sampled                   = (1u << 2),
    xaku_texture_usage_shader_storage                   = (1u << 3),
    xaku_texture_usage_color_attachment                 = (1u << 4),
    xaku_texture_usage_depth_stencil_attachment         = (1u << 5),
    xaku_texture_usage_transient_attachment             = (1u << 6),
    xaku_texture_usage_fragment_density_map             = (1u << 7),
    xaku_texture_usage_fragment_shading_rate_attachment = (1u << 8),
};
typedef u32 xaku_texture_usages;

enum xaku_texture_layout {
    xaku_texture_layout_undefined = 0,
    xaku_texture_layout_general,
    xaku_texture_layout_transfer_src_optimal,
    xaku_texture_layout_transfer_dst_optimal,
    xaku_texture_layout_read_only_optimal,
    xaku_texture_layout_attachment_optimal,
    xaku_texture_layout_present_src,
};

enum xaku_texture_aspect {
    xaku_texture_aspect_none        = (1u << 0),
    xaku_texture_aspect_color       = (1u << 1),
    xaku_texture_aspect_depth       = (1u << 2),
    xaku_texture_aspect_stencil     = (1u << 3),
    xaku_texture_aspect_metadata    = (1u << 4),
    xaku_texture_aspect_plane_0     = (1u << 5),
    xaku_texture_aspect_plane_1     = (1u << 6),
    xaku_texture_aspect_plane_2     = (1u << 7),
};
typedef u32 xaku_texture_aspects;

struct xaku_texture_mip_array_slice {
    u32                                         base_mip_level;
    u32                                         level_count;
    u32                                         base_array_layer;
    u32                                         layer_count;
};

struct xaku_texture_array_slice {
    u32                                         mip_level;
    u32                                         base_array_layer;
    u32                                         layer_count;
};

struct xaku_texture_slice {
    u32                                         mip_level;
    u32                                         array_layer;
};

struct xaku_texture_assembly {
    xaku_texture_flags                          flags;
    u32                                         dimensions;
    enum xaku_format                            format;
    struct lake_extent3d                        extent;
    u32                                         mip_level_count;
    u32                                         array_layer_count;
    u32                                         sample_count;
    xaku_texture_usages                         usage;
    enum xaku_sharing_mode                      sharing_mode;
    /* ignored when allocating with a memory block */
    xaku_memory_flags                           memory_flags;
    lake_small_string                           name;
};
#define XAKU_TEXTURE_ASSEMBLY_INIT { \
    .flags = xaku_texture_flag_none, \
    .dimensions = 2, \
    .format = xaku_format_r8g8b8a8_srgb, \
    .extent = {0u, 0u, 0u}, \
    .mip_level_count = 1, \
    .array_layer_count = 1, \
    .sample_count = 1, \
    .usage = xaku_texture_usage_none, \
    .sharing_mode = xaku_sharing_mode_concurrent, \
    .allocate_flags = xaku_memory_flag_none, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_texture_memory_assembly {
    struct xaku_texture_assembly                texture_assembly;
    struct xaku_device_memory                  *memory;
    usize                                       offset;
};
#define XAKU_TEXTURE_MEMORY_ASSEMBLY_INIT LAKE_ZERO_INIT

struct xaku_texture_memory_barrier {
    enum xaku_access                            src_access;
    enum xaku_access                            dst_access;
    enum xaku_texture_layout                    src_layout;
    enum xaku_texture_layout                    dst_layout;
    struct xaku_texture_mip_array_slice         texture_slice;
    xaku_texture_id                             texture;
};

enum xaku_texture_view_type {
    xaku_texture_view_type_1d = 0,
    xaku_texture_view_type_2d,
    xaku_texture_view_type_3d,
    xaku_texture_view_type_cube,
    xaku_texture_view_type_1d_array,
    xaku_texture_view_type_2d_array,
    xaku_texture_view_type_cube_array,
};

enum xaku_resolve_mode_bits {
    xaku_resolve_mode_none                          = 0,
    xaku_resolve_mode_sample_zero                   = (1u << 0),
    xaku_resolve_mode_average                       = (1u << 1),
    xaku_resolve_mode_min                           = (1u << 2),
    xaku_resolve_mode_max                           = (1u << 3),
    xaku_resolve_mode_external_downsample_android   = (1u << 4),
};
typedef u32 xaku_resolve_modes;

struct xaku_texture_view_assembly {
    enum xaku_texture_view_type                 type;
    enum xaku_format                            format;
    xaku_texture_id                             texture;
    struct xaku_texture_mip_array_slice         slice;
    lake_small_string                           name;
};
#define XAKU_TEXTURE_VIEW_ASSEMBLY_INIT { \
    .type = xaku_texture_view_type_2d, \
    .format = xaku_format_r8g8b8a8_srgb, \
    .texture = LAKE_ZERO_INIT, \
    .slice = LAKE_ZERO_INIT, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

enum xaku_filter_mode {
    xaku_filter_mode_nearest = 0u,
    xaku_filter_mode_linear,
};

enum xaku_border_color {
    xaku_border_color_float_transparent_black = 0u,
    xaku_border_color_int_transparent_black,
    xaku_border_color_float_opaque_black,
    xaku_border_color_int_opaque_black,
    xaku_border_color_float_opaque_white,
    xaku_border_color_int_opaque_white,
};

enum xaku_sampler_address_mode {
    xaku_sampler_address_mode_wrap = 0u,
    xaku_sampler_address_mode_mirror,
    xaku_sampler_address_mode_clamp_edge,
    xaku_sampler_address_mode_clamp_border,
};

enum xaku_sampler_reduction_mode {
    xaku_sampler_reduction_mode_weighted_average = 0u,
    xaku_sampler_reduction_mode_min,
    xaku_sampler_reduction_mode_max,
};

enum xaku_sample_count_bits {
    xaku_sample_count_1     = (1u << 0),   
    xaku_sample_count_2     = (1u << 1),   
    xaku_sample_count_4     = (1u << 2),   
    xaku_sample_count_8     = (1u << 3),   
    xaku_sample_count_16    = (1u << 4),   
    xaku_sample_count_32    = (1u << 5),   
    xaku_sample_count_64    = (1u << 6),   
};
typedef u32 xaku_sample_counts;

struct xaku_sampler_assembly {
    enum xaku_filter_mode                       magnification_filter;
    enum xaku_filter_mode                       minification_filter;
    enum xaku_filter_mode                       mipmap_filter;
    enum xaku_sampler_reduction_mode            reduction_mode;
    enum xaku_sampler_address_mode              address_mode_u;
    enum xaku_sampler_address_mode              address_mode_v;
    enum xaku_sampler_address_mode              address_mode_w;
    f32                                         mip_lod_bias;
    f32                                         max_anisotrophy;
    enum xaku_compare_op                        compare_op;
    f32                                         min_lod;
    f32                                         max_lod;
    enum xaku_border_color                      border_color;
    bool                                        enable_anisotrophy;
    bool                                        enable_compare_op;
    bool                                        enable_unnormalized_coordinates;
    lake_small_string                           name;
};
#define XAKU_SAMPLER_ASSEMBLY_INIT { \
    .magnification_filter = xaku_filter_mode_linear, \
    .minification_filter = xaku_filter_mode_linear, \
    .mipmap_filter = xaku_filter_mode_linear, \
    .reduction_mode = xaku_sampler_reduction_mode_weighted_average, \
    .address_mode_u = xaku_sampler_address_mode_clamp_edge, \
    .address_mode_v = xaku_sampler_address_mode_clamp_edge, \
    .address_mode_w = xaku_sampler_address_mode_clamp_edge, \
    .mip_lod_bias = 0.5f, \
    .max_anisotrophy = 0.0f, \
    .compare_op = xaku_compare_op_always, \
    .min_lod = 0.0f, \
    .max_lod = 1000.0f, \
    .border_color = xaku_border_color_int_transparent_black, \
    .enable_anisotrophy = false, \
    .enable_compare_op = false, \
    .enable_unnormalized_coordinates = false, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_sampled_image_context {
    xaku_texture_view_id                        texture_view;
    xaku_sampler_id                             sampler;
};

enum xaku_present_mode {
    xaku_present_mode_immediate = 0u,
    xaku_present_mode_mailbox,
    xaku_present_mode_fifo,
    xaku_present_mode_fifo_relaxed,
};

enum xaku_present_transform_bits {
    xaku_present_transform_identity             = (1u << 0),
    xaku_present_transform_rotate_90            = (1u << 1),
    xaku_present_transform_rotate_180           = (1u << 2),
    xaku_present_transform_rotate_270           = (1u << 3),
    xaku_present_transform_mirror               = (1u << 4),
    xaku_present_transform_mirror_rotate_90     = (1u << 5),
    xaku_present_transform_mirror_rotate_180    = (1u << 6),
    xaku_present_transform_mirror_rotate_270    = (1u << 7),
    xaku_present_transform_inherit              = (1u << 8),
};
typedef u32 xaku_present_transforms;

struct xaku_swapchain_assembly {
    /** A view into a native window, obtained from Hadal. */
    struct hadal_window                        *native_window;
    /** The preferred present mode will be selected if available. */
    enum xaku_present_mode                      present_mode;
    xaku_present_transforms                     present_transform;
    xaku_texture_usages                         image_usage;
    /** Timeline semaphores will be cycled by this value. */
    u32                                         max_allowed_frames_in_flight;
    /** This queue family may be used for presentation. */
    enum xaku_queue_type                        queue_type;
    /** Debug name of the swapchain. */
    lake_small_string                           name;
};
#define XAKU_DEFAULT_SWAPCHAIN_ASSEMBLY { \
    .native_window = LAKE_ZERO_INIT, \
    .present_mode = xaku_present_mode_mailbox, \
    .present_transform = xaku_present_transform_identity, \
    .image_usage = xaku_texture_usage_color_attachment, \
    .max_allowed_frames_in_flight = ~0, \
    .queue_type = xaku_queue_type_main, \
    .name = LAKE_ZERO_INIT, \
}

struct xaku_strided_address_region {
    u64                                         device_address;
    u64                                         stride;
    u64                                         size;
};

struct xaku_shader_binding_table {
    struct xaku_strided_address_region          raygen_region;
    struct xaku_strided_address_region          miss_region;
    struct xaku_strided_address_region          hit_region;
    struct xaku_strided_address_region          callable_region;
};

union xaku_clear_value {
    union xaku_clear_color_value                color;
    struct xaku_clear_depth_stencil_value       depth_stencil;
};

enum xaku_geometry_flag_bits {
    xaku_geometry_flag_opaque                           = (1u << 0),
    xaku_geometry_flag_no_duplicate_any_hit_invocation  = (1u << 1),
    xaku_geometry_flag_data_is_array_of_pointers        = (1u << 2),
};
typedef u32 xaku_geometry_flags;

struct xaku_blas_triangle_geometry {
    enum xaku_format                            vertex_format;
    u64                                         vertex_data;
    u64                                         vertex_stride;
    u32                                         max_vertex;
    enum xaku_index_format                      index_format;
    u64                                         index_data;
    u64                                         transform_data;
    u32                                         count;
    xaku_geometry_flags                         geometry_flags;
};
#define XAKU_BLAS_TRIANGLE_GEOMETRY_INIT { \
    .vertex_format = xaku_format_r32g32_float, \
    .vertex_data = LAKE_ZERO_INIT, \
    .vertex_stride = 24, \
    .max_vertex = 0, \
    .index_format = xaku_index_format_u32, \
    .index_data = LAKE_ZERO_INIT, \
    .transform_data = LAKE_ZERO_INIT, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

struct xaku_blas_aabb_geometry {
    u64                                         data;   /**< device address */
    u64                                         stride;
    u32                                         count;
    xaku_geometry_flags                         geometry_flags;
};
#define XAKU_BLAS_AABB_GEOMETRY_INIT { \
    .data = LAKE_ZERO_INIT, \
    .stride = 24, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

struct xaku_blas_triangle_geometry_span {
    const struct xaku_blas_triangle_geometry   *triangles;
    usize                                       count;
};

struct xaku_blas_aabb_geometry_span {
    const struct xaku_blas_aabb_geometry       *aabbs;
    usize                                       count;
};

union xaku_blas_geometry_span {
    struct xaku_blas_triangle_geometry_span     triangle;
    struct xaku_blas_aabb_geometry_span         aabb;
};

struct xaku_tlas_instance {
    u64                                         data;   /**< device address */
    u32                                         count;
    xaku_geometry_flags                         geometry_flags;
};
#define XAKU_TLAS_INSTANCE_INIT { \
    .data = LAKE_ZERO_INIT, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

enum xaku_acceleration_structure_create_flag_bits {
    xaku_acceleration_structure_create_flag_allow_update      = (1u << 0),
    xaku_acceleration_structure_create_flag_allow_compaction  = (1u << 1),
    xaku_acceleration_structure_create_flag_prefer_fast_trace = (1u << 2),
    xaku_acceleration_structure_create_flag_prefer_fast_build = (1u << 3),
    xaku_acceleration_structure_create_flag_low_memory        = (1u << 4),
};
typedef u32 xaku_acceleration_structure_create_flags;

struct xaku_acceleration_structure_build_sizes {
    u64 acceleration_structure_size;
    u64 update_scratch_size;
    u64 build_scratch_size;
};

struct xaku_tlas_assembly {
    xaku_acceleration_structure_create_flags    create_flags;
    bool                                        update;
    xaku_tlas_id                                src_tlas;
    xaku_tlas_id                                dst_tlas;
    const struct xaku_tlas_instance            *instances;
    u32                                         instance_count;
    u64                                         scratch_data; /**< device address */
};
#define XAKU_TLAS_ASSEMBLY_INIT { \
    .flags = xaku_acceleration_structure_assembly_flag_prefer_fast_trace, \
    .update = false, \
    .src_tlas = LAKE_ZERO_INIT, \
    .dst_tlas = LAKE_ZERO_INIT, \
    .instances = LAKE_ZERO_INIT, \
    .instance_count = LAKE_ZERO_INIT, \
    .scratch_data = LAKE_ZERO_INIT, \
}

struct xaku_tlas_buffer_assembly {
    struct xaku_tlas_assembly                   tlas_assembly;
    xaku_buffer_id                              buffer;
    u64                                         offset;
};
#define XAKU_TLAS_BUFFER_ASSEMBLY_INIT LAKE_ZERO_INIT

struct xaku_blas_assembly {
    xaku_acceleration_structure_create_flags    create_flags;
    bool                                        update;
    xaku_blas_id                                src_blas;
    xaku_blas_id                                dst_blas;
    union xaku_blas_geometry_span               geometries;
    u64                                         scratch_data; /**< device address */
};
#define XAKU_BLAS_ASSEMBLY_INIT { \
    .flags = xaku_acceleration_structure_assembly_flag_prefer_fast_trace, \
    .src_blas = LAKE_ZERO_INIT, \
    .dst_blas = LAKE_ZERO_INIT, \
    .geometries = LAKE_ZERO_INIT, \
    .scratch_data = LAKE_ZERO_INIT, \
}

struct xaku_blas_buffer_assembly {
    struct xaku_blas_assembly                   blas_assembly;
    xaku_buffer_id                              buffer;
    u64                                         offset;
};
#define XAKU_BLAS_BUFFER_ASSEMBLY_INIT LAKE_ZERO_INIT

/** For now only Slang is supported: https://shader-slang.org/ */
enum xaku_shader_language {
    xaku_shader_language_slang,
    xaku_shader_language_max_enum = 0x7fffffff,
};

struct xaku_shader_source {
    const char *path; /* TODO filesystem path */ 
};

struct xaku_shader_define {
    const char *name;
    const char *value;
};

struct xaku_shader_model {
    u32 major, minor;
};

enum xaku_shader_create_flag_bits {
    xaku_shader_create_flag_none                        = 0u,
    xaku_shader_create_flag_allow_varying_subgroup_size = (1u << 0),
    xaku_shader_create_flag_require_full_subgroups      = (1u << 1),
};
typedef u32 xaku_shader_create_flags;

struct xaku_shader_params {
    const u32                                      *byte_code;
    u32                                             byte_code_size;
    xaku_shader_create_flags                        create_flags;
    lake_optional(u32)                              required_subgroup_size;
    lake_small_string                               entry_point;
};
struct xaku_shader_compile_params {
    struct xaku_shader_source                       source;
    lake_optional(const char *)                     entry_point;
    enum xaku_shader_language                       language;
    xaku_shader_create_flags                        create_flags;
    lake_dynamic_array(struct xaku_shader_define)   defines;
    lake_optional(u32)                              required_subgroup_size;
    bool                                            enable_debug_utils;
};

struct xaku_compute_pipeline_assembly {
    struct xaku_shader_params                       shader;
    u32                                             push_constant_size;
    lake_small_string                               name;
};
#define XAKU_COMPUTE_PIPELINE_ASSEMBLY_INIT { \
    .shader = LAKE_ZERO_INIT, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

struct xaku_compute_pipeline_compile_assembly {
    struct xaku_shader_source                       source;
    lake_optional(const char *)                     entry_point;
    enum xaku_shader_language                       language;
    xaku_shader_create_flags                        create_flags;
    lake_dynamic_array(struct xaku_shader_define)   defines;
    u32                                             push_constant_size;
    lake_optional(u32)                              required_subgroup_size;
    bool                                            enable_debug_utils;
};
#define XAKU_COMPUTE_PIPELINE_COMPILE_ASSEMBLY_INIT { \
    .source = LAKE_ZERO_INIT, \
    .entry_point = NULL, \
    .defines = LAKE_ZERO_INIT, \
    .create_flags = 0, \
    .language = xaku_shader_language_slang, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .required_subgroup_size = 0, \
    .has_required_subgroup_size = false, \
    .has_entry_point = true, \
    .enable_debug_utils = false, \
}

struct xaku_depth_test_params {
    enum xaku_format                                depth_attachment_format;
    bool                                            enable_depth_write;
    enum xaku_compare_op                            depth_test_compare_op;
    f32                                             min_depth_bounds;
    f32                                             max_depth_bounds;
};
#define XAKU_DEPTH_TEST_PARAMS_INIT { \
    .depth_attachment_format = xaku_format_undefined, \
    .enable_depth_write = 0, \
    .depth_test_compare_op = xaku_compare_op_less_equal, \
    .min_depth_bounds = 0.0f, \
    .max_depth_bounds = 1.0f, \
}

struct xaku_stencil_test_params {
    u8                                              test_mask;
    u8                                              write_mask;
    enum xaku_compare_op                            stencil_test_compare_op; 
    enum xaku_stencil_op                            stencil_op_fail;
    enum xaku_stencil_op                            stencil_op_depth_fail;
    enum xaku_stencil_op                            stencil_op_depth_pass;
};
#define XAKU_STENCIL_TEST_PARAMS_INIT { \
    .test_mask = 0xffu, \
    .write_mask = 0u, \
    .stencil_test_compare_op = xaku_compare_op_always, \
    .stencil_op_fail = xaku_stencil_op_keep, \
    .stencil_op_depth_fail = xaku_stencil_op_keep, \
    .stencil_op_depth_pass = xaku_stencil_op_keep, \
}

enum xaku_conservative_rasterization_mode {
    xaku_conservative_rasterization_mode_disabled = 0,
    xaku_conservative_rasterization_mode_overestimate,
    xaku_conservative_rasterization_mode_underestimate,
};

struct xaku_conservative_raster_params {
    enum xaku_conservative_rasterization_mode               mode;
    f32                                                     size;
};

struct xaku_rasterizer_params {
    enum xaku_primitive_topology                            primitive_topology;
    enum xaku_polygon_mode                                  polygon_mode;
    enum xaku_cull_mode                                     face_culling;
    enum xaku_winding_order                                 front_face_winding;
    bool                                                    primitive_restart_enable;
    bool                                                    depth_clamp_enable;
    bool                                                    rasterizer_discard_enable;
    bool                                                    depth_bias_enable;
    f32                                                     depth_bias_constant_factor;
    f32                                                     depth_bias_clamp;
    f32                                                     depth_bias_slope_factor;
    f32                                                     line_width;
    lake_optional(struct xaku_conservative_raster_params)   conservative_raster_params;
    lake_optional(xaku_sample_counts)                       static_state_sample_count;
};
#define XAKU_RASTERIZER_PARAMS_INIT { \
    .primitive_topology = xaku_primitive_topology_triangle_list, \
    .polygon_mode = xaku_polygon_mode_fill, \
    .face_culling = xaku_cull_mode_none, \
    .front_face_winding = xaku_winding_order_cw, \
    .primitive_restart_enable = 0, \
    .depth_clamp_enable = 0, \
    .rasterizer_discard_enable = 0, \
    .depth_bias_enable = 0, \
    .depth_bias_constant_factor = 0.0f, \
    .depth_bias_clamp = 0.0f, \
    .depth_bias_slope_factor = 0.0f, \
    .line_width = 1.0f, \
    .conservative_rasterization_mode = xaku_conservative_rasterization_mode_disabled, \
    .conservative_rasterization_size = 0, \
    .static_state_sample_count = xaku_sample_count_1, \
    .has_conservative_rasterization = false, \
    .has_static_state_sample_count = true, \
}

struct xaku_blend_params {
    enum xaku_blend_factor                              src_color_blend_factor;
    enum xaku_blend_factor                              dst_color_blend_factor;
    enum xaku_blend_op                                  color_blend_op;
    enum xaku_blend_factor                              src_alpha_blend_factor;
    enum xaku_blend_factor                              dst_alpha_blend_factor;
    enum xaku_blend_op                                  alpha_blend_op;
    xaku_color_components                               color_write_mask;
};
#define XAKU_BLEND_PARAMS_INIT { \
    .src_color_blend_factor = xaku_blend_factor_one, \
    .dst_color_blend_factor = xaku_blend_factor_zero, \
    .color_blend_op = xaku_blend_op_add, \
    .src_alpha_blend_factor = xaku_blend_factor_one, \
    .dst_alpha_blend_factor = xaku_blend_factor_zero, \
    .alpha_blend_op = xaku_blend_op_add, \
    .color_write_mask = xaku_color_component_r | xaku_color_component_g | xaku_color_component_b | xaku_color_component_a, \
}

struct xaku_render_attachment {
    enum xaku_format                                    format;
    lake_optional(struct xaku_blend_params)             blend;
};

enum xaku_tesselation_domain_origin {
    xaku_tesselation_domain_origin_upper_left = 0,
    xaku_tesselation_domain_origin_lower_left,
};

struct xaku_tesselation_params {
    u32                                                 control_points;
    enum xaku_tesselation_domain_origin                 origin;
};

struct xaku_raster_pipeline_assembly {
    lake_optional(struct xaku_shader_params)            mesh_shader;
    lake_optional(struct xaku_shader_params)            vertex_shader;
    lake_optional(struct xaku_shader_params)            tesselation_control_shader;
    lake_optional(struct xaku_shader_params)            tesselation_evaluation_shader;
    lake_optional(struct xaku_shader_params)            geometry_shader;
    lake_optional(struct xaku_shader_params)            fragment_shader;
    lake_optional(struct xaku_shader_params)            task_shader;
    lake_fixed_list(struct xaku_render_attachment, 8)   color_attachments;
    lake_optional(struct xaku_depth_test_params)        depth_test;
    lake_optional(struct xaku_stencil_test_params)      stencil_test;
    lake_optional(struct xaku_tesselation_params)       tesselation;
    struct xaku_rasterizer_params                       raster;
    u32                                                 push_constant_size;
    lake_small_string                                   name;
};
#define XAKU_RASTER_PIPELINE_ASSEMBLY_INIT { \
    .mesh_shader = LAKE_ZERO_INIT, \
    .vertex_shader = LAKE_ZERO_INIT, \
    .tesselation_control_shader = LAKE_ZERO_INIT, \
    .tesselation_evaluation_shader = LAKE_ZERO_INIT, \
    .geometry_shader = LAKE_ZERO_INIT, \
    .fragment_shader = LAKE_ZERO_INIT, \
    .task_shader = LAKE_ZERO_INIT, \
    .color_attachments = LAKE_ZERO_INIT, \
    .depth_test = LAKE_ZERO_INIT, \
    .stencil_test = LAKE_ZERO_INIT, \
    .tesselation = LAKE_ZERO_INIT, \
    .raster = LAKE_ZERO_INIT, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

struct xaku_raster_pipeline_compile_assembly {
    lake_optional(struct xaku_shader_compile_params)    mesh_shader;
    lake_optional(struct xaku_shader_compile_params)    vertex_shader;
    lake_optional(struct xaku_shader_compile_params)    tesselation_control_shader;
    lake_optional(struct xaku_shader_compile_params)    tesselation_evaluation_shader;
    lake_optional(struct xaku_shader_compile_params)    geometry_shader;
    lake_optional(struct xaku_shader_compile_params)    fragment_shader;
    lake_optional(struct xaku_shader_compile_params)    task_shader;
    lake_fixed_list(struct xaku_render_attachment, 8)   color_attachments;
    lake_optional(struct xaku_depth_test_params)        depth_test;
    lake_optional(struct xaku_stencil_test_params)      stencil_test;
    lake_optional(struct xaku_tesselation_params)       tesselation;
    struct xaku_rasterizer_params                       raster;
    u32                                                 push_constant_size;
    lake_small_string                                   name;
};
#define XAKU_RASTER_PIPELINE_COMPILE_ASSEMBLY_INIT { \
    .mesh_shader = LAKE_ZERO_INIT, \
    .vertex_shader = LAKE_ZERO_INIT, \
    .tesselation_control_shader = LAKE_ZERO_INIT, \
    .tesselation_evaluation_shader = LAKE_ZERO_INIT, \
    .geometry_shader = LAKE_ZERO_INIT, \
    .fragment_shader = LAKE_ZERO_INIT, \
    .task_shader = LAKE_ZERO_INIT, \
    .color_attachments = LAKE_ZERO_INIT, \
    .depth_test = LAKE_ZERO_INIT, \
    .stencil_test = LAKE_ZERO_INIT, \
    .tesselation = LAKE_ZERO_INIT, \
    .raster = LAKE_ZERO_INIT, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

enum xaku_ray_tracing_shader_group_type {
    xaku_ray_tracing_shader_group_type_general = 0,
    xaku_ray_tracing_shader_group_type_triangles_hit_group,
    xaku_ray_tracing_shader_group_type_procedural_hit_group,
};

struct xaku_ray_tracing_shader_group_params {
    enum xaku_ray_tracing_shader_group_type             type;
    u32                                                 general_shader_index;
    u32                                                 closest_hit_shader_index;
    u32                                                 any_hit_shader_index;
    u32                                                 intersection_shader_index;
};
#define XAKU_DEFAULT_RAY_TRACING_SHADER_GROUP_PARAMS { \
    .type = xaku_ray_tracing_shader_group_type_general, \
    .general_shader_index = XAKU_SHADER_UNUSED, \
    .closest_hit_shader_index = XAKU_SHADER_UNUSED, \
    .any_hit_shader_index = XAKU_SHADER_UNUSED, \
    .intersection_shader_index = XAKU_SHADER_UNUSED, \
}

struct xaku_ray_tracing_pipeline_assembly {
    lake_span_to_const(struct xaku_shader_params)                   ray_gen_stages;
    lake_span_to_const(struct xaku_shader_params)                   miss_stages;
    lake_span_to_const(struct xaku_shader_params)                   callable_stages;
    lake_span_to_const(struct xaku_shader_params)                   intersection_stages;
    lake_span_to_const(struct xaku_shader_params)                   closest_hit_stages;
    lake_span_to_const(struct xaku_shader_params)                   any_hit_stages;
    lake_span_to_const(struct xaku_ray_tracing_shader_group_params) shader_groups;
    u32                                                             max_ray_recursion_depth; 
    u32                                                             push_constant_size;
    lake_small_string                                               name;
};
#define XAKU_RAY_TRACING_PIPELINE_ASSEMBLY_INIT { \
    .ray_gen_stages = LAKE_ZERO_INIT, \
    .miss_stages = LAKE_ZERO_INIT, \
    .callable_stages = LAKE_ZERO_INIT, \
    .intersection_stages = LAKE_ZERO_INIT, \
    .closest_hit_stages = LAKE_ZERO_INIT, \
    .any_hit_stages = LAKE_ZERO_INIT, \
    .shader_groups = LAKE_ZERO_INIT, \
    .max_ray_recursion_depth = 0, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

struct xaku_ray_tracing_pipeline_compile_assembly {
    lake_span_to_const(struct xaku_shader_compile_params)           ray_gen_stages;
    lake_span_to_const(struct xaku_shader_compile_params)           miss_stages;
    lake_span_to_const(struct xaku_shader_compile_params)           callable_stages;
    lake_span_to_const(struct xaku_shader_compile_params)           intersection_stages;
    lake_span_to_const(struct xaku_shader_compile_params)           closest_hit_stages;
    lake_span_to_const(struct xaku_shader_compile_params)           any_hit_stages;
    lake_span_to_const(struct xaku_ray_tracing_shader_group_params) shader_groups;
    u32                                                             max_ray_recursion_depth;
    u32                                                             push_constant_size;
    lake_small_string                                               name;
};
#define XAKU_RAY_TRACING_PIPELINE_COMPILE_ASSEMBLY_INIT { \
    .ray_gen_stages = LAKE_ZERO_INIT, \
    .miss_stages = LAKE_ZERO_INIT, \
    .callable_stages = LAKE_ZERO_INIT, \
    .intersection_stages = LAKE_ZERO_INIT, \
    .closest_hit_stages = LAKE_ZERO_INIT, \
    .any_hit_stages = LAKE_ZERO_INIT, \
    .shader_groups = LAKE_ZERO_INIT, \
    .max_ray_recursion_depth = 0, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

struct xaku_command_recorder_assembly {
    enum xaku_queue_type            queue_family;
    lake_small_string               name;
};
#define XAKU_COMMAND_RECORDER_ASSEMBLY_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_copy_buffer_to_buffer */
struct xaku_buffer_copy_params {
    xaku_buffer_id                  src_buffer;
    xaku_buffer_id                  dst_buffer;
    u64                             src_offset;
    u64                             dst_offset;
    u64                             size;
};
#define XAKU_BUFFER_COPY_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_copy_buffer_to_texture, PFN_xaku_cmd_copy_texture_to_buffer */
struct xaku_buffer_texture_copy_params {
    xaku_buffer_id                  buffer;
    u64                             buffer_offset;
    xaku_texture_id                 texture;
    enum xaku_texture_layout        texture_layout;
    struct xaku_texture_array_slice texture_slice;
    struct lake_offset3d            texture_offset;
    struct lake_extent3d            texture_extent;
};
#define XAKU_BUFFER_TEXTURE_COPY_PARAMS_INIT { \
    .buffer = LAKE_ZERO_INIT, \
    .buffer_offset = 0, \
    .texture = LAKE_ZERO_INIT, \
    .texture_layout = xaku_texture_layout_transfer_dst_optimal, \
    .texture_slice = LAKE_ZERO_INIT, \
    .texture_offset = LAKE_ZERO_INIT, \
    .texture_extent = LAKE_ZERO_INIT, \
}

/** PFN_xaku_cmd_copy_texture_to_texture */
struct xaku_texture_copy_params {
    xaku_texture_id                 src_texture;
    xaku_texture_id                 dst_texture;
    enum xaku_texture_layout        src_layout;
    enum xaku_texture_layout        dst_layout;
    struct xaku_texture_array_slice src_slice;
    struct lake_offset3d            src_offset;
    struct xaku_texture_array_slice dst_slice;
    struct lake_offset3d            dst_offset;
    struct lake_extent3d            extent;
};
#define XAKU_TEXTURE_COPY_PARAMS_INIT { \
    .src_texture = LAKE_ZERO_INIT, \
    .dst_texture = LAKE_ZERO_INIT, \
    .src_layout = xaku_texture_layout_transfer_src_optimal, \
    .dst_layout = xaku_texture_layout_transfer_dst_optimal, \
    .src_slice = LAKE_ZERO_INIT, \
    .src_offset = LAKE_ZERO_INIT, \
    .dst_slice = LAKE_ZERO_INIT, \
    .dst_offset = LAKE_ZERO_INIT, \
    .extent = LAKE_ZERO_INIT, \
}

/** PFN_xaku_cmd_blit_texture_to_texture */
struct xaku_texture_blit_params {
    xaku_texture_id                 src_texture;
    xaku_texture_id                 dst_texture;
    enum xaku_texture_layout        src_layout;
    enum xaku_texture_layout        dst_layout;
    struct xaku_texture_array_slice src_slice;
    struct xaku_texture_array_slice dst_slice;
    struct lake_offset3d            src_offsets[2];
    struct lake_offset3d            dst_offsets[2];
    enum xaku_filter_mode           filter;
};
#define XAKU_TEXTURE_BLIT_PARAMS_INIT { \
    .src_texture = LAKE_ZERO_INIT, \
    .dst_texture = LAKE_ZERO_INIT, \
    .src_layout = xaku_texture_layout_transfer_src_optimal, \
    .src_layout = xaku_texture_layout_transfer_dst_optimal, \
    .src_slice = LAKE_ZERO_INIT, \
    .dst_slice = LAKE_ZERO_INIT, \
    .src_offsets = LAKE_ZERO_INIT, \
    .dst_offsets = LAKE_ZERO_INIT, \
    .filter = xaku_filter_mode_nearest, \
}

/** PFN_xaku_cmd_clear_buffer */
struct xaku_buffer_clear_params {
    xaku_buffer_id                  buffer;
    u64                             offset;
    u64                             size;
    u32                             clear_value;
};
#define XAKU_BUFFER_CLEAR_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_clear_texture */
struct xaku_texture_clear_params {
    xaku_texture_id                 texture;
    enum xaku_texture_layout        texture_layout;
    struct xaku_texture_array_slice texture_slice;
    union xaku_clear_value          clear_value;
};
#define XAKU_TEXTURE_CLEAR_PARAMS_INIT { \
    .texture = LAKE_ZERO_INIT, \
    .texture_layout = xaku_texture_layout_transfer_dst_optimal, \
    .texture_slice = LAKE_ZERO_INIT, \
    .clear_value = LAKE_ZERO_INIT, \
}

/** PFN_xaku_cmd_build_acceleration_structures */
struct xaku_build_acceleration_structures_params {
    const struct xaku_tlas_assembly    *tlas_assembly;
    const struct xaku_blas_assembly    *blas_assembly;
    u32                                 tlas_assembly_count;
    u32                                 blas_assembly_count;
};
#define XAKU_BUILD_ACCELERATION_STRUCTURES_PARAMS_INIT

/** PFN_xaku_cmd_push_constant */
struct xaku_push_constant_params {
    const void *data;
    u64         size;
};

/** PFN_xaku_cmd_set_depth_bias */
struct xaku_depth_bias_params {
    f32 constant_factor;
    f32 clamp;
    f32 slope_factor;
};
#define XAKU_DEPTH_BIAS_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_set_index_buffer */
struct xaku_index_buffer_params {
    xaku_buffer_id          buffer;
    u64                     offset;
    enum xaku_index_format  index_format;
};
#define XAKU_INDEX_BUFFER_PARAMS_INIT { \
    .buffer = LAKE_ZERO_INIT, \
    .offset = 0, \
    .index_format = xaku_index_format_u32, \
}

struct xaku_render_attachment_resolve_params {
    xaku_texture_view_id        texture_view;
    enum xaku_texture_layout    layout;
    u32                         mode; /**< enum xaku_resolve_mode_bits */
};
#define XAKU_RENDER_ATTACHMENT_RESOLVE_PARAMS_INIT { \
    .texture_view = LAKE_ZERO_INIT, \
    .layout = xaku_texture_layout_attachment_optimal, \
    .mode = xaku_resolve_mode_average, \
}

struct xaku_render_attachment_params {
    xaku_texture_view_id                            texture_view;
    enum xaku_texture_layout                        layout;
    enum xaku_load_op                               load_op;
    enum xaku_store_op                              store_op;
    union xaku_clear_value                          clear_value;
    u8                                              has_resolve;
    struct xaku_render_attachment_resolve_params   *resolve; /* optional */
};
#define XAKU_RENDER_ATTACHMENT_PARAMS_INIT { \
    .texture_view = LAKE_ZERO_INIT, \
    .layout = xaku_texture_layout_attachment_optimal, \
    .load_op = xaku_load_op_dont_care, \
    .store_op = xaku_store_op_store, \
    .clear_value = LAKE_ZERO_INIT, \
    .has_resolve = 0, \
    .resolve = NULL, \
}

/** PFN_xaku_cmd_begin_rendering */
struct xaku_begin_rendering_params {
    lake_fixed_list(struct xaku_render_attachment_params, 8)    color_attachments;
    struct xaku_render_attachment_params                       *depth_attachment;
    struct xaku_render_attachment_params                       *stencil_attachment;
    struct lake_rect2d                                          render_area;
};
#define XAKU_BEGIN_RENDERING_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_dispatch */
struct xaku_dispatch_params {
    u32 group_x, group_y, group_z;
};
#define XAKU_DISPATCH_PARAMS_INIT {1u, 1u, 1u}

/** PFN_xaku_cmd_dispatch_indirect */
struct xaku_dispatch_indirect_params {
    xaku_buffer_id indirect_buffer;
    u64            indirect_buffer_offset;
};
#define XAKU_DISPATCH_INDIRECT_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_draw */
struct xaku_draw_params {
    u32 vertex_count;
    u32 instance_count;
    u32 first_vertex;
    u32 first_instance;
};
#define XAKU_DRAW_PARAMS_INIT { \
    .vertex_count = 0, \
    .instance_count = 1, \
    .first_vertex = 0, \
    .first_instance = 0, \
}

/** PFN_xaku_cmd_draw_indexed */
struct xaku_draw_indexed_params {
    u32 index_count;
    u32 instance_count;
    u32 first_index;
    s32 vertex_offset;
    u32 first_instance;
};
#define XAKU_DRAW_INDEXED_PARAMS_INIT { \
    .index_count = 0, \
    .instance_count = 1, \
    .first_index = 0, \
    .vertex_offset = 0, \
    .first_instance = 0, \
}

/** PFN_xaku_cmd_draw_indirect */
struct xaku_draw_indirect_params {
    xaku_buffer_id indirect_buffer;
    u64            indirect_buffer_offset;
    u32            draw_count;
    u32            draw_command_stride;
    bool           is_indexed;
};
#define XAKU_DRAW_INDIRECT_PARAMS_INIT { \
    .indirect_buffer = LAKE_ZERO_INIT, \
    .indirect_buffer_offset = 0, \
    .draw_count = 1, \
    .draw_command_stride = 0, \
    .is_indexed = 0, \
}

/** PFN_xaku_cmd_draw_indirect_count */
struct xaku_draw_indirect_count_params {
    xaku_buffer_id indirect_buffer;
    u64            indirect_buffer_offset;
    xaku_buffer_id count_buffer;
    u64            count_buffer_offset;
    u32            max_draw_count;
    u32            draw_command_stride;
    bool           is_indexed;
};
#define XAKU_DRAW_INDIRECT_COUNT_PARAMS_INIT { \
    .indirect_buffer = LAKE_ZERO_INIT, \
    .indirect_buffer_offset = 0, \
    .count_buffer = LAKE_ZERO_INIT, \
    .count_buffer_offset = 0, \
    .max_draw_count = ((1 << 16) - 1), \
    .draw_command_stride = 0, \
    .is_indexed = 0, \
}

/** PFN_xaku_cmd_mesh_tasks_indirect */
struct xaku_mesh_tasks_indirect_params {
    xaku_buffer_id indirect_buffer;
    u64            offset;
    u32            draw_count;
    u32            stride;
};
#define XAKU_MESH_TASKS_INDIRECT_PARAMS_INIT { \
    .indirect_buffer = LAKE_ZERO_INIT, \
    .offset = 0, \
    .draw_count = 1, \
    .stride = 12, \
}

/** PFN_xaku_cmd_mesh_tasks_indirect_count */
struct xaku_mesh_tasks_indirect_count_params {
    xaku_buffer_id indirect_buffer;
    u64            indirect_buffer_offset;
    xaku_buffer_id count_buffer;
    u64            count_buffer_offset;
    u32            max_count;
    u32            stride;
};
#define XAKU_MESH_TASKS_INDIRECT_COUNT_PARAMS_INIT { \
    .indirect_buffer = LAKE_ZERO_INIT, \
    .indirect_buffer_offset = 0, \
    .count_buffer = LAKE_ZERO_INIT, \
    .count_buffer_offset = 0, \
    .max_count = 0, \
    .stride = 12, \
}

/** PFN_xaku_cmd_trace_rays */
struct xaku_trace_rays_params {
    u32                                 width;
    u32                                 height;
    u32                                 depth;
    u32                                 raygen_handle_offset;
    u32                                 miss_handle_offset;
    u32                                 hit_handle_offset;
    u32                                 callable_handle_offset;
    struct xaku_shader_binding_table    shader_binding_table;
};
#define XAKU_TRACE_RAYS_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_trace_rays_indirect */
struct xaku_trace_rays_indirect_params {
    u64                                 indirect_device_address;
    u32                                 raygen_handle_offset;
    u32                                 miss_handle_offset;
    u32                                 hit_handle_offset;
    u32                                 callable_handle_offset;
    struct xaku_shader_binding_table    shader_binding_table;
};
#define XAKU_TRACE_RAYS_INDIRECT_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_write_timestamps */
struct xaku_write_timestamp_params {
    struct xaku_query_pool             *query_pool;
    u32                                 shader_stage; /**< enum xaku_shader_stage_bits */
    u32                                 query_index;
};
#define XAKU_WRITE_TIMESTAMP_PARAMS_INIT LAKE_ZERO_INIT

/** PFN_xaku_cmd_reset_timestamps */
struct xaku_reset_timestamp_params {
    struct xaku_query_pool             *query_pool;
    u32                                 start_index;
    u32                                 count;
#define XAKU_RESET_TIMESTAMP_PARAMS_INIT LAKE_ZERO_INIT
};

/** PFN_xaku_cmd_begin_label */
struct xaku_command_label_params {
    vec4                                label_color;
    lake_small_string                   name;
};
#define XAKU_COMMAND_LABEL_PARAMS_INIT LAKE_ZERO_INIT

/******************************
 *                            * 
 *     INTERNAL INTERFACE     * 
 *                            * 
 ******************************/

/** Must be implemented as the first member of a 'xaku_device' implementation. */
#define XAKU_INTERFACE_DEVICE_HEADER \
    /** The encore used to create this device. */ \
    struct xaku_encore                     *xaku; \
    /** The device may be destroyed when the reference count reaches zero. */ \
    atomic_u64                              refcnt; \
    /** A pointer to Riven for convenience. */ \
    const struct riven_context             *riven; \
    /** Acquired from an internal physical device, tied to the encore and not a device. */ \
    const struct xaku_device_properties    *properties; \
    /** The assembly details used to create this device. */ \
    struct xaku_device_assembly             assembly;

/** Must be implemented by opaque structures of device objects. */
#define _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(T) \
    /** Device that owns this resource. */ \
    struct xaku_device                     *device; \
    /** The resource may be released when the reference count reaches zero. */ \
    atomic_u64                              refcnt; \
    /** The assembly details used to create this resource. */ \
    struct xaku_##T##_assembly              assembly;
#define XAKU_INTERFACE_MEMORY_HEADER                _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(memory)
#define XAKU_INTERFACE_QUERY_POOL_HEADER            _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(query_pool)
#define XAKU_INTERFACE_SWAPCHAIN_HEADER             _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(swapchain)
#define XAKU_INTERFACE_COMPUTE_PIPELINE_HEADER      _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(compute_pipeline)
#define XAKU_INTERFACE_RASTER_PIPELINE_HEADER       _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(raster_pipeline)
#define XAKU_INTERFACE_RAY_TRACING_PIPELINE_HEADER  _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(ray_tracing_pipeline)
#define XAKU_INTERFACE_COMMAND_RECORDER_HEADER      _XAKU_INTERFACE_DEVICE_OBJECT_HEADER(command_recorder)
    
/** Must be implemented as the first member by the following internal structures are accessed via a render handle. */
#define _XAKU_INTERFACE_RENDER_HANDLE_HEADER(T) \
    /** The assembly details used to create this resource. */ \
    struct xaku_##T##_assembly              assembly;
#define XAKU_INTERFACE_BUFFER_HEADER                _XAKU_INTERFACE_RENDER_HANDLE_HEADER(buffer)
#define XAKU_INTERFACE_TEXTURE_HEADER               _XAKU_INTERFACE_RENDER_HANDLE_HEADER(texture)
#define XAKU_INTERFACE_TEXTURE_VIEW_HEADER          _XAKU_INTERFACE_RENDER_HANDLE_HEADER(texture_view)
#define XAKU_INTERFACE_SAMPLER_HEADER               _XAKU_INTERFACE_RENDER_HANDLE_HEADER(sampler)
#define XAKU_INTERFACE_TLAS_HEADER                  _XAKU_INTERFACE_RENDER_HANDLE_HEADER(tlas)
#define XAKU_INTERFACE_BLAS_HEADER                  _XAKU_INTERFACE_RENDER_HANDLE_HEADER(blas)

/** List properties of available physical devices, properties may be NULL to query for property count and allocate an array of pointers to properties. */
typedef void (LAKECALL *PFN_xaku_list_devices_properties)(struct xaku_encore *xaku, u32 *property_count, const struct xaku_device_properties **properties);
#define FN_XAKU_LIST_DEVICES_PROPERTIES(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_list_devices_properties(struct xaku_encore *xaku, u32 *property_count, const struct xaku_device_properties **properties)

/** Create a rendering device from given assembly details. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_assembly)(struct xaku_encore *xaku, struct xaku_device_assembly *assembly, struct xaku_device **out_device);
#define FN_XAKU_DEVICE_ASSEMBLY(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_assembly(struct xaku_encore *xaku, struct xaku_device_assembly *assembly, struct xaku_device **out_device)

/** Destroy a rendering device, zero ref callback. */
typedef void (LAKECALL *PFN_xaku_device_disassembly)(struct xaku_device *device);
#define FN_XAKU_DEVICE_DISASSEMBLY(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_device_disassembly(struct xaku_device *device)

/** Retrieve the number of queues available for a given queue type. On optional queues
 *  (sparse binding, video decode/encode), 0 means that these commands are unavailable. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_queue_count)(struct xaku_device *device, enum xaku_queue_type queue_type, u32 *out_count);
#define FN_XAKU_DEVICE_QUEUE_COUNT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_queue_count(struct xaku_device *device, enum xaku_queue_type queue_type, u32 *out_count)

/** Wait until GPU work on a given queue is done. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_queue_wait_idle)(struct xaku_device *device, struct xaku_queue queue);
#define FN_XAKU_DEVICE_QUEUE_WAIT_IDLE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_queue_wait_idle(struct xaku_device *device, struct xaku_queue queue)

/** Wait until all current GPU work is done. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_wait_idle)(struct xaku_device *device);
#define FN_XAKU_DEVICE_WAIT_IDLE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_wait_idle(struct xaku_device *device)

/** Submit work into a command queue. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_submit)(struct xaku_device *device, struct xaku_executable_command_list *cmd_list);
#define FN_XAKU_DEVICE_SUBMIT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_submit(struct xaku_device *device, struct xaku_executable_command_list *cmd_list)

/** Present swapchain images, used by a primary device that supports presentation to the window surface. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_device_present)(struct xaku_device *device, struct xaku_swapchain **swapchains, u32 swapchain_count);
#define FN_XAKU_DEVICE_PRESENT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_device_present(struct xaku_device *device, struct xaku_swapchain **swapchains, u32 swapchain_count)

/** Execute the deferred disassembly work as jobs, from disassembly zero reference callbacks of zombie GPU resources.
 *  Whether a resource becomes a zombie or not is decided in the backend, at a disassembly call. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_deferred_resource_disassembly)(struct xaku_device *device);
#define FN_XAKU_DEFERRED_RESOURCE_DISASSEMBLY(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_deferred_resource_disassembly(struct xaku_device *device)

/** Used to assemble an object that uses an internal reference counter: devices, swapchains, pipelines, etc. */
#define _PFN_XAKU_OBJECT_ASSEMBLY(T) \
    typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_##T##_assembly)(struct xaku_device *device, struct xaku_##T##_assembly *assembly, struct xaku_##T **out_##T);
#define _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, T) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_##T##_assembly(struct xaku_device *device, struct xaku_##T##_assembly *assembly, struct xaku_##T **out_##T)
/** Used as a callback for zero reference count, or to explicitly destroy an object.
 *  Can be run as a job with Riven, cast into PFN_riven_work. */
#define _PFN_XAKU_OBJECT_DISASSEMBLY(T) \
    typedef void (LAKECALL *PFN_xaku_##T##_disassembly)(struct xaku_##T *T);
#define _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, T) \
    void LAKECALL _xaku_##ENCORE##_##T##_disassembly(struct xaku_##T *T)

_PFN_XAKU_OBJECT_ASSEMBLY(memory)
_PFN_XAKU_OBJECT_DISASSEMBLY(memory);
#define FN_XAKU_MEMORY_ASSEMBLY(ENCORE)                     _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, memory)
#define FN_XAKU_MEMORY_DISASSEMBLY(ENCORE)                  _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, memory)

/** Get GPU memory requirements for a buffer. */
typedef void (LAKECALL *PFN_xaku_memory_buffer_requirements)(struct xaku_device *device, struct xaku_buffer_assembly *assembly, struct xaku_memory_requirements *out_requirements);
#define FN_XAKU_MEMORY_BUFFER_REQUIREMENTS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_memory_buffer_requirements(struct xaku_device *device, struct xaku_buffer_assembly *assembly, struct xaku_memory_requirements *out_requirements)

/** Get GPU memory requirements for a texture. */
typedef void (LAKECALL *PFN_xaku_memory_texture_requirements)(struct xaku_device *device, struct xaku_texture_assembly *assembly, struct xaku_memory_requirements *out_requirements);
#define FN_XAKU_MEMORY_TEXTURE_REQUIREMENTS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_memory_texture_requirements(struct xaku_device *device, struct xaku_texture_assembly *assembly, struct xaku_memory_requirements *out_requirements)

_PFN_XAKU_OBJECT_ASSEMBLY(query_pool);
_PFN_XAKU_OBJECT_DISASSEMBLY(query_pool);
#define FN_XAKU_QUERY_POOL_ASSEMBLY(ENCORE)                 _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, query_pool)
#define FN_XAKU_QUERY_POOL_DISASSEMBLY(ENCORE)              _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, query_pool)

_PFN_XAKU_OBJECT_ASSEMBLY(swapchain);
_PFN_XAKU_OBJECT_DISASSEMBLY(swapchain);
#define FN_XAKU_SWAPCHAIN_ASSEMBLY(ENCORE)                  _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, swapchain)
#define FN_XAKU_SWAPCHAIN_DISASSEMBLY(ENCORE)               _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, swapchain)

_PFN_XAKU_OBJECT_ASSEMBLY(compute_pipeline);
_PFN_XAKU_OBJECT_DISASSEMBLY(compute_pipeline);
#define FN_XAKU_COMPUTE_PIPELINE_ASSEMBLY(ENCORE)           _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, compute_pipeline)
#define FN_XAKU_COMPUTE_PIPELINE_DISASSEMBLY(ENCORE)        _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, compute_pipeline)

_PFN_XAKU_OBJECT_ASSEMBLY(raster_pipeline);
_PFN_XAKU_OBJECT_DISASSEMBLY(raster_pipeline);
#define FN_XAKU_RASTER_PIPELINE_ASSEMBLY(ENCORE)            _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, raster_pipeline)
#define FN_XAKU_RASTER_PIPELINE_DISASSEMBLY(ENCORE)         _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, raster_pipeline)

_PFN_XAKU_OBJECT_ASSEMBLY(ray_tracing_pipeline);
_PFN_XAKU_OBJECT_DISASSEMBLY(ray_tracing_pipeline);
#define FN_XAKU_RAY_TRACING_PIPELINE_ASSEMBLY(ENCORE)       _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, ray_tracing_pipeline)
#define FN_XAKU_RAY_TRACING_PIPELINE_DISASSEMBLY(ENCORE)    _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, ray_tracing_pipeline)

_PFN_XAKU_OBJECT_ASSEMBLY(command_recorder);
_PFN_XAKU_OBJECT_DISASSEMBLY(command_recorder);
#define FN_XAKU_COMMAND_RECORDER_ASSEMBLY(ENCORE)           _FN_XAKU_OBJECT_ASSEMBLY(ENCORE, command_recorder)
#define FN_XAKU_COMMAND_RECORDER_DISASSEMBLY(ENCORE)        _FN_XAKU_OBJECT_DISASSEMBLY(ENCORE, command_recorder)

/** Create a GPU resource using a render handle. */
#define _PFN_XAKU_CREATE_RESOURCE(NAME, T, ID) \
    typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_create_##NAME)(struct xaku_device *device, struct xaku_##T##_assembly *assembly, xaku_##ID##_id *out_##ID);
#define _FN_XAKU_CREATE_RESOURCE(ENCORE, NAME, T, ID) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_create_##NAME(struct xaku_device *device, struct xaku_##T##_assembly *assembly, xaku_##ID##_id *out_##ID)

_PFN_XAKU_CREATE_RESOURCE(buffer, buffer, buffer)
_PFN_XAKU_CREATE_RESOURCE(buffer_from_memory, buffer_memory, buffer)
_PFN_XAKU_CREATE_RESOURCE(texture, texture, texture)
_PFN_XAKU_CREATE_RESOURCE(texture_from_memory, texture_memory, texture)
_PFN_XAKU_CREATE_RESOURCE(texture_view, texture_view, texture_view)
_PFN_XAKU_CREATE_RESOURCE(sampler, sampler, sampler)
_PFN_XAKU_CREATE_RESOURCE(tlas, tlas, tlas)
_PFN_XAKU_CREATE_RESOURCE(blas, blas, blas)
_PFN_XAKU_CREATE_RESOURCE(tlas_from_buffer, tlas_buffer, tlas)
_PFN_XAKU_CREATE_RESOURCE(blas_from_buffer, blas_buffer, blas)
#define FN_XAKU_CREATE_BUFFER(ENCORE)                _FN_XAKU_CREATE_RESOURCE(ENCORE, buffer, buffer, buffer)
#define FN_XAKU_CREATE_BUFFER_FROM_MEMORY(ENCORE)    _FN_XAKU_CREATE_RESOURCE(ENCORE, buffer_from_memory, buffer_memory, buffer)
#define FN_XAKU_CREATE_TEXTURE(ENCORE)               _FN_XAKU_CREATE_RESOURCE(ENCORE, texture, texture, texture)
#define FN_XAKU_CREATE_TEXTURE_FROM_MEMORY(ENCORE)   _FN_XAKU_CREATE_RESOURCE(ENCORE, texture_from_memory, texture_memory, texture)
#define FN_XAKU_CREATE_TEXTURE_VIEW(ENCORE)          _FN_XAKU_CREATE_RESOURCE(ENCORE, texture_view, texture_view, texture_view)
#define FN_XAKU_CREATE_SAMPLER(ENCORE)               _FN_XAKU_CREATE_RESOURCE(ENCORE, sampler, sampler, sampler)
#define FN_XAKU_CREATE_TLAS(ENCORE)                  _FN_XAKU_CREATE_RESOURCE(ENCORE, tlas, tlas, tlas)
#define FN_XAKU_CREATE_BLAS(ENCORE)                  _FN_XAKU_CREATE_RESOURCE(ENCORE, blas, blas, blas)
#define FN_XAKU_CREATE_TLAS_FROM_BUFFER(ENCORE)      _FN_XAKU_CREATE_RESOURCE(ENCORE, tlas_from_buffer, tlas_buffer, tlas)
#define FN_XAKU_CREATE_BLAS_FROM_BUFFER(ENCORE)      _FN_XAKU_CREATE_RESOURCE(ENCORE, blas_from_buffer, blas_buffer, blas)

/** Copy the assembly details used to create this resource. */
#define _PFN_XAKU_GET_RESOURCE_ASSEMBLY(T) \
    typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_get_##T##_assembly)(struct xaku_device *device, xaku_##T##_id T, struct xaku_##T##_assembly *out_assembly);
#define _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, T) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_get_##T##_assembly(struct xaku_device *device, xaku_##T##_id T, struct xaku_##T##_assembly *out_assembly)

_PFN_XAKU_GET_RESOURCE_ASSEMBLY(buffer)
_PFN_XAKU_GET_RESOURCE_ASSEMBLY(texture)
_PFN_XAKU_GET_RESOURCE_ASSEMBLY(texture_view)
_PFN_XAKU_GET_RESOURCE_ASSEMBLY(sampler)
_PFN_XAKU_GET_RESOURCE_ASSEMBLY(tlas)
_PFN_XAKU_GET_RESOURCE_ASSEMBLY(blas)
#define FN_XAKU_GET_BUFFER_ASSEMBLY(ENCORE)         _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, buffer)
#define FN_XAKU_GET_TEXTURE_ASSEMBLY(ENCORE)        _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, texture)
#define FN_XAKU_GET_TEXTURE_VIEW_ASSEMBLY(ENCORE)   _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, texture_view)
#define FN_XAKU_GET_SAMPLER_ASSEMBLY(ENCORE)        _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, sampler)
#define FN_XAKU_GET_TLAS_ASSEMBLY(ENCORE)           _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, tlas)
#define FN_XAKU_GET_BLAS_ASSEMBLY(ENCORE)           _FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, blas)

typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_get_tlas_build_sizes)(struct xaku_device *device, struct xaku_tlas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes);
#define FN_XAKU_GET_TLAS_BUILD_SIZES(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_get_tlas_build_sizes(struct xaku_device *device, struct xaku_tlas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes)

typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_get_blas_build_sizes)(struct xaku_device *device, struct xaku_blas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes);
#define FN_XAKU_GET_BLAS_BUILD_SIZES(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_get_blas_build_sizes(struct xaku_device *device, struct xaku_blas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes)

/** Destroy a GPU resource using a render handle. */
#define _PFN_XAKU_DESTROY_RESOURCE(T) \
    typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_destroy_##T)(struct xaku_device *device, xaku_##T##_id T);
#define _FN_XAKU_DESTROY_RESOURCE(ENCORE, T) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_destroy_##T(struct xaku_device *device, xaku_##T##_id T)

_PFN_XAKU_DESTROY_RESOURCE(buffer)
_PFN_XAKU_DESTROY_RESOURCE(texture)
_PFN_XAKU_DESTROY_RESOURCE(texture_view)
_PFN_XAKU_DESTROY_RESOURCE(sampler)
_PFN_XAKU_DESTROY_RESOURCE(tlas)
_PFN_XAKU_DESTROY_RESOURCE(blas)
#define FN_XAKU_DESTROY_BUFFER(ENCORE)               _FN_XAKU_DESTROY_RESOURCE(ENCORE, buffer)
#define FN_XAKU_DESTROY_TEXTURE(ENCORE)              _FN_XAKU_DESTROY_RESOURCE(ENCORE, texture)
#define FN_XAKU_DESTROY_TEXTURE_VIEW(ENCORE)         _FN_XAKU_DESTROY_RESOURCE(ENCORE, texture_view)
#define FN_XAKU_DESTROY_SAMPLER(ENCORE)              _FN_XAKU_DESTROY_RESOURCE(ENCORE, sampler)
#define FN_XAKU_DESTROY_TLAS(ENCORE)                 _FN_XAKU_DESTROY_RESOURCE(ENCORE, tlas)
#define FN_XAKU_DESTROY_BLAS(ENCORE)                 _FN_XAKU_DESTROY_RESOURCE(ENCORE, blas)

/** Copy data between buffer regions. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_copy_params *params)

/** Copy data from a buffer into an texture. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_texture(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params)

/** Copy texture data into a buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params)

/** Copy data between texture regions. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_copy_params *params)

/** Copy regions of a source image into a destination image, potentially 
 *  performing format conversion, arbitrary scaling, and filtering. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_blit_texture_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_blit_params *params);
#define FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_blit_texture_to_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_blit_params *params)

/** Fill a region of a buffer with a fixed value, used to zero out the buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_clear_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_clear_params *params);
#define FN_XAKU_CMD_CLEAR_BUFFER(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_clear_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_clear_params *params)

/** Fill a region of a texture with a fixed value, used to zero out the texture. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_clear_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_clear_params *params);
#define FN_XAKU_CMD_CLEAR_TEXTURE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_clear_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_clear_params *params)

/** Builds and updates acceleration structures. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_build_acceleration_structures)(struct xaku_command_recorder *cmd, const struct xaku_build_acceleration_structures_params *params);
#define FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_build_acceleration_structures(struct xaku_command_recorder *cmd, const struct xaku_build_acceleration_structures_params *params)

/** Destroys a GPU resource AFTER the GPU is finished executing the command list.
 *  Useful for large uploads exceeding staging memory pools. Can be used for buffers,
 *  textures, texture views, samplers, acceleration structures. */
#define PFN_XAKU_CMD_DESTROY_DEFERRED(T) \
    typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_destroy_##T##_deferred)(struct xaku_command_recorder *cmd, xaku_##T##_id T);
#define FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, T) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_destroy_##T##_deferred(struct xaku_command_recorder *cmd, xaku_##T##_id T)

PFN_XAKU_CMD_DESTROY_DEFERRED(buffer)
PFN_XAKU_CMD_DESTROY_DEFERRED(texture)
PFN_XAKU_CMD_DESTROY_DEFERRED(texture_view)
PFN_XAKU_CMD_DESTROY_DEFERRED(sampler)
PFN_XAKU_CMD_DESTROY_DEFERRED(tlas)
PFN_XAKU_CMD_DESTROY_DEFERRED(blas)
#define FN_XAKU_CMD_DESTROY_BUFFER_DEFERRED(ENCORE)         FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, buffer)
#define FN_XAKU_CMD_DESTROY_TEXTURE_DEFERRED(ENCORE)        FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, texture)
#define FN_XAKU_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(ENCORE)   FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, texture_view)
#define FN_XAKU_CMD_DESTROY_SAMPLER_DEFERRED(ENCORE)        FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, sampler)
#define FN_XAKU_CMD_DESTROY_TLAS_DEFERRED(ENCORE)           FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, tlas)
#define FN_XAKU_CMD_DESTROY_BLAS_DEFERRED(ENCORE)           FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, blas)

/** Push constant data to shader stages. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_push_constant)(struct xaku_command_recorder *cmd, const struct xaku_push_constant_params *params);
#define FN_XAKU_CMD_PUSH_CONSTANT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_push_constant(struct xaku_command_recorder *cmd, const struct xaku_push_constant_params *params)

/** Bind compute pipeline for dispatch commands. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_set_compute_pipeline)(struct xaku_command_recorder *cmd, struct xaku_compute_pipeline *pipeline);
#define FN_XAKU_CMD_SET_COMPUTE_PIPELINE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_set_compute_pipeline(struct xaku_command_recorder *cmd, struct xaku_compute_pipeline *pipeline)

/** Bind raster pipeline for draw commands. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_set_raster_pipeline)(struct xaku_command_recorder *cmd, struct xaku_raster_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RASTER_PIPELINE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_set_raster_pipeline(struct xaku_command_recorder *cmd, struct xaku_raster_pipeline *pipeline)

/** Bind ray tracing pipeline for trace commands. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_set_ray_tracing_pipeline)(struct xaku_command_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_set_ray_tracing_pipeline(struct xaku_command_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline)

/** Configure viewport transformation dimensions. */
typedef void (LAKECALL *PFN_xaku_cmd_set_viewport)(struct xaku_command_recorder *cmd, const struct lake_viewport *viewport);
#define FN_XAKU_CMD_SET_VIEWPORT(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_viewport(struct xaku_command_recorder *cmd, const struct lake_viewport *viewport)

/** Configure scissor rectangle for fragment clipping. */
typedef void (LAKECALL *PFN_xaku_cmd_set_scissor)(struct xaku_command_recorder *cmd, const struct lake_rect2d *scissor);
#define FN_XAKU_CMD_SET_SCISSOR(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_scissor(struct xaku_command_recorder *cmd, const struct lake_rect2d *scissor)

/** Configure depth bias constants for depth calculations. */
typedef void (LAKECALL *PFN_xaku_cmd_set_depth_bias)(struct xaku_command_recorder *cmd, const struct xaku_depth_bias_params *params);
#define FN_XAKU_CMD_SET_DEPTH_BIAS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_depth_bias(struct xaku_command_recorder *cmd, const struct xaku_depth_bias_params *params)

/** Bind index buffer for indexed draw command. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_set_index_buffer)(struct xaku_command_recorder *cmd, const struct xaku_index_buffer_params *params);
#define FN_XAKU_CMD_SET_INDEX_BUFFER(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_set_index_buffer(struct xaku_command_recorder *cmd, const struct xaku_index_buffer_params *params)

/** Start a renderpass with specified render targets. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_begin_rendering)(struct xaku_command_recorder *cmd, const struct xaku_begin_rendering_params *params);
#define FN_XAKU_CMD_BEGIN_RENDERING(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_begin_rendering(struct xaku_command_recorder *cmd, const struct xaku_begin_rendering_params *params)

/** Ends the current renderpass. */
typedef void (LAKECALL *PFN_xaku_cmd_end_rendering)(struct xaku_command_recorder *cmd);
#define FN_XAKU_CMD_END_RENDERING(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_end_rendering(struct xaku_command_recorder *cmd)

/** Execute compute workgroups with direct parameters. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_dispatch)(struct xaku_command_recorder *cmd, const struct xaku_dispatch_params *params);
#define FN_XAKU_CMD_DISPATCH(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_dispatch(struct xaku_command_recorder *cmd, const struct xaku_dispatch_params *params)

/** Execute compute workgroups with indirect parameters buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_dispatch_indirect)(struct xaku_command_recorder *cmd, const struct xaku_dispatch_indirect_params *params);
#define FN_XAKU_CMD_DISPATCH_INDIRECT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_dispatch_indirect(struct xaku_command_recorder *cmd, const struct xaku_dispatch_indirect_params *params)

/** Draw non-indexed primitives with vertex count. */
typedef void (LAKECALL *PFN_xaku_cmd_draw)(struct xaku_command_recorder *cmd, const struct xaku_draw_params *params);
#define FN_XAKU_CMD_DRAW(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw(struct xaku_command_recorder *cmd, const struct xaku_draw_params *params)

/** Draw indexed primitives used bound index buffer. */
typedef void (LAKECALL *PFN_xaku_cmd_draw_indexed)(struct xaku_command_recorder *cmd, const struct xaku_draw_indexed_params *params);
#define FN_XAKU_CMD_DRAW_INDEXED(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw_indexed(struct xaku_command_recorder *cmd, const struct xaku_draw_indexed_params *params)

/** Draw primitives using indirect parameters buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_draw_indirect)(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_params *params)

/** Draw primitives using indirect parameters buffer with deferred count argument. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_draw_indirect_count)(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_count_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect_count(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_count_params *params)

/** Execute mesh shader workgroups directly. */
typedef void (LAKECALL *PFN_xaku_cmd_mesh_tasks)(struct xaku_command_recorder *cmd, u32 x, u32 y, u32 z);
#define FN_XAKU_CMD_MESH_TASKS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks(struct xaku_command_recorder *cmd, u32 x, u32 y, u32 z)

/** Execute mesh shader workgroups using indirect parameters buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_mesh_tasks_indirect)(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_params *params);
#define FN_XAKU_CMD_MESH_TASKS_INDIRECT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks_indirect(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_params *params)

/** Execute mesh shader workgroups using indirect parameters buffer with deferred count argument. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_mesh_tasks_indirect_count)(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_count_params *params);
#define FN_XAKU_CMD_MESH_TASKS_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks_indirect_count(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_count_params *params)

/** Execute ray tracing workload directly. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_trace_rays)(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_params *params);
#define FN_XAKU_CMD_TRACE_RAYS(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_params *params)

/** Execute ray tracing workload using indirect parameters buffer. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_cmd_trace_rays_indirect)(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_indirect_params *params);
#define FN_XAKU_CMD_TRACE_RAYS_INDIRECT(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays_indirect(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_indirect_params *params)

/** Record timestamp values into a query pool. */
typedef void (LAKECALL *PFN_xaku_cmd_write_timestamps)(struct xaku_command_recorder *cmd, const struct xaku_write_timestamp_params *params);
#define FN_XAKU_CMD_WRITE_TIMESTAMPS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_write_timestamps(struct xaku_command_recorder *cmd, const struct xaku_write_timestamp_params *params)

/** Reset timestamp values in the query pool. */
typedef void (LAKECALL *PFN_xaku_cmd_reset_timestamps)(struct xaku_command_recorder *cmd, const struct xaku_reset_timestamp_params *params);
#define FN_XAKU_CMD_RESET_TIMESTAMPS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_reset_timestamps(struct xaku_command_recorder *cmd, const struct xaku_reset_timestamp_params *params)

/** Begin a debug label in the command stream. */
typedef void (LAKECALL *PFN_xaku_cmd_begin_label)(struct xaku_command_recorder *cmd, const struct xaku_command_label_params *params);
#define FN_XAKU_CMD_BEGIN_LABEL(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_begin_label(struct xaku_command_recorder *cmd, const struct xaku_command_label_params *params)

/** End a debug label in the command stream. */
typedef void (LAKECALL *PFN_xaku_cmd_end_label)(struct xaku_command_recorder *cmd);
#define FN_XAKU_CMD_END_LABEL(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_end_label(struct xaku_command_recorder *cmd)

/** Compile the recorded commands into an executable buffer. Returns the index of the device in use. */
typedef lake_nodiscard enum lake_result (LAKECALL *PFN_xaku_compile_command_list)(struct xaku_command_recorder *cmd, u32 *out_device_index, struct xaku_executable_command_list **out_list);
#define FN_XAKU_COMPILE_COMMAND_LIST(ENCORE) \
    lake_nodiscard enum lake_result LAKECALL _xaku_##ENCORE##_compile_command_list(struct xaku_command_recorder *cmd, u32 *out_device_index, struct xaku_executable_command_list **out_list)

/******************************
 *                            * 
 *      PUBLIC INTERFACE      * 
 *                            * 
 ******************************/

/** Custom preprocessor for a pipeline builder. TODO filesystem path */
typedef void (LAKECALL *PFN_xaku_shader_custom_preprocessor)(const char *code, const char *path);

/** Assembly details for the shader pipeline builder. */
struct xake_pipeline_builder_assembly {
    lake_dynamic_array(const char *)                root_paths;                     /* TODO filesystem path */ 
    lake_optional(const char *)                     write_out_preprocessed_code;    /* TODO filesystem path */  
    lake_optional(const char *)                     write_out_spirv;                /* TODO filesystem path */  
    lake_optional(const char *)                     spirv_cache_folder;             /* TODO filesystem path */  
    PFN_xaku_shader_custom_preprocessor             custom_preprocessor;
    enum xaku_shader_language                       default_language;
    xaku_shader_create_flags                        default_create_flags;
    lake_dynamic_array(struct xaku_shader_define)   default_defines;
    lake_optional(u32)                              default_required_subgroup_size;
    bool                                            default_enable_debug_utils;
    bool                                            register_null_pipelines_when_first_compile_fails;
    lake_small_string                               name;
};

/** Controls how rendering work is done. */
enum xaku_rendering_strategy {
    /** Allow the initialization process to figure out what strategy is best, not a valid strategy. */
    xaku_rendering_strategy_auto = 0,
    /** Rendering is done on a single primary device, no mGPU-related scheduling or transfer overhead.
     *  It's the default */
    xaku_rendering_strategy_primary_optimal,
    /** Rendering on the discrete GPU is supported by the less-powerful integrated GPU.
     *  The rendering work is distributed such that the integrated GPU may not be a bottleneck. */
    xaku_rendering_strategy_dual_discrete_integrated,
    /** Multi-device rendering, explicit primary device and multiple secondary devices. 
     *  Sort first redistributes primitives early in the graphics pipeline, during geometry 
     *  processing. This is achieved by dividing the screenspace such that each GPU renders 
     *  it's own region, and the final image is composed from the contributing regions. */
    xaku_rendering_strategy_mgpu_sort_first,
    /** Multi-device rendering, explicit primary device and multiple secondary devices.
     *  Sort last is defined as deferring primitive redistribution until the end of the graphics 
     *  pipeline. It divides primitives such that each GPU renders it's own portion of mesh data 
     *  into the framebuffer. Then these are composed into the final image. */
    xaku_rendering_strategy_mgpu_sort_last,
};

/** A view onto a rendering device. */
union xaku_device_view { 
    struct {XAKU_INTERFACE_DEVICE_HEADER}  *header; 
    struct xaku_device                     *device; 
    union xaku_encore_view                 *encore_view; 
};

/** Views onto opaque device objects. */
#define _XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(T) \
    union xaku_##T##_view { \
        struct {_XAKU_INTERFACE_DEVICE_OBJECT_HEADER(T)} *header; \
        struct xaku_##T                                  *T; \
        union xaku_device_view                           *device_view; \
    };
/** A view into 'struct xaku_memory'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(memory)
/** A view into 'struct xaku_query_pool'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(query_pool)
/** A view into 'struct xaku_swapchain'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(swapchain)
/** A view into 'struct xaku_compute_pipeline'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(compute_pipeline)
/** A view into 'struct xaku_raster_pipeline'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(raster_pipeline)
/** A view into 'struct xaku_ray_tracing_pipeline'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(ray_tracing_pipeline)
/** A view into 'struct xaku_command_recorder'. */
_XAKU_INTERFACE_OPAQUE_OBJECT_VIEW(command_recorder)

/** A public interface of the rendering engine, implemented by a backend 'xaku_encore'. */
struct xaku_interface {
    struct riven_interface                          riven;
    lake_dynamic_array(union xaku_device_view)      devices;

    /* rendering settings */
    enum xaku_rendering_strategy                    rendering_strategy;

    /* procedures, implemented by the backend */
    PFN_xaku_list_devices_properties                list_devices_properties;
    PFN_xaku_device_assembly                        device_assembly;
    PFN_xaku_device_disassembly                     device_disassembly;
    PFN_xaku_device_queue_count                     device_queue_count;
    PFN_xaku_device_queue_wait_idle                 device_queue_wait_idle;
    PFN_xaku_device_wait_idle                       device_wait_idle;
    PFN_xaku_device_submit                          device_submit;
    PFN_xaku_device_present                         device_present;
    /** Should be called at some point, to destroy any zombified resources (memory, query pools, swapchains, pipelines, etc.). */
    PFN_xaku_deferred_resource_disassembly          deferred_resource_disassembly;

    PFN_xaku_memory_assembly                        memory_assembly;
    PFN_xaku_memory_disassembly                     memory_disassembly;
    PFN_xaku_memory_buffer_requirements             memory_buffer_requirements;
    PFN_xaku_memory_texture_requirements            memory_texture_requirements;

    PFN_xaku_query_pool_assembly                    query_pool_assembly;
    PFN_xaku_query_pool_disassembly                 query_pool_disassembly;

    PFN_xaku_swapchain_assembly                     swapchain_assembly;
    PFN_xaku_swapchain_disassembly                  swapchain_disassembly;

    PFN_xaku_compute_pipeline_assembly              compute_pipeline_assembly;
    PFN_xaku_compute_pipeline_disassembly           compute_pipeline_disassembly;

    PFN_xaku_raster_pipeline_assembly               raster_pipeline_assembly;
    PFN_xaku_raster_pipeline_disassembly            raster_pipeline_disassembly;

    PFN_xaku_ray_tracing_pipeline_assembly          ray_tracing_pipeline_assembly;
    PFN_xaku_ray_tracing_pipeline_disassembly       ray_tracing_pipeline_disassembly;

    PFN_xaku_command_recorder_assembly              command_recorder_assembly;
    PFN_xaku_command_recorder_disassembly           command_recorder_disassembly;

    PFN_xaku_create_buffer                          create_buffer;
    PFN_xaku_create_buffer_from_memory              create_buffer_from_memory;
    PFN_xaku_create_texture                         create_texture;
    PFN_xaku_create_texture_from_memory             create_texture_from_memory;
    PFN_xaku_create_texture_view                    create_texture_view;
    PFN_xaku_create_sampler                         create_sampler;
    PFN_xaku_create_tlas                            create_tlas;
    PFN_xaku_create_tlas_from_buffer                create_tlas_from_buffer;
    PFN_xaku_create_blas                            create_blas;
    PFN_xaku_create_blas_from_buffer                create_blas_from_buffer;

    PFN_xaku_get_buffer_assembly                    get_buffer_assembly;
    PFN_xaku_get_texture_assembly                   get_texture_assembly;
    PFN_xaku_get_texture_view_assembly              get_texture_view_assembly;
    PFN_xaku_get_sampler_assembly                   get_sampler_assembly;
    PFN_xaku_get_tlas_assembly                      get_tlas_assembly;
    PFN_xaku_get_tlas_build_sizes                   get_tlas_build_sizes;
    PFN_xaku_get_blas_assembly                      get_blas_assembly;
    PFN_xaku_get_blas_build_sizes                   get_blas_build_sizes;

    PFN_xaku_destroy_buffer                         destroy_buffer;
    PFN_xaku_destroy_texture                        destroy_texture;
    PFN_xaku_destroy_texture_view                   destroy_texture_view;
    PFN_xaku_destroy_sampler                        destroy_sampler;
    PFN_xaku_destroy_tlas                           destroy_tlas;
    PFN_xaku_destroy_blas                           destroy_blas;

    PFN_xaku_cmd_copy_buffer_to_buffer              cmd_copy_buffer_to_buffer;
    PFN_xaku_cmd_copy_buffer_to_texture             cmd_copy_buffer_to_texture;
    PFN_xaku_cmd_copy_texture_to_buffer             cmd_copy_texture_to_buffer;
    PFN_xaku_cmd_copy_texture_to_texture            cmd_copy_texture_to_texture;
    PFN_xaku_cmd_blit_texture_to_texture            cmd_blit_texture_to_texture;
    PFN_xaku_cmd_clear_buffer                       cmd_clear_buffer;
    PFN_xaku_cmd_clear_texture                      cmd_clear_texture;
    PFN_xaku_cmd_build_acceleration_structures      cmd_build_acceleration_structures;
    PFN_xaku_cmd_destroy_buffer_deferred            cmd_destroy_buffer_deferred;
    PFN_xaku_cmd_destroy_texture_deferred           cmd_destroy_texture_deferred;
    PFN_xaku_cmd_destroy_texture_view_deferred      cmd_destroy_texture_view_deferred;
    PFN_xaku_cmd_destroy_sampler_deferred           cmd_destroy_sampler_deferred;
    PFN_xaku_cmd_destroy_tlas_deferred              cmd_destroy_tlas_deferred;
    PFN_xaku_cmd_destroy_blas_deferred              cmd_destroy_blas_deferred;
    PFN_xaku_cmd_push_constant                      cmd_push_constant;
    PFN_xaku_cmd_set_compute_pipeline               cmd_set_compute_pipeline;
    PFN_xaku_cmd_set_raster_pipeline                cmd_set_raster_pipeline;
    PFN_xaku_cmd_set_ray_tracing_pipeline           cmd_set_ray_tracing_pipeline;
    PFN_xaku_cmd_set_viewport                       cmd_set_viewport;
    PFN_xaku_cmd_set_scissor                        cmd_set_scissor;
    PFN_xaku_cmd_set_depth_bias                     cmd_set_depth_bias;
    PFN_xaku_cmd_set_index_buffer                   cmd_set_index_buffer;
    PFN_xaku_cmd_begin_rendering                    cmd_begin_rendering;
    PFN_xaku_cmd_end_rendering                      cmd_end_rendering;
    PFN_xaku_cmd_dispatch                           cmd_dispatch;
    PFN_xaku_cmd_dispatch_indirect                  cmd_dispatch_indirect;
    PFN_xaku_cmd_draw                               cmd_draw;
    PFN_xaku_cmd_draw_indexed                       cmd_draw_indexed;
    PFN_xaku_cmd_draw_indirect                      cmd_draw_indirect;
    PFN_xaku_cmd_draw_indirect_count                cmd_draw_indirect_count;
    PFN_xaku_cmd_mesh_tasks                         cmd_mesh_tasks;
    PFN_xaku_cmd_mesh_tasks_indirect                cmd_mesh_tasks_indirect;
    PFN_xaku_cmd_mesh_tasks_indirect_count          cmd_mesh_tasks_indirect_count;
    PFN_xaku_cmd_trace_rays                         cmd_trace_rays;
    PFN_xaku_cmd_trace_rays_indirect                cmd_trace_rays_indirect;
    PFN_xaku_cmd_write_timestamps                   cmd_write_timestamps;
    PFN_xaku_cmd_reset_timestamps                   cmd_reset_timestamps;
    PFN_xaku_cmd_begin_label                        cmd_begin_label;
    PFN_xaku_cmd_end_label                          cmd_end_label;
    PFN_xaku_compile_command_list                   compile_command_list;
};
union xaku_renderer {
    struct riven_interface     *riven;
    struct xaku_interface      *interface;
    struct xaku_encore         *encore;
    void                       *data;
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
