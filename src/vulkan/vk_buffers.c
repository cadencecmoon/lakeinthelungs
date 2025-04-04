#ifdef REZNOR_VULKAN
#include "vk_reznor.h"

FN_REZNOR_ASSEMBLY(vulkan, buffer) 
{
    assert_debug(work && work->assembly.buffer && work->memory.data && work->type == reznor_resource_type_buffer && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->buffer_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, buffer) 
{
    assert_debug(buffer && buffer->header.device);
    struct reznor_device *device = buffer->header.device;

    if (buffer->buffer != VK_NULL_HANDLE)
        device->vkDestroyBuffer(device->logical, buffer->buffer, &device->host_allocator);

    if (buffer->allocation->is_dedicated_allocation)
        _reznor_vulkan_device_memory_disassembly(buffer->allocation);

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->buffer_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(buffer);
}

FN_REZNOR_ASSEMBLY(vulkan, bottom_level) 
{
    assert_debug(work && work->assembly.bottom_level && work->memory.data && work->type == reznor_resource_type_bottom_level && work->device);

    struct reznor_device *device = work->device;
    assert_debug(device);
}

FN_REZNOR_DISASSEMBLY(vulkan, bottom_level) 
{
    assert_debug(bottom_level && bottom_level->header.device);
    struct reznor_device *device = bottom_level->header.device;

    (void)device;
}

FN_REZNOR_ASSEMBLY(vulkan, top_level) 
{
    assert_debug(work && work->assembly.top_level && work->memory.data && work->type == reznor_resource_type_top_level && work->device);
    struct reznor_device *device = work->device;

    (void)device;
}

FN_REZNOR_DISASSEMBLY(vulkan, top_level) 
{
    assert_debug(top_level && top_level->header.device);
    struct reznor_device *device = top_level->header.device;

    (void)device;
}

FN_REZNOR_ASSEMBLY(vulkan, shader_binding_table) 
{
    assert_debug(work && work->assembly.shader_binding_table && work->memory.data && work->type == reznor_resource_type_shader_binding_table && work->device);
    struct reznor_device *device = work->device;

    (void)device;
}

FN_REZNOR_DISASSEMBLY(vulkan, shader_binding_table) 
{
    assert_debug(shader_binding_table && shader_binding_table->header.device);
    struct reznor_device *device = shader_binding_table->header.device;

    (void)device;

    zerop(shader_binding_table);
}

#endif /* REZNOR_VULKAN */
