#pragma once

#include <amwe/xaku/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum xaku_device_type {
    xaku_device_type_other = 0,
    xaku_device_type_integrated_gpu,
    xaku_device_type_discrete_gpu,
    xaku_device_type_virtual_gpu,
    xaku_device_type_cpu,
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

enum xaku_missing_required_feature {
    xaku_missing_required_feature_none = 0,
    xaku_missing_required_feature_image_cube_array,
    xaku_missing_required_feature_independent_blend,
    xaku_missing_required_feature_tesselation_shader,
    xaku_missing_required_feature_multi_draw_indirect,
    xaku_missing_required_feature_depth_clamp,
    xaku_missing_required_feature_fill_mode_non_solid,
    xaku_missing_required_feature_wide_lines,
    xaku_missing_required_feature_sampler_anisotrophy,
    xaku_missing_required_feature_fragment_stores_and_atomics,
    xaku_missing_required_feature_shader_storage_image_multisample,
    xaku_missing_required_feature_shader_storage_image_read_without_format,
    xaku_missing_required_feature_shader_storage_image_write_without_format,
    xaku_missing_required_feature_shader_int64,
    xaku_missing_required_feature_variable_pointers_storage_buffer,
    xaku_missing_required_feature_variable_pointers,
    xaku_missing_required_feature_buffer_device_address,
    xaku_missing_required_feature_buffer_device_address_capture_replay,
    xaku_missing_required_feature_buffer_device_address_multi_device,
    xaku_missing_required_feature_shader_sampled_image_array_non_uniform_indexing,
    xaku_missing_required_feature_shader_storage_buffer_array_non_uniform_indexing,
    xaku_missing_required_feature_shader_storage_image_array_non_uniform_indexing,
    xaku_missing_required_feature_descriptor_binding_sampled_image_update_after_bind,
    xaku_missing_required_feature_descriptor_binding_storage_image_update_after_bind,
    xaku_missing_required_feature_descriptor_binding_storage_buffer_update_after_bind,
    xaku_missing_required_feature_descriptor_binding_update_unused_while_pending,
    xaku_missing_required_feature_runtime_descriptor_array,
    xaku_missing_required_feature_host_query_reset,
    xaku_missing_required_feature_khr_dynamic_rendering_local_read,
    xaku_missing_required_feature_khr_dynamic_rendering,
    xaku_missing_required_feature_khr_synchronization2,
    xaku_missing_required_feature_timeline_semaphore,
    xaku_missing_required_feature_subgroup_size_control,
    xaku_missing_required_feature_compute_full_subgroups,
    xaku_missing_required_feature_scalar_block_layout,
    xaku_missing_required_feature_acceleration_structure_capture_replay,
    xaku_missing_required_feature_vulkan_memory_model,
    xaku_missing_required_feature_robust_buffer_access2,
    xaku_missing_required_feature_robust_image_access2,
    xaku_missing_required_feature_max_enum,
};

enum xaku_explicit_feature_flag_bits {
    xaku_explicit_feature_flag_none                                     = 0,
    xaku_explicit_feature_flag_buffer_device_address_capture_replay     = (1u << 0),
    xaku_explicit_feature_flag_acceleration_structure_capture_replay    = (1u << 1),
    xaku_explicit_feature_flag_vulkan_memory_model                      = (1u << 2),
    xaku_explicit_feature_flag_robustness2                              = (1u << 3),
};

enum xaku_implicit_feature_flag_bits {
    xaku_implicit_feature_flag_none                                     = 0,
    xaku_implicit_feature_flag_mesh_shader                              = (1u << 0),
    xaku_implicit_feature_flag_basic_ray_tracing                        = (1u << 1),
    xaku_implicit_feature_flag_ray_tracing_pipeline                     = (1u << 2),
    xaku_implicit_feature_flag_ray_tracing_invocation_reorder           = (1u << 3),
    xaku_implicit_feature_flag_ray_tracing_position_fetch               = (1u << 4),
    xaku_implicit_feature_flag_conservative_rasterization               = (1u << 5),
    xaku_implicit_feature_flag_image_atomic64                           = (1u << 6),
    xaku_implicit_feature_flag_shader_atomic_float                      = (1u << 7),
    xaku_implicit_feature_flag_shader_atomic_int64                      = (1u << 8),
    xaku_implicit_feature_flag_shader_float16                           = (1u << 9),
    xaku_implicit_feature_flag_shader_int16                             = (1u << 10),
    xaku_implicit_feature_flag_shader_int8                              = (1u << 11),
    xaku_implicit_feature_flag_swapchain                                = (1u << 12),
    xaku_implicit_feature_flag_khr_dynamic_state3                       = (1u << 13),
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
    u32                                                     implicit_features; /**< enum xaku_implicit_feature_flag_bits  */
    u32                                                     explicit_features; /**< enum xaku_explicit_feature_flag_bits */
    enum xaku_missing_required_feature                      missing_required_feature;
}; 

#ifdef __cplusplus
}
#endif /* __cplusplus */
