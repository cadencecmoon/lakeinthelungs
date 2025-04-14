#pragma once

#include <amwe/xaku/encore.h>
#include <amwe/xaku/gpu_resources.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct xaku_buffer_copy_params {
    /* TODO */
};

struct xaku_buffer_texture_copy_params {
    /* TODO */
};

struct xaku_texture_buffer_copy_params {
    /* TODO */
};

struct xaku_texture_copy_params {
    /* TODO */
};

struct xaku_texture_blit_params {
    /* TODO */
};

struct xaku_build_acceleration_structures_params {
    /* TODO */
};

struct xaku_buffer_clear_params {
    /* TODO */
};

struct xaku_texture_clear_params {
    /* TODO */
};

struct xaku_push_constant_params {
    /* TODO */
};

struct xaku_barriers_params {
    /* TODO */
};

struct xaku_transitions_params {
    /* TODO */
};

struct xaku_depth_bias_params {
    /* TODO */
};

struct xaku_index_buffer_params {
    /* TODO */
};

struct xaku_begin_rendering_params {
    /* TODO */
};

struct xaku_trace_rays_params {
    /* TODO */
};

struct xaku_trace_rays_indirect_params {
    /* TODO */
};

struct xaku_dispatch_params {
    /* TODO */
};

struct xaku_dispatch_indirect_params {
    /* TODO */
};

struct xaku_draw_params {
    /* TODO */
};

struct xaku_draw_indexed_params {
    /* TODO */
};

struct xaku_draw_indirect_params {
    /* TODO */
};

struct xaku_draw_indirect_count_params {
    /* TODO */
};

struct xaku_draw_mesh_tasks_params {
    /* TODO */
};

struct xaku_draw_mesh_tasks_indirect_params {
    /* TODO */
};

struct xaku_draw_mesh_tasks_indirect_count_params {
    /* TODO */
};

struct xaku_write_timestamp_params {
    /* TODO */
};

struct xaku_reset_timestamp_params {
    /* TODO */
};

struct xaku_command_label_params {
    /* TODO */
};

/** TODO copy buffer to buffer */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_buffer)(struct xaku_commands_recorder *cmd, const struct xaku_buffer_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_buffer(struct xaku_commands_recorder *cmd, const struct xaku_buffer_copy_params *params)

/** TODO copy buffer to texture */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_buffer_to_texture)(struct xaku_commands_recorder *cmd, const struct xaku_buffer_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_BUFFER_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_buffer_to_texture(struct xaku_commands_recorder *cmd, const struct xaku_buffer_texture_copy_params *params)

/** TODO copy texture to buffer */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_buffer)(struct xaku_commands_recorder *cmd, const struct xaku_texture_buffer_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_buffer(struct xaku_commands_recorder *cmd, const struct xaku_texture_buffer_copy_params *params)

/** TODO copy texture to texture */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_copy_texture_to_texture)(struct xaku_commands_recorder *cmd, const struct xaku_texture_copy_params *params);
#define FN_XAKU_CMD_COPY_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_copy_texture_to_texture(struct xaku_commands_recorder *cmd, const struct xaku_texture_copy_params *params)

/** TODO blit texture to texture */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_blit_texture_to_texture)(struct xaku_commands_recorder *cmd, const struct xaku_texture_blit_params *params);
#define FN_XAKU_CMD_BLIT_TEXTURE_TO_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_blit_texture_to_texture(struct xaku_commands_recorder *cmd, const struct xaku_texture_blit_params *params)

/** TODO build acceleration structures */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_build_acceleration_structures)(struct xaku_commands_recorder *cmd, const struct xaku_build_acceleration_structures_params *params);
#define FN_XAKU_CMD_BUILD_ACCELERATION_STRUCTURES(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_build_acceleration_structures(struct xaku_commands_recorder *cmd, const struct xaku_build_acceleration_structures_params *params)

/** TODO clear buffer */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_clear_buffer)(struct xaku_commands_recorder *cmd, const struct xaku_buffer_clear_params *params);
#define FN_XAKU_CMD_CLEAR_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_clear_buffer(struct xaku_commands_recorder *cmd, const struct xaku_buffer_clear_params *params)

/** TODO clear texture */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_clear_texture)(struct xaku_commands_recorder *cmd, const struct xaku_texture_clear_params *params);
#define FN_XAKU_CMD_CLEAR_TEXTURE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_clear_texture(struct xaku_commands_recorder *cmd, const struct xaku_texture_clear_params *params)

/** Destroys a GPU resource AFTER the GPU is finished executing the command list.
 *  Useful for large uploads exceeding staging memory pools. Can be used for buffers,
 *  textures, texture views, samplers, acceleration structures. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_destroy_deferred)(struct xaku_commands_recorder *cmd, xaku_render_handle resource);
#define FN_XAKU_CMD_DESTROY_DEFERRED(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_destroy_deferred(struct xaku_commands_recorder *cmd, xaku_render_handle resource)

/** TODO push constant */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_push_constant)(struct xaku_commands_recorder *cmd, const struct xaku_push_constant_params *params);
#define FN_XAKU_CMD_PUSH_CONSTANT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_push_constant(struct xaku_commands_recorder *cmd, const struct xaku_push_constant_params *params)

/** TODO barriers */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_barriers)(struct xaku_commands_recorder *cmd, const struct xaku_barriers_params *params);
#define FN_XAKU_CMD_BARRIERS(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_barriers(struct xaku_commands_recorder *cmd, const struct xaku_barriers_params *params)

/** TODO transitions */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_transitions)(struct xaku_commands_recorder *cmd, const struct xaku_transitions_params *params);
#define FN_XAKU_CMD_TRANSITIONS(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_transitions(struct xaku_commands_recorder *cmd, const struct xaku_transitions_params *params)

/** TODO set compute pipeline */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_compute_pipeline)(struct xaku_commands_recorder *cmd, struct xaku_compute_pipeline *pipeline);
#define FN_XAKU_CMD_SET_COMPUTE_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_compute_pipeline(struct xaku_commands_recorder *cmd, struct xaku_compute_pipeline *pipeline)

/** TODO set raster pipeline */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_raster_pipeline)(struct xaku_commands_recorder *cmd, struct xaku_raster_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RASTER_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_raster_pipeline(struct xaku_commands_recorder *cmd, struct xaku_raster_pipeline *pipeline)

/** TODO set compute pipeline */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_ray_tracing_pipeline)(struct xaku_commands_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline);
#define FN_XAKU_CMD_SET_RAY_TRACING_PIPELINE(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_ray_tracing_pipeline(struct xaku_commands_recorder *cmd, struct xaku_ray_tracing_pipeline *pipeline)

/** TODO set viewport. */
typedef void (LAKECALL *PFN_xaku_cmd_set_viewport)(struct xaku_commands_recorder *cmd, const struct xaku_viewport *viewport);
#define FN_XAKU_CMD_SET_VIEWPORT(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_viewport(struct xaku_commands_recorder *cmd, const struct xaku_viewport *viewport)

/** TODO set scissor. */
typedef void (LAKECALL *PFN_xaku_cmd_set_scissor)(struct xaku_commands_recorder *cmd, const struct xaku_rect2d *scissor);
#define FN_XAKU_CMD_SET_SCISSOR(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_scissor(struct xaku_commands_recorder *cmd, const struct xaku_rect2d *scissor)

/** TODO set depth bias. */
typedef void (LAKECALL *PFN_xaku_cmd_set_depth_bias)(struct xaku_commands_recorder *cmd, const struct xaku_depth_bias_params *params);
#define FN_XAKU_CMD_SET_DEPTH_BIAS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_set_depth_bias(struct xaku_commands_recorder *cmd, const struct xaku_depth_bias_params *params)

/** TODO set index buffer. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_set_index_buffer)(struct xaku_commands_recorder *cmd, const struct xaku_index_buffer_params *params);
#define FN_XAKU_CMD_SET_INDEX_BUFFER(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_set_index_buffer(struct xaku_commands_recorder *cmd, const struct xaku_index_buffer_params *params)

/** TODO begin rendering. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_begin_rendering)(struct xaku_commands_recorder *cmd, const struct xaku_begin_rendering_params *params);
#define FN_XAKU_CMD_BEGIN_RENDERING(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_begin_rendering(struct xaku_commands_recorder *cmd, const struct xaku_begin_rendering_params *params)

/** TODO end rendering. */
typedef void (LAKECALL *PFN_xaku_cmd_end_rendering)(struct xaku_commands_recorder *cmd);
#define FN_XAKU_CMD_END_RENDERING(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_end_rendering(struct xaku_commands_recorder *cmd)

/** TODO dispatch */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_dispatch)(struct xaku_commands_recorder *cmd, const struct xaku_dispatch_params *params);
#define FN_XAKU_CMD_DISPATCH(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_dispatch(struct xaku_commands_recorder *cmd, const struct xaku_dispatch_params *params)

/** TODO dispatch indirect */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_dispatch_indirect)(struct xaku_commands_recorder *cmd, const struct xaku_dispatch_indirect_params *params);
#define FN_XAKU_CMD_DISPATCH_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_dispatch_indirect(struct xaku_commands_recorder *cmd, const struct xaku_dispatch_indirect_params *params)

/** TODO draw. */
typedef void (LAKECALL *PFN_xaku_cmd_draw)(struct xaku_commands_recorder *cmd, const struct xaku_draw_params *params);
#define FN_XAKU_CMD_DRAW(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw(struct xaku_commands_recorder *cmd, const struct xaku_draw_params *params)

/** TODO draw indexed. */
typedef void (LAKECALL *PFN_xaku_cmd_draw_indexed)(struct xaku_commands_recorder *cmd, const struct xaku_draw_indexed_params *params);
#define FN_XAKU_CMD_DRAW_INDEXED(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw_indexed(struct xaku_commands_recorder *cmd, const struct xaku_draw_indexed_params *params)

/** TODO draw indirect. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_indirect)(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_params *params)

/** TODO draw indirect count. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_indirect_count)(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_count_params *params);
#define FN_XAKU_CMD_DRAW_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_indirect_count(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_count_params *params)

/** TODO draw mesh tasks. */
typedef void (LAKECALL *PFN_xaku_cmd_draw_mesh_tasks)(struct xaku_commands_recorder *cmd, const struct xaku_draw_mesh_tasks_params *params);
#define FN_XAKU_CMD_DRAW_MESH_TASKS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_draw_mesh_tasks(struct xaku_commands_recorder *cmd, const struct xaku_draw_mesh_tasks_params *params)

/** TODO draw mesh tasks indirect. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_mesh_tasks_indirect)(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_params *params);
#define FN_XAKU_CMD_DRAW_MESH_TASKS_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_mesh_tasks_indirect(struct xaku_commands_recorder *cmd, const struct xaku_draw_indirect_params *params)

/** TODO draw mesh tasks indirect count. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_draw_mesh_tasks_indirect_count)(struct xaku_commands_recorder *cmd, const struct xaku_draw_mesh_tasks_indirect_count_params *params);
#define FN_XAKU_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_draw_mesh_tasks_indirect_count(struct xaku_commands_recorder *cmd, const struct xaku_draw_mesh_tasks_indirect_count_params *params)

/** TODO trace rays. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_trace_rays)(struct xaku_commands_recorder *cmd, const struct xaku_trace_rays_params *params);
#define FN_XAKU_CMD_TRACE_RAYS(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays(struct xaku_commands_recorder *cmd, const struct xaku_trace_rays_params *params)

/** TODO trace rays indirect. */
typedef lake_nodiscard enum xaku_result (LAKECALL *PFN_xaku_cmd_trace_rays_indirect)(struct xaku_commands_recorder *cmd, const struct xaku_trace_rays_indirect_params *params);
#define FN_XAKU_CMD_TRACE_RAYS_INDIRECT(ENCORE) \
    lake_nodiscard enum xaku_result LAKECALL _xaku_##ENCORE##_cmd_trace_rays_indirect(struct xaku_commands_recorder *cmd, const struct xaku_trace_rays_indirect_params *params)

/** TODO write timestamps */
typedef void (LAKECALL *PFN_xaku_cmd_write_timestamps)(struct xaku_commands_recorder *cmd, const struct xaku_write_timestamp_params *params);
#define FN_XAKU_CMD_WRITE_TIMESTAMPS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_write_timestamps(struct xaku_commands_recorder *cmd, const struct xaku_write_timestamp_params *params)

/** TODO reset timestamps */
typedef void (LAKECALL *PFN_xaku_cmd_reset_timestamps)(struct xaku_commands_recorder *cmd, const struct xaku_reset_timestamp_params *params);
#define FN_XAKU_CMD_RESET_TIMESTAMPS(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_reset_timestamps(struct xaku_commands_recorder *cmd, const struct xaku_reset_timestamp_params *params)

/** TODO begin label */
typedef void (LAKECALL *PFN_xaku_cmd_begin_label)(struct xaku_commands_recorder *cmd, const struct xaku_command_label_params *params);
#define FN_XAKU_CMD_BEGIN_LABEL(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_begin_label(struct xaku_commands_recorder *cmd, const struct xaku_command_label_params *params)

/** TODO end label */
typedef void (LAKECALL *PFN_xaku_cmd_end_label)(struct xaku_commands_recorder *cmd);
#define FN_XAKU_CMD_END_LABEL(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_cmd_end_label(struct xaku_commands_recorder *cmd)

struct xaku_interface_cmd {
    PFN_xaku_cmd_copy_buffer_to_buffer          copy_buffer_to_buffer;
    PFN_xaku_cmd_copy_buffer_to_texture         copy_buffer_to_texture;
    PFN_xaku_cmd_copy_texture_to_buffer         copy_texture_to_buffer;
    PFN_xaku_cmd_copy_texture_to_texture        copy_texture_to_texture;
    PFN_xaku_cmd_blit_texture_to_texture        blit_texture_to_texture;
    PFN_xaku_cmd_build_acceleration_structures  build_acceleration_structures;
    PFN_xaku_cmd_clear_buffer                   clear_buffer;
    PFN_xaku_cmd_clear_texture                  clear_texture;
    PFN_xaku_cmd_destroy_deferred               destroy_deferred;
    PFN_xaku_cmd_push_constant                  push_constant;
    PFN_xaku_cmd_barriers                       barriers;
    PFN_xaku_cmd_transitions                    transitions;
    PFN_xaku_cmd_set_compute_pipeline           set_compute_pipeline;
    PFN_xaku_cmd_set_raster_pipeline            set_raster_pipeline;
    PFN_xaku_cmd_set_ray_tracing_pipeline       set_ray_tracing_pipeline;
    PFN_xaku_cmd_set_viewport                   set_viewport;
    PFN_xaku_cmd_set_scissor                    set_scissor;
    PFN_xaku_cmd_set_depth_bias                 set_depth_bias;
    PFN_xaku_cmd_set_index_buffer               set_index_buffer;
    PFN_xaku_cmd_begin_rendering                begin_rendering;
    PFN_xaku_cmd_end_rendering                  end_rendering;
    PFN_xaku_cmd_dispatch                       dispatch;
    PFN_xaku_cmd_dispatch_indirect              dispatch_indirect;
    PFN_xaku_cmd_draw                           draw;
    PFN_xaku_cmd_draw_indexed                   draw_indexed;
    PFN_xaku_cmd_draw_indirect                  draw_indirect;
    PFN_xaku_cmd_draw_indirect_count            draw_indirect_count;
    PFN_xaku_cmd_draw_mesh_tasks                draw_mesh_tasks;
    PFN_xaku_cmd_draw_mesh_tasks_indirect       draw_mesh_tasks_indirect;
    PFN_xaku_cmd_draw_mesh_tasks_indirect_count draw_mesh_tasks_indirect_count;
    PFN_xaku_cmd_trace_rays                     trace_rays;
    PFN_xaku_cmd_trace_rays_indirect            trace_rays_indirect;
    PFN_xaku_cmd_write_timestamps               write_timestamps;
    PFN_xaku_cmd_reset_timestamps               reset_timestamps;
    PFN_xaku_cmd_begin_label                    begin_label;
    PFN_xaku_cmd_end_label                      end_label;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
