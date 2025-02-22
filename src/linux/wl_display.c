#include "wl_hadal.h"

#include <amw/log.h>
#include <amw/plugin.h>

#include <sys/timerfd.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

struct wayland_display *g_wl_hadal = NULL;

#include <wayland-protocol-code.h>
#include <xdg-shell-protocol-code.h>

s32 hadal_wayland_entry_point(struct hadopelagic *hadal, b32 verbose)
{
    if (UNLIKELY(g_wl_hadal != NULL)) {
        if (verbose)
            log_error("Only one Wayland display at a time is supported.");
        return result_error;
    }

    void *client = plugin_load_dll("libwayland-client.so.0");
    if (!client) {
        if (verbose)
            log_error("Missing libwayland-client.so.0 library.");
        return result_error;
    }

    PFN_wl_display_connect connect = (PFN_wl_display_connect)plugin_get_proc_address(client, "wl_display_connect");
    if (!connect) {
        if (verbose) 
            log_error("Missing libwayland-client entry point procedure.");
        plugin_close_dll(client);
        return result_error;
    }
    struct wl_display *display = connect(NULL);
    if (!display) {
        if (verbose)
            log_error("Can't connect to a Wayland display. Make sure that a Wayland compositor is currently running.");
        plugin_close_dll(client);
        return result_error;
    }

    struct wayland_display *wl = (struct wayland_display *)riven_alloc(hadal->riven, hadal->tag, sizeof(struct wayland_display), _Alignof(struct wayland_display));
    zerop(wl);

    wl->api.module_core = client;
    wl->api.wl.display_connect = connect;
    wl->display = display;
    hadal->display = g_wl_hadal = wl;

    hadal->interface = (struct hadal_interface){
        .id = hadal_backend_wayland,
        .name = str_init("wayland"),
        .display_init = hadal_wayland_display_init,
        .display_fini = hadal_wayland_display_fini,
    };
    return result_success;
}

static void handle_wm_base_ping(
    void               *data,
    struct xdg_wm_base *wm_base,
    u32                 serial)
{
    (void)data;
    xdg_wm_base_pong(wm_base, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {
    .ping = handle_wm_base_ping,
};

static void handle_registry_global(
    void               *data,
    struct wl_registry *registry,
    u32                 name,
    char const         *interface,
    u32                 version)
{
    struct hadopelagic  *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (!strcmp(interface, "wl_compositor")) {
        wl->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(3, version));

    } else if (!strcmp(interface, "wl_subcompositor")) {
        wl->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);

    } else if (!strcmp(interface, "wl_shm")) {
        wl->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);

    } else if (!strcmp(interface, "wl_output")) {
        log_debug("Add wayland output: %u ver. %u", name, version);

    } else if (!strcmp(interface, "wl_seat")) {
        if (!wl->seat) {
            wl->seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
            /* TODO listener */

            if (wl_seat_get_version(wl->seat) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION) {
                wl->key_repeat_timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
            }
        }
    } else if (!strcmp(interface, "xdg_wm_base")) {
        wl->shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(wl->shell, &wm_base_listener, hadal);
    }
}

static void handle_registry_global_remove(
    void               *data,
    struct wl_registry *registry,
    u32                 name)
{
    /* unused */
    (void)registry;

    (void)data;
    (void)name;
    //struct hadopelagic *hadal = (struct hadopelagic *)data;
    //struct wayland_display *wl = (struct wayland_display *)hadal->display;

    /* TODO delete outputs */
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

static void create_key_tables(struct hadopelagic *hadal)
{
    memset(hadal->keycodes, -1, sizeof(hadal->keycodes));
    memset(hadal->scancodes, -1, sizeof(hadal->scancodes));

    hadal->keycodes[KEY_GRAVE]      = keycode_grave_accent;
    hadal->keycodes[KEY_1]          = keycode_1;
    hadal->keycodes[KEY_2]          = keycode_2;
    hadal->keycodes[KEY_3]          = keycode_3;
    hadal->keycodes[KEY_4]          = keycode_4;
    hadal->keycodes[KEY_5]          = keycode_5;
    hadal->keycodes[KEY_6]          = keycode_6;
    hadal->keycodes[KEY_7]          = keycode_7;
    hadal->keycodes[KEY_8]          = keycode_8;
    hadal->keycodes[KEY_9]          = keycode_9;
    hadal->keycodes[KEY_0]          = keycode_0;
    hadal->keycodes[KEY_SPACE]      = keycode_space;
    hadal->keycodes[KEY_MINUS]      = keycode_minus;
    hadal->keycodes[KEY_EQUAL]      = keycode_equal;
    hadal->keycodes[KEY_Q]          = keycode_q;
    hadal->keycodes[KEY_W]          = keycode_w;
    hadal->keycodes[KEY_E]          = keycode_e;
    hadal->keycodes[KEY_R]          = keycode_r;
    hadal->keycodes[KEY_T]          = keycode_t;
    hadal->keycodes[KEY_Y]          = keycode_y;
    hadal->keycodes[KEY_U]          = keycode_u;
    hadal->keycodes[KEY_I]          = keycode_i;
    hadal->keycodes[KEY_O]          = keycode_o;
    hadal->keycodes[KEY_P]          = keycode_p;
    hadal->keycodes[KEY_LEFTBRACE]  = keycode_left_bracket;
    hadal->keycodes[KEY_RIGHTBRACE] = keycode_right_bracket;
    hadal->keycodes[KEY_A]          = keycode_a;
    hadal->keycodes[KEY_S]          = keycode_s;
    hadal->keycodes[KEY_D]          = keycode_d;
    hadal->keycodes[KEY_F]          = keycode_f;
    hadal->keycodes[KEY_G]          = keycode_g;
    hadal->keycodes[KEY_H]          = keycode_h;
    hadal->keycodes[KEY_J]          = keycode_j;
    hadal->keycodes[KEY_K]          = keycode_k;
    hadal->keycodes[KEY_L]          = keycode_l;
    hadal->keycodes[KEY_SEMICOLON]  = keycode_semicolon;
    hadal->keycodes[KEY_APOSTROPHE] = keycode_apostrophe;
    hadal->keycodes[KEY_Z]          = keycode_z;
    hadal->keycodes[KEY_X]          = keycode_x;
    hadal->keycodes[KEY_C]          = keycode_c;
    hadal->keycodes[KEY_V]          = keycode_v;
    hadal->keycodes[KEY_B]          = keycode_b;
    hadal->keycodes[KEY_N]          = keycode_n;
    hadal->keycodes[KEY_M]          = keycode_m;
    hadal->keycodes[KEY_COMMA]      = keycode_comma;
    hadal->keycodes[KEY_DOT]        = keycode_period;
    hadal->keycodes[KEY_SLASH]      = keycode_slash;
    hadal->keycodes[KEY_BACKSLASH]  = keycode_backslash;
    hadal->keycodes[KEY_ESC]        = keycode_escape;
    hadal->keycodes[KEY_TAB]        = keycode_tab;
    hadal->keycodes[KEY_LEFTSHIFT]  = keycode_left_shift;
    hadal->keycodes[KEY_RIGHTSHIFT] = keycode_right_shift;
    hadal->keycodes[KEY_LEFTCTRL]   = keycode_left_control;
    hadal->keycodes[KEY_RIGHTCTRL]  = keycode_right_control;
    hadal->keycodes[KEY_LEFTALT]    = keycode_left_alt;
    hadal->keycodes[KEY_RIGHTALT]   = keycode_right_alt;
    hadal->keycodes[KEY_LEFTMETA]   = keycode_left_super;
    hadal->keycodes[KEY_RIGHTMETA]  = keycode_right_super;
    hadal->keycodes[KEY_COMPOSE]    = keycode_menu;
    hadal->keycodes[KEY_NUMLOCK]    = keycode_num_lock;
    hadal->keycodes[KEY_CAPSLOCK]   = keycode_caps_lock;
    hadal->keycodes[KEY_PRINT]      = keycode_print_screen;
    hadal->keycodes[KEY_SCROLLLOCK] = keycode_scroll_lock;
    hadal->keycodes[KEY_PAUSE]      = keycode_pause;
    hadal->keycodes[KEY_DELETE]     = keycode_delete;
    hadal->keycodes[KEY_BACKSPACE]  = keycode_backspace;
    hadal->keycodes[KEY_ENTER]      = keycode_enter;
    hadal->keycodes[KEY_HOME]       = keycode_home;
    hadal->keycodes[KEY_END]        = keycode_end;
    hadal->keycodes[KEY_PAGEUP]     = keycode_page_up;
    hadal->keycodes[KEY_PAGEDOWN]   = keycode_page_down;
    hadal->keycodes[KEY_INSERT]     = keycode_insert;
    hadal->keycodes[KEY_LEFT]       = keycode_left;
    hadal->keycodes[KEY_RIGHT]      = keycode_right;
    hadal->keycodes[KEY_DOWN]       = keycode_down;
    hadal->keycodes[KEY_UP]         = keycode_up;
    hadal->keycodes[KEY_F1]         = keycode_f1;
    hadal->keycodes[KEY_F2]         = keycode_f2;
    hadal->keycodes[KEY_F3]         = keycode_f3;
    hadal->keycodes[KEY_F4]         = keycode_f4;
    hadal->keycodes[KEY_F5]         = keycode_f5;
    hadal->keycodes[KEY_F6]         = keycode_f6;
    hadal->keycodes[KEY_F7]         = keycode_f7;
    hadal->keycodes[KEY_F8]         = keycode_f8;
    hadal->keycodes[KEY_F9]         = keycode_f9;
    hadal->keycodes[KEY_F10]        = keycode_f10;
    hadal->keycodes[KEY_F11]        = keycode_f11;
    hadal->keycodes[KEY_F12]        = keycode_f12;
    hadal->keycodes[KEY_F13]        = keycode_f13;
    hadal->keycodes[KEY_F14]        = keycode_f14;
    hadal->keycodes[KEY_F15]        = keycode_f15;
    hadal->keycodes[KEY_F16]        = keycode_f16;
    hadal->keycodes[KEY_F17]        = keycode_f17;
    hadal->keycodes[KEY_F18]        = keycode_f18;
    hadal->keycodes[KEY_F19]        = keycode_f19;
    hadal->keycodes[KEY_F20]        = keycode_f20;
    hadal->keycodes[KEY_F21]        = keycode_f21;
    hadal->keycodes[KEY_F22]        = keycode_f22;
    hadal->keycodes[KEY_F23]        = keycode_f23;
    hadal->keycodes[KEY_F24]        = keycode_f24;
    hadal->keycodes[KEY_KPSLASH]    = keycode_kp_divide;
    hadal->keycodes[KEY_KPASTERISK] = keycode_kp_multiply;
    hadal->keycodes[KEY_KPMINUS]    = keycode_kp_subtract;
    hadal->keycodes[KEY_KPPLUS]     = keycode_kp_add;
    hadal->keycodes[KEY_KP0]        = keycode_kp_0;
    hadal->keycodes[KEY_KP1]        = keycode_kp_1;
    hadal->keycodes[KEY_KP2]        = keycode_kp_2;
    hadal->keycodes[KEY_KP3]        = keycode_kp_3;
    hadal->keycodes[KEY_KP4]        = keycode_kp_4;
    hadal->keycodes[KEY_KP5]        = keycode_kp_5;
    hadal->keycodes[KEY_KP6]        = keycode_kp_6;
    hadal->keycodes[KEY_KP7]        = keycode_kp_7;
    hadal->keycodes[KEY_KP8]        = keycode_kp_8;
    hadal->keycodes[KEY_KP9]        = keycode_kp_9;
    hadal->keycodes[KEY_KPDOT]      = keycode_kp_decimal;
    hadal->keycodes[KEY_KPEQUAL]    = keycode_kp_equal;
    hadal->keycodes[KEY_KPENTER]    = keycode_kp_enter;
    hadal->keycodes[KEY_102ND]      = keycode_world_2;      

    for (s32 scancode = 0; scancode < 256; scancode++) {
        if (hadal->keycodes[scancode] > 0)
            hadal->scancodes[hadal->keycodes[scancode]] = scancode;
    }
}

s32 hadal_wayland_display_init(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (atomic_load_explicit(&hadal->flags, memory_order_relaxed) & hadal_flag_display_is_valid)
        return result_success;

    wl->key_repeat_timerfd = -1;
    wl->cursor_timerfd = -1;

    wl_display_flush = (PFN_wl_display_flush)
        plugin_get_proc_address(wl->api.module_core, "wl_display_flush");
    wl_display_cancel_read = (PFN_wl_display_cancel_read)
        plugin_get_proc_address(wl->api.module_core, "wl_display_cancel_read");
    wl_display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        plugin_get_proc_address(wl->api.module_core, "wl_display_dispatch_pending");
    wl_display_read_events = (PFN_wl_display_read_events)
        plugin_get_proc_address(wl->api.module_core, "wl_display_read_events");
    wl_display_disconnect = (PFN_wl_display_disconnect)
        plugin_get_proc_address(wl->api.module_core, "wl_display_disconnect");
    wl_display_roundtrip = (PFN_wl_display_roundtrip)
        plugin_get_proc_address(wl->api.module_core, "wl_display_roundtrip");
    wl_display_get_fd = (PFN_wl_display_get_fd)
        plugin_get_proc_address(wl->api.module_core, "wl_display_get_fd");
    wl_display_prepare_read = (PFN_wl_display_prepare_read)
        plugin_get_proc_address(wl->api.module_core, "wl_display_prepare_read");
    wl_proxy_marshal = (PFN_wl_proxy_marshal)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_marshal");
    wl_proxy_add_listener = (PFN_wl_proxy_add_listener)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_add_listener");
    wl_proxy_destroy = (PFN_wl_proxy_destroy)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_destroy");
    wl_proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_marshal_constructor");
    wl_proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_marshal_constructor_versioned");
    wl_proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_get_user_data");
    wl_proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_set_user_data");
    wl_proxy_get_tag = (PFN_wl_proxy_get_tag)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_get_tag");
    wl_proxy_set_tag = (PFN_wl_proxy_set_tag)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_set_tag");
    wl_proxy_get_version = (PFN_wl_proxy_get_version)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_get_version");
    wl_proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        plugin_get_proc_address(wl->api.module_core, "wl_proxy_marshal_flags");
    if (!wl->api.wl.display_flush ||
        !wl->api.wl.display_cancel_read ||
        !wl->api.wl.display_dispatch_pending ||
        !wl->api.wl.display_read_events ||
        !wl->api.wl.display_disconnect ||
        !wl->api.wl.display_roundtrip ||
        !wl->api.wl.display_get_fd ||
        !wl->api.wl.display_prepare_read ||
        !wl->api.wl.proxy_marshal ||
        !wl->api.wl.proxy_add_listener ||
        !wl->api.wl.proxy_destroy ||
        !wl->api.wl.proxy_marshal_constructor ||
        !wl->api.wl.proxy_marshal_constructor_versioned ||
        !wl->api.wl.proxy_get_user_data ||
        !wl->api.wl.proxy_set_user_data ||
        !wl->api.wl.proxy_get_tag ||
        !wl->api.wl.proxy_set_tag ||
        !wl->api.wl.proxy_get_version ||
        !wl->api.wl.proxy_marshal_flags) 
    {
        log_error("Failed to load libwayland-client procedures.");
        return result_error;
    }

    wl->api.module_cursor = plugin_load_dll("libwayland-cursor.so.0");
    if (!wl->api.module_cursor) {
        log_error("Missing libwayland-cursor.so.0 library.");
        return result_error;
    }
    wl_cursor_theme_load = (PFN_wl_cursor_theme_load)
        plugin_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_load");
    wl_cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        plugin_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_destroy");
    wl_cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        plugin_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_get_cursor");
    wl_cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        plugin_get_proc_address(wl->api.module_cursor, "wl_cursor_image_get_buffer");
    if (!wl->api.wl.cursor_theme_load ||
        !wl->api.wl.cursor_theme_destroy ||
        !wl->api.wl.cursor_theme_get_cursor ||
        !wl->api.wl.cursor_image_get_buffer)
    {
        log_error("Failed to load libwayland-cursor procedures.");
        return result_error;
    }

    wl->api.module_xkb = plugin_load_dll("libxkbcommon.so.0");
    if (!wl->api.module_xkb) {
        log_error("Missing libxkbcommon.so.0 library.");
    }
    xkb_context_new = (PFN_xkb_context_new)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_context_new");
    xkb_context_unref = (PFN_xkb_context_unref)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_context_unref");
    xkb_keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_keymap_new_from_string");
    xkb_keymap_unref = (PFN_xkb_keymap_unref)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_keymap_unref");
    xkb_keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_keymap_mod_get_index");
    xkb_keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_keymap_key_repeats");
    xkb_keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_keymap_key_get_syms_by_level");
    xkb_state_new = (PFN_xkb_state_new)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_new");
    xkb_state_unref = (PFN_xkb_state_unref)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_unref");
    xkb_state_key_get_syms = (PFN_xkb_state_key_get_syms)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_key_get_syms");
    xkb_state_update_mask = (PFN_xkb_state_update_mask)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_update_mask");
    xkb_state_key_get_layout = (PFN_xkb_state_key_get_layout)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_key_get_layout");
    xkb_state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_state_mod_index_is_active");
    xkb_compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_table_new_from_locale");
    xkb_compose_table_unref = (PFN_xkb_compose_table_unref)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_table_unref");
    xkb_compose_state_new = (PFN_xkb_compose_state_new)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_state_new");
    xkb_compose_state_unref = (PFN_xkb_compose_state_unref)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_state_unref");
    xkb_compose_state_feed = (PFN_xkb_compose_state_feed)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_state_feed");
    xkb_compose_state_get_status = (PFN_xkb_compose_state_get_status)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_state_get_status");
    xkb_compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        plugin_get_proc_address(wl->api.module_xkb, "xkb_compose_state_get_one_sym");

    if (!wl->api.xkb.context_new ||
        !wl->api.xkb.context_unref ||
        !wl->api.xkb.keymap_new_from_string ||
        !wl->api.xkb.keymap_unref ||
        !wl->api.xkb.keymap_mod_get_index ||
        !wl->api.xkb.keymap_key_repeats ||
        !wl->api.xkb.keymap_key_get_syms_by_level ||
        !wl->api.xkb.state_new ||
        !wl->api.xkb.state_unref ||
        !wl->api.xkb.state_key_get_syms ||
        !wl->api.xkb.state_update_mask ||
        !wl->api.xkb.state_key_get_layout ||
        !wl->api.xkb.state_mod_index_is_active ||
        !wl->api.xkb.compose_table_new_from_locale ||
        !wl->api.xkb.compose_table_unref ||
        !wl->api.xkb.compose_state_new ||
        !wl->api.xkb.compose_state_unref ||
        !wl->api.xkb.compose_state_feed ||
        !wl->api.xkb.compose_state_get_status ||
        !wl->api.xkb.compose_state_get_one_sym)
    {
        log_error("Failed to load libxkbcommon procedures.");
        return result_error; 
    }

    wl->registry = wl_display_get_registry(wl->display);
    wl_registry_add_listener(wl->registry, &registry_listener, hadal);

    create_key_tables(hadal);

    wl->xkb_context = xkb_context_new(0);
    if (!wl->xkb_context) {
        log_error("Failed to initialize xkb context.");
        return result_error;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(wl->display);

    /* sync to get initial output events */
    wl_display_roundtrip(wl->display);

    if (!wl->shell) {
        log_error("Wayland compositor is missing xdg-wm-base protocol support.");
        return result_error;
    } else if (!wl->shm) {
        log_error("Wayland compositor is missing wl-shm support.");
        return result_error;
    }

    atomic_fetch_and_explicit(&hadal->flags, hadal_flag_display_is_valid, memory_order_release);
    return result_success;
}

void hadal_wayland_display_fini(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl->api.module_xkb)
        plugin_close_dll(wl->api.module_xkb);
    if (wl->api.module_cursor)
        plugin_close_dll(wl->api.module_cursor);

    if (wl->subcompositor)
        wl_subcompositor_destroy(wl->subcompositor);
    if (wl->compositor)
        wl_compositor_destroy(wl->compositor);
    if (wl->shm)
        wl_shm_destroy(wl->shm);
    if (wl->shell)
        xdg_wm_base_destroy(wl->shell);
    if (wl->seat)
        wl_seat_destroy(wl->seat);
    if (wl->registry)
        wl_registry_destroy(wl->registry);
    if (wl->display) {
        wl_display_flush(wl->display);
        wl_display_disconnect(wl->display);
    }

    if (wl->api.module_core)
        plugin_close_dll(wl->api.module_core);
    g_wl_hadal = NULL;
}

