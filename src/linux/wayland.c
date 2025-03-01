#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <amw/bedrock.h>
#include <amw/hadal.h>

/* hadal interface */
static s32 hadal_wayland_display_init(struct hadopelagic *hadal);
static void hadal_wayland_display_fini(struct hadopelagic *hadal);
static s32 hadal_wayland_create_vulkan_surface(
    struct hadopelagic *hadal, 
    struct harridan    *harridan, 
    struct swapchain   *swapchain);

#include <wayland-client-core.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

struct wl_shm;
struct wl_output;
struct wl_display;
struct wl_proxy;
struct wl_interface;

typedef s32                             (*PFN_wl_display_flush)(struct wl_display *display);
typedef void                            (*PFN_wl_display_cancel_read)(struct wl_display *display);
typedef s32                             (*PFN_wl_display_dispatch_pending)(struct wl_display *display);
typedef s32                             (*PFN_wl_display_read_events)(struct wl_display *display);
typedef struct wl_display *             (*PFN_wl_display_connect)(const char *);
typedef void                            (*PFN_wl_display_disconnect)(struct wl_display *);
typedef s32                             (*PFN_wl_display_roundtrip)(struct wl_display *);
typedef s32                             (*PFN_wl_display_get_fd)(struct wl_display *);
typedef s32                             (*PFN_wl_display_prepare_read)(struct wl_display *);
typedef void                            (*PFN_wl_proxy_marshal)(struct wl_proxy *, uint32_t, ...);
typedef s32                             (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void(**)(void), void *);
typedef void                            (*PFN_wl_proxy_destroy)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor)(struct wl_proxy *, u32, const struct wl_interface *, ...);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *, u32, const struct wl_interface *, u32, ...);
typedef void *                          (*PFN_wl_proxy_get_user_data)(struct wl_proxy *);
typedef void                            (*PFN_wl_proxy_set_user_data)(struct wl_proxy *, void *);
typedef void                            (*PFN_wl_proxy_set_tag)(struct wl_proxy *, const char * const *);
typedef const char * const *            (*PFN_wl_proxy_get_tag)(struct wl_proxy *);
typedef u32                             (*PFN_wl_proxy_get_version)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_flags)(struct wl_proxy *, u32, const struct wl_interface *, u32, u32, ...);
#define wl_display_flush                        g_wl_hadal->api.wl.display_flush
#define wl_display_cancel_read                  g_wl_hadal->api.wl.display_cancel_read
#define wl_display_dispatch_pending             g_wl_hadal->api.wl.display_dispatch_pending
#define wl_display_read_events                  g_wl_hadal->api.wl.display_read_events
#define wl_display_connect                      g_wl_hadal->api.wl.display_connect
#define wl_display_disconnect                   g_wl_hadal->api.wl.display_disconnect
#define wl_display_roundtrip                    g_wl_hadal->api.wl.display_roundtrip
#define wl_display_get_fd                       g_wl_hadal->api.wl.display_get_fd
#define wl_display_prepare_read                 g_wl_hadal->api.wl.display_prepare_read
#define wl_proxy_marshal                        g_wl_hadal->api.wl.proxy_marshal
#define wl_proxy_add_listener                   g_wl_hadal->api.wl.proxy_add_listener
#define wl_proxy_destroy                        g_wl_hadal->api.wl.proxy_destroy
#define wl_proxy_marshal_constructor            g_wl_hadal->api.wl.proxy_marshal_constructor
#define wl_proxy_marshal_constructor_versioned  g_wl_hadal->api.wl.proxy_marshal_constructor_versioned
#define wl_proxy_get_user_data                  g_wl_hadal->api.wl.proxy_get_user_data
#define wl_proxy_set_user_data                  g_wl_hadal->api.wl.proxy_set_user_data
#define wl_proxy_set_tag                        g_wl_hadal->api.wl.proxy_set_tag
#define wl_proxy_get_tag                        g_wl_hadal->api.wl.proxy_get_tag
#define wl_proxy_get_version                    g_wl_hadal->api.wl.proxy_get_version
#define wl_proxy_marshal_flags                  g_wl_hadal->api.wl.proxy_marshal_flags

struct wl_cursor {
    u32                      image_count;
    struct wl_cursor_image **images;
    char                    *name;
};

typedef struct wl_cursor_theme *        (*PFN_wl_cursor_theme_load)(const char *, s32, struct wl_shm *);
typedef void                            (*PFN_wl_cursor_theme_destroy)(struct wl_cursor_theme *);
typedef struct wl_cursor *              (*PFN_wl_cursor_theme_get_cursor)(struct wl_cursor_theme *, const char *);
typedef struct wl_buffer *              (*PFN_wl_cursor_image_get_buffer)(struct wl_cursor_image *);
#define wl_cursor_theme_load                    g_wl_hadal->api.wl.cursor_theme_load
#define wl_cursor_theme_destroy                 g_wl_hadal->api.wl.cursor_theme_destroy
#define wl_cursor_theme_get_cursor              g_wl_hadal->api.wl.cursor_theme_get_cursor
#define wl_cursor_image_get_buffer              g_wl_hadal->api.wl.cursor_image_get_buffer

typedef struct xkb_context *            (*PFN_xkb_context_new)(enum xkb_context_flags);
typedef void                            (*PFN_xkb_context_unref)(struct xkb_context *);
typedef struct xkb_keymap *             (*PFN_xkb_keymap_new_from_string)(struct xkb_context *, const char *, enum xkb_keymap_format, enum xkb_keymap_compile_flags);
typedef void                            (*PFN_xkb_keymap_unref)(struct xkb_keymap *);
typedef xkb_mod_index_t                 (*PFN_xkb_keymap_mod_get_index)(struct xkb_keymap *, const char *);
typedef s32                             (*PFN_xkb_keymap_key_repeats)(struct xkb_keymap *, xkb_keycode_t);
typedef s32                             (*PFN_xkb_keymap_key_get_syms_by_level)(struct xkb_keymap *, xkb_keycode_t, xkb_layout_index_t, xkb_level_index_t, const xkb_keysym_t **);
typedef struct xkb_state *              (*PFN_xkb_state_new)(struct xkb_keymap *);
typedef void                            (*PFN_xkb_state_unref)(struct xkb_state *);
typedef s32                             (*PFN_xkb_state_key_get_syms)(struct xkb_state *, xkb_keycode_t, const xkb_keysym_t **);
typedef enum xkb_state_component        (*PFN_xkb_state_update_mask)(struct xkb_state *, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
typedef xkb_layout_index_t              (*PFN_xkb_state_key_get_layout)(struct xkb_state *, xkb_keycode_t);
typedef s32                             (*PFN_xkb_state_mod_index_is_active)(struct xkb_state *, xkb_mod_index_t, enum xkb_state_component);
typedef struct xkb_compose_table *      (*PFN_xkb_compose_table_new_from_locale)(struct xkb_context *, const char *, enum xkb_compose_compile_flags);
typedef void                            (*PFN_xkb_compose_table_unref)(struct xkb_compose_table *);
typedef struct xkb_compose_state *      (*PFN_xkb_compose_state_new)(struct xkb_compose_table *, enum xkb_compose_state_flags);
typedef void                            (*PFN_xkb_compose_state_unref)(struct xkb_compose_state *);
typedef enum xkb_compose_feed_result    (*PFN_xkb_compose_state_feed)(struct xkb_compose_state *, xkb_keysym_t);
typedef enum xkb_compose_status         (*PFN_xkb_compose_state_get_status)(struct xkb_compose_state *);
typedef xkb_keysym_t                    (*PFN_xkb_compose_state_get_one_sym)(struct xkb_compose_state *);
#define xkb_context_new                         g_wl_hadal->api.xkb.context_new
#define xkb_context_unref                       g_wl_hadal->api.xkb.context_unref
#define xkb_keymap_new_from_string              g_wl_hadal->api.xkb.keymap_new_from_string
#define xkb_keymap_unref                        g_wl_hadal->api.xkb.keymap_unref
#define xkb_keymap_mod_get_index                g_wl_hadal->api.xkb.keymap_mod_get_index
#define xkb_keymap_key_repeats                  g_wl_hadal->api.xkb.keymap_key_repeats
#define xkb_keymap_key_get_syms_by_level        g_wl_hadal->api.xkb.keymap_key_get_syms_by_level
#define xkb_state_new                           g_wl_hadal->api.xkb.state_new
#define xkb_state_unref                         g_wl_hadal->api.xkb.state_unref
#define xkb_state_key_get_syms                  g_wl_hadal->api.xkb.state_key_get_syms
#define xkb_state_update_mask                   g_wl_hadal->api.xkb.state_update_mask
#define xkb_state_key_get_layout                g_wl_hadal->api.xkb.state_key_get_layout
#define xkb_state_mod_index_is_active           g_wl_hadal->api.xkb.state_mod_index_is_active
#define xkb_compose_table_new_from_locale       g_wl_hadal->api.xkb.compose_table_new_from_locale
#define xkb_compose_table_unref                 g_wl_hadal->api.xkb.compose_table_unref
#define xkb_compose_state_new                   g_wl_hadal->api.xkb.compose_state_new
#define xkb_compose_state_unref                 g_wl_hadal->api.xkb.compose_state_unref
#define xkb_compose_state_feed                  g_wl_hadal->api.xkb.compose_state_feed
#define xkb_compose_state_get_status            g_wl_hadal->api.xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym           g_wl_hadal->api.xkb.compose_state_get_one_sym

struct wayland_display {
    const char                     *tag;

    struct wl_display              *display;
    struct wl_registry             *registry;
    struct wl_compositor           *compositor;
    struct wl_subcompositor        *subcompositor;
    struct wl_shm                  *shm;
    struct wl_seat                 *seat;
    struct wl_surface              *surface;

    struct xdg_wm_base             *shell;
    struct xdg_surface             *app_surface;
    struct xdg_toplevel            *toplevel;

    s32                             cursor_timerfd;

    s32                             key_repeat_timerfd;
    s32                             key_repeat_rate;
    s32                             key_repeat_delay;
    s32                             key_repeat_scancode;

    struct xkb_context             *xkb_context;
    struct xkb_keymap              *xkb_keymap;
    struct xkb_state               *xkb_state;
    struct xkb_compose_state       *xkb_compose_state;

    xkb_mod_index_t                 xkb_control_idx;
    xkb_mod_index_t                 xkb_alt_idx;
    xkb_mod_index_t                 xkb_shift_idx;
    xkb_mod_index_t                 xkb_super_idx;
    xkb_mod_index_t                 xkb_caps_lock_idx;
    xkb_mod_index_t                 xkb_num_lock_idx;
    u32                             xkb_modifiers;

    struct {
        void *module_core;
        void *module_cursor;
        struct {
            PFN_wl_display_flush                        display_flush;
            PFN_wl_display_cancel_read                  display_cancel_read;
            PFN_wl_display_dispatch_pending             display_dispatch_pending;
            PFN_wl_display_read_events                  display_read_events;
            PFN_wl_display_connect                      display_connect;
            PFN_wl_display_disconnect                   display_disconnect;
            PFN_wl_display_roundtrip                    display_roundtrip;
            PFN_wl_display_get_fd                       display_get_fd;
            PFN_wl_display_prepare_read                 display_prepare_read;
            PFN_wl_proxy_marshal                        proxy_marshal;
            PFN_wl_proxy_add_listener                   proxy_add_listener;
            PFN_wl_proxy_destroy                        proxy_destroy;
            PFN_wl_proxy_marshal_constructor            proxy_marshal_constructor;
            PFN_wl_proxy_marshal_constructor_versioned  proxy_marshal_constructor_versioned;
            PFN_wl_proxy_get_user_data                  proxy_get_user_data;
            PFN_wl_proxy_set_user_data                  proxy_set_user_data;
            PFN_wl_proxy_set_tag                        proxy_set_tag;
            PFN_wl_proxy_get_tag                        proxy_get_tag;
            PFN_wl_proxy_get_version                    proxy_get_version;
            PFN_wl_proxy_marshal_flags                  proxy_marshal_flags;
            PFN_wl_cursor_theme_load                    cursor_theme_load;
            PFN_wl_cursor_theme_destroy                 cursor_theme_destroy;
            PFN_wl_cursor_theme_get_cursor              cursor_theme_get_cursor;
            PFN_wl_cursor_image_get_buffer              cursor_image_get_buffer;
        } wl;

        void *module_xkb;
        struct {
            PFN_xkb_context_new                         context_new;
            PFN_xkb_context_unref                       context_unref;
            PFN_xkb_keymap_new_from_string              keymap_new_from_string;
            PFN_xkb_keymap_unref                        keymap_unref;
            PFN_xkb_keymap_mod_get_index                keymap_mod_get_index;
            PFN_xkb_keymap_key_repeats                  keymap_key_repeats;
            PFN_xkb_keymap_key_get_syms_by_level        keymap_key_get_syms_by_level;
            PFN_xkb_state_new                           state_new;
            PFN_xkb_state_unref                         state_unref;
            PFN_xkb_state_key_get_syms                  state_key_get_syms;
            PFN_xkb_state_update_mask                   state_update_mask;
            PFN_xkb_state_key_get_layout                state_key_get_layout;
            PFN_xkb_state_mod_index_is_active           state_mod_index_is_active;
            PFN_xkb_compose_table_new_from_locale       compose_table_new_from_locale;
            PFN_xkb_compose_table_unref                 compose_table_unref;
            PFN_xkb_compose_state_new                   compose_state_new;
            PFN_xkb_compose_state_unref                 compose_state_unref;
            PFN_xkb_compose_state_feed                  compose_state_feed;
            PFN_xkb_compose_state_get_status            compose_state_get_status;
            PFN_xkb_compose_state_get_one_sym           compose_state_get_one_sym;
        } xkb;
    } api;
};

/** A global pointer to the allocated display backend. We need this for redefining 
 *  symbols of core wayland functions for them to be available to generated protocol 
 *  sources, so that runtime loading for the wayland backend works correctly. It will 
 *  also help with keeping the backend clean, so that there are no multiple wayland 
 *  displays at any time. We only support one window at a time anyway. */
extern struct wayland_display *g_wl_hadal;

/* Protocols are generated with wayland-scanner, their sources are included in
 * the project repository: resources/wayland/<protocol>.xml.
 *
 * We need the WAYLAND global and some macro magic to redirect the procedures declared by 
 * wayland-client-core to be available in the generated protocols, when dynamically 
 * loading the pointers to the procedures at runtime. The global pointer also helps with 
 * keeping the backend clean, if for some reasons multiple wayland displays were to be 
 * created via hadopelagic, for example when testing. */
#include <wayland-protocol.h>
#include <xdg-shell-protocol.h>

#include <amw/harridan.h>
#include <amw/plugin.h>
#include <amw/log.h>

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
        .name = string_init("wayland"),
        .display_init = hadal_wayland_display_init,
        .display_fini = hadal_wayland_display_fini,
        .create_vulkan_surface = hadal_wayland_create_vulkan_surface,
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

static void handle_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *output)
{
    /* unused */
    (void)surface;

    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl_proxy_get_tag((struct wl_proxy *)output) != &wl->tag)
        return;

    /* handle output scale buffers */
    /* TODO */
}

static void handle_surface_leave(
    void              *data,
    struct wl_surface *surface,
    struct wl_output  *output)
{
    /* unused */
    (void)surface;

    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl_proxy_get_tag((struct wl_proxy *)output) != &wl->tag)
        return;

    /* handle output scale buffers */
    /* TODO */
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
    .preferred_buffer_scale = NULL,
    .preferred_buffer_transform = NULL,
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

static s32 hadal_wayland_display_init(struct hadopelagic *hadal)
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

    /* creating the surface */
    wl->surface = wl_compositor_create_surface(wl->compositor);
    if (!wl->surface) {
        log_error("Failed to create a surface via the Wayland compositor.");
        return result_error;
    }
    wl_proxy_set_tag((struct wl_proxy *)wl->surface, &wl->tag);
    wl_surface_add_listener(wl->surface, &surface_listener, hadal);
    atomic_store_explicit(&hadal->fb_width, hadal->window_width, memory_order_release);
    atomic_store_explicit(&hadal->fb_height, hadal->window_height, memory_order_release);

    atomic_fetch_and_explicit(&hadal->flags, hadal_flag_display_is_valid, memory_order_release);
    return result_success;
}

static void hadal_wayland_display_fini(struct hadopelagic *hadal)
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

static s32 hadal_wayland_create_vulkan_surface(
    struct hadopelagic *hadal, 
    struct harridan    *harridan, 
    struct swapchain   *swapchain)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    assert_debug(harridan && harridan->instance && harridan->api.vkCreateWaylandSurfaceKHR);

    VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = wl->display,
        .surface = wl->surface
    };
    VERIFY_VK(harridan->api.vkCreateWaylandSurfaceKHR(harridan->instance, &surface_info, NULL, &swapchain->surface));
    return swapchain->surface ? result_success : result_error;
}
