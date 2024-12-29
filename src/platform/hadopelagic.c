#include <lake/bedrock/log.h>
#include <lake/hadopelagic.h>
#include <lake/ipomoeaalba.h>

/* TODO replace malloc & strdup with the tagged heap allocator */
#include <stdlib.h>
#include <string.h>

static const char *backend_name_string(uint32_t id)
{
    switch (id) {
        case hadal_backend_win32: return "win32";
        case hadal_backend_cocoa: return "cocoa";
        case hadal_backend_ios: return "ios";
        case hadal_backend_android: return "android";
        case hadal_backend_wayland: return "wayland";
        case hadal_backend_xcb: return "xcb";
        case hadal_backend_kms: return "kms";
        case hadal_backend_headless: return "headless";
        default: break;
    }
    return "(none)";
}

static const struct { uint32_t backend; bool (*entry_point)(struct hadal *hadal, uint32_t desired_id); } supported_backends[] = {
#if defined(AMW_PLATFORM_WINDOWS)
    /* TODO */
#endif
#if defined(AMW_PLATFORM_MACOS)
    /* TODO */
#endif
#if defined(AMW_PLATFORM_IOS)
    /* TODO */
#endif
#if defined(AMW_PLATFORM_ANDROID)
    /* TODO */
#endif
#ifdef AMW_NATIVE_WAYLAND
    { hadal_backend_wayland, _hadal_wayland_entry_point},
#endif
#ifdef AMW_NATIVE_XCB
    /* TODO */
#endif
#ifdef AMW_NATIVE_KMS
    /* TODO */
#endif
};

static bool select_api(struct hadal *hadal, uint32_t desired_id, bool allow_headless)
{
    const size_t count = arraysize(supported_backends);

    if (desired_id >= hadal_backend_count) {
        log_error("Invalid hadal backend id %X", desired_id);
        return false;
    }

    /* only allow headless mode if explicitly requested */
    if (desired_id == hadal_backend_headless || (count == 0 && allow_headless)) {
        /* TODO init headless */
        return false;
    } else if (count == 0) {
        log_warn("Only headless mode display is available in this binary, it must be called explicitly.");
        return false;
    }

    if (desired_id == hadal_backend_invalid) {
        if (count == 1)
            return supported_backends[0].entry_point(hadal, desired_id);

        for (uint32_t i = 0; i < count; i++) {
            if (supported_backends[i].entry_point(hadal, desired_id))
                return true;
        }
        log_error("Failed to detect any supported display backends.");
    } else {
        for (uint32_t i = 0; i < count; i++) {
            if (supported_backends[i].backend == desired_id)
                return supported_backends[i].entry_point(hadal, desired_id);
        }
        log_error("The requested display backend is not supported on this platform.");
    }
    return false;
}


static void terminate(struct hadal *hadal)
{
    if (hadal->api.window_destroy)
        hadal->api.window_destroy(hadal);

    if (hadal->api.fini)
        hadal->api.fini(hadal);
}

bool hadal_api_is_supported(const uint32_t api)
{
    const uint32_t count = arraysize(supported_backends);

    if (api == hadal_backend_headless)
        return true;
    for (uint32_t i = 0; i < count; i++)
        if (api == supported_backends[i].backend)
            return true;
    return false;
}

struct hadal *hadal_init(
        enum hadal_backend backend, 
        uint32_t window_width,
        uint32_t window_height,
        const char *window_title,
        bool allow_headless)
{
    if (backend <= 0)
        backend = hadal_backend_invalid;

    struct hadal *hadal = (struct hadal *)malloc(sizeof(struct hadal));
    iazerop(hadal);

    /* connect to the desired display backend */
    if (!select_api(hadal, backend, allow_headless)) {
        log_error("Can't connect to a native display backend");
        terminate(hadal);
        free(hadal);
        return NULL;
    }
    hadal->api.last_api_fallback = hadal_backend_invalid;

    if (hadal->api.init(hadal) != result_success) {
        log_error("Can't initialize the display backend.");
        if (hadal_fallback(hadal, allow_headless) != result_success) {
            terminate(hadal);
            free(hadal);
            return NULL;
        }
    }

    assert_debug(window_title);
    char *dup_title = strdup(window_title); /* TODO replace strdup */

    /* create the window */
    hadal->window.title = dup_title;
    hadal->window.minwidth = 0;
    hadal->window.minheight = 0;
    hadal->window.maxwidth = 0;
    hadal->window.maxheight = 0;
    hadal->window.numer = 0;
    hadal->window.denom = 0;

    if (hadal->api.window_create(hadal, window_width, window_height) != result_success) {
        log_error("Can't create a system window.");
        if (hadal_fallback(hadal, allow_headless) != result_success ||
            hadal->api.window_create(hadal, window_width, window_height) != result_success) 
        {
            terminate(hadal);
            free(dup_title);
            free(hadal);
            return NULL;
        }
    }

    /* this flag is always set for proper hadal handles */
    at_fetch_or_relaxed(&hadal->flags, hadal_flag_initialized);

    return hadal;
}

int32_t hadal_fallback(struct hadal *hadal, bool allow_headless)
{
    if (!hadal || at_read_relaxed(&hadal->flags) & hadal_flag_initialized)
        return result_error_invalid_engine_context;

    /* For all platforms except headless mode there are no more than 3 native targets.
     * Those are: android + DRM/KMS ; Wayland + X11 + DRM/KMS. So 2 variables for API id work. */
    uint32_t now = hadal->api.api;
    uint32_t last = hadal->api.last_api_fallback;
    uint32_t supp = arraysize(supported_backends);

    if (now == hadal_backend_headless)
        return result_error_no_fallback;
    log_debug("The display backend %s is not sufficient, looking for a fallback...", backend_name_string(now));

    terminate(hadal);
    hadal->api.last_api_fallback = now;

    for (uint32_t i = 0; i < supp; i++) {
        if (supported_backends[i].backend == now || supported_backends[i].backend == last) 
            continue;
        if (supported_backends[i].entry_point(hadal, now) == result_success) {
            log_debug("Connected to a display fallback: %s", backend_name_string(supported_backends[i].backend));
            if (hadal->api.init(hadal) == result_success)
                return result_success;
            log_debug("Can't initialize the connected backend. Looking further...");
        }
        terminate(hadal);
    }
    if (allow_headless && select_api(hadal, hadal_backend_headless, true)) {
        log_debug("Connected to a headless fallback.");
        if (hadal->api.init(hadal) == result_success) 
            return result_success;
        log_debug("Can't initialize the headless display backend.");
    }

    hadal_fini(hadal);
    log_error("Hadal will terminate, no fallback.");
    return result_error_no_fallback;
}

void hadal_fini(struct hadal *hadal)
{
    if (!hadal)
        return;

    terminate(hadal);
    if (hadal->window.title) /* TODO replace stdlib free */
        free(hadal->window.title);
    free(hadal);
}

void hadal_window_size(
        struct hadal *hadal,
        uint32_t *out_width, 
        uint32_t *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (hadal && hadal->api.get_window_size)
        hadal->api.get_window_size(hadal, out_width, out_height);
}

void hadal_framebuffer_size(
        struct hadal *hadal,
        uint32_t *out_width, 
        uint32_t *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (hadal && hadal->api.get_framebuffer_size)
        hadal->api.get_framebuffer_size(hadal, out_width, out_height);
}

uint32_t hadal_flags(struct hadal *hadal, enum memory_model model)
{
    if (hadal)
        return at_read_explicit(&hadal->flags, model);
    return 0u;
}

at_uint32_t *hadal_at_flags(struct hadal *hadal)
{
    if (hadal)
        return &hadal->flags;
    return NULL;
}

uint32_t hadal_window_should_close(struct hadal *hadal, bool should)
{
    if (!hadal)
        return 0u;

    /* a simple atomic operation on the flags value is enough */
    if (should) {
        return at_fetch_or_explicit(&hadal->flags, hadal_flag_should_close, memory_model_acq_rel);
    } else {
        return at_fetch_and_explicit(&hadal->flags, ~hadal_flag_should_close, memory_model_acq_rel);
    }
}

uint32_t hadal_window_visible(struct hadal *hadal, bool visible)
{
    if (!hadal)
        return 0u;

    /* If the bit of hadal_flag_visible is different than bool visible, then the window state will change. */
    if ((at_read_explicit(&hadal->flags, memory_model_acquire) & hadal_flag_visible) != (visible ? hadal_flag_visible : 0u)) {
        /* backend call to update the window state */
        if (visible)
            hadal->api.visible_show(hadal);
        else 
            hadal->api.visible_hide(hadal);
        return at_fetch_xor_explicit(&hadal->flags, visible ? hadal_flag_visible : 0u, memory_model_acq_rel);
    }
    /* no modify/write operation was done */
    return at_read_relaxed(&hadal->flags);
}

#ifndef AMW_NDEBUG
bool _hadal_debug_validate_display_api(const struct hadopelagic_api *api)
{
    bool out = true;
    assert_debug(api != NULL);
    const char *name = backend_name_string(api->api);
#ifdef AMW_LOG_DISABLE_OUTPUT
    (void)name;
#endif

#define APICHECK(fn) \
        if (api->fn == NULL) { log_warn("Missing call in internal hadal API - '_hadal_%s_%s'", name, #fn); out = false; }
    APICHECK(init)
    APICHECK(fini)
    APICHECK(window_create)
    APICHECK(window_destroy)
    APICHECK(get_window_size)
    APICHECK(get_framebuffer_size)
    APICHECK(visible_show)
    APICHECK(visible_hide)
#undef APICHECK
    return out;
}
#endif

#ifndef AMW_PLATFORM_WINDOWS
AMWAPI bool AMWAPIENTRY hadopelagic_win32_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The Win32 display backend is not available in this build.");
    return false;
}
#endif /* AMW_PLATFORM_WINDOWS */

#ifndef AMW_PLATFORM_MACOS
AMWAPI bool AMWAPIENTRY hadopelagic_cocoa_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The Cocoa display backend is not available in this build.");
    return false;
}
#endif /* AMW_PLATFORM_MACOS */

#ifndef AMW_PLATFORM_IOS
AMWAPI bool AMWAPIENTRY hadopelagic_ios_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The iOS display backend is not available in this build.");
    return false;
}
#endif /* AMW_PLATFORM_IOS */

#ifndef AMW_PLATFORM_ANDROID
AMWAPI bool AMWAPIENTRY hadopelagic_android_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The Android display backend is not available in this build.");
    return false;
}
#endif /* AMW_PLATFORM_ANDROID */

#ifndef AMW_NATIVE_WAYLAND
AMWAPI bool AMWAPIENTRY hadopelagic_wayland_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The Wayland display backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_WAYLAND */

#ifndef AMW_NATIVE_XCB
AMWAPI bool AMWAPIENTRY hadopelagic_xcb_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The XCB (X11) display backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_XCB */

#ifndef AMW_NATIVE_DRM
AMWAPI bool AMWAPIENTRY hadopelagic_drm_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The DRM/KMS display backend is not available in this build.");
    return false;
}
#endif /* AMW_NATIVE_DRM */

AMWAPI bool AMWAPIENTRY hadopelagic_headless_entry_point(struct hadopelagic *hadal)
{
    (void)hadal;
    log_debug("The headless display backend is not available right now, TODO.");
    return false
}

AMWAPI bool AMWAPIENTRY hadopelagic_entry_point(struct hadopelagic *hadal)
{
    if (hadopelagic_wayland_entry_point(hadal)) return true;
#if 0
    if (hadopelagic_win32_entry_point(hadal)) return true;
    if (hadopelagic_cocoa_entry_point(hadal)) return true;
    if (hadopelagic_ios_entry_point(hadal)) return true;
    if (hadopelagic_android_entry_point(hadal)) return true;
    if (hadopelagic_xcb_entry_point(hadal)) return true;
    if (hadopelagic_drm_entry_point(hadal)) return true;
    if (hadopelagic_headless_entry_point(hadal)) return true;
#endif
    return false;
}
