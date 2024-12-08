/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include "hadopelagic.h"

#include <amw/common/assert.h>
#include <amw/ipomoea.h>

/* TODO replace malloc & strdup with the tagged heap allocator */
#include <stdlib.h>
#include <string.h>

struct hadal HADAL = {0};

static const char *api_name_string(u32 id)
{
    switch (id) {
        case hadal_api_win32: return "win32";
        case hadal_api_cocoa: return "cocoa";
        case hadal_api_ios: return "ios";
        case hadal_api_android: return "android";
        case hadal_api_wayland: return "wayland";
        case hadal_api_xcb: return "xcb";
        case hadal_api_kms: return "kms";
        case hadal_api_headless: return "headless";
        default: break;
    }
    return "(none)";
}

static const struct { u32 api; bool (*connect)(u32 desired_api); } supported_apis[] = {
#if defined(AMW_PLATFORM_WINDOWS)
    /* TODO */
#elif defined(AMW_PLATFORM_MACOS)
    /* TODO */
#elif defined(AMW_PLATFORM_IOS)
    /* TODO */
#elif defined(AMW_PLATFORM_ANDROID)
    /* TODO */
#endif
#ifdef AMW_NATIVE_WAYLAND
    { hadal_api_wayland, _hadal_wayland_connect},
#endif
#ifdef AMW_NATIVE_XCB
    /* TODO */
#endif
#ifdef AMW_NATIVE_KMS
    /* TODO */
#endif
};

static bool select_api(u32 api, bool allow_headless)
{
    const size_t count = arraysize(supported_apis);

    if (api >= hadal_api_count) {
        log_error("Invalid backend ID '%X : %s'", id, api_name_string(id));
        return false;
    }

    /* only allow headless mode if explicitly requested */
    if (api == hadal_api_headless || (count == 0 && allow_headless)) {
        /* TODO init headless */
        return false;
    } else if (count == 0) {
        log_warn("Only headless mode display is available in this binary, it must be called explicitly.");
        return false;
    }

    if (api == hadal_api_auto) {
        if (count == 1)
            return supported_apis[0].connect(api);

        for (u32 i = 0; i < count; i++) {
            fprintf(stderr, "mlemiauka 2222 %d\n", i);
            if (supported_apis[i].connect(api))
                return true;
        }
        log_error("Failed to detect any supported backends.");
    } else {
        for (u32 i = 0; i < count; i++) {
            fprintf(stderr, "mlemiauka 3333 %d\n", i);
            if (supported_apis[i].api == api)
                return supported_apis[i].connect(api);
        }
        log_error("The requested backend is not supported on this platform.");
    }
    return false;
}


static void terminate()
{
    if (HADAL.api.window_destroy)
        HADAL.api.window_destroy(HADAL.window);

    if (HADAL.window) {
        free(HADAL.window->title);
        free(HADAL.window);
    }

    if (HADAL.api.fini)
        HADAL.api.fini();
}

bool hadal_api_is_supported(const u32 api)
{
    const u32 count = arraysize(supported_apis);

    if (api == hadal_api_headless)
        return true;
    for (u32 i = 0; i < count; i++)
        if (api == supported_apis[i].api)
            return true;
    return false;
}

u32 hadal_api_get_current(void)
{
    return HADAL.api.api;
}

i32 hadal_init(
        enum hadal_api api, 
        u32 window_width,
        u32 window_height,
        const char *window_title,
        bool allow_headless)
{
    if (api <= 0)
        api = hadal_api_auto;

    /* if already initialized, just return early */
    if (at_read_relaxed(&HADAL.flags) & hadal_flag_initialized)
        return result_success;

    /* clear the global state */
    iazero(HADAL);

    /* connect to the desired display backend */
    if (!select_api(api, allow_headless)) {
        log_error("Can't connect to a native display backend");
        terminate();
        return result_error_unknown; /* TODO */
    }
    HADAL.api.last_api_fallback = hadal_api_auto;

    if (HADAL.api.init() != result_success) {
        log_error("Can't initialize the display backend.");
        if (hadal_fallback(allow_headless) != result_success) {
            terminate();
            return result_error_unknown; /* TODO */
        }
    }

    assert_debug(window_title);
    char *dup_title = strdup(window_title); /* TODO replace strdup */

    /* create the window */
    HADAL.window = (struct window *)malloc(sizeof(struct window)); /* TODO replace malloc */
    HADAL.window->title = dup_title;
    HADAL.window->minwidth = 0;
    HADAL.window->minheight = 0;
    HADAL.window->maxwidth = 0;
    HADAL.window->maxheight = 0;
    HADAL.window->numer = 0;
    HADAL.window->denom = 0;

    if (HADAL.api.window_create(HADAL.window, window_width, window_height) != result_success) {
        log_error("Can't create a system window.");
        if (hadal_fallback(allow_headless) != result_success ||
            HADAL.api.window_create(HADAL.window, window_width, window_height) != result_success) 
        {
            terminate();
            return result_error_unknown; /* TODO */
        }
    }

    /* this flag is always set for proper hadal handles */
    at_fetch_or_relaxed(&HADAL.flags, hadal_flag_initialized);

    return result_success;
}

i32 hadal_fallback(bool allow_headless)
{
    if (at_read_relaxed(&HADAL.flags) & hadal_flag_initialized)
        return result_error_invalid_context;

    /* For all platforms except headless mode there are no more than 3 native targets.
     * Those are: android + DRM/KMS ; Wayland + X11 + DRM/KMS. So 2 variables for API id work. */
    u32 now = HADAL.api.api;
    u32 last = HADAL.api.last_api_fallback;
    u32 supp = arraysize(supported_apis);

    if (now == hadal_api_headless)
        return result_error_no_fallback;
    log_debug("The display backend %s is not sufficient, looking for a fallback...", api_name_string(now));

    terminate();
    HADAL.api.last_api_fallback = now;

    for (u32 i = 0; i < supp; i++) {
        if (supported_apis[i].api == now || supported_apis[i].api == last) 
            continue;
        if (supported_apis[i].connect(now) == result_success) {
            log_debug("Connected to a display fallback: %s", api_name_string(supported_apis[i].api));
            if (HADAL.api.init() == result_success)
                return result_success;
            log_debug("Can't initialize the connected backend. Looking further...");
        }
        terminate();
    }
    if (allow_headless && select_api(hadal_api_headless, true)) {
        log_debug("Connected to a headless fallback.");
        if (HADAL.api.init() == result_success) 
            return result_success;
        log_debug("Can't initialize the headless display backend.");
    }

    terminate();
    log_error("Hadal will terminate, no fallback.");
    return result_error_no_fallback;
}

void hadal_fini(void)
{
    terminate();
    iazero(HADAL);
}

void hadal_window_size(
        u32 *out_width, 
        u32 *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (HADAL.api.get_window_size)
        HADAL.api.get_window_size(HADAL.window, out_width, out_height);
}

void hadal_framebuffer_size(
        u32 *out_width, 
        u32 *out_height)
{
    if (out_width)  *out_width = 0;
    if (out_height) *out_height = 0;

    if (HADAL.api.get_framebuffer_size)
        HADAL.api.get_framebuffer_size(HADAL.window, out_width, out_height);
}

u32 hadal_flags(void)
{
    return at_read_relaxed(&HADAL.flags);
}

void hadal_should_close(bool should)
{
    /* a simple atomic operation on the flags value is enough */
    if (should) {
        at_fetch_or_explicit(&HADAL.flags, hadal_flag_should_close, memory_model_release);
    } else {
        at_fetch_and_explicit(&HADAL.flags, ~hadal_flag_should_close, memory_model_release);
    }
}

void hadal_visible(bool visible)
{
    if (!(at_read_relaxed(&HADAL.flags) & hadal_flag_initialized))
        return;

    /* If the bit of hadal_flag_visible is different than bool visible, then the window state will change. */
    if ((at_read_explicit(&HADAL.flags, memory_model_acquire) & hadal_flag_visible) != (visible ? hadal_flag_visible : 0u)) {
        at_fetch_xor_explicit(&HADAL.flags, visible ? hadal_flag_visible : 0u, memory_model_acq_rel);

        /* backend call to update the window state */
        if (visible) {
            HADAL.api.visible_show(HADAL.window);
        } else {
            HADAL.api.visible_hide(HADAL.window);
        }
    }
}

bool _hadal_debug_validate_display_api(const struct hadopelagic_api *api)
{
    bool out = true;
#ifdef AMW_DEBUG
    assert_debug(api != NULL);
    AMW_UNUSED const char *name = api_name_string(api->api);

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
#endif
    return out;
}
