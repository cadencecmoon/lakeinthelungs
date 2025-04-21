#pragma once

#include <amwe/renderer/encore.h>
#include <amwe/renderer/gpu_resources.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define XAKU_SHADER_UNUSED (~0u)

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
#define XAKU_DEFAULT_COMPUTE_PIPELINE_ASSEMBLY { \
    .shader = LAKE_ZERO_INIT, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

/** The first member of a 'xaku_compute_pipeline' struct implementation. */
struct xaku_compute_pipeline_header {
    /** Device that owns this memory block. */
    struct xaku_device                     *device; 
    /** The memory may be released when the reference count reaches zero. */
    atomic_u64                              refcnt;
    /** The assembly details used to create this pipeline. */
    struct xaku_compute_pipeline_assembly   assembly;
};

struct xaku_depth_test_params {
    enum xaku_format                        depth_attachment_format;
    bool                                    enable_depth_write;
    enum xaku_compare_op                    depth_test_compare_op;
    f32                                     min_depth_bounds;
    f32                                     max_depth_bounds;
};
#define XAKU_DEFAULT_DEPTH_TEST_PARAMS { \
    .depth_attachment_format = xaku_format_undefined, \
    .enable_depth_write = 0, \
    .depth_test_compare_op = xaku_compare_op_less_equal, \
    .min_depth_bounds = 0.0f, \
    .max_depth_bounds = 1.0f, \
}

struct xaku_stencil_test_params {
    u8                                      test_mask;
    u8                                      write_mask;
    enum xaku_compare_op                    stencil_test_compare_op; 
    enum xaku_stencil_op                    stencil_op_fail; /* keep */
    enum xaku_stencil_op                    stencil_op_depth_fail; /* keep */
    enum xaku_stencil_op                    stencil_op_depth_pass; /* keep */
};
#define XAKU_DEFAULT_STENCIL_TEST_PARAMS { \
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

struct xaku_rasterizer_params {
    enum xaku_primitive_topology                primitive_topology;
    enum xaku_polygon_mode                      polygon_mode;
    enum xaku_cull_mode                         face_culling;
    enum xaku_winding_order                     front_face_winding;
    bool                                        primitive_restart_enable;
    bool                                        depth_clamp_enable;
    bool                                        rasterizer_discard_enable;
    bool                                        depth_bias_enable;
    f32                                         depth_bias_constant_factor;
    f32                                         depth_bias_clamp;
    f32                                         depth_bias_slope_factor;
    f32                                         line_width;
    enum xaku_conservative_rasterization_mode   conservative_rasterization_mode;
    f32                                         conservative_rasterization_size;
    u32                                         static_state_sample_count;
    bool                                        has_conservative_rasterization;
    bool                                        has_static_state_sample_count;
};
#define XAKU_DEFAULT_RASTERIZER_PARAMS { \
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
    enum xaku_blend_factor                  src_color_blend_factor;
    enum xaku_blend_factor                  dst_color_blend_factor;
    enum xaku_blend_op                      color_blend_op;
    enum xaku_blend_factor                  src_alpha_blend_factor;
    enum xaku_blend_factor                  dst_alpha_blend_factor;
    enum xaku_blend_op                      alpha_blend_op;
    u32                                     color_write_mask; /**< enum xaku_color_component_bits */
};
#define XAKU_DEFAULT_BLEND_PARAMS { \
    .src_color_blend_factor = xaku_blend_factor_one, \
    .dst_color_blend_factor = xaku_blend_factor_zero, \
    .color_blend_op = xaku_blend_op_add, \
    .src_alpha_blend_factor = xaku_blend_factor_one, \
    .dst_alpha_blend_factor = xaku_blend_factor_zero, \
    .alpha_blend_op = xaku_blend_op_add, \
    .color_write_mask = xaku_color_component_r | xaku_color_component_g | xaku_color_component_b | xaku_color_component_a, \
}

struct xaku_render_attachment {
    enum xaku_format                        format;
    struct xaku_blend_params               *blend; /* optional */
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
    struct xaku_shader_params                          *mesh_shader;
    struct xaku_shader_params                          *vertex_shader;
    struct xaku_shader_params                          *tesselation_control_shader;
    struct xaku_shader_params                          *tesselation_evaluation_shader;
    struct xaku_shader_params                          *geometry_shader;
    struct xaku_shader_params                          *fragment_shader;
    struct xaku_shader_params                          *task_shader;
    lake_fixed_list(struct xaku_render_attachment, 8)   color_attachments;
    struct xaku_depth_test_params                      *depth_test;
    struct xaku_stencil_test_params                    *stencil_test;
    struct xaku_tesselation_params                     *tesselation;
    struct xaku_rasterizer_params                       raster;
    u32                                                 push_constant_size;
    lake_small_string                                   name;
};
#define XAKU_DEFAULT_RASTER_PIPELINE_ASSEMBLY { \
    .mesh_shader = NULL, \
    .vertex_shader = NULL, \
    .tesselation_control_shader = NULL, \
    .tesselation_evaluation_shader = NULL, \
    .geometry_shader = NULL, \
    .fragment_shader = NULL, \
    .task_shader = NULL, \
    .color_attachments = LAKE_ZERO_INIT, \
    .depth_test = NULL, \
    .stencil_test = NULL, \
    .tesselation = NULL, \
    .raster = LAKE_ZERO_INIT, \
    .push_constant_size = XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE, \
    .name = LAKE_ZERO_INIT, \
}

/** The first member of a 'xaku_raster_pipeline' struct implementation. */
struct xaku_raster_pipeline_header {
    /** Device that owns this memory block. */
    struct xaku_device                     *device; 
    /** The memory may be released when the reference count reaches zero. */
    atomic_u64                              refcnt;
    /** The assembly details used to create this pipeline. */
    struct xaku_raster_pipeline_assembly    assembly;
};

enum xaku_ray_tracing_shader_group_type {
    xaku_ray_tracing_shader_group_type_general = 0,
    xaku_ray_tracing_shader_group_type_triangles_hit_group,
    xaku_ray_tracing_shader_group_type_procedural_hit_group,
};

struct xaku_ray_tracing_shader_group_params {
    enum xaku_ray_tracing_shader_group_type type;
    u32                                     general_shader_index;
    u32                                     closest_hit_shader_index;
    u32                                     any_hit_shader_index;
    u32                                     intersection_shader_index;
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
#define XAKU_DEFAULT_RAY_TRACING_PIPELINE_ASSEMBLY { \
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

/** The first member of a 'xaku_ray_tracing_pipeline' struct implementation. */
struct xaku_ray_tracing_pipeline_header {
    /** Device that owns this memory block. */
    struct xaku_device                         *device; 
    /** The memory may be released when the reference count reaches zero. */
    atomic_u64                                  refcnt;
    /** The assembly details used to create this pipeline. */
    struct xaku_ray_tracing_pipeline_assembly   assembly;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
