#include "vk_xaku.h"
#ifdef XAKU_VULKAN

FN_XAKU_QUERY_POOL_ASSEMBLY(vulkan)
{
    (void)device;
    (void)assembly;
    (void)out_query_pool;
    return xaku_result_max_enum;
}

FN_XAKU_QUERY_POOL_DISASSEMBLY(vulkan)
{
    (void)query_pool;
}

#endif /* XAKU_VULKAN */
