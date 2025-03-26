#include "vk_reznor.h"

void vulkan_device_memory_destroy(struct reznor_device_memory *restrict device_memory)
{
    (void)device_memory;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, device_memory)
{
    (void)device;
    (void)device_memory_count;
    (void)device_memory_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}
