#include "vk_xaku.h"

#ifdef XAKU_VULKAN
FN_XAKU_DEVICE_ASSEMBLY(vulkan)
{
    /* discard the device early if requirements are not met */
    u32 pd_index = assembly->physical_device_index;
    if (pd_index >= xaku->physical_device_count)
        return lake_result_error_invalid_device_index;

    const struct vulkan_physical_device *physical_device = &xaku->physical_devices[pd_index];
    const struct xaku_device_properties *properties = &physical_device->xaku_properties;

    if (properties->missing_required_features != xaku_missing_required_feature_none)
        return lake_result_error_device_not_supported;

    if ((assembly->explicit_features & properties->explicit_features) != assembly->explicit_features)
        return lake_result_error_feature_not_present;

    /* get memory requirements */
    allocation->size = A8(lake_sizeof(struct xaku_device));
    allocation->alignment = lake_alignof(struct xaku_device);

    enum lake_result result = riven_allocation_callback(xaku->interface.riven.context.self, allocation);
    if (result != lake_result_success) return result;

    struct xaku_device *device = (struct xaku_device *)allocation->memory;
    bedrock_memset(allocation->memory, 0, allocation->size);

    device->xaku = xaku;
    device->riven = &xaku->interface.riven.context;
    device->properties = properties;
    device->physical_details = physical_device;
    device->host_allocator = &xaku->allocator;
    bedrock_memcpy(&device->assembly, assembly, lake_sizeof(struct xaku_device_assembly));
    device->assembly.physical_device_index = pd_index;

    /* create the Vulkan device */
    result = vulkan_device_assembly_helper(device, assembly);
    if (result != lake_result_success) {
        _xaku_vulkan_device_disassembly(device);
        return result;
    }

    /* TODO assemble data structures of the device */

    riven_inc_refcnt(&device->refcnt);
    *out_device = device;
    return lake_result_success;
}

FN_XAKU_DEVICE_DISASSEMBLY(vulkan)
{
    bedrock_assert_debug(device);

    /* TODO */

    if (device->vk_device)
        device->vkDestroyDevice(device->vk_device, device->host_allocator);
    bedrock_zerop(device);
}

FN_XAKU_DEVICE_QUEUE_COUNT(vulkan)
{
    bedrock_assert_debug(out_count);
    if (queue_type >= xaku_queue_type_max_enum)
        return lake_result_error_invalid_queue;
    *out_count = device->physical_details->queue_families[queue_type].queue_count;
    return lake_result_success;
}

FN_XAKU_DEVICE_QUEUE_WAIT_IDLE(vulkan)
{
    if (!vulkan_device_valid_queue(device, queue))
        return lake_result_error_invalid_queue;
    return vulkan_lake_result(device->vkQueueWaitIdle(vulkan_device_get_queue(device, queue)->vk_queue));
}

FN_XAKU_DEVICE_WAIT_IDLE(vulkan)
{
    return vulkan_lake_result(device->vkDeviceWaitIdle(device->vk_device));
}

FN_XAKU_DEVICE_SUBMIT(vulkan)
{
    (void)device;
    (void)cmd_list;
    return lake_result_success;
}

FN_XAKU_DEVICE_PRESENT(vulkan)
{
    (void)device;
    (void)swapchains;
    (void)swapchain_count;
    return lake_result_success;
}

FN_XAKU_MEMORY_BUFFER_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
}

FN_XAKU_MEMORY_TEXTURE_REQUIREMENTS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_requirements;
}

FN_XAKU_MEMORY_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)allocation;
    (void)out_memory;
    return lake_result_max_enum;
}

FN_XAKU_MEMORY_DISASSEMBLY(vulkan)
{
    (void)memory;
}

FN_XAKU_DEFERRED_RESOURCE_DISASSEMBLY(vulkan)
{
    (void)device;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_buffer;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_BUFFER_FROM_MEMORY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_buffer;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_TEXTURE(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_TEXTURE_FROM_MEMORY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_TEXTURE_VIEW(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_texture_view;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_SAMPLER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_sampler;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_TLAS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_tlas;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_TLAS_FROM_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_tlas;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_BLAS(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_blas;
    return lake_result_max_enum;
}

FN_XAKU_CREATE_BLAS_FROM_BUFFER(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_blas;
    return lake_result_max_enum;
}

FN_XAKU_GET_BUFFER_ASSEMBLY(vulkan)
{
    (void)device;
    (void)buffer;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_GET_TEXTURE_ASSEMBLY(vulkan)
{
    (void)device;
    (void)texture;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_GET_TEXTURE_VIEW_ASSEMBLY(vulkan)
{
    (void)device;
    (void)texture_view;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_GET_SAMPLER_ASSEMBLY(vulkan)
{
    (void)device;
    (void)sampler;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_GET_TLAS_BUILD_SIZES(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_sizes;
    return lake_result_max_enum;
}

FN_XAKU_GET_TLAS_ASSEMBLY(vulkan)
{
    (void)device;
    (void)tlas;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_GET_BLAS_BUILD_SIZES(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_sizes;
    return lake_result_max_enum;
}

FN_XAKU_GET_BLAS_ASSEMBLY(vulkan)
{
    (void)device;
    (void)blas;
    (void)out_assembly;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_BUFFER(vulkan)
{
    (void)device;
    (void)buffer;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_TEXTURE(vulkan)
{
    (void)device;
    (void)texture;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_TEXTURE_VIEW(vulkan)
{
    (void)device;
    (void)texture_view;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_SAMPLER(vulkan)
{
    (void)device;
    (void)sampler;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_TLAS(vulkan)
{
    (void)device;
    (void)tlas;
    return lake_result_max_enum;
}

FN_XAKU_DESTROY_BLAS(vulkan)
{
    (void)device;
    (void)blas;
    return lake_result_max_enum;
}

struct vulkan_queue *vulkan_device_get_queue(struct xaku_device *device, struct xaku_queue queue)
{
    u32 offsets[6] = { 
        XAKU_QUEUE_MAIN_INDEX,
        XAKU_QUEUE_COMPUTE_BEGIN_INDEX,
        XAKU_QUEUE_TRANSFER_BEGIN_INDEX,
        XAKU_QUEUE_SPARSE_BINDING_INDEX,
        XAKU_QUEUE_VIDEO_DECODE_INDEX,
        XAKU_QUEUE_VIDEO_ENCODE_INDEX,
    };
    return &device->queues[offsets[queue.type] + queue.index];
}

#endif /* XAKU_VULKAN */
