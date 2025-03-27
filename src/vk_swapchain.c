#include "vk_reznor.h"

void vulkan_swapchain_destroy(struct reznor_swapchain *restrict swapchain)
{
    (void)swapchain;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, swapchain)
{
    (void)work;
}

FN_REZNOR_SWAPCHAIN_TRY_RECREATE(vulkan)
{
    (void)swapchain;
    return result_success;
}

FN_REZNOR_FRAME_NEXT_IMAGES(vulkan)
{
    (void)swapchains;
    (void)swapchain_count;
}
