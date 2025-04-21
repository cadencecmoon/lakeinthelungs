#pragma once

#include <amwe/xaku/encore.h>
#include <amwe/xaku/gpu_resources.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define XAKU_SHADER_UNUSED (~0u)

enum xaku_shader_create_flag_bits {
    xaku_shader_create_flag_none                        = 0,
    xaku_shader_create_flag_allow_varying_subgroup_size = (1u << 0),
    xaku_shader_create_flag_require_full_subgroups      = (1u << 1),
};

/** For now only SLang is supported: https://shader-slang.org/ */
enum xaku_shader_language {
    xaku_shader_language_slang,
    xaku_shader_language_max_enum = 0x7fffffff,
};

struct xaku_shader_file {
    /** TODO filesystem */
    const char *path;
};

/** This string will only work if it is valid SLang code. */
struct xaku_shader_code {
    const char *code;
};

union xaku_shader_source {
    struct xaku_shader_file file;
    struct xaku_shader_code code;
};

struct xaku_shader_define {
    const char *name;
    const char *value;
};

struct xaku_shader_model {
    u32 major, minor;
};

struct xaku_shader_params {
    const u32                                      *byte_code;
    u32                                             byte_code_size;
    u32                                             create_flags; /**< enum xaku_shader_create_flag_bits */
    u32                                             required_subgroup_size;
    lake_small_string                               entry_point;
};

struct xaku_shader_compile_params {
    union xaku_shader_source                        source;
    const char                                     *entry_point;
    enum xaku_shader_language                       language;
    u32                                             create_flags; /**< enum xaku_shader_create_flag_bits */
    lake_dynamic_array(struct xaku_shader_define)   defines;
    u32                                             required_subgroup_size;
    bool                                            has_required_subgroup_size;
    bool                                            has_entry_point;
    bool                                            enable_debug_utils;
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

struct xaku_compute_pipeline_compile_assembly {
    union xaku_shader_source                        source;
    const char                                     *entry_point;
    enum xaku_shader_language                       language;
    u32                                             create_flags; /**< enum xaku_shader_create_flag_bits */
    lake_dynamic_array(struct xaku_shader_define)   defines;
    u32                                             push_constant_size;
    u32                                             required_subgroup_size;
    bool                                            has_required_subgroup_size;
    bool                                            has_entry_point;
    bool                                            enable_debug_utils;
};
#define XAKU_DEFAULT_COMPUTE_PIPELINE_COMPILE_ASSEMBLY { \
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

struct xaku_raster_pipeline_compile_assembly {
    struct xaku_shader_compile_params                  *mesh_shader;
    struct xaku_shader_compile_params                  *vertex_shader;
    struct xaku_shader_compile_params                  *tesselation_control_shader;
    struct xaku_shader_compile_params                  *tesselation_evaluation_shader;
    struct xaku_shader_compile_params                  *geometry_shader;
    struct xaku_shader_compile_params                  *fragment_shader;
    struct xaku_shader_compile_params                  *task_shader;
    lake_fixed_list(struct xaku_render_attachment, 8)   color_attachments;
    struct xaku_depth_test_params                      *depth_test;
    struct xaku_stencil_test_params                    *stencil_test;
    struct xaku_tesselation_params                     *tesselation;
    struct xaku_rasterizer_params                       raster;
    u32                                                 push_constant_size;
    lake_small_string                                   name;
};
#define XAKU_DEFAULT_RASTER_PIPELINE_COMPILE_ASSEMBLY { \
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
#define XAKU_DEFAULT_RAY_TRACING_PIPELINE_COMPILE_ASSEMBLY { \
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

/** Custom preprocessor for a pipeline manager. TODO filesystem path */
typedef void (LAKECALL *PFN_xaku_shader_custom_preprocessor)(const char *code, const char *path);

/** Not used within encores! */
struct xaku_pipeline_manager_assembly {
    lake_dynamic_array(const char *)                root_paths;                     /* TODO filesystem path */ 
    const char                                     *write_out_preprocessed_code;    /* TODO filesystem path */  
    const char                                     *write_out_spirv;                /* TODO filesystem path */  
    const char                                     *spirv_cache_folder;             /* TODO filesystem path */  
    PFN_xaku_shader_custom_preprocessor             custom_preprocessor;
    enum xaku_shader_language                       default_language;
    u32                                             default_create_flags; /**< enum xaku_shader_create_flag_bits */
    lake_dynamic_array(struct xaku_shader_define)   default_defines;
    u32                                             default_required_subgroup_size;
    bool                                            default_enable_debug_utils;
    bool                                            has_default_required_subgroup_size;
    bool                                            has_write_out_preprocessed_code;   
    bool                                            has_write_out_spirv;               
    bool                                            has_spirv_cache_folder;            
    bool                                            register_null_pipelines_when_first_compile_fails;
    lake_small_string                               name;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
