#include <lake/cobalt.h>

AMWAPI s32 cobalt_init(
    PFN_cobalt_entry_point entry_point__,
    cobalt                *co, 
    ipomoeaalba           *ia,
    hadopelagic           *hadal, 
    rivens_rift           *riven,
    const char            *application_name,
    u32                    application_version,
    thread_id             *threads,
    ssize                  thread_count,
    s32                    preferred_main_device_idx,
    s32                    max_device_count)
{
    s32 ret = result_success;

    assert_debug(entry_point__ && co && ia && hadal && riven && threads);
    iazerop(co);

    ret = entry_point__(co, ia);
    log_debug("%d", ret);
    if (ret != result_success) return ret;

#define COBALTCHECK(fn) \
        if (co->calls.fn == NULL) { log_warn("Missing Cobalt call in the rendering backend - 'PFN_cobalt_%s'.", #fn); ret = result_error_missing_procedure; }
    COBALTCHECK(renderer_init)
    COBALTCHECK(renderer_fini)
    COBALTCHECK(create_swapchain_surface)
    COBALTCHECK(construct_devices)
    COBALTCHECK(destroy_devices)
#undef COBALTCHECK
    if (ret == result_error_missing_procedure) return ret;

    arena_allocator temp_arena;
    arena_init(&temp_arena, 12 * 1024);
    co->backend_name = "null";

    ret = co->calls.renderer_init(co, ia, hadal, application_name, application_version, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Cobalt call for %s_renderer_init failed with return code '%d'.", co->backend_name, ret);
        arena_fini(&temp_arena);
        return ret;
    } else if (!co->renderer) {
        log_error("The rendering backend is missing."); 
        arena_fini(&temp_arena);
        return ret;
    }
    arena_reset(&temp_arena);
    ret = co->calls.construct_devices(co, riven, threads, thread_count, preferred_main_device_idx, max_device_count, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Cobalt call for %s_construct_devices failed with return code '%d'.", co->backend_name, ret);
        co->calls.renderer_fini(co);
        iazerop(co);
    } else if (co->device_count == 0) {
        log_error("No rendering devices were created.");
        co->calls.renderer_fini(co);
        iazerop(co);
        ret = result_error_invalid_engine_context;
    }

    arena_fini(&temp_arena);
    return ret;
}

AMWAPI void cobalt_fini(cobalt *co)
{
    if (!co) return;

    if (co->device_count > 0 || co->devices)
        co->calls.destroy_devices(co);
    if (co->renderer)
        co->calls.renderer_fini(co);
    iazerop(co);
}

AMWAPI s32 cobalt_entry_point(cobalt *co, ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_VULKAN
    if (cobalt_vulkan_entry_point(co, ia) == result_success) return result_success;
#endif
    if (cobalt_mock_entry_point(co, ia) == result_success) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_VULKAN
AMWAPI s32 cobalt_vulkan_entry_point(cobalt *co, ipomoeaalba *ia)
{
    (void)co; (void)ia;
    log_debug("The Vulkan backend is not built in this binary.");
    return result_feature_unsupported;
}
#endif /* AMW_NATIVE_VULKAN */
