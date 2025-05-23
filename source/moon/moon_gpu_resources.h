#pragma once

/** @file moon/moon_gpu_resources.h
 *  @brief Resources created and managed on the GPU.
 *
 *  Following interface procedures are defined:
 *      PFN_moon_create_buffer
 *      PFN_moon_create_buffer_from_memory_heap
 *      PFN_moon_create_texture
 *      PFN_moon_create_texture_from_memory_heap
 *      PFN_moon_create_texture_view
 *      PFN_moon_create_sampler
 *      PFN_moon_create_tlas
 *      PFN_moon_create_tlas_from_buffer
 *      PFN_moon_create_blas
 *      PFN_moon_create_blas_from_buffer
 *      PFN_moon_is_buffer_valid
 *      PFN_moon_is_texture_valid
 *      PFN_moon_is_texture_view_valid
 *      PFN_moon_is_sampler_valid
 *      PFN_moon_is_tlas_valid
 *      PFN_moon_is_blas_valid
 *      PFN_moon_buffer_host_address
 *      PFN_moon_buffer_device_address
 *      PFN_moon_tlas_device_address
 *      PFN_moon_blas_device_address
 *      PFN_moon_destroy_buffer
 *      PFN_moon_destroy_texture
 *      PFN_moon_destroy_texture_view
 *      PFN_moon_destroy_sampler
 *      PFN_moon_destroy_tlas
 *      PFN_moon_destroy_blas
 */
#include "moon_adapter.h"
#include "moon_memory_heap.h"

typedef struct {
    u64                         size;
    moon_memory_flags           memory_flags; /**< Ignored if allocating from existing heap memory. */
    u8                      pad0[7];
    lake_small_string           name;
} moon_buffer_assembly;
static constexpr moon_buffer_assembly MOON_BUFFER_ASSEMBLY_INIT = {
    .size = 0lu,
    .memory_flags = moon_memory_flag_none,
    .name = {0},
};

typedef struct {
    moon_buffer_assembly        buffer_assembly;
    moon_memory_heap            memory_heap;
    u64                         offset;
} moon_buffer_from_memory_heap_assembly;
static constexpr moon_buffer_from_memory_heap_assembly MOON_BUFFER_FROM_MEMORY_HEAP_ASSEMBLY_INIT = {
    .buffer_assembly = MOON_BUFFER_ASSEMBLY_INIT,
    .memory_heap = nullptr,
    .offset = 0lu,
};

union moon_color_value {
    f32 vec[4];
    s32 sint[4];
    u32 uint[4];
};

struct moon_depth_stencil_value {
    f32 depth;
    u32 stencil;
};

typedef enum : s8 {
    moon_border_color_float_transparent_black = 0,
    moon_border_color_int_transparent_black,
    moon_border_color_float_opaque_black,
    moon_border_color_int_opaque_black,
    moon_border_color_float_opaque_white,
    moon_border_color_int_opaque_white,
} moon_border_color;

typedef enum : s8 {
    moon_index_format_invalid = 0,
    moon_index_format_u16,
    moon_index_format_u32,
} moon_index_format;

typedef enum : s8 {
    moon_compare_op_never = 0,
    moon_compare_op_less,
    moon_compare_op_equal,
    moon_compare_op_less_or_equal,
    moon_compare_op_not_equal,
    moon_compare_op_greater,
    moon_compare_op_greater_or_equal,
    moon_compare_op_always,
} moon_compare_op;

typedef enum : s8 {
    moon_load_op_load = 0,
    moon_load_op_clear,
    moon_load_op_dont_care,
    moon_load_op_none,
} moon_load_op;

typedef enum : s8 {
    moon_store_op_store = 0,
    moon_store_op_dont_care,
    moon_store_op_none,
} moon_store_op;

typedef enum : s8 {
    moon_stencil_face_front = 0,
    moon_stencil_face_back,
} moon_stencil_face;

typedef enum : s8 {
    moon_stencil_op_keep = 0,
    moon_stencil_op_zero,
    moon_stencil_op_replace,
    moon_stencil_op_increment,
    moon_stencil_op_decrement,
    moon_stencil_op_increment_wrap,
    moon_stencil_op_decrement_wrap,
    moon_stencil_op_invert,
} moon_stencil_op;

typedef enum : s8 {
    moon_sharing_mode_exclusive = 0,
    moon_sharing_mode_concurrent,
} moon_sharing_mode;

/** A list of supported formats of textures, they describe how memory is laid out.
 *  Availability of formats depends on physical device and backend. */
typedef enum : s32 {
    moon_format_undefined = 0,
    moon_format_r4g4_unorm_pack8 = 1,
    moon_format_r4g4b4a4_unorm_pack16 = 2,
    moon_format_b4g4r4a4_unorm_pack16 = 3,
    moon_format_r5g6b5_unorm_pack16 = 4,
    moon_format_b5g6r5_unorm_pack16 = 5,
    moon_format_r5g5b5a1_unorm_pack16 = 6,
    moon_format_b5g5r5a1_unorm_pack16 = 7,
    moon_format_a1r5g5b5_unorm_pack16 = 8,
    moon_format_r8_unorm = 9,
    moon_format_r8_snorm = 10,
    moon_format_r8_uscaled = 11,
    moon_format_r8_sscaled = 12,
    moon_format_r8_uint = 13,
    moon_format_r8_sint = 14,
    moon_format_r8_srgb = 15,
    moon_format_r8g8_unorm = 16,
    moon_format_r8g8_snorm = 17,
    moon_format_r8g8_uscaled = 18,
    moon_format_r8g8_sscaled = 19,
    moon_format_r8g8_uint = 20,
    moon_format_r8g8_sint = 21,
    moon_format_r8g8_srgb = 22,
    moon_format_r8g8b8_unorm = 23,
    moon_format_r8g8b8_snorm = 24,
    moon_format_r8g8b8_uscaled = 25,
    moon_format_r8g8b8_sscaled = 26,
    moon_format_r8g8b8_uint = 27,
    moon_format_r8g8b8_sint = 28,
    moon_format_r8g8b8_srgb = 29,
    moon_format_b8g8r8_unorm = 30,
    moon_format_b8g8r8_snorm = 31,
    moon_format_b8g8r8_uscaled = 32,
    moon_format_b8g8r8_sscaled = 33,
    moon_format_b8g8r8_uint = 34,
    moon_format_b8g8r8_sint = 35,
    moon_format_b8g8r8_srgb = 36,
    moon_format_r8g8b8a8_unorm = 37,
    moon_format_r8g8b8a8_snorm = 38,
    moon_format_r8g8b8a8_uscaled = 39,
    moon_format_r8g8b8a8_sscaled = 40,
    moon_format_r8g8b8a8_uint = 41,
    moon_format_r8g8b8a8_sint = 42,
    moon_format_r8g8b8a8_srgb = 43,
    moon_format_b8g8r8a8_unorm = 44,
    moon_format_b8g8r8a8_snorm = 45,
    moon_format_b8g8r8a8_uscaled = 46,
    moon_format_b8g8r8a8_sscaled = 47,
    moon_format_b8g8r8a8_uint = 48,
    moon_format_b8g8r8a8_sint = 49,
    moon_format_b8g8r8a8_srgb = 50,
    moon_format_a8b8g8r8_unorm_pack32 = 51,
    moon_format_a8b8g8r8_snorm_pack32 = 52,
    moon_format_a8b8g8r8_uscaled_pack32 = 53,
    moon_format_a8b8g8r8_sscaled_pack32 = 54,
    moon_format_a8b8g8r8_uint_pack32 = 55,
    moon_format_a8b8g8r8_sint_pack32 = 56,
    moon_format_a8b8g8r8_srgb_pack32 = 57,
    moon_format_a2r10g10b10_unorm_pack32 = 58,
    moon_format_a2r10g10b10_snorm_pack32 = 59,
    moon_format_a2r10g10b10_uscaled_pack32 = 60,
    moon_format_a2r10g10b10_sscaled_pack32 = 61,
    moon_format_a2r10g10b10_uint_pack32 = 62,
    moon_format_a2r10g10b10_sint_pack32 = 63,
    moon_format_a2b10g10r10_unorm_pack32 = 64,
    moon_format_a2b10g10r10_snorm_pack32 = 65,
    moon_format_a2b10g10r10_uscaled_pack32 = 66,
    moon_format_a2b10g10r10_sscaled_pack32 = 67,
    moon_format_a2b10g10r10_uint_pack32 = 68,
    moon_format_a2b10g10r10_sint_pack32 = 69,
    moon_format_r16_unorm = 70,
    moon_format_r16_snorm = 71,
    moon_format_r16_uscaled = 72,
    moon_format_r16_sscaled = 73,
    moon_format_r16_uint = 74,
    moon_format_r16_sint = 75,
    moon_format_r16_sfloat = 76,
    moon_format_r16g16_unorm = 77,
    moon_format_r16g16_snorm = 78,
    moon_format_r16g16_uscaled = 79,
    moon_format_r16g16_sscaled = 80,
    moon_format_r16g16_uint = 81,
    moon_format_r16g16_sint = 82,
    moon_format_r16g16_sfloat = 83,
    moon_format_r16g16b16_unorm = 84,
    moon_format_r16g16b16_snorm = 85,
    moon_format_r16g16b16_uscaled = 86,
    moon_format_r16g16b16_sscaled = 87,
    moon_format_r16g16b16_uint = 88,
    moon_format_r16g16b16_sint = 89,
    moon_format_r16g16b16_sfloat = 90,
    moon_format_r16g16b16a16_unorm = 91,
    moon_format_r16g16b16a16_snorm = 92,
    moon_format_r16g16b16a16_uscaled = 93,
    moon_format_r16g16b16a16_sscaled = 94,
    moon_format_r16g16b16a16_uint = 95,
    moon_format_r16g16b16a16_sint = 96,
    moon_format_r16g16b16a16_sfloat = 97,
    moon_format_r32_uint = 98,
    moon_format_r32_sint = 99,
    moon_format_r32_sfloat = 100,
    moon_format_r32g32_uint = 101,
    moon_format_r32g32_sint = 102,
    moon_format_r32g32_sfloat = 103,
    moon_format_r32g32b32_uint = 104,
    moon_format_r32g32b32_sint = 105,
    moon_format_r32g32b32_sfloat = 106,
    moon_format_r32g32b32a32_uint = 107,
    moon_format_r32g32b32a32_sint = 108,
    moon_format_r32g32b32a32_sfloat = 109,
    moon_format_r64_uint = 110,
    moon_format_r64_sint = 111,
    moon_format_r64_sfloat = 112,
    moon_format_r64g64_uint = 113,
    moon_format_r64g64_sint = 114,
    moon_format_r64g64_sfloat = 115,
    moon_format_r64g64b64_uint = 116,
    moon_format_r64g64b64_sint = 117,
    moon_format_r64g64b64_sfloat = 118,
    moon_format_r64g64b64a64_uint = 119,
    moon_format_r64g64b64a64_sint = 120,
    moon_format_r64g64b64a64_sfloat = 121,
    moon_format_b10g11r11_ufloat_pack32 = 122,
    moon_format_e5b9g9r9_ufloat_pack32 = 123,
    moon_format_d16_unorm = 124,
    moon_format_x8_d24_unorm_pack32 = 125,
    moon_format_d32_sfloat = 126,
    moon_format_s8_uint = 127,
    moon_format_d16_unorm_s8_uint = 128,
    moon_format_d24_unorm_s8_uint = 129,
    moon_format_d32_sfloat_s8_uint = 130,
    moon_format_bc1_rgb_unorm_block = 131,
    moon_format_bc1_rgb_srgb_block = 132,
    moon_format_bc1_rgba_unorm_block = 133,
    moon_format_bc1_rgba_srgb_block = 134,
    moon_format_bc2_unorm_block = 135,
    moon_format_bc2_srgb_block = 136,
    moon_format_bc3_unorm_block = 137,
    moon_format_bc3_srgb_block = 138,
    moon_format_bc4_unorm_block = 139,
    moon_format_bc4_snorm_block = 140,
    moon_format_bc5_unorm_block = 141,
    moon_format_bc5_snorm_block = 142,
    moon_format_bc6h_ufloat_block = 143,
    moon_format_bc6h_sfloat_block = 144,
    moon_format_bc7_unorm_block = 145,
    moon_format_bc7_srgb_block = 146,
    moon_format_etc2_r8g8b8_unorm_block = 147,
    moon_format_etc2_r8g8b8_srgb_block = 148,
    moon_format_etc2_r8g8b8a1_unorm_block = 149,
    moon_format_etc2_r8g8b8a1_srgb_block = 150,
    moon_format_etc2_r8g8b8a8_unorm_block = 151,
    moon_format_etc2_r8g8b8a8_srgb_block = 152,
    moon_format_eac_r11_unorm_block = 153,
    moon_format_eac_r11_snorm_block = 154,
    moon_format_eac_r11g11_unorm_block = 155,
    moon_format_eac_r11g11_snorm_block = 156,
    moon_format_astc_4x4_unorm_block = 157,
    moon_format_astc_4x4_srgb_block = 158,
    moon_format_astc_5x4_unorm_block = 159,
    moon_format_astc_5x4_srgb_block = 160,
    moon_format_astc_5x5_unorm_block = 161,
    moon_format_astc_5x5_srgb_block = 162,
    moon_format_astc_6x5_unorm_block = 163,
    moon_format_astc_6x5_srgb_block = 164,
    moon_format_astc_6x6_unorm_block = 165,
    moon_format_astc_6x6_srgb_block = 166,
    moon_format_astc_8x5_unorm_block = 167,
    moon_format_astc_8x5_srgb_block = 168,
    moon_format_astc_8x6_unorm_block = 169,
    moon_format_astc_8x6_srgb_block = 170,
    moon_format_astc_8x8_unorm_block = 171,
    moon_format_astc_8x8_srgb_block = 172,
    moon_format_astc_10x5_unorm_block = 173,
    moon_format_astc_10x5_srgb_block = 174,
    moon_format_astc_10x6_unorm_block = 175,
    moon_format_astc_10x6_srgb_block = 176,
    moon_format_astc_10x8_unorm_block = 177,
    moon_format_astc_10x8_srgb_block = 178,
    moon_format_astc_10x10_unorm_block = 179,
    moon_format_astc_10x10_srgb_block = 180,
    moon_format_astc_12x10_unorm_block = 181,
    moon_format_astc_12x10_srgb_block = 182,
    moon_format_astc_12x12_unorm_block = 183,
    moon_format_astc_12x12_srgb_block = 184,
    moon_format_g8b8g8r8_422_unorm = 1000156000,
    moon_format_b8g8r8g8_422_unorm = 1000156001,
    moon_format_g8_b8_r8_3plane_420_unorm = 1000156002,
    moon_format_g8_b8r8_2plane_420_unorm = 1000156003,
    moon_format_g8_b8_r8_3plane_422_unorm = 1000156004,
    moon_format_g8_b8r8_2plane_422_unorm = 1000156005,
    moon_format_g8_b8_r8_3plane_444_unorm = 1000156006,
    moon_format_r10x6_unorm_pack16 = 1000156007,
    moon_format_r10x6g10x6_unorm_2pack16 = 1000156008,
    moon_format_r10x6g10x6b10x6a10x6_unorm_4pack16 = 1000156009,
    moon_format_g10x6b10x6g10x6r10x6_422_unorm_4pack16 = 1000156010,
    moon_format_b10x6g10x6r10x6g10x6_422_unorm_4pack16 = 1000156011,
    moon_format_g10x6_b10x6_r10x6_3plane_420_unorm_3pack16 = 1000156012,
    moon_format_g10x6_b10x6r10x6_2plane_420_unorm_3pack16 = 1000156013,
    moon_format_g10x6_b10x6_r10x6_3plane_422_unorm_3pack16 = 1000156014,
    moon_format_g10x6_b10x6r10x6_2plane_422_unorm_3pack16 = 1000156015,
    moon_format_g10x6_b10x6_r10x6_3plane_444_unorm_3pack16 = 1000156016,
    moon_format_r12x4_unorm_pack16 = 1000156017,
    moon_format_r12x4g12x4_unorm_2pack16 = 1000156018,
    moon_format_r12x4g12x4b12x4a12x4_unorm_4pack16 = 1000156019,
    moon_format_g12x4b12x4g12x4r12x4_422_unorm_4pack16 = 1000156020,
    moon_format_b12x4g12x4r12x4g12x4_422_unorm_4pack16 = 1000156021,
    moon_format_g12x4_b12x4_r12x4_3plane_420_unorm_3pack16 = 1000156022,
    moon_format_g12x4_b12x4r12x4_2plane_420_unorm_3pack16 = 1000156023,
    moon_format_g12x4_b12x4_r12x4_3plane_422_unorm_3pack16 = 1000156024,
    moon_format_g12x4_b12x4r12x4_2plane_422_unorm_3pack16 = 1000156025,
    moon_format_g12x4_b12x4_r12x4_3plane_444_unorm_3pack16 = 1000156026,
    moon_format_g16b16g16r16_422_unorm = 1000156027,
    moon_format_b16g16r16g16_422_unorm = 1000156028,
    moon_format_g16_b16_r16_3plane_420_unorm = 1000156029,
    moon_format_g16_b16r16_2plane_420_unorm = 1000156030,
    moon_format_g16_b16_r16_3plane_422_unorm = 1000156031,
    moon_format_g16_b16r16_2plane_422_unorm = 1000156032,
    moon_format_g16_b16_r16_3plane_444_unorm = 1000156033,
    moon_format_g8_b8r8_2plane_444_unorm = 1000330000,
    moon_format_g10x6_b10x6r10x6_2plane_444_unorm_3pack16 = 1000330001,
    moon_format_g12x4_b12x4r12x4_2plane_444_unorm_3pack16 = 1000330002,
    moon_format_g16_b16r16_2plane_444_unorm = 1000330003,
    moon_format_a4r4g4b4_unorm_pack16 = 1000340000,
    moon_format_a4b4g4r4_unorm_pack16 = 1000340001,
    moon_format_astc_4x4_sfloat_block = 1000066000,
    moon_format_astc_5x4_sfloat_block = 1000066001,
    moon_format_astc_5x5_sfloat_block = 1000066002,
    moon_format_astc_6x5_sfloat_block = 1000066003,
    moon_format_astc_6x6_sfloat_block = 1000066004,
    moon_format_astc_8x5_sfloat_block = 1000066005,
    moon_format_astc_8x6_sfloat_block = 1000066006,
    moon_format_astc_8x8_sfloat_block = 1000066007,
    moon_format_astc_10x5_sfloat_block = 1000066008,
    moon_format_astc_10x6_sfloat_block = 1000066009,
    moon_format_astc_10x8_sfloat_block = 1000066010,
    moon_format_astc_10x10_sfloat_block = 1000066011,
    moon_format_astc_12x10_sfloat_block = 1000066012,
    moon_format_astc_12x12_sfloat_block = 1000066013,
    moon_format_pvrtc1_2bpp_unorm_block_img = 1000054000,
    moon_format_pvrtc1_4bpp_unorm_block_img = 1000054001,
    moon_format_pvrtc2_2bpp_unorm_block_img = 1000054002,
    moon_format_pvrtc2_4bpp_unorm_block_img = 1000054003,
    moon_format_pvrtc1_2bpp_srgb_block_img = 1000054004,
    moon_format_pvrtc1_4bpp_srgb_block_img = 1000054005,
    moon_format_pvrtc2_2bpp_srgb_block_img = 1000054006,
    moon_format_PVRTc2_4bpp_srgb_block_img = 1000054007,
} moon_format;

typedef enum : s32 {
    moon_color_space_srgb_nonlinear             = 0,
    moon_color_space_display_p3_nonlinear       = 1000104001,
    moon_color_space_extended_srgb_linear       = 1000104002,
    moon_color_space_display_p3_linear          = 1000104003,
    moon_color_space_dci_p3_nonlinear           = 1000104004,
    moon_color_space_bt709_linear               = 1000104005,
    moon_color_space_bt709_nonlinear            = 1000104006,
    moon_color_space_bt2020_linear              = 1000104007,
    moon_color_space_hdr10_st2084               = 1000104008,
    moon_color_space_dolbyvision                = 1000104009,
    moon_color_space_hdr10_hlg                  = 1000104010,
    moon_color_space_adobergb_linear            = 1000104011,
    moon_color_space_adobergb_nonlinear         = 1000104012,
    moon_color_space_pass_through               = 1000104013,
    moon_color_space_extended_srgb_nonlinear    = 1000104014,
    moon_color_space_display_native_amd         = 1000213000,
} moon_color_space;

typedef u8 moon_texture_flags;
typedef enum : moon_texture_flags {
    moon_texture_flag_none                  = 0,
    moon_texture_flag_allow_mutable_format  = (1u << 0),
    moon_texture_flag_compatible_cube       = (1u << 1),
    moon_texture_flag_compatible_2d_array   = (1u << 2),
    moon_texture_flag_allow_alias           = (1u << 3),
} moon_texture_flag_bits;

typedef u16 moon_texture_aspect;
typedef enum : moon_texture_aspect {
    moon_texture_aspect_none            = (1u << 0),
    moon_texture_aspect_color           = (1u << 1),
    moon_texture_aspect_depth           = (1u << 2),
    moon_texture_aspect_stencil         = (1u << 3),
    moon_texture_aspect_metadata        = (1u << 4),
    moon_texture_aspect_plane_0         = (1u << 5),
    moon_texture_aspect_plane_1         = (1u << 6),
    moon_texture_aspect_plane_2         = (1u << 7),
    moon_texture_aspect_memory_plane_0  = (1u << 8),
    moon_texture_aspect_memory_plane_1  = (1u << 9),
    moon_texture_aspect_memory_plane_2  = (1u << 10),
    moon_texture_aspect_memory_plane_3  = (1u << 11),
} moon_texture_aspect_bits;

typedef u16 moon_texture_usage;
typedef enum : moon_texture_usage {
    moon_texture_usage_none                             = 0,
    moon_texture_usage_transfer_src                     = (1u << 0),
    moon_texture_usage_transfer_dst                     = (1u << 1),
    moon_texture_usage_shader_sampled                   = (1u << 2),
    moon_texture_usage_shader_storage                   = (1u << 3),
    moon_texture_usage_color_attachment                 = (1u << 4),
    moon_texture_usage_depth_stencil_attachment         = (1u << 5),
    moon_texture_usage_transient_attachment             = (1u << 6),
    moon_texture_usage_fragment_density_map             = (1u << 7),
    moon_texture_usage_fragment_shading_rate_attachment = (1u << 8),
} moon_texture_usage_bits;

typedef enum : s8 {
    moon_texture_layout_undefined = 0,
    moon_texture_layout_general,
    moon_texture_layout_transfer_src_optimal,
    moon_texture_layout_transfer_dst_optimal,
    moon_texture_layout_read_only_optimal,
    moon_texture_layout_attachment_optimal,
    moon_texture_layout_present_src,
} moon_texture_layout;

typedef u8 moon_resolve_mode;
typedef enum : moon_resolve_mode {
    moon_resolve_mode_none                          = 0,
    moon_resolve_mode_sample_zero                   = (1u << 0),
    moon_resolve_mode_average                       = (1u << 1),
    moon_resolve_mode_min                           = (1u << 2),
    moon_resolve_mode_max                           = (1u << 3),
    moon_resolve_mode_external_downsample_android   = (1u << 4),
} moon_resolve_mode_bits;

typedef struct {
    u32 base_mip_level;
    u32 level_count;
    u32 base_array_layer;
    u32 layer_count;
} moon_texture_mip_array_slice;

typedef struct {
    u32 mip_level;
    u32 base_array_layer;
    u32 layer_count;
} moon_texture_array_slice;

typedef struct {
    u32 mip_level;
    u32 array_layer;
} moon_texture_slice;

typedef struct {
    moon_texture_usage      usage;
    moon_texture_flags      flags;
    moon_memory_flags       memory_flags; /** Ignored if allocating from existing memory heap. */
    moon_sharing_mode       sharing_mode;
    u8                      dimensions;
    u8                  pad0[6];
    moon_format             format;
    lake_extent3d           extent;
    u32                     mip_level_count;
    u32                     array_layer_count;
    u32                     sample_count;
    lake_small_string       name;
} moon_texture_assembly;
static constexpr moon_texture_assembly MOON_TEXTURE_ASSEMBLY_INIT = {
    .usage = moon_texture_usage_none,
    .flags = moon_texture_flag_none,
    .memory_flags = moon_memory_flag_none,
    .sharing_mode = moon_sharing_mode_concurrent,
    .dimensions = 2,
    .format = moon_format_r8g8b8a8_srgb,
    .extent = {0u, 0u, 0u},
    .mip_level_count = 1,
    .array_layer_count = 1,
    .sample_count = 1,
    .name = {0},
};

typedef struct {
    moon_texture_assembly   texture_assembly;
    moon_memory_heap        memory_heap;
    u64                     offset;
} moon_texture_from_memory_heap_assembly;
static constexpr moon_texture_from_memory_heap_assembly MOON_TEXTURE_FROM_MEMORY_HEAP_ASSEMBLY_INIT = {
    .texture_assembly = MOON_TEXTURE_ASSEMBLY_INIT,
    .memory_heap = nullptr,
    .offset = 0lu,
};

typedef enum : s8 {
    moon_texture_view_type_1d = 0,
    moon_texture_view_type_2d,
    moon_texture_view_type_3d,
    moon_texture_view_type_cube,
    moon_texture_view_type_1d_array,
    moon_texture_view_type_2d_array,
    moon_texture_view_type_cube_array,
} moon_texture_view_type;

typedef struct {
    moon_format                     format;
    moon_texture_view_type          type;
    u8                          pad0[3];
    moon_texture_mip_array_slice    slice;
    moon_texture_id                 texture;
    lake_small_string               name;
} moon_texture_view_assembly;
static constexpr moon_texture_view_assembly MOON_TEXTURE_VIEW_ASSEMBLY_INIT = {
    .format = moon_format_r8g8b8a8_srgb,
    .type = moon_texture_view_type_2d,
    .slice = {0},
    .texture = {0},
    .name = {0},
};

typedef enum : s8 {
    moon_filter_mode_nearest = 0,
    moon_filter_mode_linear,
} moon_filter_mode;

typedef enum : s8 {
    moon_sampler_address_mode_wrap = 0,
    moon_sampler_address_mode_mirror,
    moon_sampler_address_mode_clamp_edge,
    moon_sampler_address_mode_clamp_border,
} moon_sampler_address_mode;

typedef enum : s8 {
    moon_sampler_reduction_mode_weighted_average = 0,
    moon_sampler_reduction_mode_min,
    moon_sampler_reduction_mode_max,
} moon_sampler_reduction_mode;

typedef u8 moon_sample_count;
typedef enum : moon_sample_count {
    moon_sample_count_1     = (1u << 0),   
    moon_sample_count_2     = (1u << 1),   
    moon_sample_count_4     = (1u << 2),   
    moon_sample_count_8     = (1u << 3),   
    moon_sample_count_16    = (1u << 4),   
    moon_sample_count_32    = (1u << 5),   
    moon_sample_count_64    = (1u << 6),   
} moon_sample_count_bits;

typedef struct {
    moon_filter_mode            magnification_filter;
    moon_filter_mode            minification_filter;
    moon_filter_mode            mipmap_filter;
    moon_sampler_reduction_mode reduction_mode;
    moon_sampler_address_mode   address_mode_u;
    moon_sampler_address_mode   address_mode_v;
    moon_sampler_address_mode   address_mode_w;
    moon_compare_op             compare_op;
    f32                         mip_lod_bias;
    f32                         min_lod;
    f32                         max_lod;
    f32                         max_anisotrophy;
    moon_border_color           border_color;
    bool                        enable_anisotrophy;
    bool                        enable_compare_op;
    bool                        enable_unnormalized_coordinates;
    lake_small_string           name;
} moon_sampler_assembly;
static constexpr moon_sampler_assembly MOON_SAMPLER_ASSEMBLY_INIT = {
    .magnification_filter = moon_filter_mode_linear,
    .minification_filter = moon_filter_mode_linear,
    .mipmap_filter = moon_filter_mode_linear,
    .reduction_mode = moon_sampler_reduction_mode_weighted_average,
    .address_mode_u = moon_sampler_address_mode_clamp_edge,
    .address_mode_v = moon_sampler_address_mode_clamp_edge,
    .address_mode_w = moon_sampler_address_mode_clamp_edge,
    .compare_op = moon_compare_op_always,
    .mip_lod_bias = 0.5f,
    .min_lod = 0.0f,
    .max_lod = 1000.0f,
    .max_anisotrophy = 0.0f,
    .border_color = moon_border_color_int_transparent_black,
    .enable_anisotrophy = false,
    .enable_compare_op = false,
    .enable_unnormalized_coordinates = false,
    .name = {0},
};

typedef struct {
    moon_texture_view_id        texture_view;
    moon_sampler_id             sampler;
} moon_sampled_texture;

typedef struct {
    moon_device_address         data;
    u64                         stride;
    u64                         size;
} moon_strided_address_region;

typedef struct {
    moon_strided_address_region raygen_region;
    moon_strided_address_region miss_region;
    moon_strided_address_region hit_region;
    moon_strided_address_region callable_region;
} moon_shader_binding_table;

typedef u8 moon_geometry_flags;
typedef enum : moon_geometry_flags {
    moon_geometry_flag_opaque                           = (1u << 0),
    moon_geometry_flag_no_duplicate_any_hit_invocation  = (1u << 1),
} moon_geometry_flag_bits;

typedef struct {
    moon_device_address vertex_data;
    u64                 vertex_stride;
    moon_format         vertex_format;
    u32                 max_vertex;
    moon_device_address transform_data;
    moon_device_address index_data;
    moon_index_format   index_format;
    moon_geometry_flags geometry_flags;
    u8              pad0[2];
    u32                 count;
} moon_blas_triangle_geometry;
static constexpr moon_blas_triangle_geometry MOON_BLAS_TRIANGLE_GEOMETRY_INIT = {
    .vertex_data = 0,
    .vertex_stride = 12,
    .vertex_format = moon_format_r32g32b32_sfloat,
    .max_vertex = 0,
    .transform_data = 0,
    .index_data = 0,
    .index_format = moon_index_format_u32,
};

typedef struct {
    moon_device_address data;
    u64                 stride;
    moon_geometry_flags geometry_flags;
    u8              pad0[3];
    u32                 count;
} moon_blas_aabb_geometry;
static constexpr moon_blas_aabb_geometry MOON_BLAS_AABB_GEOMETRY_INIT = {
    .data = 0,
    .stride = 12,
    .geometry_flags = moon_geometry_flag_opaque,
    .count = 0,
};

typedef lake_span_t(moon_blas_triangle_geometry const) moon_blas_triangle_geometry_span;
typedef lake_span_t(moon_blas_aabb_geometry const) moon_blas_aabb_geometry_span;
typedef union {
    moon_blas_triangle_geometry_span    triangle;
    moon_blas_aabb_geometry_span        aabb;
} moon_blas_geometry_spans;

typedef struct {
    moon_device_address                 data;
    bool                                data_is_array_of_pointers;
    moon_geometry_flags                 geometry_flags;
    u8                              pad0[2];
    u32                                 count;
} moon_tlas_instance;
static constexpr moon_tlas_instance MOON_TLAS_INSTANCE_INIT = {
    .data = 0,
    .data_is_array_of_pointers = false,
    .geometry_flags = moon_geometry_flag_opaque,
    .count = 0,
};

typedef u8 moon_build_acceleration_structure_flags;
typedef enum : moon_build_acceleration_structure_flags {
    moon_build_acceleration_structure_flag_allow_update         = (1u << 0),
    moon_build_acceleration_structure_flag_allow_compaction     = (1u << 1),
    moon_build_acceleration_structure_flag_prefer_fast_trace    = (1u << 2),
    moon_build_acceleration_structure_flag_prefer_fast_build    = (1u << 3),
    moon_build_acceleration_structure_flag_low_memory           = (1u << 4),
} moon_build_acceleration_structure_flag_bits;

typedef struct {
    u64 acceleration_structure_size;
    u64 update_scratch_size;
    u64 build_scratch_size;
} moon_build_acceleration_structure_sizes;

typedef struct {
    moon_tlas_id                            src_tlas;
    moon_tlas_id                            dst_tlas;
    moon_tlas_instance const               *instances;
    u32                                     instance_count;
    moon_build_acceleration_structure_flags flags;
    bool                                    update;
    u8                                  pad0[2];
    moon_device_address                     scratch_data;
} moon_tlas_assembly;
static constexpr moon_tlas_assembly MOON_TLAS_ASSEMBLY_INIT = {
    .src_tlas = {0},
    .dst_tlas = {0},
    .instances = nullptr,
    .instance_count = 0,
    .flags = moon_build_acceleration_structure_flag_prefer_fast_trace,
    .update = 0,
    .scratch_data = 0,
};

typedef struct {
    moon_tlas_assembly      tlas_assembly;
    moon_buffer_id          buffer;
    u64                     offset;
} moon_tlas_from_buffer_assembly;
static constexpr moon_tlas_from_buffer_assembly MOON_TLAS_FROM_BUFFER_ASSEMBLY_INIT = {
    .tlas_assembly = MOON_TLAS_ASSEMBLY_INIT,
    .buffer = {0},
    .offset = 0,
};

typedef struct {
    moon_blas_id                            src_blas;
    moon_blas_id                            dst_blas;
    moon_blas_geometry_spans                geometries;
    u8                                      geometry_variant;
    moon_build_acceleration_structure_flags flags;
    bool                                    update;
    u8                                  pad0[5];
    moon_device_address                     scratch_data;
} moon_blas_assembly;
static constexpr moon_blas_assembly MOON_BLAS_ASSEMBLY_INIT = {
    .src_blas = {0},
    .dst_blas = {0},
    .geometries = {0},
    .geometry_variant = 0,
    .flags = moon_build_acceleration_structure_flag_prefer_fast_trace,
    .update = 0,
    .scratch_data = 0,
};

typedef struct {
    moon_blas_assembly      blas_assembly;
    moon_buffer_id          buffer;
    u64                     offset;
} moon_blas_from_buffer_assembly;
static constexpr moon_blas_from_buffer_assembly MOON_BLAS_FROM_BUFFER_ASSEMBLY_INIT = {
    .blas_assembly = MOON_BLAS_ASSEMBLY_INIT,
    .buffer = {0},
    .offset = 0,
};

typedef struct {
    u64                     size;
    lake_small_string       name;
} moon_acceleration_structure;

/** Create a buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer)(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer(moon_device device, moon_buffer_assembly const *assembly, moon_buffer_id *out_buffer)

/** Create a buffer from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_buffer_from_memory_heap)(moon_device device, moon_buffer_from_memory_heap_assembly const *assembly, moon_buffer_id *out_buffer);
#define FN_MOON_CREATE_BUFFER_FROM_MEMORY_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_buffer_from_memory_heap(moon_device device, moon_buffer_from_memory_heap_assembly const *assembly, moon_buffer_id *out_buffer)

/** Create a texture on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture)(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture(moon_device device, moon_texture_assembly const *assembly, moon_texture_id *out_texture)

/** Create a texture from memory on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_from_memory_heap)(moon_device device, moon_texture_from_memory_heap_assembly const *assembly, moon_texture_id *out_texture);
#define FN_MOON_CREATE_TEXTURE_FROM_MEMORY_HEAP(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_from_memory_heap(moon_device device, moon_texture_from_memory_heap_assembly const *assembly, moon_texture_id *out_texture)

/** Create a texture view on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_texture_view)(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view);
#define FN_MOON_CREATE_TEXTURE_VIEW(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_texture_view(moon_device device, moon_texture_view_assembly const *assembly, moon_texture_view_id *out_texture_view)

/** Create a sampler on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_sampler)(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler);
#define FN_MOON_CREATE_SAMPLER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_sampler(moon_device device, moon_sampler_assembly const *assembly, moon_sampler_id *out_sampler)

/** Create a top-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas)(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas(moon_device device, moon_tlas_assembly const *assembly, moon_tlas_id *out_tlas)

/** Create a top-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_tlas_from_buffer)(moon_device device, moon_tlas_from_buffer_assembly const *assembly, moon_tlas_id *out_tlas);
#define FN_MOON_CREATE_TLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_tlas_from_buffer(moon_device device, moon_tlas_from_buffer_assembly const *assembly, moon_tlas_id *out_tlas)

/** Create a bottom-level on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas)(moon_device device, moon_tlas_assembly const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas(moon_device device, moon_tlas_assembly const *assembly, moon_blas_id *out_blas)

/** Create a bottom-level from buffer on device. The handle allows for one-to-many cardinality. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_create_blas_from_buffer)(moon_device device, moon_blas_from_buffer_assembly const *assembly, moon_blas_id *out_blas);
#define FN_MOON_CREATE_BLAS_FROM_BUFFER(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_create_blas_from_buffer(moon_device device, moon_blas_from_buffer_assembly const *assembly, moon_blas_id *out_blas)

/** The buffer handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_buffer_valid)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_IS_BUFFER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_buffer_valid(moon_device device, moon_buffer_id buffer)

/** The texture handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_valid)(moon_device device, moon_texture_id texture);
#define FN_MOON_IS_TEXTURE_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_valid(moon_device device, moon_texture_id texture)

/** The texture view handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_texture_view_valid)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_IS_TEXTURE_VIEW_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_texture_view_valid(moon_device device, moon_texture_view_id texture_view)

/** The sampler handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_sampler_valid)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_IS_SAMPLER_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_sampler_valid(moon_device device, moon_sampler_id sampler)

/** The tlas-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_tlas_valid)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_IS_TLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_tlas_valid(moon_device device, moon_tlas_id tlas)

/** The bottom-level handle is valid as long as it was created by the device and not yet destroyed. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_moon_is_blas_valid)(moon_device device, moon_blas_id blas);
#define FN_MOON_IS_BLAS_VALID(backend) \
    LAKE_NODISCARD bool LAKECALL _moon_##backend##_is_blas_valid(moon_device device, moon_blas_id blas)

/** Returns the host address of a mapped buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_host_address)(moon_device device, moon_buffer_id buffer, void **out_host_address);
#define FN_MOON_BUFFER_HOST_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_host_address(moon_device device, moon_buffer_id buffer, void **out_host_address)

/** Returns the device address of a buffer handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_buffer_device_address)(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address);
#define FN_MOON_BUFFER_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_buffer_device_address(moon_device device, moon_buffer_id buffer, moon_device_address *out_device_address)

/** Returns the device address of an top-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_tlas_device_address)(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address);
#define FN_MOON_TLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_tlas_device_address(moon_device device, moon_tlas_id tlas, moon_device_address *out_device_address)

/** Returns the device address of an bottom-level acceleration structure handle loaded in a given device. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_moon_blas_device_address)(moon_device device, moon_blas_id blas, moon_device_address *out_device_address);
#define FN_MOON_BLAS_DEVICE_ADDRESS(backend) \
    LAKE_NODISCARD lake_result LAKECALL _moon_##backend##_blas_device_address(moon_device device, moon_blas_id blas, moon_device_address *out_device_address)

/** The buffer will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_buffer)(moon_device device, moon_buffer_id buffer);
#define FN_MOON_DESTROY_BUFFER(backend) \
    void LAKECALL _moon_##backend##_destroy_buffer(moon_device device, moon_buffer_id buffer)

/** The texture will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture)(moon_device device, moon_texture_id texture);
#define FN_MOON_DESTROY_TEXTURE(backend) \
    void LAKECALL _moon_##backend##_destroy_texture(moon_device device, moon_texture_id texture)

/** The texture view will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_texture_view)(moon_device device, moon_texture_view_id texture_view);
#define FN_MOON_DESTROY_TEXTURE_VIEW(backend) \
    void LAKECALL _moon_##backend##_destroy_texture_view(moon_device device, moon_texture_view_id texture_view)

/** The sampler will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_sampler)(moon_device device, moon_sampler_id sampler);
#define FN_MOON_DESTROY_SAMPLER(backend) \
    void LAKECALL _moon_##backend##_destroy_sampler(moon_device device, moon_sampler_id sampler)

/** The top-level will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_tlas)(moon_device device, moon_tlas_id tlas);
#define FN_MOON_DESTROY_TLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_tlas(moon_device device, moon_tlas_id tlas)

/** The bottom-level will be zombified and destroyed after `PFN_moon_device_destroy_deferred` is called. */
typedef void (LAKECALL *PFN_moon_destroy_blas)(moon_device device, moon_blas_id blas);
#define FN_MOON_DESTROY_BLAS(backend) \
    void LAKECALL _moon_##backend##_destroy_blas(moon_device device, moon_blas_id blas)
