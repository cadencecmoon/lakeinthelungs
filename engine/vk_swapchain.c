#include "vk_xaku.h"
#ifdef XAKU_VULKAN

FN_XAKU_SWAPCHAIN_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_swapchain;
    return lake_result_max_enum;
}

FN_XAKU_SWAPCHAIN_DISASSEMBLY(vulkan)
{
    (void)swapchain;
}

#endif /* XAKU_VULKAN */
