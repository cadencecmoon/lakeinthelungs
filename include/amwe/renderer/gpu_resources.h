#pragma once

#include <amwe/renderer/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** We represent GPU resources (buffers, textures, views, samplers and acceleration structures) as IDs.
 *  They are all accessed via a bindless API, and can represent the same resource on multiple devices. */
typedef struct { u64 value; } xaku_resource_id;
/* safe types */
typedef struct { u64 value; } xaku_buffer_id;
typedef struct { u64 value; } xaku_texture_id;
typedef struct { u64 value; } xaku_texture_view_id;
typedef struct { u64 value; } xaku_sampler_id;
typedef struct { u64 value; } xaku_tlas_id;
typedef struct { u64 value; } xaku_blas_id;

#define XAKU_ID_INDEX_MASK      ((1ull << 20) - 1)
#define XAKU_ID_VERSION_SHIFT   20

#define xaku_id_make(T, IDX, VER) \
    ((T){ .value = ((u64)(VER) << XAKU_ID_VERSION_SHIFT) | ((IDX) & XAKU_ID_INDEX_MASK) })
#define xaku_id_get_index(ID) \
    ((u32)((ID).value & XAKU_ID_INDEX_MASK))
#define xaku_id_get_version(ID) \
    ((u64)(((ID).value) >> XAKU_ID_VERSION_SHIFT))
#define xaku_id_set_index(ID, IDX) \
    ((ID).value = ((ID).value & ~XAKU_ID_INDEX_MASK) | ((IDX) & XAKU_ID_INDEX_MASK))
#define xaku_id_set_version(ID, VER) \
    ((ID).value = ((ID).value & XAKU_ID_INDEX_MASK) | ((u64)(VER) << XAKU_ID_VERSION_SHIFT))

struct xaku_buffer_assembly {
    usize                   size;
    /* ignored when allocating with a memory block */
    u32                     allocate_flags; /**< enum xaku_memory_flag_bits */
    lake_small_string       name;
};
#define XAKU_DEFAULT_BUFFER_ASSEMBLY { \
    .size = 0lu, \
    .allocate_flags = xaku_memory_flag_none, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_buffer_memory_assembly {
    struct xaku_buffer_assembly     buffer_assembly;
    struct xaku_device_memory      *memory;
    usize                           offset;
};
#define XAKU_DEFAULT_DEVICE_MEMORY_BUFFER_ASSEMBLY LAKE_ZERO_INIT

enum xaku_color_component_bits {
    xaku_color_component_r = (1u << 0),
    xaku_color_component_g = (1u << 1),
    xaku_color_component_b = (1u << 2),
    xaku_color_component_a = (1u << 3),
};

enum xaku_blend_op {
    xaku_blend_op_none = 0u,
    xaku_blend_op_add,
    xaku_blend_op_subtract,
    xaku_blend_op_rev_subtract,
    xaku_blend_op_min,
    xaku_blend_op_max,
};

enum xaku_blend_factor {
    xaku_blend_factor_zero = 0u,
    xaku_blend_factor_one,
    xaku_blend_factor_src_color,
    xaku_blend_factor_inv_src_color,
    xaku_blend_factor_src_alpha,
    xaku_blend_factor_inv_src_alpha,
    xaku_blend_factor_dest_alpha,
    xaku_blend_factor_inv_dest_alpha,
    xaku_blend_factor_dest_color,
    xaku_blend_factor_inv_dest_color,
    xaku_blend_factor_src_alpha_sat,
};

enum xaku_primitive_topology {
    xaku_primitive_topology_point_list = 0,
    xaku_primitive_topology_line_list,
    xaku_primitive_topology_line_strip,
    xaku_primitive_topology_triangle_list,
    xaku_primitive_topology_triangle_strip,
    xaku_primitive_topology_triangle_fan,
    xaku_primitive_topology_patch_list,
};

enum xaku_sharing_mode {
    xaku_sharing_mode_exclusive = 0,
    xaku_sharing_mode_concurrent,
};

enum xaku_polygon_mode {
    xaku_polygon_mode_fill = 0,
    xaku_polygon_mode_line,
    xaku_polygon_mode_point,
};

enum xaku_index_format {
    xaku_index_format_invalid = 0u,
    xaku_index_format_u16,
    xaku_index_format_u32,
};

enum xaku_cull_mode {
    xaku_cull_mode_none = 0u,
    xaku_cull_mode_front,
    xaku_cull_mode_back,
};

/* counter clockwise, clockwise */
enum xaku_winding_order {
    xaku_winding_order_ccw = 0u,
    xaku_winding_order_cw,
};

enum xaku_compare_op {
    xaku_compare_op_never = 0u,
    xaku_compare_op_less,
    xaku_compare_op_equal,
    xaku_compare_op_less_equal,
    xaku_compare_op_not_equal,
    xaku_compare_op_greater,
    xaku_compare_op_greater_equal,
    xaku_compare_op_always,
};

enum xaku_load_op {
    xaku_load_op_load = 0u,
    xaku_load_op_clear,
    xaku_load_op_dont_care,
    xaku_load_op_none,
};

enum xaku_store_op {
    xaku_store_op_store = 0u,
    xaku_store_op_dont_care,
    xaku_store_op_none,
};

enum xaku_stencil_face {
    xaku_stencil_face_front = 0u,
    xaku_stencil_face_back,
};

enum xaku_stencil_op {
    xaku_stencil_op_keep = 0u,
    xaku_stencil_op_zero,
    xaku_stencil_op_replace,
    xaku_stencil_op_increment,
    xaku_stencil_op_decrement,
    xaku_stencil_op_increment_wrap,
    xaku_stencil_op_decrement_wrap,
    xaku_stencil_op_invert,
};

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
enum xaku_format {
    xaku_format_unknown = 0u,
    xaku_format_r1_unorm,        /**< 1-bit no format */
    xaku_format_a8_unorm,        /**< 8-bit alpha */
    /* depth formats */
    xaku_format_d16_unorm,       /**< 16-bit unorm depth */
    xaku_format_d16s8_unorm,     /**< 24-bit unorm depth/stencil */
    xaku_format_d24s8_unorm,     /**< 32-bit unorm depth/stencil */
    xaku_format_d32_float,       /**< 32-bit float depth */
    xaku_format_d0s8_unorm,      /**< 8-bit unorm stencil */
    /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
    xaku_format_r8_snorm,
    xaku_format_r8_unorm,
    xaku_format_r8_sint,
    xaku_format_r8_uint,
    /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r16_float,
    xaku_format_r16_snorm,
    xaku_format_r16_unorm,
    xaku_format_r16_sint,
    xaku_format_r16_uint,
    xaku_format_r8g8_snorm,
    xaku_format_r8g8_unorm,
    xaku_format_r8g8_sint,
    xaku_format_r8g8_uint,
    /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r8g8b8_snorm,
    xaku_format_r8g8b8_unorm,
    xaku_format_r8g8b8_sint,
    xaku_format_r8g8b8_uint,
    /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
    xaku_format_r32_float,
    xaku_format_r32_sint,
    xaku_format_r32_uint,
    xaku_format_r16g16_float,
    xaku_format_r16g16_snorm,
    xaku_format_r16g16_unorm,
    xaku_format_r16g16_sint,
    xaku_format_r16g16_uint,
    xaku_format_r8g8b8a8_srgb,
    xaku_format_r8g8b8a8_unorm,
    xaku_format_r8g8b8a8_sint,
    xaku_format_r8g8b8a8_uint,
    xaku_format_b8g8r8a8_snorm,
    xaku_format_b8g8r8a8_unorm,
    xaku_format_b8g8r8a8_sint,
    xaku_format_b8g8r8a8_uint,
    xaku_format_r10g10b10a2,
    xaku_format_b10g10r10a2,
    /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r16g16b16_float,
    xaku_format_r16g16b16_snorm,
    xaku_format_r16g16b16_unorm,
    xaku_format_r16g16b16_sint,
    xaku_format_r16g16b16_uint,
    /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64_float,
    xaku_format_r64_sint,
    xaku_format_r64_uint,
    xaku_format_r32g32_float,
    xaku_format_r32g32_sint,
    xaku_format_r32g32_uint,
    xaku_format_r16g16b16a16_float,
    xaku_format_r16g16b16a16_snorm,
    xaku_format_r16g16b16a16_unorm,
    xaku_format_r16g16b16a16_sint,
    xaku_format_r16g16b16a16_uint,
    /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r32g32b32_float,
    xaku_format_r32g32b32_sint,
    xaku_format_r32g32b32_uint,
    /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r32g32b32a32_float,
    xaku_format_r32g32b32a32_sint,
    xaku_format_r32g32b32a32_uint,
    xaku_format_r64g64_float,
    xaku_format_r64g64_sint,
    xaku_format_r64g64_uint,
    /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64g64b64_float,
    xaku_format_r64g64b64_sint,
    xaku_format_r64g64b64_uint,
    /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
    xaku_format_r64g64b64a64_float,
    xaku_format_r64g64b64a64_sint,
    xaku_format_r64g64b64a64_uint,
    /* compressed texture formats */
    xaku_format_BC1,         /**< DXT1 r5g6b5a1 */
    xaku_format_BC2,         /**< DXT3 r5g6b6a4 */
    xaku_format_BC3,         /**< DXT5 r5g6b5a8 */
    xaku_format_BC4,         /**< LATC1/ATI1 r8 */
    xaku_format_BC5,         /**< LATC2/ATI2 r8g8 */
    xaku_format_BC6H,        /**< BC6H r16g16b16_float */
    xaku_format_BC7,         /**< BC7 rgb 4-7 bits per color channel */
    xaku_format_ETC1,        /**< ETC1 r8g8b8 */
    xaku_format_ETC2,        /**< ETC2 r8g8b8 */
    xaku_format_ETC2A,       /**< ETC2 r8g8b8a8 */
    xaku_format_ETC2A1,      /**< ETC2 r8g8b8a1 */
    xaku_format_PTC12,       /**< PVRTC1 rgb 2BPP */
    xaku_format_PTC14,       /**< PVRTC1 rgb 4BPP */
    xaku_format_PTC22,       /**< PVRTC2 rgba 2BPP */
    xaku_format_PTC24,       /**< PVRTC2 rgba 4BPP */
    xaku_format_ASTC4x4,     /**< ASTC 4x4 8.0 BPP */
    xaku_format_ASTC5x4,     /**< ASTC 5x4 6.40 BPP */
    xaku_format_ASTC5x5,     /**< ASTC 5x5 5.12 BPP */
    xaku_format_ASTC6x5,     /**< ASTC 6x5 4.27 BPP */
    xaku_format_ASTC6x6,     /**< ASTC 6x6 3.56 BPP */
    xaku_format_ASTC8x5,     /**< ASTC 8x5 3.20 BPP */
    xaku_format_ASTC8x6,     /**< ASTC 8x6 2.67 BPP */
    xaku_format_ASTC8x8,     /**< ASTC 8x8 2.0 BPP */
    xaku_format_ASTC10x5,    /**< ASTC 10x5 2.56 BPP */
    xaku_format_ASTC10x6,    /**< ASTC 10x6 2.13 BPP */
    xaku_format_ASTC10x8,    /**< ASTC 10x8 1.60 BPP */
    xaku_format_ASTC10x10,   /**< ASTC 10x10 1.28 BPP */
    xaku_format_ASTC12x10,   /**< ASTC 12x10 1.07 BPP */
    xaku_format_ASTC12x12,   /**< ASTC 12x12 0.89 BPP */
    /* format count */
    xaku_format_count,
};

/** inspired by https://github.com/Tobski/simple_vulkan_synchronization */
enum xaku_access {
    xaku_access_general = 0u,                                                             /**< Covers any access - useful for debugging, avoid for performance reasons. */
    /* read access */
    xaku_access_indirect_buffer,                                                          /**< Read as an indirect buffer for drawing or dispatch. */
    xaku_access_index_buffer,                                                             /**< Read as an index buffer for drawing. */
    xaku_access_vertex_buffer,                                                            /**< Read as a vertex buffer for drawing. */
    xaku_access_vertex_shader_read_uniform_buffer,                                        /**< Read as a uniform buffer in a vertex shader. */
    xaku_access_vertex_shader_read_sampled_image_or_uniform_texel_buffer,                 /**< Read as a sampled image/uniform texel buffer in a vertex shader. */
    xaku_access_vertex_shader_read_other,                                                 /**< Read as any other resource in a vertex shader. */
    xaku_access_tesselation_control_shader_read_uniform_buffer,                           /**< Read as a uniform buffer in a tesselation control shader. */
    xaku_access_tesselation_control_shader_read_sampled_image_or_uniform_texel_buffer,    /**< Read as a sampled image/uniform texel buffer in a tesselation control shader. */
    xaku_access_tesselation_control_shader_read_other,                                    /**< Read as any other resource in a tesselation control shader. */
    xaku_access_tesselation_evaluation_shader_read_uniform_buffer,                        /**< Read as a uniform buffer in a tesselation evaluation shader. */
    xaku_access_tesselation_evaluation_shader_read_sampled_image_or_uniform_texel_buffer, /**< Read as a sampled image/uniform texel buffer in a tesselation evaluation shader. */
    xaku_access_tesselation_evaluation_shader_read_other,                                 /**< Read as any other resource in a tesselation evaluation shader. */
    xaku_access_geometry_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a geometry shader. */
    xaku_access_geometry_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a geometry shader. */
    xaku_access_geometry_shader_read_other,                                               /**< Read as any other resource in a geometry shader. */
    xaku_access_task_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a task shader. */
    xaku_access_task_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a task shader. */
    xaku_access_task_shader_read_other,                                                   /**< Read as any other resource in a task shader. */
    xaku_access_mesh_shader_read_uniform_buffer,                                          /**< Read as a uniform buffer in a mesh shader. */
    xaku_access_mesh_shader_read_sampled_image_or_uniform_texel_buffer,                   /**< Read as a sampled image/uniform texel buffer in a mesh shader. */
    xaku_access_mesh_shader_read_other,                                                   /**< Read as any other resource in a mesh shader. */
    xaku_access_transform_feedback_counter_read,                                          /**< Read as a transform feedback counter buffer. */
    xaku_access_fragment_density_map_read,                                                /**< Read as a fragment density map image. */
    xaku_access_shading_rate_read,                                                        /**< Read as a shading rate image. */
    xaku_access_fragment_shader_read_uniform_buffer,                                      /**< Read as a uniform buffer in a fragment shader. */
    xaku_access_fragment_shader_read_sampled_image_or_uniform_texel_buffer,               /**< Read as a sampled image/uniform texel buffer in a fragment shader. */
    xaku_access_fragment_shader_read_color_input_attachment,                              /**< Read as an input attachment with a color format in a fragment shader. */
    xaku_access_fragment_shader_read_depth_stencil_input_attachment,                      /**< Read as an input attachment with a depth/stencil format in a fragment shader. */
    xaku_access_fragment_shader_read_other,                                               /**< Read as any other resource in a fragment shader. */
    xaku_access_color_attachment_read,                                                    /**< Read by standard blending/logic operations or local read/subpass load operations. */
    xaku_access_color_attachment_advanced_blending,                                       /**< Read by advanced blending, standard blending, logic operations or local read/subpass load operations. */
    xaku_access_depth_stencil_attachment_read,                                            /**< Read by depth/stencil tests or local read/subpass load operations. */
    xaku_access_compute_shader_read_uniform_buffer,                                       /**< Read as a uniform buffer in a compute shader. */
    xaku_access_compute_shader_read_sampled_image_or_uniform_texel_buffer,                /**< Read as a sampled image/uniform texel buffer in a compute shader. */
    xaku_access_compute_shader_read_other,                                                /**< Read as any other resource in a compute shader. */
    xaku_access_any_shader_read_uniform_buffer,                                           /**< Read as a uniform buffer in any shader. */
    xaku_access_any_shader_read_uniform_buffer_or_vertex_buffer,                          /**< Read as a uniform buffer in any shader, or a vertex buffer. */
    xaku_access_any_shader_read_sampled_image_or_uniform_texel_buffer,                    /**< Read as a sample image/uniform texel buffer in any shader. */
    xaku_access_any_shader_read_other,                                                    /**< Read as any other resource (excluding attachments) in any shader. */
    xaku_access_transfer_read,                                                            /**< Read as the source of a transfer operation. */
    xaku_access_host_read,                                                                /**< Read on the host. */
    xaku_access_present,                                                                  /**< Read by presentation engine (i.e. vkQueuePresentKHR). */
    xaku_access_conditional_rendering_read,                                               /**< Read by conditional rendering. */
    xaku_access_raytracing_shader_acceleration_structure_read,                            /**< Read by a raytracing shader as an acceleration structure. */
    xaku_access_acceleration_structure_build_read,                                        /**< Read as an acceleration structure during a build. */
    xaku_access_end_read,                                                                 /**< Read access end. */
    /* write access */
    xaku_access_command_buffer_write,                                                     /**< Command buffer write operation. */
    xaku_access_vertex_shader_write,                                                      /**< Written as any resource in a vertex shader. */
    xaku_access_tesselation_control_shader_write,                                         /**< Written as any resource in a tesselation control shader. */
    xaku_access_tesselation_evaluation_shader_write,                                      /**< Written as any resource in a tesselation evaluation shader. */
    xaku_access_geometry_shader_write,                                                    /**< Written as any resource in a geometry shader. */
    xaku_access_task_shader_write,                                                        /**< Written as any resource in a task shader. */
    xaku_access_mesh_shader_write,                                                        /**< Written as any resource in a mesh shader. */
    xaku_access_transform_feedback_write,                                                 /**< Written as a transform feedback buffer. */
    xaku_access_transform_feedback_counter_write,                                         /**< Written as a transform feedback counter buffer. */
    xaku_access_fragment_shader_write,                                                    /**< Written as any resource in a fragment shader, */
    xaku_access_color_attachment_write,                                                   /**< Written as a color attachment during rendering, or via a subpass store op. */
    xaku_access_depth_stencil_attachment_write,                                           /**< Written as a depth/stencil attachment during rendering, or via a subpass store op. */
    xaku_access_depth_attachment_write_stencil_read_only,                                 /**< Written as a depth aspect of a depth/stencil attachment during rendering, whilst the stencil aspect is read-only. */
    xaku_access_stencil_attachment_write_depth_read_only,                                 /**< Written as a stencil aspect of a depth/stencil attachment during rendering, whilst the depth aspect is read-only. */
    xaku_access_compute_shader_write,                                                     /**< Written as any resource in a compute shader. */
    xaku_access_any_shader_write,                                                         /**< Written as any resource in any shader. */
    xaku_access_transfer_write,                                                           /**< Written as the destination of a transfer operation. */
    xaku_access_host_preinitialized,                                                      /**< Data pre-filled by host before device access starts. */
    xaku_access_host_write,                                                               /**< Written on the host. */
    xaku_access_acceleration_structure_build_write,                                       /**< Written as an acceleration structure during a build. */
    xaku_access_color_attachment_read_write,                                              /**< Read or written as a color attachment during rendering. */
    xaku_access_end_write,                                                                /**< Write access end, access types count. */
};

struct xaku_texture_mip_array_slice {
    u32 base_mip_level;
    u32 level_count;
    u32 base_array_layer;
    u32 layer_count;
};

struct xaku_texture_array_slice {
    u32 mip_level;
    u32 base_array_layer;
    u32 layer_count;
};

struct xaku_texture_slice {
    u32 mip_level;
    u32 array_layer;
};

enum xaku_texture_flag_bits {
    xaku_texture_flag_none                  = 0,
    xaku_texture_flag_allow_mutable_format  = (1u << 0),
    xaku_texture_flag_compatible_cube       = (1u << 0),
    xaku_texture_flag_compatible_2d_array   = (1u << 0),
    xaku_texture_flag_allow_alias           = (1u << 0),
};

enum xaku_texture_usage_bits {
    xaku_texture_usage_none                             = 0,
    xaku_texture_usage_transfer_src                     = (1u << 0),
    xaku_texture_usage_transfer_dst                     = (1u << 1),
    xaku_texture_usage_shader_sampled                   = (1u << 2),
    xaku_texture_usage_shader_storage                   = (1u << 3),
    xaku_texture_usage_color_attachment                 = (1u << 4),
    xaku_texture_usage_depth_stencil_attachment         = (1u << 5),
    xaku_texture_usage_transient_attachment             = (1u << 6),
    xaku_texture_usage_fragment_density_map             = (1u << 7),
    xaku_texture_usage_fragment_shading_rate_attachment = (1u << 8),
};

enum xaku_texture_layout {
    xaku_texture_layout_undefined = 0,
    xaku_texture_layout_general,
    xaku_texture_layout_transfer_src_optimal,
    xaku_texture_layout_transfer_dst_optimal,
    xaku_texture_layout_read_only_optimal,
    xaku_texture_layout_attachment_optimal,
    xaku_texture_layout_present_src,
};

enum xaku_texture_aspect {
    xaku_texture_aspect_none        = (1u << 0),
    xaku_texture_aspect_color       = (1u << 1),
    xaku_texture_aspect_depth       = (1u << 2),
    xaku_texture_aspect_stencil     = (1u << 3),
    xaku_texture_aspect_metadata    = (1u << 4),
    xaku_texture_aspect_plane_0     = (1u << 5),
    xaku_texture_aspect_plane_1     = (1u << 6),
    xaku_texture_aspect_plane_2     = (1u << 7),
};

struct xaku_texture_assembly {
    u32                     flags; /**< enum xaku_texture_flag_bits */
    u32                     dimensions;
    enum xaku_format        format;
    struct lake_extent3d    extent;
    u32                     mip_level_count;
    u32                     array_layer_count;
    u32                     sample_count;
    u32                     usage; /**< enum xaku_texture_usage_bits */
    enum xaku_sharing_mode  sharing_mode;
    /* ignored when allocating with a memory block */
    u32                     allocate_flags; /**< enum xaku_memory_flag_bits */
    lake_small_string       name;
};
#define XAKU_DEFAULT_TEXTURE_ASSEMBLY { \
    .flags = xaku_texture_flag_none, \
    .dimensions = 2, \
    .format = xaku_format_r8g8b8a8_srgb, \
    .extent = {0u, 0u, 0u}, \
    .mip_level_count = 1, \
    .array_layer_count = 1, \
    .sample_count = 1, \
    .usage = xaku_texture_usage_none, \
    .sharing_mode = xaku_sharing_mode_concurrent, \
    .allocate_flags = xaku_memory_flag_none, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_texture_memory_assembly {
    struct xaku_texture_assembly    texture_assembly;
    struct xaku_device_memory      *memory;
    usize                           offset;
};
#define XAKU_DEFAULT_DEVICE_MEMORY_TEXTURE_ASSEMBLY LAKE_ZERO_INIT

struct xaku_memory_barrier {
    enum xaku_access                    src_access;
    enum xaku_access                    dst_access;
};

struct xaku_texture_memory_barrier {
    enum xaku_access                    src_access;
    enum xaku_access                    dst_access;
    enum xaku_texture_layout            src_layout;
    enum xaku_texture_layout            dst_layout;
    struct xaku_texture_mip_array_slice texture_slice;
    xaku_texture_id                     texture;
};

enum xaku_texture_view_type {
    xaku_texture_view_type_1d = 0,
    xaku_texture_view_type_2d,
    xaku_texture_view_type_3d,
    xaku_texture_view_type_cube,
    xaku_texture_view_type_1d_array,
    xaku_texture_view_type_2d_array,
    xaku_texture_view_type_cube_array,
};

enum xaku_resolve_mode_bits {
    xaku_resolve_mode_none                          = 0,
    xaku_resolve_mode_sample_zero                   = (1u << 0),
    xaku_resolve_mode_average                       = (1u << 1),
    xaku_resolve_mode_min                           = (1u << 2),
    xaku_resolve_mode_max                           = (1u << 3),
    xaku_resolve_mode_external_downsample_android   = (1u << 4),
};

struct xaku_texture_view_assembly {
    enum xaku_texture_view_type         type;
    enum xaku_format                    format;
    xaku_texture_id                     texture;
    struct xaku_texture_mip_array_slice slice;
    lake_small_string                   name;
};
#define XAKU_DEFAULT_TEXTURE_VIEW_ASSEMBLY { \
    .type = xaku_texture_view_type_2d, \
    .format = xaku_format_r8g8b8a8_srgb, \
    .texture = LAKE_ZERO_INIT, \
    .slice = LAKE_ZERO_INIT, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

enum xaku_filter_mode {
    xaku_filter_mode_nearest = 0u,
    xaku_filter_mode_linear,
};

enum xaku_border_color {
    xaku_border_color_float_transparent_black = 0u,
    xaku_border_color_int_transparent_black,
    xaku_border_color_float_opaque_black,
    xaku_border_color_int_opaque_black,
    xaku_border_color_float_opaque_white,
    xaku_border_color_int_opaque_white,
};

enum xaku_sampler_address_mode {
    xaku_sampler_address_mode_wrap = 0u,
    xaku_sampler_address_mode_mirror,
    xaku_sampler_address_mode_clamp_edge,
    xaku_sampler_address_mode_clamp_border,
};

enum xaku_sampler_reduction_mode {
    xaku_sampler_reduction_mode_weighted_average = 0u,
    xaku_sampler_reduction_mode_min,
    xaku_sampler_reduction_mode_max,
};

enum xaku_sample_count_bits {
    xaku_sample_count_1     = (1u << 0),   
    xaku_sample_count_2     = (1u << 1),   
    xaku_sample_count_4     = (1u << 2),   
    xaku_sample_count_8     = (1u << 3),   
    xaku_sample_count_16    = (1u << 4),   
    xaku_sample_count_32    = (1u << 5),   
    xaku_sample_count_64    = (1u << 6),   
};

struct xaku_sampler_assembly {
    enum xaku_filter_mode               magnification_filter;
    enum xaku_filter_mode               minification_filter;
    enum xaku_filter_mode               mipmap_filter;
    enum xaku_sampler_reduction_mode    reduction_mode;
    enum xaku_sampler_address_mode      address_mode_u;
    enum xaku_sampler_address_mode      address_mode_v;
    enum xaku_sampler_address_mode      address_mode_w;
    f32                                 mip_lod_bias;
    f32                                 max_anisotrophy;
    enum xaku_compare_op                compare_op;
    f32                                 min_lod;
    f32                                 max_lod;
    enum xaku_border_color              border_color;
    bool                                enable_anisotrophy;
    bool                                enable_compare_op;
    bool                                enable_unnormalized_coordinates;
    lake_small_string                   name;
};
#define XAKU_DEFAULT_SAMPLER_ASSEMBLY { \
    .magnification_filter = xaku_filter_mode_linear, \
    .minification_filter = xaku_filter_mode_linear, \
    .mipmap_filter = xaku_filter_mode_linear, \
    .reduction_mode = xaku_sampler_reduction_mode_weighted_average, \
    .address_mode_u = xaku_sampler_address_mode_clamp_edge, \
    .address_mode_v = xaku_sampler_address_mode_clamp_edge, \
    .address_mode_w = xaku_sampler_address_mode_clamp_edge, \
    .mip_lod_bias = 0.5f, \
    .max_anisotrophy = 0.0f, \
    .compare_op = xaku_compare_op_always, \
    .min_lod = 0.0f, \
    .max_lod = 1000.0f, \
    .border_color = xaku_border_color_int_transparent_black, \
    .enable_anisotrophy = false, \
    .enable_compare_op = false, \
    .enable_unnormalized_coordinates = false, \
    .name = { .data = LAKE_ZERO_INIT, .size = 0, }, \
}

struct xaku_strided_address_region {
    u64 device_address;
    u64 stride;
    u64 size;
};

struct xaku_shader_binding_table {
    struct xaku_strided_address_region raygen_region;
    struct xaku_strided_address_region miss_region;
    struct xaku_strided_address_region hit_region;
    struct xaku_strided_address_region callable_region;
};

union xaku_clear_color_value {
    f32 vec[4];
    s32 sint[4];
    u32 uint[4];
};

struct xaku_clear_depth_stencil_value {
    f32 depth;
    u32 stencil;
};

union xaku_clear_value {
    union xaku_clear_color_value            color;
    struct xaku_clear_depth_stencil_value   depth_stencil;
};

enum xaku_geometry_flag_bits {
    xaku_geometry_flag_opaque                           = (1u << 0),
    xaku_geometry_flag_no_duplicate_any_hit_invocation  = (1u << 1),
    xaku_geometry_flag_data_is_array_of_pointers        = (1u << 2),
};

struct xaku_blas_triangle_geometry {
    enum xaku_format        vertex_format;
    u64                     vertex_data;
    u64                     vertex_stride;
    u32                     max_vertex;
    enum xaku_index_format  index_format;
    u64                     index_data;
    u64                     transform_data;
    u32                     count;
    u32                     flags; /**< enum xaku_geometry_flag_bits */
};
#define XAKU_DEFAULT_BLAS_TRIANGLE_GEOMETRY { \
    .vertex_format = xaku_format_r32g32_float, \
    .vertex_data = LAKE_ZERO_INIT, \
    .vertex_stride = 24, \
    .max_vertex = 0, \
    .index_format = xaku_index_format_u32, \
    .index_data = LAKE_ZERO_INIT, \
    .transform_data = LAKE_ZERO_INIT, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

struct xaku_blas_aabb_geometry {
    u64     data;   /**< device address */
    u64     stride;
    u32     count;
    u32     flags;  /**< enum xaku_geometry_flag_bits */
};
#define XAKU_DEFAULT_BLAS_AABB_GEOMETRY { \
    .data = LAKE_ZERO_INIT, \
    .stride = 24, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

struct xaku_blas_triangle_geometry_span {
    const struct xaku_blas_triangle_geometry   *triangles;
    usize                                       count;
};

struct xaku_blas_aabb_geometry_span {
    const struct xaku_blas_aabb_geometry       *aabbs;
    usize                                       count;
};

union xaku_blas_geometry_span {
    struct xaku_blas_triangle_geometry_span     triangle;
    struct xaku_blas_aabb_geometry_span         aabb;
};

struct xaku_tlas_instance {
    u64     data;   /**< device address */
    u32     count;
    u32     flags;  /**< enum xaku_geometry_flag_bits, data may be an array of pointers */
};
#define XAKU_DEFAULT_TLAS_INSTANCE { \
    .data = LAKE_ZERO_INIT, \
    .count = 0, \
    .flags = xaku_geometry_flag_opaque, \
}

enum xaku_acceleration_structure_assembly_flag_bits {
    xaku_acceleration_structure_assembly_flag_allow_update      = (1u << 0),
    xaku_acceleration_structure_assembly_flag_allow_compaction  = (1u << 1),
    xaku_acceleration_structure_assembly_flag_prefer_fast_trace = (1u << 2),
    xaku_acceleration_structure_assembly_flag_prefer_fast_build = (1u << 3),
    xaku_acceleration_structure_assembly_flag_low_memory        = (1u << 4),
};

struct xaku_acceleration_structure_build_sizes {
    u64 acceleration_structure_size;
    u64 update_scratch_size;
    u64 build_scratch_size;
};

struct xaku_tlas_assembly {
    u32                                 flags;  /**< enum xaku_acceleration_structure_assembly_flag_bits */
    bool                                update;
    xaku_tlas_id                        src_tlas;
    xaku_tlas_id                        dst_tlas;
    const struct xaku_tlas_instance    *instances;
    u32                                 instance_count;
    u64                                 scratch_data; /**< device address */
};
#define XAKU_DEFAULT_TLAS_ASSEMBLY { \
    .flags = xaku_acceleration_structure_assembly_flag_prefer_fast_trace, \
    .update = false, \
    .src_tlas = LAKE_ZERO_INIT, \
    .dst_tlas = LAKE_ZERO_INIT, \
    .instances = LAKE_ZERO_INIT, \
    .instance_count = LAKE_ZERO_INIT, \
    .scratch_data = LAKE_ZERO_INIT, \
}

struct xaku_buffer_tlas_assembly {
    struct xaku_tlas_assembly   tlas_assembly;
    xaku_buffer_id              buffer;
    u64                         offset;
};
#define XAKU_DEFAULT_BUFFER_TLAS_ASSEMBLY LAKE_ZERO_INIT

struct xaku_blas_assembly {
    u32                                 flags;  /**< enum xaku_acceleration_structure_assembly_flag_bits */
    bool                                update;
    xaku_blas_id                        src_blas;
    xaku_blas_id                        dst_blas;
    union xaku_blas_geometry_span       geometries;
    u64                                 scratch_data; /**< device address */
};
#define XAKU_DEFAULT_BLAS_ASSEMBLY { \
    .flags = xaku_acceleration_structure_assembly_flag_prefer_fast_trace, \
    .src_blas = LAKE_ZERO_INIT, \
    .dst_blas = LAKE_ZERO_INIT, \
    .geometries = LAKE_ZERO_INIT, \
    .scratch_data = LAKE_ZERO_INIT, \
}

struct xaku_buffer_blas_assembly {
    struct xaku_blas_assembly   blas_assembly;
    xaku_buffer_id              buffer;
    u64                         offset;
};
#define XAKU_DEFAULT_BUFFER_TLAS_ASSEMBLY LAKE_ZERO_INIT

#define XAKU_TEMPLATE_GPU_RESOURCE_POOL__MAX_RESOURCE_COUNT (1u << 20u)
#define XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_BITS          (10u)
#define XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_SIZE          (1u << XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_BITS)
#define XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_MASK          (XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_SIZE - 1u)
#define XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_COUNT         (XAKU_TEMPLATE_GPU_RESOURCE_POOL__MAX_RESOURCE_COUNT / XAKU_TEMPLATE_GPU_RESOURCE_POOL__PAGE_SIZE)
/** This template is intended to be used internally akin to a specialized memory allocator, 
 *  specific to GPU resource types (like buffers, textures), defined as 'struct <encore>_<type>_pool'.
 *
 *  This struct is threadsafe if the following assumptions are met:
 *  - never dereference a deleted resource 
 *  - never delete a resource twice
 *  That means that function <encore>_<gpu_resource>_pool_dereference_id can be used without synchronization, 
 *  even calling <encore>_<gpu_resource>: 'get_new_slot' or 'return_old_slot' in parallel is save.
 *
 *  To check if these assumptions are met at runtime, the debug define XAKU_ENABLE_GPU_ID_VALIDATION 
 *  can be enabled for additional validation at cost of performance. This may work or not per-implementation.
 *
 *  A table may be defined internally, used within a 'xaku_device' - e.g. 'struct vulkan_shader_resource_table'. */
#define XAKU_TEMPLATE_GPU_RESOURCE_POOL(ENCORE, T) \
    struct ENCORE##_##T##_pool { \
    };

#ifdef __cplusplus
}
#endif /* __cplusplus */
