#pragma once

#include <amwe/xaku/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum xaku_shader_stage_bits {
    xaku_shader_stage_vertex                 = (1u << 0),
    xaku_shader_stage_tesselation_control    = (1u << 1),
    xaku_shader_stage_tesselation_evaluation = (1u << 2),
    xaku_shader_stage_geometry               = (1u << 3),
    xaku_shader_stage_fragment               = (1u << 4),
    xaku_shader_stage_compute                = (1u << 5),
    xaku_shader_stage_all_raster             = 0x0000001F,
    xaku_shader_stage_all                    = 0x7FFFFFFF,
    /* mesh shader */
    xaku_shader_stage_task                   = (1u << 6),
    xaku_shader_stage_mesh                   = (1u << 7),
    /* raytracing */
    xaku_shader_stage_raygen                 = (1u << 8),
    xaku_shader_stage_any_hit                = (1u << 9),
    xaku_shader_stage_closest_hit            = (1u << 10),
    xaku_shader_stage_miss                   = (1u << 11),
    xaku_shader_stage_intersection           = (1u << 12),
    xaku_shader_stage_callable               = (1u << 13),
};

struct xaku_shader_params {
    const u32                              *byte_code;
    u32                                     byte_code_size;
    u32                                     stage_flags; /**< enum xaku_shader_stage_bits */
    u32                                     required_subgroup_size;
    lake_small_string                       entry_point;
};

struct xaku_compute_pipeline_assembly {
    struct xaku_shader_params               shader;
    u32                                     push_constant_size;
    lake_small_string                       name;
};

struct xaku_depth_test_params {
    enum xaku_format                        depth_attachment_format;
    bool                                    enable_depth_write;
    enum xaku_compare_op                    depth_test_compare_operation;
    f32                                     min_depth_bounds;
    f32                                     max_depth_bounds;
};

struct xaku_stencil_test_params {
    u8                                      test_mask; /* 0xffu */
    u8                                      write_mask;
    enum xaku_compare_op                    stencil_test_compare_operation; /* always */
    enum xaku_stencil_op                    stencil_op_fail; /* keep */
    enum xaku_stencil_op                    stencil_op_depth_fail; /* keep */
    enum xaku_stencil_op                    stencil_op_depth_pass; /* keep */
};

struct xaku_rasterizer_params {
    enum xaku_primitive_topology            primitive_topology;
    enum xaku_polygon_mode                  polygon_mode;
    enum xaku_cull_mode                     face_culling;
    bool                                    primitive_restart_enable;
    bool                                    depth_clamp_enable;
    bool                                    rasterizer_discard_enable;
    bool                                    depth_bias_enable;
    f32                                     depth_bias_constant_factor;
    f32                                     depth_bias_clamp;
    f32                                     depth_bias_slope_factor;
    f32                                     line_width;
    u32                                     static_state_sample_count;
};

struct xaku_blend_params {
    enum xaku_blend_factor                  src_color_blend_factor;
    enum xaku_blend_factor                  dst_color_blend_factor;
    enum xaku_blend_op                      color_blend_op;
    enum xaku_blend_factor                  src_alpha_blend_factor;
    enum xaku_blend_factor                  dst_alpha_blend_factor;
    enum xaku_blend_op                      alpha_blend_op;
    u32                                     color_write_mask; /**< enum xaku_color_component_flag_bits */
};

struct xaku_render_attachment {
    enum xaku_format                        format;
    bool                                    has_blend;
    struct xaku_blend_params                blend;
};

enum xaku_tesselation_domain_origin {
    xaku_tesselation_domain_origin_upper_left = 0,
    xaku_tesselation_domain_origin_lower_left,
};

struct xaku_tesselation_params {
    u32                                     control_points;
    enum xaku_tesselation_domain_origin     origin;
};

struct xaku_raster_pipeline_assembly {
    struct xaku_shader_params                                      *mesh_shader;
    struct xaku_shader_params                                      *vertex_shader;
    struct xaku_shader_params                                      *tesselation_control_shader;
    struct xaku_shader_params                                      *tesselation_evaluation_shader;
    struct xaku_shader_params                                      *geometry_shader;
    struct xaku_shader_params                                      *fragment_shader;
    struct xaku_shader_params                                      *task_shader;
    lake_fixed_list(struct xaku_render_attachment, 8)               color_attachments;
    struct xaku_depth_test_params                                  *depth_test;
    struct xaku_stencil_test_params                                *stencil_test;
    struct xaku_tesselation_params                                 *tesselation;
    bool has_mesh_shader;
    bool has_vertex_shader;
    bool has_tesselation_control_shader;
    bool has_tesselation_evaluation_shader;
    bool has_tesselation;
    bool has_geometry_shader;
    bool has_fragment_shader;
    bool has_task_shader;
    bool has_depth_test;
    bool has_stencil_test;
};

enum xaku_ray_tracing_shader_group_type {
    xaku_ray_tracing_shader_group_type_ = 0,
};

struct xaku_ray_tracing_shader_group_params {
    enum xaku_ray_tracing_shader_group_type type;
    u32                                     general_shader_index;
    u32                                     closest_hit_shader_index;
    u32                                     any_hit_shader_index;
    u32                                     intersection_shader_index;
};

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

#ifdef __cplusplus
}
#endif /* __cplusplus */
