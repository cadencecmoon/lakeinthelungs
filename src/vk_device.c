#include "vk_reznor.h"

#include <amw/hadal.h>
#include <amw/bits.h>

static const char *device_type_string(VkPhysicalDeviceType type)
{
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "integrated";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "discrete";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "virtual";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "cpu";
        default:                                     return "unknown";
    }
}

static const char *vendor_name_string(u32 vendor_id)
{
    switch (vendor_id) {
        case 0x1002: return "AMD";
        case 0x1010: return "ImgTec";
        case 0x106B: return "Apple";
        case 0x10DE: return "NVIDIA";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8086: return "Intel";
        default:     return "unknown";
    }
}

struct physical_device_query_work {
    s32                             result;
    u32                             index;
    const struct reznor            *reznor;
    const struct hadal             *hadal;
    struct vulkan_physical_device   physical;
};

static void physical_device_query(struct physical_device_query_work *work)
{
    const struct reznor *reznor = work->reznor;
    struct riven *riven = reznor->interface.header.riven;
    struct vulkan_physical_device *physical = &work->physical;
    assert_debug(physical->device);

    VkVideoFormatPropertiesKHR *video_decode_format_properties;
    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties *extension_properties;

    u32 api_version = 0;
    u32 extension_count = 0;
    u32 video_decode_format_count = 0;
    u32 queue_family_count = 0;
    
    usize log_buffer_size = 1500;
    usize log_offset = 0;
    char *log_buffer;

    /* save the device name */
    reznor->vkGetPhysicalDeviceProperties(physical->device, &physical->properties2.properties);
    struct str formatted_name = str_null;
    riven_format_string(riven, riven_tag_deferred, &formatted_name, "Vulkan physical device '%s'", physical->properties2.properties.deviceName);
    const char *name = formatted_name.ptr;

    /* check driver's version */
    api_version = physical->properties2.properties.apiVersion;
    if (api_version < VK_API_VERSION_1_2) {
        log_warn("DISCARD: %s has too old drivers - found API version %u.%u.%u, we target atleast 1.2.X.", 
                 name, (api_version >> 22u), (api_version >> 12u) & 0x3ffu, api_version & 0xfffu);
        return;
    }
    b32 has_vk13 = api_version >= VK_API_VERSION_1_3;
    b32 has_vk14 = api_version >= VK_API_VERSION_1_4;
    physical->score += (api_version & 0xfffu) * 13;

    /* check presence of queue families */
    reznor->vkGetPhysicalDeviceQueueFamilyProperties2(physical->device, &queue_family_count, NULL);
    if (queue_family_count == 0) {
        log_warn("DISCARD: %s has no queue families.", name);
        return;
    }

    /* check presence of device extensions */
    VERIFY_VK(reznor->vkEnumerateDeviceExtensionProperties(physical->device, NULL, &extension_count, NULL));
    if (extension_count == 0) {
        log_warn("DISCARD: %s has zero Vulkan extensions.", name);
        return;
    }

    if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        /* we expect a discrete GPU to be generally a better option than other types */
        physical->feats |= reznor_device_feature_is_discrete;
        physical->score += 2500;
    } else if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        /* we generally want to favor integrated GPU's over CPU/virtual ones. */
        physical->score += 1250;
    }

    physical->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    physical->properties_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES;
    physical->properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    physical->properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    physical->properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    physical->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    physical->raytracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    physical->descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    physical->fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    physical->mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;

    physical->properties2.pNext = has_vk14 ? &physical->properties_14 : (has_vk13 ? (void *)&physical->properties_13 : (void *)&physical->properties_12);
    physical->properties_14.pNext = &physical->properties_13;
    physical->properties_13.pNext = &physical->properties_12;
    physical->properties_12.pNext = &physical->properties_11;
    physical->properties_11.pNext = &physical->acceleration_structure_properties;
    physical->acceleration_structure_properties.pNext = &physical->raytracing_pipeline_properties;
    physical->raytracing_pipeline_properties.pNext = &physical->descriptor_indexing_properties;
    physical->descriptor_indexing_properties.pNext = &physical->fragment_shading_rate_properties;
    physical->fragment_shading_rate_properties.pNext = &physical->mesh_shader_properties;

    physical->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physical->memory_properties2.pNext = &physical->memory_budget;
    physical->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

    physical->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical->features_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    physical->features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    physical->features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    physical->features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    physical->acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    physical->raytracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    physical->ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    physical->descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    physical->dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    physical->dynamic_rendering_local_read_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
    physical->synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    physical->fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
    physical->mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    physical->features2.pNext = has_vk14 ? &physical->features_14 : (has_vk13 ? (void *)&physical->features_13 : (void *)&physical->properties_12);
    physical->features_14.pNext = &physical->features_13;
    physical->features_13.pNext = &physical->features_12;
    physical->features_12.pNext = &physical->features_11;
    physical->features_11.pNext = &physical->acceleration_structure_features;
    physical->acceleration_structure_features.pNext = &physical->raytracing_pipeline_features;
    physical->raytracing_pipeline_features.pNext = &physical->ray_query_features;
    physical->ray_query_features.pNext = &physical->descriptor_indexing_features;
    physical->descriptor_indexing_features.pNext = &physical->dynamic_rendering_features;
    physical->dynamic_rendering_features.pNext = &physical->dynamic_rendering_local_read_features;
    physical->dynamic_rendering_local_read_features.pNext = &physical->synchronization2_features;
    physical->synchronization2_features.pNext = &physical->fragment_shading_rate_features;
    physical->fragment_shading_rate_features.pNext = &physical->mesh_shader_features;

    reznor->vkGetPhysicalDeviceProperties2(physical->device, &physical->properties2);
    reznor->vkGetPhysicalDeviceMemoryProperties2(physical->device, &physical->memory_properties2);
    reznor->vkGetPhysicalDeviceFeatures2(physical->device, &physical->features2);

    /* video capabilities are optional */
    VkPhysicalDeviceVideoFormatInfoKHR video_decode_format = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR,
        .pNext = NULL,
        .imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
    };
    VERIFY_VK(reznor->vkGetPhysicalDeviceVideoFormatPropertiesKHR(physical->device, &video_decode_format, &video_decode_format_count, NULL));

    /* calculate memory requirements for scratch data */
    {
        usize decode_video_format_properties_bytes = A8(sizeof(VkVideoFormatPropertiesKHR) * video_decode_format_count);
        usize queue_family_video_properties_bytes = A8(sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count);
        usize queue_family_properties_bytes = A8(sizeof(VkQueueFamilyProperties2) * queue_family_count);
        usize extension_properties_bytes = A8(sizeof(VkExtensionProperties) * extension_count);
        usize total_bytes = 
            log_buffer_size +
            decode_video_format_properties_bytes +
            queue_family_video_properties_bytes +
            queue_family_properties_bytes +
            extension_properties_bytes;

        u8 *raw = (u8 *)riven_alloc(riven, riven_tag_deferred, total_bytes, 8);
        memset(raw, 0, total_bytes);

        usize o = 0;
        log_buffer = (char *)&raw[o];
        o += log_buffer_size;
        video_decode_format_properties = (VkVideoFormatPropertiesKHR *)&raw[o];
        o += decode_video_format_properties_bytes;
        queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)&raw[o];
        o += queue_family_video_properties_bytes;
        queue_family_properties2 = (VkQueueFamilyProperties2 *)&raw[o];
        o += queue_family_properties_bytes;
        extension_properties = (VkExtensionProperties *)&raw[o];

        assert_debug(o + extension_properties_bytes == total_bytes);
    }

    /* prepare the log buffer */
    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
            "Vulkan physical device accepted (internal idx %u):\n"
            "    Name        %s\n"
            "    Type        %-10s id %X\n"
            "    Vendor      %-10s id %X\n"
            "    Driver ver. %u.%u.%u\n"
            "    API ver.    %u.%u.%u\n"
            "Memory heaps (%u):\n", 
            work->index, physical->properties2.properties.deviceName,
            device_type_string(physical->properties2.properties.deviceType), physical->properties2.properties.deviceID,
            vendor_name_string(physical->properties2.properties.vendorID), physical->properties2.properties.vendorID,
            (physical->properties2.properties.driverVersion >> 22u),
            (physical->properties2.properties.driverVersion >> 12u) & 0x3ffu,
            physical->properties2.properties.driverVersion & 0xfffu,
            (physical->properties2.properties.apiVersion >> 22u),
            (physical->properties2.properties.apiVersion >> 12u) & 0x3ffu,
            physical->properties2.properties.apiVersion & 0xfffu,
            physical->memory_properties2.memoryProperties.memoryHeapCount);

    /* check VRAM */
    for (u32 i = 0; i < physical->memory_properties2.memoryProperties.memoryHeapCount; i++) {
        VkMemoryHeap *heap = &physical->memory_properties2.memoryProperties.memoryHeaps[i];

        b32 device_local = heap->flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
        physical->score += (device_local ? 42 : 21) * (heap->size >> 28); /* 256 MB -> 1 */
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                "    Heap %u   %6lu MB (%s)\n", i, (heap->size >> 20), device_local ? "device local" : "host visible");
        if (device_local)
            physical->total_vram += heap->size;
    }
    if (physical->feats & reznor_device_feature_is_discrete)
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                "Total VRAM available %lu MB.\n", physical->total_vram >> 20);

    /* resolve queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        queue_family_video_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        queue_family_video_properties[i].pNext = NULL;
        queue_family_properties2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queue_family_properties2[i].pNext = &queue_family_video_properties[i];
    }
    reznor->vkGetPhysicalDeviceQueueFamilyProperties2(physical->device, &queue_family_count, queue_family_properties2);

    physical->graphics_queue_family_index = UINT32_MAX;
    physical->compute_queue_family_index = UINT32_MAX;
    physical->transfer_queue_family_index = UINT32_MAX;
    physical->sparse_binding_queue_family_index = UINT32_MAX;
    physical->video_decode_queue_family_index = UINT32_MAX;
    physical->presentation_queue_family_index = UINT32_MAX;

    /* search for specialized queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        u32 queue_count = queue_family_properties2[i].queueFamilyProperties.queueCount;
        if (queue_count <= 0)
            continue;

        VkQueueFlags flags = queue_family_properties2[i].queueFamilyProperties.queueFlags;
        if (physical->graphics_queue_family_index == UINT32_MAX && (flags & VK_QUEUE_GRAPHICS_BIT)) 
        {   /* don't be picky about the graphics queue */
            physical->graphics_queue_family_queue_count = queue_count;
            physical->graphics_queue_family_index = i;

        } else if (physical->compute_queue_family_index == UINT32_MAX 
            && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == VK_QUEUE_COMPUTE_BIT) 
        {   /* request an async compute queue family */
            physical->has_async_compute = true;
            physical->async_compute_queue_family_queue_count = queue_count;
            physical->compute_queue_family_index = i;

        } else if (physical->transfer_queue_family_index == UINT32_MAX 
            && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) == VK_QUEUE_TRANSFER_BIT) 
        {   /* request an async transfer queue family */
            physical->has_async_transfer = true;
            physical->transfer_queue_family_index = i;

        } else if (physical->sparse_binding_queue_family_index == UINT32_MAX 
            && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_SPARSE_BINDING_BIT)) == VK_QUEUE_SPARSE_BINDING_BIT) 
        {   /* request an async sparse binding queue family */
            physical->has_async_sparse_binding = true;
            physical->sparse_binding_queue_family_index = i;

        } else if (physical->video_decode_queue_family_index == UINT32_MAX 
            && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR)) == VK_QUEUE_VIDEO_DECODE_BIT_KHR) 
        {   /* request an async video queue family */
            physical->has_async_video = true;
            physical->video_decode_queue_family_index = i;
        }
    }
    if (physical->graphics_queue_family_index == UINT32_MAX) {
        log_warn("DISCARD: %s does not support graphics operations.", name);
        return;
    }

    u32 graphics_family_queue_count = 1;
    u32 async_compute_family_queue_count = 1;

    u32 o = 0;
    physical->queue_family_indices[o++] = physical->graphics_queue_family_index;

    /* resolve the compute queue */
    if (physical->compute_queue_family_index == UINT32_MAX) {
        physical->compute_queue_family_index = physical->graphics_queue_family_index;
        if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
            graphics_family_queue_count++;
            physical->has_async_compute = true;
        }
        physical->async_compute_queue_family_queue_count = async_compute_family_queue_count = 0;
    } else {
        physical->queue_family_indices[o++] = physical->compute_queue_family_index;
    }

    /* resolve the transfer queue */
    if (physical->transfer_queue_family_index == UINT32_MAX) {
        if (async_compute_family_queue_count) {
            physical->transfer_queue_family_index = physical->compute_queue_family_index;
            if (physical->async_compute_queue_family_queue_count > async_compute_family_queue_count) {
                async_compute_family_queue_count++;
                physical->has_async_transfer = true;
            }
        } else {
            physical->transfer_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_transfer = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->transfer_queue_family_index;
    }

    /* resolve the optional sparse binding queue */
    if (physical->sparse_binding_queue_family_index == UINT32_MAX) {
        if (queue_family_properties2[physical->graphics_queue_family_index].queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            physical->sparse_binding_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_sparse_binding = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->sparse_binding_queue_family_index;
    }

    /* resolve the optional video decoding queue */
    if (physical->video_decode_queue_family_index == UINT32_MAX) {
        if (queue_family_properties2[physical->graphics_queue_family_index].queueFamilyProperties.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
            physical->video_decode_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_video = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->video_decode_queue_family_index;
    }

    /* resolve the present queue */
    struct hadal_interface *hadal = (struct hadal_interface *)work->hadal;
    if (physical->compute_queue_family_index != physical->graphics_queue_family_index 
        && hadal->vulkan_presentation_support(work->hadal, physical->device, physical->compute_queue_family_index)) 
    {
        physical->presentation_queue_family_index = physical->compute_queue_family_index;
        if (physical->async_compute_queue_family_queue_count > async_compute_family_queue_count) {
            async_compute_family_queue_count++;
            physical->has_async_present = true;
        }
        
    } else if (hadal->vulkan_presentation_support(work->hadal, physical->device, physical->graphics_queue_family_index)) {

        if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
            graphics_family_queue_count++;
            physical->has_async_present = true;
        }

    } else {
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                "This device has NO presentation support for '%s'.", hadal->header.name.ptr);
    }
    physical->graphics_queue_family_queue_count = graphics_family_queue_count;
    physical->async_compute_queue_family_queue_count = async_compute_family_queue_count;
    physical->queue_family_count = o;

    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
            "Specialized command queues:\n"
            "    %-17s %11s, %u family index\n",
            "Graphics", "Yes", physical->graphics_queue_family_index);
    { /* log queues, in order: compute, transfer, sparse binding, video, present */
        b32 queue_supported[5] = {
            true, true, 
            physical->sparse_binding_queue_family_index != UINT32_MAX,
            physical->video_decode_queue_family_index != UINT32_MAX,
            physical->presentation_queue_family_index != UINT32_MAX,
        };
        u32 queue_families[5] = {
            physical->compute_queue_family_index,
            physical->transfer_queue_family_index,
            physical->sparse_binding_queue_family_index,
            physical->video_decode_queue_family_index,
            physical->presentation_queue_family_index,
        };
        u32 queue_async[5] = {
            physical->has_async_compute,
            physical->has_async_transfer,
            physical->has_async_sparse_binding,
            physical->has_async_video,
            physical->has_async_present,
        };
        const char *queue_names[5] = { "Compute", "Transfer", "Sparse binding", "Video", "Present" };

        for (u32 i = 0; i < 5; i++) {
            log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, 
                "    %-17s %11s", queue_names[i], queue_supported[i] ? (queue_async[i] ? "Async" : "Overloaded") : "No");
            if (queue_supported[i])
                log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, ", %u family index", queue_families[i]);
            log_buffer[log_offset++] = '\n';
        }
    }

    /* resolve extensions (required are: dynamic_rendering, dynamic_rendering_local_read, synchronization2) */
    VERIFY_VK(reznor->vkEnumerateDeviceExtensionProperties(physical->device, NULL, &extension_count, extension_properties));
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_swapchain_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_DEVICE_FAULT_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_device_fault_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_memory_budget_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_memory_priority_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MESH_SHADER_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_mesh_shader_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_fragment_shading_rate_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_deferred_host_operations_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_acceleration_structure_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_pipeline_library_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_QUERY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_ray_query_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_raytracing_pipeline_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_raytracing_maintenance1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_queue_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_queue_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_av1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_h264_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_maintenance1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_amd_device_coherent_memory_bit;

#ifndef VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION
    /* for backwards compatibility */
    b32 valid_ext = true;
#endif /* VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION */

    if (!has_vk14) {
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_dynamic_rendering_local_read_bit;
#ifndef VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION
        } else if (!physical->dynamic_rendering_local_read_features.dynamicRenderingLocalRead) {
            log_warn("%s does not support a required extension: %s.", name, VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 600;
#endif /* VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION */
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_MAINTENANCE_5_EXTENSION_NAME))
            physical->extension_bits |= vulkan_ext_maintenance5_bit;
    }
    if (!has_vk13) {
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_dynamic_rendering_bit;
#ifndef VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION
        } else if (!physical->dynamic_rendering_features.dynamicRendering) {
            log_warn("%s does not support a required extension: %s.", name, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 600;
#endif /* VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION */
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_synchronization2_bit;
#ifndef VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION
        } else if (!physical->synchronization2_features.synchronization2) {
            log_warn("%s does not support a required extension: %s.", name, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 600;
#endif /* VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION */
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
            physical->extension_bits |= vulkan_ext_maintenance4_bit;
    }
#ifndef VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION
    if (!valid_ext) {
        log_warn("DISCARD: %s is not suitable for our rendering work (required extensions are missing).", name);
        return;
    }
#endif /* VULKAN_REQUIRED_EXTENSIONS_NO_VALIDATION */

    /* check for swapchain support */
    if (!(physical->extension_bits & vulkan_ext_swapchain_bit)) {
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, "Device has NO swapchain support.\n");

        /* invalidate presentation support */
        if (physical->has_async_present) {
            if (physical->presentation_queue_family_index == physical->graphics_queue_family_index) {
                physical->graphics_queue_family_queue_count--;
            } else if (physical->presentation_queue_family_index == physical->compute_queue_family_index) {
                physical->async_compute_queue_family_queue_count--;
            }
        }
        physical->has_async_present = false;
        physical->presentation_queue_family_index = UINT32_MAX;
        physical->score -= 600;
    } else {
        physical->feats |= reznor_device_feature_swapchain_support;
    }

    /* check video format properties */
    VERIFY_VK(reznor->vkGetPhysicalDeviceVideoFormatPropertiesKHR(physical->device, &video_decode_format, &video_decode_format_count, video_decode_format_properties));
    /* TODO video decode AV1 H.264 */

    /* collect optional features */
    u32 feat_swapchain = physical->feats & reznor_device_feature_swapchain_support;
    u32 feat_sparse_binding = physical->features2.features.sparseBinding;
    u32 feat_dynamic_local_read = (physical->features_14.dynamicRenderingLocalRead || physical->dynamic_rendering_local_read_features.dynamicRenderingLocalRead);
    u32 feat_dynamic_rendering = (physical->features_13.dynamicRendering || physical->dynamic_rendering_features.dynamicRendering);
    u32 feat_synchronization2 = (physical->features_13.synchronization2 || physical->synchronization2_features.synchronization2);
    u32 feat_descriptor_indexing = physical->features_12.descriptorIndexing;
    u32 feat_fragment_shading_rate = 
        (physical->fragment_shading_rate_features.attachmentFragmentShadingRate + 
         physical->fragment_shading_rate_features.pipelineFragmentShadingRate +
         physical->fragment_shading_rate_features.primitiveFragmentShadingRate);
    u32 feat_acceleration_structure = 
        (physical->acceleration_structure_features.accelerationStructure +
         physical->acceleration_structure_features.accelerationStructureCaptureReplay +
         physical->acceleration_structure_features.accelerationStructureHostCommands +
         physical->acceleration_structure_features.accelerationStructureIndirectBuild +
         physical->acceleration_structure_features.descriptorBindingAccelerationStructureUpdateAfterBind);
    u32 feat_raytracing_pipeline = 
        (physical->raytracing_pipeline_features.rayTracingPipeline +
         physical->raytracing_pipeline_features.rayTracingPipelineShaderGroupHandleCaptureReplay +
         physical->raytracing_pipeline_features.rayTracingPipelineShaderGroupHandleCaptureReplayMixed +
         physical->raytracing_pipeline_features.rayTracingPipelineTraceRaysIndirect +
         physical->raytracing_pipeline_features.rayTraversalPrimitiveCulling);
    u32 feat_ray_query = physical->ray_query_features.rayQuery;
    u32 feat_mesh_shader = 
        (physical->mesh_shader_features.meshShader +
         physical->mesh_shader_features.meshShaderQueries +
         physical->mesh_shader_features.multiviewMeshShader +
         physical->mesh_shader_features.taskShader +
         physical->mesh_shader_features.primitiveFragmentShadingRateMeshShader);
    u32 feat_geometry_shader = physical->features2.features.geometryShader;
    u32 feat_tesselation_shader = physical->features2.features.tessellationShader;
    u32 feat_decode_av1 = (physical->extension_bits & vulkan_ext_video_decode_av1_bit);
    u32 feat_decode_h264 = (physical->extension_bits & vulkan_ext_video_decode_h264_bit);

    u32 features = 
        feat_swapchain +
        feat_sparse_binding +
        feat_dynamic_local_read +
        feat_dynamic_rendering +
        feat_synchronization2 +
        feat_descriptor_indexing +
        feat_fragment_shading_rate +
        feat_acceleration_structure +
        feat_raytracing_pipeline +
        feat_ray_query +
        feat_mesh_shader +
        feat_geometry_shader +
        feat_tesselation_shader +
        feat_decode_av1 +
        feat_decode_h264;
    physical->score += 51 * features;

    if (feat_sparse_binding)
        physical->feats |= reznor_device_feature_sparse_binding;
    if (feat_acceleration_structure)
        physical->feats |= reznor_device_feature_acceleration_structures;
    if (feat_raytracing_pipeline)
        physical->feats |= reznor_device_feature_raytracing_pipeline;
    if (feat_mesh_shader)
        physical->feats |= reznor_device_feature_mesh_shader;
    if (feat_geometry_shader)
        physical->feats |= reznor_device_feature_geometry_shader;
    if (feat_tesselation_shader)
        physical->feats |= reznor_device_feature_tesselation_shader;
    if (feat_decode_av1)
        physical->feats |= reznor_device_feature_video_av1;
    if (feat_decode_h264)
        physical->feats |= reznor_device_feature_video_h264;

    struct next_feat {
        VkStructureType sType;
        void           *pNext;
    } *feat = (struct next_feat *)&physical->features_11;

    /* reorganize features to be enabled later */
#define ENABLE_FEATURE(_feat) \
        { feat->pNext = (void *)&physical->_feat; feat = feat->pNext; feat->pNext = NULL; }

    ENABLE_FEATURE(features_12)
    if (has_vk13) {
        ENABLE_FEATURE(features_13)
    } else {
        if (feat_dynamic_rendering)
            ENABLE_FEATURE(dynamic_rendering_features)
        if (feat_synchronization2)
            ENABLE_FEATURE(synchronization2_features)
    }
    if (has_vk14) {
        ENABLE_FEATURE(features_14)
    } else {
        if (feat_dynamic_local_read)
            ENABLE_FEATURE(dynamic_rendering_local_read_features)
    }
    if (feat_fragment_shading_rate)
        ENABLE_FEATURE(fragment_shading_rate_features)
    if (feat_mesh_shader)
        ENABLE_FEATURE(mesh_shader_features)
    if (feat_ray_query)
        ENABLE_FEATURE(ray_query_features)
    if (feat_raytracing_pipeline)
        ENABLE_FEATURE(raytracing_pipeline_features)
    if (feat_acceleration_structure)
        ENABLE_FEATURE(acceleration_structure_features)

#undef ENABLE_FEATURE

    /* we accept this physical device */
    work->result = result_success;

    /* overwrite the last \n */
    log_buffer[log_offset-1] = '\0';
    log_info("%s", log_buffer);
}

static b32 write_physical_devices(
    struct reznor   *reznor,
    struct hadal    *hadal)
{
    assert_debug(reznor->physical_devices == NULL);

    struct riven *riven = reznor->interface.header.riven;
    riven_tag_t tag = reznor->interface.header.tag;

    /* we checked availability of any physical devices at encore */
    u32 physical_device_count = 0;
    VERIFY_VK(reznor->vkEnumeratePhysicalDevices(reznor->instance, &physical_device_count, NULL));

    struct hadal_interface *hadal_interface = (struct hadal_interface *)hadal;
    hadal_interface->vulkan_write_instance_procedures(hadal, reznor->instance, reznor->vkGetInstanceProcAddr);

    usize physical_devices_bytes = A8(sizeof(VkPhysicalDevice) * physical_device_count);
    usize query_work_bytes = A8(sizeof(struct physical_device_query_work) * physical_device_count);
    usize riven_work_bytes = A8(sizeof(struct riven_work) * physical_device_count);
    usize indices_bytes = A8(sizeof(u32) * physical_device_count);
    usize total_bytes = physical_devices_bytes + query_work_bytes + riven_work_bytes + indices_bytes;

    u8 *raw = (u8 *)riven_alloc(riven, riven_tag_deferred, total_bytes, 8);
    memset(raw, 0, total_bytes);

    u32 o = 0;
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)&raw[o];
    o += physical_devices_bytes;
    struct physical_device_query_work *query_work = (struct physical_device_query_work *)&raw[o];
    o += query_work_bytes;
    struct riven_work *riven_work = (struct riven_work *)&raw[o];
    o += riven_work_bytes;
    u32 *indices = (u32 *)&raw[o];
    assert_debug(o + indices_bytes == total_bytes);

    /* prepare the physical device query */
    VERIFY_VK(reznor->vkEnumeratePhysicalDevices(reznor->instance, &physical_device_count, physical_devices));

    for (u32 i = 0; i < physical_device_count; i++) {
        query_work[i].index = i;
        query_work[i].result = result_error;
        query_work[i].physical.device = physical_devices[i];
        query_work[i].reznor = reznor;
        query_work[i].hadal = hadal;
        riven_work[i].procedure = (PFN_riven_work)physical_device_query;
        riven_work[i].argument = (void *)&query_work[i];
        riven_work[i].name = str_init("reznor:vulkan:physical_device_query");
    }
    riven_split_work_and_unchain(riven, riven_work, physical_device_count);

    o = 0;
    /* collect indices of accepted physical devices */
    for (u32 i = 0; i < physical_device_count; i++)
        if (query_work[i].result == result_success)
            indices[o++] = i;
    if (o == 0) {
        log_error("Device query invalidated all of %u physical devices available to Vulkan.", physical_device_count);
        return false;
    }

    /* sort indices by score */
    for (u32 i = 0, j, max_idx; i < o - 1; i++) {
        max_idx = i;
        for (j = i + 1; j < o; j++)
            if (query_work[indices[j]].physical.score > query_work[indices[max_idx]].physical.score)
                max_idx = j;
        xorswap(&indices[max_idx], &indices[i]);
    }

    /* copy the accepted physical devices */
    struct vulkan_physical_device *devices = (struct vulkan_physical_device *)
        riven_alloc(riven, tag, sizeof(struct vulkan_physical_device), _Alignof(struct vulkan_physical_device));
    for (u32 i = 0; i < o; i++)
        memcpy(&devices[i], &query_work[indices[i]].physical, sizeof(struct vulkan_physical_device));
    reznor->physical_devices = devices;
    reznor->physical_device_count = o;
    return true;
}

FN_REZNOR_DEVICE_QUERY(vulkan)
{
    if (reznor->physical_devices == NULL && !write_physical_devices(reznor, hadal))
        return result_error;
    
    if (out_devices) {
        assert_debug(*out_device_count >= reznor->physical_device_count);

        for (u32 i = 0; i < reznor->physical_device_count; i++) {
            const struct vulkan_physical_device *physical = &reznor->physical_devices[i];
            out_devices[i].name.ptr = physical->properties2.properties.deviceName;
            out_devices[i].name.length = strnlen(out_devices[i].name.ptr, 255);
            out_devices[i].internal = (void *)physical;
            out_devices[i].features = physical->feats;
            out_devices[i].score = physical->score;
        }
    }
    *out_device_count = reznor->physical_device_count;
    return result_success;
}

FN_REZNOR_DEVICE_CREATE(vulkan)
{
    struct reznor *reznor = work->reznor;
    assert_debug(reznor);

    usize device_bytes = sizeof(struct reznor_device);
    usize frames_in_flight_bytes = sizeof(struct reznor_device_frame) * work->frames_in_flight;
    usize graphics_command_pools_bytes = sizeof(struct vulkan_command_pool) * work->frames_in_flight * reznor->interface.thread_count;
    usize total_bytes =
        device_bytes +
        frames_in_flight_bytes +
        graphics_command_pools_bytes;
    usize alignment = _Alignof(struct reznor_device);
    usize bytes = 0;

    if (!work->memory.data) {
        work->memory.size = total_bytes;
        work->memory.alignment = alignment;

        if (work->memory.tag) {
            work->memory.data = riven_alloc(reznor->interface.header.riven, work->memory.tag, total_bytes, alignment);
        } else {
            /* the caller must allocate memory */
            assert_debug(work->result != result_allocation_query);
            work->result = result_allocation_query;
            return;
        }
    } else {
        assert_debug(work->memory.size == total_bytes);
        assert_debug(work->memory.alignment == alignment);
    }
    assert_debug(work->out_device && work->physical_info);

    memset(work->memory.data, 0, total_bytes);
    u8 *raw = (u8 *)work->memory.data;

    struct vulkan_physical_device *physical = work->physical_info->internal;
    struct reznor_device *device = (struct reznor_device *)&raw[bytes];
    bytes += device_bytes;

    device->header.name = work->physical_info->name;
    device->header.reznor = reznor;
    device->header.features = physical->feats;
    device->header.frames_in_flight = work->frames_in_flight;
    device->physical = physical;
    vulkan_write_allocation_callbacks(&device->host_allocator, work->physical_info->name.ptr);

    f32 queue_priorities[6] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };
    VkDeviceQueueCreateInfo queue_infos[5];

    for (u32 i = 0; i < physical->queue_family_count; i++) {
        u32 family = physical->queue_family_indices[i];
        queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_infos[i].pNext = NULL;
        queue_infos[i].flags = 0;
        queue_infos[i].pQueuePriorities = queue_priorities;
        queue_infos[i].queueFamilyIndex = family;
        queue_infos[i].queueCount = 1;
        if (family == physical->graphics_queue_family_index) {
            queue_infos[i].queueCount = physical->graphics_queue_family_queue_count;
        } else if (family == physical->compute_queue_family_index) {
            queue_infos[i].queueCount = physical->async_compute_queue_family_queue_count;
        }
    }

    u32 o = 0;
    u32 extension_count = bits_popcnt_lookup((const u8 *)&physical->extension_bits, sizeof(u64));
    const char **extensions = riven_alloc(reznor->interface.header.riven, riven_tag_deferred, sizeof(const char *) * extension_count, _Alignof(const char *));
    if (physical->extension_bits & vulkan_ext_swapchain_bit)
        extensions[o++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_device_fault_bit)
        extensions[o++] = VK_EXT_DEVICE_FAULT_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_memory_budget_bit)
        extensions[o++] = VK_EXT_MEMORY_BUDGET_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_memory_priority_bit)
        extensions[o++] = VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_mesh_shader_bit)
        extensions[o++] = VK_EXT_MESH_SHADER_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_fragment_shading_rate_bit)
        extensions[o++] = VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_deferred_host_operations_bit)
        extensions[o++] = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_acceleration_structure_bit)
        extensions[o++] = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_pipeline_library_bit)
        extensions[o++] = VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_ray_query_bit)
        extensions[o++] = VK_KHR_RAY_QUERY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_raytracing_pipeline_bit)
        extensions[o++] = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_raytracing_maintenance1_bit)
        extensions[o++] = VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_queue_bit)
        extensions[o++] = VK_KHR_VIDEO_QUEUE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_queue_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_av1_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_h264_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_maintenance1_bit)
        extensions[o++] = VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_amd_device_coherent_memory_bit)
        extensions[o++] = VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_dynamic_rendering_local_read_bit)
        extensions[o++] = VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_dynamic_rendering_bit)
        extensions[o++] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_synchronization2_bit)
        extensions[o++] = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_maintenance5_bit)
        extensions[o++] = VK_KHR_MAINTENANCE_5_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_maintenance4_bit)
        extensions[o++] = VK_KHR_MAINTENANCE_4_EXTENSION_NAME;
    assert_debug(extension_count == o);

    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = physical->features2.pNext,
        .flags = 0,
        .pEnabledFeatures = &physical->features2.features,
        .ppEnabledExtensionNames = (const char * const *)extensions,
        .enabledExtensionCount = extension_count,
        .ppEnabledLayerNames = NULL,
        .enabledLayerCount = 0,
        .pQueueCreateInfos = queue_infos,
        .queueCreateInfoCount = physical->queue_family_count,
    };
    VERIFY_VK(reznor->vkCreateDevice(physical->device, &device_info, &device->host_allocator, &device->logical));

    if (!vulkan_load_device_procedures(device, physical->properties2.properties.apiVersion, physical->extension_bits)) {
        log_error("Can't load Vulkan device procedures for %s.", work->physical_info->name.ptr);
        if (device->vkDestroyDevice)
            device->vkDestroyDevice(device->logical, &device->host_allocator);
        return;
    }

    /* get device queues */
    for (u32 i = 0; i < physical->queue_family_count; i++) {
        u32 family = queue_infos[i].queueFamilyIndex;
        u32 count = queue_infos[i].queueCount;

        for (u32 j = 0; j < count; j++) {
            VkQueue queue = VK_NULL_HANDLE;

            device->vkGetDeviceQueue(device->logical, family, j, &queue);
            if (device->graphics_queue == VK_NULL_HANDLE && family == physical->graphics_queue_family_index) {
                device->graphics_queue = queue;
            } else if (device->compute_queue == VK_NULL_HANDLE && family == physical->compute_queue_family_index) {
                device->compute_queue = queue;
            } else if (device->transfer_queue == VK_NULL_HANDLE && family == physical->transfer_queue_family_index) {
                device->transfer_queue = queue;
            } else if (device->sparse_binding_queue == VK_NULL_HANDLE && family == physical->sparse_binding_queue_family_index) {
                device->sparse_binding_queue = queue;
            } else if (device->video_decode_queue == VK_NULL_HANDLE && family == physical->video_decode_queue_family_index) {
                device->video_decode_queue = queue;
            } else if (device->present_queue == VK_NULL_HANDLE && family == physical->presentation_queue_family_index) {
                device->present_queue = queue;
            }
        }
    }

    /* overload non async queues */
    if (device->compute_queue == VK_NULL_HANDLE)
        device->compute_queue = device->graphics_queue;
    if (device->transfer_queue == VK_NULL_HANDLE)
        device->transfer_queue = device->graphics_queue;
    if (device->sparse_binding_queue == VK_NULL_HANDLE && physical->sparse_binding_queue_family_index == physical->graphics_queue_family_index)
        device->sparse_binding_queue = device->graphics_queue;
    if (device->video_decode_queue == VK_NULL_HANDLE && physical->video_decode_queue_family_index == physical->graphics_queue_family_index)
        device->video_decode_queue = device->graphics_queue;
    if (device->present_queue == VK_NULL_HANDLE && physical->presentation_queue_family_index == physical->graphics_queue_family_index)
        device->present_queue = device->graphics_queue;

    /* prepare frames in flight */
    for (u32 i = 0; i < work->frames_in_flight; i++) {
        device->header.frames[i] = (struct reznor_device_frame *)&raw[bytes];
        bytes += sizeof(struct reznor_device_frame);

        struct reznor_device_frame *frame = device->header.frames[i];

        frame->device = device;
        frame->descriptor_pool = NULL; /* TODO */
        frame->graphics_command_buffer = VK_NULL_HANDLE;

        frame->graphics_command_pools = (struct vulkan_command_pool *)&raw[bytes];
        bytes += sizeof(struct vulkan_command_pool) * reznor->interface.thread_count;

        VkCommandPoolCreateInfo cmd_pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR,
            .queueFamilyIndex = physical->graphics_queue_family_index,
        };
        VkCommandBufferAllocateInfo cmd_buffer_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
            .commandBufferCount = 1,
        };
        for (u32 j = 0; j < reznor->interface.thread_count; j++) {
            struct vulkan_command_pool *pool = &frame->graphics_command_pools[j];
            VERIFY_VK(device->vkCreateCommandPool(device->logical, &cmd_pool_info, &device->host_allocator, &pool->command_pool));

            cmd_buffer_info.commandPool = pool->command_pool;
            VERIFY_VK(device->vkAllocateCommandBuffers(device->logical, &cmd_buffer_info, pool->command_buffers));
            pool->command_buffer_count = cmd_buffer_info.commandBufferCount;
        }
        cmd_buffer_info.commandBufferCount = 1;
        cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_buffer_info.commandPool = frame->graphics_command_pools[0].command_pool;
        VERIFY_VK(device->vkAllocateCommandBuffers(device->logical, &cmd_buffer_info, &frame->graphics_command_buffer));

        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        VERIFY_VK(device->vkCreateFence(device->logical, &fence_info, &device->host_allocator, &frame->fence));

        VkSemaphoreCreateInfo sem_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VERIFY_VK(device->vkCreateSemaphore(device->logical, &sem_info, &device->host_allocator, &frame->rendering_finished_semaphore));
    }

    *work->out_device = device;
    work->result = result_success;
    log_info("Created Vulkan device: %s.", device->header.name.ptr);
}

FN_REZNOR_DEVICE_DESTROY(vulkan)
{
    if (device->logical == VK_NULL_HANDLE) return;

    struct reznor *reznor = device->header.reznor;
    device->vkDeviceWaitIdle(device->logical);

    for (u32 i = 0; i < device->header.frames_in_flight; i++) {
        struct reznor_device_frame *frame = device->header.frames[i];

        if (!frame) continue;
        
        for (u32 j = 0; j < reznor->interface.thread_count; j++) 
            if (frame->graphics_command_pools[j].command_pool != VK_NULL_HANDLE)
                device->vkDestroyCommandPool(device->logical, frame->graphics_command_pools[j].command_pool, &device->host_allocator);
        if (frame->fence != VK_NULL_HANDLE)
            device->vkDestroyFence(device->logical, frame->fence, &device->host_allocator);
        if (frame->rendering_finished_semaphore != VK_NULL_HANDLE)
            device->vkDestroySemaphore(device->logical, frame->rendering_finished_semaphore, &device->host_allocator);
    }
#if REZNOR_ENABLE_GPU_PROFILER
    assert_debug(atomic_load_explicit(&device->device_memory_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->buffer_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->texture_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->sampler_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->pipeline_layout_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->graphics_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->compute_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->raytracing_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->descriptor_set_layout_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->descriptor_pool_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->swapchain_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->query_pool_count, memory_order_relaxed) == 0);
#endif
    device->vkDestroyDevice(device->logical, &device->host_allocator);
    log_info("Destroyed Vulkan device: %s.", device->header.name.ptr);

    zerop(device);
}

static b32 resolve_swapchain_state(struct reznor_swapchain *swapchain, VkResult result)
{
    /* check result */
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        atomic_fetch_or_explicit(&swapchain->header.flags, reznor_swapchain_flag_timed_out, memory_order_release);
    } else if (result == VK_ERROR_SURFACE_LOST_KHR) {
        atomic_fetch_or_explicit(&swapchain->header.flags, reznor_swapchain_flag_surface_was_lost, memory_order_release);
    } else if (result != VK_SUCCESS) {
        log_error("Unexpected error after vkAcquireNextImageKHR: %s. Swapchain '%s' will be destroyed.", vulkan_result_string(result), swapchain->header.debug_name.ptr);
        _reznor_vulkan_swapchain_disassembly(swapchain);
        return false;
    }

    /* ensure the swapchain is fine */
    if (atomic_load_explicit(&swapchain->header.flags, memory_order_acquire) & 
          (reznor_swapchain_flag_surface_was_lost | reznor_swapchain_flag_framebuffer_resized | reznor_swapchain_flag_timed_out))
    {
        _reznor_vulkan_try_recreate_swapchain(swapchain);
        if (swapchain->swapchain == VK_NULL_HANDLE)
            return false;
    }
    return true; /* the swapchain is fine */
}

FN_REZNOR_FRAME_NEXT_IMAGES(vulkan)
{
    assert_debug(swapchain_count <= REZNOR_MAX_SWAPCHAINS);

    for (u32 i = 0; i < swapchain_count; i++) {
        struct reznor_swapchain *swapchain = swapchains[i];
        struct reznor_device *device = swapchain->header.device;

        VkSemaphore image_available_semaphore = swapchain->image_available_semaphores[swapchain->image_available_semaphore_index];

        VkResult result = device->vkAcquireNextImageKHR(
            device->logical, 
            swapchain->swapchain, 
            UINT32_MAX, /* TIMEOUT */
            image_available_semaphore,
            VK_NULL_HANDLE, /* FENCE */ 
            &swapchain->image_index);

        if (!resolve_swapchain_state(swapchain, result))
            continue;

        struct reznor_device_frame *frame = device->header.frames[frame_index % device->header.frames_in_flight];
        frame->swapchains[frame->swapchain_count] = swapchain;
        frame->swapchain_image_available_semaphores[frame->swapchain_count] = image_available_semaphore;
        frame->swapchain_image_indices[frame->swapchain_count] = swapchain->image_index;
        frame->swapchain_count++;

        swapchain->image_available_semaphore_index = (swapchain->image_available_semaphore_index + 1) % device->header.frames_in_flight;
    }
}

FN_REZNOR_FRAME_BEGIN(vulkan)
{
    struct reznor_device *device = frame->device;
    struct reznor *reznor = device->header.reznor;

    /* TODO do rendering work */
    //VERIFY_VK(device->vkWaitForFences(device->logical, 1, &frame->fence, VK_TRUE, UINT32_MAX));
    VERIFY_VK(device->vkResetFences(device->logical, 1, &frame->fence));

    for (u32 i = 0; i < reznor->interface.thread_count; i++)
        VERIFY_VK(device->vkResetCommandPool(device->logical, frame->graphics_command_pools[i].command_pool, 0));
}

FN_REZNOR_FRAME_SUBMIT(vulkan)
{
    struct reznor_device *device = frame->device;
    struct reznor *reznor = device->header.reznor;

    /* setup the command buffer for drawing */
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };
    VERIFY_VK(device->vkBeginCommandBuffer(frame->graphics_command_buffer, &command_buffer_begin_info));

    /* execute secondary command buffers */
    for (u32 i = 0; i < reznor->interface.thread_count; i++) {
        struct vulkan_command_pool *pool = &frame->graphics_command_pools[i];
        device->vkCmdExecuteCommands(frame->graphics_command_buffer, pool->command_buffer_count, pool->command_buffers);
    }
    VERIFY_VK(device->vkEndCommandBuffer(frame->graphics_command_buffer));

    /* TODO do rendering work */
    return;

    /* submit to the graphics queue */
    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo graphics_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = frame->swapchain_count,
        .pWaitSemaphores = frame->swapchain_image_available_semaphores,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &frame->rendering_finished_semaphore,
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &frame->graphics_command_buffer,
    };
    VERIFY_VK(device->vkQueueSubmit(device->graphics_queue, 1, &graphics_submit_info, frame->fence));

    /* skip presentation if there are no swapchains */
    if (!frame->swapchain_count)
        return;

    VkResult swapchain_results[REZNOR_MAX_SWAPCHAINS];
    VkSwapchainKHR swapchains[REZNOR_MAX_SWAPCHAINS];
    for (u32 i = 0; i < frame->swapchain_count; i++)
        swapchains[i] = frame->swapchains[i]->swapchain;

    /* present the image */
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = frame->swapchain_count,
        .pWaitSemaphores = frame->swapchain_image_available_semaphores,
        .swapchainCount = frame->swapchain_count,
        .pSwapchains = swapchains,
        .pImageIndices = frame->swapchain_image_indices,
        .pResults = swapchain_results,
    };
    VkResult present_result = device->vkQueuePresentKHR(device->present_queue, &present_info);

    /* check results per swapchain */
    if (present_result != VK_SUCCESS) {
        for (u32 i = 0; i < frame->swapchain_count; i++) {
            struct reznor_swapchain *swapchain = frame->swapchains[i];
            VkResult result = swapchain_results[i];

            b32 resolved = resolve_swapchain_state(swapchain, result);

            if (!resolved) {
                log_error("Presentation failed without recovery for swapchain '%s' and rendering device '%s': %s.", 
                    swapchain->header.debug_name.ptr, device->header.name.ptr, vulkan_result_string(result));
            }
        }
    }
}
