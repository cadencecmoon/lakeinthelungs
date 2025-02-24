#include <amw/hadal.h>
#include <amw/log.h>

static void finalize(struct hadopelagic *hadal)
{
    if (hadal->interface.display_fini)
        hadal->interface.display_fini(hadal);
    zerop(hadal);
}

s32 hadal_init(
    struct hadopelagic     *hadal,
    PFN_hadal_entry_point   entry,
    struct rivens          *riven,
    rivens_tag_t            tag,
    u32                     width,
    u32                     height,
    const struct string    *title,
    b32                     verbose_backend)
{
    s32 res = result_success;

    if (hadal->display) {
        log_debug("Hadal: the display backend is already initialized.");
        return result_success;
    }

    hadal->riven = riven;
    hadal->tag = tag;
    atomic_store_explicit(&hadal->window_width, width, memory_order_relaxed);
    atomic_store_explicit(&hadal->window_height, height, memory_order_relaxed);
    res = entry(hadal, verbose_backend);
    if (res != result_success)
        return res;

    const char *err = "Hadal: missing interface procedure - 'PFN_hadal_%s'.";
#define hadal_interface_check(fn) \
        if (hadal->interface.fn == NULL) { log_warn(err, #fn); res = result_error; }

    hadal_interface_check(display_init);
    hadal_interface_check(display_fini);
#ifdef HARRIDAN_VULKAN
    hadal_interface_check(create_vulkan_surface);
#endif

#undef hadal_interface_check
    if (res != result_success) {
        finalize(hadal);
        return res;
    }
    log_info("Hadal: entered the display backend of id %d '%s'.", hadal->interface.id, hadal->interface.name.ptr);

    hadal->window_title.ptr = memcpy(riven_alloc(riven, tag, title->len, 1), title->ptr, title->len);
    hadal->window_title.len = title->len;

    res = hadal->interface.display_init(hadal);
    if (res != result_success) {
        finalize(hadal);
        return res;
    }
    
    /* TODO continue here.. */
    
    return res;
}

void hadal_fini(struct hadopelagic *hadal)
{
    if (hadal->display) {
        if (hadal->interface.display_fini)
            hadal->interface.display_fini(hadal);
    }
    zerop(hadal);
}

/** Backend entry point definitions for builds where such a backend is not supported. */
#define hadal_stub(name)                                                                    \
    s32 hadal_##name##_entry_point(struct hadopelagic *hadal, b32 verbose) {                \
        (void)hadal;                                                                        \
        if (verbose)                                                                        \
            log_error("Hadal: display backend '%s' is not supported in this build.", #name);\
        return result_error;                                                                \
    }
#ifndef HADAL_WAYLAND
hadal_stub(wayland)
#endif

s32 hadal_entry_point(struct hadopelagic *hadal, b32 verbose)
{
#ifdef HADAL_WAYLAND
    if (hadal_wayland_entry_point(hadal, verbose) == result_success) return result_success;
#endif
    if (verbose)
        log_error("Hadal: there is no valid known entry point.");
    return result_error;
}
