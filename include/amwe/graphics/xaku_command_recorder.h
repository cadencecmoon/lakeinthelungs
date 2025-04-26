#pragma once

#include <amwe/graphics/xaku_encore.h>
#include <amwe/graphics/xaku_gpu_resources.h>
#include <amwe/graphics/xaku_pipelines.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum xaku_queue_type {
    xaku_queue_type_main = 0u,
    xaku_queue_type_compute,
    xaku_queue_type_transfer,
    xaku_queue_type_sparse_binding,
    xaku_queue_type_video_decode,
    xaku_queue_type_video_encode,
    xaku_queue_type_max_enum,
};

struct xaku_queue {
    enum xaku_queue_type    type;
    u32                     index;
};

struct xaku_command_recorder_assembly {
    enum xaku_queue_type            queue_family;
    lake_small_string               name;
};
#define XAKU_DEFAULT_COMMAND_RECORDER_ASSEMBLY LAKE_ZERO_INIT

/** PFN_xaku_cmd_copy_buffer_to_buffer */
struct xaku_buffer_copy_params {
    xaku_buffer_id                  src_buffer;
    xaku_buffer_id                  dst_buffer;
    u64                             src_offset;
    u64                             dst_offset;
    u64                             size;
};
#define XAKU_DEFAULT_BUFFER_COPY_PARAMS LAKE_ZERO_INIT

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
#define XAKU_DEFAULT_BUFFER_TEXTURE_COPY_PARAMS { \
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
#define XAKU_DEFAULT_TEXTURE_COPY_PARAMS { \
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
#define XAKU_DEFAULT_TEXTURE_BLIT_PARAMS { \
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
#define XAKU_DEFAULT_BUFFER_CLEAR_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_clear_texture */
struct xaku_texture_clear_params {
    xaku_texture_id                 texture;
    enum xaku_texture_layout        texture_layout;
    struct xaku_texture_array_slice texture_slice;
    union xaku_clear_value          clear_value;
};
#define XAKU_DEFAULT_TEXTURE_CLEAR_PARAMS { \
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
#define XAKU_DEFAULT_BUILD_ACCELERATION_STRUCTURES_PARAMS

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
#define XAKU_DEFAULT_DEPTH_BIAS_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_set_index_buffer */
struct xaku_index_buffer_params {
    xaku_buffer_id          buffer;
    u64                     offset;
    enum xaku_index_format  index_format;
};
#define XAKU_DEFAULT_INDEX_BUFFER_PARAMS { \
    .buffer = LAKE_ZERO_INIT, \
    .offset = 0, \
    .index_format = xaku_index_format_u32, \
}

struct xaku_render_attachment_resolve_params {
    xaku_texture_view_id        texture_view;
    enum xaku_texture_layout    layout;
    u32                         mode; /**< enum xaku_resolve_mode_bits */
};
#define XAKU_DEFAULT_RENDER_ATTACHMENT_RESOLVE_PARAMS { \
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
#define XAKU_DEFAULT_RENDER_ATTACHMENT_PARAMS { \
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
#define XAKU_DEFAULT_BEGIN_RENDERING_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_dispatch */
struct xaku_dispatch_params {
    u32 group_x, group_y, group_z;
};
#define XAKU_DEFAULT_DISPATCH_PARAMS {1u, 1u, 1u}

/** PFN_xaku_cmd_dispatch_indirect */
struct xaku_dispatch_indirect_params {
    xaku_buffer_id indirect_buffer;
    u64            indirect_buffer_offset;
};
#define XAKU_DEFAULT_DISPATCH_INDIRECT_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_draw */
struct xaku_draw_params {
    u32 vertex_count;
    u32 instance_count;
    u32 first_vertex;
    u32 first_instance;
};
#define XAKU_DEFAULT_DRAW_PARAMS { \
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
#define XAKU_DEFAULT_DRAW_INDEXED_PARAMS { \
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
#define XAKU_DEFAULT_DRAW_INDIRECT_PARAMS { \
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
#define XAKU_DEFAULT_DRAW_INDIRECT_COUNT_PARAMS { \
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
#define XAKU_DEFAULT_MESH_TASKS_INDIRECT_PARAMS { \
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
#define XAKU_DEFAULT_MESH_TASKS_INDIRECT_COUNT_PARAMS { \
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
#define XAKU_DEFAULT_TRACE_RAYS_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_trace_rays_indirect */
struct xaku_trace_rays_indirect_params {
    u64                                 indirect_device_address;
    u32                                 raygen_handle_offset;
    u32                                 miss_handle_offset;
    u32                                 hit_handle_offset;
    u32                                 callable_handle_offset;
    struct xaku_shader_binding_table    shader_binding_table;
};
#define XAKU_DEFAULT_TRACE_RAYS_INDIRECT_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_write_timestamps */
struct xaku_write_timestamp_params {
    struct xaku_query_pool             *query_pool;
    u32                                 shader_stage; /**< enum xaku_shader_stage_bits */
    u32                                 query_index;
};
#define XAKU_DEFAULT_WRITE_TIMESTAMP_PARAMS LAKE_ZERO_INIT

/** PFN_xaku_cmd_reset_timestamps */
struct xaku_reset_timestamp_params {
    struct xaku_query_pool             *query_pool;
    u32                                 start_index;
    u32                                 count;
#define XAKU_DEFAULT_RESET_TIMESTAMP_PARAMS LAKE_ZERO_INIT
};

/** PFN_xaku_cmd_begin_label */
struct xaku_command_label_params {
    vec4                                label_color;
    lake_small_string                   name;
};
#define XAKU_DEFAULT_COMMAND_LABEL_PARAMS LAKE_ZERO_INIT

/** Copy data between buffer regions. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_copy_params *params)

/** Copy data from a buffer into an texture. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_texture(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params)

/** Copy texture data into a buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_texture_copy_params *params)

/** Copy data between texture regions. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_copy_params *params)

/** Copy regions of a source image into a destination image, potentially 
 *  performing format conversion, arbitrary scaling, and filtering. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_blit_texture_to_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_blit_params *params);
#define FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_blit_texture_to_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_blit_params *params)

/** Fill a region of a buffer with a fixed value, used to zero out the buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_clear_buffer)(struct xaku_command_recorder *cmd, const struct xaku_buffer_clear_params *params);
#define FN_XAKU_CMD_CLEAR_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_clear_buffer(struct xaku_command_recorder *cmd, const struct xaku_buffer_clear_params *params)

/** Fill a region of a texture with a fixed value, used to zero out the texture. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_clear_texture)(struct xaku_command_recorder *cmd, const struct xaku_texture_clear_params *params);
#define FN_XAKU_CMD_CLEAR_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_clear_texture(struct xaku_command_recorder *cmd, const struct xaku_texture_clear_params *params)

/** Builds and updates acceleration structures. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_build_acceleration_structures)(struct xaku_command_recorder *cmd, const struct xaku_build_acceleration_structures_params *params);
#define FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_build_acceleration_structures(struct xaku_command_recorder *cmd, const struct xaku_build_acceleration_structures_params *params)

/** Destroys a GPU resource AFTER the GPU is finished executing the command list.
 *  Useful for large uploads exceeding staging memory pools. Can be used for buffers,
 *  textures, texture views, samplers, acceleration structures. */
#define PFN_XAKU_CMD_DESTROY_DEFERRED(T) \
    typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_destroy_##T##_deferred)(struct xaku_command_recorder *cmd, xaku_##T##_id T);
#define FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE, T) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_destroy_##T##_deferred(struct xaku_command_recorder *cmd, xaku_##T##_id T)

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
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_push_constant)(struct xaku_command_recorder *cmd, const struct xaku_push_constant_params *params);
#define FN_XAKU_CMD_PUSH_CONSTANT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_push_constant(struct xaku_command_recorder *cmd, const struct xaku_push_constant_params *params)

/** Bind compute pipeline for dispatch commands. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_compute_pipeline)(struct xaku_command_recorder *cmd, struct xaku_compute_pipeline *pipeline);
#define FN_XAKU_CMD_SET_COMPUTE_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_compute_pipeline(struct xaku_command_recorder *cmd, struct xaku_compute_pipeline *pipeline)

/** Bind raster pipeline for draw commands. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_raster_pipeline)(struct xaku_command_recorder *cmd, struct xaku_raster_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RASTER_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_raster_pipeline(struct xaku_command_recorder *cmd, struct xaku_raster_pipeline *pipeline)

/** Bind ray tracing pipeline for trace commands. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_ray_tracing_pipeline)(struct xaku_command_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_ray_tracing_pipeline(struct xaku_command_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline)

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
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_index_buffer)(struct xaku_command_recorder *cmd, const struct xaku_index_buffer_params *params);
#define FN_XAKU_CMD_SET_INDEX_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_index_buffer(struct xaku_command_recorder *cmd, const struct xaku_index_buffer_params *params)

/** Start a renderpass with specified render targets. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_begin_rendering)(struct xaku_command_recorder *cmd, const struct xaku_begin_rendering_params *params);
#define FN_XAKU_CMD_BEGIN_RENDERING(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_begin_rendering(struct xaku_command_recorder *cmd, const struct xaku_begin_rendering_params *params)

/** Ends the current renderpass. */
typedef void (LAKECALL *PFN_xaku_cmd_end_rendering)(struct xaku_command_recorder *cmd);
#define FN_XAKU_CMD_END_RENDERING(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_end_rendering(struct xaku_command_recorder *cmd)

/** Execute compute workgroups with direct parameters. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_dispatch)(struct xaku_command_recorder *cmd, const struct xaku_dispatch_params *params);
#define FN_XAKU_CMD_DISPATCH(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_dispatch(struct xaku_command_recorder *cmd, const struct xaku_dispatch_params *params)

/** Execute compute workgroups with indirect parameters buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_dispatch_indirect)(struct xaku_command_recorder *cmd, const struct xaku_dispatch_indirect_params *params);
#define FN_XAKU_CMD_DISPATCH_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_dispatch_indirect(struct xaku_command_recorder *cmd, const struct xaku_dispatch_indirect_params *params)

/** Draw non-indexed primitives with vertex count. */
typedef void (LAKECALL *PFN_xaku_cmd_draw)(struct xaku_command_recorder *cmd, const struct xaku_draw_params *params);
#define FN_XAKU_CMD_DRAW(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw(struct xaku_command_recorder *cmd, const struct xaku_draw_params *params)

/** Draw indexed primitives used bound index buffer. */
typedef void (LAKECALL *PFN_xaku_cmd_draw_indexed)(struct xaku_command_recorder *cmd, const struct xaku_draw_indexed_params *params);
#define FN_XAKU_CMD_DRAW_INDEXED(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw_indexed(struct xaku_command_recorder *cmd, const struct xaku_draw_indexed_params *params)

/** Draw primitives using indirect parameters buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_indirect)(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_params *params)

/** Draw primitives using indirect parameters buffer with deferred count argument. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_indirect_count)(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_count_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect_count(struct xaku_command_recorder *cmd, const struct xaku_draw_indirect_count_params *params)

/** Execute mesh shader workgroups directly. */
typedef void (LAKECALL *PFN_xaku_cmd_mesh_tasks)(struct xaku_command_recorder *cmd, u32 x, u32 y, u32 z);
#define FN_XAKU_CMD_MESH_TASKS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks(struct xaku_command_recorder *cmd, u32 x, u32 y, u32 z)

/** Execute mesh shader workgroups using indirect parameters buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_mesh_tasks_indirect)(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_params *params);
#define FN_XAKU_CMD_MESH_TASKS_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks_indirect(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_params *params)

/** Execute mesh shader workgroups using indirect parameters buffer with deferred count argument. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_mesh_tasks_indirect_count)(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_count_params *params);
#define FN_XAKU_CMD_MESH_TASKS_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_mesh_tasks_indirect_count(struct xaku_command_recorder *cmd, const struct xaku_mesh_tasks_indirect_count_params *params)

/** Execute ray tracing workload directly. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_trace_rays)(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_params *params);
#define FN_XAKU_CMD_TRACE_RAYS(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_params *params)

/** Execute ray tracing workload using indirect parameters buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_trace_rays_indirect)(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_indirect_params *params);
#define FN_XAKU_CMD_TRACE_RAYS_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays_indirect(struct xaku_command_recorder *cmd, const struct xaku_trace_rays_indirect_params *params)

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
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_compile_command_list)(struct xaku_command_recorder *cmd, u32 *out_device_index, struct xaku_executable_command_list **out_list);
#define FN_XAKU_COMPILE_COMMAND_LIST(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_compile_command_list(struct xaku_command_recorder *cmd, u32 *out_device_index, struct xaku_executable_command_list **out_list)

#ifdef __cplusplus
}
#endif /* __cplusplus */
