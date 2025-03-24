#pragma once

#include <amw/bedrock.h>
#include <amw/log.h>
#include <amw/riven.h>

/* limits */
#define REZNOR_MAX_DESCRIPTOR_SET_SLOT_COUNT 4u
#define REZNOR_MAX_COLOR_TARGET_COUNT        6u
#define REZNOR_MAX_SWAPCHAIN_IMAGES          8u

#ifndef REZNOR_MAX_SWAPCHAINS
#define REZNOR_MAX_SWAPCHAINS 16u
#endif

#ifndef REZNOR_MAX_FRAMES_IN_FLIGHT
#define REZNOR_MAX_FRAMES_IN_FLIGHT 4
#endif

#ifndef NDEBUG
#define _REZNOR_DEBUG_OPTS 1
#else
#define _REZNOR_DEBUG_OPTS 0
#endif

#ifndef REZNOR_ENABLE_DEBUG_LABELS
#define REZNOR_ENABLE_DEBUG_LABELS _REZNOR_DEBUG_OPTS
#endif
#ifndef REZNOR_ENABLE_GPU_PROFILER
#define REZNOR_ENABLE_GPU_PROFILER _REZNOR_DEBUG_OPTS
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct reznor_device;
struct reznor_device_frame;
struct reznor_device_memory;
struct reznor_command_buffer;
struct reznor_buffer;
struct reznor_texture;
struct reznor_sampler;
struct reznor_descriptor_set_layout;
struct reznor_descriptor_set;
struct reznor_pipeline_layout;
struct reznor_graphics_pipeline;
struct reznor_compute_pipeline;
struct reznor_raytracing_pipeline;
struct reznor_shader_binding_table;
struct reznor_top_level;
struct reznor_bottom_level;
struct reznor_query_pool;
struct reznor_swapchain;
struct reznor;

struct reznor_encore {
    struct riven_encore_header header;
    b32 debug_utils;
};

#ifdef REZNOR_D3D12
/** The entry point to a Windows Direct 3D 12 Ultimate rendering backend. */
AMWAPI RIVEN_ENCORE(reznor, d3d12);
#endif
#ifdef REZNOR_METAL
/** The entry point to an Apple Metal3 rendering backend. */
AMWAPI RIVEN_ENCORE(reznor, metal);
#endif
#ifdef REZNOR_WEBGPU
/** The entry point to a Vulkan rendering backend. */
AMWAPI RIVEN_ENCORE(reznor, webgpu);
#endif
#ifdef REZNOR_VULKAN
/** The entry point to a WebGPU rendering backend. */
AMWAPI RIVEN_ENCORE(reznor, vulkan);
#endif

/** The entry point to a stub rendering backend, available on all platforms.
 *  This is a valid implementation that provides a zeroed-out configuration and spec validation. */
AMWAPI RIVEN_ENCORE(reznor, null);

/** Executes native encores until one writes to the interface:
 *  - d3d12 - Windows platforms with DX12 drivers.
 *  - metal - Apple platforms with Metal 3 drivers (Apple Silicon).
 *  - vulkan - All platforms (except Emscripten) with Vulkan 1.2+ drivers.
 *  - webgpu - WebAssembly builds, or expose layers from native implementations I guess.
 *
 *  Available encores are tried in order:
 *      native option (d3d12 -> metal -> webgpu) -> vulkan
 *
 *  If all options fail, the stub interface may be written as a fallback.
 *  This may be disabled by defining REZNOR_DISABLE_NULL_FALLBACK.
 *
 *  Other possible rendering backends that could be created:
 *      AGC (PS5), GNM (PS4), NVN, serialized proxy, older APIs(OpenGL, OpenGL ES, DX11, etc.)? */
AMWAPI RIVEN_ENCORE(reznor, native);

enum reznor_device_features {
    reznor_device_feature_is_discrete              = (1u << 0),
    reznor_device_feature_swapchain_support        = (1u << 1),
    reznor_device_feature_sparse_binding           = (1u << 2),
    reznor_device_feature_raytracing_pipeline      = (1u << 3),
    reznor_device_feature_acceleration_structures  = (1u << 4),
    reznor_device_feature_mesh_shader              = (1u << 5),
    reznor_device_feature_tesselation_shader       = (1u << 6),
    reznor_device_feature_geometry_shader          = (1u << 7),
    reznor_device_feature_video_av1                = (1u << 8),
    reznor_device_feature_video_h264               = (1u << 9),
};

struct reznor_physical_device_info {
    struct str  name;
    u32         index;
    u32         score;
    u32         features;
};

#define ARGS_REZNOR_DEVICE_QUERY                                \
    struct reznor                          *reznor,             \
    u32                                    *out_device_count,   \
    struct reznor_physical_device_info     *out_devices
typedef s32 (AMWCALL *PFN_reznor_device_query)(ARGS_REZNOR_DEVICE_QUERY);
#define FN_REZNOR_DEVICE_QUERY(encore) \
    extern s32 AMWCALL _reznor_##encore##_device_query(ARGS_REZNOR_DEVICE_QUERY)

enum reznor_device_flag {
    reznor_device_flag_is_primary = (1u << 0),
};

struct reznor_device_header {
    struct reznor                          *reznor;
    at_u32                                  flags;
    u32                                     features;
    u32                                     thread_count;
    u32                                     frames_in_flight;
    struct reznor_device_frame             *frames[REZNOR_MAX_FRAMES_IN_FLIGHT];
};

struct reznor_device_config {
    struct reznor_physical_device_info      physical_info;
    u32                                     frames_in_flight;
    u32                                     thread_count;
    b32                                     debug_utils;
};

#define ARGS_REZNOR_DEVICE_CREATE                   \
    struct reznor                          *reznor, \
    const struct reznor_device_config      *config, \
    struct memory_requirements             *memory, \
    struct reznor_device                  **restrict out_device
typedef s32 (AMWCALL *PFN_reznor_device_create)(ARGS_REZNOR_DEVICE_CREATE);
#define FN_REZNOR_DEVICE_CREATE(encore) \
    extern s32 AMWCALL _reznor_##encore##_device_create(ARGS_REZNOR_DEVICE_CREATE)

typedef void (AMWCALL *PFN_reznor_device_destroy)(struct reznor_device *restrict device);
#define FN_REZNOR_DEVICE_DESTROY(encore) \
    extern void AMWCALL _reznor_##encore##_device_destroy(struct reznor_device *restrict device)

/** Queue types for scheduling command buffers. */
enum reznor_queue_type {
    reznor_queue_type_graphics         = (1u << 0),
    reznor_queue_type_async_compute    = (1u << 1),
    reznor_queue_type_async_transfer   = (1u << 2),
    reznor_queue_type_sparse_binding   = (1u << 3),
    reznor_queue_type_video_decode     = (1u << 4),
};

enum reznor_index_format {
    reznor_index_format_invalid = 0u,
    reznor_index_format_u16,
    reznor_index_format_u32,
};

enum reznor_primitive_type {
    reznor_primitive_type_triangle_list = 0,
    reznor_primitive_type_triangle_strip,
    reznor_primitive_type_line_list,
    reznor_primitive_type_patch_list,
    reznor_primitive_type_count,
};

enum reznor_blend_operation {
    reznor_blend_operation_none = 0u,
    reznor_blend_operation_add,
    reznor_blend_operation_subtract,
    reznor_blend_operation_rev_subtract,
    reznor_blend_operation_min,
    reznor_blend_operation_max,
};

enum reznor_blend_factor {
    reznor_blend_factor_zero = 0u,
    reznor_blend_factor_one,
    reznor_blend_factor_src_color,
    reznor_blend_factor_inv_src_color,
    reznor_blend_factor_src_alpha,
    reznor_blend_factor_inv_src_alpha,
    reznor_blend_factor_dest_alpha,
    reznor_blend_factor_inv_dest_alpha,
    reznor_blend_factor_dest_color,
    reznor_blend_factor_inv_dest_color,
    reznor_blend_factor_src_alpha_sat,
};

enum reznor_comparison_function {
    reznor_comparison_function_never = 0u,
    reznor_comparison_function_less,
    reznor_comparison_function_equal,
    reznor_comparison_function_less_equal,
    reznor_comparison_function_not_equal,
    reznor_comparison_function_greater,
    reznor_comparison_function_greate_equal,
    reznor_comparison_function_always,
};

enum reznor_fill_mode {
    reznor_fill_mode_wireframe = 0u,
    reznor_fill_mode_solid,
};

enum reznor_cull_mode {
    reznor_cull_mode_none = 0u,
    reznor_cull_mode_front,
    reznor_cull_mode_back,
};

enum reznor_winding_order {
    reznor_winding_order_counter_clockwise = 0u,
    reznor_winding_order_clockwise,
};

enum reznor_stencil_face {
    reznor_stencil_face_front = 0u,
    reznor_stencil_face_back,
};

enum reznor_stencil_operation {
    reznor_stencil_operation_keep = 0u,
    reznor_stencil_operation_zero,
    reznor_stencil_operation_replace,
    reznor_stencil_operation_increment,
    reznor_stencil_operation_decrement,
    reznor_stencil_operation_increment_wrap,
    reznor_stencil_operation_decrement_wrap,
    reznor_stencil_operation_invert,
};

enum reznor_color_write {
    reznor_color_write_red = 0u,
    reznor_color_write_green,
    reznor_color_write_blue,
    reznor_color_write_alpha,
};

enum reznor_dynamic_state_flag {
    reznor_dynamic_state_flag_scissor               = (1u << 0),
    reznor_dynamic_state_flag_viewport              = (1u << 1),
    reznor_dynamic_state_flag_stencil_reference     = (1u << 2),
    reznor_dynamic_state_flag_stencil_write_mask    = (1u << 3),
    reznor_dynamic_state_flag_stencil_compare_mask  = (1u << 4),
};

struct reznor_stencil_config {
    u8                                          test_mask;       /* 0xffu default */
    u8                                          write_mask;
    u8                                          test_enabled;
    u8                                          test_func;       /**< enum reznor_comparison_function, always */
    u8                                          op_fail;         /**< enum reznor_stencil_operation, keep */
    u8                                          op_depth_fail;   /**< enum reznor_stencil_operation, keep */
    u8                                          op_depth_pass;   /**< enum reznor_stencil_operation, keep */
};

/** inspired by https://github.com/Tobski/simple_vulkan_synchronization */
enum reznor_access {
    reznor_access_general = 0u,                                                             /**< Covers any access - useful for debugging, avoid for performance reasons. */
    /* read access */
    reznor_access_indirect_buffer,                                                          /**< Read as an indirect buffer for drawing or dispatch. */
    reznor_access_index_buffer,                                                             /**< Read as an index buffer for drawing. */
    reznor_access_vertex_buffer,                                                            /**< Read as a vertex buffer for drawing. */
    reznor_access_vertex_shader_read_uniform_buffer,                                        /**< Read as a uniform buffer in a vertex shader. */
    reznor_access_vertex_shader_read_sampled_image_or_uniform_texel_buffer,                 /**< Read as a sampled image/uniform texel buffer in a vertex shader. */
    reznor_access_vertex_shader_read_other,                                                 /**< Read as any other resource in a vertex shader. */
    reznor_access_tesselation_control_shader_read_uniform_buffer,                           /**< Read as a uniform buffer in a tesselation control shader. */
    reznor_access_tesselation_control_shader_read_sampled_image_or_uniform_texel_buffer,    /**< Read as a sampled image/uniform texel buffer in a tesselation control shader. */
    reznor_access_tesselation_control_shader_read_other,                                    /**< Read as any other resource in a tesselation control shader. */
    reznor_access_tesselation_evaluation_shader_read_uniform_buffer,                        /**< Read as a uniform buffer in a tesselation evaluation shader. */
    reznor_access_tesselation_evaluation_shader_read_sampled_image_or_uniform_texel_buffer, /**< Read as a sampled image/uniform texel buffer in a tesselation evaluation shader. */
    reznor_access_tesselation_evaluation_shader_read_other,                                 /**< Read as any other resource in a tesselation evaluation shader. */
    reznor_access_geometry_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a geometry shader. */
    reznor_access_geometry_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a geometry shader. */
    reznor_access_geometry_shader_read_other,                                               /**< Read as any other resource in a geometry shader. */
    reznor_access_task_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a task shader. */
    reznor_access_task_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a task shader. */
    reznor_access_task_shader_read_other,                                                   /**< Read as any other resource in a task shader. */
    reznor_access_mesh_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a mesh shader. */
    reznor_access_mesh_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a mesh shader. */
    reznor_access_mesh_shader_read_other,                                                   /**< Read as any other resource in a mesh shader. */
    reznor_access_transform_feedback_counter_read,                                          /**< Read as a transform feedback counter buffer. */
    reznor_access_fragment_density_map_read,                                                /**< Read as a fragment density map image. */
    reznor_access_shading_rate_read,                                                        /**< Read as a shading rate image. */
    reznor_access_fragment_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a fragment shader. */
    reznor_access_fragment_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a fragment shader. */
    reznor_access_fragment_shader_read_color_input_attachment,                              /**< Read as an input attachment with a color format in a fragment shader. */
    reznor_access_fragment_shader_read_depth_stencil_input_attachment,                      /**< Read as an input attachment with a depth/stencil format in a fragment shader. */
    reznor_access_fragment_shader_read_other,                                               /**< Read as any other resource in a fragment shader. */
    reznor_access_color_attachment_read,                                                    /**< Read by standard blending/logic operations or local read/subpass load operations. */
    reznor_access_color_attachment_advanced_blending,                                       /**< Read by advanced blending, standard blending, logic operations or local read/subpass load operations. */
    reznor_access_depth_stencil_attachment_read,                                            /**< Read by depth/stencil tests or local read/subpass load operations. */
    reznor_access_compute_shader_read_uniform_buffer,                                       /**< Read as a uniform buffer in a compute shader. */
    reznor_access_compute_shader_read_sampled_image_or_uniform_texel_buffer,                /**< Read as a sampled image/uniform texel buffer in a compute shader. */
    reznor_access_compute_shader_read_other,                                                /**< Read as any other resource in a compute shader. */
    reznor_access_any_shader_read_uniform_buffer,                                           /**< Read as a uniform buffer in any shader. */
    reznor_access_any_shader_read_uniform_buffer_or_vertex_buffer,                          /**< Read as a uniform buffer in any shader, or a vertex buffer. */
    reznor_access_any_shader_read_sampled_image_or_uniform_texel_buffer,                    /**< Read as a sample image/uniform texel buffer in any shader. */
    reznor_access_any_shader_read_other,                                                    /**< Read as any other resource (excluding attachments) in any shader. */
    reznor_access_transfer_read,                                                            /**< Read as the source of a transfer operation. */
    reznor_access_host_read,                                                                /**< Read on the host. */
    reznor_access_present,                                                                  /**< Read by presentation engine (i.e. vkQueuePresentKHR). */
    reznor_access_conditional_rendering_read,                                               /**< Read by conditional rendering. */
    reznor_access_raytracing_shader_acceleration_structure_read,                            /**< Read by a raytracing shader as an acceleration structure. */
    reznor_access_acceleration_structure_build_read,                                        /**< Read as an acceleration structure during a build. */
    reznor_access_end_read,                                                                 /**< Read access end. */
    /* write access */
    reznor_access_command_buffer_write,                                                     /**< Command buffer write operation. */
    reznor_access_vertex_shader_write,                                                      /**< Written as any resource in a vertex shader. */
    reznor_access_tesselation_control_shader_write,                                         /**< Written as any resource in a tesselation control shader. */
    reznor_access_tesselation_evaluation_shader_write,                                      /**< Written as any resource in a tesselation evaluation shader. */
    reznor_access_geometry_shader_write,                                                    /**< Written as any resource in a geometry shader. */
    reznor_access_task_shader_write,                                                        /**< Written as any resource in a task shader. */
    reznor_access_mesh_shader_write,                                                        /**< Written as any resource in a mesh shader. */
    reznor_access_transform_feedback_write,                                                 /**< Written as a transform feedback buffer. */
    reznor_access_transform_feedback_counter_write,                                         /**< Written as a transform feedback counter buffer. */
    reznor_access_fragment_shader_write,                                                    /**< Written as any resource in a fragment shader, */
    reznor_access_color_attachment_write,                                                   /**< Written as a color attachment during rendering, or via a subpass store op. */
    reznor_access_depth_stencil_attachment_write,                                           /**< Written as a depth/stencil attachment during rendering, or via a subpass store op. */
    reznor_access_depth_attachment_write_stencil_read_only,                                 /**< Written as a depth aspect of a depth/stencil attachment during rendering, whilst the stencil aspect is read-only. */
    reznor_access_stencil_attachment_write_depth_read_only,                                 /**< Written as a stencil aspect of a depth/stencil attachment during rendering, whilst the depth aspect is read-only. */
    reznor_access_compute_shader_write,                                                     /**< Written as any resource in a compute shader. */
    reznor_access_any_shader_write,                                                         /**< Written as any resource in any shader. */
    reznor_access_transfer_write,                                                           /**< Written as the destination of a transfer operation. */
    reznor_access_host_preinitialized,                                                      /**< Data pre-filled by host before device access starts. */
    reznor_access_host_write,                                                               /**< Written on the host. */
    reznor_access_acceleration_structure_build_write,                                       /**< Written as an acceleration structure during a build. */
    reznor_access_color_attachment_read_write,                                              /**< Read or written as a color attachment during rendering. */
    reznor_access_end_write,                                                                /**< Write access end, access types count. */
};

/** Types of GPU resources. */
enum reznor_resource_type {
    reznor_resource_invalid = 0u,
    reznor_resource_device_memory,
    reznor_resource_buffer,
    reznor_resource_texture,
    reznor_resource_sampler,
    reznor_resource_descriptor_set_layout,
    reznor_resource_descriptor_set,
    reznor_resource_pipeline_layout,
    reznor_resource_graphics_pipeline,
    reznor_resource_compute_pipeline,
    reznor_resource_raytracing_pipeline,
    reznor_resource_shader_binding_table,
    reznor_resource_bottom_level,
    reznor_resource_top_level,
    reznor_resource_query_pool,
    reznor_resource_swapchain,
    reznor_resource_count,
};
#define reznor_resource_type_index(__type) \
    (reznor_resource_type_##__type - 1u)

struct reznor_resource_header {
    struct reznor_device       *device; 
    enum reznor_resource_type   type;
    at_u32                      flags;
};

#define ARGS_REZNOR_RESOURCE_ASSEMBLY(_type)                \
    const struct reznor_device           *device,           \
    u32                                   _type##_count,    \
    const struct reznor_##_type##_config *config,           \
    struct memory_requirements           *memory,           \
    struct reznor_##_type               **restrict write
#define PFN_REZNOR_RESOURCE_ASSEMBLY(__type) \
    typedef s32 (AMWCALL *PFN_reznor_##__type##_assembly)(ARGS_REZNOR_RESOURCE_ASSEMBLY(__type));
#define FN_REZNOR_RESOURCE_ASSEMBLY(encore, __type) \
    extern s32 AMWCALL _reznor_##encore##_##__type##_assembly(ARGS_REZNOR_RESOURCE_ASSEMBLY(__type))

enum reznor_memory_priority {
    reznor_memory_priority_lowest = 0,
    reznor_memory_priority_low,
    reznor_memory_priority_normal,
    reznor_memory_priority_high,
    reznor_memory_priority_highest,
};

struct reznor_memory_requirements {
    usize                                       size;
    usize                                       alignment;
    s32                                         supported_memory_type_indices;
    b32                                         prefers_dedicated_allocation;
    b32                                         requires_dedicated_allocation;
};

enum reznor_device_memory_type {
    reznor_device_memory_type_gpu_local,
    reznor_device_memory_type_host_accessible,
    reznor_device_memory_type_host_cached,
    reznor_device_memory_type_host_coherent,
};

struct reznor_device_memory_range {
    const struct reznor_device_memory          *memory;
    usize                                       offset;
    usize                                       size;
};

struct reznor_device_memory_config {
    struct str                                  debug_name;
    usize                                       size;
    u32                                         memory_type_index;
    b32                                         map_memory;
    enum reznor_memory_priority                 priority;
    /* optional */
    const struct reznor_buffer                 *dedicated_buffer;
    const struct reznor_buffer                 *dedicated_texture;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(device_memory);

enum reznor_buffer_usage {
    reznor_buffer_usage_transfer_source         = (1u << 0),
    reznor_buffer_usage_transfer_target         = (1u << 1),
    reznor_buffer_usage_uniform                 = (1u << 2),
    reznor_buffer_usage_storage                 = (1u << 3),
    reznor_buffer_usage_index                   = (1u << 4),
    reznor_buffer_usage_vertex                  = (1u << 5),
    reznor_buffer_usage_indirect_argument       = (1u << 6), /* indirect commands */
    reznor_buffer_usage_raytrace_scratch        = (1u << 7),
    reznor_buffer_usage_bottom_level            = (1u << 8),
    reznor_buffer_usage_top_level               = (1u << 9),
    reznor_buffer_usage_shader_binding_table    = (1u << 10),
};

struct reznor_buffer_region {
    usize                                       src_offset;
    usize                                       dst_offset;
    usize                                       size;
};

struct reznor_buffer_range {
    const struct reznor_buffer                 *buffer;
    usize                                       offset;
    usize                                       size;
};

struct reznor_buffer_strided_range {
    const struct reznor_buffer                 *buffer;
    usize                                       offset;
    usize                                       stride;
    usize                                       size;
};

struct reznor_buffer_memory_binding {
    const struct reznor_buffer                 *buffer;
    struct reznor_device_memory_range           memory_range;
};

struct reznor_buffer_config {
    struct str  debug_name;
    u32         usage;      /**< enum reznor_buffer_usage */
};
PFN_REZNOR_RESOURCE_ASSEMBLY(buffer)

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
enum reznor_texture_format {
    reznor_texture_format_unknown = 0u,
    reznor_texture_format_r1_unorm,        /**< 1-bit no format */
    reznor_texture_format_a8_unorm,        /**< 8-bit alpha */
    /* depth formats */
    reznor_texture_format_d16_unorm,       /**< 16-bit unorm depth */
    reznor_texture_format_d16s8_unorm,     /**< 24-bit unorm depth/stencil */
    reznor_texture_format_d24s8_unorm,     /**< 32-bit unorm depth/stencil */
    reznor_texture_format_d32_float,       /**< 32-bit float depth */
    reznor_texture_format_d0s8_unorm,      /**< 8-bit unorm stencil */
    /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
    reznor_texture_format_r8_snorm,
    reznor_texture_format_r8_unorm,
    reznor_texture_format_r8_sint,
    reznor_texture_format_r8_uint,
    /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
    reznor_texture_format_r16_float,
    reznor_texture_format_r16_snorm,
    reznor_texture_format_r16_unorm,
    reznor_texture_format_r16_sint,
    reznor_texture_format_r16_uint,
    reznor_texture_format_r8g8_snorm,
    reznor_texture_format_r8g8_unorm,
    reznor_texture_format_r8g8_sint,
    reznor_texture_format_r8g8_uint,
    /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
    reznor_texture_format_r8g8b8_snorm,
    reznor_texture_format_r8g8b8_unorm,
    reznor_texture_format_r8g8b8_sint,
    reznor_texture_format_r8g8b8_uint,
    /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
    reznor_texture_format_r32_float,
    reznor_texture_format_r32_sint,
    reznor_texture_format_r32_uint,
    reznor_texture_format_r16g16_float,
    reznor_texture_format_r16g16_snorm,
    reznor_texture_format_r16g16_unorm,
    reznor_texture_format_r16g16_sint,
    reznor_texture_format_r16g16_uint,
    reznor_texture_format_r8g8b8a8_snorm,
    reznor_texture_format_r8g8b8a8_unorm,
    reznor_texture_format_r8g8b8a8_sint,
    reznor_texture_format_r8g8b8a8_uint,
    reznor_texture_format_b8g8r8a8_snorm,
    reznor_texture_format_b8g8r8a8_unorm,
    reznor_texture_format_b8g8r8a8_sint,
    reznor_texture_format_b8g8r8a8_uint,
    reznor_texture_format_r10g10b10a2,
    reznor_texture_format_b10g10r10a2,
    /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r16g16b16_float,
    reznor_texture_format_r16g16b16_snorm,
    reznor_texture_format_r16g16b16_unorm,
    reznor_texture_format_r16g16b16_sint,
    reznor_texture_format_r16g16b16_uint,
    /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r64_float,
    reznor_texture_format_r64_sint,
    reznor_texture_format_r64_uint,
    reznor_texture_format_r32g32_float,
    reznor_texture_format_r32g32_sint,
    reznor_texture_format_r32g32_uint,
    reznor_texture_format_r16g16b16a16_float,
    reznor_texture_format_r16g16b16a16_snorm,
    reznor_texture_format_r16g16b16a16_unorm,
    reznor_texture_format_r16g16b16a16_sint,
    reznor_texture_format_r16g16b16a16_uint,
    /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r32g32b32_float,
    reznor_texture_format_r32g32b32_sint,
    reznor_texture_format_r32g32b32_uint,
    /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r32g32b32a32_float,
    reznor_texture_format_r32g32b32a32_sint,
    reznor_texture_format_r32g32b32a32_uint,
    reznor_texture_format_r64g64_float,
    reznor_texture_format_r64g64_sint,
    reznor_texture_format_r64g64_uint,
    /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r64g64b64_float,
    reznor_texture_format_r64g64b64_sint,
    reznor_texture_format_r64g64b64_uint,
    /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
    reznor_texture_format_r64g64b64a64_float,
    reznor_texture_format_r64g64b64a64_sint,
    reznor_texture_format_r64g64b64a64_uint,
    /* compressed texture formats */
    reznor_texture_format_BC1,         /**< DXT1 r5g6b5a1 */
    reznor_texture_format_BC2,         /**< DXT3 r5g6b6a4 */
    reznor_texture_format_BC3,         /**< DXT5 r5g6b5a8 */
    reznor_texture_format_BC4,         /**< LATC1/ATI1 r8 */
    reznor_texture_format_BC5,         /**< LATC2/ATI2 r8g8 */
    reznor_texture_format_BC6H,        /**< BC6H r16g16b16_float */
    reznor_texture_format_BC7,         /**< BC7 rgb 4-7 bits per color channel */
    reznor_texture_format_ETC1,        /**< ETC1 r8g8b8 */
    reznor_texture_format_ETC2,        /**< ETC2 r8g8b8 */
    reznor_texture_format_ETC2A,       /**< ETC2 r8g8b8a8 */
    reznor_texture_format_ETC2A1,      /**< ETC2 r8g8b8a1 */
    reznor_texture_format_PTC12,       /**< PVRTC1 rgb 2BPP */
    reznor_texture_format_PTC14,       /**< PVRTC1 rgb 4BPP */
    reznor_texture_format_PTC22,       /**< PVRTC2 rgba 2BPP */
    reznor_texture_format_PTC24,       /**< PVRTC2 rgba 4BPP */
    reznor_texture_format_ASTC4x4,     /**< ASTC 4x4 8.0 BPP */
    reznor_texture_format_ASTC5x4,     /**< ASTC 5x4 6.40 BPP */
    reznor_texture_format_ASTC5x5,     /**< ASTC 5x5 5.12 BPP */
    reznor_texture_format_ASTC6x5,     /**< ASTC 6x5 4.27 BPP */
    reznor_texture_format_ASTC6x6,     /**< ASTC 6x6 3.56 BPP */
    reznor_texture_format_ASTC8x5,     /**< ASTC 8x5 3.20 BPP */
    reznor_texture_format_ASTC8x6,     /**< ASTC 8x6 2.67 BPP */
    reznor_texture_format_ASTC8x8,     /**< ASTC 8x8 2.0 BPP */
    reznor_texture_format_ASTC10x5,    /**< ASTC 10x5 2.56 BPP */
    reznor_texture_format_ASTC10x6,    /**< ASTC 10x6 2.13 BPP */
    reznor_texture_format_ASTC10x8,    /**< ASTC 10x8 1.60 BPP */
    reznor_texture_format_ASTC10x10,   /**< ASTC 10x10 1.28 BPP */
    reznor_texture_format_ASTC12x10,   /**< ASTC 12x10 1.07 BPP */
    reznor_texture_format_ASTC12x12,   /**< ASTC 12x12 0.89 BPP */
    /* format count */
    reznor_texture_format_count,
};

enum reznor_texture_usage {
    reznor_texture_usage_transfer_source            = (1u << 0),
    reznor_texture_usage_transfer_target            = (1u << 1),
    reznor_texture_usage_render_sampled             = (1u << 2),
    reznor_texture_usage_render_color_target        = (1u << 3),
    reznor_texture_usage_render_depth_target        = (1u << 4),
    reznor_texture_usage_render_stencil_attachment  = (1u << 5),
    reznor_texture_usage_render_shader_storage      = (1u << 6),
};

enum reznor_texture_layout {
    reznor_texture_layout_undefined = 0u,
    reznor_texture_layout_general,
    reznor_texture_layout_color_attachment_optimal,
    reznor_texture_layout_depth_attachment_optimal,
    reznor_texture_layout_depth_stencil_read_only_optimal,
    reznor_texture_layout_stencil_attachment_optimal,
    reznor_texture_layout_shader_read_only_optimal,
    reznor_texture_layout_transfer_source_optimal,
    reznor_texture_layout_transfer_target_optimal,
};

enum reznor_texture_aspect {
    reznor_texture_aspect_none      = (1u << 0),
    reznor_texture_aspect_color     = (1u << 1),
    reznor_texture_aspect_depth     = (1u << 2),
    reznor_texture_aspect_stencil   = (1u << 3),
    reznor_texture_aspect_metadata  = (1u << 4),
    reznor_texture_aspect_plane_0   = (1u << 5),
    reznor_texture_aspect_plane_1   = (1u << 6),
    reznor_texture_aspect_plane_2   = (1u << 7),
};

struct reznor_texture_subresource_layers {
    u32                                         aspect_mask; /**< enum reznor_texture_aspect bits */
    u32                                         mip_level;
    u32                                         base_array_layer;
    u32                                         layer_count;
};

struct reznor_texture_region {
    struct reznor_texture_subresource_layers    src_subresource;
    usize                                       src_offset;
    struct reznor_texture_subresource_layers    dst_subresource;
    usize                                       dst_offset;
    u32                                         width;
    u32                                         height;
    u32                                         depth;
};

struct reznor_texture_memory_binding {
    const struct reznor_texture                *texture;
    struct reznor_device_memory_range           memory_range;
};

struct reznor_texture_config {
    struct str                                  debug_name;
    u32                                         width;
    u32                                         height;
    u32                                         depth;          /* 1 default */
    u32                                         usage;          /**< enum reznor_texture_usage */
    u16                                         layer_count;    /* 1 default */
    u8                                          level_count;    /* 1 default */
    u8                                          sample_count;   /* 1 default */
    u8                                          format;         /**< enum reznor_texture_format */
    u8                                          owner_queue;    /**< enum reznor_queue_type */
    b8                                          allocate_memory;
    u8                                          view_format_count;
    const u8                                   *view_formats;
    enum reznor_comparison_function             preferred_comparison_function;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(texture)

enum reznor_sampler_filter_mode {
    reznor_sampler_filter_mode_nearest = 0u,
    reznor_sampler_filter_mode_linear,
};

enum reznor_sampler_address_mode {
    reznor_sampler_address_mode_wrap = 0u,
    reznor_sampler_address_mode_mirror,
    reznor_sampler_address_mode_clamp,
    reznor_sampler_address_mode_clamp_zo,
};

enum reznor_sampler_reduction_mode {
    reznor_sampler_reduction_mode_disabled = 0u,
    reznor_sampler_reduction_mode_min,
    reznor_sampler_reduction_mode_max,
};

struct reznor_sampler_config {
    struct str                                  debug_name;
    f32                                         mip_lod_bias;
    f32                                         min_lod;
    f32                                         max_lod;
    f32                                         max_anisotrophy_level;
    u8                                          min_filter;             /**< enum reznor_sampler_filter_mode */
    u8                                          mag_filter;             /**< enum reznor_sampler_filter_mode */
    u8                                          mip_filter;             /**< enum reznor_sampler_filter_mode */
    u8                                          address_u;              /**< enum reznor_sampler_address_mode*/
    u8                                          address_v;              /**< enum reznor_sampler_address_mode*/
    u8                                          address_w;              /**< enum reznor_sampler_address_mode*/
    u8                                          comparison_function;    /**< enum reznor_comparison_function */
    u8                                          reduction_mode;         /**< enum reznor_sampler_reduction_mode */
};
PFN_REZNOR_RESOURCE_ASSEMBLY(sampler)

struct reznor_shader_source {
    u8                                         *code;
    usize                                       size;
    const char                                 *entry_point;
};

enum reznor_shader_stage {
    reznor_shader_stage_vertex                  = (1u << 0),
    reznor_shader_stage_tesselation_control     = (1u << 1),
    reznor_shader_stage_tesselation_evaluation  = (1u << 2),
    reznor_shader_stage_geometry                = (1u << 3),
    reznor_shader_stage_fragment                = (1u << 4),
    reznor_shader_stage_compute                 = (1u << 5),
    reznor_shader_stage_all_graphics            = 0x0000001F,
    reznor_shader_stage_all                     = 0x7FFFFFFF,
    /* mesh shader */
    reznor_shader_stage_task                    = (1u << 6),
    reznor_shader_stage_mesh                    = (1u << 7),
    /* raytracing */
    reznor_shader_stage_raygen                  = (1u << 8),
    reznor_shader_stage_any_hit                 = (1u << 9),
    reznor_shader_stage_closest_hit             = (1u << 10),
    reznor_shader_stage_miss                    = (1u << 11),
    reznor_shader_stage_intersection            = (1u << 12),
    reznor_shader_stage_callable                = (1u << 13),
};

struct reznor_descriptor_set_layout_config {
    struct str                                  debug_name;
    /* TODO descriptor layout bindings */
};
PFN_REZNOR_RESOURCE_ASSEMBLY(descriptor_set_layout)

struct reznor_descriptor_set_config {
    struct str                                  debug_name;
    const struct reznor_descriptor_set_layout  *descriptor_set_layout;
    /* TODO descriptor data */
};
PFN_REZNOR_RESOURCE_ASSEMBLY(descriptor_set)
    
struct reznor_pipeline_layout_config {
    struct str                                  debug_name;
    const struct pelagia_descriptor_set_layout *descriptor_set_layouts[REZNOR_MAX_DESCRIPTOR_SET_SLOT_COUNT];
    u32                                         descriptor_set_layout_count;
    u16                                         push_constants_stage_mask;          /**< enum reznor_pipeline_stage */
    u16                                         push_constants_size;
    b32                                         use_bindless_descriptors;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(pipeline_layout)

struct reznor_graphics_pipeline_config {
    struct str                                  debug_name;
    struct reznor_shader_source                 vertex_shader;
    struct reznor_shader_source                 tesselation_control_shader; 
    struct reznor_shader_source                 tesselation_evaluation_shader; 
    struct reznor_shader_source                 geometry_shader;
    struct reznor_shader_source                 fragment_shader;
    struct reznor_shader_source                 task_shader;
    struct reznor_shader_source                 mesh_shader;

    const struct reznor_pipeline_layout        *pipeline_layout;
    f32                                         const_depth_bias;
    f32                                         slope_depth_bias;

    u16                                         shader_stages_mask;                 /**< enum reznor_shader_stage */
    u8                                          render_target_color_format;         /**< enum reznor_texture_format */
    u8                                          render_target_depth_stencil_format; /**< enum reznor_texture_format */
    u8                                          dynamic_state_mask;                 /**< enum reznor_dynamic_state_flag */
    u8                                          sample_count; /* 1 default */
    u8                                          patch_size;
    u8                                          primitive_type;                     /**< enum reznor_primitive_type */
    u8                                          cull_mode;                          /**< enum reznor_cull_mode */
    u8                                          fill_mode;                          /**< enum reznor_fill_mode */
    u8                                          winding_order;                      /**< enum reznor_winding_order */

    struct reznor_stencil_config                front_stencil;
    struct reznor_stencil_config                back_stencil;
    u8                                          color_write_mask[REZNOR_MAX_COLOR_TARGET_COUNT]; /**< enum reznor_color_write */
    u8                                          blend_op;                           /**< enum reznor_blend_operation */
    u8                                          blend_src_factor;                   /**< enum reznor_blend_factor */
    u8                                          blend_dst_factor;                   /**< enum reznor_blend_factor */
    u8                                          depth_comparison_function;          /**< enum reznor_comparison_function */

    u8                                          enable_sample_shading;
    u8                                          enable_alpha_to_coverage;
    u8                                          enable_depth_write;
    u8                                          enable_scissor_test;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(graphics_pipeline)

struct reznor_compute_pipeline_config {
    struct str                                  debug_name;
    struct reznor_shader_source                 shader;
    const struct reznor_pipeline_layout        *pipeline_layout;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(compute_pipeline)

struct reznor_raytracing_pipeline_config {
    struct str                                  debug_name;
    struct reznor_shader_source                 shader;
    const struct reznor_pipeline_layout        *pipeline_layout;

    const u8                                   *shader_group_handle_storage;
    u32                                         shader_group_handle_size;
    u32                                         shader_group_count;
    u32                                         raygen_group_index;
    u32                                         miss_group_index;
    u32                                         closest_hit_group_index;
    u32                                         miss_shader_count;
    u32                                         closest_hit_shader_count;
    u32                                         max_recursion_depth; /* 1 default */

};
PFN_REZNOR_RESOURCE_ASSEMBLY(raytracing_pipeline)

struct reznor_shader_binding_table_config { 
    struct str                                  debug_name;
    struct reznor_buffer_strided_range          raygen_shader_entry;
    struct reznor_buffer_strided_range          miss_shader_entry;
    struct reznor_buffer_strided_range          hit_shader_entry;
    struct reznor_buffer_strided_range          callable_shader_entry;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(shader_binding_table)

enum reznor_raytracing_vertex_format {
    reznor_raytracing_vertex_format_vec3,
    reznor_raytracing_vertex_format_u16_xyz_snorm,
    reznor_raytracing_vertex_format_u16_xyz_unorm,
};

struct reznor_bottom_level_config {
    struct reznor_buffer_range                  scratch_buffer;
    struct reznor_buffer_range                  vertex_buffer;
    struct reznor_buffer_range                  index_buffer;
    enum reznor_raytracing_vertex_format        vertex_format;
    enum reznor_index_format                    index_format;
    u32                                         vertex_stride;
    u32                                         vertex_count;
    u32                                         triangle_count;
    u32                                         max_vertex_count;
    u32                                         max_triangle_count;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(bottom_level)

enum reznor_top_level_instance_flags {
    reznor_top_level_instance_flag_disable_triangle_cull                = (1u << 0),
    reznor_top_level_instance_flag_triangle_facing_counter_clockwise    = (1u << 1),
    reznor_top_level_instance_flag_force_opaque                         = (1u << 2),
    reznor_top_level_instance_flag_force_non_opaque                     = (1u << 3),
};

struct attr_aligned(16) reznor_top_level_instance {
    mat4x3                                      transform;
    u32                                         instance_custom_index:24;
    u32                                         mask:8;
    u32                                         instance_shader_binding_table_record_offset:24;
    u32                                         flags:8;
    u64                                         mesh_address;
};
static_assert(sizeof(struct reznor_top_level_instance) == 64u, "Reznor top-level instance is a GPU/hardware format, must be 64 bytes.");

struct reznor_top_level_config {
    struct reznor_buffer_range                  scratch_buffer;
    struct reznor_buffer_range                  instance_buffer;
    u32                                         instance_mask;
    u32                                         instance_count;
    u32                                         max_instance_count;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(top_level)

enum reznor_query_type {
    reznor_query_type_timestamp_pipeline_bottom = 0,
    reznor_query_type_timestamp_pipeline_top,
};

struct reznor_query_pool_config {
    /* TODO */
};
PFN_REZNOR_RESOURCE_ASSEMBLY(query_pool)

/** Flags that control the state of a swapchain. */
enum reznor_swapchain_flags {
    /** True if the swapchain is currently connected to a system window. */
    reznor_swapchain_flag_attached_to_window       = (1u << 0),
    /** True whenever presentation fails due to a lost surface or destroyed window. */
    reznor_swapchain_flag_surface_was_lost         = (1u << 1),
    /** True whenever the present queue timed out for some reason. */
    reznor_swapchain_flag_timed_out                = (1u << 2),
    /** Set from a window event whenever the framebuffer extent changes. */
    reznor_swapchain_flag_framebuffer_resized      = (1u << 3),
    /** Vertical synchronization is used as the present mode. */
    reznor_swapchain_flag_vsync_enabled            = (1u << 4),
    /** True if the swapchain images can be saved on disk. */
    reznor_swapchain_flag_screenshot_supported     = (1u << 5),
    /** Will invoke an exit event for the attached window on 'PFN_pelagia_destroy_resources'. */
    reznor_swapchain_flag_close_window_on_destroy  = (1u << 6),
};

struct reznor_swapchain_config {
    struct hadal_window        *window;
    enum reznor_texture_format  preferred_format;
    u32                         presentation_interval;
};
PFN_REZNOR_RESOURCE_ASSEMBLY(swapchain)

typedef s32 (AMWCALL *PFN_reznor_swapchain_try_recreate)(struct reznor_swapchain *restrict swapchain);
#define FN_REZNOR_SWAPCHAIN_TRY_RECREATE(encore) \
    extern s32 AMWCALL _reznor_##encore##_swapchain_try_recreate(struct reznor_swapchain *restrict swapchain)

typedef void (AMWCALL *PFN_reznor_swapchain_next_image)(struct reznor_swapchain *swapchain);
#define FN_REZNOR_SWAPCHAIN_NEXT_IMAGE(encore) \
    extern void AMWCALL _reznor_##encore##_swapchain_next_image(struct reznor_swapchain *swapchain)

typedef void (AMWCALL *PFN_reznor_disassembly)(void **restrict resources, u32 resource_count);
#define FN_REZNOR_DISSASEMBLY(encore) \
    extern void AMWCALL _reznor_##encore##_dissasembly(void **restrict resources, u32 resource_count)

typedef void (AMWCALL *PFN_reznor_frame_begin)(struct reznor_device_frame *frame);
#define FN_REZNOR_FRAME_BEGIN(encore) \
    extern void AMWCALL _reznor_##encore##_frame_begin(struct reznor_device_frame *frame);

typedef void (AMWCALL *PFN_reznor_frame_submit)(struct reznor_device_frame *frame);
#define FN_REZNOR_FRAME_SUBMIT(encore) \
    extern void AMWCALL _reznor_##encore##_frame_submit(struct reznor_device_frame *frame);

typedef void (AMWCALL *PFN_reznor_command_draw)(
    struct reznor_command_buffer       *command_buffer,
    u32                                 vertex_count,
    u32                                 instance_count,
    u32                                 vertex_offset,
    u32                                 instance_offset);
#define FN_REZNOR_COMMAND_DRAW(encore)                      \
    extern void AMWCALL _reznor_##encore##_command_draw(    \
        struct reznor_command_buffer   *command_buffer,     \
        u32                             vertex_count,       \
        u32                             instance_count,     \
        u32                             vertex_offset,      \
        u32                             instance_offset)

#define ARGS_REZNOR_COMMAND_DRAW_INDEXED                \
    struct reznor_command_buffer       *command_buffer, \
    u32                                 index_count,    \
    u32                                 instance_count, \
    u32                                 index_offset,   \
    u32                                 vertex_offset,  \
    u32                                 instance_offset
typedef void (AMWCALL *PFN_reznor_command_draw_indexed)(ARGS_REZNOR_COMMAND_DRAW_INDEXED);
#define FN_REZNOR_COMMAND_DRAW_INDEXED(encore) \
    extern void AMWCALL _reznor_##encore##_command_draw_indexed(ARGS_REZNOR_COMMAND_DRAW_INDEXED)

struct reznor_command_draw_indexed_indirect {
    u32 index_count;
    u32 instance_count;
    u32 index_offset;
    u32 vertex_offset;
    u32 instance_offset;
};

#define ARGS_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT       \
    struct reznor_command_buffer       *command_buffer, \
    struct reznor_buffer               *buffer,         \
    usize                               offset,         \
    u32                                 draw_count,     \
    u32                                 stride
typedef void (AMWCALL *PFN_reznor_command_draw_indexed_indirect)(ARGS_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT);
#define FN_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT(encore) \
    extern void AMWCALL _reznor_##encore##_command_draw_indexed_indirect(ARGS_REZNOR_COMMAND_DRAW_INDEXED_INDIRECT)

struct reznor_command_draw_indirect {
    u32 vertex_count;
    u32 instance_count;
    u32 vertex_offset;
    u32 instance_offset;
};

#define ARGS_REZNOR_COMMAND_DRAW_INDIRECT               \
    struct reznor_command_buffer       *command_buffer, \
    struct reznor_buffer               *buffer,         \
    usize                               offset,         \
    u32                                 draw_count,     \
    u32                                 stride
typedef void (AMWCALL *PFN_reznor_command_draw_indirect)(ARGS_REZNOR_COMMAND_DRAW_INDIRECT);
#define FN_REZNOR_COMMAND_DRAW_INDIRECT(encore) \
    extern void AMWCALL _reznor_##encore##_command_draw_indirect(ARGS_REZNOR_COMMAND_DRAW_INDIRECT)

#define ARGS_REZNOR_COMMAND_DISPATCH                    \
    struct reznor_command_buffer       *command_buffer, \
    u32                                 group_count_x,  \
    u32                                 group_count_y,  \
    u32                                 group_count_z
typedef void (AMWCALL *PFN_reznor_command_dispatch)(ARGS_REZNOR_COMMAND_DISPATCH);
#define FN_REZNOR_COMMAND_DISPATCH(encore) \
    extern void AMWCALL _reznor_##encore##_command_dispatch(ARGS_REZNOR_COMMAND_DISPATCH)

struct reznor_command_dispatch_indirect {
    u32 group_count_x;
    u32 group_count_y;
    u32 group_count_z;
};

#define ARGS_REZNOR_COMMAND_DISPATCH_INDIRECT           \
    struct reznor_command_buffer       *command_buffer, \
    struct reznor_buffer               *buffer,         \
    usize                               offset
typedef void (AMWCALL *PFN_reznor_command_dispatch_indirect)(ARGS_REZNOR_COMMAND_DISPATCH_INDIRECT);
#define FN_REZNOR_COMMAND_DISPATCH_INDIRECT(encore) \
    extern void AMWCALL _reznor_##encore##_command_dispatch_indirect(ARGS_REZNOR_COMMAND_DISPATCH_INDIRECT)

#define ARGS_REZNOR_COMMAND_COPY_BUFFER                 \
    struct reznor_command_buffer       *command_buffer, \
    struct reznor_buffer               *src_buffer,     \
    struct reznor_buffer               *dst_buffer,     \
    u32                                 region_count,   \
    const struct reznor_buffer_region  *regions
typedef void (AMWCALL *PFN_reznor_command_copy_buffer)(ARGS_REZNOR_COMMAND_COPY_BUFFER);
#define FN_REZNOR_COMMAND_COPY_BUFFER(encore) \
    extern void AMWCALL _reznor_##encore##_command_copy_buffer(ARGS_REZNOR_COMMAND_COPY_BUFFER)

#define ARGS_REZNOR_COMMAND_COPY_TEXTURE                \
    struct reznor_command_buffer       *command_buffer, \
    struct reznor_texture              *src_texture,    \
    struct reznor_texture              *dst_texture,    \
    u32                                 region_count,   \
    const struct reznor_texture_region *regions
typedef void (AMWCALL *PFN_reznor_command_copy_texture)(ARGS_REZNOR_COMMAND_COPY_TEXTURE);
#define FN_REZNOR_COMMAND_COPY_TEXTURE(encore) \
    extern void AMWCALL _reznor_##encore##_command_copy_texture(ARGS_REZNOR_COMMAND_COPY_TEXTURE)

struct reznor_rendering_attachment {
    const struct reznor_texture        *texture;
    const struct reznor_texture        *resolve_texture;
    enum reznor_texture_layout          texture_layout;
    enum reznor_texture_layout          resolve_texture_layout;
    /* XXX load action, store action ? clear value ? */
};

struct reznor_rendering_config {
    struct str                          debug_name;
    struct reznor_rendering_attachment  color_attachment[REZNOR_MAX_COLOR_TARGET_COUNT];
    u32                                 color_attachment_count;
    struct reznor_rendering_attachment  depth_attachment;
    struct reznor_rendering_attachment  stencil_attachment;
};

typedef void (AMWCALL *PFN_reznor_command_begin_render_pass)(struct reznor_command_buffer *command_buffer, struct reznor_rendering_config *config);
#define FN_REZNOR_COMMAND_BEGIN_RENDER_PASS(encore) \
    extern void AMWCALL _reznor_##encore##_command_begin_render_pass(struct reznor_command_buffer *command_buffer, struct reznor_rendering_config *config)

typedef void (AMWCALL *PFN_reznor_command_end_render_pass)(struct reznor_command_buffer *command_buffer);
#define FN_REZNOR_COMMAND_END_RENDER_PASS(encore) \
    extern void AMWCALL _reznor_##encore##_command_end_render_pass(struct reznor_command_buffer *command_buffer)

struct reznor_interface {
    struct riven_interface_header                   header;

    PFN_reznor_device_query                         device_query;
    PFN_reznor_device_create                        device_create;
    PFN_reznor_device_destroy                       device_destroy;

    PFN_reznor_device_memory_assembly               device_memory_assembly;
    PFN_reznor_buffer_assembly                      buffer_assembly;
    PFN_reznor_texture_assembly                     texture_assembly;
    PFN_reznor_sampler_assembly                     sampler_assembly;
    PFN_reznor_descriptor_set_layout_assembly       descriptor_set_layout_assembly;
    PFN_reznor_descriptor_set_assembly              descriptor_set_assembly;
    PFN_reznor_pipeline_layout_assembly             pipeline_layout_assembly;
    PFN_reznor_graphics_pipeline_assembly           graphics_pipeline_assembly;
    PFN_reznor_compute_pipeline_assembly            compute_pipeline_assembly;
    PFN_reznor_raytracing_pipeline_assembly         raytracing_pipeline_assembly;
    PFN_reznor_shader_binding_table_assembly        shader_binding_table_assembly;
    PFN_reznor_bottom_level_assembly                bottom_level_assembly;
    PFN_reznor_top_level_assembly                   top_level_assembly;
    PFN_reznor_query_pool_assembly                  query_pool_assembly;
    PFN_reznor_swapchain_assembly                   swapchain_assembly;
    PFN_reznor_swapchain_try_recreate               swapchain_try_recreate;
    PFN_reznor_swapchain_next_image                 swapchain_next_image;
    PFN_reznor_disassembly                          disassembly;

    PFN_reznor_frame_begin                          frame_begin;
    PFN_reznor_frame_submit                         frame_submit;
    
    PFN_reznor_command_draw                         command_draw;
    PFN_reznor_command_draw_indexed                 command_draw_indexed;
    PFN_reznor_command_draw_indexed_indirect        command_draw_indexed_indirect;
    PFN_reznor_command_draw_indirect                command_draw_indirect;
    PFN_reznor_command_dispatch                     command_dispatch;
    PFN_reznor_command_dispatch_indirect            command_dispatch_indirect;
    PFN_reznor_command_copy_buffer                  command_copy_buffer;
    PFN_reznor_command_copy_texture                 command_copy_texture;
    PFN_reznor_command_begin_render_pass            command_begin_render_pass;
    PFN_reznor_command_end_render_pass              command_end_render_pass;
};

struct reznor_devices_assembly_work {
    struct work_header              header;
    struct reznor                  *reznor;
    struct riven                   *riven;
    struct memory_requirements      memory;
    u32                             frames_in_flight;
    u32                             thread_count;
    /** A device config of this index, if exists, will be of a primary role and first in the 'out_devices' array. */
    s32                             force_primary_index;
    /** Discard all physical devices that are not of discrete type, unless no discrete GPUs are available. */
    b32                             only_discrete;                           
    /* output */
    struct reznor_device          **out_devices;
    u32                            *out_device_count;
};

/** Creates a bunch of rendering devices and per-device work structures for frames in flight.
 *  Performs a physical devices query and selects best, for the given configuration.
 *  The allocation structure can be used for querying memory requirements and performing
 *  a single allocation for devices and internal structures of per-frame data, or it can 
 *  reference a single riven_tag_t so this function can allocate memory by itself
 *  under a tagged heap. Can be run as a job. The work to create a device and related 
 *  per-frame structures is run as jobs too, for every device to be created. */
AMWAPI void AMWCALL reznor_devices_assembly(struct reznor_devices_assembly_work *restrict work);

/** Must be a valid GPU resource. */
attr_inline void attr_nonnull_all reznor_destroy_resource(struct reznor_interface *reznor, void *resource) 
{
    reznor->disassembly(&resource, 1);
}

attr_inline struct reznor_device_frame *attr_nonnull_all 
reznor_frame_acquire(u64 frame_index, struct reznor_device *device)
{
    struct reznor_device_header *header = (struct reznor_device_header *)device;    
    return header->frames[frame_index % header->frames_in_flight];
}

#ifdef __cplusplus
}
#endif
