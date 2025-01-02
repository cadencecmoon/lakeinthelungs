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
    ssize                  thread_count)
{
    return result_success;
}

AMWAPI void cobalt_fini(cobalt *co)
{
    
}

AMWAPI s32 cobalt_entry_point(cobalt *co, ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_VULKAN
    cobalt_vulkan_entry_point(co, ia);
#endif
    cobalt_mock_entry_point(co, ia);
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
