#include "vk_reznor.h"

static const VkFormat g_d16_candidates[] = { 
    VK_FORMAT_D16_UNORM, 
    VK_FORMAT_D16_UNORM_S8_UINT, 
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT, 
    VK_FORMAT_D32_SFLOAT_S8_UINT,
}; 

static const VkFormat g_d24s8_candidates[] = {
    VK_FORMAT_D24_UNORM_S8_UINT, 
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM,
}; 

VkCompareOp vulkan_get_compare_operation(enum reznor_comparison_function fn)
{
    switch (fn) {
    case reznor_comparison_function_never: return VK_COMPARE_OP_NEVER;
    case reznor_comparison_function_less: return VK_COMPARE_OP_LESS;
    case reznor_comparison_function_equal: return VK_COMPARE_OP_EQUAL;
    case reznor_comparison_function_less_equal: return VK_COMPARE_OP_LESS_OR_EQUAL;
    case reznor_comparison_function_not_equal: return VK_COMPARE_OP_NOT_EQUAL;
    case reznor_comparison_function_greater: return VK_COMPARE_OP_GREATER;
    case reznor_comparison_function_greater_equal: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case reznor_comparison_function_always: return VK_COMPARE_OP_ALWAYS;
    }
    UNREACHABLE;
}

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
    UNREACHABLE;
}

VkImageLayout vulkan_texture_layout_translate(enum reznor_texture_layout layout)
{
    switch (layout) {
        case reznor_texture_layout_undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
        case reznor_texture_layout_general: return VK_IMAGE_LAYOUT_GENERAL;
        case reznor_texture_layout_color_attachment_optimal: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case reznor_texture_layout_depth_attachment_optimal: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        case reznor_texture_layout_depth_stencil_read_only_optimal: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        case reznor_texture_layout_stencil_attachment_optimal: return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
        case reznor_texture_layout_shader_read_only_optimal: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case reznor_texture_layout_transfer_source_optimal: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case reznor_texture_layout_transfer_target_optimal: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    UNREACHABLE;
}

VkImageSubresourceRange vulkan_get_image_subresource_range(const struct reznor_texture *texture)
{
    VkImageAspectFlags aspect_mask = 0u;
    if (texture->header.usage_mask & (reznor_texture_usage_render_depth_target | reznor_texture_usage_render_depth_target)) {
        aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (vulkan_texture_format_has_stencil(texture->header.format))
            aspect_mask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    } else {
        aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkImageSubresourceRange result = {
        .aspectMask = aspect_mask,
        .baseMipLevel = texture->header.first_level,
        .levelCount = texture->header.level_count,
        .baseArrayLayer = texture->header.first_array_layer,
        .layerCount = texture->header.layer_count,
    };
    return result;
}

FN_REZNOR_ASSEMBLY(vulkan, texture) {
    assert_debug(work && work->assembly.texture && work->memory.data && work->type == reznor_resource_type_texture && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->texture_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, texture) { 
    assert_debug(texture && texture->header.resource.device);
    struct reznor_device *device = texture->header.resource.device;

    if (texture->image_view)
        device->vkDestroyImageView(device->logical, texture->image_view, &device->host_allocator);
    if (texture->image)
        device->vkDestroyImage(device->logical, texture->image, &device->host_allocator);

    if (texture->allocation->is_dedicated_allocation)
        _reznor_vulkan_device_memory_disassembly(texture->allocation);

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->texture_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(texture);
}

VkSamplerAddressMode vulkan_get_sampler_address_mode(enum reznor_sampler_address_mode mode)
{
    switch (mode) {
        case reznor_sampler_address_mode_wrap: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case reznor_sampler_address_mode_mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case reznor_sampler_address_mode_clamp_edge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case reznor_sampler_address_mode_clamp_border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default: return 0;
    }
    UNREACHABLE;
}

FN_REZNOR_ASSEMBLY(vulkan, sampler) { 
    assert_debug(work && work->assembly.sampler && work->memory.data && work->type == reznor_resource_type_sampler && work->device);
    struct reznor_device *device = work->device;
    struct reznor_sampler *samplers = work->memory.data;

    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    for (u32 i = 0; i < work->assembly_count; i++) {
        struct reznor_sampler_assembly *assembly = &work->assembly.sampler[i];
        struct reznor_sampler *sampler = &samplers[i];

        sampler->header.device = device;
        sampler->header.debug_name = assembly->header.debug_name;
        sampler->header.type = reznor_resource_type_sampler;
        sampler->sampler = VK_NULL_HANDLE;

        /* write the vulkan create info */
        sampler_info.magFilter = vulkan_get_filter(assembly->mag_filter);
        sampler_info.minFilter = vulkan_get_filter(assembly->min_filter);
        sampler_info.mipmapMode = vulkan_get_sampler_mipmap_mode(assembly->mip_filter);
        sampler_info.addressModeU = vulkan_get_sampler_address_mode(assembly->address_u);
        sampler_info.addressModeV = vulkan_get_sampler_address_mode(assembly->address_v);
        sampler_info.addressModeW = vulkan_get_sampler_address_mode(assembly->address_w);
        sampler_info.mipLodBias = assembly->mip_lod_bias;
        sampler_info.anisotropyEnable = assembly->max_anisotrophy_level > 0.0 ? VK_TRUE : VK_FALSE;
        sampler_info.maxAnisotropy = assembly->max_anisotrophy_level;
        sampler_info.compareEnable = assembly->enable_comparison;
        sampler_info.compareOp = assembly->comparison_function;
        sampler_info.minLod = assembly->min_lod;
        sampler_info.maxLod = assembly->max_lod;

        VkResult res = device->vkCreateSampler(device->logical, &sampler_info, &device->host_allocator, &sampler->sampler);
        if (res != VK_SUCCESS) {
            log_error("Failed to create a sampler '%s' (%u out of %u), using device '%s': %s.", 
                sampler->header.debug_name.ptr, i, work->assembly_count, device->header.name.ptr, vulkan_result_string(res));

            /* destroy all previous samplers */
            for (u32 j = 0; j < i; j++) {
                work->assembly.sampler[i].header.output.sampler = NULL;
                device->vkDestroySampler(device->logical, samplers[j].sampler, &device->host_allocator);
            }
            memset(samplers, 0, sizeof(struct reznor_sampler) * i);
            work->result = result_error;
            return;
        }
        assembly->header.output.sampler = sampler;
    }

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->sampler_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, sampler) { 
    assert_debug(sampler && sampler->header.device);
    struct reznor_device *device = sampler->header.device;

    if (sampler->sampler)
        device->vkDestroySampler(device->logical, sampler->sampler, &device->host_allocator);

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->sampler_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(sampler);
}
