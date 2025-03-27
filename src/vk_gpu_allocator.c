#include "vk_reznor.h"

void vulkan_device_memory_destroy(struct reznor_device_memory *restrict device_memory)
{
    (void)device_memory;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, device_memory)
{
    (void)work;
}
