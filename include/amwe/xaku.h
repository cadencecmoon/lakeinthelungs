#pragma once

#include <amwe/xaku/encore.h>
#include <amwe/xaku/commands.h>
#include <amwe/xaku/gpu_resources.h>
#include <amwe/xaku/pipelines.h>
#include <amwe/xaku/device.h>
#include <amwe/render_graph.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the rendering engine, implemented by a backend 'xaku_encore'.
 *
 *  The encore 'userdata' type is 'struct xaku_encore_assembly'. */
struct xaku_interface {
    struct riven_interface_header                   header;

    PFN_xaku_list_devices_properties                list_devices_properties;
    PFN_xaku_device_assembly                        device_assembly;
    PFN_xaku_device_disassembly                     device_disassembly;
    PFN_xaku_device_queue_count                     device_queue_count;
    PFN_xaku_device_queue_wait_idle                 device_queue_wait_idle;
    PFN_xaku_device_wait_idle                       device_wait_idle;
    PFN_xaku_device_submit                          device_submit;
    PFN_xaku_device_present                         device_present;
    /** Should be called at some point, to destroy any zombified resources (memory, query pools, swapchains, pipelines, etc.). */
    PFN_xaku_deferred_resource_disassembly          deferred_resource_disassembly;

    PFN_xaku_memory_assembly                        memory_assembly;
    PFN_xaku_memory_disassembly                     memory_disassembly;
    PFN_xaku_memory_buffer_requirements             memory_buffer_requirements;
    PFN_xaku_memory_texture_requirements            memory_texture_requirements;

    PFN_xaku_query_pool_assembly                    query_pool_assembly;
    PFN_xaku_query_pool_disassembly                 query_pool_disassembly;

    PFN_xaku_swapchain_assembly                     swapchain_assembly;
    PFN_xaku_swapchain_disassembly                  swapchain_disassembly;

    PFN_xaku_compute_pipeline_assembly              compute_pipeline_assembly;
    PFN_xaku_compute_pipeline_disassembly           compute_pipeline_disassembly;

    PFN_xaku_raster_pipeline_assembly               raster_pipeline_assembly;
    PFN_xaku_raster_pipeline_disassembly            raster_pipeline_disassembly;

    PFN_xaku_ray_tracing_pipeline_assembly          ray_tracing_pipeline_assembly;
    PFN_xaku_ray_tracing_pipeline_disassembly       ray_tracing_pipeline_disassembly;

    PFN_xaku_command_recorder_assembly              command_recorder_assembly;
    PFN_xaku_command_recorder_disassembly           command_recorder_disassembly;

    PFN_xaku_create_buffer                          create_buffer;
    PFN_xaku_create_buffer_from_memory              create_buffer_from_memory;
    PFN_xaku_create_texture                         create_texture;
    PFN_xaku_create_texture_from_memory             create_texture_from_memory;
    PFN_xaku_create_texture_view                    create_texture_view;
    PFN_xaku_create_sampler                         create_sampler;
    PFN_xaku_create_tlas                            create_tlas;
    PFN_xaku_create_blas                            create_blas;
    PFN_xaku_create_tlas_from_buffer                create_tlas_from_buffer;
    PFN_xaku_create_blas_from_buffer                create_blas_from_buffer;

    PFN_xaku_get_buffer_assembly                    get_buffer_assembly;
    PFN_xaku_get_texture_assembly                   get_texture_assembly;
    PFN_xaku_get_texture_view_assembly              get_texture_view_assembly;
    PFN_xaku_get_sampler_assembly                   get_sampler_assembly;
    PFN_xaku_get_tlas_assembly                      get_tlas_assembly;
    PFN_xaku_get_tlas_build_sizes                   get_tlas_build_sizes;
    PFN_xaku_get_blas_assembly                      get_blas_assembly;
    PFN_xaku_get_blas_build_sizes                   get_blas_build_sizes;

    PFN_xaku_destroy_buffer                         destroy_buffer;
    PFN_xaku_destroy_texture                        destroy_texture;
    PFN_xaku_destroy_texture_view                   destroy_texture_view;
    PFN_xaku_destroy_sampler                        destroy_sampler;
    PFN_xaku_destroy_tlas                           destroy_tlas;
    PFN_xaku_destroy_blas                           destroy_blas;

    PFN_xaku_cmd_copy_buffer_to_buffer              cmd_copy_buffer_to_buffer;
    PFN_xaku_cmd_copy_buffer_to_texture             cmd_copy_buffer_to_texture;
    PFN_xaku_cmd_copy_texture_to_buffer             cmd_copy_texture_to_buffer;
    PFN_xaku_cmd_copy_texture_to_texture            cmd_copy_texture_to_texture;
    PFN_xaku_cmd_blit_texture_to_texture            cmd_blit_texture_to_texture;
    PFN_xaku_cmd_clear_buffer                       cmd_clear_buffer;
    PFN_xaku_cmd_clear_texture                      cmd_clear_texture;
    PFN_xaku_cmd_build_acceleration_structures      cmd_build_acceleration_structures;
    PFN_xaku_cmd_destroy_buffer_deferred            cmd_destroy_buffer_deferred;
    PFN_xaku_cmd_destroy_texture_deferred           cmd_destroy_texture_deferred;
    PFN_xaku_cmd_destroy_texture_view_deferred      cmd_destroy_texture_view_deferred;
    PFN_xaku_cmd_destroy_sampler_deferred           cmd_destroy_sampler_deferred;
    PFN_xaku_cmd_destroy_tlas_deferred              cmd_destroy_tlas_deferred;
    PFN_xaku_cmd_destroy_blas_deferred              cmd_destroy_blas_deferred;
    PFN_xaku_cmd_push_constant                      cmd_push_constant;
    PFN_xaku_cmd_set_compute_pipeline               cmd_set_compute_pipeline;
    PFN_xaku_cmd_set_raster_pipeline                cmd_set_raster_pipeline;
    PFN_xaku_cmd_set_ray_tracing_pipeline           cmd_set_ray_tracing_pipeline;
    PFN_xaku_cmd_set_viewport                       cmd_set_viewport;
    PFN_xaku_cmd_set_scissor                        cmd_set_scissor;
    PFN_xaku_cmd_set_depth_bias                     cmd_set_depth_bias;
    PFN_xaku_cmd_set_index_buffer                   cmd_set_index_buffer;
    PFN_xaku_cmd_begin_rendering                    cmd_begin_rendering;
    PFN_xaku_cmd_end_rendering                      cmd_end_rendering;
    PFN_xaku_cmd_dispatch                           cmd_dispatch;
    PFN_xaku_cmd_dispatch_indirect                  cmd_dispatch_indirect;
    PFN_xaku_cmd_draw                               cmd_draw;
    PFN_xaku_cmd_draw_indexed                       cmd_draw_indexed;
    PFN_xaku_cmd_draw_indirect                      cmd_draw_indirect;
    PFN_xaku_cmd_draw_indirect_count                cmd_draw_indirect_count;
    PFN_xaku_cmd_mesh_tasks                         cmd_mesh_tasks;
    PFN_xaku_cmd_mesh_tasks_indirect                cmd_mesh_tasks_indirect;
    PFN_xaku_cmd_mesh_tasks_indirect_count          cmd_mesh_tasks_indirect_count;
    PFN_xaku_cmd_trace_rays                         cmd_trace_rays;
    PFN_xaku_cmd_trace_rays_indirect                cmd_trace_rays_indirect;
    PFN_xaku_cmd_write_timestamps                   cmd_write_timestamps;
    PFN_xaku_cmd_reset_timestamps                   cmd_reset_timestamps;
    PFN_xaku_cmd_begin_label                        cmd_begin_label;
    PFN_xaku_cmd_end_label                          cmd_end_label;
    PFN_xaku_compile_command_list                   compile_command_list;
};

/** Implies different strategies for the rendering engine. */
enum xaku_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    xaku_strategy_auto = 0,
    /** Rendering is done on a single primary device, no mGPU scheduling overhead. */
    xaku_strategy_optimal_primary,
    /** TODO Rendering on the discrete GPU is supported by the less-powerful integrated GPU.
     *  The rendering work is distributed such that the integrated GPU may not be a bottleneck. */
    xaku_strategy_optimal_discrete_integrated,
    /** TODO Multi-device rendering, explicit primary device and multiple secondary devices. 
     *  Sort first redistributes primitives early in the graphics pipeline, during geometry 
     *  processing. This is achieved by dividing the screenspace such that each GPU renders 
     *  it's own region, and the final image is composed from the contributing regions. */
    xaku_strategy_mgpu_sort_first,
    /** TODO Multi-device rendering, explicit primary device and multiple secondary devices.
     *  Sort last is defined as deferring primitive redistribution until the end of the graphics 
     *  pipeline. It divides primitives such that each GPU renders it's own portion of mesh data 
     *  into the framebuffer. Then these are composed into the final image. */
    xaku_strategy_mgpu_sort_last,
};

/** An engine structure for GPU renderering. */
struct xaku {
    enum xaku_strategy                          strategy;
    atomic_u32                                  flags;
    union xaku_encore_view                      backend;

    lake_dynamic_array(union xaku_device_view)  devices;
};

/** Returns previous reference count of the encore:
 *  - 0 means invalid backend.
 *  - 1 means the backend may be safely destroyed. 
 *  - >1 means that another system holds onto the backend. */
LAKEAPI u32 LAKECALL
xaku_fini(struct xaku *xaku, struct riven_work *out_zero_refcnt);

#ifdef __cplusplus
}
#endif /* __cplusplus */
