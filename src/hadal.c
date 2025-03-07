#include <amw/hadal.h>
#include <amw/log.h>

static const PFN_rivens_encore g_native_encores[] = {
#if defined(PLATFORM_WINDOWS)
    (PFN_rivens_encore)hadal_win32_encore,
#elif defined(PLATFORM_APPLE_MACOS)
    (PFN_rivens_encore)hadal_cocoa_encore,
#elif defined(PLATFORM_APPLE_IOS)
    (PFN_rivens_encore)hadal_ios_encore,
#elif defined(PLATFORM_ANDROID)
    (PFN_rivens_encore)hadal_android_encore,
#elif defined(PLATFORM_EMSCRIPTEN)
    (PFN_rivens_encore)hadal_emscripten_encore,
#endif
#ifdef HADAL_WAYLAND
    (PFN_rivens_encore)hadal_wayland_encore,
#endif
#ifdef HADAL_XCB
    (PFN_rivens_encore)hadal_xcb_encore,
#endif
#ifdef HADAL_DRMKMS
    (PFN_rivens_encore)hadal_drmkms_encore,
#endif
    (PFN_rivens_encore)hadal_headless_encore,
};

const PFN_rivens_encore *hadal_acquire_native_encores(u32 *out_count, b32 fallback)
{
    u32 count = arraysize(g_native_encores);
    *out_count = fallback ? count : max(1, count-1);
    return g_native_encores;
}

b32 hadal_interface_validate(struct hadal *hadal)
{
    struct hadal_interface *interface = (struct hadal_interface *)hadal;
    const char *fmt = "Hadal '%s' is missing interface procedure - 'PFN_hadal_%s'.";
    b32 result = true;

#define CHECK(fn) \
    if (interface->fn == NULL) { log_warn(fmt, interface->header.name.ptr, #fn); result = false; }
    CHECK(acquire_framebuffer_extent)
#undef CHECK
    return result;
}

static void headless_interface_fini(struct hadal *hadal)
{
    (void)hadal;
}

static void headless_acquire_framebuffer_extent(struct hadal *hadal, u32 *width, u32 *height)
{
    (void)hadal;
    (void)width;
    (void)height;
}

RIVENS_ENCORE(hadal, headless)
{
    struct hadal_interface *interface = (struct hadal_interface *) 
        riven_alloc(overture->header.riven, overture->header.tag, sizeof(struct hadal_interface), _Alignof(struct hadal_interface));

    *interface = (struct hadal_interface) {
        .header = riven_write_interface_header(
            str_init("headless"), 
            headless_interface_fini, 
            hadal_interface_validate),
        .acquire_framebuffer_extent = headless_acquire_framebuffer_extent,
    };
    return (struct hadal *)interface;
}

/* XXX encore stubs, to be implemented */
#define ENCORE_STUB(backend) \
    RIVENS_ENCORE(hadal, backend) { log_error("Hadal encore '%s' is not yet implemented.", #backend); (void)overture; return NULL; }

#ifdef PLATFORM_WINDOWS
ENCORE_STUB(win32)
#endif
#ifdef PLATFORM_APPLE_MACOS
ENCORE_STUB(cocoa)
#endif
#ifdef PLATFORM_APPLE_IOS
ENCORE_STUB(ios)
#endif
#ifdef PLATFORM_ANDROID
ENCORE_STUB(android)
#endif
#ifdef PLATFORM_EMSCRIPTEN
ENCORE_STUB(html5)
#endif
#ifdef HADAL_XCB
ENCORE_STUB(xcb)
#endif
#ifdef HADAL_DRMKMS
ENCORE_STUB(drmkms)
#endif
