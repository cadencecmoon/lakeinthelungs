#ifdef REZNOR_VULKAN
#include "vk_reznor.h"

FN_REZNOR_DEVICE_MEMORY_REQUIREMENTS(vulkan) 
{
    (void)TODO;
    return 0;
}

FN_REZNOR_ASSEMBLY(vulkan, device_memory) 
{
    assert_debug(work && work->assembly.device_memory && work->memory.data && work->type == reznor_resource_type_device_memory && work->device);
    struct reznor_device *device = work->device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->device_memory_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
}

FN_REZNOR_DISASSEMBLY(vulkan, device_memory) 
{
    assert_debug(device_memory && device_memory->header.device);
    struct reznor_device *device = device_memory->header.device;

#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->device_memory_count, 1, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(device_memory);
}

#endif /* REZNOR_VULKAN */
