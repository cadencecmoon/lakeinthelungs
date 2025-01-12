/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_RENDER_GRAPH_H
#define _AMW_RENDER_GRAPH_H

#include <lake/bedrock/defines.h>

#include <lake/riven.h>

struct pelagia;
struct silver;

/* TODO */
struct render_graph;

/** A handle that represents a rendering resource - buffer, textures, pipelines. */
typedef u64 render_resource_id;

/** A render pass can specify the scheduling, and also request transfers 
 *  in or out of the pass. The render graph controls them. */
enum render_graph_schedule {
    render_graph_schedule_no_devices                = 0x01, /**< The pass is disabled. */
    render_graph_schedule_all_devices               = 0x02, /**< Pass runs on all devices. */
    render_graph_schedule_primary_device            = 0x04, /**< Pass only runs on the primary device. */
    render_graph_schedule_secondary_devices         = 0x08, /**< Pass runs on secondaries if GPU count > 1, otherwise primary. */
    render_graph_schedule_only_secondary_devices    = 0x10, /**< Pass only runs on secondaries, disabled unless mGPU. */
};

/** Work is divided between devices as partitions on the X-axis (width). 
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
enum render_graph_transfer_partition {
    render_graph_transfer_partition_all             = 0x01, /**< Select all partitions from device. */
    render_graph_transfer_partition_isolated        = 0x02, /**< Select isolated region from device. */
};

/** Transfers specify what partitions are copied from source to destination, and specify 
 *  rules for transfer destination, allowing for basic copies, multi-cast copies, etc. */
enum render_graph_transfer_filter {
    render_graph_transfer_filter_all_devices        = 0x01, /**< Transfer completes on all devices. */
    render_graph_transfer_filter_primary_device     = 0x02, /**< Transfer completes on the primary device. */
    render_graph_transfer_filter_secondary_devices  = 0x04, /**< Transfer completes on all secondary devices. */
};

/** Different render targets used in render passes, except for swapchain images.
 *  The total amount of render targets is multiplied by each swapchain image. */
enum render_target_type {
    render_target_type_depth_buffer = 0u,
    render_target_type_visibility_buffer,
    /** The total amount of render targets. */
    render_target_type_count,
};

/** The different render passes implemented with Vulkan. Used for a constant-time lookup
 *  of shader pipeline states and descriptor sets. */
enum render_pass_type {
    /** The pass that renders a screen filling triangle to perform deferred shading. 
     *  This pass will be later expanded into multiple independent passes, for now I need 
     *  just anything that will work. */
    render_pass_type_shading = 0u,
    /** The pass that produces the visibility buffer by rasterizing all geometry once. */
    render_pass_type_visibility,
    /** The total amount of render passes. */
    render_pass_type_count,
};

/** TODO study the Frostbite's Frame Graph.
 *
 *  The purpose of a rendering graph is to handle transient resources,
 *  manage lifetimes efficiently, include the proper resource barriers and transitions,
 *  automatic queue sheduling, device memory management, splitting the work between multiple
 *  devices (mostly implicit and automatic, for scaling) and have a single implementation 
 *  regardless of the running backend. A main render graph, and render graphs per device.
 *  The structure below is for the primary render graph, to control all rendering flow 
 *  and GPU workload. Secondary graphs may be constructed later. */
struct render_graph {
    u32 todo;
};

#endif /* _AMW_RENDER_GRAPH_H */
