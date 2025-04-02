#include <amw/bedrock/log.h>
#include <amw/hadal.h>

RIVEN_ENCORE(hadal, native)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#if defined(PLATFORM_WINDOWS)
        (PFN_riven_work)hadal_encore_win32,
#elif defined(PLATFORM_APPLE_MACOS)
        (PFN_riven_work)hadal_encore_cocoa,
#elif defined(PLATFORM_APPLE_IOS)
        (PFN_riven_work)hadal_encore_uikit,
#elif defined(PLATFORM_ANDROID)
        (PFN_riven_work)hadal_encore_android,
#elif defined(PLATFORM_EMSCRIPTEN)
        (PFN_riven_work)hadal_encore_emscripten,
#endif
#ifdef HADAL_WAYLAND
        (PFN_riven_work)hadal_encore_wayland,
#endif
#ifdef HADAL_XCB
        (PFN_riven_work)hadal_encore_xcb,
#endif
#ifdef HADAL_DRMKMS
        (PFN_riven_work)hadal_encore_drmkms,
#endif
#ifndef HADAL_DISABLE_HEADLESS_FALLBACK
        (PFN_riven_work)hadal_encore_headless,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i]((void *)encore);

        if (*encore->header.interface == NULL) 
            continue;

        const struct hadal_interface *interface = *encore->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_hadal_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_debug(fmt, interface->header.name.ptr, #fn); valid = false; }

        VALIDATE(window_create)
        VALIDATE(window_destroy)
        VALIDATE(window_acquire_framebuffer_extent)
#ifdef REZNOR_VULKAN
        VALIDATE(vulkan_write_instance_procedures)
        VALIDATE(vulkan_surface_create)
        VALIDATE(vulkan_presentation_support)
#endif
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((void *)interface);
        *encore->header.interface = NULL;
    }
}

RIVEN_ENCORE(hadal, headless)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    struct hadal_interface *interface = (struct hadal_interface *) 
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct hadal_interface), _Alignof(struct hadal_interface));

    /* write the interface */
    interface->header.name = str_init("hadal_headless");
    interface->header.riven = encore->header.riven;
    interface->header.tag = encore->header.tag;
    interface->header.fini = (PFN_riven_work)riven_work_nop;

    *encore->header.interface = (void *)(struct hadal *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef PLATFORM_APPLE_MACOS
RIVEN_ENCORE_STUB(hadal, cocoa)
#endif
#ifdef PLATFORM_APPLE_IOS
RIVEN_ENCORE_STUB(hadal, uikit)
#endif
#ifdef PLATFORM_ANDROID
RIVEN_ENCORE_STUB(hadal, android)
#endif
#ifdef PLATFORM_EMSCRIPTEN
RIVEN_ENCORE_STUB(hadal, emscripten)
#endif
#ifdef HADAL_XCB
RIVEN_ENCORE_STUB(hadal, xcb)
#endif
#ifdef HADAL_DRMKMS
RIVEN_ENCORE_STUB(hadal, drmkms)
#endif
