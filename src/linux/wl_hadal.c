#include "wl_hadal.h"

#include <amw/log.h>
#include <amw/process.h>

struct hadal *g_wl_hadal = NULL;

static void wayland_interface_fini(struct hadal *hadal)
{
    if (!hadal) return;

    if (hadal->api.module_xkb)
        process_close_dll(hadal->api.module_xkb);
    if (hadal->api.module_cursor)
        process_close_dll(hadal->api.module_cursor);

    if (hadal->subcompositor)
        wl_subcompositor_destroy(hadal->subcompositor);
    if (hadal->compositor)
        wl_compositor_destroy(hadal->compositor);
    if (hadal->shm)
        wl_shm_destroy(hadal->shm);
    if (hadal->xdg_shell)
        xdg_wm_base_destroy(hadal->xdg_shell);
    if (hadal->seat)
        wl_seat_destroy(hadal->seat);
    if (hadal->registry)
        wl_registry_destroy(hadal->registry);
    if (hadal->display) {
        wl_display_flush(hadal->display);
        wl_display_disconnect(hadal->display);
    }

    if (hadal->api.module_core)
        process_close_dll(hadal->api.module_core);
    zerop(hadal);
    g_wl_hadal = NULL;
}

static void wayland_acquire_framebuffer_extent(struct hadal *hadal, u32 *width, u32 *height)
{
    *width = atomic_load_explicit(&hadal->fb_width, memory_order_acquire);
    *height = atomic_load_explicit(&hadal->fb_height, memory_order_acquire);
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
        hadal->xdg_shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(hadal->xdg_shell, &wm_base_listener, hadal);
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

static void handle_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *output)
{
    /* unused */
    (void)surface;

    struct hadal *hadal = (struct hadal *)data;

    if (wl_proxy_get_tag((struct wl_proxy *)output) != &hadal->tag)
        return;

    /* handle output scale buffers */
}

static void handle_surface_leave(
    void              *data,
    struct wl_surface *surface,
    struct wl_output  *output)
{
    /* unused */
    (void)surface;

    struct hadal *hadal = (struct hadal *)data;

    if (wl_proxy_get_tag((struct wl_proxy *)output) != &hadal->tag)
        return;

    /* handle output scale buffers */
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
};

RIVEN_ENCORE(hadal, wayland) {
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    const char *fmt = "'hadal_encore_wayland' %s.";

    /* we allow only one wayland backend at a time, so the interface will be shared */
    if (UNLIKELY(g_wl_hadal != NULL)) {
        *create_info->header.interface = (riven_argument_t)g_wl_hadal;
        return;
    }

    /* check all required libraries upfront */
    void *module_client = process_load_dll("libwayland-client.so.0");
    void *module_cursor = process_load_dll("libwayland-cursor.so.0");
    void *module_xkb = process_load_dll("libxkbcommon.so.0");

    if (!module_client || !module_cursor || !module_xkb) {
        if (!module_client)
            log_error(fmt, "missing libwayland-client.so.0 library");
        else process_close_dll(module_client);

        if (!module_cursor)
            log_error(fmt, "missing libwayland-cursor.so.0 library");
        else process_close_dll(module_cursor);
        
        if (!module_xkb)
            log_error(fmt, "missing libxkbcommon.so.0 library");
        else process_close_dll(module_xkb);

        return;
    }
    
    PFN_wl_display_connect display_connect = (PFN_wl_display_connect)process_get_address(module_client, "wl_display_connect");
    if (!display_connect) {
        log_error(fmt, "can't load 'wl_display_connect' from libwayland-client");
        goto leave;
    }

    struct wl_display *display = display_connect(NULL);
    if (!display) { 
        log_error(fmt, "can't connect to a Wayland display. Make sure that a Wayland compositor is currently running");
        goto leave;
    }

    /* okay lets build the interface ;3 */
    struct hadal *hadal = (struct hadal *)
        riven_alloc(create_info->header.riven, create_info->header.tag, sizeof(struct hadal), _Alignof(struct hadal));
    zerop(hadal);

    hadal->api.module_core = module_client;
    hadal->api.module_cursor = module_cursor;
    hadal->api.module_xkb = module_xkb;
    hadal->api.wl.display_connect = display_connect;
    hadal->display = display;
    g_wl_hadal = hadal;

    hadal->key_repeat_timerfd = -1;
    hadal->cursor_timerfd = -1;

    wl_display_flush = (PFN_wl_display_flush)
        process_get_address(hadal->api.module_core, "wl_display_flush");
    wl_display_cancel_read = (PFN_wl_display_cancel_read)
        process_get_address(hadal->api.module_core, "wl_display_cancel_read");
    wl_display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        process_get_address(hadal->api.module_core, "wl_display_dispatch_pending");
    wl_display_read_events = (PFN_wl_display_read_events)
        process_get_address(hadal->api.module_core, "wl_display_read_events");
    wl_display_disconnect = (PFN_wl_display_disconnect)
        process_get_address(hadal->api.module_core, "wl_display_disconnect");
    wl_display_roundtrip = (PFN_wl_display_roundtrip)
        process_get_address(hadal->api.module_core, "wl_display_roundtrip");
    wl_display_get_fd = (PFN_wl_display_get_fd)
        process_get_address(hadal->api.module_core, "wl_display_get_fd");
    wl_display_prepare_read = (PFN_wl_display_prepare_read)
        process_get_address(hadal->api.module_core, "wl_display_prepare_read");
    wl_proxy_marshal = (PFN_wl_proxy_marshal)
        process_get_address(hadal->api.module_core, "wl_proxy_marshal");
    wl_proxy_add_listener = (PFN_wl_proxy_add_listener)
        process_get_address(hadal->api.module_core, "wl_proxy_add_listener");
    wl_proxy_destroy = (PFN_wl_proxy_destroy)
        process_get_address(hadal->api.module_core, "wl_proxy_destroy");
    wl_proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        process_get_address(hadal->api.module_core, "wl_proxy_marshal_constructor");
    wl_proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        process_get_address(hadal->api.module_core, "wl_proxy_marshal_constructor_versioned");
    wl_proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        process_get_address(hadal->api.module_core, "wl_proxy_get_user_data");
    wl_proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        process_get_address(hadal->api.module_core, "wl_proxy_set_user_data");
    wl_proxy_get_tag = (PFN_wl_proxy_get_tag)
        process_get_address(hadal->api.module_core, "wl_proxy_get_tag");
    wl_proxy_set_tag = (PFN_wl_proxy_set_tag)
        process_get_address(hadal->api.module_core, "wl_proxy_set_tag");
    wl_proxy_get_version = (PFN_wl_proxy_get_version)
        process_get_address(hadal->api.module_core, "wl_proxy_get_version");
    wl_proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        process_get_address(hadal->api.module_core, "wl_proxy_marshal_flags");
    if (!hadal->api.wl.display_flush ||
        !hadal->api.wl.display_cancel_read ||
        !hadal->api.wl.display_dispatch_pending ||
        !hadal->api.wl.display_read_events ||
        !hadal->api.wl.display_disconnect ||
        !hadal->api.wl.display_roundtrip ||
        !hadal->api.wl.display_get_fd ||
        !hadal->api.wl.display_prepare_read ||
        !hadal->api.wl.proxy_marshal ||
        !hadal->api.wl.proxy_add_listener ||
        !hadal->api.wl.proxy_destroy ||
        !hadal->api.wl.proxy_marshal_constructor ||
        !hadal->api.wl.proxy_marshal_constructor_versioned ||
        !hadal->api.wl.proxy_get_user_data ||
        !hadal->api.wl.proxy_set_user_data ||
        !hadal->api.wl.proxy_get_tag ||
        !hadal->api.wl.proxy_set_tag ||
        !hadal->api.wl.proxy_get_version ||
        !hadal->api.wl.proxy_marshal_flags) 
    {
        log_error(fmt, "failed to load libwayland-client procedures");
        goto leave;
    }

    wl_cursor_theme_load = (PFN_wl_cursor_theme_load)
        process_get_address(hadal->api.module_cursor, "wl_cursor_theme_load");
    wl_cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        process_get_address(hadal->api.module_cursor, "wl_cursor_theme_destroy");
    wl_cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        process_get_address(hadal->api.module_cursor, "wl_cursor_theme_get_cursor");
    wl_cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        process_get_address(hadal->api.module_cursor, "wl_cursor_image_get_buffer");
    if (!hadal->api.wl.cursor_theme_load ||
        !hadal->api.wl.cursor_theme_destroy ||
        !hadal->api.wl.cursor_theme_get_cursor ||
        !hadal->api.wl.cursor_image_get_buffer)
    {
        log_error(fmt, "failed to load libwayland-cursor procedures");
        goto leave;
    }

    xkb_context_new = (PFN_xkb_context_new)
        process_get_address(hadal->api.module_xkb, "xkb_context_new");
    xkb_context_unref = (PFN_xkb_context_unref)
        process_get_address(hadal->api.module_xkb, "xkb_context_unref");
    xkb_keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        process_get_address(hadal->api.module_xkb, "xkb_keymap_new_from_string");
    xkb_keymap_unref = (PFN_xkb_keymap_unref)
        process_get_address(hadal->api.module_xkb, "xkb_keymap_unref");
    xkb_keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        process_get_address(hadal->api.module_xkb, "xkb_keymap_mod_get_index");
    xkb_keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        process_get_address(hadal->api.module_xkb, "xkb_keymap_key_repeats");
    xkb_keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        process_get_address(hadal->api.module_xkb, "xkb_keymap_key_get_syms_by_level");
    xkb_state_new = (PFN_xkb_state_new)
        process_get_address(hadal->api.module_xkb, "xkb_state_new");
    xkb_state_unref = (PFN_xkb_state_unref)
        process_get_address(hadal->api.module_xkb, "xkb_state_unref");
    xkb_state_key_get_syms = (PFN_xkb_state_key_get_syms)
        process_get_address(hadal->api.module_xkb, "xkb_state_key_get_syms");
    xkb_state_update_mask = (PFN_xkb_state_update_mask)
        process_get_address(hadal->api.module_xkb, "xkb_state_update_mask");
    xkb_state_key_get_layout = (PFN_xkb_state_key_get_layout)
        process_get_address(hadal->api.module_xkb, "xkb_state_key_get_layout");
    xkb_state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        process_get_address(hadal->api.module_xkb, "xkb_state_mod_index_is_active");
    xkb_compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        process_get_address(hadal->api.module_xkb, "xkb_compose_table_new_from_locale");
    xkb_compose_table_unref = (PFN_xkb_compose_table_unref)
        process_get_address(hadal->api.module_xkb, "xkb_compose_table_unref");
    xkb_compose_state_new = (PFN_xkb_compose_state_new)
        process_get_address(hadal->api.module_xkb, "xkb_compose_state_new");
    xkb_compose_state_unref = (PFN_xkb_compose_state_unref)
        process_get_address(hadal->api.module_xkb, "xkb_compose_state_unref");
    xkb_compose_state_feed = (PFN_xkb_compose_state_feed)
        process_get_address(hadal->api.module_xkb, "xkb_compose_state_feed");
    xkb_compose_state_get_status = (PFN_xkb_compose_state_get_status)
        process_get_address(hadal->api.module_xkb, "xkb_compose_state_get_status");
    xkb_compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        process_get_address(hadal->api.module_xkb, "xkb_compose_state_get_one_sym");

    if (!hadal->api.xkb.context_new ||
        !hadal->api.xkb.context_unref ||
        !hadal->api.xkb.keymap_new_from_string ||
        !hadal->api.xkb.keymap_unref ||
        !hadal->api.xkb.keymap_mod_get_index ||
        !hadal->api.xkb.keymap_key_repeats ||
        !hadal->api.xkb.keymap_key_get_syms_by_level ||
        !hadal->api.xkb.state_new ||
        !hadal->api.xkb.state_unref ||
        !hadal->api.xkb.state_key_get_syms ||
        !hadal->api.xkb.state_update_mask ||
        !hadal->api.xkb.state_key_get_layout ||
        !hadal->api.xkb.state_mod_index_is_active ||
        !hadal->api.xkb.compose_table_new_from_locale ||
        !hadal->api.xkb.compose_table_unref ||
        !hadal->api.xkb.compose_state_new ||
        !hadal->api.xkb.compose_state_unref ||
        !hadal->api.xkb.compose_state_feed ||
        !hadal->api.xkb.compose_state_get_status ||
        !hadal->api.xkb.compose_state_get_one_sym)
    {
        log_error(fmt, "failed to load libxkbcommon procedures");
        goto leave;
    }

    hadal->registry = wl_display_get_registry(hadal->display);
    wl_registry_add_listener(hadal->registry, &registry_listener, hadal);

    hadal->xkb_context = xkb_context_new(0);
    if (!hadal->xkb_context) {
        log_error(fmt, "failed to initialize XKB context");
        goto leave;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(hadal->display);

    /* sync to get initial output events */
    wl_display_roundtrip(hadal->display);

    if (!hadal->xdg_shell) {
        log_error(fmt, "the compositor is missing xdg-wm-base protocol support");
        goto leave;
    } else if (!hadal->shm) {
        log_error(fmt, "the compositor is missing wl-shm support");
        goto leave;
    }

    /* creating the surface */
    hadal->surface = wl_compositor_create_surface(hadal->compositor);
    if (!hadal->surface) {
        log_error(fmt, "failed to create a surface via the Wayland compositor");
        goto leave;
    }
    wl_proxy_set_tag((struct wl_proxy *)hadal->surface, &hadal->tag);
    wl_surface_add_listener(hadal->surface, &surface_listener, hadal);
    atomic_store_explicit(&hadal->fb_width, create_info->width, memory_order_release);
    atomic_store_explicit(&hadal->fb_height, create_info->height, memory_order_release);
    
    hadal->interface = (struct hadal_interface){
        .header = {
            .name = str_init("hadal_wayland"), 
            .riven = create_info->header.riven,
            .tag = create_info->header.tag,
            .fini = (PFN_riven_work)wayland_interface_fini,
        },
        .acquire_framebuffer_extent = wayland_acquire_framebuffer_extent,
    };
    *create_info->header.interface = (riven_argument_t)hadal;
    log_verbose("Hadal '%s' interface write.", hadal->interface.header.name.ptr);
    return;

leave:
    process_close_dll(module_client);
    process_close_dll(module_cursor);
    process_close_dll(module_xkb);
}

#include <wayland-protocol-code.h>
#include <xdg-shell-protocol-code.h>
