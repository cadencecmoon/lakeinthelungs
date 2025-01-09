#ifdef AMW_NATIVE_WAYLAND

#include <lake/bedrock/assert.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/os.h>

#include <lake/hadopelagic.h>
#include <lake/ipomoeaalba.h>
#include <lake/riven.h>

#include <string.h>

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
typedef int32_t                         (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void(**)(void), void *);
typedef void                            (*PFN_wl_proxy_destroy)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor)(struct wl_proxy *, u32, const struct wl_interface *, ...);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *, u32, const struct wl_interface *, u32, ...);
typedef void *                          (*PFN_wl_proxy_get_user_data)(struct wl_proxy *);
typedef void                            (*PFN_wl_proxy_set_user_data)(struct wl_proxy *, void *);
typedef void                            (*PFN_wl_proxy_set_tag)(struct wl_proxy *, const char * const *);
typedef const char * const *            (*PFN_wl_proxy_get_tag)(struct wl_proxy *);
typedef u32                             (*PFN_wl_proxy_get_version)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_flags)(struct wl_proxy *, u32, const struct wl_interface *, u32, u32, ...);
#define wl_display_flush                        WAYLAND->api.wl.display_flush
#define wl_display_cancel_read                  WAYLAND->api.wl.display_cancel_read
#define wl_display_dispatch_pending             WAYLAND->api.wl.display_dispatch_pending
#define wl_display_read_events                  WAYLAND->api.wl.display_read_events
#define wl_display_connect                      WAYLAND->api.wl.display_connect
#define wl_display_disconnect                   WAYLAND->api.wl.display_disconnect
#define wl_display_roundtrip                    WAYLAND->api.wl.display_roundtrip
#define wl_display_get_fd                       WAYLAND->api.wl.display_get_fd
#define wl_display_prepare_read                 WAYLAND->api.wl.display_prepare_read
#define wl_proxy_marshal                        WAYLAND->api.wl.proxy_marshal
#define wl_proxy_add_listener                   WAYLAND->api.wl.proxy_add_listener
#define wl_proxy_destroy                        WAYLAND->api.wl.proxy_destroy
#define wl_proxy_marshal_constructor            WAYLAND->api.wl.proxy_marshal_constructor
#define wl_proxy_marshal_constructor_versioned  WAYLAND->api.wl.proxy_marshal_constructor_versioned
#define wl_proxy_get_user_data                  WAYLAND->api.wl.proxy_get_user_data
#define wl_proxy_set_user_data                  WAYLAND->api.wl.proxy_set_user_data
#define wl_proxy_set_tag                        WAYLAND->api.wl.proxy_set_tag
#define wl_proxy_get_tag                        WAYLAND->api.wl.proxy_get_tag
#define wl_proxy_get_version                    WAYLAND->api.wl.proxy_get_version
#define wl_proxy_marshal_flags                  WAYLAND->api.wl.proxy_marshal_flags

struct wl_cursor_image {
    u32 width;
    u32 height;
    u32 hotspot_x;
    u32 hotspot_y;
    u32 delay;
};

struct wl_cursor {
    u32                      image_count;
    struct wl_cursor_image **images;
    char                    *name;
};

typedef struct wl_cursor_theme *        (*PFN_wl_cursor_theme_load)(const char *, s32, struct wl_shm *);
typedef void                            (*PFN_wl_cursor_theme_destroy)(struct wl_cursor_theme *);
typedef struct wl_cursor *              (*PFN_wl_cursor_theme_get_cursor)(struct wl_cursor_theme *, const char *);
typedef struct wl_buffer *              (*PFN_wl_cursor_image_get_buffer)(struct wl_cursor_image *);
#define wl_cursor_theme_load                    WAYLAND->api.wl.cursor_theme_load
#define wl_cursor_theme_destroy                 WAYLAND->api.wl.cursor_theme_destroy
#define wl_cursor_theme_get_cursor              WAYLAND->api.wl.cursor_theme_get_cursor
#define wl_cursor_image_get_buffer              WAYLAND->api.wl.cursor_image_get_buffer

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
#define xkb_context_new                         WAYLAND->api.xkb.context_new
#define xkb_context_unref                       WAYLAND->api.xkb.context_unref
#define xkb_keymap_new_from_string              WAYLAND->api.xkb.keymap_new_from_string
#define xkb_keymap_unref                        WAYLAND->api.xkb.keymap_unref
#define xkb_keymap_mod_get_index                WAYLAND->api.xkb.keymap_mod_get_index
#define xkb_keymap_key_repeats                  WAYLAND->api.xkb.keymap_key_repeats
#define xkb_keymap_key_get_syms_by_level        WAYLAND->api.xkb.keymap_key_get_syms_by_level
#define xkb_state_new                           WAYLAND->api.xkb.state_new
#define xkb_state_unref                         WAYLAND->api.xkb.state_unref
#define xkb_state_key_get_syms                  WAYLAND->api.xkb.state_key_get_syms
#define xkb_state_update_mask                   WAYLAND->api.xkb.state_update_mask
#define xkb_state_key_get_layout                WAYLAND->api.xkb.state_key_get_layout
#define xkb_state_mod_index_is_active           WAYLAND->api.xkb.state_mod_index_is_active
#define xkb_compose_table_new_from_locale       WAYLAND->api.xkb.compose_table_new_from_locale
#define xkb_compose_table_unref                 WAYLAND->api.xkb.compose_table_unref
#define xkb_compose_state_new                   WAYLAND->api.xkb.compose_state_new
#define xkb_compose_state_unref                 WAYLAND->api.xkb.compose_state_unref
#define xkb_compose_state_feed                  WAYLAND->api.xkb.compose_state_feed
#define xkb_compose_state_get_status            WAYLAND->api.xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym           WAYLAND->api.xkb.compose_state_get_one_sym

struct wayland_output {
    struct wl_output       *wl_output;
    u32                     name;
    s32                     current_mode;
    s32                     xpos, ypos, scale;
    u32                     width_mm, height_mm;

    /* A window is binded to an output monitor if in fullscreen mode. */
    struct wayland_window  *window;
};

struct wayland_window {
    char                   *app_id;
    char                   *title;

    at_u32                  width, height;
    at_u32                  fb_width, fb_height;

    u32                     minwidth, minheight;
    u32                     maxwidth, maxheight;
    u32                     numer, denom;

    /* A window is binded to an output monitor if in fullscreen mode. */
    struct wayland_output  *output;

    struct wl_surface       *surface;
    struct {
        struct xdg_surface  *surface;
        struct xdg_toplevel *toplevel;
    } xdg;

    struct {
        u32 width, height, flags;
    } pending;
};

struct wayland_display {
    const char *tag;

    struct wl_display       *display;
    struct wl_registry      *registry;
    struct wl_shm           *shm;
    struct wl_seat          *seat;
    struct wl_compositor    *compositor;
    struct wl_subcompositor *subcompositor;
    struct wp_viewporter    *viewporter;
    struct xdg_wm_base      *shell;

    /* only support one window at a time */
    struct wayland_window   window;

    struct {
        void *module_core;
        void *module_cursor;
        void *module_xkb;

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

    u32 serial;
};

/* global pointer to the allocated display backend, for convenience */
static struct wayland_display *WAYLAND;

/* Protocols are generated with wayland-scanner, their sources are included in
 * the project repository: resources/wayland/<protocol>.xml.
 *
 * We need the WAYLAND global and some macro magic to redirect the procedures declared by 
 * wayland-client-core to be available in the generated protocols, when dynamically 
 * loading the pointers to the procedures at runtime. The global pointer also helps with 
 * keeping the backend clean, if for some reasons multiple wayland displays were to be 
 * created via hadopelagic, for example when testing. */
#include <wayland-protocol.h>
#include <fractional-scale-v1-protocol.h>
#include <idle-inhibit-unstable-v1-protocol.h>
#include <pointer-constraints-unstable-v1-protocol.h>
#include <pointer-gestures-unstable-v1-protocol.h>
#include <relative-pointer-unstable-v1-protocol.h>
#include <tablet-v2-protocol.h>
#include <viewporter-protocol.h>
#include <xdg-activation-v1-protocol.h>
#include <xdg-decoration-unstable-v1-protocol.h>
#include <xdg-shell-protocol.h>

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
    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (!strcmp(interface, "wl_compositor")) {
        wl->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(3, version));
    } else if (!strcmp(interface, "wl_subcompositor")) {
        wl->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    } else if (!strcmp(interface, "wl_shm")) {
        wl->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, "wl_output")) {
        /* TODO add output */
        log_warn("TODO add output: %d, %d", name, version);
    } else if (!strcmp(interface, "wl_seat")) {
        if (!wl->seat) {
            wl->seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
            /* TODO add seat listener */
            /* TODO key repeat timerfd */
        }
    } else if (!strcmp(interface, "xdg_wm_base")) {
        wl->shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(wl->shell, &wm_base_listener, hadal);
    } else if (!strcmp(interface, "wp_viewporter")) {
        wl->viewporter = wl_registry_bind(registry, name, &wp_viewporter_interface, 1);
    }
}

static void handle_registry_global_remove(
    void               *data,
    struct wl_registry *registry,
    u32                 name)
{
    /* unused */
    (void)registry;

    struct hadopelagic *hadal = (struct hadopelagic *)data;

    for (u32 i = 0; i < hadal->output_count; i++) {
        struct wayland_output *output = (struct wayland_output *)hadal->outputs[i];
        if (output->name == name) {
            /* TODO disconnect output */
            return;
        }
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

static void resize_framebuffer(struct wayland_display *wl, u32 width, u32 height)
{
    /* TODO add fractional scale */
    at_store_explicit(&wl->window.fb_width, width * 1.f, memory_model_seq_cst);   // window->wl.buffer_scale
    at_store_explicit(&wl->window.fb_height, height * 1.f, memory_model_seq_cst); // window->wl.buffer_scale
}

static b32 resize_window(struct wayland_display *wl, u32 width, u32 height)
{
    if (width == at_read_relaxed(&wl->window.width) && height == at_read_relaxed(&wl->window.height))
        return false;

    resize_framebuffer(wl, width, height);
    at_store_explicit(&wl->window.width, width, memory_model_seq_cst);
    at_store_explicit(&wl->window.height, height, memory_model_seq_cst);

    /* TODO scaling viewport */

    return true;
}

static void handle_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *wl_output)
{
    /* unused */
    (void)surface;

    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &wl->tag)
        return;

    struct wayland_output *output = wl_output_get_user_data(wl_output);

    if (!output) return;

    /* TODO handle output scale buffers */
}

static void handle_surface_leave(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *wl_output)
{
    /* unused */
    (void)surface;

    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &wl->tag)
        return;

    /* TODO handle output scale buffers */
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
    .preferred_buffer_scale = NULL,
    .preferred_buffer_transform = NULL,
};

static void handle_xdg_toplevel_configure(
    void                *data,
    struct xdg_toplevel *toplevel,
    s32                  width, 
    s32                  height,
    struct wl_array     *states)
{
    /* unused */
    (void)toplevel;

    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    u32 *state;

    /* unset those flags from pending */
    wl->window.pending.flags &= ~(
            hadal_flag_activated |
            hadal_flag_maximized |
            hadal_flag_fullscreen
    );

    wl_array_for_each(state, states) {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            wl->window.pending.flags |= hadal_flag_maximized;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            wl->window.pending.flags |= hadal_flag_fullscreen;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            wl->window.pending.flags |= hadal_flag_is_resizing;
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            wl->window.pending.flags |= hadal_flag_activated;
            break;
        }
    }

    if (width && height) {
        wl->window.pending.width = width;
        wl->window.pending.height = height;
    } else {
        wl->window.pending.width = at_read_relaxed(&wl->window.width);
        wl->window.pending.height = at_read_relaxed(&wl->window.height);
    }
}

static void handle_xdg_toplevel_close(
    void                *data, 
    struct xdg_toplevel *toplevel)
{
    /* unused */
    (void)toplevel;

    struct hadopelagic *hadal = (struct hadopelagic *)data;

    at_fetch_or_explicit(&hadal->flags, hadal_flag_should_close, memory_model_seq_cst);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = handle_xdg_toplevel_configure,
    .close = handle_xdg_toplevel_close,
};

static void handle_xdg_surface_configure(
    void               *data, 
    struct xdg_surface *surface, 
    u32                 serial)
{
    struct hadopelagic *hadal = (struct hadopelagic *)data;
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    xdg_surface_ack_configure(surface, serial);
    if ((at_read_relaxed(&hadal->flags) & hadal_flag_activated) != (wl->window.pending.flags & hadal_flag_activated)) {
        at_fetch_xor_explicit(&hadal->flags, hadal_flag_activated, memory_model_relaxed);
        if (!(at_read_relaxed(&hadal->flags) & hadal_flag_activated)) {
            if (wl->window.output && (at_read_relaxed(&hadal->flags) & hadal_flag_auto_minimize)) {
                xdg_toplevel_set_minimized(wl->window.xdg.toplevel);
                // TODO window minimize event
            }
        }
    }
    if ((at_read_relaxed(&hadal->flags) & hadal_flag_maximized) != (wl->window.pending.flags & hadal_flag_maximized)) {
        at_fetch_xor_explicit(&hadal->flags, hadal_flag_maximized, memory_model_relaxed);
        /* TODO window maximized event */
    }
    if (wl->window.pending.flags & hadal_flag_fullscreen) {
        at_fetch_or_relaxed(&hadal->flags, hadal_flag_fullscreen);
    } else {
        at_fetch_and_relaxed(&hadal->flags, ~hadal_flag_fullscreen);
    }
            
    u32 width = wl->window.pending.width;
    u32 height = wl->window.pending.height;

    if (!(at_read_relaxed(&hadal->flags) & (hadal_flag_maximized | hadal_flag_fullscreen))) {
        if (wl->window.numer != 0 && wl->window.denom != 0) {
            const f32 aspect_ratio = (f32)width / (f32)height;
            const f32 target_ratio = (f32)wl->window.numer / (f32)wl->window.denom;
            if (aspect_ratio < target_ratio) {
                height = width / target_ratio;
            } else if (aspect_ratio > target_ratio) {
                width = height * target_ratio;
            }
        }
    }

    if (resize_window(wl, width, height)) {
        /* TODO window resized event */
        if (at_read_relaxed(&hadal->flags) & hadal_flag_visible) {
            /* TODO window content damaged */
        }
    }
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void update_size_limits(struct hadopelagic *hadal)
{
    uint32_t minwidth, minheight, maxwidth, maxheight;

    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (at_read_relaxed(&hadal->flags) & hadal_flag_resizable) {
        if (wl->window.minwidth == 0 || wl->window.minheight == 0) {
            minwidth = minheight = 0;
        } else {
            minwidth = wl->window.minwidth;
            minheight = wl->window.minheight;
        }
        if (wl->window.maxwidth == 0 || wl->window.maxheight == 0) {
            maxwidth = maxheight = 0;
        } else {
            maxwidth = wl->window.maxwidth;
            maxheight = wl->window.maxheight;
        }
    } else {
        minwidth = maxwidth = at_read_relaxed(&wl->window.width);
        minheight = maxheight = at_read_relaxed(&wl->window.height);
    }
    xdg_toplevel_set_min_size(wl->window.xdg.toplevel, minwidth, minheight);
    xdg_toplevel_set_max_size(wl->window.xdg.toplevel, maxwidth, maxheight);
}

static b32 create_xdg_shell_objects(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    wl->window.xdg.surface = xdg_wm_base_get_xdg_surface(wl->shell, wl->window.surface);
    if (!wl->window.xdg.surface) {
        log_error("Failed to create Wayland xdg-surface.");
        return false;
    }
    xdg_surface_add_listener(wl->window.xdg.surface, &xdg_surface_listener, hadal);

    wl->window.xdg.toplevel = xdg_surface_get_toplevel(wl->window.xdg.surface);
    if (!wl->window.xdg.toplevel) {
        log_error("Failed to create Wayland xdg-toplevel.");
        return false;
    }
    xdg_toplevel_add_listener(wl->window.xdg.toplevel, &xdg_toplevel_listener, hadal);

    if (wl->window.app_id)
        xdg_toplevel_set_app_id(wl->window.xdg.toplevel, wl->window.app_id);
    xdg_toplevel_set_title(wl->window.xdg.toplevel, wl->window.title);

    if (wl->window.output) {
        xdg_toplevel_set_fullscreen(wl->window.xdg.toplevel, wl->window.output->wl_output);
        // TODO set idle inhibitor, true
    } else {
        if (at_read_relaxed(&hadal->flags) & hadal_flag_maximized)
            xdg_toplevel_set_maximized(wl->window.xdg.toplevel);
        // TODO set idle inhibitor, false
    }
    update_size_limits(hadal);
    wl_surface_commit(wl->window.surface);
    wl_display_roundtrip(wl->display);
    return true;
}

static void destroy_xdg_shell_objects(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (wl->window.xdg.toplevel)
        xdg_toplevel_destroy(wl->window.xdg.toplevel);
    if (wl->window.xdg.surface)
        xdg_surface_destroy(wl->window.xdg.surface);
    wl->window.xdg.toplevel = NULL;
    wl->window.xdg.surface = NULL;
}

static b32 create_surface(struct hadopelagic *hadal, u32 width, u32 height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    wl->window.surface = wl_compositor_create_surface(wl->compositor);
    if (!wl->window.surface) {
        log_error("Failed to create a Wayland window surface.");
        return false;
    }
    wl_proxy_set_tag((struct wl_proxy *)wl->window.surface, &wl->tag);
    wl_surface_add_listener(wl->window.surface, &surface_listener, hadal);

    at_store_relaxed(&wl->window.width, width);
    at_store_relaxed(&wl->window.height, height);
    at_store_relaxed(&wl->window.fb_width, width);
    at_store_relaxed(&wl->window.fb_height, height);

    /* TODO fractional scale */

    return true;
}

AMWAPI void hadal_wayland_get_window_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (out_width)  *out_width  = at_read_explicit(&wl->window.width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&wl->window.height, memory_model_acquire);
}

AMWAPI void hadal_wayland_get_framebuffer_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (out_width)  *out_width  = at_read_explicit(&wl->window.fb_width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&wl->window.fb_height, memory_model_acquire);
}

AMWAPI void hadal_wayland_show_window(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (!wl->window.xdg.toplevel)
        create_xdg_shell_objects(hadal);
}

AMWAPI void hadal_wayland_hide_window(struct hadopelagic *hadal)
{
    const struct wayland_display *wl = (const struct wayland_display *)hadal->display;
    destroy_xdg_shell_objects(hadal);
    wl_surface_attach(wl->window.surface, NULL, 0, 0);
    wl_surface_commit(wl->window.surface);
}

AMWAPI s32 hadal_wayland_display_init(struct hadopelagic *hadal, struct ipomoeaalba *ia, u32 width, u32 height, const char *title)
{
    (void)ia; // TODO

    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (at_read_relaxed(&hadal->flags) & hadal_flag_initialized) 
        return result_success;
    assert_debug(wl == WAYLAND);

    wl->tag = "TODO";

    wl_display_flush = (PFN_wl_display_flush)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_flush");
    wl_display_cancel_read = (PFN_wl_display_cancel_read)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_cancel_read");
    wl_display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_dispatch_pending");
    wl_display_read_events = (PFN_wl_display_read_events)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_read_events");
    wl_display_disconnect = (PFN_wl_display_disconnect)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_disconnect");
    wl_display_roundtrip = (PFN_wl_display_roundtrip)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_roundtrip");
    wl_display_get_fd = (PFN_wl_display_get_fd)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_get_fd");
    wl_display_prepare_read = (PFN_wl_display_prepare_read)
        bedrock_get_proc_address(wl->api.module_core, "wl_display_prepare_read");
    wl_proxy_marshal = (PFN_wl_proxy_marshal)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_marshal");
    wl_proxy_add_listener = (PFN_wl_proxy_add_listener)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_add_listener");
    wl_proxy_destroy = (PFN_wl_proxy_destroy)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_destroy");
    wl_proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_marshal_constructor");
    wl_proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_marshal_constructor_versioned");
    wl_proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_get_user_data");
    wl_proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_set_user_data");
    wl_proxy_get_tag = (PFN_wl_proxy_get_tag)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_get_tag");
    wl_proxy_set_tag = (PFN_wl_proxy_set_tag)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_set_tag");
    wl_proxy_get_version = (PFN_wl_proxy_get_version)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_get_version");
    wl_proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        bedrock_get_proc_address(wl->api.module_core, "wl_proxy_marshal_flags");
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
        return result_error_missing_procedure;
    }

    wl->api.module_cursor = bedrock_load_dll("libwayland-cursor.so.0");
    if (!wl->api.module_cursor) {
        log_error("Missing libwayland-client.so.0");
        return result_error_missing_shared_library;
    }
    wl_cursor_theme_load = (PFN_wl_cursor_theme_load)
        bedrock_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_load");
    wl_cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        bedrock_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_destroy");
    wl_cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        bedrock_get_proc_address(wl->api.module_cursor, "wl_cursor_theme_get_cursor");
    wl_cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        bedrock_get_proc_address(wl->api.module_cursor, "wl_cursor_image_get_buffer");
    if (!wl->api.wl.cursor_theme_load ||
        !wl->api.wl.cursor_theme_destroy ||
        !wl->api.wl.cursor_theme_get_cursor ||
        !wl->api.wl.cursor_image_get_buffer)
    {
        log_error("Failed to load libwayland-cursor procedures.");
        return result_error_missing_procedure;
    }

    wl->api.module_xkb = bedrock_load_dll("libxkbcommon.so.0");
    if (!wl->api.module_xkb) {
        log_error("Missing libxkbcommon.so.0");
        return result_error_missing_shared_library;
    }
    xkb_context_new = (PFN_xkb_context_new)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_context_new");
    xkb_context_unref = (PFN_xkb_context_unref)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_context_unref");
    xkb_keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_keymap_new_from_string");
    xkb_keymap_unref = (PFN_xkb_keymap_unref)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_keymap_unref");
    xkb_keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_keymap_mod_get_index");
    xkb_keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_keymap_key_repeats");
    xkb_keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_keymap_key_get_syms_by_level");
    xkb_state_new = (PFN_xkb_state_new)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_new");
    xkb_state_unref = (PFN_xkb_state_unref)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_unref");
    xkb_state_key_get_syms = (PFN_xkb_state_key_get_syms)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_key_get_syms");
    xkb_state_update_mask = (PFN_xkb_state_update_mask)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_update_mask");
    xkb_state_key_get_layout = (PFN_xkb_state_key_get_layout)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_key_get_layout");
    xkb_state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_state_mod_index_is_active");
    xkb_compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_table_new_from_locale");
    xkb_compose_table_unref = (PFN_xkb_compose_table_unref)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_table_unref");
    xkb_compose_state_new = (PFN_xkb_compose_state_new)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_state_new");
    xkb_compose_state_unref = (PFN_xkb_compose_state_unref)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_state_unref");
    xkb_compose_state_feed = (PFN_xkb_compose_state_feed)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_state_feed");
    xkb_compose_state_get_status = (PFN_xkb_compose_state_get_status)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_state_get_status");
    xkb_compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        bedrock_get_proc_address(wl->api.module_xkb, "xkb_compose_state_get_one_sym");

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
        return result_error_missing_procedure; 
    }

    wl->registry = wl_display_get_registry(wl->display);
    wl_registry_add_listener(wl->registry, &registry_listener, hadal);

    /* TODO key tables */

    /* sync to get all registry objects */
    wl_display_roundtrip(wl->display);

    /* sync to get initial output events */
    wl_display_roundtrip(wl->display);

    if (!wl->shell) {
        log_error("Wayland compositor is missing xdg-wm-base protocol support.");
        return result_error_wayland_unsupported_protocol;
    } else if (!wl->shm) {
        log_error("Wayland compositor is missing wl-shm support.");
        return result_error_wayland_unsupported_protocol; 
    }

    /* TODO data device manager */

    /* create a window */
    if (title)
        wl->window.title = strdup(title);
    else
        wl->window.title = strdup("A Moonlit Walk Engine");

    if (!create_surface(hadal, width, height))
        return result_error_undefined; // TODO
    if (wl->window.output || at_read_relaxed(&hadal->flags) & hadal_flag_visible) {
        if (!create_xdg_shell_objects(hadal))
            return result_error_undefined; // TODO
    }
    return result_success;
}

AMWAPI void hadal_wayland_display_fini(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    destroy_xdg_shell_objects(hadal);
    if (wl->window.surface)
        wl_surface_destroy(wl->window.surface);
    if (wl->window.title)
        free(wl->window.title);

    if (wl->api.module_xkb)
        bedrock_close_dll(wl->api.module_xkb);
    if (wl->api.module_cursor)
        bedrock_close_dll(wl->api.module_cursor);

    if (wl->subcompositor)
        wl_subcompositor_destroy(wl->subcompositor);
    if (wl->compositor)
        wl_compositor_destroy(wl->compositor);
    if (wl->shm)
        wl_shm_destroy(wl->shm);
    if (wl->viewporter)
        wp_viewporter_destroy(wl->viewporter);
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
        bedrock_close_dll(wl->api.module_core);

    free(wl); // TODO
}

AMWAPI void hadal_wayland_expose_native_window(struct hadopelagic *hadal, void **out_display_handle, void **out_surface_handle)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (out_display_handle)
        *out_display_handle = wl->display;
    if (out_surface_handle)
        *out_surface_handle = wl->window.surface;
}

AMWAPI s32 hadal_wayland_entry_point(struct hadopelagic *hadal, struct ipomoeaalba *ia)
{
    (void)ia; // TODO

    if (WAYLAND != NULL) return result_error_invalid_engine_context;

    struct wayland_display *wl = (struct wayland_display *)malloc(sizeof(struct wayland_display));
    iazerop(wl);

    wl->api.module_core = bedrock_load_dll("libwayland-client.so.0");
    if (!wl->api.module_core) {
        log_debug("Missing libwayland-client.so.0");
        free(wl);
        return result_error_missing_shared_library;
    }

    wl->api.wl.display_connect = (PFN_wl_display_connect)bedrock_get_proc_address(wl->api.module_core, "wl_display_connect");
    if (!wl->api.wl.display_connect) {
        log_debug("Missing libwayland-client entry point procedure");
        bedrock_close_dll(wl->api.module_core);
        free(wl); // TODO
        return result_error_missing_procedure;
    }

    wl->display = wl->api.wl.display_connect(NULL);
    if (!wl->display) {
        log_debug("Can't connect to a Wayland display. Make sure that a Wayland display server is currently running.");
        bedrock_close_dll(wl->api.module_core);
        free(wl); // TODO
        return result_error_missing_procedure;
    }

    /* a wayland display is valid, so we can now setup the hadopelagic struct */
    hadal->display = WAYLAND = wl;
    hadal->backend_api = hadal_backend_api_wayland;
    hadal->backend_name = "wayland";

    hadal->calls = (struct hadal_calls){
        .display_init = hadal_wayland_display_init,
        .display_fini = hadal_wayland_display_fini,
        .get_window_size = hadal_wayland_get_window_size,
        .get_framebuffer_size = hadal_wayland_get_framebuffer_size,
        .show_window = hadal_wayland_show_window,
        .hide_window = hadal_wayland_hide_window,
        .expose_native_window = hadal_wayland_expose_native_window,
    };

    return result_success;
}

#endif /* AMW_NATIVE_WAYLAND */
