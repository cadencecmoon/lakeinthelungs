#include <lake/bedrock/log.h>
#include <lake/hadopelagic.h>
#include <lake/ipomoeaalba.h>

/* TODO replace malloc & strdup with the tagged heap allocator */
#include <stdlib.h>
#include <string.h>

static void terminate(struct hadopelagic *hadal)
{
    if (hadal->calls.display_fini)
        hadal->calls.display_fini(hadal);
    iazerop(hadal);
}

AMWAPI s32 hadal_init(
    PFN_hadal_entry_point entry_point__,
    struct hadopelagic   *hadal,
    struct ipomoeaalba   *ia,
    u32                   window_width,
    u32                   window_height,
    const char           *window_title)
{
    s32 ret;

    /* FIXME the tagged heap is not implemented for now, later the mallocs/free will be replaced with it as a common backend */
    if (!hadal || !ia || !entry_point__)
        return result_error_invalid_engine_context;

    if (hadal->display || at_read_relaxed(&hadal->flags) & hadal_flag_initialized)
        return result_success;
    iazerop(hadal);

    ret = entry_point__(hadal, ia);
    if (ret != result_success) { 
        log_error("Entry point for the display backend '%s' failed, returned %d.", hadal->backend_name ? hadal->backend_name : "?", ret);
        return ret;
    }

    /* validate the internal backend calls */
#define HADALCHECK(fn) \
        if (hadal->calls.fn == NULL) { log_warn("Missing call in internal display backend - 'PFN_hadal_%s'", #fn); ret = result_error_missing_procedure; }
    HADALCHECK(display_init)
    HADALCHECK(display_fini)
    HADALCHECK(get_window_size)
    HADALCHECK(get_framebuffer_size)
    HADALCHECK(show_window)
    HADALCHECK(hide_window)
    HADALCHECK(expose_native_window)
#undef HADALCHECK
    if (ret == result_error_missing_procedure) {
        terminate(hadal);
        return ret;
    }

    ret = hadal->calls.display_init(hadal, ia, window_width, window_height, window_title);
    if (ret != result_success) {
        terminate(hadal);
        return ret;
    }
    at_fetch_or_relaxed(&hadal->flags, hadal_flag_initialized);

    return result_success;
}

AMWAPI void hadal_fini(struct hadopelagic *hadal)
{
    if (hadal && at_read_relaxed(&hadal->flags) & hadal_flag_initialized)
        terminate(hadal);
}

AMWAPI void hadal_window_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (hadal && hadal->calls.get_window_size)
        hadal->calls.get_window_size(hadal, out_width, out_height);
}

AMWAPI void hadal_framebuffer_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (hadal && hadal->calls.get_framebuffer_size)
        hadal->calls.get_framebuffer_size(hadal, out_width, out_height);
}

AMWAPI u32 hadal_window_should_close(struct hadopelagic *hadal, b32 should)
{
    if (!hadal)
        return 0u;

    /* a simple atomic operation on the flags value is enough */
    if (should) {
        return at_fetch_or_explicit(&hadal->flags, hadal_flag_should_close, memory_model_seq_cst);
    } else {
        return at_fetch_and_explicit(&hadal->flags, ~hadal_flag_should_close, memory_model_seq_cst);
    }
}

AMWAPI u32 hadal_window_visible(struct hadopelagic *hadal, b32 visible)
{
    u32 flags = 0u;

    if (!hadal)
        return flags;
    flags = at_read_explicit(&hadal->flags, memory_model_acquire);

    /* If the bit of flag visible is different than bool visible, then the window state should change. */
    if ((flags & hadal_flag_visible) != (visible ? hadal_flag_visible : 0u)) {
        /* backend call to update the window state */
        if (visible)
            hadal->calls.show_window(hadal);
        else 
            hadal->calls.hide_window(hadal);
        /* flip the visible flag */
        return at_fetch_xor_explicit(&hadal->flags, hadal_flag_visible, memory_model_acq_rel);
    }
    /* no modify/write operation will be done */
    return flags;
}

AMWAPI s32 hadal_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia)
{
#ifdef AMW_NATIVE_WAYLAND
    if (hadal_wayland_entry_point(hadal, ia) == 0) return result_success;
#endif /* AMW_NATIVE_WAYLAND */
    if (hadal_headless_entry_point(hadal, ia) == 0) return result_success;
    return result_error_no_fallback;
}

#ifndef AMW_NATIVE_WAYLAND
AMWAPI s32 hadal_wayland_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia)
{ 
    (void)hadal; (void)ia; 
    log_debug("The Wayland display backend is not available in this build."); 
    return result_feature_unsupported; 
}
#endif /* AMW_NATIVE_WAYLAND */
