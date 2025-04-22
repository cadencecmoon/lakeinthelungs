#include <amwe/hadal.h>

static const PFN_riven_encore native_encores[] = {
#if defined(LAKE_PLATFORM_WINDOWS)
    (PFN_riven_encore)hadal_encore_win32,
#elif defined(LAKE_PLATFORM_APPLE_MACOS)
    (PFN_riven_encore)hadal_encore_cocoa,
#elif defined(LAKE_PLATFORM_APPLE_IOS)
    (PFN_riven_encore)hadal_encore_uikit,
#elif defined(LAKE_PLATFORM_ANDROID)
    (PFN_riven_encore)hadal_encore_android,
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
    (PFN_riven_encore)hadal_encore_html5,
#endif /* LAKE_PLATFORM_WINDOWS */
#ifdef HADAL_WAYLAND
    (PFN_riven_encore)hadal_encore_wayland,
#endif /* HADAL_WAYLAND */
#ifdef HADAL_XCB
    (PFN_riven_encore)hadal_encore_xcb,
#endif /* HADAL_XCB */
#ifdef HADAL_DRMKMS
    (PFN_riven_encore)hadal_encore_drmkms,
#endif /* HADAL_DRMKMS */
    (PFN_riven_encore)hadal_encore_null,
};

const PFN_riven_encore *hadal_native_encores(bool null_fallback, u32 *out_encore_count)
{
    u32 encore_count = lake_arraysize(native_encores);
    if (encore_count > 1 && !null_fallback)
        encore_count--;

    *out_encore_count = encore_count;
    return native_encores;
}

extern FN_RIVEN_INTERFACE_VALIDATION(hadal)
{
    const char *fmt = "'%s: %s' is missing an interface procedure - 'PFN_hadal_%s'.";
    bool valid = true;

#define VALIDATE(FN) \
    if (interface->FN == NULL) { bedrock_log_debug(fmt, interface->header.name, interface->header.backend, #FN); valid = false; }

    VALIDATE(window_create)
    VALIDATE(window_destroy)
    VALIDATE(window_visibility)
#ifdef XAKU_VULKAN
    VALIDATE(vulkan_write_instance)
    VALIDATE(vulkan_presentation_support)
    VALIDATE(vulkan_surface_create)
#endif /* XAKU_VULKAN */
#undef VALIDATE

    return valid;
}

#if defined(LAKE_PLATFORM_WINDOWS)
FN_RIVEN_ENCORE_STUB(hadal, win32)
#elif defined(LAKE_PLATFORM_APPLE_MACOS)
FN_RIVEN_ENCORE_STUB(hadal, cocoa)
#elif defined(LAKE_PLATFORM_APPLE_IOS)
FN_RIVEN_ENCORE_STUB(hadal, uikit)
#elif defined(LAKE_PLATFORM_ANDROID)
FN_RIVEN_ENCORE_STUB(hadal, android)
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
FN_RIVEN_ENCORE_STUB(hadal, html5)
#endif /* LAKE_PLATFORM_WINDOWS */
#ifdef HADAL_XCB
FN_RIVEN_ENCORE_STUB(hadal, xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_DRMKMS
FN_RIVEN_ENCORE_STUB(hadal, drmkms)
#endif /* HADAL_DRMKMS */

enum hadal_result hadal_display_init(
    const struct hadal_display_assembly *assembly,
    struct hadal_display                *out_display)
{
    out_display->hadal = assembly->hadal;
    out_display->strategy = assembly->strategy;

    riven_inc_refcnt(&out_display->hadal.header->refcnt);
    return hadal_result_success;
}

u32 hadal_display_fini(struct hadal_display *display, struct riven_work *out_zero_refcnt)
{
    if (!display->hadal.data)
        return 0;

    u32 prev = riven_dec_refcnt(&display->hadal.header->refcnt);
    if (prev == 1) {
        out_zero_refcnt->procedure = display->hadal.header->zero_ref_callback;
        out_zero_refcnt->userdata = display->hadal.data;
        out_zero_refcnt->name = "hadal_fini:zero_refcnt";
        bedrock_log_verbose("hadal zero ref trace");
    }
    return prev;
}
