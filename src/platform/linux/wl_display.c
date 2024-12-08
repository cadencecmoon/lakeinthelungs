/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/common/assert.h>

#include "../hadopelagic.h"

#include "wayland-protocol.h"
#include "viewporter-protocol.h"
#include "xdg-shell-protocol.h"

#include <string.h>

static void handle_wm_base_ping(
        void *data,
        struct xdg_wm_base *wm_base,
        u32 serial)
{
    (void)data;
    xdg_wm_base_pong(wm_base, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {
    .ping = handle_wm_base_ping,
};

static void handle_registry_global(
        void *data,
        struct wl_registry *registry,
        u32 name,
        char const *interface,
        u32 version)
{
    (void)data;

    if (!strcmp(interface, "wl_compositor")) {
        HADAL.wl.compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(3, version));

    } else if (!strcmp(interface, "wl_subcompositor")) {
        HADAL.wl.subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);

    } else if (!strcmp(interface, "wl_shm")) {
        HADAL.wl.shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);

    } else if (!strcmp(interface, "wl_output")) {
        /* TODO add output */
        log_warn("TODO add output: %d, %d", name, version);

    } else if (!strcmp(interface, "wl_seat")) {
        if (!HADAL.wl.seat) {
            HADAL.wl.seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
            /* TODO add seat listener */
            /* TODO key repeat timerfd */
        }

    } else if (!strcmp(interface, "xdg_wm_base")) {
        HADAL.wl.shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(HADAL.wl.shell, &wm_base_listener, NULL);

    } else if (!strcmp(interface, "wp_viewporter")) {
        HADAL.wl.viewporter = wl_registry_bind(registry, name, &wp_viewporter_interface, 1);
    }
}

static void handle_registry_global_remove(
        void *data,
        struct wl_registry *registry,
        u32 name)
{
    /* unused */
    (void)registry;
    (void)data;

    for (u32 i = 0; i < HADAL.monitor_count; i++) {
        struct monitor *output = HADAL.monitors[i];
        if (output->wl.name == name) {
            /* TODO disconnect output */
            return;
        }
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

bool _hadal_wayland_connect(u32 desired_api)
{
    struct hadopelagic_api wayland = {
        .api = hadal_api_wayland,
        .init = _hadal_wayland_init,
        .fini = _hadal_wayland_fini,
        .window_create = _hadal_wayland_window_create,
        .window_destroy = _hadal_wayland_window_destroy,
        .get_window_size = _hadal_wayland_get_window_size,
        .get_framebuffer_size = _hadal_wayland_get_framebuffer_size,
        .visible_show = _hadal_wayland_visible_show,
        .visible_hide = _hadal_wayland_visible_hide,
    };

#ifndef AMW_NDEBUG
    if (!_hadal_debug_validate_display_api(&wayland)) {
        if (desired_api == hadal_api_wayland) {
            log_error("Internal display API for Wayland is not up to date.");
        }
        return false;
    }
#endif /* AMW_NDEBUG */

    wayland.wl.module_core = hadal_load_dll("libwayland-client.so.0");
    if (!wayland.wl.module_core) {
        if (desired_api == hadal_api_wayland) {
            log_error("Failed to load libwayland-client.so.0");
        }
        return false;
    }

    wayland.wl.display_connect = (PFN_wl_display_connect)hadal_get_proc_address(wayland.wl.module_core, "wl_display_connect");
    if (!wayland.wl.display_connect) {
        if (desired_api == hadal_api_wayland) {
            log_error("Failed to load libwayland-client entry point");
        }
        hadal_close_dll(wayland.wl.module_core);
        return false;
    }

    struct wl_display *display = wayland.wl.display_connect(NULL);
    if (!display) {
        if (desired_api == hadal_api_wayland) {
            log_error("Can't connect to a Wayland display.");
        }
        hadal_close_dll(wayland.wl.module_core);
        return false;
    }
    HADAL.wl.display = display;
    HADAL.api = wayland;

    log_debug("Connected to a Wayland display backend.");
    return true;
}

i32 _hadal_wayland_init(void)
{
    HADAL.wl.tag = "TODO"; /* TODO */

    wl_display_flush = (PFN_wl_display_flush)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_flush");
    wl_display_cancel_read = (PFN_wl_display_cancel_read)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_cancel_read");
    wl_display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_dispatch_pending");
    wl_display_read_events = (PFN_wl_display_read_events)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_read_events");
    wl_display_disconnect = (PFN_wl_display_disconnect)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_disconnect");
    wl_display_roundtrip = (PFN_wl_display_roundtrip)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_roundtrip");
    wl_display_get_fd = (PFN_wl_display_get_fd)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_get_fd");
    wl_display_prepare_read = (PFN_wl_display_prepare_read)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_display_prepare_read");
    wl_proxy_marshal = (PFN_wl_proxy_marshal)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_marshal");
    wl_proxy_add_listener = (PFN_wl_proxy_add_listener)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_add_listener");
    wl_proxy_destroy = (PFN_wl_proxy_destroy)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_destroy");
    wl_proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_marshal_constructor");
    wl_proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_marshal_constructor_versioned");
    wl_proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_get_user_data");
    wl_proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_set_user_data");
    wl_proxy_get_tag = (PFN_wl_proxy_get_tag)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_get_tag");
    wl_proxy_set_tag = (PFN_wl_proxy_set_tag)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_set_tag");
    wl_proxy_get_version = (PFN_wl_proxy_get_version)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_get_version");
    wl_proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        hadal_get_proc_address(HADAL.api.wl.module_core, "wl_proxy_marshal_flags");

    if (!HADAL.api.wl.display_flush ||
        !HADAL.api.wl.display_cancel_read ||
        !HADAL.api.wl.display_dispatch_pending ||
        !HADAL.api.wl.display_read_events ||
        !HADAL.api.wl.display_disconnect ||
        !HADAL.api.wl.display_roundtrip ||
        !HADAL.api.wl.display_get_fd ||
        !HADAL.api.wl.display_prepare_read ||
        !HADAL.api.wl.proxy_marshal ||
        !HADAL.api.wl.proxy_add_listener ||
        !HADAL.api.wl.proxy_destroy ||
        !HADAL.api.wl.proxy_marshal_constructor ||
        !HADAL.api.wl.proxy_marshal_constructor_versioned ||
        !HADAL.api.wl.proxy_get_user_data ||
        !HADAL.api.wl.proxy_set_user_data ||
        !HADAL.api.wl.proxy_get_tag ||
        !HADAL.api.wl.proxy_set_tag ||
        !HADAL.api.wl.proxy_get_version ||
        !HADAL.api.wl.proxy_marshal_flags) 
    {
        log_error("Failed to load libwayland-client functions.");
        return result_error_unknown; /* TODO */
    }

    HADAL.api.wl.module_cursor = hadal_load_dll("libwayland-cursor.so.0");
    if (!HADAL.api.wl.module_cursor) {
        log_error("Failed to load libwayland-cursor.so.0");
        return result_error_unknown; /* TODO */
    }

    wl_cursor_theme_load = (PFN_wl_cursor_theme_load)
        hadal_get_proc_address(HADAL.api.wl.module_cursor, "wl_cursor_theme_load");
    wl_cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        hadal_get_proc_address(HADAL.api.wl.module_cursor, "wl_cursor_theme_destroy");
    wl_cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        hadal_get_proc_address(HADAL.api.wl.module_cursor, "wl_cursor_theme_get_cursor");
    wl_cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        hadal_get_proc_address(HADAL.api.wl.module_cursor, "wl_cursor_image_get_buffer");

    if (!HADAL.api.wl.cursor_theme_load ||
        !HADAL.api.wl.cursor_theme_destroy ||
        !HADAL.api.wl.cursor_theme_get_cursor ||
        !HADAL.api.wl.cursor_image_get_buffer)
    {
        log_error("Failed to load libwayland-cursor functions.");
        return result_error_unknown; /* TODO */
    }

    HADAL.api.xkb.module = hadal_load_dll("libxkbcommon.so.0");
    if (!HADAL.api.xkb.module) {
        log_error("Failed to load libxkbcommon.so.0");
        return result_error_unknown; /* TODO */
    }

    xkb_context_new = (PFN_xkb_context_new)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_context_new");
    xkb_context_unref = (PFN_xkb_context_unref)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_context_unref");
    xkb_keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_keymap_new_from_string");
    xkb_keymap_unref = (PFN_xkb_keymap_unref)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_keymap_unref");
    xkb_keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_keymap_mod_get_index");
    xkb_keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_keymap_key_repeats");
    xkb_keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_keymap_key_get_syms_by_level");
    xkb_state_new = (PFN_xkb_state_new)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_new");
    xkb_state_unref = (PFN_xkb_state_unref)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_unref");
    xkb_state_key_get_syms = (PFN_xkb_state_key_get_syms)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_key_get_syms");
    xkb_state_update_mask = (PFN_xkb_state_update_mask)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_update_mask");
    xkb_state_key_get_layout = (PFN_xkb_state_key_get_layout)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_key_get_layout");
    xkb_state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_state_mod_index_is_active");
    xkb_compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_table_new_from_locale");
    xkb_compose_table_unref = (PFN_xkb_compose_table_unref)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_table_unref");
    xkb_compose_state_new = (PFN_xkb_compose_state_new)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_state_new");
    xkb_compose_state_unref = (PFN_xkb_compose_state_unref)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_state_unref");
    xkb_compose_state_feed = (PFN_xkb_compose_state_feed)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_state_feed");
    xkb_compose_state_get_status = (PFN_xkb_compose_state_get_status)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_state_get_status");
    xkb_compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        hadal_get_proc_address(HADAL.api.xkb.module, "xkb_compose_state_get_one_sym");

    if (!HADAL.api.xkb.context_new ||
        !HADAL.api.xkb.context_unref ||
        !HADAL.api.xkb.keymap_new_from_string ||
        !HADAL.api.xkb.keymap_unref ||
        !HADAL.api.xkb.keymap_mod_get_index ||
        !HADAL.api.xkb.keymap_key_repeats ||
        !HADAL.api.xkb.keymap_key_get_syms_by_level ||
        !HADAL.api.xkb.state_new ||
        !HADAL.api.xkb.state_unref ||
        !HADAL.api.xkb.state_key_get_syms ||
        !HADAL.api.xkb.state_update_mask ||
        !HADAL.api.xkb.state_key_get_layout ||
        !HADAL.api.xkb.state_mod_index_is_active ||
        !HADAL.api.xkb.compose_table_new_from_locale ||
        !HADAL.api.xkb.compose_table_unref ||
        !HADAL.api.xkb.compose_state_new ||
        !HADAL.api.xkb.compose_state_unref ||
        !HADAL.api.xkb.compose_state_feed ||
        !HADAL.api.xkb.compose_state_get_status ||
        !HADAL.api.xkb.compose_state_get_one_sym)
    {
        log_error("Failed to load libxkbcommon functions.");
        return result_error_unknown; /* TODO */
    }

    HADAL.wl.registry = wl_display_get_registry(HADAL.wl.display);
    wl_registry_add_listener(HADAL.wl.registry, &registry_listener, NULL);

    /* TODO key tables, scancodes, xkb_context */

    /* sync to get all registry objects */
    wl_display_roundtrip(HADAL.wl.display);

    /* sync to get initial output events */
    wl_display_roundtrip(HADAL.wl.display);

    if (!HADAL.wl.shell) {
        log_error("Wayland compositor is missing xdg-wm-base protocol support.");
        return result_error_unknown; /* TODO */
    } else if (!HADAL.wl.shm) {
        log_error("Wayland compositor is missing wl-shm.");
        return result_error_unknown; /* TODO */
    }

    /* TODO data device manager */

    return result_success;
}

void _hadal_wayland_fini(void)
{
    if (HADAL.api.xkb.module)
        hadal_close_dll(HADAL.api.xkb.module);

    if (HADAL.api.wl.module_cursor)
        hadal_close_dll(HADAL.api.wl.module_cursor);

    if (HADAL.wl.subcompositor)
        wl_subcompositor_destroy(HADAL.wl.subcompositor);
    if (HADAL.wl.compositor)
        wl_compositor_destroy(HADAL.wl.compositor);
    if (HADAL.wl.shm)
        wl_shm_destroy(HADAL.wl.shm);
    if (HADAL.wl.viewporter)
        wp_viewporter_destroy(HADAL.wl.viewporter);
    if (HADAL.wl.shell)
        xdg_wm_base_destroy(HADAL.wl.shell);
    if (HADAL.wl.seat)
        wl_seat_destroy(HADAL.wl.seat);
    if (HADAL.wl.registry)
        wl_registry_destroy(HADAL.wl.registry);
    if (HADAL.wl.display) {
        wl_display_flush(HADAL.wl.display);
        wl_display_disconnect(HADAL.wl.display);
    }
    if (HADAL.api.wl.module_core)
        hadal_close_dll(HADAL.api.wl.module_core);
}
