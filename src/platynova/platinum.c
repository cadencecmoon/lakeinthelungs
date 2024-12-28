#include <lake/bedrock/log.h>
#include <lake/datastructures/arena_allocator.h>

#include <lake/ipomoea.h>
#include <lake/platynova.h>

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk_platynova.h"
#endif

static void terminate_backend(struct platinum *plat)
{
    if (plat->nova_count)
        platinum_implode_novas(plat);
    _platinum_vulkan_fini(plat->internal_plat);
}

int32_t platinum_init(
        struct platinum *plat,
        struct hadal *hadal,
        const char *application_name,
        uint32_t application_version,
        uint32_t thread_count)
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

    plat->thread_count = thread_count;
    result = _platinum_vulkan_init(plat->internal_plat, hadal, application_name, application_version, &temp_arena);
    /* TODO Check if failed, and issue a backend fallback when possible. For now doesn't matter. */

    arena_fini(&temp_arena);
    return result;
}

void platinum_fini(struct platinum *plat)
{
    if (plat && plat->internal_plat)
        terminate_backend(plat);
}
