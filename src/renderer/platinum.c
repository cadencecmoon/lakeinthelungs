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
    int32_t ret = result_success;

    if (!plat) return result_error_invalid_engine_context;
    iazerop(plat);

    (void)ia; // TODO

    if (!(entry_point__(plat))) return result_error_undefined; // TODO

    /* validate the internal backend calls */
#define PLATCHECK(fn) \
        if (plat->calls.fn == NULL) { log_warn("Missing call in internal Platinum backend - 'PFN_platinum_%s'", #fn); ret = result_error_missing_procedure; }
    PLATCHECK(init)
    PLATCHECK(fini)
#undef PLATCHECK
    if (ret == result_error_missing_procedure) return ret;

    struct arena_allocator temp_arena;
    arena_init(&temp_arena, 8 * 1024);

    plat->thread_count = thread_count;
    ret = plat->calls.init(plat->internal_plat, hadal, application_name, application_version, &temp_arena);
    /* TODO Check if failed, and issue a backend fallback when possible. For now doesn't matter. */

    arena_fini(&temp_arena);
    return ret;
}

AMWAPI void AMWAPIENTRY 
platinum_fini(
        struct platinum *plat)
{
    if (plat && plat->internal_plat)
        terminate_backend(plat);
}

AMWAPI int32_t AMWAPIENTRY 
platinum_entry_point(
        struct platinum *plat)
{
#ifdef AMW_NATIVE_VULKAN
    if (platinum_vulkan_entry_point(plat) == 0) return result_success;
#endif /* AMW_NATIVE_VULKAN */
    if (platinum_mock_entry_point(plat) == 0) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_VULKAN
AMWAPI int32_t AMWAPIENTRY platinum_vulkan_entry_point(struct platinum *plat) { (void)plat; log_debug("The Vulkan rendering backend is not available in this build."); return result_feature_unsupported; }
#endif /* AMW_NATIVE_VULKAN */
