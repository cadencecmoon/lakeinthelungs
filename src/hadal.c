#include <amw/hadal.h>
#include <amw/log.h>

RIVEN_ENCORE(hadal, native)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#if defined(PLATFORM_WINDOWS)
        (PFN_riven_work)hadal_encore_win32,
#elif defined(PLATFORM_APPLE_MACOS)
        (PFN_riven_work)hadal_encore_cocoa,
#elif defined(PLATFORM_APPLE_IOS)
        (PFN_riven_work)hadal_encore_ios,
#elif defined(PLATFORM_ANDROID)
        (PFN_riven_work)hadal_encore_android,
#elif defined(PLATFORM_EMSCRIPTEN)
        (PFN_riven_work)hadal_encore_html5,
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
        encores[i]((riven_argument_t)create_info);

        if (*create_info->header.interface == NULL) 
            continue;

        const struct hadal_interface *interface = *create_info->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_hadal_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_warn(fmt, interface->header.name.ptr, #fn); valid = false; }

        VALIDATE(acquire_framebuffer_extent)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((riven_argument_t)interface);
        *create_info->header.interface = NULL;
    }
}

static void headless_acquire_framebuffer_extent(struct hadal *hadal, u32 *width, u32 *height)
{
    (void)hadal;
    (void)width;
    (void)height;
}

RIVEN_ENCORE(hadal, headless)
{
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    struct riven *riven = create_info->header.riven;
    riven_tag_t tag = create_info->header.tag;

    struct hadal_interface *interface = (struct hadal_interface *) 
        riven_alloc(riven, tag, sizeof(struct hadal_interface), _Alignof(struct hadal_interface));

    *interface = (struct hadal_interface){
        .header = {
            .name = str_init("hadal_headless"), 
            .riven = riven,
            .tag = tag,
            .fini = riven_work_nop,
        },
        .acquire_framebuffer_extent = headless_acquire_framebuffer_extent,
    };
    *create_info->header.interface = (riven_argument_t)(struct hadal *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef PLATFORM_WINDOWS
RIVEN_ENCORE_STUB(hadal, win32)
#endif
#ifdef PLATFORM_APPLE_MACOS
RIVEN_ENCORE_STUB(hadal, cocoa)
#endif
#ifdef PLATFORM_APPLE_IOS
RIVEN_ENCORE_STUB(hadal, ios)
#endif
#ifdef PLATFORM_ANDROID
RIVEN_ENCORE_STUB(hadal, android)
#endif
#ifdef PLATFORM_EMSCRIPTEN
RIVEN_ENCORE_STUB(hadal, html5)
#endif
#ifdef HADAL_XCB
RIVEN_ENCORE_STUB(hadal, xcb)
#endif
#ifdef HADAL_DRMKMS
RIVEN_ENCORE_STUB(hadal, drmkms)
#endif
