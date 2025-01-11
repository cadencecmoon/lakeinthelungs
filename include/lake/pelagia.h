/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_PELAGIA_H
#define _AMW_PELAGIA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/assert.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/os.h>

#include <lake/datastructures/arena_allocator.h>

#include <lake/ipomoeaalba.h>
#include <lake/hadopelagic.h>
#include <lake/riven.h>
#include <lake/silver.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A handle that represents a rendering resource. */
typedef u64 pelagia_resource_t;

/** Types of command queues used in the rendering command stream. */
enum pelagia_command_queue_type {
    pelagia_command_queue_type_none      = 0x00,
    pelagia_command_queue_type_graphics  = 0x01,
    pelagia_command_queue_type_compute   = 0x02,
    pelagia_command_queue_type_transfer  = 0x04,
    pelagia_command_queue_type_all       = 0x07,
};

/** Types of commands to express how a scene should be updated and rendered.
 *  Backends are allowed to control how this is done, including render state changes
 *  and resource barriers or transitions. */
enum pelagia_command_type {
    pelagia_command_type_none = 0,
    pelagia_command_type_draw,
    pelagia_command_type_draw_indirect,
    pelagia_command_type_dispatch,
    pelagia_command_type_dispatch_indirect,
    pelagia_command_type_update_buffer,
    pelagia_command_type_update_texture,
    pelagia_command_type_copy_buffer,
    pelagia_command_type_copy_texture,
    pelagia_command_type_barriers,
    pelagia_command_type_transitions,
    pelagia_command_type_begin_timing,
    pelagia_command_type_end_timing,
    pelagia_command_type_resolve_profiling,
    pelagia_command_type_begin_event,
    pelagia_command_type_end_event,
    pelagia_command_type_begin_render_pass,
    pelagia_command_type_end_render_pass,
    pelagia_command_type_ray_trace,
    pelagia_command_type_update_top_level,
    pelagia_command_type_update_bottom_level,
    pelagia_command_type_update_shader_table,
};

/** Draw rendering command. */
struct pelagia_command_draw {
    u32 cmdtype;        /**< pelagia_command_type_draw */
    u32 queuetype;      /**< graphics */
    /* TODO */
};

/** Draw indirect rendering command. */
struct pelagia_command_draw_indirect {
    u32 cmdtype;        /**< pelagia_command_type_draw_indirect */
    u32 queuetype;      /**< graphics */
    /* TODO */
};

/** Dispatch rendering command. */
struct pelagia_command_dispatch {
    u32 cmdtype;        /**< pelagia_command_type_dispatch */
    u32 queuetype;      /**< compute */
    /* TODO pipeline state, shaders */
    u32 dispatch_x;
    u32 dispatch_y;
    u32 dispatch_z;
};

/** Dispatch indirect rendering command. */
struct pelagia_command_dispatch_indirect {
    u32 cmdtype;        /**< pelagia_command_type_dispatch_indirect */
    u32 queuetype;      /**< compute */
    /* TODO */
};

/** Update buffer rendering command. */
struct pelagia_command_update_buffer {
    u32 cmdtype;        /**< pelagia_command_type_update_buffer */
    u32 queuetype;      /**< transfer */
    /* TODO */
};

/** Update texture rendering command. */
struct pelagia_command_update_texture {
    u32 cmdtype;        /**< pelagia_command_type_update_texture */
    u32 queuetype;      /**< transfer */
    /* TODO */
};

/** Copy buffer rendering command. */
struct pelagia_command_copy_buffer {
    u32 cmdtype;        /**< pelagia_command_type_copy_buffer */
    u32 queuetype;      /**< transfer */
    /* TODO */
};

/** Copy texture rendering command. */
struct pelagia_command_copy_texture {
    u32 cmdtype;        /**< pelagia_command_type_copy_texture */
    u32 queuetype;      /**< transfer */
    /* TODO */
};

/** Barriers rendering command. */
struct pelagia_command_barriers {
    u32 cmdtype;        /**< pelagia_command_type_barriers */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** Transitions rendering command. */
struct pelagia_command_transitions {
    u32 cmdtype;        /**< pelagia_command_type_transitions */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** Begin timing rendering command. */
struct pelagia_command_begin_timing {
    u32 cmdtype;        /**< pelagia_command_type_begin_timing */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** End timing rendering command. */
struct pelagia_command_end_timing {
    u32 cmdtype;        /**< pelagia_command_type_end_timing */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** Resolve profiling rendering command. */
struct pelagia_command_resolve_profiling {
    u32 cmdtype;        /**< pelagia_command_type_resolve_profiling */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** Begin event rendering command. */
struct pelagia_command_begin_event {
    u32 cmdtype;        /**< pelagia_command_type_begin_event */
    u32 queuetype;      /**< graphics + compute */
    /* TODO */
};

/** End event rendering command. */
struct pelagia_command_end_event {
    u32 cmdtype;        /**< pelagia_command_type_end_event */
    u32 queuetype;      /**< graphics + compute */
    /* TODO */
};

/** Begin render pass rendering command. */
struct pelagia_command_begin_render_pass {
    u32 cmdtype;        /**< pelagia_command_type_begin_render_pass */
    u32 queuetype;      /**< graphics */
    /* TODO */
};

/** End render pass rendering command. */
struct pelagia_command_end_render_pass {
    u32 cmdtype;        /**< pelagia_command_type_end_render_pass */
    u32 queuetype;      /**< graphics */
    /* TODO */
};

/** Ray trace rendering command. */
struct pelagia_command_ray_trace {
    u32 cmdtype;        /**< pelagia_command_type_ray_trace */
    u32 queuetype;      /**< graphics + compute */
    /* TODO */
};

/** Update top level acceleration mesh rendering command. */
struct pelagia_command_update_top_level {
    u32 cmdtype;        /**< pelagia_command_type_update_top_level */
    u32 queuetype;      /**< compute */
    /* TODO */
};

/** Update bottom level acceleration mesh rendering command. */
struct pelagia_command_update_bottom_level {
    u32 cmdtype;        /**< pelagia_command_type_update_bottom_level */
    u32 queuetype;      /**< compute */
    /* TODO */
};

/** Update shader table rendering command. */
struct pelagia_command_update_shader_table {
    u32 cmdtype;        /**< pelagia_command_type_update_shader_table */
    u32 queuetype;      /**< all */
    /* TODO */
};

/** Collects all possible rendering command objects in an union. */
union pelagia_command {
    /** Common members of all rendering commands. */
    struct {
        u32 cmdtype;        /**< The type of the rendering command, declares what pelagia_command_* structure is to be used. */
        u32 queuetype;      /**< Every command must declare the command queues it will be using (graphics / compute / transfer). */
    };
    struct pelagia_command_draw                     draw;
    struct pelagia_command_draw_indirect            draw_indirect;
    struct pelagia_command_dispatch                 dispatch;
    struct pelagia_command_dispatch_indirect        dispatch_indirect;
    struct pelagia_command_update_buffer            update_buffer;
    struct pelagia_command_update_texture           update_texture;
    struct pelagia_command_copy_buffer              copy_buffer;
    struct pelagia_command_copy_texture             copy_texture;
    struct pelagia_command_barriers                 barriers;
    struct pelagia_command_transitions              transitions;
    struct pelagia_command_begin_timing             begin_timing;
    struct pelagia_command_end_timing               end_timing;
    struct pelagia_command_resolve_profiling        resolve_profiling;
    struct pelagia_command_begin_event              begin_event;
    struct pelagia_command_end_event                end_event;
    struct pelagia_command_begin_render_pass        begin_render_pass;
    struct pelagia_command_end_render_pass          end_render_pass;
    struct pelagia_command_ray_trace                ray_trace;
    struct pelagia_command_update_top_level         update_top_level;
    struct pelagia_command_update_bottom_level      update_bottom_level;
    struct pelagia_command_update_shader_table      update_shader_table;
    u8 padding[20];
};

static_assertion(sizeof(union pelagia_command) == sizeof(((union pelagia_command *)NULL)->padding), "Union pelagia_command padding is not large enough to fit all possible command objects.");

/* TODO implement the high level command stream */

/* forward declarations */
struct pelagia;
struct pelagia_pass;
struct pelagia_render_graph;

/** A render pass can specify the scheduling, and also request transfers 
 *  in or out of the pass. The render graph controls them. */
enum pelagia_render_graph_schedule {
    pelagia_render_graph_schedule_no_devices                = 0x01, /**< The pass is disabled. */
    pelagia_render_graph_schedule_all_devices               = 0x02, /**< Pass runs on all devices. */
    pelagia_render_graph_schedule_primary_device            = 0x04, /**< Pass only runs on the primary device. */
    pelagia_render_graph_schedule_secondary_devices         = 0x08, /**< Pass runs on secondaries if GPU count > 1, otherwise primary. */
    pelagia_render_graph_schedule_only_secondary_devices    = 0x10, /**< Pass only runs on secondaries, disabled unless mGPU. */
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
enum pelagia_render_graph_transfer_partition {
    pelagia_render_graph_transfer_partition_all             = 0x01, /**< Select all partitions from device. */
    pelagia_render_graph_transfer_partition_isolated        = 0x02, /**< Select isolated region from device. */
};

/** Transfers specify what partitions are copied from source to destination, and specify 
 *  rules for transfer destination, allowing for basic copies, multi-cast copies, etc. */
enum pelagia_render_graph_transfer_filter {
    pelagia_render_graph_transfer_filter_all_devices        = 0x01, /**< Transfer completes on all devices. */
    pelagia_render_graph_transfer_filter_primary_device     = 0x02, /**< Transfer completes on the primary device. */
    pelagia_render_graph_transfer_filter_secondary_devices  = 0x04, /**< Transfer completes on all secondary devices. */
};

/** An identifier for GPU state resources, associated with a render pass.
 *  Those will be compiled shaders and pipelines. */
enum pelagia_shader_pipeline {
    pelagia_shader_pipeline_invalid = 0,

    pelagia_render_graph_pass_gbuffer,
    pelagia_render_graph_pass_present,
    pelagia_render_graph_pass_ray_tracing,
    pelagia_render_graph_pass_shading,

    pelagia_render_graph_pass_count,    /**< Can be used to declare application-defined render passes. */
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
struct pelagia_render_graph {
    u32 todo;
};

/** TODO A tear to define render pass callbacks, that will record to the command stream.
 *  After a high-level command stream is evaluated, they will be sent to the rendering 
 *  backend to be recorded to command buffers. */
RIVENS_TEAR_TYPEDEF(PFN_pelagia_record_render_pass, struct pelagia *pelagia);

/** Defines a render pass for recording high-level commands in the render graph.
 *  A pass will need to reference device-specific render graph resources, like shader 
 *  pipelines, so they can be referenced back in the backend. The callback to be 
 *  registered in this structure is supposed to prepare the work and record commands 
 *  later, after the render graph for a frame has been constructed. */
struct pelagia_pass {
    u32                 render_graph_pass;
    u8                  render_graph_sheduling;
    u8                  src_transfer_partition;
    u8                  dst_transfer_partition;
    u8                  dst_transfer_filter;
    struct rivens_tear  tear; /* Callback to record high-level commands. */
};

/** Entry point to the rendering backend. 
 *  @return True if loading the backend module and procedures was successful. */
typedef s32 (*PFN_pelagia_entry_point)(struct pelagia *pelagia, struct ipomoeaalba *ia);

/** Defines ID's of supported rendering backends. */
enum pelagia_backend_api {
    pelagia_backend_api_vulkan = 0,
    pelagia_backend_api_d3dx12,      // TODO reserved id
    pelagia_backend_api_metal3,      // TODO reserved id
    pelagia_backend_api_webgpu,      // TODO reserved id
    pelagia_backend_api_proxy,       // TODO reserved id
    pelagia_backend_api_mock,
};

AMWAPI s32 pelagia_vulkan_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);
AMWAPI s32 pelagia_mock_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);
/** Select the first appropriate rendering backend supported by this build. */
AMWAPI s32 pelagia_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);

/** Initialize the internal rendering backend. */
typedef s32 (*PFN_pelagia_renderer_init)(
    struct pelagia         *pelagia, 
    struct ipomoeaalba     *ia, 
    struct hadopelagic     *hadal, 
    const char             *application_name, 
    u32                     application_version, 
    struct arena_allocator *temp_arena);

/** Finalize the internal rendering backend. */
typedef void (*PFN_pelagia_renderer_fini)(struct pelagia *pelagia);

/** Create new rendering devices from physical GPUs for this backend. */
typedef s32 (*PFN_pelagia_construct_devices)(
    struct pelagia         *pelagia,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    struct arena_allocator *temp_arena);

/** Destroy all existing rendering devices for this backend. */
typedef void (*PFN_pelagia_destroy_devices)(struct pelagia *pelagia);

/** Using the display backend, create a swapchain surface we can draw to. */
typedef s32 (*PFN_pelagia_create_swapchain_surface)(struct pelagia *pelagia, struct hadopelagic *hadal);

/** An argument holding work for creating/recreating a swapchain. */
struct pelagia_construct_swapchain_work {
    struct pelagia     *pelagia;         /**< Holds the swapchain and a device that controls it. */
    struct hadopelagic *hadal;          /**< The display backend will provide necessary window info. */
    struct ipomoeaalba *ia;             /**< Needed for initialization of the swapchain arena, can be NULL'ed afterwards */
    b32                 use_vsync;      /**< Whether to enable vertical synchronization. */
    s32                 out_result;     /**< A return code, check for errors. */
};

/** Creates or recreates the swapchain, for a given display backend and main rendering device. */
RIVENS_TEAR_TYPEDEF(PFN_pelagia_construct_swapchain_tear, struct pelagia_construct_swapchain_work *work);

/** Internal calls to be implemented by a rendering backend and assigned from the entry point. */
struct pelagia_calls {
    PFN_pelagia_renderer_init            renderer_init;
    PFN_pelagia_renderer_fini            renderer_fini;
    PFN_pelagia_construct_devices        construct_devices;
    PFN_pelagia_destroy_devices          destroy_devices;
    PFN_pelagia_create_swapchain_surface create_swapchain_surface;

    /* Tears for parallel work. */
    PFN_pelagia_construct_swapchain_tear construct_swapchain_tear;
};

/** Boolean flags describing the state of the rendering context. */
enum pelagia_flags {
    pelagia_flag_initialized             = (1u << 0),  /**< Set on a valid rendering backend flags value. */
    pelagia_flag_vsync_enabled           = (1u << 1),  /**< Whether vertical synchronization is being used. */
    pelagia_flag_screenshot_supported    = (1u << 2),  /**< Set if the swapchain allows reading and saving the image data. */

    pelagia_flag_reserved_bits           = 20,
};
#define pelagia_flag_mask_reserved ((1u << pelagia_flag_reserved_bits) - 1)

/** Boolean flags describing the state of an explicit rendering device.
 *  The device state is hold internally and API-dependent, but we can use these flags 
 *  to either control or read the state of the device. It's internally represented by 
 *  an atomic uint64_t value, bits from 0 to 39 are reserved, flags from 40 to 63 bits 
 *  are for internal use and as such are backend-dependent. */
enum pelagia_device_flags {
    pelagia_device_flag_is_valid = (1llu << 0), /**< Set on a valid rendering device flags value. */
    pelagia_device_flag_primary  = (1llu << 1), /**< True for only one device, it's responsible for controlling the rendering and shared internal constructs, like the swapchain. */

    /** Checks if the device supports accelerated ray tracing meshes - if not, ray tracing passes may only use software implementations instead. */
    pelagia_device_flag_accelerated_ray_tracing_supported = (1llu << 10),

    pelagia_device_flag_reserved_bits = 40,
}; 
/** A bitmask for the reserved bits of a cobalt_device_flags value. */
#define pelagia_device_flag_mask_reserved ((1llu << pelagia_device_flag_reserved_bits) - 1)

/** The renderer. */
struct pelagia {
    at_s32                          flags;          /**< Any write-modify operations must be externally synchronized. */

    void                           *backend;        /**< A rendering context shared between devices. */
    void                           *devices;        /**< An array of rendering devices, representing an execution context per GPU. The device of idx 0 is always the main GPU. */
    u32                             device_count;   /**< How many devices are available in the renderer. This value will not change during the lifetime of a rendering backend. */

    u32                             backend_api;    /**< Either enum cobalt_backend_api or a custom identifier. */
    const char                     *backend_name;   /**< A readable name of the running rendering backend, for logging purposes. */
    struct pelagia_calls            calls;

    struct pelagia_render_graph     render_graph;   /**< The main render graph, collecting all GPU workload. */
};

AMWAPI s32 pelagia_init(
    PFN_pelagia_entry_point entry_point__,
    struct pelagia         *pelagia, 
    struct ipomoeaalba     *ia,
    struct hadopelagic     *hadal, 
    const char             *application_name,
    u32                     application_version,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    b32                     enable_vsync);

AMWAPI void pelagia_fini(struct pelagia *pelagia);

/* Inline functions to fill backend tears. */

AMW_INLINE void pelagia_construct_swapchain_tear__(
    struct pelagia_construct_swapchain_work *work,
    struct rivens_tear                      *tear) 
{
    tear->procedure = (PFN_riven_tear)work->pelagia->calls.construct_swapchain_tear;
    tear->argument  = work;
    tear->name      = "pelagia:construct_swapchain_tear";
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PELAGIA_H */
