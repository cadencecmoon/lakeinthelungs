#include <amw/log.h>
#include <amw/process.h>

#include "wl_hadal.h"

struct hadal *g_wayland = NULL;

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <wayland-util.h>

#include "wayland-protocol-code.h"
#include "alpha-modifier-v1-protocol-code.h"
#include "color-management-v1-protocol-code.h"
#include "cursor-shape-v1-protocol-code.h"
#include "fractional-scale-v1-protocol-code.h"
#include "frog-color-management-v1-protocol-code.h"
#include "idle-inhibit-unstable-v1-protocol-code.h"
#include "input-timestamps-unstable-v1-protocol-code.h"
#include "keyboard-shortcuts-inhibit-unstable-v1-protocol-code.h"
#include "pointer-constraints-unstable-v1-protocol-code.h"
#include "pointer-gestures-unstable-v1-protocol-code.h"
#include "primary-selection-unstable-v1-protocol-code.h"
#include "relative-pointer-unstable-v1-protocol-code.h"
#include "tablet-unstable-v2-protocol-code.h"
#include "text-input-unstable-v3-protocol-code.h"
#include "viewporter-protocol-code.h"
#include "xdg-activation-v1-protocol-code.h"
#include "xdg-decoration-unstable-v1-protocol-code.h"
#include "xdg-dialog-v1-protocol-code.h"
#include "xdg-foreign-unstable-v2-protocol-code.h"
#include "xdg-output-unstable-v1-protocol-code.h"
#include "xdg-shell-protocol-code.h"
#include "xdg-toplevel-icon-v1-protocol-code.h"

static const char *g_wayland_surface_tag = "hadal_window";
static const char *g_wayland_output_tag = "hadal_output";

void wayland_register_surface(struct wl_surface *surface)
{
    wl_proxy_set_tag((struct wl_proxy *)surface, &g_wayland_surface_tag);
}

void wayland_register_output(struct wl_output *output) 
{
    wl_proxy_set_tag((struct wl_proxy *)output, &g_wayland_output_tag);
}

b32 wayland_own_surface(struct wl_surface *surface)
{
    return wl_proxy_get_tag((struct wl_proxy *)surface) == &g_wayland_surface_tag;
}

b32 wayland_own_output(struct wl_output *output)
{
    return wl_proxy_get_tag((struct wl_proxy *)output) == &g_wayland_output_tag;
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
    struct hadal *hadal = (struct hadal *)data;

    if (!strcmp(interface, "wl_compositor")) {
        hadal->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(3, version));

    } else if (!strcmp(interface, "wl_subcompositor")) {
        hadal->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);

    } else if (!strcmp(interface, "wl_shm")) {
        hadal->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);

    } else if (!strcmp(interface, "wl_output")) {
        log_debug("Add wayland output: %u ver. %u", name, version);

    } else if (!strcmp(interface, "wl_seat")) {
        if (!hadal->seat) {
            hadal->seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
            /* TODO listener */

            if (wl_seat_get_version(hadal->seat) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION) {
                hadal->key_repeat_timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
            }
        }
    } else if (!strcmp(interface, "xdg_wm_base")) {
        hadal->shell.xdg = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(hadal->shell.xdg, &wm_base_listener, hadal);
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
    /* TODO delete outputs */
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

static b32 load_symbols(struct hadal *hadal, const char *fn)
{
    /* should be already loaded */
    assert_debug(hadal->wl.display_connect);

    hadal->wl.proxy_marshal = (PFN_wl_proxy_marshal)
        process_get_address(hadal->module_client, "wl_proxy_marshal");
    hadal->wl.proxy_create = (PFN_wl_proxy_create)
        process_get_address(hadal->module_client, "wl_proxy_create");
    hadal->wl.proxy_destroy = (PFN_wl_proxy_destroy)
        process_get_address(hadal->module_client, "wl_proxy_destroy");
    hadal->wl.proxy_add_listener = (PFN_wl_proxy_add_listener)
        process_get_address(hadal->module_client, "wl_proxy_add_listener");
    hadal->wl.proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        process_get_address(hadal->module_client, "wl_proxy_set_user_data");
    hadal->wl.proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        process_get_address(hadal->module_client, "wl_proxy_get_user_data");
    hadal->wl.proxy_get_version = (PFN_wl_proxy_get_version)
        process_get_address(hadal->module_client, "wl_proxy_get_version");
    hadal->wl.proxy_get_id = (PFN_wl_proxy_get_id)
        process_get_address(hadal->module_client, "wl_proxy_get_id");
    hadal->wl.proxy_get_class = (PFN_wl_proxy_get_class)
        process_get_address(hadal->module_client, "wl_proxy_get_class");
    hadal->wl.proxy_set_queue = (PFN_wl_proxy_set_queue)
        process_get_address(hadal->module_client, "wl_proxy_set_queue");
    hadal->wl.proxy_create_wrapper = (PFN_wl_proxy_create_wrapper)
        process_get_address(hadal->module_client, "wl_proxy_create_wrapper");
    hadal->wl.proxy_wrapper_destroy = (PFN_wl_proxy_wrapper_destroy)
        process_get_address(hadal->module_client, "wl_proxy_wrapper_destroy");
    hadal->wl.proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        process_get_address(hadal->module_client, "wl_proxy_marshal_constructor");
    hadal->wl.proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        process_get_address(hadal->module_client, "wl_proxy_marshal_constructor_versioned");
    hadal->wl.proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        process_get_address(hadal->module_client, "wl_proxy_marshal_flags");
    hadal->wl.proxy_marshal_array_flags = (PFN_wl_proxy_marshal_array_flags)
        process_get_address(hadal->module_client, "wl_proxy_marshal_array_flags");
    hadal->wl.proxy_set_tag = (PFN_wl_proxy_set_tag)
        process_get_address(hadal->module_client, "wl_proxy_set_tag");
    hadal->wl.proxy_get_tag = (PFN_wl_proxy_get_tag)
        process_get_address(hadal->module_client, "wl_proxy_get_tag");
    hadal->wl.display_connect_to_fd = (PFN_wl_display_connect_to_fd)
        process_get_address(hadal->module_client, "wl_display_connect_to_fd");
    hadal->wl.display_disconnect = (PFN_wl_display_disconnect)
        process_get_address(hadal->module_client, "wl_display_disconnect");
    hadal->wl.display_get_fd = (PFN_wl_display_get_fd)
        process_get_address(hadal->module_client, "wl_display_get_fd");
    hadal->wl.display_dispatch = (PFN_wl_display_dispatch)
        process_get_address(hadal->module_client, "wl_display_dispatch");
    hadal->wl.display_dispatch_queue = (PFN_wl_display_dispatch_queue)
        process_get_address(hadal->module_client, "wl_display_dispatch_queue");
    hadal->wl.display_dispatch_queue_pending = (PFN_wl_display_dispatch_queue_pending)
        process_get_address(hadal->module_client, "wl_display_dispatch_queue_pending");
    hadal->wl.display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        process_get_address(hadal->module_client, "wl_display_dispatch_pending");
    hadal->wl.display_prepare_read = (PFN_wl_display_prepare_read)
        process_get_address(hadal->module_client, "wl_display_prepare_read");
    hadal->wl.display_prepare_read_queue = (PFN_wl_display_prepare_read_queue)
        process_get_address(hadal->module_client, "wl_display_prepare_read_queue");
    hadal->wl.display_read_events = (PFN_wl_display_read_events)
        process_get_address(hadal->module_client, "wl_display_read_events");
    hadal->wl.display_cancel_read = (PFN_wl_display_cancel_read)
        process_get_address(hadal->module_client, "wl_display_cancel_read");
    hadal->wl.display_get_error = (PFN_wl_display_get_error)
        process_get_address(hadal->module_client, "wl_display_get_error");
    hadal->wl.display_flush = (PFN_wl_display_flush)
        process_get_address(hadal->module_client, "wl_display_flush");
    hadal->wl.display_roundtrip = (PFN_wl_display_roundtrip)
        process_get_address(hadal->module_client, "wl_display_roundtrip");
    hadal->wl.display_create_queue = (PFN_wl_display_create_queue)
        process_get_address(hadal->module_client, "wl_display_create_queue");
    hadal->wl.event_queue_destroy = (PFN_wl_event_queue_destroy)
        process_get_address(hadal->module_client, "wl_event_queue_destroy");
    hadal->wl.log_set_handler_client = (PFN_wl_log_set_handler_client)
        process_get_address(hadal->module_client, "wl_log_set_handler_client");
    hadal->wl.list_init = (PFN_wl_list_init)
        process_get_address(hadal->module_client, "wl_list_init");
    hadal->wl.list_insert = (PFN_wl_list_insert)
        process_get_address(hadal->module_client, "wl_list_insert");
    hadal->wl.list_remove = (PFN_wl_list_remove)
        process_get_address(hadal->module_client, "wl_list_remove");
    hadal->wl.list_length = (PFN_wl_list_length)
        process_get_address(hadal->module_client, "wl_list_length");
    hadal->wl.list_empty = (PFN_wl_list_empty)
        process_get_address(hadal->module_client, "wl_list_empty");
    hadal->wl.list_insert_list = (PFN_wl_list_insert_list)
        process_get_address(hadal->module_client, "wl_list_insert_list");

    if (!hadal->wl.proxy_marshal ||
        !hadal->wl.proxy_create ||
        !hadal->wl.proxy_destroy ||
        !hadal->wl.proxy_add_listener ||
        !hadal->wl.proxy_set_user_data ||
        !hadal->wl.proxy_get_user_data ||
        !hadal->wl.proxy_get_version ||
        !hadal->wl.proxy_get_id ||
        !hadal->wl.proxy_get_class ||
        !hadal->wl.proxy_set_queue ||
        !hadal->wl.proxy_create_wrapper ||
        !hadal->wl.proxy_wrapper_destroy ||
        !hadal->wl.proxy_marshal_constructor ||
        !hadal->wl.proxy_marshal_constructor_versioned ||
        !hadal->wl.proxy_marshal_flags ||
        !hadal->wl.proxy_marshal_array_flags ||
        !hadal->wl.proxy_set_tag ||
        !hadal->wl.proxy_get_tag ||
        !hadal->wl.display_connect_to_fd ||
        !hadal->wl.display_disconnect ||
        !hadal->wl.display_get_fd ||
        !hadal->wl.display_dispatch ||
        !hadal->wl.display_dispatch_queue ||
        !hadal->wl.display_dispatch_queue_pending ||
        !hadal->wl.display_dispatch_pending ||
        !hadal->wl.display_prepare_read ||
        !hadal->wl.display_prepare_read_queue ||
        !hadal->wl.display_read_events ||
        !hadal->wl.display_cancel_read ||
        !hadal->wl.display_get_error ||
        !hadal->wl.display_flush ||
        !hadal->wl.display_roundtrip ||
        !hadal->wl.display_create_queue ||
        !hadal->wl.event_queue_destroy ||
        !hadal->wl.log_set_handler_client ||
        !hadal->wl.list_init ||
        !hadal->wl.list_insert ||
        !hadal->wl.list_remove ||
        !hadal->wl.list_length ||
        !hadal->wl.list_empty ||
        !hadal->wl.list_insert_list)
    {
        log_error("%s can't load libwayland-client procedures, can't continue.", fn);
        return false;
    }

    hadal->wl.cursor_theme_load = (PFN_wl_cursor_theme_load)
        process_get_address(hadal->module_cursor, "wl_cursor_theme_load");
    hadal->wl.cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        process_get_address(hadal->module_cursor, "wl_cursor_theme_destroy");
    hadal->wl.cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        process_get_address(hadal->module_cursor, "wl_cursor_theme_get_cursor");
    hadal->wl.cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        process_get_address(hadal->module_cursor, "wl_cursor_image_get_buffer");
    hadal->wl.cursor_frame = (PFN_wl_cursor_frame)
        process_get_address(hadal->module_cursor, "wl_cursor_frame");

    if (!hadal->wl.cursor_theme_load ||
        !hadal->wl.cursor_theme_destroy ||
        !hadal->wl.cursor_theme_get_cursor ||
        !hadal->wl.cursor_image_get_buffer ||
        !hadal->wl.cursor_frame)
    {
        log_error("%s can't load libwayland-cursor procedures, can't continue.", fn);
        return false;
    }

    hadal->xkb.context_new = (PFN_xkb_context_new)
        process_get_address(hadal->module_xkbcommon, "xkb_context_new");
    hadal->xkb.context_unref = (PFN_xkb_context_unref)
        process_get_address(hadal->module_xkbcommon, "xkb_context_unref");
    hadal->xkb.keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_new_from_string");
    hadal->xkb.keymap_unref = (PFN_xkb_keymap_unref)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_unref");
    hadal->xkb.keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_mod_get_index");
    hadal->xkb.keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_key_repeats");
    hadal->xkb.keymap_key_for_each = (PFN_xkb_keymap_key_for_each)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_key_for_each");
    hadal->xkb.keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_key_get_syms_by_level");
    hadal->xkb.keymap_layout_get_name = (PFN_xkb_keymap_layout_get_name)
        process_get_address(hadal->module_xkbcommon, "xkb_keymap_layout_get_name");
    hadal->xkb.keysym_to_utf8 = (PFN_xkb_keysym_to_utf8)
        process_get_address(hadal->module_xkbcommon, "xkb_keysym_to_utf8");
    hadal->xkb.keysym_to_utf32 = (PFN_xkb_keysym_to_utf32)
        process_get_address(hadal->module_xkbcommon, "xkb_keysym_to_utf32");
    hadal->xkb.state_new = (PFN_xkb_state_new)
        process_get_address(hadal->module_xkbcommon, "xkb_state_new");
    hadal->xkb.state_unref = (PFN_xkb_state_unref)
        process_get_address(hadal->module_xkbcommon, "xkb_state_unref");
    hadal->xkb.state_key_get_syms = (PFN_xkb_state_key_get_syms)
        process_get_address(hadal->module_xkbcommon, "xkb_state_key_get_syms");
    hadal->xkb.state_key_get_layout = (PFN_xkb_state_key_get_layout)
        process_get_address(hadal->module_xkbcommon, "xkb_state_key_get_layout");
    hadal->xkb.state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        process_get_address(hadal->module_xkbcommon, "xkb_state_mod_index_is_active");
    hadal->xkb.state_update_mask = (PFN_xkb_state_update_mask)
        process_get_address(hadal->module_xkbcommon, "xkb_state_update_mask");
    hadal->xkb.compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_table_new_from_locale");
    hadal->xkb.compose_table_unref = (PFN_xkb_compose_table_unref)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_table_unref");
    hadal->xkb.compose_state_new = (PFN_xkb_compose_state_new)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_state_new");
    hadal->xkb.compose_state_reset = (PFN_xkb_compose_state_reset)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_state_reset");
    hadal->xkb.compose_state_feed = (PFN_xkb_compose_state_feed)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_state_feed");
    hadal->xkb.compose_state_get_status = (PFN_xkb_compose_state_get_status)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_state_get_status");
    hadal->xkb.compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        process_get_address(hadal->module_xkbcommon, "xkb_compose_state_get_one_sym");

    if (!hadal->xkb.context_new ||
        !hadal->xkb.context_unref ||
        !hadal->xkb.keymap_new_from_string ||
        !hadal->xkb.keymap_unref ||
        !hadal->xkb.keymap_mod_get_index ||
        !hadal->xkb.keymap_key_repeats ||
        !hadal->xkb.keymap_key_for_each ||
        !hadal->xkb.keymap_key_get_syms_by_level ||
        !hadal->xkb.keymap_layout_get_name ||
        !hadal->xkb.keysym_to_utf8 ||
        !hadal->xkb.keysym_to_utf32 ||
        !hadal->xkb.state_new ||
        !hadal->xkb.state_unref ||
        !hadal->xkb.state_key_get_syms ||
        !hadal->xkb.state_key_get_layout ||
        !hadal->xkb.state_mod_index_is_active ||
        !hadal->xkb.state_update_mask ||
        !hadal->xkb.compose_table_new_from_locale ||
        !hadal->xkb.compose_table_unref ||
        !hadal->xkb.compose_state_new ||
        !hadal->xkb.compose_state_reset ||
        !hadal->xkb.compose_state_feed ||
        !hadal->xkb.compose_state_get_status ||
        !hadal->xkb.compose_state_get_one_sym)
    {
        log_error("%s can't load libxkbcommon procedures, can't continue.", fn);
        return false;
    }

    /* optional */
    if (!hadal->module_libdecor) return true;

    hadal->libdecor.new = (PFN_libdecor_new)
        process_get_address(hadal->module_libdecor, "libdecor_new");
    hadal->libdecor.unref = (PFN_libdecor_unref)
        process_get_address(hadal->module_libdecor, "libdecor_unref");
    hadal->libdecor.get_fd = (PFN_libdecor_get_fd)
        process_get_address(hadal->module_libdecor, "libdecor_get_fd");
    hadal->libdecor.decorate = (PFN_libdecor_decorate)
        process_get_address(hadal->module_libdecor, "libdecor_decorate");
    hadal->libdecor.frame_unref = (PFN_libdecor_frame_unref)
        process_get_address(hadal->module_libdecor, "libdecor_frame_unref");
    hadal->libdecor.frame_set_title = (PFN_libdecor_frame_set_title)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_title");
    hadal->libdecor.frame_set_app_id = (PFN_libdecor_frame_set_app_id)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_app_id");
    hadal->libdecor.frame_set_max_content_size = (PFN_libdecor_frame_set_max_content_size)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_max_content_size");
    hadal->libdecor.frame_get_max_content_size = (PFN_libdecor_frame_get_max_content_size)
        process_get_address(hadal->module_libdecor, "libdecor_frame_get_max_content_size");
    hadal->libdecor.frame_set_min_content_size = (PFN_libdecor_frame_set_min_content_size)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_min_content_size");
    hadal->libdecor.frame_get_min_content_size = (PFN_libdecor_frame_get_min_content_size)
        process_get_address(hadal->module_libdecor, "libdecor_frame_get_min_content_size");
    hadal->libdecor.frame_resize = (PFN_libdecor_frame_resize)
        process_get_address(hadal->module_libdecor, "libdecor_frame_resize");
    hadal->libdecor.frame_move = (PFN_libdecor_frame_move)
        process_get_address(hadal->module_libdecor, "libdecor_frame_move");
    hadal->libdecor.frame_commit = (PFN_libdecor_frame_commit)
        process_get_address(hadal->module_libdecor, "libdecor_frame_commit");
    hadal->libdecor.frame_set_minimized = (PFN_libdecor_frame_set_minimized)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_minimized");
    hadal->libdecor.frame_set_maximized = (PFN_libdecor_frame_set_maximized)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_maximized");
    hadal->libdecor.frame_unset_maximized = (PFN_libdecor_frame_unset_maximized)
        process_get_address(hadal->module_libdecor, "libdecor_frame_unset_maximized");
    hadal->libdecor.frame_set_fullscreen = (PFN_libdecor_frame_set_fullscreen)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_fullscreen");
    hadal->libdecor.frame_unset_fullscreen = (PFN_libdecor_frame_unset_fullscreen)
        process_get_address(hadal->module_libdecor, "libdecor_frame_unset_fullscreen");
    hadal->libdecor.frame_set_capabilities = (PFN_libdecor_frame_set_capabilities)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_capabilities");
    hadal->libdecor.frame_unset_capabilities = (PFN_libdecor_frame_unset_capabilities)
        process_get_address(hadal->module_libdecor, "libdecor_frame_unset_capabilities");
    hadal->libdecor.frame_has_capability = (PFN_libdecor_frame_has_capability)
        process_get_address(hadal->module_libdecor, "libdecor_frame_has_capability");
    hadal->libdecor.frame_set_visibility = (PFN_libdecor_frame_set_visibility)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_visibility");
    hadal->libdecor.frame_is_visible = (PFN_libdecor_frame_is_visible)
        process_get_address(hadal->module_libdecor, "libdecor_frame_is_visible");
    hadal->libdecor.frame_is_floating = (PFN_libdecor_frame_is_floating)
        process_get_address(hadal->module_libdecor, "libdecor_frame_is_floating");
    hadal->libdecor.frame_set_parent = (PFN_libdecor_frame_set_parent)
        process_get_address(hadal->module_libdecor, "libdecor_frame_set_parent");
    hadal->libdecor.frame_get_xdg_surface = (PFN_libdecor_frame_get_xdg_surface)
        process_get_address(hadal->module_libdecor, "libdecor_frame_get_xdg_surface");
    hadal->libdecor.frame_get_xdg_toplevel = (PFN_libdecor_frame_get_xdg_toplevel)
        process_get_address(hadal->module_libdecor, "libdecor_frame_get_xdg_toplevel");
    hadal->libdecor.frame_map = (PFN_libdecor_frame_map)
        process_get_address(hadal->module_libdecor, "libdecor_frame_map");
    hadal->libdecor.state_new = (PFN_libdecor_state_new)
        process_get_address(hadal->module_libdecor, "libdecor_state_new");
    hadal->libdecor.state_free = (PFN_libdecor_state_free)
        process_get_address(hadal->module_libdecor, "libdecor_state_free");
    hadal->libdecor.configuration_get_content_size = (PFN_libdecor_configuration_get_content_size)
        process_get_address(hadal->module_libdecor, "libdecor_configuration_get_content_size");
    hadal->libdecor.configuration_get_window_state = (PFN_libdecor_configuration_get_window_state)
        process_get_address(hadal->module_libdecor, "libdecor_configuration_get_window_state");
    hadal->libdecor.dispatch = (PFN_libdecor_dispatch)
        process_get_address(hadal->module_libdecor, "libdecor_dispatch");

    if (!hadal->libdecor.new ||
        !hadal->libdecor.unref ||
        !hadal->libdecor.get_fd ||
        !hadal->libdecor.decorate ||
        !hadal->libdecor.frame_unref ||
        !hadal->libdecor.frame_set_title ||
        !hadal->libdecor.frame_set_app_id ||
        !hadal->libdecor.frame_set_max_content_size ||
        !hadal->libdecor.frame_get_max_content_size ||
        !hadal->libdecor.frame_set_min_content_size ||
        !hadal->libdecor.frame_get_min_content_size ||
        !hadal->libdecor.frame_resize ||
        !hadal->libdecor.frame_move ||
        !hadal->libdecor.frame_commit ||
        !hadal->libdecor.frame_set_minimized ||
        !hadal->libdecor.frame_set_maximized ||
        !hadal->libdecor.frame_unset_maximized ||
        !hadal->libdecor.frame_set_fullscreen ||
        !hadal->libdecor.frame_unset_fullscreen ||
        !hadal->libdecor.frame_set_capabilities ||
        !hadal->libdecor.frame_unset_capabilities ||
        !hadal->libdecor.frame_has_capability ||
        !hadal->libdecor.frame_set_visibility ||
        !hadal->libdecor.frame_is_visible ||
        !hadal->libdecor.frame_is_floating ||
        !hadal->libdecor.frame_set_parent ||
        !hadal->libdecor.frame_get_xdg_surface ||
        !hadal->libdecor.frame_get_xdg_toplevel ||
        !hadal->libdecor.frame_map ||
        !hadal->libdecor.state_new ||
        !hadal->libdecor.state_free ||
        !hadal->libdecor.configuration_get_content_size ||
        !hadal->libdecor.configuration_get_window_state ||
        !hadal->libdecor.dispatch) 
    {
        /* disable libdecor */
        log_error("%s can't load libdecor procedures, this functionality will be disabled.", fn);
        process_close_dll(hadal->module_libdecor);
        hadal->module_libdecor = NULL;
    }
    return true;
}

static void wayland_interface_fini(struct hadal *hadal)
{
    if (!hadal) return;

    process_close_dll(hadal->module_libdecor);
    process_close_dll(hadal->module_xkbcommon);
    process_close_dll(hadal->module_cursor);

    if (hadal->subcompositor)
        wl_subcompositor_destroy(hadal->subcompositor);
    if (hadal->compositor)
        wl_compositor_destroy(hadal->compositor);
    if (hadal->shm)
        wl_shm_destroy(hadal->shm);
    if (hadal->shell.xdg)
        xdg_wm_base_destroy(hadal->shell.xdg);
    if (hadal->seat)
        wl_seat_destroy(hadal->seat);
    if (hadal->registry)
        wl_registry_destroy(hadal->registry);
    if (hadal->display) {
        hadal->wl.display_flush(hadal->display);
        hadal->wl.display_disconnect(hadal->display);
    }
    process_close_dll(hadal->module_client);

    zerop(hadal);
    g_wayland = NULL;
}

RIVEN_ENCORE(hadal, wayland)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    const char *fn = "'hadal_wayland'";

    /* we allow only one wayland backend at a time, so the interface will be shared */
    if (UNLIKELY(g_wayland != NULL)) {
        *encore->header.interface = (riven_argument_t)g_wayland;
        return;
    }

    void *module_client = process_load_dll("libwayland-client.so.0");
    void *module_cursor = process_load_dll("libwayland-cursor.so.0");
    void *module_xkbcommon = process_load_dll("libxkbcommon.so.0");

#ifndef HADAL_WAYLAND_DEBUG_DISABLE_LIBDECOR
    /* optional modules */
    void *module_libdecor = process_load_dll("libdecor-0.so.0");
    if (!module_libdecor)
        log_info("%s optional libdecor.so.0 is missing, window decorations will be limited.", fn);
#endif

    if (!module_client || !module_cursor || !module_xkbcommon) {
        if (!module_xkbcommon)
            log_error("%s libxkbcommon.so.0 is missing.", fn);
        if (!module_cursor)
            log_error("%s libwayland-cursor.so.0 is missing.", fn);
        if (!module_client)
            log_error("%s libwayland-client.so.0 is missing.", fn);
disconnect:
        log_error("%s can't continue.", fn);
        process_close_dll(module_libdecor);
        process_close_dll(module_xkbcommon);
        process_close_dll(module_cursor);
        process_close_dll(module_client);
        g_wayland = NULL;
        return;
    }

    PFN_wl_display_connect display_connect = (PFN_wl_display_connect)process_get_address(module_client, "wl_display_connect");
    if (!display_connect) {
        log_error("%s can't load 'wl_display_connect' from libwayland-client.so.0", fn);
        goto disconnect;
    }

    struct wl_display *display = display_connect(NULL);
    if (!display) { 
        log_error("%s can't connect to a Wayland display. Make sure that a Wayland compositor is currently running", fn);
        goto disconnect;
    }

    struct hadal *hadal = (struct hadal *)riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct hadal), _Alignof(struct hadal));
    zerop(hadal);

    hadal->module_client = module_client;
    hadal->module_cursor = module_cursor;
    hadal->module_xkbcommon = module_xkbcommon;
    hadal->module_libdecor = module_libdecor;
    hadal->wl.display_connect = display_connect;
    hadal->display = display;
    g_wayland = hadal;

    if (!load_symbols(hadal, fn)) {
        wayland_interface_fini(hadal);
        return;
    }

    hadal->key_repeat_timerfd = -1;
    hadal->cursor_timerfd = -1;

    hadal->registry = wl_display_get_registry(hadal->display);
    wl_registry_add_listener(hadal->registry, &registry_listener, hadal);

    hadal->xkb_context = xkb_context_new(0);
    if (!hadal->xkb_context) {
        log_error("%s failed to initialize XKB context", fn);
        wayland_interface_fini(hadal);
        return;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(hadal->display);

    /* sync to get initial output events */
    wl_display_roundtrip(hadal->display);

    /* write the interface */
    hadal->interface.header.name = str_init("hadal_wayland");
    hadal->interface.header.riven = encore->header.riven;
    hadal->interface.header.tag = encore->header.tag;
    hadal->interface.header.fini = (PFN_riven_work)wayland_interface_fini;

    hadal->interface.window_create = _hadal_wayland_window_create;
    hadal->interface.window_destroy = _hadal_wayland_window_destroy;
    hadal->interface.window_attach_swapchain = _hadal_wayland_window_attach_swapchain;
    hadal->interface.window_acquire_framebuffer_extent = _hadal_wayland_window_acquire_framebuffer_extent;
#ifdef REZNOR_VULKAN
    hadal->interface.vulkan_write_instance_procedures = _hadal_wayland_vulkan_write_instance_procedures;
    hadal->interface.vulkan_surface_create = _hadal_wayland_vulkan_surface_create;
    hadal->interface.vulkan_presentation_support = _hadal_wayland_vulkan_presentation_support;
#endif

    *encore->header.interface = (riven_argument_t)hadal;
    log_verbose("'%s' interface write.", hadal->interface.header.name.ptr);
}
