/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/assert.h>
#include <lake/pelagia.h>

AMWAPI s32 pelagia_init(
    PFN_pelagia_entry_point entry_point__,
    struct pelagia         *pelagia,
    struct ipomoeaalba     *ia,
    struct hadopelagic     *hadal, 
    struct riven           *riven,
    const char             *application_name,
    u32                     application_version,
    thread_id              *threads,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    b32                     enable_vsync)
{
    s32 ret = result_success;

    assert_debug(entry_point__ && pelagia);
    if (pelagia->backend)
        return result_error_invalid_engine_context;
    iazerop(pelagia);

    ret = entry_point__(pelagia, ia);
    if (ret != result_success) return ret;

#define PELACHECK(fn) \
        if (pelagia->calls.fn == NULL) { log_warn("Missing Pelagia call in the rendering backend - 'PFN_pelagia_%s'.", #fn); ret = result_error_missing_procedure; }
    PELACHECK(renderer_init)
    PELACHECK(renderer_fini)
    PELACHECK(create_swapchain_surface)
    PELACHECK(construct_devices)
    PELACHECK(destroy_devices)
    PELACHECK(construct_swapchain_tear);
#undef COBALTCHECK
    if (ret == result_error_missing_procedure) return ret;

    struct arena_allocator temp_arena = {0};
    arena_init(&temp_arena, 12 * 1024);
    pelagia->backend_name = "null";

    ret = pelagia->calls.renderer_init(pelagia, ia, hadal, application_name, application_version, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Pelagia call for %s_renderer_init failed with return code '%d'.", pelagia->backend_name, ret);
        arena_fini(&temp_arena);
        return ret;
    } else if (!pelagia->backend) {
        log_error("The rendering backend is missing, invalid context."); 
        arena_fini(&temp_arena);
        return result_error_invalid_engine_context;
    }
    arena_reset(&temp_arena);
    ret = pelagia->calls.construct_devices(pelagia, riven, threads, thread_count, preferred_main_device_idx, max_device_count, &temp_arena);
    if (ret != result_success) {
        log_error("Internal Pelagia call for %s_construct_devices failed with return code '%d'.", pelagia->backend_name, ret);
        pelagia->calls.renderer_fini(pelagia);
        iazerop(pelagia);
        arena_fini(&temp_arena);
        return ret;
    } else if (pelagia->device_count == 0) {
        log_error("No rendering devices were created, invalid context.");
        pelagia->calls.renderer_fini(pelagia);
        iazerop(pelagia);
        arena_fini(&temp_arena);
        return result_error_invalid_engine_context;
    }

    struct pelagia_construct_swapchain_work swapchain_work = {
        .pelagia = pelagia,
        .hadal = hadal,
        .ia = ia,
        .use_vsync = enable_vsync,
        .out_result = result_success,
    };
    pelagia->calls.construct_swapchain_tear(&swapchain_work);

    if (swapchain_work.out_result != result_success) {
        log_error("Work 'pelagia_construct_swapchain_tear' failed at Pelagia initialization with return code '%d'.", swapchain_work.out_result);
        ret = swapchain_work.out_result;
    } 

    arena_fini(&temp_arena);
    return ret;
}

AMWAPI void pelagia_fini(struct pelagia *pelagia)
{
    if (!pelagia) return;

    if (pelagia->device_count > 0 || pelagia->devices)
        pelagia->calls.destroy_devices(pelagia);
    if (pelagia->backend)
        pelagia->calls.renderer_fini(pelagia);
    iazerop(pelagia);
}

AMWAPI s32 pelagia_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_VULKAN
    if (pelagia_vulkan_entry_point(pelagia, ia) == result_success) return result_success;
#endif
    if (pelagia_mock_entry_point(pelagia, ia) == result_success) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_VULKAN
AMWAPI s32 pelagia_vulkan_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia)
{
    (void)pelagia; (void)ia;
    log_debug("The Vulkan backend is not built in this binary.");
    return result_feature_unsupported;
}
#endif /* AMW_NATIVE_VULKAN */
