#pragma once

/** @file moon/moon.h
 *  @brief Exposes an interface of the rendering backend.
 *
 *  TODO docs
 */
#include "moon_adapter.h"
#include "moon_memory_heap.h"
#include "moon_gpu_resources.h"
#include "moon_pipelines.h"
#include "moon_swapchain.h"
#include "moon_timeline.h"
#include "moon_device.h"

/** Interface of the rendering backend. */
typedef struct {
    lake_interface_header                           header;

    PFN_moon_list_device_details                    list_device_details;
    PFN_moon_device_assembly                        device_assembly;
    PFN_moon_device_zero_refcnt                     device_zero_refcnt;
    PFN_moon_device_queue_count                     device_queue_count;
    PFN_moon_device_queue_wait_idle                 device_queue_wait_idle;
    PFN_moon_device_wait_idle                       device_wait_idle;
    PFN_moon_device_submit_commands                 device_submit_commands;
    PFN_moon_device_present_frames                  device_present_frames;
    PFN_moon_device_destroy_deferred                device_destroy_deferred;
    PFN_moon_device_memory_buffer_requirements      device_memory_buffer_requirements;
    PFN_moon_device_memory_texture_requirements     device_memory_texture_requirements;
    PFN_moon_device_memory_report                   device_memory_report;
    /* TODO commands */

    PFN_moon_memory_heap_assembly                   memory_heap_assembly;
    PFN_moon_memory_heap_zero_refcnt                memory_heap_zero_refcnt;
    PFN_moon_timeline_query_pool_assembly           timeline_query_pool_assembly;
    PFN_moon_timeline_query_pool_zero_refcnt        timeline_query_pool_zero_refcnt;
    PFN_moon_timeline_semaphore_assembly            timeline_semaphore_assembly;
    PFN_moon_timeline_semaphore_zero_refcnt         timeline_semaphore_zero_refcnt;
    PFN_moon_binary_semaphore_assembly              binary_semaphore_assembly;
    PFN_moon_binary_semaphore_zero_refcnt           binary_semaphore_zero_refcnt;

    PFN_moon_compute_pipeline_assembly              compute_pipeline_assembly;
    PFN_moon_compute_pipeline_zero_refcnt           compute_pipeline_zero_refcnt;
    PFN_moon_raster_pipeline_assembly               raster_pipeline_assembly;
    PFN_moon_raster_pipeline_zero_refcnt            raster_pipeline_zero_refcnt;
    PFN_moon_ray_tracing_pipeline_assembly          ray_tracing_pipeline_assembly;
    PFN_moon_ray_tracing_pipeline_zero_refcnt       ray_tracing_pipeline_zero_refcnt;
    PFN_moon_work_graph_pipeline_assembly           work_graph_pipeline_assembly;
    PFN_moon_work_graph_pipeline_zero_refcnt        work_graph_pipeline_zero_refcnt;

    PFN_moon_swapchain_assembly                     swapchain_assembly;
    PFN_moon_swapchain_zero_refcnt                  swapchain_zero_refcnt;
    PFN_moon_swapchain_wait_for_next_frame          swapchain_wait_for_next_frame;
    PFN_moon_swapchain_acquire_next_image           swapchain_acquire_next_image;
    PFN_moon_swapchain_current_acquire_semaphore    swapchain_current_acquire_semaphore;
    PFN_moon_swapchain_current_present_semaphore    swapchain_current_present_semaphore;
    PFN_moon_swapchain_current_cpu_timeline_value   swapchain_current_cpu_timeline_value;
    PFN_moon_swapchain_current_timeline_pair        swapchain_current_timeline_pair;
    PFN_moon_swapchain_gpu_timeline_semaphore       swapchain_gpu_timeline_semaphore;
    PFN_moon_swapchain_set_present_mode             swapchain_set_present_mode;
    PFN_moon_swapchain_resize                       swapchain_resize;

    PFN_moon_create_buffer                          create_buffer;
    PFN_moon_create_buffer_from_memory_heap         create_buffer_from_memory_heap;
    PFN_moon_create_texture                         create_texture;
    PFN_moon_create_texture_from_memory_heap        create_texture_from_memory_heap;
    PFN_moon_create_texture_view                    create_texture_view;
    PFN_moon_create_sampler                         create_sampler;
    PFN_moon_create_tlas                            create_tlas;
    PFN_moon_create_tlas_from_buffer                create_tlas_from_buffer;
    PFN_moon_create_blas                            create_blas;
    PFN_moon_create_blas_from_buffer                create_blas_from_buffer;

    PFN_moon_is_buffer_valid                        is_buffer_valid;
    PFN_moon_is_texture_valid                       is_texture_valid;
    PFN_moon_is_texture_view_valid                  is_texture_view_valid;
    PFN_moon_is_sampler_valid                       is_sampler_valid;
    PFN_moon_is_tlas_valid                          is_tlas_valid;
    PFN_moon_is_blas_valid                          is_blas_valid;

    PFN_moon_buffer_host_address                    buffer_host_address;
    PFN_moon_buffer_device_address                  buffer_device_address;
    PFN_moon_tlas_device_address                    tlas_device_address;
    PFN_moon_blas_device_address                    blas_device_address;

    PFN_moon_destroy_buffer                         destroy_buffer;
    PFN_moon_destroy_texture                        destroy_texture;
    PFN_moon_destroy_texture_view                   destroy_texture_view;
    PFN_moon_destroy_sampler                        destroy_sampler;
    PFN_moon_destroy_tlas                           destroy_tlas;
    PFN_moon_destroy_blas                           destroy_blas;
} moon_interface;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Performs a quick check to validate an interface implementation. */
LAKEAPI FN_LAKE_INTERFACE_VALIDATION(moon_interface_validation, moon_interface);

#ifdef __cplusplus
}
#endif /* __cplusplus */
