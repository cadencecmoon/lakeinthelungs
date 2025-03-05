#include <amw/hadal.h>
#include <amw/log.h>

struct hadopelagic *hadal_init(
    const PFN_hadal_entry_point          *entries,
    u32                                   entry_count,
    const struct hadopelagic_create_info *create_info,
    b32                                   verbose)
{
    struct hadopelagic *hadal = NULL;
    if (entry_count == 0) {
        if (verbose)
            log_error("Hadal: no entry points were given.");
        return NULL;
    }

    for (u32 o = 0; o < entry_count; o++) {
        struct hadopelagic_interface interface = {0};
        s32 res = entries[o](&interface, create_info, verbose);

        if (res == result_reiterate) {
            hadal = (struct hadopelagic *)interface.internal;
            if (verbose)
                log_info("Hadal: sharing an existing display backend of id %u '%s'.", hadal->interface.id, hadal->interface.name.ptr);
            return hadal;
        } else if (res != result_success) {
            continue;
        }

        const char *err = "Hadal: id %u '%s' is missing interface procedure - 'PFN_hadal_%s'.";
#define INTERFACE_CHECK(fn) \
        if (interface.fn == NULL) { log_warn(err, interface.id, interface.name.ptr, #fn); res = result_error; }

        INTERFACE_CHECK(display_init);
        INTERFACE_CHECK(display_fini);
        INTERFACE_CHECK(get_framebuffer_extent);
        INTERFACE_CHECK(create_swapchain_surface);

#undef INTERFACE_CHECK
        if (res != result_success) {
            if (interface.display_fini)
                interface.display_fini(interface.internal);
            continue;
        }

        if (hadal == NULL) {
            hadal = (struct hadopelagic *)riven_alloc(create_info->riven, create_info->tag, sizeof(struct hadopelagic), _Alignof(struct hadopelagic));
            zerop(hadal);
        }

        hadal->interface = interface;
        hadal->riven = create_info->riven;
        hadal->tag = create_info->tag;
        atomic_store_explicit(&hadal->window_width, create_info->window_width, memory_order_relaxed);
        atomic_store_explicit(&hadal->window_height, create_info->window_height, memory_order_relaxed);

        res = hadal->interface.display_init(hadal, create_info);
        if (res != result_success) {
            hadal_fini(hadal);
            continue;
        }
        if (verbose)
            log_info("Hadal: initialized a display backend of id %d '%s'.", interface.id, interface.name.ptr);

        hadal->window_title.ptr = memcpy(riven_alloc(hadal->riven, hadal->tag, create_info->window_title->len, 1), 
            create_info->window_title->ptr, create_info->window_title->len);
        hadal->window_title.len = create_info->window_title->len;
        return hadal;
    }
    if (verbose)
        log_error("Hadal: tried %u entry points, none of them resulted in a valid display backend.", entry_count);
    return NULL;
}

void hadal_fini(struct hadopelagic *hadal)
{
    if (hadal->interface.display_fini)
        hadal->interface.display_fini(hadal->interface.internal);
    zerop(hadal);
}

/** Backend entry point definitions for builds where such a backend is not supported. */
#define ENTRY_POINT_STUB(TYPE)                                                          \
    HADAL_ENTRY_POINT(TYPE) {                                                           \
        (void)create_info;                                                              \
        *interface = (struct hadopelagic_interface){                                    \
            .id = hadal_backend_##TYPE,                                                 \
            .name = string_init(#TYPE),                                                 \
        };                                                                              \
        if (verbose)                                                                    \
            log_error("Hadal: display backend %u '%s' is not supported in this build.", \
                interface->id, interface->name.ptr);                                    \
        return result_error;                                                            \
    }

/* TODO to be implemented */
ENTRY_POINT_STUB(win32)
ENTRY_POINT_STUB(cocoa)
ENTRY_POINT_STUB(ios)
ENTRY_POINT_STUB(android)
ENTRY_POINT_STUB(html5)
ENTRY_POINT_STUB(xcb)
ENTRY_POINT_STUB(drmkms)

#ifndef PLATFORM_WINDOWS
    /* TODO */
#endif
#ifndef PLATFORM_MACOS
    /* TODO */
#endif
#ifndef PLATFORM_IOS
    /* TODO */
#endif
#ifndef PLATFORM_ANDROID
    /* TODO */
#endif
#ifndef PLATFORM_EMSCRIPTEN
    /* TODO */
#endif
#ifndef HADAL_WAYLAND
    ENTRY_POINT_STUB(wayland)
#endif
#ifndef HADAL_XCB
    /* TODO */
#endif
#ifndef HADAL_DRMKMS
    /* TODO */
#endif
#undef ENTRY_POINT_STUB

AMWAPI attr_nonnull_all 
s32 hadal_entry_point(_HADAL_ENTRY_POINT_ARGS)
{
    (void)interface; (void)create_info; (void)verbose;

    s32 res = result_error;
#define ENTRY_POINT_CHECK(name)                                        \
    res = hadal_##name##_entry_point(interface, create_info, verbose); \
    if (res == result_success || res == result_reiterate) return res;

#ifdef PLATFORM_WINDOWS
    ENTRY_POINT_CHECK(win32)
#endif
#ifdef PLATFORM_MACOS
    ENTRY_POINT_CHECK(cocoa)
#endif
#ifdef PLATFORM_IOS
    ENTRY_POINT_CHECK(ios)
#endif
#ifdef PLATFORM_ANDROID
    ENTRY_POINT_CHECK(android)
#endif
#ifdef PLATFORM_EMSCRIPTEN
    ENTRY_POINT_CHECK(html5)
#endif
#ifdef HADAL_WAYLAND
    ENTRY_POINT_CHECK(wayland)
#endif
#ifdef HADAL_XCB
    ENTRY_POINT_CHECK(xcb)
#endif
#ifdef HADAL_DRMKMS
    ENTRY_POINT_CHECK(drmkms)
#endif

#undef ENTRY_POINT_CHECK
    interface->id = hadal_backend_invalid;
    interface->name = string_init("auto");
    return res;
}

/* headless display backend */

HADAL_ENTRY_POINT(headless)
{
    (void)create_info; (void)verbose;

    *interface = (struct hadopelagic_interface){
        .id = hadal_backend_headless,
        .name = string_init("headless"),
    };
    return result_success;
}
