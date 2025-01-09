#include <lake/silver.h>

AMWAPI s32 silver_entry_point(struct silver *silv, struct ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_PIPEWIRE
    if (silver_pipewire_entry_point(silv, ia) == 0) return result_success;
#endif /* AMW_NATIVE_PIPEWIRE */
    if (silver_dummy_entry_point(silv, ia) == 0) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_PIPEWIRE
AMWAPI s32 silver_pipewire_entry_point(struct silver *silv, struct ipomoeaalba *ia) 
{ 
    (void)silv; (void)ia; 
    log_debug("The PipeWire audio backend is not available in this build."); 
    return result_feature_unsupported; 
}
#endif /* AMW_NATIVE_PIPEWIRE */
