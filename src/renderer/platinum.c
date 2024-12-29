#include <lake/bedrock/log.h>
#include <lake/datastructures/arena_allocator.h>

#include <lake/ipomoeaalba.h>
#include <lake/platynova.h>

static void terminate_backend(struct platinum *plat)
{
    if (plat->nova_count)
        platinum_implode_novas(plat);
    plat->calls.fini(plat->internal_plat);
}

AMWAPI int32_t AMWAPIENTRY 
platinum_init(
        PFN_platinum_entry_point entry_point__,
        struct platinum *plat,
        struct ipomoeaalba *ia,
        struct hadopelagic *hadal,
        const char *application_name,
        uint32_t application_version,
        uint32_t thread_count)
{
    int32_t result = result_success;

    if (!plat) return result_error_invalid_engine_context;
    iazerop(plat);

    (void)ia; // TODO

    if (!(entry_point__(plat))) return result_error_undefined; // TODO

    /* validate the internal backend calls */
#define PLATCHECK(fn) \
        if (plat->calls.fn == NULL) { log_warn("Missing call in internal Platinum backend - 'PFN_platinum_%s'", #fn); result = result_error_invalid_engine_context; }
    PLATCHECK(init)
    PLATCHECK(fini)
#undef PLATCHECK
    if (result != result_success) return result;

    struct arena_allocator temp_arena;
    arena_init(&temp_arena, 8 * 1024);

    plat->thread_count = thread_count;
    result = plat->calls.init(plat->internal_plat, hadal, application_name, application_version, &temp_arena);
    /* TODO Check if failed, and issue a backend fallback when possible. For now doesn't matter. */

    arena_fini(&temp_arena);
    return result;
}

AMWAPI void AMWAPIENTRY 
platinum_fini(
        struct platinum *plat)
{
    if (plat && plat->internal_plat)
        terminate_backend(plat);
}

#ifndef AMW_NATIVE_METAL
AMWAPI bool AMWAPIENTRY platinum_entry_point_metal(struct platinum *plat)
{
    (void)plat;
    log_debug("The Apple Metal rendering backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_METAL */

#ifndef AMW_NATIVE_D3D12
AMWAPI bool AMWAPIENTRY platinum_entry_point_d3d12(struct platinum *plat)
{
    (void)plat;
    log_debug("The Direct3D 12 rendering backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_D3D12 */

#ifndef AMW_NATIVE_VULKAN
AMWAPI bool AMWAPIENTRY platinum_entry_point_vulkan(struct platinum *plat)
{
    (void)plat;
    log_debug("The Vulkan rendering backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_VULKAN */

#ifndef AMW_NATIVE_WEBGPU
AMWAPI bool AMWAPIENTRY platinum_entry_point_webgpu(struct platinum *plat)
{
    (void)plat;
    log_debug("The WebGPU rendering backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_WEBGPU */

AMWAPI bool AMWAPIENTRY platinum_entry_point_proxy(struct platinum *plat)
{
    (void)plat;
    log_debug("The Proxy rendering backend is not available right now, TODO.");
    return false;
}

AMWAPI bool AMWAPIENTRY platinum_entry_point_mock(struct platinum *plat)
{
    (void)plat;
    log_debug("The mock debug rendering backend is not available right now, TODO.");
    return false;
}

AMWAPI bool AMWAPIENTRY platinum_entry_point(struct platinum *plat)
{
    if (platinum_vulkan_entry_point(plat)) return true;
#if 0
    if (platinum_metal_entry_point(plat)) return true;
    if (platinum_d3d12_entry_point(plat)) return true;
    if (platinum_webgpu_entry_point(plat)) return true;
    if (platinum_mock_entry_point(plat)) return true;
#endif
    return false;
}
