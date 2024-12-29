#include <lake/bedrock/log.h>
#include <lake/platynova.h>

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk_platynova.h"
#endif

int32_t platinum_construct_novas(
        struct platinum *plat, 
        struct riven *riven,
        int32_t preferred_main_idx, 
        uint32_t max_nova_count,
        bool use_raytracing)
{
    uint32_t nova_count = 0;

    if (!plat || !plat->internal_plat)
        return result_error_invalid_engine_context;

    struct arena_allocator temp_arena;
    arena_init(&temp_arena, 16 * 1024);

    _platinum_vulkan_select_physical_devices(plat->internal_plat, &plat->internal_novas,
            &temp_arena, &nova_count, max_nova_count, preferred_main_idx);
    if (plat->internal_novas == NULL || nova_count == 0) {
        arena_fini(&temp_arena);
        log_error("No Vulkan-capable devices found.");
        return result_error_undefined; // TODO
    }

    if (_platinum_vulkan_create_devices(plat->internal_plat, plat->internal_novas, &temp_arena, nova_count, plat->thread_count) != result_success) {
        _platinum_vulkan_destroy_devices(plat->internal_novas, plat->nova_count, plat->thread_count);
        arena_fini(&temp_arena);
        log_error("Can't create a Vulkan device.");
        return result_error_undefined;
    }
    plat->nova_count = nova_count;

    /* Internal data has been allocated with malloc, not the temp arena. */
    struct platynova *novas = (struct platynova *)malloc(sizeof(struct platynova) * nova_count);
    for (uint32_t i = 0; i < nova_count; i++) {
        novas[i].plat = plat;
        at_store_relaxed(&novas[i].flags, i == 0u ? platynova_flag_is_the_main_device : 0u);
        _platynova_vulkan_setup_internal_device(&novas[i], plat->internal_novas, i, use_raytracing);
    }
    plat->novas = novas;

    arena_fini(&temp_arena);
    return result_success;
}

void platinum_implode_novas(struct platinum *plat)
{
    if (!plat)
        return;

    if (plat->novas) {
        free(plat->novas);
    }
    plat->novas = NULL;

    if (plat->internal_novas) {
        _platinum_vulkan_destroy_devices(plat->internal_novas, plat->nova_count, plat->thread_count);
        free(plat->internal_novas);
    }
    plat->internal_novas = NULL;
    plat->nova_count = 0;
}
