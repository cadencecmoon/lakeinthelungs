#include <lake/bedrock/log.h>
#include <lake/platynova.h>

#include "internal_renderer.h"

int32_t platinum_construct_novas(
        struct platinum *plat, 
        int32_t preferred_main_idx, 
        uint32_t max_nova_count, 
        uint32_t thread_count)
{
    if (!plat || !plat->internal)
        return result_error_invalid_engine_context;

    (void)preferred_main_idx;
    (void)max_nova_count;
    (void)thread_count;

    return result_success;
}

void platinum_implode_novas(struct platinum *plat)
{
    if (!plat || !plat->nova_count)
        return;
}
