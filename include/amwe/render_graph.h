#pragma once

#include <amwe/xaku/device.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum render_attachment_type {
    render_attachment_type_undefined = 0,
    render_attachment_type_buffer,
    render_attachment_type_texture,
    render_attachment_type_tlas,
    render_attachment_type_blas,
};

struct render_buffer_attachment_index   { u32 value; };
struct render_texture_attachment_index  { u32 value; };
struct render_tlas_attachment_index     { u32 value; };
struct render_blas_attachment_index     { u32 value; };

enum render_access_type {
    render_access_type_none                     = 0,
    render_access_type_concurrent               = (1u << 0),
    render_access_type_read                     = (1u << 1) | render_access_type_concurrent,
    render_access_type_sampled                  = (1u << 2) | render_access_type_concurrent,
    render_access_type_write                    = (1u << 3),
    render_access_type_read_write               = (1u << 1) | render_access_type_write,
    render_access_type_write_concurrent         = render_access_type_write | render_access_type_concurrent,
    render_access_type_read_write_concurrent    = render_access_type_read_write | render_access_type_concurrent,
};

enum render_stage {
    render_stage_none = 0,
    render_stage_vertex_shader,
    render_stage_tesselation_control_shader,
    render_stage_tesselation_evaluation_shader,
    render_stage_geometry_shader,
    render_stage_fragment_shader,
    render_stage_task_shader,
    render_stage_mesh_shader,
    render_stage_pre_rasterization_shaders,
    render_stage_raster_shader,
    render_stage_compute_shader,
    render_stage_ray_tracing_shader,
    render_stage_shader,
    render_stage_color_attachment,
    render_stage_depth_stencil_attachment,
    render_stage_resolve,
    render_stage_present,
    render_stage_indirect_command,
    render_stage_index_input,
    render_stage_transfer,
    render_stage_host,
    render_stage_acceleration_structure_build,
    render_stage_any_command,
};

struct lake_alignment(lake_alignof(u32))
render_access {
    u8  restriction;    /**< enum render_attachment_type */
    u8  type;           /**< enum render_access_type */
    u16 stage;          /**< enum render_stage */
};

enum render_task_type {
    render_task_type_undefined = 0,
    render_task_type_general,
    render_task_type_raster,
    render_task_type_compute,
    render_task_type_ray_tracing,
    render_task_type_transfer,
    render_task_type_sparse_binding,
    render_task_type_video,
};

struct render_gpu_resource_view {
    u32                     render_graph_index;
    u32                     index;
};
#define render_gpu_resource_view_null(T) \
    ((T){ .render_graph_index = UINT32_MAX, .index = UINT32_MAX })

struct render_buffer_view {
    u32                     render_graph_index;
    u32                     index;
    enum render_stage       stage_override;
    enum render_access_type access_type_override;
};

struct render_texture_view {
    u32                                 render_graph_index;
    u32                                 index;
    struct xaku_texture_mip_array_slice slice;
    enum render_stage                   stage_override;
    enum render_access_type             access_type_override;
};

struct render_tlas_view {
    u32                     render_graph_index;
    u32                     index;
    enum render_stage       stage_override;
    enum render_access_type access_type_override;
};

struct render_blas_view {
    u32                     render_graph_index;
    u32                     index;
    enum render_stage       stage_override;
    enum render_access_type access_type_override;
};

/** A render pass can specify valid scheduling, and also request transfers 
 *  in or out of the pass. The render graph controls them. */
enum render_schedule {
    render_schedule_no_devices                = (1u << 0), /**< The pass is disabled. */
    render_schedule_all_devices               = (1u << 1), /**< Pass runs on all devices. */
    render_schedule_primary_device            = (1u << 2), /**< Pass only runs on the primary device. */
    render_schedule_secondary_devices         = (1u << 3), /**< Pass runs on secondaries if GPU count > 1, otherwise primary. */
    render_schedule_only_secondary_devices    = (1u << 4), /**< Pass only runs on secondaries, disabled unless mGPU. */
};

/** With sort-first approach, work is divided between devices as partitions on the X-axis (width). 
 *  This specifies rules for the transfer for both the source and the destination GPU(s),
 *  allowing for a variety of transfer patterns:
 *
 *  partition_all -> partition_all
 *  - Copies full resource on one GPU to full resource on all specified GPUs.
 *
 *  partition_all -> partition_isolated 
 *  - Copies full resource on one GPU to isolated regions on all specified GPUs (partial copies).
 *
 *  partition_isolated -> partition_all 
 *  - (invalid configuration, will be asserted).
 *
 *  partition_isolated -> partition_isolated 
 *  - Copies isolated region on one GPU to isolated regions on all specified GPUs (partial copies). */
enum render_transfer_partition {
    render_transfer_partition_all             = (1u << 0),  /**< Select all partitions from device. */
    render_transfer_partition_isolated        = (1u << 1),  /**< Select isolated region from device. */
};

/** Transfers specify what partitions are copied from source to destination, and specify 
 *  rules for transfer destination, allowing for basic copies, multi-cast copies, etc. */
enum render_transfer_filter {
    render_transfer_filter_all_devices        = (1u << 0),  /**< Transfer completes on all devices. */
    render_transfer_filter_primary_device     = (1u << 1),  /**< Transfer completes on the primary device. */
    render_transfer_filter_secondary_devices  = (1u << 2),  /**< Transfer completes on all secondary devices. */
};

/** The purpose of a render graph is to handle transient resources, manage lifetimes efficiently,
 *  include the proper resource barriers and transitions, schedule the command queues, manage device 
 *  memory, split work between multiple rendering devices (mostly implicit and automatic) and have a 
 *  single implementation regardless of the backend graphics API. */
struct render_graph {
    /** A rendering device is the owner of a render graph. Multiple render graphs may be used for 
     *  different frequencies of submit (e.g. main rendering, async compute, async transfer), 
     *  and in a mGPU context, a render graph instance may not be shared between devices, 
     *  but resource handles and render passes may. */
    struct xaku_device *device;
    /** The lifetime of this render graph. */
    u64                 refcnt;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
