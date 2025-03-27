#include "vk_reznor.h"

void vulkan_query_pool_destroy(struct reznor_query_pool *restrict query_pool)
{
    (void)query_pool;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, query_pool)
{
    (void)work;
}
