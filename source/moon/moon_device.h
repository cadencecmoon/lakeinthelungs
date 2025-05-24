#pragma once

/** @file moon/moon_device.h
 *  @brief A logical rendering device.
 *
 *  Following interface procedures are defined:
 *      PFN_moon_list_device_details
 *      PFN_moon_device_assembly
 *      PFN_moon_device_zero_refcnt
 *      PFN_moon_device_queue_count
 *      PFN_moon_device_queue_wait_idle
 *      PFN_moon_device_wait_idle
 *      PFN_moon_device_submit_commands
 *      PFN_moon_device_present_frames
 *      PFN_moon_device_destroy_deferred
 *      PFN_moon_device_memory_buffer_requirements
 *      PFN_moon_device_memory_texture_requirements
 *      PFN_moon_device_memory_report
 */
#include "moon_adapter.h"
#include "moon_gpu_resources.h"
#include "moon_pipelines.h"
#include "moon_timeline.h"

typedef enum : s16 {
    moon_device_type_other = 0,
    moon_device_type_integrated_gpu,
    moon_device_type_discrete_gpu,
    moon_device_type_virtual_gpu,
    moon_device_type_cpu,
} moon_device_type;

typedef struct {
    u32 max_texture_dimension_1d;
    u32 max_texture_dimension_2d;
    u32 max_texture_dimension_3d;
    u32 max_texture_array_layers;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_root_constant_size;
    u32 max_sampled_textures;
    u32 max_storage_textures;
    u32 max_samplers;
    u32 max_uniform_buffers;
    u32 max_storage_buffers;
    u32 max_vertex_input_bindings;
    u32 max_vertex_input_attributes;
    u32 max_viewports;
    u32 max_viewport_dimensions[2];
    u32 max_scissor_dimensions[2];
    u32 max_compute_work_group_count[3];
    u32 max_compute_work_group_size[3];
    u32 max_compute_work_group_invocations;
    u32 uniform_buffer_offset_alignment;
    u32 storage_buffer_offset_alignment;
    u32 min_texel_buffer_offset_alignment;
    u32 min_uniform_buffer_offset_alignment;
    u32 max_color_attachments;
} moon_device_limits;

typedef struct {
    u32 shader_group_handle_size;
    u32 shader_group_handle_capture_replay_size;
    u32 shader_group_handle_alignment;
    u32 shader_group_base_alignment;
    u32 max_shader_group_stride;
    u32 max_ray_recursion_depth;
    u32 max_ray_dispatch_invocation_count;
    u32 max_ray_hit_attribute_size;
} moon_device_ray_tracing_pipeline_details;

typedef struct {
    u64 max_geometry_count;
    u64 max_instance_count;
    u64 max_primitive_count;
    u32 max_per_stage_descriptor_acceleration_structures;
    u32 max_per_stage_descriptor_update_after_bind_acceleration_structures;
    u32 max_descriptor_set_acceleration_structures;
    u32 max_descriptor_set_update_after_bind_acceleration_structures;
    u32 min_acceleration_structure_scratch_offset_alignment;
    bool has_invocation_reorder;
} moon_device_acceleration_structure_details;

typedef struct {
    u32 mesh_output_per_vertex_granularity;
    u32 mesh_output_per_primitive_granularity;
    u32 max_task_work_group_total_count;
    u32 max_task_work_group_count[3];
    u32 max_task_work_group_invocations;
    u32 max_task_work_group_size[3];
    u32 max_task_payload_size;
    u32 max_task_payload_and_shared_memory_size;
    u32 max_task_shared_memory_size;
    u32 max_mesh_work_group_total_count;
    u32 max_mesh_work_group_count[3];
    u32 max_mesh_work_group_invocations;
    u32 max_mesh_work_group_size[3];
    u32 max_mesh_payload_and_shared_memory_size;
    u32 max_mesh_payload_and_output_memory_size;
    u32 max_mesh_shared_memory_size;
    u32 max_mesh_output_memory_size;
    u32 max_mesh_output_components;
    u32 max_mesh_output_vertices;
    u32 max_mesh_output_primitives;
    u32 max_mesh_output_layers;
    u32 max_mesh_multiview_view_count;
    u32 max_preferred_task_work_group_invocations;
    u32 max_preferred_mesh_work_group_invocations;
    bool prefers_local_invocation_vertex_output;
    bool prefers_local_invocation_primitive_output;
    bool prefers_compact_vertex_output;
    bool prefers_compact_primitive_output;
} moon_device_mesh_shader_details;

typedef struct {
    u32 work_graph_dispatch_address_alignment;
    u32 max_work_graph_depth;
    u32 max_work_graph_shader_output_nodes;
    u32 max_work_graph_shader_payload_size;
    u32 max_work_graph_shader_payload_count;
    u32 max_work_graph_workgroup_count[3];
    u32 max_work_graph_workgroups;
} moon_device_work_graph_details;

/** These features are required, a device is discarded if incompatibilities are unresolved. */
typedef u32 moon_missing_required_features;
typedef enum : moon_missing_required_features {
    moon_missing_required_feature_none = 0u,
    moon_missing_required_feature_descriptor_indexing                   = (1u << 0), /* update after bind, runtime descriptor array */
    moon_missing_required_feature_buffer_device_address                 = (1u << 1), /* capture replay, multi device */
    moon_missing_required_feature_multi_draw_indirect                   = (1u << 2),
    moon_missing_required_feature_tessellation_shader                   = (1u << 3),
    moon_missing_required_feature_depth_clamp                           = (1u << 4),
    moon_missing_required_feature_sampler_anisotropy                    = (1u << 5),
    moon_missing_required_feature_framebuffer_local_dependencies        = (1u << 6), /* dynamic rendering, local read */
    moon_missing_required_feature_fragment_stores_and_atomics           = (1u << 7),
    moon_missing_required_feature_timeline_semaphore                    = (1u << 8),
    moon_missing_required_feature_texture_cube_array                    = (1u << 9),
    moon_missing_required_feature_shader_storage_texture                = (1u << 10), /* multisample, read/write without format */
    moon_missing_required_feature_shader_int64                          = (1u << 11),
    moon_missing_required_feature_fill_mode_wireframe                   = (1u << 12),
    moon_missing_required_feature_resolve_host_query_data               = (1u << 13),
    moon_missing_required_feature_subgroup_size_control                 = (1u << 14),
    moon_missing_required_feature_scalar_block_layout                   = (1u << 15),
    moon_missing_required_feature_independent_blend                     = (1u << 16),
    moon_missing_required_feature_variable_pointers                     = (1u << 17),
} moon_missing_required_feature_bits;

/** These features are always enabled if the device supports them. */
typedef u32 moon_implicit_features;
typedef enum : moon_implicit_features {
    moon_implicit_feature_none = 0u,
    moon_implicit_feature_mesh_shader                                   = (1u << 0),
    moon_implicit_feature_basic_ray_tracing                             = (1u << 1),
    moon_implicit_feature_ray_tracing_pipeline                          = (1u << 2),
    moon_implicit_feature_ray_tracing_invocation_reorder                = (1u << 3),
    moon_implicit_feature_ray_tracing_position_fetch                    = (1u << 4),
    moon_implicit_feature_conservative_rasterization                    = (1u << 5),
    moon_implicit_feature_work_graph                                    = (1u << 6),
    moon_implicit_feature_work_graph_mesh_tasks                         = (1u << 7),
    moon_implicit_feature_image_atomic64                                = (1u << 8),
    moon_implicit_feature_shader_atomic_float                           = (1u << 9),
    moon_implicit_feature_shader_atomic_int64                           = (1u << 10),
    moon_implicit_feature_shader_float16                                = (1u << 11),
    moon_implicit_feature_shader_int16                                  = (1u << 12),
    moon_implicit_feature_shader_int8                                   = (1u << 13),
    moon_implicit_feature_dynamic_state                                 = (1u << 14),
    moon_implicit_feature_sparse_binding                                = (1u << 15),
    moon_implicit_feature_swapchain                                     = (1u << 16),
} moon_implicit_feature_bits;

/** These features must be enabled explicitly by the user. */
typedef u32 moon_explicit_features;
typedef enum : moon_explicit_features {
    moon_explicit_feature_none = 0u,
    moon_explicit_feature_buffer_device_address_capture_replay          = (1u << 0),
    moon_explicit_feature_acceleration_structure_capture_replay         = (1u << 1),
    moon_explicit_feature_vulkan_memory_model                           = (1u << 2),
    moon_explicit_feature_robust_access                                 = (1u << 3),
    moon_explicit_feature_video_decode_queue                            = (1u << 4),
    moon_explicit_feature_video_encode_queue                            = (1u << 5),
    moon_explicit_feature_multiview_vr                                  = (1u << 6),
} moon_explicit_feature_bits;

typedef struct {
    u32                                         api_version;
    u32                                         driver_version;
    u16                                         vendor_id;
    u16                                         device_id;
    moon_device_type                            device_type;
    char                                        device_name[256];
    char                                        pipeline_cache_uuid[16];
    u16                                         heap_type_count;
    moon_memory_heap_type                       heap_types[MOON_MAX_MEMORY_HEAP_TYPES];
    moon_device_work_graph_details              work_graph_details;
    moon_device_mesh_shader_details             mesh_shader_details;
    moon_device_acceleration_structure_details  acceleration_structure_details;
    moon_device_ray_tracing_pipeline_details    ray_tracing_pipeline_details;
    moon_device_limits                          limits;
    u16                                         compute_queue_count;
    u16                                         transfer_queue_count;
    moon_missing_required_features              missing_required_features;
    moon_implicit_features                      implicit_features;
    moon_explicit_features                      explicit_features;
    u32                                         total_score;
} moon_device_details;

typedef struct {
    /** Index into a list of device details returned from `PFN_moon_list_device_details`. */
    u32                                                 device_index;
    /** Explicit features of set bits will be enabled, otherwise they are ignored. */
    moon_explicit_features                              explicit_features;
    /** User's device limits. A device may be discarded if it doesn't meet these requirements. */
    u32                                                 max_allowed_textures;
    u32                                                 max_allowed_buffers;
    u32                                                 max_allowed_samplers;
    u32                                                 max_allowed_acceleration_structures;
} moon_device_assembly;
static constexpr moon_device_assembly MOON_DEVICE_ASSEMBLY_INIT = {
    .device_index = ~0u,
    .explicit_features = moon_explicit_feature_buffer_device_address_capture_replay,
    .max_allowed_textures = 10000,
    .max_allowed_buffers = 10000,
    .max_allowed_samplers = 400,
    .max_allowed_acceleration_structures = 10000,
};

/** List properties of available physical devices. The `out_details` argument may be nullptr to query 
 *  the details count and allocate an array of pointers to grab the device details with a second call. */
typedef void (LAKECALL *PFN_moon_list_device_details)(moon_adapter adapter, u32 *out_device_count, const moon_device_details **out_details);
#define FN_MOON_LIST_DEVICE_DETAILS(backend) \
    void LAKECALL _moon_##backend##_list_device_details(moon_adapter adapter, u32 *out_device_count, const moon_device_details **out_details)

/** Assemble a rendering device from given details. */
PFN_MOON_ASSEMBLY(device, device_assembly, adapter);
#define FN_MOON_DEVICE_ASSEMBLY(backend) \
    FN_MOON_ASSEMBLY(backend, device, device_assembly, adapter)

/** Destroy a rendering device. */
PFN_MOON_ZERO_REFCNT(device);
#define FN_MOON_DEVICE_ZERO_REFCNT(backend) \
    FN_MOON_ZERO_REFCNT(backend, device)

/** Retrieve the number of queues available for a given queue type. Writes 0 if the command queue is unavailable. */
typedef u32 (LAKECALL *PFN_moon_device_queue_count)(moon_device device, moon_queue_type queue_type);
#define FN_MOON_DEVICE_QUEUE_COUNT(backend) \
    u32 LAKECALL _moon_##backend##_device_queue_count(moon_device device, moon_queue_type queue_type)

/** Wait until GPU work on a given command queue is done. */
typedef void (LAKECALL *PFN_moon_device_queue_wait_idle)(moon_device device, moon_queue queue);
#define FN_MOON_DEVICE_QUEUE_WAIT_IDLE(backend) \
    void LAKECALL _moon_##backend##_device_queue_wait_idle(moon_device device, moon_queue queue)

/** Wait until all GPU work is done. */
PFN_LAKE_WORK(PFN_moon_device_wait_idle, moon_device device);
#define FN_MOON_DEVICE_WAIT_IDLE(backend) \
    FN_LAKE_WORK(_moon_##backend##_device_wait_idle, moon_device device)

/** Describes a submit. */
typedef struct {
    moon_queue                      queue;
    moon_pipeline_stages            wait_stages;
    moon_staged_command_list const *staged_command_lists;
    usize                           staged_command_list_count;
    moon_binary_semaphore const    *wait_binary_semaphores;
    usize                           wait_binary_semaphore_count;
    moon_binary_semaphore const    *signal_binary_semaphores;
    usize                           signal_binary_semaphore_count;
    moon_timeline_pair const       *wait_timeline_semaphores;
    usize                           wait_timeline_semaphore_count;
    moon_timeline_pair const       *signal_timeline_semaphores;
    usize                           signal_timeline_semaphore_count;
} moon_device_submit;
static constexpr moon_device_submit MOON_DEVICE_SUBMIT_INIT = {0};

/** Submit work into a command queue. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_submit_commands)(moon_device device, moon_device_submit const *submit);
#define FN_MOON_DEVICE_SUBMIT_COMMANDS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_submit_commands(moon_device device, moon_device_submit const *submit)

/** Describes a present. */
typedef struct {
    moon_binary_semaphore const    *wait_binary_semaphores;
    moon_swapchain const           *swapchains;
    u32                             wait_binary_semaphore_count;
    u16                             swapchain_count;
    moon_queue                      queue;
} moon_device_present;
static constexpr moon_device_present MOON_DEVICE_PRESENT_INIT = {0};

/** Present swapchain images. Used by a primary device that supports presentation to a window surface. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_present_frames)(moon_device device, moon_device_present const *present);
#define FN_MOON_DEVICE_PRESENT_FRAME(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_present_frames(moon_device device, moon_device_present const *present)

/** Destroys all resources that were zombified and are ready to be destroyed. */
PFN_LAKE_WORK(PFN_moon_device_destroy_deferred, moon_device device);
#define FN_MOON_DEVICE_DESTROY_DEFERRED(backend) \
    FN_LAKE_WORK(_moon_##backend##_device_destroy_deferred, moon_device device)

/** Get GPU memory requirements for a buffer. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_memory_buffer_requirements)(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_MEMORY_BUFFER_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_memory_buffer_requirements(moon_device device, moon_buffer_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Get GPU memory requirements for a texture. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_memory_texture_requirements)(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements);
#define FN_MOON_DEVICE_MEMORY_TEXTURE_REQUIREMENTS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_memory_texture_requirements(moon_device device, moon_texture_assembly const *assembly, moon_memory_requirements *out_requirements)

/** Writes statistics of all device memory in use. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_device_memory_report)(moon_device device, moon_memory_report *out_report);
#define FN_MOON_DEVICE_MEMORY_REPORT(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_device_memory_report(moon_device device, moon_memory_report *out_report)
