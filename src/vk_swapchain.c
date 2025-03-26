#include "vk_reznor.h"

void vulkan_swapchain_destroy(struct reznor_swapchain *restrict swapchain)
{
    (void)swapchain;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, swapchain)
{
    (void)device;
    (void)swapchain_count;
    (void)swapchain_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}

FN_REZNOR_SWAPCHAIN_TRY_RECREATE(vulkan)
{
    (void)swapchain;
    return result_success;
}

FN_REZNOR_SWAPCHAIN_NEXT_IMAGE(vulkan)
{
    (void)swapchain;
}
