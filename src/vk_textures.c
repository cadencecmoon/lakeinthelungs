#include "vk_reznor.h"

VkFormat vulkan_texture_format_translate(enum reznor_texture_format format)
{
    switch (format) {
        case reznor_texture_format_r1_unorm:            return VK_FORMAT_UNDEFINED;
        case reznor_texture_format_a8_unorm:            return VK_FORMAT_A8_UNORM;
        /* depth formats */
        case reznor_texture_format_d16_unorm:           return VK_FORMAT_D16_UNORM;
        case reznor_texture_format_d16s8_unorm:         return VK_FORMAT_D16_UNORM_S8_UINT;
        case reznor_texture_format_d24s8_unorm:         return VK_FORMAT_D24_UNORM_S8_UINT;
        case reznor_texture_format_d32_float:           return VK_FORMAT_D32_SFLOAT;
        case reznor_texture_format_d0s8_unorm:          return VK_FORMAT_S8_UINT;
        /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
        case reznor_texture_format_r8_snorm:            return VK_FORMAT_R8_SNORM;
        case reznor_texture_format_r8_unorm:            return VK_FORMAT_R8_UNORM;
        case reznor_texture_format_r8_sint:             return VK_FORMAT_R8_SINT;
        case reznor_texture_format_r8_uint:             return VK_FORMAT_R8_UINT;
        /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
        case reznor_texture_format_r16_float:           return VK_FORMAT_R16_SFLOAT;
        case reznor_texture_format_r16_snorm:           return VK_FORMAT_R16_SNORM;
        case reznor_texture_format_r16_unorm:           return VK_FORMAT_R16_UNORM;
        case reznor_texture_format_r16_sint:            return VK_FORMAT_R16_SINT;
        case reznor_texture_format_r16_uint:            return VK_FORMAT_R16_UINT;
        case reznor_texture_format_r8g8_snorm:          return VK_FORMAT_R8G8_SNORM;
        case reznor_texture_format_r8g8_unorm:          return VK_FORMAT_R8G8_UNORM;
        case reznor_texture_format_r8g8_sint:           return VK_FORMAT_R8G8_SINT;
        case reznor_texture_format_r8g8_uint:           return VK_FORMAT_R8G8_UINT;
        /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
        case reznor_texture_format_r8g8b8_snorm:        return VK_FORMAT_R8G8B8_SNORM;
        case reznor_texture_format_r8g8b8_unorm:        return VK_FORMAT_R8G8B8_UNORM;
        case reznor_texture_format_r8g8b8_sint:         return VK_FORMAT_R8G8B8_SINT;
        case reznor_texture_format_r8g8b8_uint:         return VK_FORMAT_R8G8B8_UINT;
        /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
        case reznor_texture_format_r32_float:           return VK_FORMAT_R32_SFLOAT;
        case reznor_texture_format_r32_sint:            return VK_FORMAT_R32_SINT;
        case reznor_texture_format_r32_uint:            return VK_FORMAT_R32_UINT;
        case reznor_texture_format_r16g16_float:        return VK_FORMAT_R16G16_SFLOAT;
        case reznor_texture_format_r16g16_snorm:        return VK_FORMAT_R16G16_SNORM;
        case reznor_texture_format_r16g16_unorm:        return VK_FORMAT_R16G16_UNORM;
        case reznor_texture_format_r16g16_sint:         return VK_FORMAT_R16G16_SINT;
        case reznor_texture_format_r16g16_uint:         return VK_FORMAT_R16G16_UINT;
        case reznor_texture_format_r8g8b8a8_snorm:      return VK_FORMAT_R8G8B8A8_SNORM;
        case reznor_texture_format_r8g8b8a8_unorm:      return VK_FORMAT_R8G8B8A8_UNORM;
        case reznor_texture_format_r8g8b8a8_sint:       return VK_FORMAT_R8G8B8A8_SINT;
        case reznor_texture_format_r8g8b8a8_uint:       return VK_FORMAT_R8G8B8A8_UINT;
        case reznor_texture_format_b8g8r8a8_snorm:      return VK_FORMAT_B8G8R8A8_SNORM;
        case reznor_texture_format_b8g8r8a8_unorm:      return VK_FORMAT_B8G8R8A8_SNORM;
        case reznor_texture_format_b8g8r8a8_sint:       return VK_FORMAT_B8G8R8A8_SINT;
        case reznor_texture_format_b8g8r8a8_uint:       return VK_FORMAT_B8G8R8A8_UINT;
        case reznor_texture_format_r10g10b10a2:         return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case reznor_texture_format_b10g10r10a2:         return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r16g16b16_float:     return VK_FORMAT_R16G16B16_SFLOAT;
        case reznor_texture_format_r16g16b16_snorm:     return VK_FORMAT_R16G16B16_SNORM;
        case reznor_texture_format_r16g16b16_unorm:     return VK_FORMAT_R16G16B16_UNORM;
        case reznor_texture_format_r16g16b16_sint:      return VK_FORMAT_R16G16B16_SINT;
        case reznor_texture_format_r16g16b16_uint:      return VK_FORMAT_R16G16B16_UINT;
        /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r64_float:           return VK_FORMAT_R64_SFLOAT;
        case reznor_texture_format_r64_sint:            return VK_FORMAT_R64_SINT;
        case reznor_texture_format_r64_uint:            return VK_FORMAT_R64_UINT;
        case reznor_texture_format_r32g32_float:        return VK_FORMAT_R32G32_SFLOAT;
        case reznor_texture_format_r32g32_sint:         return VK_FORMAT_R32G32_SINT;
        case reznor_texture_format_r32g32_uint:         return VK_FORMAT_R32G32_UINT;
        case reznor_texture_format_r16g16b16a16_float:  return VK_FORMAT_R16G16B16A16_SFLOAT;
        case reznor_texture_format_r16g16b16a16_snorm:  return VK_FORMAT_R16G16B16A16_SNORM;
        case reznor_texture_format_r16g16b16a16_unorm:  return VK_FORMAT_R16G16B16A16_UNORM;
        case reznor_texture_format_r16g16b16a16_sint:   return VK_FORMAT_R16G16B16A16_SINT;
        case reznor_texture_format_r16g16b16a16_uint:   return VK_FORMAT_R16G16B16A16_UINT;
        /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r32g32b32_float:     return VK_FORMAT_R32G32B32_SFLOAT;
        case reznor_texture_format_r32g32b32_sint:      return VK_FORMAT_R32G32B32_SINT;
        case reznor_texture_format_r32g32b32_uint:      return VK_FORMAT_R32G32B32_UINT;
        /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r32g32b32a32_float:  return VK_FORMAT_R32G32B32A32_SFLOAT;
        case reznor_texture_format_r32g32b32a32_sint:   return VK_FORMAT_R32G32B32A32_SINT;
        case reznor_texture_format_r32g32b32a32_uint:   return VK_FORMAT_R32G32B32A32_UINT;
        case reznor_texture_format_r64g64_float:        return VK_FORMAT_R64G64_SFLOAT;
        case reznor_texture_format_r64g64_sint:         return VK_FORMAT_R64G64_SINT;
        case reznor_texture_format_r64g64_uint:         return VK_FORMAT_R64G64_UINT;
        /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r64g64b64_float:     return VK_FORMAT_R64G64B64_SFLOAT;
        case reznor_texture_format_r64g64b64_sint:      return VK_FORMAT_R64G64B64_SINT;
        case reznor_texture_format_r64g64b64_uint:      return VK_FORMAT_R64G64B64_UINT;
        /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
        case reznor_texture_format_r64g64b64a64_float:  return VK_FORMAT_R64G64B64A64_SFLOAT;
        case reznor_texture_format_r64g64b64a64_sint:   return VK_FORMAT_R64G64B64A64_SINT;
        case reznor_texture_format_r64g64b64a64_uint:   return VK_FORMAT_R64G64B64A64_UINT;
        /* compressed texture formats */
        case reznor_texture_format_BC1:                 return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
        case reznor_texture_format_BC2:                 return VK_FORMAT_BC2_SRGB_BLOCK;
        case reznor_texture_format_BC3:                 return VK_FORMAT_BC3_SRGB_BLOCK;
        case reznor_texture_format_BC4:                 return VK_FORMAT_BC4_SNORM_BLOCK;
        case reznor_texture_format_BC5:                 return VK_FORMAT_BC5_SNORM_BLOCK;
        case reznor_texture_format_BC6H:                return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        case reznor_texture_format_BC7:                 return VK_FORMAT_BC7_SRGB_BLOCK;
        case reznor_texture_format_ETC1:                return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case reznor_texture_format_ETC2:                return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case reznor_texture_format_ETC2A:               return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        case reznor_texture_format_ETC2A1:              return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        case reznor_texture_format_PTC12:               return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
        case reznor_texture_format_PTC14:               return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
        case reznor_texture_format_PTC22:               return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
        case reznor_texture_format_PTC24:               return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
        case reznor_texture_format_ASTC4x4:             return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        case reznor_texture_format_ASTC5x4:             return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
        case reznor_texture_format_ASTC5x5:             return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        case reznor_texture_format_ASTC6x5:             return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
        case reznor_texture_format_ASTC6x6:             return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        case reznor_texture_format_ASTC8x5:             return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
        case reznor_texture_format_ASTC8x6:             return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
        case reznor_texture_format_ASTC8x8:             return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        case reznor_texture_format_ASTC10x5:            return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
        case reznor_texture_format_ASTC10x6:            return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
        case reznor_texture_format_ASTC10x8:            return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
        case reznor_texture_format_ASTC10x10:           return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        case reznor_texture_format_ASTC12x10:           return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
        case reznor_texture_format_ASTC12x12:           return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
        default:                                        return VK_FORMAT_UNDEFINED;
    };
}

void vulkan_texture_destroy(struct reznor_texture *restrict texture)
{
    (void)texture;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, texture) 
{
    (void)device;
    (void)texture_count;
    (void)texture_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}


void vulkan_sampler_destroy(struct reznor_sampler *restrict sampler)
{
    (void)sampler;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, sampler)
{
    (void)device;
    (void)sampler_count;
    (void)sampler_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}
