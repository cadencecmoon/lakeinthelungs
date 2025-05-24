#pragma once

/** @file moon/moon_commands.h
 *  @brief The API to record GPU commands.
 *
 *  Following interface procedures are defined:
 *      PFN_moon_command_recorder_assembly
 *      PFN_moon_command_recorder_zero_refcnt
 *      PFN_moon_staged_command_list_assembly
 *      PFN_moon_staged_command_list_zero_refcnt
 *      PFN_moon_cmd_copy_buffer
 *      PFN_moon_cmd_copy_buffer_to_texture
 *      PFN_moon_cmd_copy_texture_to_buffer
 *      PFN_moon_cmd_copy_texture
 *      PFN_moon_cmd_blit_texture
 *      PFN_moon_cmd_resolve_texture
 *      PFN_moon_cmd_clear_buffer
 *      PFN_moon_cmd_clear_texture
 *      PFN_moon_cmd_destroy_buffer_deferred
 *      PFN_moon_cmd_destroy_texture_deferred
 *      PFN_moon_cmd_destroy_texture_view_deferred
 *      PFN_moon_cmd_destroy_sampler_deferred
 *      PFN_moon_cmd_build_acceleration_structures
 *      PFN_moon_cmd_root_constants
 *      PFN_moon_cmd_set_compute_pipeline
 *      PFN_moon_cmd_set_raster_pipeline
 *      PFN_moon_cmd_set_ray_tracing_pipeline
 *      PFN_moon_cmd_set_work_graph_pipeline
 *      PFN_moon_cmd_set_viewport
 *      PFN_moon_cmd_set_scissor
 *      PFN_moon_cmd_set_rasterization_samples
 *      PFN_moon_cmd_set_depth_bias
 *      PFN_moon_cmd_set_index_buffer
 *      PFN_moon_cmd_barriers_and_transitions
 *      PFN_moon_cmd_begin_renderpass
 *      PFN_moon_cmd_end_renderpass
 *      PFN_moon_cmd_write_timestamps
 *      PFN_moon_cmd_resolve_timestamps
 *      PFN_moon_cmd_begin_label
 *      PFN_moon_cmd_end_label
 *      PFN_moon_cmd_trace_rays
 *      PFN_moon_cmd_trace_rays_indirect
 *      PFN_moon_cmd_dispatch
 *      PFN_moon_cmd_dispatch_indirect
 *      PFN_moon_cmd_dispatch_graph
 *      PFN_moon_cmd_dispatch_graph_indirect
 *      PFN_moon_cmd_dispatch_graph_indirect_count
 *      PFN_moon_cmd_draw
 *      PFN_moon_cmd_draw_indexed
 *      PFN_moon_cmd_draw_indirect
 *      PFN_moon_cmd_draw_indirect_count
 *      PFN_moon_cmd_draw_mesh_tasks
 *      PFN_moon_cmd_draw_mesh_tasks_indirect
 *      PFN_moon_cmd_draw_mesh_tasks_indirect_count
 */
#include "moon_device.h"

typedef struct {
    moon_queue_type                     queue_type;
    lake_small_string                   name;
} moon_command_recorder_assembly;

typedef struct {
    bool                                one_time_submit;
    lake_small_string                   name;
} moon_staged_command_list_assembly;

typedef struct {
    u64                                 src_offset;
    u64                                 dst_offset;
    u64                                 size;
} moon_buffer_region;

typedef struct {
    moon_buffer_id                      src_buffer;
    moon_buffer_id                      dst_buffer;
    u32                                 region_count;
    moon_buffer_region const           *regions;
} moon_cmd_copy_buffer;
static constexpr moon_cmd_copy_buffer MOON_CMD_COPY_BUFFER_INIT = {
    .region_count = 1,
};

typedef struct {
    u64                                 buffer_offset;
    u32                                 buffer_row_length;
    u32                                 buffer_texture_height;
    moon_texture_array_slice            texture_slice;
    lake_offset3d                       texture_offset;
    lake_extent3d                       texture_extent;
} moon_buffer_and_texture_copy;

typedef struct {
    moon_buffer_id                      buffer;
    moon_texture_id                     texture;
    moon_texture_layout                 texture_layout;
    u32                                 region_count;
    moon_buffer_and_texture_copy const *regions;
} moon_cmd_copy_buffer_and_texture;
static constexpr moon_cmd_copy_buffer_and_texture MOON_CMD_COPY_BUFFER_TO_TEXTURE_INIT = {
    .texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};
static constexpr moon_cmd_copy_buffer_and_texture MOON_CMD_COPY_TEXTURE_TO_BUFFER_INIT = {
    .texture_layout = moon_texture_layout_transfer_src_optimal,
    .region_count = 1,
};

typedef struct {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_copy;

typedef struct {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_copy const            *regions;
} moon_cmd_copy_texture;
static constexpr moon_cmd_copy_texture MOON_CMD_COPY_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

typedef struct {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offsets[2];
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offsets[2];
} moon_texture_blit;

typedef struct {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    moon_filter_mode                    filter;
    u32                                 region_count;
    moon_texture_blit const            *regions;
} moon_cmd_blit_texture;
static constexpr moon_cmd_blit_texture MOON_CMD_BLIT_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .filter = moon_filter_mode_linear,
    .region_count = 1,
};

typedef struct {
    moon_texture_array_slice            src_slice;
    lake_offset3d                       src_offset;
    moon_texture_array_slice            dst_slice;
    lake_offset3d                       dst_offset;
    lake_extent3d                       extent;
} moon_texture_resolve;

typedef struct {
    moon_texture_id                     src_texture;
    moon_texture_id                     dst_texture;
    moon_texture_layout                 src_texture_layout;
    moon_texture_layout                 dst_texture_layout;
    u32                                 region_count;
    moon_texture_resolve const         *regions;
} moon_cmd_resolve_texture;
static constexpr moon_cmd_resolve_texture MOON_CMD_RESOLVE_TEXTURE_INIT = {
    .src_texture_layout = moon_texture_layout_transfer_src_optimal,
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
    .region_count = 1,
};

typedef struct {
    moon_buffer_id      dst_buffer;
    u64                 dst_offset;
    u64                 size;
    s32                 clear_value;
} moon_cmd_clear_buffer;
static constexpr moon_cmd_clear_buffer MOON_CMD_CLEAR_BUFFER_INIT = {0};

typedef struct {
    moon_texture_id     dst_texture;
    moon_texture_layout dst_texture_layout;
    moon_color_value    clear_value;
} moon_cmd_clear_texture;
static constexpr moon_cmd_clear_texture MOON_CMD_CLEAR_TEXTURE_INIT = {
    .dst_texture_layout = moon_texture_layout_transfer_dst_optimal,
};

typedef struct {
    u32                 tlas_assembly_count;
    u32                 blas_assembly_count;
    moon_tlas_assembly *tlas_assembly;
    moon_blas_assembly *blas_assembly;
} moon_cmd_build_acceleration_structures;
static constexpr moon_cmd_build_acceleration_structures MOON_CMD_BUILD_ACCELERATION_STRUCTURES_INIT = {0};

typedef struct {
    void const         *data;
    u32                 offset;
    u32                 size;
} moon_cmd_root_constants;
static constexpr moon_cmd_root_constants MOON_CMD_ROOT_CONSTANTS_INIT = {0};

typedef struct {
    moon_buffer_id      buffer;
    u64                 offset;
    moon_index_format   index_format;
} moon_cmd_set_index_buffer;
static constexpr moon_cmd_set_index_buffer MOON_CMD_SET_INDEX_BUFFER_INIT = {
    .index_format = moon_index_format_u32,
};

typedef struct {
    u32                                 barrier_count;
    u32                                 transition_count;
    moon_memory_barrier const          *barriers;
    moon_texture_memory_barrier const  *transitions;
} moon_cmd_barriers_and_transitions;

typedef struct {
    moon_texture_view_id                texture_view;
    moon_texture_view_id                resolve_texture_view;
    moon_texture_layout                 texture_layout;
    moon_texture_layout                 resolve_texture_layout;
    moon_resolve_mode                   resolve_mode;
    moon_load_op                        load_op;
    moon_store_op                       store_op;
    bool                                has_resolve;
    moon_color_value                    clear_value;
} moon_renderpass_attachment;
static constexpr moon_renderpass_attachment MOON_RENDERPASS_ATTACHMENT_INIT = {
    .texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_texture_layout = moon_texture_layout_attachment_optimal,
    .resolve_mode = moon_resolve_mode_average,
    .load_op = moon_load_op_dont_care,
    .store_op = moon_store_op_store,
    .has_resolve = false,
};

typedef struct {
    moon_renderpass_attachment          color_attachments[MOON_MAX_COLOR_ATTACHMENTS];
    u32                                 color_attachment_count;
    bool                                has_depth_attachment;
    bool                                has_stencil_attachment;
    moon_renderpass_attachment          depth_attachment;
    moon_renderpass_attachment          stencil_attachment;
    lake_rect2d                         render_area;

} moon_cmd_begin_renderpass;
static constexpr moon_cmd_begin_renderpass MOON_CMD_BEGIN_RENDERPASS_INIT = {
    .color_attachments = {
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
        MOON_RENDERPASS_ATTACHMENT_INIT,
    },
    .color_attachment_count = 1,
    .has_depth_attachment = false,
    .has_stencil_attachment = false,
    .depth_attachment = MOON_RENDERPASS_ATTACHMENT_INIT,
    .stencil_attachment = MOON_RENDERPASS_ATTACHMENT_INIT,
};

typedef struct {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         query_index;
    moon_pipeline_stage_bits    stage;
} moon_cmd_write_timestamps;
static constexpr moon_cmd_write_timestamps MOON_CMD_WRITE_TIMESTAMPS_INIT = {0};

typedef struct {
    moon_timeline_query_pool    timeline_query_pool;
    u32                         start_index;
    u32                         count;
} moon_cmd_resolve_timestamps;
static constexpr moon_cmd_resolve_timestamps MOON_CMD_RESOLVE_TIMESTAMPS_INIT = {0};

typedef struct {
    vec4                        label_color;
    char const                 *label_name;
} moon_cmd_begin_label;
static constexpr moon_cmd_begin_label MOON_CMD_BEGIN_LABEL_INIT = {0};

typedef struct {
    u32                         width, height, depth;
} moon_ray_trace;

typedef struct {
    u32                         raygen_shader_binding_table_offset;
    u32                         miss_shader_binding_table_offset;
    u32                         miss_shader_binding_table_stride;
    u32                         hit_shader_binding_table_offset;
    moon_shader_binding_table   shader_binding_table;
    u32                         payload_count;
    moon_ray_trace const       *payloads;
} moon_cmd_trace_rays;
static constexpr moon_cmd_trace_rays MOON_CMD_TRACE_RAYS_INIT = {
    .payload_count = 1,
};

typedef struct {
    moon_device_address         indirect_buffer_address;
    moon_shader_binding_table   shader_binding_table;
} moon_cmd_trace_rays_indirect;
static constexpr moon_cmd_trace_rays_indirect MOON_CMD_TRACE_RAYS_INDIRECT_INIT = {0};

/** Compatible with `moon_draw_mesh_tasks`. */
typedef struct {
    u32                         group_x, group_y, group_z;
} moon_dispatch;

typedef struct {
    u32                         payload_count;
    moon_dispatch const        *payloads;
} moon_cmd_dispatch;
static constexpr moon_cmd_dispatch MOON_CMD_DISPATCH_INIT = {
    .payload_count = 1,
};

typedef struct {
    moon_buffer_id              indirect_buffer;
    u64                         offset;
} moon_cmd_dispatch_indirect;
static constexpr moon_cmd_dispatch_indirect MOON_CMD_DISPATCH_INDIRECT_INIT = {0};

typedef struct {
    u32                         node_index;
    u32                         payload_count;
    moon_device_or_host_address payloads; /**< Either `moon_dispatch` or `moon_draw_mesh_tasks`. */
    u64                         payload_stride;
} moon_dispatch_node;

typedef struct {
    u32                         node_count;
    moon_device_or_host_address nodes;
    u64                         node_stride;
} moon_dispatch_graph;

typedef struct {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_dispatch_graph         graph;
} moon_cmd_dispatch_graph;
static constexpr moon_cmd_dispatch_graph MOON_CMD_DISPATCH_GRAPH_INIT = {0};
static constexpr moon_cmd_dispatch_graph MOON_CMD_DISPATCH_GRAPH_INDIRECT_INIT = {0};

typedef struct {
    moon_device_address         scratch;
    u64                         scratch_size;
    moon_device_address         graph;
} moon_cmd_dispatch_graph_count;
static constexpr moon_cmd_dispatch_graph_count MOON_CMD_DISPATCH_GRAPH_COUNT_INIT = {0};

typedef struct {
    u32                         vertex_count;
    u32                         instance_count;
    u32                         first_vertex;
    u32                         first_instance;
} moon_draw;

typedef struct {
    u32                         draw_count;
    moon_draw const            *draw_commands;
} moon_cmd_draw;
static constexpr moon_cmd_draw MOON_CMD_DRAW_INIT = {
    .draw_count = 1,
};

typedef struct {
    u32                         index_count;
    u32                         instance_count;
    u32                         first_index;
    s32                         vertex_offset;
    u32                         first_instance;
} moon_draw_indexed;

typedef struct {
    u32                         draw_count;
    moon_draw const            *draw_commands;
} moon_cmd_draw_indexed;
static constexpr moon_cmd_draw_indexed MOON_CMD_DRAW_INDEXED_INIT = {
    .draw_count = 1,
};

typedef struct {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_cmd_draw_indirect;

typedef struct {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
    bool                        is_indexed;
} moon_cmd_draw_indirect_count;

/** Compatible with `moon_dispatch`. */
typedef struct {
    u32                         group_x, group_y, group_z;
} moon_draw_mesh_tasks;

typedef struct {
    u32                         draw_count;
    moon_draw_mesh_tasks const *draw_commands;
} moon_cmd_draw_mesh_tasks;
static constexpr moon_cmd_draw_mesh_tasks MOON_CMD_DRAW_MESH_TASKS_INIT = {
    .draw_count = 1,
};

typedef struct {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    u32                         draw_count;
    u32                         draw_stride;
} moon_cmd_draw_mesh_tasks_indirect;
static constexpr moon_cmd_draw_mesh_tasks_indirect MOON_CMD_DRAW_MESH_TASKS_INDIRECT_INIT = {0};

typedef struct {
    moon_buffer_id              indirect_buffer;
    u64                         indirect_buffer_offset;
    moon_buffer_id              count_buffer;
    u64                         count_buffer_offset;
    u32                         max_draw_count;
    u32                         draw_stride;
} moon_cmd_draw_mesh_tasks_indirect_count;
static constexpr moon_cmd_draw_mesh_tasks_indirect_count MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_INIT = {0};

PFN_MOON_ASSEMBLY(command_recorder, command_recorder_assembly, device);
#define FN_MOON_COMMAND_RECORDER_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, command_recorder, command_recorder_assembly, device)

PFN_MOON_ASSEMBLY(staged_command_list, staged_command_list_assembly, command_recorder);
#define FN_MOON_STAGED_COMMAND_LIST_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, staged_command_list, staged_command_list_assembly, command_recorder)

PFN_MOON_ZERO_REFCNT(command_recorder);
#define FN_MOON_COMMAND_RECORDER_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, command_recorder)

PFN_MOON_ZERO_REFCNT(staged_command_list);
#define FN_MOON_STAGED_COMMAND_LIST_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, staged_command_list)

#define PFN_MOON_COMMAND(fn, ...) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd, __VA_ARGS__)
#define FN_MOON_COMMAND(fn, backend, ...) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd, __VA_ARGS__)

#define PFN_MOON_COMMAND_NO_ARGS(fn) \
    typedef void (LAKECALL *PFN_moon_cmd_##fn)(moon_command_recorder cmd)
#define FN_MOON_COMMAND_NO_ARGS(fn, backend) \
    void LAKECALL _moon_##backend##_cmd_##fn(moon_command_recorder cmd)

/** Copy data between buffer regions. */
PFN_MOON_COMMAND(copy_buffer, moon_cmd_copy_buffer const *params);
#define FN_MOON_CMD_COPY_BUFFER(backend) \
    FN_MOON_COMMAND(copy_buffer, moon_cmd_copy_buffer const *params)

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND(copy_buffer_to_texture, moon_cmd_copy_buffer_and_texture const *params);
#define FN_MOON_CMD_COPY_BUFFER_TO_TEXTURE(backend) \
    FN_MOON_COMMAND(copy_buffer_to_texture, moon_cmd_copy_buffer_and_texture const *params)

/** Copy data between source buffer and destination texture regions. */
PFN_MOON_COMMAND(copy_texture_to_buffer, moon_cmd_copy_buffer_and_texture const *params);
#define FN_MOON_CMD_COPY_TEXTURE_TO_BUFFER(backend) \
    FN_MOON_COMMAND(copy_texture_to_buffer, moon_cmd_copy_buffer_and_texture const *params)

/** Copy regions between textures. */
PFN_MOON_COMMAND(copy_texture, moon_cmd_copy_texture const *params);
#define FN_MOON_CMD_COPY_TEXTURE(backend) \
    FN_MOON_COMMAND(copy_texture, moon_cmd_copy_texture const *params)

/** Copy regions of source texture into a destination texture, potentially performing 
 *  format conversion, arbitrary scaling, and filtering. */
PFN_MOON_COMMAND(blit_texture, moon_cmd_blit_texture const *params);
#define FN_MOON_CMD_BLIT_TEXTURE(backend) \
    FN_MOON_COMMAND(blit_texture, moon_cmd_blit_texture const *params)

/** Resolve regions of a multisample color image to a non-multisample color image. */
PFN_MOON_COMMAND(resolve_texture, moon_cmd_resolve_texture const *params);
#define FN_MOON_CMD_RESOLVE_TEXTURE(backend) \
    FN_MOON_COMMAND(resolve_texture, moon_cmd_resolve_texture const *params)

/** Fill a region of a buffer with a fixed value, used to zero out a buffer. */
PFN_MOON_COMMAND(clear_buffer, moon_cmd_clear_buffer const *params);
#define FN_MOON_CMD_CLEAR_BUFFER(backend) \
    FN_MOON_COMMAND(clear_buffer, moon_cmd_clear_buffer const *params)

/** Fill a region of a texture with a fixed value, used to zero out a texture. */
PFN_MOON_COMMAND(clear_texture, moon_cmd_clear_texture const *params);
#define FN_MOON_CMD_CLEAR_TEXTURE(backend) \
    FN_MOON_COMMAND(clear_texture, moon_cmd_clear_texture const *params)

/** Destroys a buffer after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_buffer_deferred, moon_buffer_id buffer);
#define FN_MOON_CMD_DESTROY_BUFFER_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_buffer_deferred, moon_buffer_id buffer)

/** Destroys a texture after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_texture_deferred, moon_texture_id texture);
#define FN_MOON_CMD_DESTROY_TEXTURE_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_texture_deferred, moon_texture_id texture)

/** Destroys a texture view after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_texture_view_deferred, moon_texture_view_id texture_view);
#define FN_MOON_CMD_DESTROY_TEXTURE_VIEW_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_texture_view_deferred, moon_texture_view_id texture_view)

/** Destroys a sampler after the GPU is finished executing the command list. */
PFN_MOON_COMMAND(destroy_sampler_deferred, moon_sampler_id sampler);
#define FN_MOON_CMD_DESTROY_SAMPLER_DEFERRED(backend) \
    FN_MOON_COMMAND(destroy_sampler_deferred, moon_sampler_id sampler)

/** Build or update bottom-level and top-level acceleration structures. */
PFN_MOON_COMMAND(build_acceleration_structures, moon_cmd_build_acceleration_structures const *params);
#define FN_MOON_CMD_BUILD_ACCELERATION_STRUCTURES(backend) \
    FN_MOON_COMMAND(build_acceleration_structures, moon_cmd_build_acceleration_structures const *params)

/** Root a range of constant data to shader stages. */
PFN_MOON_COMMAND(root_constants, moon_cmd_root_constants const *params);
#define FN_MOON_CMD_ROOT_CONSTANTS(backend) \
    FN_MOON_COMMAND(root_constants, moon_cmd_root_constants const *params)

/** Bind compute pipeline for dispatch commands. */
PFN_MOON_COMMAND(set_compute_pipeline, moon_compute_pipeline pipeline);
#define FN_MOON_CMD_SET_COMPUTE_PIPELINE(backend) \
    FN_MOON_COMMAND(set_compute_pipeline, moon_compute_pipeline pipeline)

/** Bind raster pipeline for draw commands. */
PFN_MOON_COMMAND(set_raster_pipeline, moon_raster_pipeline pipeline);
#define FN_MOON_CMD_SET_RASTER_PIPELINE(backend) \
    FN_MOON_COMMAND(set_raster_pipeline, moon_raster_pipeline pipeline)

/** Bind ray tracing pipeline for trace commands. */
PFN_MOON_COMMAND(set_ray_tracing_pipeline, moon_ray_tracing_pipeline pipeline);
#define FN_MOON_CMD_SET_RAY_TRACING_PIPELINE(backend) \
    FN_MOON_COMMAND(set_ray_tracing_pipeline, moon_ray_tracing_pipeline pipeline)

/** Bind work graph pipeline for dispatch graph commands. */
PFN_MOON_COMMAND(set_work_graph_pipeline, moon_work_graph_pipeline pipeline);
#define FN_MOON_CMD_SET_WORK_GRAPH_PIPELINE(backend) \
    FN_MOON_COMMAND(set_work_graph_pipeline, moon_work_graph_pipeline pipeline)

/** Configure viewport transformation dimensions. */
PFN_MOON_COMMAND(set_viewport, lake_viewport const *viewport);
#define FN_MOON_CMD_SET_VIEWPORT(backend) \
    FN_MOON_COMMAND(set_viewport, lake_viewport const *viewport)

/** Configure scissor rectangle for fragment clipping. */
PFN_MOON_COMMAND(set_scissor, lake_rect2d const *scissor);
#define FN_MOON_CMD_SET_SCISSOR(backend) \
    FN_MOON_COMMAND(set_scissor, lake_rect2d const *scissor)

/** Configure number of rasterization samples for raster pipelines. */
PFN_MOON_COMMAND(set_rasterization_samples, moon_sample_count sample_count);
#define FN_MOON_CMD_SET_RASTERIZATION_SAMPLES(backend) \
    FN_MOON_COMMAND(set_rasterization_samples, moon_sample_count sample_count)

/** Configure depth bias constants for depth calculations. */
PFN_MOON_COMMAND(set_depth_bias, moon_depth_bias const *params);
#define FN_MOON_CMD_SET_DEPTH_BIAS(backend) \
    FN_MOON_COMMAND(set_depth_bias, moon_depth_bias const *params)

/** Configure index buffer for indexed draw calls. */
PFN_MOON_COMMAND(set_index_buffer, moon_cmd_set_index_buffer const *params);
#define FN_MOON_CMD_SET_INDEX_BUFFER(backend) \
    FN_MOON_COMMAND(set_index_buffer, moon_cmd_set_index_buffer const *params)

/** Insert pipeline memory barriers and texture barriers for layout transitions. Successive barriers_and_transitions 
 *  calls are combined, as soon as any other command is recorder, the barrier batch is flushed. */
PFN_MOON_COMMAND(barriers_and_transitions, moon_cmd_barriers_and_transitions const *params);
#define FN_MOON_CMD_BARRIERS_AND_TRANSITIONS(backend) \
    FN_MOON_COMMAND(barriers_and_transitions, moon_cmd_barriers_and_transitions const *params)

/** Start a renderpass with specified render targets for draw commands. */
PFN_MOON_COMMAND(begin_renderpass, moon_cmd_begin_renderpass const *params);
#define FN_MOON_CMD_BEGIN_RENDERPASS(backend) \
    FN_MOON_COMMAND(begin_renderpass, moon_cmd_begin_renderpass const *params)

/** End the current renderpass. */
PFN_MOON_COMMAND_NO_ARGS(end_renderpass);
#define FN_MOON_CMD_END_RENDERPASS(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_renderpass)

/** Record timestamp values into a query pool. */
PFN_MOON_COMMAND(write_timestamps, moon_cmd_write_timestamps const *params);
#define FN_MOON_CMD_WRITE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(write_timestamps, moon_cmd_write_timestamps const *params)

/** Reset timestamp values in the query pool. */
PFN_MOON_COMMAND(resolve_timestamps, moon_cmd_resolve_timestamps const *params);
#define FN_MOON_CMD_RESOLVE_TIMESTAMPS(backend) \
    FN_MOON_COMMAND(resolve_timestamps, moon_cmd_resolve_timestamps const *params)

/** Begin a debug label. */
PFN_MOON_COMMAND(begin_label, moon_cmd_begin_label const *params);
#define FN_MOON_CMD_BEGIN_LABEL(backend) \
    FN_MOON_COMMAND(begin_label, moon_cmd_begin_label const *params)

/** End a debug label. */
PFN_MOON_COMMAND_NO_ARGS(end_label);
#define FN_MOON_CMD_END_LABEL(backend) \
    FN_MOON_COMMAND_NO_ARGS(end_label)

/** Execute ray tracing workload with direct parameters. */
PFN_MOON_COMMAND(trace_rays, moon_cmd_trace_rays const *params);
#define FN_MOON_CMD_TRACE_RAYS(backend) \
    FN_MOON_COMMAND(trace_rays, moon_cmd_trace_rays const *params)

/** Execute ray tracing workload using indirect parameters buffer. */
PFN_MOON_COMMAND(trace_rays_indirect, moon_cmd_trace_rays_indirect const *params);
#define FN_MOON_CMD_TRACE_RAYS_INDIRECT(backend) \
    FN_MOON_COMMAND(trace_rays_indirect, moon_cmd_trace_rays_indirect const *params)

/** Execute compute workgroups with direct parameters. */
PFN_MOON_COMMAND(dispatch, moon_cmd_dispatch const *params);
#define FN_MOON_CMD_DISPATCH(backend) \
    FN_MOON_COMMAND(dispatch, moon_cmd_dispatch const *params)

/** Execute compute workgroups with indirect parameters buffer. */
PFN_MOON_COMMAND(dispatch_indirect, moon_cmd_dispatch_indirect const *params);
#define FN_MOON_CMD_DISPATCH_INDIRECT(backend) \
    FN_MOON_COMMAND(dispatch_indirect, moon_cmd_dispatch_indirect const *params)

/** Execute dispatch nodes with direct parameters. */
PFN_MOON_COMMAND(dispatch_graph, moon_cmd_dispatch_graph const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH(backend) \
    FN_MOON_COMMAND(dispatch_graph, moon_cmd_dispatch_graph const *params)

/** Execute dispatch nodes and payload parameters read on the device. */
PFN_MOON_COMMAND(dispatch_graph_indirect, moon_cmd_dispatch_graph const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT(backend) \
    FN_MOON_COMMAND(dispatch_graph_indirect, moon_cmd_dispatch_graph const *params)

/** Execute dispatch nodes with all parameters read on the device. */
PFN_MOON_COMMAND(dispatch_graph_indirect_count, moon_cmd_dispatch_graph_count const *params);
#define FN_MOON_CMD_DISPATCH_GRAPH_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(dispatch_graph_indirect_count, moon_cmd_dispatch_graph_count const *params)

/** Draw non-indexed primitives with vertex count. */
PFN_MOON_COMMAND(draw, moon_cmd_draw const *params);
#define FN_MOON_CMD_DRAW(backend) \
    FN_MOON_COMMAND(draw, moon_cmd_draw const *params)

/** Draw indexed primitives using bound index buffer. */
PFN_MOON_COMMAND(draw_indexed, moon_cmd_draw_indexed const *params);
#define FN_MOON_CMD_DRAW_INDEXED(backend) \
    FN_MOON_COMMAND(draw_indexed, moon_cmd_draw_indexed const *params)

/** Draw primitives using indirect parameters buffer. */
PFN_MOON_COMMAND(draw_indirect, moon_cmd_draw_indirect const *params);
#define FN_MOON_CMD_DRAW_INDIRECT(backend) \
    FN_MOON_COMMAND(draw_indirect, moon_cmd_draw_indirect const *params)

/** Draw primitives using indirect parameters buffer with device count argument. */
PFN_MOON_COMMAND(draw_indirect_count, moon_cmd_draw_indirect_count const *params);
#define FN_MOON_CMD_DRAW_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(draw_indirect_count, moon_cmd_draw_indirect_count const *params)

/** Execute mesh shader workgroups with direct parameters. */
PFN_MOON_COMMAND(draw_mesh_tasks, moon_cmd_draw_mesh_tasks const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks, moon_cmd_draw_mesh_tasks const *params)

/** Execute mesh shader workgroups using indirect parameters buffer. */
PFN_MOON_COMMAND(draw_mesh_tasks_indirect, moon_cmd_draw_mesh_tasks_indirect const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks_indirect, moon_cmd_draw_mesh_tasks_indirect const *params)

/** Execute mesh shader workgroups using indirect parameters buffer with deferred count argument. */
PFN_MOON_COMMAND(draw_mesh_tasks_indirect_count, moon_cmd_draw_mesh_tasks_indirect_count const *params);
#define FN_MOON_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT(backend) \
    FN_MOON_COMMAND(draw_mesh_tasks_indirect_count, moon_cmd_draw_mesh_tasks_indirect_count const *params)
