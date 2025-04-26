#include "vk_xaku.h"

#ifdef XAKU_VULKAN
FN_XAKU_DEVICE_ASSEMBLY(vulkan)
{
    /* get memory requirements */
    usize device_bytes = A16(lake_sizeof(struct xaku_device));
    usize total_bytes = 
        device_bytes;
    usize alignment = lake_alignof(struct xaku_device);

    if (!assembly->host_allocation.data || assembly->host_allocation.size < total_bytes) {
        assembly->host_allocation.size = total_bytes;
        assembly->host_allocation.alignment = alignment;
        return lake_result_allocation_callback;
    }
    struct xaku_device *device = (struct xaku_device *)assembly->host_allocation.data;
    bedrock_memset(assembly->host_allocation.data, 0, total_bytes);

    u32 pd_index = assembly->physical_device_index;
    if (pd_index >= xaku->physical_device_count)
        pd_index = 0;
    const struct vulkan_physical_device *physical_device = &xaku->physical_devices[pd_index];

    device->xaku = xaku;
    device->riven = &xaku->interface.riven.context;
    device->properties = &physical_device->xaku_properties;
    device->assembly = *assembly;
    device->assembly.physical_device_index = pd_index;
    device->physical_details = physical_device;
    device->physical = physical_device->vk_physical_device;
    device->host_allocator = &xaku->allocator;

    /* TODO process required, explicit and implicit features */

    /* TODO call the helper to create the device itself */

    /* TODO assemble data structures of the device */

    (void)riven_inc_refcnt(&device->refcnt);
    *out_device = device;
    return lake_result_success;
}

FN_XAKU_DEVICE_DISASSEMBLY(vulkan)
{
    bedrock_assert_debug(device);

    /* TODO */

    if (device->logical)
        device->vkDestroyDevice(device->logical, device->host_allocator);
    bedrock_zerop(device);
}

FN_XAKU_DEVICE_QUEUE_COUNT(vulkan)
{
    (void)device;
    (void)queue_type;
    (void)out_count;
    return lake_result_success;
}

FN_XAKU_DEVICE_QUEUE_WAIT_IDLE(vulkan)
{
    (void)device;
    (void)queue;
    return lake_result_success;
}

FN_XAKU_DEVICE_WAIT_IDLE(vulkan)
{
    (void)device;
    return lake_result_success;
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

#endif /* XAKU_VULKAN */
