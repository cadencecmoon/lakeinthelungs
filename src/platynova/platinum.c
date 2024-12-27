#include <lake/bedrock/log.h>
#include <lake/datastructures/arena_allocator.h>

#include <lake/ipomoea.h>
#include <lake/platynova.h>

#include "internal_renderer.h"

static void terminate_backend(struct platinum *plat)
{
    if (plat->nova_count)
        platinum_implode_novas(plat);
    _platinum_vulkan_fini(plat->internal);
}

int32_t platinum_init(
        struct platinum *plat,
        struct hadal *hadal,
        const char *application_name,
        uint32_t application_version)
{
    int32_t result = result_success;

    if (!plat) 
        return result_error_invalid_engine_context;
    iazerop(plat);

    result = _platinum_vulkan_entry_point(plat);
    if (result != result_success)
        return result;

    struct arena_allocator temp_arena;
    arena_init(&temp_arena, 8 * 1024);

    result = _platinum_vulkan_init(plat->internal, hadal, application_name, application_version, &temp_arena);
    /* TODO Check if failed, and issue a backend fallback when possible. For now doesn't matter. */

    arena_fini(&temp_arena);
    return result;
}

void platinum_fini(struct platinum *plat)
{
    if (plat && plat->internal)
        terminate_backend(plat);
}
