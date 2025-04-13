#pragma once

#include <amwe/riven.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define XAKU_MAX_PUSH_CONSTANT_WORD_SIZE (32u)
#define XAKU_MAX_PUSH_CONSTANT_BYTE_SIZE (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE * 4)
#define XAKU_PIPELINE_LAYOUT_COUNT       (XAKU_MAX_PUSH_CONSTANT_WORD_SIZE + 1)

#ifndef XAKU_ENABLE_GPU_PROFILER
    #ifndef LAKE_NDEBUG
        #define XAKU_ENABLE_GPU_PROFILER 1
    #else
        #define XAKU_ENABLE_GPU_PROFILER 0
    #endif /* LAKE_NDEBUG */
#endif /* XAKU_ENABLE_GPU_PROFILER */

/* opaque handles, implemented by the backend */
struct xaku_device;
struct xaku_command_recorder;
struct xaku_executable_command_list;
struct xaku_raster_pipeline;
struct xaku_compute_pipeline;
struct xaku_ray_tracing_pipeline;
struct xaku_event;
struct xaku_query_pool;
struct xaku_swapchain;
struct xaku_memory_block;
struct xaku_encore;

#ifdef XAKU_D3D12
LAKEAPI FN_RIVEN_ENCORE(xaku, d3d12);
#endif /* XAKU_D3D12 */
#ifdef XAKU_METAL
LAKEAPI FN_RIVEN_ENCORE(xaku, metal);
#endif /* XAKU_METAL */
#ifdef XAKU_WEBGPU
LAKEAPI FN_RIVEN_ENCORE(xaku, webgpu);
#endif /* XAKU_WEBGPU */
#ifdef XAKU_VULKAN
LAKEAPI FN_RIVEN_ENCORE(xaku, vulkan);
#endif /* XAKU_VULKAN */

/* A stub rendering backend, used for testing. */
LAKEAPI FN_RIVEN_ENCORE(xaku, null);

/** Returns an array of native encores, with a predefined priority. */
LAKEAPI lake_nonnull(2) const PFN_riven_encore *LAKECALL 
xaku_native_encores(bool null_fallback, u32 *out_encore_count);

enum xaku_device_type {
    xaku_device_type_other = 0,
    xaku_device_type_integrated_gpu,
    xaku_device_type_discrete_gpu,
    xaku_device_type_virtual_gpu,
    xaku_device_type_cpu,
};

struct xaku_device_limits {
    u32 max_image_dimension1d;
    u32 max_image_dimension2d;
    u32 max_image_dimension3d;
    u32 max_image_dimension_cube;
    u32 max_image_array_layers;
    u32 max_texel_buffer_elements;
    u32 max_uniform_buffer_range;
    u32 max_storage_buffer_range;
    u32 max_push_constants_size;
    u32 max_memory_allocation_count;
    u32 max_sampler_allocation_count;
    u64 buffer_image_granularity;
    u64 sparse_address_space_size;
    u32 max_bound_descriptor_sets;
    u32 max_per_stage_descriptor_samplers;
    u32 max_per_stage_descriptor_uniform_buffers;
    u32 max_per_stage_descriptor_storage_buffers;
    u32 max_per_stage_descriptor_sampled_images;
    u32 max_per_stage_descriptor_storage_images;
    u32 max_per_stage_descriptor_input_attachments;
    u32 max_per_stage_resources;
    u32 max_descriptor_set_samplers;
    u32 max_descriptor_set_uniform_buffers;
    u32 max_descriptor_set_uniform_buffers_dynamic;
    u32 max_descriptor_set_storage_buffers;
    u32 max_descriptor_set_storage_buffers_dynamic;
    u32 max_descriptor_set_sampled_images;
    u32 max_descriptor_set_storage_images;
    u32 max_descriptor_set_input_attachments;
    u32 max_vertex_input_attributes;
    u32 max_vertex_input_bindings;
    u32 max_vertex_input_attribute_offset;
    u32 max_vertex_input_binding_stride;
    u32 max_vertex_output_components;
    u32 max_tessellation_generation_level;
    u32 max_tessellation_patch_size;
    u32 max_tessellation_control_per_vertex_input_components;
    u32 max_tessellation_control_per_vertex_output_components;
    u32 max_tessellation_control_per_patch_output_components;
    u32 max_tessellation_control_total_output_components;
    u32 max_tessellation_evaluation_input_components;
    u32 max_tessellation_evaluation_output_components;
    u32 max_geometry_shader_invocations;
    u32 max_geometry_input_components;
    u32 max_geometry_output_components;
    u32 max_geometry_output_vertices;
    u32 max_geometry_total_output_components;
    u32 max_fragment_input_components;
    u32 max_fragment_output_attachments;
    u32 max_fragment_dual_src_attachments;
    u32 max_fragment_combined_output_resources;
    u32 max_compute_shared_memory_size;
    u32 max_compute_work_group_count[3];
    u32 max_compute_work_group_invocations;
    u32 max_compute_work_group_size[3];
    u32 sub_pixel_precision_bits;
    u32 sub_texel_precision_bits;
    u32 mipmap_precision_bits;
    u32 max_draw_indexed_index_value;
    u32 max_draw_indirect_count;
    f32 max_sampler_lod_bias;
    f32 max_sampler_anisotropy;
    u32 max_viewports;
    u32 max_viewport_dimensions[2];
    f32 viewport_bounds_range[2];
    u32 viewport_sub_pixel_bits;
    usize min_memory_map_alignment;
    u64 min_texel_buffer_offset_alignment;
    u64 min_uniform_buffer_offset_alignment;
    u64 min_storage_buffer_offset_alignment;
    s32 min_texel_offset;
    u32 max_texel_offset;
    s32 min_texel_gather_offset;
    u32 max_texel_gather_offset;
    f32 min_interpolation_offset;
    f32 max_interpolation_offset;
    u32 sub_pixel_interpolation_offset_bits;
    u32 max_framebuffer_width;
    u32 max_framebuffer_height;
    u32 max_framebuffer_layers;
    u32 framebuffer_color_sample_counts;
    u32 framebuffer_depth_sample_counts;
    u32 framebuffer_stencil_sample_counts;
    u32 framebuffer_no_attachments_sample_counts;
    u32 max_color_attachments;
    u32 sampled_image_color_sample_counts;
    u32 sampled_image_integer_sample_counts;
    u32 sampled_image_depth_sample_counts;
    u32 sampled_image_stencil_sample_counts;
    u32 storage_image_sample_counts;
    u32 max_sample_mask_words;
    u32 timestamp_compute_and_graphics;
    f32 timestamp_period;
    u32 max_clip_distances;
    u32 max_cull_distances;
    u32 max_combined_clip_and_cull_distances;
    u32 discrete_queue_priorities;
    f32 point_size_range[2];
    f32 line_width_range[2];
    f32 point_size_granularity;
    f32 line_width_granularity;
    u32 strict_lines;
    u32 standard_sample_locations;
    u64 optimal_buffer_copy_offset_alignment;
    u64 optimal_buffer_copy_row_pitch_alignment;
    u64 non_coherent_atom_size;
};

#define ARGS_XAKU_DEVICE_CREATE(ENCORE) \
    struct xaku_encore *ENCORE
typedef struct xaku_device *(LAKECALL *PFN_xaku_device_create)(ARGS_XAKU_DEVICE_CREATE(encore));
#define FN_XAKU_DEVICE_CREATE(ENCORE) \
    struct xaku_device *LAKECALL _xaku_##ENCORE##_device_create(ARGS_XAKU_DEVICE_CREATE(ENCORE))

typedef void (LAKECALL *PFN_xaku_device_destroy)(struct xaku_device *device);
#define FN_XAKU_DEVICE_DESTROY(ENCORE) \
    void LAKECALL _xaku_##ENCORE##_device_destroy(struct xaku_device *device)

/** We represent GPU resources (buffers, textures, views, samplers and acceleration structures) as IDs.
 *  They are all accessed via a bindless API, and can represent the same resource on multiple devices. */
typedef u64 xaku_id;
#define XAKU_ID_INDEX_BITS      (20u)
#define XAKU_ID_INDEX_MASK      ((1ull << 20) - 1ull)
#define XAKU_ID_INDEX_OFFSET    (0u)
#define XAKU_ID_VERSION_BITS    (44u)
#define XAKU_ID_VERSION_MASK    ((1ull << 44) - 1ull)
#define XAKU_ID_VERSION_OFFSET  (20u)
typedef xaku_id xaku_buffer_id;
typedef xaku_id xaku_texture_id;
typedef xaku_id xaku_texture_view_id;
typedef xaku_id xaku_sampler_id;
typedef xaku_id xaku_tlas_id;
typedef xaku_id xaku_blas_id;

/** A public interface of the rendering engine, implemented by a backend 'xaku_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct xaku_interface {
    struct riven_interface_header           header;

    PFN_xaku_device_create                  device_create;
    PFN_xaku_device_destroy                 device_destroy;
};

/** A view into the backend. */
union xaku_encore_view {
    struct riven_interface_header          *header;
    struct xaku_interface                  *interface;
    struct xaku_encore                     *encore;
};

/** A view into a rendering device. */
union xaku_device_view {
    struct xaku_device_header              *header;
    struct xaku_device                     *data;
};

/** Implies different strategies for the rendering engine. */
enum xaku_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    xaku_strategy_auto = 0,
    /** Rendering is done on a single device, with only one rendering backend at a time. */
    xaku_strategy_optimal = 0,
    /** Multi-GPU setting with one encore, using a primary device 
     *  and multiple secondary devices. Implies that the system is or will be 
     *  running on more than one rendering device (from one or more backends). */
    xaku_strategy_optimal_mgpu,
};

#ifndef XAKU_MAX_ENCORE_COUNT
    #define XAKU_MAX_ENCORE_COUNT 1
#endif /* XAKU_MAX_ENCORE_COUNT */

/** An engine structure for GPU renderering. */
struct xaku {
    enum xaku_strategy                      strategy;
    atomic_u32                              flags;
    union xaku_encore_view                  backends[XAKU_MAX_ENCORE_COUNT];

    union xaku_device_view                 *devices;
    u32                                     device_count;
    u32                                     device_capacity;
};
