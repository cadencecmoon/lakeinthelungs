/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/common/assert.h>

#include "../hadopelagic.h"

#include "wayland-protocol.h"
#include "viewporter-protocol.h"
#include "xdg-shell-protocol.h"

static void resize_framebuffer(struct window *window, u32 width, u32 height)
{
    /* TODO add fractional scale */

    at_store_explicit(&window->wl.fb_width, width * 1.f, memory_model_seq_cst);   // window->wl.buffer_scale
    at_store_explicit(&window->wl.fb_height, height * 1.f, memory_model_seq_cst); // window->wl.buffer_scale
}

static bool resize_window(struct window *window, u32 width, u32 height)
{
    if (width == at_read_relaxed(&window->wl.width) && height == at_read_relaxed(&window->wl.height))
        return false;

    resize_framebuffer(window, width, height);

    /* TODO scaling viewport */

    return true;
}

static void handle_surface_enter(
        void *data,
        struct wl_surface *surface,
        struct wl_output *wl_output)
{
    /* unused */
    (void)surface;

    struct window *window = (struct window *)data;
    (void)window;

    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &HADAL.wl.tag)
        return;

    struct output *output = wl_output_get_user_data(wl_output);

    if (!output)
        return;

    /* TODO handle output scale buffers */
}

static void handle_surface_leave(
        void *data,
        struct wl_surface *surface,
        struct wl_output *wl_output)
{
    /* unused */
    (void)surface;

    struct window *window = (struct window *)data;
    (void)window;

    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &HADAL.wl.tag)
        return;

    /* TODO handle output scale buffers */
    (void)wl_output;
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
    .preferred_buffer_scale = NULL,
    .preferred_buffer_transform = NULL,
};

static void handle_xdg_toplevel_configure(
        void *data,
        struct xdg_toplevel *toplevel,
        i32 width, 
        i32 height,
        struct wl_array *states)
{
    /* unused */
    (void)toplevel;

    struct window *window = (struct window *)data;
    u32 *state;

    /* unset those flags from pending */
    window->wl.pending.flags &= ~(
            hadal_flag_activated 
          | hadal_flag_maximized 
          | hadal_flag_fullscreen
    );

    wl_array_for_each(state, states) {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            window->wl.pending.flags |= hadal_flag_maximized;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            window->wl.pending.flags |= hadal_flag_fullscreen;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            window->wl.pending.flags |= hadal_flag_is_resizing;
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            window->wl.pending.flags |= hadal_flag_activated;
            break;
        }
    }

    if (width && height) {
        window->wl.pending.width = width;
        window->wl.pending.height = height;
    } else {
        window->wl.pending.width = at_read_relaxed(&window->wl.width);
        window->wl.pending.height = at_read_relaxed(&window->wl.height);
    }
}

static void handle_xdg_toplevel_close(
        void *data, 
        struct xdg_toplevel *toplevel)
{
    /* unused */
    (void)toplevel;

    struct window *window = (struct window *)data;
    (void)window;

    at_fetch_or_explicit(&HADAL.flags, hadal_flag_should_close, memory_model_seq_cst);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = handle_xdg_toplevel_configure,
    .close = handle_xdg_toplevel_close,
};

static void handle_xdg_surface_configure(
        void *data,
        struct xdg_surface *surface,
        u32 serial)
{
    struct window *window = (struct window *)data;

    xdg_surface_ack_configure(surface, serial);
    if ((at_read_relaxed(&HADAL.flags) & hadal_flag_activated) != (window->wl.pending.flags & hadal_flag_activated)) {
        at_fetch_xor_explicit(&HADAL.flags, hadal_flag_activated, memory_model_acq_rel);
        if (!(at_read_relaxed(&HADAL.flags) & hadal_flag_activated)) {
            if (window->monitor && (at_read_relaxed(&HADAL.flags) & hadal_flag_auto_minimize)) {
                xdg_toplevel_set_minimized(window->wl.xdg.toplevel);
                // TODO window minimize event
            }
        }
    }
    if ((at_read_relaxed(&HADAL.flags) & hadal_flag_maximized) != (window->wl.pending.flags & hadal_flag_maximized)) {
        at_fetch_xor_explicit(&HADAL.flags, hadal_flag_maximized, memory_model_acq_rel);
        /* TODO window maximized event */
    }
    if (window->wl.pending.flags & hadal_flag_fullscreen) {
        at_fetch_or_relaxed(&HADAL.flags, hadal_flag_fullscreen);
    } else {
        at_fetch_and_relaxed(&HADAL.flags, ~hadal_flag_fullscreen);
    }
            
    u32 width = window->wl.pending.width;
    u32 height = window->wl.pending.height;

    if (!(at_read_relaxed(&HADAL.flags) & (hadal_flag_maximized | hadal_flag_fullscreen))) {
        if (window->numer != 0 && window->denom != 0) {
            const f32 aspect_ratio = (f32)width / (f32)height;
            const f32 target_ratio = (f32)window->numer / (f32)window->denom;
            if (aspect_ratio < target_ratio) {
                height = width / target_ratio;
            } else if (aspect_ratio > target_ratio) {
                width = height * target_ratio;
            }
        }
    }

    if (resize_window(window, width, height)) {
        /* TODO window resized event */
        if (at_read_relaxed(&HADAL.flags) & hadal_flag_visible) {
            /* TODO window content damaged */
        }
    }
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void update_size_limits(struct window *window)
{
    u32 minwidth, minheight, maxwidth, maxheight;

    if (at_read_relaxed(&HADAL.flags) & hadal_flag_resizable) {
        if (window->minwidth == 0 || window->minheight == 0) {
            minwidth = minheight = 0;
        } else {
            minwidth = window->minwidth;
            minheight = window->minheight;
        }
        if (window->maxwidth == 0 || window->maxheight == 0) {
            maxwidth = maxheight = 0;
        } else {
            maxwidth = window->maxwidth;
            maxheight = window->maxheight;
        }
    } else {
        minwidth = maxwidth = at_read_relaxed(&window->wl.width);
        minheight = maxheight = at_read_relaxed(&window->wl.height);
    }
    xdg_toplevel_set_min_size(window->wl.xdg.toplevel, minwidth, minheight);
    xdg_toplevel_set_max_size(window->wl.xdg.toplevel, maxwidth, maxheight);
}

static bool create_xdg_shell_objects(struct window *window)
{
    window->wl.xdg.surface = xdg_wm_base_get_xdg_surface(HADAL.wl.shell, window->wl.surface);
    if (!window->wl.xdg.surface) {
        log_error("Failed to create Wayland xdg-surface.");
        return false;
    }
    xdg_surface_add_listener(window->wl.xdg.surface, &xdg_surface_listener, window);

    window->wl.xdg.toplevel = xdg_surface_get_toplevel(window->wl.xdg.surface);
    if (!window->wl.xdg.toplevel) {
        log_error("Failed to create Wayland xdg-toplevel.");
        return false;
    }
    xdg_toplevel_add_listener(window->wl.xdg.toplevel, &xdg_toplevel_listener, window);

    if (window->wl.app_id)
        xdg_toplevel_set_app_id(window->wl.xdg.toplevel, window->wl.app_id);
    xdg_toplevel_set_title(window->wl.xdg.toplevel, window->title);

    if (window->monitor) {
        xdg_toplevel_set_fullscreen(window->wl.xdg.toplevel, window->monitor->wl.output);
        // TODO set idle inhibitor, true
    } else {
        if (at_read_relaxed(&HADAL.flags) & hadal_flag_maximized)
            xdg_toplevel_set_maximized(window->wl.xdg.toplevel);
        // TODO set idle inhibitor, false
    }
    update_size_limits(window);
    wl_surface_commit(window->wl.surface);
    wl_display_roundtrip(HADAL.wl.display);
    return true;
}

static void destroy_xdg_shell_objects(struct window *window)
{
    if (window->wl.xdg.toplevel)
        xdg_toplevel_destroy(window->wl.xdg.toplevel);
    if (window->wl.xdg.surface)
        xdg_surface_destroy(window->wl.xdg.surface);
    window->wl.xdg.toplevel = NULL;
    window->wl.xdg.surface = NULL;
}

static bool create_surface(struct window *window, u32 width, u32 height)
{
    window->wl.surface = wl_compositor_create_surface(HADAL.wl.compositor);
    if (!window->wl.surface) {
        log_error("Failed to create a Wayland window surface.");
        return false;
    }
    wl_proxy_set_tag((struct wl_proxy *)window->wl.surface, &HADAL.wl.tag);
    wl_surface_add_listener(window->wl.surface, &surface_listener, window);

    at_store_relaxed(&window->wl.width, width);
    at_store_relaxed(&window->wl.height, height);
    at_store_relaxed(&window->wl.fb_width, width);
    at_store_relaxed(&window->wl.fb_height, height);

    /* TODO fractional scale */

    return true;
}

i32 _hadal_wayland_window_create(struct window *window, u32 width, u32 height)
{
    if (!create_surface(window, width, height))
        return result_error_unknown; /* TODO */

    if (window->monitor || at_read_relaxed(&HADAL.flags) & hadal_flag_visible) {
        if (!create_xdg_shell_objects(window)) {
            return result_error_unknown; /* TODO */
        }
    }
    return result_success;
}

void _hadal_wayland_window_destroy(struct window *window)
{
    destroy_xdg_shell_objects(window);

    if (window->wl.surface)
        wl_surface_destroy(window->wl.surface);
}

void _hadal_wayland_get_window_size(const struct window *window, u32 *out_width, u32 *out_height)
{
    if (out_width)  *out_width  = at_read_explicit(&window->wl.width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&window->wl.height, memory_model_acquire);
}

void _hadal_wayland_get_framebuffer_size(const struct window *window, u32 *out_width, u32 *out_height)
{
    if (out_width)  *out_width  = at_read_explicit(&window->wl.fb_width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&window->wl.fb_height, memory_model_acquire);
}

void _hadal_wayland_visible_show(struct window *window)
{
    if (!window->wl.xdg.toplevel)
        create_xdg_shell_objects(window);
}

void _hadal_wayland_visible_hide(struct window *window)
{
    destroy_xdg_shell_objects(window);
    wl_surface_attach(window->wl.surface, NULL, 0, 0);
    wl_surface_commit(window->wl.surface);
}
