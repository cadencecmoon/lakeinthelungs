/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/assert.h>
#include <lake/cobalt.h>

AMWAPI s32 cobalt_init(
    PFN_cobalt_entry_point entry_point__,
    struct cobalt         *cobalt,
    struct ipomoeaalba    *ia,
    struct hadopelagic    *hadal, 
    struct riven          *riven,
    const char            *application_name,
    u32                    application_version,
    thread_id             *threads,
    ssize                  thread_count,
    s32                    preferred_main_device_idx,
    s32                    max_device_count,
    b32                    enable_vsync)
{
    s32 ret = result_success;

    assert_debug(entry_point__ && cobalt && ia && hadal && riven && threads);
    iazerop(cobalt);

    ret = entry_point__(cobalt, ia);
    if (ret != result_success) return ret;

#define COBALTCHECK(fn) \
        if (cobalt->calls.fn == NULL) { log_warn("Missing Cobalt call in the rendering backend - 'PFN_cobalt_%s'.", #fn); ret = result_error_missing_procedure; }
    COBALTCHECK(renderer_init)
    COBALTCHECK(renderer_fini)
    COBALTCHECK(create_swapchain_surface)
    COBALTCHECK(construct_devices)
    COBALTCHECK(destroy_devices)
    COBALTCHECK(construct_swapchain_tear);
#undef COBALTCHECK
    if (ret == result_error_missing_procedure) return ret;

    struct arena_allocator temp_arena = {0};
    arena_init(&temp_arena, 12 * 1024);
    cobalt->backend_name = "null";

    ret = cobalt->calls.renderer_init(cobalt, ia, hadal, application_name, application_version, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Cobalt call for %s_renderer_init failed with return code '%d'.", cobalt->backend_name, ret);
        arena_fini(&temp_arena);
        return ret;
    } else if (!cobalt->backend) {
        log_error("The rendering backend is missing, invalid context."); 
        arena_fini(&temp_arena);
        return result_error_invalid_engine_context;
    }
    arena_reset(&temp_arena);
    ret = cobalt->calls.construct_devices(cobalt, riven, threads, thread_count, preferred_main_device_idx, max_device_count, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Cobalt call for %s_construct_devices failed with return code '%d'.", cobalt->backend_name, ret);
        cobalt->calls.renderer_fini(cobalt);
        iazerop(cobalt);
        arena_fini(&temp_arena);
        return ret;
    } else if (cobalt->device_count == 0) {
        log_error("No rendering devices were created, invalid context.");
        cobalt->calls.renderer_fini(cobalt);
        iazerop(cobalt);
        arena_fini(&temp_arena);
        return result_error_invalid_engine_context;
    }

    struct cobalt_construct_swapchain_work swapchain_work = {
        .cobalt = cobalt,
        .hadal = hadal,
        .ia = ia,
        .use_vsync = enable_vsync,
        .out_result = result_success,
    };
    cobalt->calls.construct_swapchain_tear(&swapchain_work);

    if (swapchain_work.out_result != result_success) {
        log_error("Cobalt construct swap chain work failed at initialization with return code '%d'.", swapchain_work.out_result);
        ret = swapchain_work.out_result;
    } 

    arena_fini(&temp_arena);
    return ret;
}

AMWAPI void cobalt_fini(struct cobalt *co)
{
    if (!co) return;

    if (co->device_count > 0 || co->devices)
        co->calls.destroy_devices(co);
    if (co->backend)
        co->calls.renderer_fini(co);
    iazerop(co);
}

AMWAPI s32 cobalt_entry_point(struct cobalt *cobalt, struct ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_VULKAN
    if (cobalt_vulkan_entry_point(cobalt, ia) == result_success) return result_success;
#endif
    if (cobalt_mock_entry_point(cobalt, ia) == result_success) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_VULKAN
AMWAPI s32 cobalt_vulkan_entry_point(struct cobalt *cobalt, struct ipomoeaalba *ia)
{
    (void)cobalt; (void)ia;
    log_debug("The Vulkan backend is not built in this binary.");
    return result_feature_unsupported;
}
#endif /* AMW_NATIVE_VULKAN */
