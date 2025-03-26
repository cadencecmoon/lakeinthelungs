#include "vk_reznor.h"

void vulkan_query_pool_destroy(struct reznor_query_pool *restrict query_pool)
{
    (void)query_pool;
}

FN_REZNOR_RESOURCE_ASSEMBLY(vulkan, query_pool)
{
    (void)device;
    (void)query_pool_count;
    (void)query_pool_configs;
    (void)memory_requirements;
    (void)out_assembly;
    return result_error;
}
