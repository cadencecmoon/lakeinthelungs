#pragma once

#include <amwe/xaku/encore.h>
#include <amwe/xaku/gpu_resources.h>
#include <amwe/xaku/pipelines.h>
#include <amwe/xaku/commands.h>
#include <amwe/hadal.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

enum xaku_missing_required_feature_bits {
    xaku_missing_required_feature_none                                                  = 0,
    xaku_missing_required_feature_image_cube_array                                      = (1ull << 0),
    xaku_missing_required_feature_independent_blend                                     = (1ull << 1),
    xaku_missing_required_feature_tesselation_shader                                    = (1ull << 2),
    xaku_missing_required_feature_multi_draw_indirect                                   = (1ull << 3),
    xaku_missing_required_feature_depth_clamp                                           = (1ull << 4),
    xaku_missing_required_feature_fill_mode_non_solid                                   = (1ull << 5),
    xaku_missing_required_feature_wide_lines                                            = (1ull << 6),
    xaku_missing_required_feature_sampler_anisotrophy                                   = (1ull << 7),
    xaku_missing_required_feature_fragment_stores_and_atomics                           = (1ull << 8),
    xaku_missing_required_feature_shader_storage_image_multisample                      = (1ull << 9),
    xaku_missing_required_feature_shader_storage_image_read_without_format              = (1ull << 10),
    xaku_missing_required_feature_shader_storage_image_write_without_format             = (1ull << 11),
    xaku_missing_required_feature_shader_int64                                          = (1ull << 12),
    xaku_missing_required_feature_variable_pointers_storage_buffer                      = (1ull << 13),
    xaku_missing_required_feature_variable_pointers                                     = (1ull << 14),
    xaku_missing_required_feature_buffer_device_address                                 = (1ull << 15),
    xaku_missing_required_feature_buffer_device_address_capture_replay                  = (1ull << 16),
    xaku_missing_required_feature_buffer_device_address_multi_device                    = (1ull << 17),
    xaku_missing_required_feature_shader_sampled_image_array_non_uniform_indexing       = (1ull << 18),
    xaku_missing_required_feature_shader_storage_buffer_array_non_uniform_indexing      = (1ull << 19),
    xaku_missing_required_feature_shader_storage_image_array_non_uniform_indexing       = (1ull << 20),
    xaku_missing_required_feature_descriptor_binding_sampled_image_update_after_bind    = (1ull << 21),
    xaku_missing_required_feature_descriptor_binding_storage_image_update_after_bind    = (1ull << 22),
    xaku_missing_required_feature_descriptor_binding_storage_buffer_update_after_bind   = (1ull << 23),
    xaku_missing_required_feature_descriptor_binding_update_unused_while_pending        = (1ull << 24),
    xaku_missing_required_feature_runtime_descriptor_array                              = (1ull << 25),
    xaku_missing_required_feature_host_query_reset                                      = (1ull << 26),
    xaku_missing_required_feature_khr_dynamic_rendering_local_read                      = (1ull << 27),
    xaku_missing_required_feature_khr_dynamic_rendering                                 = (1ull << 28),
    xaku_missing_required_feature_khr_synchronization2                                  = (1ull << 29),
    xaku_missing_required_feature_timeline_semaphore                                    = (1ull << 30),
    xaku_missing_required_feature_subgroup_size_control                                 = (1ull << 31),
    xaku_missing_required_feature_compute_full_subgroups                                = (1ull << 32),
    xaku_missing_required_feature_scalar_block_layout                                   = (1ull << 33),
    xaku_missing_required_feature_acceleration_structure_capture_replay                 = (1ull << 34),
    xaku_missing_required_feature_vulkan_memory_model                                   = (1ull << 35),
    xaku_missing_required_feature_robust_buffer_access2                                 = (1ull << 36),
    xaku_missing_required_feature_robust_image_access2                                  = (1ull << 37),
};

enum xaku_explicit_feature_bits {
    xaku_explicit_feature_none                                                          = 0,
    xaku_explicit_feature_buffer_device_address_capture_replay                          = (1u << 0),
    xaku_explicit_feature_acceleration_structure_capture_replay                         = (1u << 1),
    xaku_explicit_feature_vulkan_memory_model                                           = (1u << 2),
    xaku_explicit_feature_robustness2                                                   = (1u << 3),
    xaku_explicit_feature_video_decode_av1                                              = (1u << 4),
    xaku_explicit_feature_video_decode_h264                                             = (1u << 5),
};

enum xaku_implicit_feature_bits {
    xaku_implicit_feature_none                                                          = 0,
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

struct xaku_device_properties {
    u32                                                     api_version;
    u32                                                     driver_version;
    u32                                                     vendor_id;
    u32                                                     device_id;
    enum xaku_device_type                                   device_type;
    char                                                    device_name[256u];
    char                                                    pipeline_cache_uuid[16u];
    struct xaku_device_limits                               limits;
    struct xaku_mesh_shader_properties                      mesh_shader_properties;
    struct xaku_ray_tracing_pipeline_properties             ray_tracing_pipeline_properties;
    struct xaku_ray_tracing_invocation_reorder_properties   ray_tracing_invocation_reorder_properties;
    struct xaku_acceleration_structure_properties           acceleration_structure_properties;
    bool                                                    has_mesh_shader_properties;
    bool                                                    has_ray_tracing_pipeline_properties;
    bool                                                    has_ray_tracing_invocation_reoder_properties;
    bool                                                    has_acceleration_structure_properties;
    u32                                                     compute_queue_count;
    u32                                                     transfer_queue_count;
    u32                                                     implicit_features;          /**< enum xaku_implicit_feature_bits  */
    u32                                                     explicit_features;          /**< enum xaku_explicit_feature_bits */
    u64                                                     missing_required_feature;   /**< enum xaku_missing_required_feature_bits */
}; 

struct xaku_device_assembly {
    /** Index into a list of devices returned from PFN_xaku_encore_list_devices_properties. */
    u32                     physical_device_index;
    /** Index into an external list of devices where this handle will be used within a mGPU context. */
    u32                     assign_index;
    /** Explicit features must be manually enabled. */
    u32                     explicit_features;
    u32                     max_allowed_textures;
    u32                     max_allowed_buffers;
    u32                     max_allowed_samplers;
    u32                     max_allowed_acceleration_structures;
    lake_small_string       name;
};
#define XAKU_DEFAULT_DEVICE_ASSEMBLY { \
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

struct xaku_memory_requirements {
    u64 size;
    u64 alignment;
    u32 memory_type_bits;
};

struct xaku_memory_assembly {
    struct xaku_memory_requirements requirements;
    u32                             flags; /**< enum xaku_memory_flag_bits */
};
#define XAKU_DEFAULT_MEMORY_ASSEMBLY LAKE_ZERO_INIT

struct xaku_allocation {
    struct xaku_memory             *memory;
    u64                             offset;
    u64                             range;
};

struct xaku_query_pool_assembly {
    u32                             query_count;
    lake_small_string               name;
};
#define XAKU_DEFAULT_QUERY_POOL_ASSEMBLY LAKE_ZERO_INIT

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

struct xaku_swapchain_assembly {
    /** A view into a native window, obtained from Hadal. */
    union hadal_window_view         native_window;
    /** The preferred present mode will be selected if available. */
    enum xaku_present_mode          present_mode;
    /** enum xaku_present_transform_bits */
    u32                             present_transform;
    /** enum xaku_texture_usage_bits */
    u32                             image_usage;
    /** Timeline semaphores will be cycled by this value. */
    u32                             max_allowed_frames_in_flight;
    /** This queue family may be used for presentation. */
    enum xaku_queue_type            queue_type;
    /** Debug name of the swapchain. */
    lake_small_string               name;
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

typedef void (LAKECALL *PFN_xaku_list_devices_properties)(struct xaku_encore *encore, u32 *property_count, const struct xaku_device_properties **properties);
#define FN_XAKU_LIST_DEVICES_PROPERTIES(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_list_devices_properties(struct xaku_encore *encore, u32 *property_count, const struct xaku_device_properties **properties)

PFN_XAKU_ASSEMBLY(encore, device);
PFN_XAKU_DISASSEMBLY(device);
#define FN_XAKU_DEVICE_ASSEMBLY(ENCORE)     FN_XAKU_ASSEMBLY(ENCORE, encore, device)
#define FN_XAKU_DEVICE_DISASSEMBLY(ENCORE)  FN_XAKU_DISASSEMBLY(ENCORE, device)

/** Retrieve the number of queues available for a given queue type. On optional queues
 *  (sparse binding, video decode/encode), 0 means that these commands are unavailable. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_device_queue_count)(struct xaku_device *device, enum xaku_queue_type queue_type, u32 *out_count);
#define FN_XAKU_DEVICE_QUEUE_COUNT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_device_queue_count(struct xaku_device *device, enum xaku_queue_type queue_type, u32 *out_count)

/** Wait until GPU work on a given queue is done. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_device_queue_wait_idle)(struct xaku_device *device, struct xaku_queue queue);
#define FN_XAKU_DEVICE_QUEUE_WAIT_IDLE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_device_queue_wait_idle(struct xaku_device *device, struct xaku_queue queue)

/** Wait until all current GPU work is done. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_device_wait_idle)(struct xaku_device *device);
#define FN_XAKU_DEVICE_WAIT_IDLE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_device_wait_idle(struct xaku_device *device)

/** Submit work into a command queue. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_device_submit)(struct xaku_device *device, struct xaku_executable_command_list *cmd_list);
#define FN_XAKU_DEVICE_SUBMIT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_device_submit(struct xaku_device *device, struct xaku_executable_command_list *cmd_list)

/** Present swapchain images, used by a primary device that supports presentation to the window surface. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_device_present)(struct xaku_device *device, const struct xaku_swapchain **swapchains, u32 swapchain_count);
#define FN_XAKU_DEVICE_PRESENT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_device_present(struct xaku_device *device, const struct xaku_swapchain **swapchains, u32 swapchain_count)

/** Execute the deferred disassembly work as jobs, from disassembly zero reference callbacks of zombie GPU resources.
 *  Whether a resource becomes a zombie or not is decided in the backend, at a disassembly call. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_deferred_resource_disassembly)(struct xaku_device *device);
#define FN_XAKU_DEFERRED_RESOURCE_DISASSEMBLY(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_deferred_resource_disassembly(struct xaku_device *device)

PFN_XAKU_ASSEMBLY(device, memory);
PFN_XAKU_DISASSEMBLY(memory);
#define FN_XAKU_MEMORY_ASSEMBLY(ENCORE)                     FN_XAKU_ASSEMBLY(ENCORE, device, memory)
#define FN_XAKU_MEMORY_DISASSEMBLY(ENCORE)                  FN_XAKU_DISASSEMBLY(ENCORE, memory)

/** Get GPU memory requirements for a buffer. */
typedef void (LAKECALL *PFN_xaku_memory_buffer_requirements)(struct xaku_device *device, const struct xaku_buffer_assembly *assembly, struct xaku_memory_requirements *out_requirements);
#define FN_XAKU_MEMORY_BUFFER_REQUIREMENTS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_memory_buffer_requirements(struct xaku_device *device, const struct xaku_buffer_assembly *assembly, struct xaku_memory_requirements *out_requirements)

/** Get GPU memory requirements for a texture. */
typedef void (LAKECALL *PFN_xaku_memory_texture_requirements)(struct xaku_device *device, const struct xaku_texture_assembly *assembly, struct xaku_memory_requirements *out_requirements);
#define FN_XAKU_MEMORY_TEXTURE_REQUIREMENTS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_memory_texture_requirements(struct xaku_device *device, const struct xaku_texture_assembly *assembly, struct xaku_memory_requirements *out_requirements)

PFN_XAKU_ASSEMBLY(device, query_pool);
PFN_XAKU_DISASSEMBLY(query_pool);
#define FN_XAKU_QUERY_POOL_ASSEMBLY(ENCORE)                 FN_XAKU_ASSEMBLY(ENCORE, device, query_pool)
#define FN_XAKU_QUERY_POOL_DISASSEMBLY(ENCORE)              FN_XAKU_DISASSEMBLY(ENCORE, query_pool)

PFN_XAKU_ASSEMBLY(device, swapchain);
PFN_XAKU_DISASSEMBLY(swapchain);
#define FN_XAKU_SWAPCHAIN_ASSEMBLY(ENCORE)                  FN_XAKU_ASSEMBLY(ENCORE, device, swapchain)
#define FN_XAKU_SWAPCHAIN_DISASSEMBLY(ENCORE)               FN_XAKU_DISASSEMBLY(ENCORE, swapchain)

PFN_XAKU_ASSEMBLY(device, compute_pipeline);
PFN_XAKU_DISASSEMBLY(compute_pipeline);
#define FN_XAKU_COMPUTE_PIPELINE_ASSEMBLY(ENCORE)           FN_XAKU_ASSEMBLY(ENCORE, device, compute_pipeline)
#define FN_XAKU_COMPUTE_PIPELINE_DISASSEMBLY(ENCORE)        FN_XAKU_DISASSEMBLY(ENCORE, compute_pipeline)

PFN_XAKU_ASSEMBLY(device, raster_pipeline);
PFN_XAKU_DISASSEMBLY(raster_pipeline);
#define FN_XAKU_RASTER_PIPELINE_ASSEMBLY(ENCORE)            FN_XAKU_ASSEMBLY(ENCORE, device, raster_pipeline)
#define FN_XAKU_RASTER_PIPELINE_DISASSEMBLY(ENCORE)         FN_XAKU_DISASSEMBLY(ENCORE, raster_pipeline)

PFN_XAKU_ASSEMBLY(device, ray_tracing_pipeline);
PFN_XAKU_DISASSEMBLY(ray_tracing_pipeline);
#define FN_XAKU_RAY_TRACING_PIPELINE_ASSEMBLY(ENCORE)       FN_XAKU_ASSEMBLY(ENCORE, device, ray_tracing_pipeline)
#define FN_XAKU_RAY_TRACING_PIPELINE_DISASSEMBLY(ENCORE)    FN_XAKU_DISASSEMBLY(ENCORE, ray_tracing_pipeline)

PFN_XAKU_ASSEMBLY(device, command_recorder);
PFN_XAKU_DISASSEMBLY(command_recorder);
#define FN_XAKU_COMMAND_RECORDER_ASSEMBLY(ENCORE)           FN_XAKU_ASSEMBLY(ENCORE, device, command_recorder)
#define FN_XAKU_COMMAND_RECORDER_DISASSEMBLY(ENCORE)        FN_XAKU_DISASSEMBLY(ENCORE, command_recorder)

/** Create a GPU resource using a render handle. */
#define PFN_XAKU_CREATE_RESOURCE(NAME, T, ID) \
    typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_create_##NAME)(struct xaku_device *device, const struct xaku_##T##_assembly *assembly, xaku_##ID##_id *out_##ID);
#define FN_XAKU_CREATE_RESOURCE(ENCORE, NAME, T, ID) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_create_##NAME(struct xaku_device *device, const struct xaku_##T##_assembly *assembly, xaku_##ID##_id *out_##ID)

PFN_XAKU_CREATE_RESOURCE(buffer, buffer, buffer)
PFN_XAKU_CREATE_RESOURCE(buffer_from_memory, buffer_memory, buffer)
PFN_XAKU_CREATE_RESOURCE(texture, texture, texture)
PFN_XAKU_CREATE_RESOURCE(texture_from_memory, texture_memory, texture)
PFN_XAKU_CREATE_RESOURCE(texture_view, texture_view, texture_view)
PFN_XAKU_CREATE_RESOURCE(sampler, sampler, sampler)
PFN_XAKU_CREATE_RESOURCE(tlas, tlas, tlas)
PFN_XAKU_CREATE_RESOURCE(blas, blas, blas)
PFN_XAKU_CREATE_RESOURCE(tlas_from_buffer, buffer_tlas, tlas)
PFN_XAKU_CREATE_RESOURCE(blas_from_buffer, buffer_blas, blas)
#define FN_XAKU_CREATE_BUFFER(ENCORE)                FN_XAKU_CREATE_RESOURCE(ENCORE, buffer, buffer, buffer)
#define FN_XAKU_CREATE_BUFFER_FROM_MEMORY(ENCORE)    FN_XAKU_CREATE_RESOURCE(ENCORE, buffer_from_memory, buffer_memory, buffer)
#define FN_XAKU_CREATE_TEXTURE(ENCORE)               FN_XAKU_CREATE_RESOURCE(ENCORE, texture, texture, texture)
#define FN_XAKU_CREATE_TEXTURE_FROM_MEMORY(ENCORE)   FN_XAKU_CREATE_RESOURCE(ENCORE, texture_from_memory, texture_memory, texture)
#define FN_XAKU_CREATE_TEXTURE_VIEW(ENCORE)          FN_XAKU_CREATE_RESOURCE(ENCORE, texture_view, texture_view, texture_view)
#define FN_XAKU_CREATE_SAMPLER(ENCORE)               FN_XAKU_CREATE_RESOURCE(ENCORE, sampler, sampler, sampler)
#define FN_XAKU_CREATE_TLAS(ENCORE)                  FN_XAKU_CREATE_RESOURCE(ENCORE, tlas, tlas, tlas)
#define FN_XAKU_CREATE_BLAS(ENCORE)                  FN_XAKU_CREATE_RESOURCE(ENCORE, blas, blas, blas)
#define FN_XAKU_CREATE_TLAS_FROM_BUFFER(ENCORE)      FN_XAKU_CREATE_RESOURCE(ENCORE, tlas_from_buffer, buffer_tlas, tlas)
#define FN_XAKU_CREATE_BLAS_FROM_BUFFER(ENCORE)      FN_XAKU_CREATE_RESOURCE(ENCORE, blas_from_buffer, buffer_blas, blas)

/** Copy the assembly details used to create this resource. */
#define PFN_XAKU_GET_RESOURCE_ASSEMBLY(T) \
    typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_get_##T##_assembly)(struct xaku_device *device, xaku_##T##_id T, struct xaku_##T##_assembly *out_assembly);
#define FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, T) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_get_##T##_assembly(struct xaku_device *device, xaku_##T##_id T, struct xaku_##T##_assembly *out_assembly)

PFN_XAKU_GET_RESOURCE_ASSEMBLY(buffer)
PFN_XAKU_GET_RESOURCE_ASSEMBLY(texture)
PFN_XAKU_GET_RESOURCE_ASSEMBLY(texture_view)
PFN_XAKU_GET_RESOURCE_ASSEMBLY(sampler)
PFN_XAKU_GET_RESOURCE_ASSEMBLY(tlas)
PFN_XAKU_GET_RESOURCE_ASSEMBLY(blas)
#define FN_XAKU_GET_BUFFER_ASSEMBLY(ENCORE)         FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, buffer)
#define FN_XAKU_GET_TEXTURE_ASSEMBLY(ENCORE)        FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, texture)
#define FN_XAKU_GET_TEXTURE_VIEW_ASSEMBLY(ENCORE)   FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, texture_view)
#define FN_XAKU_GET_SAMPLER_ASSEMBLY(ENCORE)        FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, sampler)
#define FN_XAKU_GET_TLAS_ASSEMBLY(ENCORE)           FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, tlas)
#define FN_XAKU_GET_BLAS_ASSEMBLY(ENCORE)           FN_XAKU_GET_RESOURCE_ASSEMBLY(ENCORE, blas)

typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_get_tlas_build_sizes)(struct xaku_device *device, const struct xaku_tlas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes);
#define FN_XAKU_GET_TLAS_BUILD_SIZES(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_get_tlas_build_sizes(struct xaku_device *device, const struct xaku_tlas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes)

typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_get_blas_build_sizes)(struct xaku_device *device, const struct xaku_blas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes);
#define FN_XAKU_GET_BLAS_BUILD_SIZES(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_get_blas_build_sizes(struct xaku_device *device, const struct xaku_blas_assembly *assembly, struct xaku_acceleration_structure_build_sizes *out_sizes)

/** Destroy a GPU resource using a render handle. */
#define PFN_XAKU_DESTROY_RESOURCE(T) \
    typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_destroy_##T)(struct xaku_device *device, xaku_##T##_id T);
#define FN_XAKU_DESTROY_RESOURCE(ENCORE, T) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_destroy_##T(struct xaku_device *device, xaku_##T##_id T)

PFN_XAKU_DESTROY_RESOURCE(buffer)
PFN_XAKU_DESTROY_RESOURCE(texture)
PFN_XAKU_DESTROY_RESOURCE(texture_view)
PFN_XAKU_DESTROY_RESOURCE(sampler)
PFN_XAKU_DESTROY_RESOURCE(tlas)
PFN_XAKU_DESTROY_RESOURCE(blas)
#define FN_XAKU_DESTROY_BUFFER(ENCORE)               FN_XAKU_DESTROY_RESOURCE(ENCORE, buffer)
#define FN_XAKU_DESTROY_TEXTURE(ENCORE)              FN_XAKU_DESTROY_RESOURCE(ENCORE, texture)
#define FN_XAKU_DESTROY_TEXTURE_VIEW(ENCORE)         FN_XAKU_DESTROY_RESOURCE(ENCORE, texture_view)
#define FN_XAKU_DESTROY_SAMPLER(ENCORE)              FN_XAKU_DESTROY_RESOURCE(ENCORE, sampler)
#define FN_XAKU_DESTROY_TLAS(ENCORE)                 FN_XAKU_DESTROY_RESOURCE(ENCORE, tlas)
#define FN_XAKU_DESTROY_BLAS(ENCORE)                 FN_XAKU_DESTROY_RESOURCE(ENCORE, blas)

/** A view into a rendering device. */
union xaku_device_view {
    struct {XAKU_INTERFACE_DEVICE_HEADER}  *header;
    struct xaku_device                     *device;
    union xaku_encore_view                 *encore_view;
};

/** A view into a device resource structure that has a reference count.
 *  For buffers, textures, texture views, samplers and acceleration structures,
 *  render handles are used instead (amwe/renderer/gpu_resources.h). */
#define UNION_XAKU_REFERENCED_RESOURCE_VIEW(T) \
    union xaku_##T##_view { struct {XAKU_INTERFACE_DEVICE_RESOURCE_HEADER(T)} *header; struct xaku_##T *T; }
UNION_XAKU_REFERENCED_RESOURCE_VIEW(memory);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(query_pool);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(swapchain);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(compute_pipeline);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(raster_pipeline);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(ray_tracing_pipeline);
UNION_XAKU_REFERENCED_RESOURCE_VIEW(command_recorder);

#ifdef __cplusplus
}
#endif /* __cplusplus */
