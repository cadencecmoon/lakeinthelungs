#include "hadopelagic.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <linux/input.h>
#include <sys/mman.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

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

struct wayland_offer {
    struct wl_data_offer   *offer;
    b32                     text_plain_utf8;
    b32                     text_uri_list;
};

struct wayland_scale {
    struct wl_output       *output;
    s32                     factor;
};

#define WAYLAND_OUTPUT_NAME_INVALID UINT32_MAX
struct wayland_output {
    struct wayland_output  *next;
    struct wayland_output  *prev;

    struct wl_output       *wl_output;
    u32                     name;

    s32                     current_mode;
    s32                     xpos, ypos, scale;
    u32                     width_mm, height_mm;

    /* A window is binded to an output monitor if in fullscreen mode. */
    struct wayland_window  *window;
};

struct wayland_cursor {
    struct wayland_cursor  *next;
    struct wl_cursor       *cursor;
    struct wl_cursor       *cursor_hi_dpi;
    struct wl_buffer       *buffer;
    s32                     width, height;
    s32                     xhot, yhot;
    s32                     current_image;
};

struct wayland_window {
    char                   *app_id;
    char                   *title;

    at_u32                  width, height;
    at_u32                  fb_width, fb_height;

    u32                     minwidth, minheight;
    u32                     maxwidth, maxheight;
    u32                     numer, denom;

    struct wayland_cursor  *current_cursor;
    double                  cursor_xpos, cursor_ypos;

    /* A window is binded to an output monitor if in fullscreen mode. */
    struct wayland_output  *output;

    struct wl_surface      *surface;
    struct wl_callback     *callback;

    struct {
        struct xdg_surface     *surface;
        struct xdg_toplevel    *toplevel;
    } xdg;

    struct {
        u32 width, height, flags;
    } pending;

    s32                             buffer_scale;
    struct wayland_scale           *output_scales;
    usize                           output_scale_count;
    usize                           output_scale_size;

    struct wp_viewport             *scaling_viewport;
    u32                             scaling_numerator;
    struct wp_fractional_scale_v1  *fractional_scale;
};

struct wayland_display {
    const char                     *tag;

    struct wl_display              *display;
    struct wl_registry             *registry;
    struct wl_compositor           *compositor;
    struct wl_subcompositor        *subcompositor;
    struct wl_shm                  *shm;
    struct wl_seat                 *seat;
    struct xdg_wm_base             *shell;
    struct wl_pointer              *pointer;
    struct wl_keyboard             *keyboard;
    struct wp_viewporter           *viewporter;
    struct wl_data_device_manager  *data_device_manager;
    struct wl_data_device          *data_device;

    struct wayland_offer           *offers;
    u32                             offer_count;
    struct wl_data_offer           *selection_offer;
    struct wl_data_source          *selection_source;
    
    char                           *clipboard_string;
    struct wl_data_offer           *drag_offer;
    u32                             drag_serial;
    u32                             serial;
    u32                             pointer_enter_serial;

    struct wl_cursor_theme         *cursor_theme;
    struct wl_cursor_theme         *cursor_theme_hi_dpi;
    struct wl_surface              *cursor_surface;
    const char                     *cursor_previous_name;
    s32                             cursor_timerfd;

    s16                             keycodes[256];
    s16                             scancodes[keycode_last + 1];
    s8                              keynames[keycode_last + 1][5];

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

    /* only support one window at a time */
    struct wayland_window           window;
    struct wayland_output          *outputs;

    struct allocation               internal_allocator;

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
#include <xdg-shell-protocol.h>
#include <viewporter-protocol.h>
#include <fractional-scale-v1-protocol.h>

#include <wayland-protocol-code.h>
#include <xdg-shell-protocol-code.h>
#include <viewporter-protocol-code.h>
#include <fractional-scale-v1-protocol-code.h>

static const struct wl_output_listener output_listener = {

};

static void add_output(struct wayland_display *wl, u32 name, u32 version)
{
    struct allocation *alloc = &wl->internal_allocator;

    if (version < 2) {
        log_error("Wayland: unsupported output interface version for output %u:%u.", name, version);
        return;
    } else if (!alloc || !alloc->data) {
        log_error("Wayland: null allocation structure for output %u:%u was given.", name, version);
        return;
    } else if (alloc->offset + sizeof(struct wayland_output) > alloc->size) {
        log_error("Wayland: no allocation space for output %u:%u", name, version);
        return;
    }

    version = min(version, WL_OUTPUT_NAME_SINCE_VERSION);

    struct wl_output *wl_output = wl_registry_bind(wl->registry, name, &wl_output_interface, version);

    if (!wl_output) return;

    struct wayland_output *prev = NULL;
    struct wayland_output *tail = NULL;
    struct wayland_output *output = NULL;

    /* reuse existing allocations */
    for (tail = wl->outputs; tail; tail = tail->next) {
        if (tail->name == UINT32_MAX) {
            output = tail;
            prev = output->prev;
            break;
        }
        prev = tail->prev;
    }

    if (output == NULL) {
        output = (struct wayland_output *)allocate(alloc, sizeof(struct wayland_output));
        zerop(output);
        output->prev = tail;
    } else {
        /* we can reuse an existing allocation */
        zerop(output);
        output->next = tail;
        output->prev = prev;
    }
    output->scale = 1;
    output->wl_output = wl_output;
    output->name = name;

    if (wl->outputs == NULL) {
        wl->outputs = output;
    }

    wl_proxy_set_tag((struct wl_proxy *)wl_output, &wl->tag);
    wl_output_add_listener(wl_output, &output_listener, output);
}

static void delete_output(struct wayland_output *output)
{
    if (output->wl_output)
        wl_output_destroy(output->wl_output);
    output->name = WAYLAND_OUTPUT_NAME_INVALID;
}

static void resize_framebuffer(struct hadopelagic *hadal, u32 width, u32 height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    /* TODO add fractional scale */
    at_store_explicit(&wl->window.fb_width, width * 1.f, memory_model_seq_cst);   // window->wl.buffer_scale
    at_store_explicit(&wl->window.fb_height, height * 1.f, memory_model_seq_cst); // window->wl.buffer_scale
    at_fetch_or_explicit(&hadal->flags, hadal_flag_framebuffer_resized, memory_model_seq_cst);
}

static b32 resize_window(struct hadopelagic *hadal, u32 width, u32 height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (width == at_read_relaxed(&wl->window.width) && height == at_read_relaxed(&wl->window.height))
        return 0;

    resize_framebuffer(hadal, width, height);
    at_store_explicit(&wl->window.width, width, memory_model_seq_cst);
    at_store_explicit(&wl->window.height, height, memory_model_seq_cst);

    /* TODO scaling viewport */

    return 1;
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

    if (resize_window(hadal, width, height)) {
        /* TODO window resized event */
        if (at_read_relaxed(&hadal->flags) & hadal_flag_window_visible) {
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
        return 0;
    }
    xdg_surface_add_listener(wl->window.xdg.surface, &xdg_surface_listener, hadal);

    wl->window.xdg.toplevel = xdg_surface_get_toplevel(wl->window.xdg.surface);
    if (!wl->window.xdg.toplevel) {
        log_error("Failed to create Wayland xdg-toplevel.");
        return 0;
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
    return 1;
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
        return 0;
    }
    wl_proxy_set_tag((struct wl_proxy *)wl->window.surface, &wl->tag);
    wl_surface_add_listener(wl->window.surface, &surface_listener, hadal);

    at_store_relaxed(&wl->window.width, width);
    at_store_relaxed(&wl->window.height, height);
    at_store_relaxed(&wl->window.fb_width, width);
    at_store_relaxed(&wl->window.fb_height, height);

    /* TODO fractional scale */

    return 1;
}

static void create_key_tables(struct wayland_display *wl)
{
    bedrock_memset(wl->keycodes, -1, sizeof(wl->keycodes));
    bedrock_memset(wl->scancodes, -1, sizeof(wl->scancodes));

    wl->keycodes[KEY_GRAVE]      = keycode_grave_accent;
    wl->keycodes[KEY_1]          = keycode_1;
    wl->keycodes[KEY_2]          = keycode_2;
    wl->keycodes[KEY_3]          = keycode_3;
    wl->keycodes[KEY_4]          = keycode_4;
    wl->keycodes[KEY_5]          = keycode_5;
    wl->keycodes[KEY_6]          = keycode_6;
    wl->keycodes[KEY_7]          = keycode_7;
    wl->keycodes[KEY_8]          = keycode_8;
    wl->keycodes[KEY_9]          = keycode_9;
    wl->keycodes[KEY_0]          = keycode_0;
    wl->keycodes[KEY_SPACE]      = keycode_space;
    wl->keycodes[KEY_MINUS]      = keycode_minus;
    wl->keycodes[KEY_EQUAL]      = keycode_equal;
    wl->keycodes[KEY_Q]          = keycode_q;
    wl->keycodes[KEY_W]          = keycode_w;
    wl->keycodes[KEY_E]          = keycode_e;
    wl->keycodes[KEY_R]          = keycode_r;
    wl->keycodes[KEY_T]          = keycode_t;
    wl->keycodes[KEY_Y]          = keycode_y;
    wl->keycodes[KEY_U]          = keycode_u;
    wl->keycodes[KEY_I]          = keycode_i;
    wl->keycodes[KEY_O]          = keycode_o;
    wl->keycodes[KEY_P]          = keycode_p;
    wl->keycodes[KEY_LEFTBRACE]  = keycode_left_bracket;
    wl->keycodes[KEY_RIGHTBRACE] = keycode_right_bracket;
    wl->keycodes[KEY_A]          = keycode_a;
    wl->keycodes[KEY_S]          = keycode_s;
    wl->keycodes[KEY_D]          = keycode_d;
    wl->keycodes[KEY_F]          = keycode_f;
    wl->keycodes[KEY_G]          = keycode_g;
    wl->keycodes[KEY_H]          = keycode_h;
    wl->keycodes[KEY_J]          = keycode_j;
    wl->keycodes[KEY_K]          = keycode_k;
    wl->keycodes[KEY_L]          = keycode_l;
    wl->keycodes[KEY_SEMICOLON]  = keycode_semicolon;
    wl->keycodes[KEY_APOSTROPHE] = keycode_apostrophe;
    wl->keycodes[KEY_Z]          = keycode_z;
    wl->keycodes[KEY_X]          = keycode_x;
    wl->keycodes[KEY_C]          = keycode_c;
    wl->keycodes[KEY_V]          = keycode_v;
    wl->keycodes[KEY_B]          = keycode_b;
    wl->keycodes[KEY_N]          = keycode_n;
    wl->keycodes[KEY_M]          = keycode_m;
    wl->keycodes[KEY_COMMA]      = keycode_comma;
    wl->keycodes[KEY_DOT]        = keycode_period;
    wl->keycodes[KEY_SLASH]      = keycode_slash;
    wl->keycodes[KEY_BACKSLASH]  = keycode_backslash;
    wl->keycodes[KEY_ESC]        = keycode_escape;
    wl->keycodes[KEY_TAB]        = keycode_tab;
    wl->keycodes[KEY_LEFTSHIFT]  = keycode_left_shift;
    wl->keycodes[KEY_RIGHTSHIFT] = keycode_right_shift;
    wl->keycodes[KEY_LEFTCTRL]   = keycode_left_control;
    wl->keycodes[KEY_RIGHTCTRL]  = keycode_right_control;
    wl->keycodes[KEY_LEFTALT]    = keycode_left_alt;
    wl->keycodes[KEY_RIGHTALT]   = keycode_right_alt;
    wl->keycodes[KEY_LEFTMETA]   = keycode_left_super;
    wl->keycodes[KEY_RIGHTMETA]  = keycode_right_super;
    wl->keycodes[KEY_COMPOSE]    = keycode_menu;
    wl->keycodes[KEY_NUMLOCK]    = keycode_num_lock;
    wl->keycodes[KEY_CAPSLOCK]   = keycode_caps_lock;
    wl->keycodes[KEY_PRINT]      = keycode_print_screen;
    wl->keycodes[KEY_SCROLLLOCK] = keycode_scroll_lock;
    wl->keycodes[KEY_PAUSE]      = keycode_pause;
    wl->keycodes[KEY_DELETE]     = keycode_delete;
    wl->keycodes[KEY_BACKSPACE]  = keycode_backspace;
    wl->keycodes[KEY_ENTER]      = keycode_enter;
    wl->keycodes[KEY_HOME]       = keycode_home;
    wl->keycodes[KEY_END]        = keycode_end;
    wl->keycodes[KEY_PAGEUP]     = keycode_page_up;
    wl->keycodes[KEY_PAGEDOWN]   = keycode_page_down;
    wl->keycodes[KEY_INSERT]     = keycode_insert;
    wl->keycodes[KEY_LEFT]       = keycode_left;
    wl->keycodes[KEY_RIGHT]      = keycode_right;
    wl->keycodes[KEY_DOWN]       = keycode_down;
    wl->keycodes[KEY_UP]         = keycode_up;
    wl->keycodes[KEY_F1]         = keycode_f1;
    wl->keycodes[KEY_F2]         = keycode_f2;
    wl->keycodes[KEY_F3]         = keycode_f3;
    wl->keycodes[KEY_F4]         = keycode_f4;
    wl->keycodes[KEY_F5]         = keycode_f5;
    wl->keycodes[KEY_F6]         = keycode_f6;
    wl->keycodes[KEY_F7]         = keycode_f7;
    wl->keycodes[KEY_F8]         = keycode_f8;
    wl->keycodes[KEY_F9]         = keycode_f9;
    wl->keycodes[KEY_F10]        = keycode_f10;
    wl->keycodes[KEY_F11]        = keycode_f11;
    wl->keycodes[KEY_F12]        = keycode_f12;
    wl->keycodes[KEY_F13]        = keycode_f13;
    wl->keycodes[KEY_F14]        = keycode_f14;
    wl->keycodes[KEY_F15]        = keycode_f15;
    wl->keycodes[KEY_F16]        = keycode_f16;
    wl->keycodes[KEY_F17]        = keycode_f17;
    wl->keycodes[KEY_F18]        = keycode_f18;
    wl->keycodes[KEY_F19]        = keycode_f19;
    wl->keycodes[KEY_F20]        = keycode_f20;
    wl->keycodes[KEY_F21]        = keycode_f21;
    wl->keycodes[KEY_F22]        = keycode_f22;
    wl->keycodes[KEY_F23]        = keycode_f23;
    wl->keycodes[KEY_F24]        = keycode_f24;
    wl->keycodes[KEY_KPSLASH]    = keycode_kp_divide;
    wl->keycodes[KEY_KPASTERISK] = keycode_kp_multiply;
    wl->keycodes[KEY_KPMINUS]    = keycode_kp_subtract;
    wl->keycodes[KEY_KPPLUS]     = keycode_kp_add;
    wl->keycodes[KEY_KP0]        = keycode_kp_0;
    wl->keycodes[KEY_KP1]        = keycode_kp_1;
    wl->keycodes[KEY_KP2]        = keycode_kp_2;
    wl->keycodes[KEY_KP3]        = keycode_kp_3;
    wl->keycodes[KEY_KP4]        = keycode_kp_4;
    wl->keycodes[KEY_KP5]        = keycode_kp_5;
    wl->keycodes[KEY_KP6]        = keycode_kp_6;
    wl->keycodes[KEY_KP7]        = keycode_kp_7;
    wl->keycodes[KEY_KP8]        = keycode_kp_8;
    wl->keycodes[KEY_KP9]        = keycode_kp_9;
    wl->keycodes[KEY_KPDOT]      = keycode_kp_decimal;
    wl->keycodes[KEY_KPEQUAL]    = keycode_kp_equal;
    wl->keycodes[KEY_KPENTER]    = keycode_kp_enter;
    wl->keycodes[KEY_102ND]      = keycode_world_2;      

    for (s32 scancode = 0; scancode < 256; scancode++) {
        if (wl->keycodes[scancode] > 0)
            wl->scancodes[wl->keycodes[scancode]] = scancode;
    }
}

static s32 load_cursor_theme(struct wayland_display *wl)
{
    s32 cursor_size = 16;

    const char *size_string = getenv("XCURSOR_SIZE");
    if (size_string) {
        errno = 0;
        const long cursor_size_long = strtol(size_string, NULL, 10);
        if (errno == 0 && cursor_size_long > 0 && cursor_size_long < INT_MAX)
            cursor_size = (s32)cursor_size_long;
    }

    const char *theme_name = getenv("XCURSOR_THEME");

    wl->cursor_theme = wl_cursor_theme_load(theme_name, cursor_size, wl->shm);
    if (!wl->cursor_theme) {
        log_error("Wayland: failed to load default cursor theme.");
        return result_failure;
    }

    /* if this happens to be NULL, we just fallback to the scale=1 version */
    wl->cursor_theme_hi_dpi = wl_cursor_theme_load(theme_name, cursor_size * 2, wl->shm);

    wl->cursor_surface = wl_compositor_create_surface(wl->compositor);
    wl->cursor_timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    return result_success;
}

static const struct wl_seat_listener seat_listener = {

};

static const struct wl_data_device_listener data_device_listener = {

};

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
        add_output(wl, name, version);

    } else if (!strcmp(interface, "wl_seat")) {
        if (!wl->seat) {
            wl->seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
            wl_seat_add_listener(wl->seat, &seat_listener, NULL);

            if (wl_seat_get_version(wl->seat) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION) {
                wl->key_repeat_timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
            }
        }
    } else if (!strcmp(interface, "wl_data_device_manager")) {
        if (!wl->data_device_manager) {
            wl->data_device_manager = wl_registry_bind(registry, name, &wl_data_device_manager_interface, 1);
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
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    for (struct wayland_output *output = wl->outputs; output; output = output->next) {
        if (output->name == name) {
            delete_output(output);
            return;
        }
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

static void hadal_wayland_get_window_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (out_width)  *out_width  = at_read_explicit(&wl->window.width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&wl->window.height, memory_model_acquire);
}

static void hadal_wayland_get_framebuffer_size(struct hadopelagic *hadal, u32 *out_width, u32 *out_height)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (out_width)  *out_width  = at_read_explicit(&wl->window.fb_width, memory_model_acquire);
    if (out_height) *out_height = at_read_explicit(&wl->window.fb_height, memory_model_acquire);
}

static void hadal_wayland_show_window(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;
    if (!wl->window.xdg.toplevel)
        create_xdg_shell_objects(hadal);
}

static void hadal_wayland_hide_window(struct hadopelagic *hadal)
{
    const struct wayland_display *wl = (const struct wayland_display *)hadal->display;
    destroy_xdg_shell_objects(hadal);
    wl_surface_attach(wl->window.surface, NULL, 0, 0);
    wl_surface_commit(wl->window.surface);
}

static void hadal_wayland_expose_native_window(struct hadopelagic *hadal, void **out_display_handle, void **out_surface_handle)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (out_display_handle)
        *out_display_handle = wl->display;
    if (out_surface_handle)
        *out_surface_handle = wl->window.surface;
}

static s32 hadal_wayland_display_init(
    struct hadopelagic *hadal, 
    struct allocation  *alloc, 
    u32                 width, 
    u32                 height,
    const char         *title)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    if (at_read_relaxed(&hadal->flags) & hadal_flag_display_is_valid) 
        return result_success;
    assert_debug(wl == WAYLAND);

    wl->key_repeat_timerfd = -1;
    wl->cursor_timerfd = -1;

    wl->tag = "A Moonlit Walk Engine";

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
        return result_missing_procedures;
    }

    wl->api.module_cursor = bedrock_load_dll("libwayland-cursor.so.0");
    if (!wl->api.module_cursor) {
        log_error("Missing libwayland-client.so.0");
        return result_missing_library;
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
        return result_missing_procedures;
    }

    wl->api.module_xkb = bedrock_load_dll("libxkbcommon.so.0");
    if (!wl->api.module_xkb) {
        log_error("Missing libxkbcommon.so.0");
        return result_missing_library;
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
        return result_missing_procedures; 
    }

    wl->registry = wl_display_get_registry(wl->display);
    wl_registry_add_listener(wl->registry, &registry_listener, hadal);

    create_key_tables(wl);

    wl->xkb_context = xkb_context_new(0);
    if (!wl->xkb_context) {
        log_error("Wayland: failed to initialize xkb context.");
        return result_failure;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(wl->display);

    /* sync to get initial output events */
    wl_display_roundtrip(wl->display);

    if (!wl->shell) {
        log_error("Wayland compositor is missing xdg-wm-base protocol support.");
        return result_failure;
    } else if (!wl->shm) {
        log_error("Wayland compositor is missing wl-shm support.");
        return result_failure; 
    }
    if (load_cursor_theme(wl) != result_success) 
        return result_failure;

    if (wl->seat && wl->data_device_manager) {
        wl->data_device = wl_data_device_manager_get_data_device(wl->data_device_manager, wl->seat);
        wl_data_device_add_listener(wl->data_device, &data_device_listener, NULL);
    }

    /* TODO data device manager */

    /* create a window */
    wl->window.title = allocate_cstr(alloc, title);
    if (!create_surface(hadal, width, height))
        return result_failure; // TODO
    if (wl->window.output || at_read_relaxed(&hadal->flags) & hadal_flag_window_visible) {
        if (!create_xdg_shell_objects(hadal))
            return result_failure; // TODO
    }
    return result_success;
}

static void hadal_wayland_display_fini(struct hadopelagic *hadal)
{
    struct wayland_display *wl = (struct wayland_display *)hadal->display;

    destroy_xdg_shell_objects(hadal);
    if (wl->window.surface)
        wl_surface_destroy(wl->window.surface);

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
    WAYLAND = NULL;
}

s32 hadal_wayland_entry_point(
    struct hadopelagic *hadal, 
    struct allocation  *alloc,
    b32                 quiet)
{
    if (WAYLAND != NULL) {
        if (!quiet) log_error("Wayland: only one Wayland display backend can be running.");
        return result_failure;
    }

    struct wayland_display *wl = (struct wayland_display *)allocate(alloc, sizeof(struct wayland_display));
    zerop(wl);

    wl->api.module_core = bedrock_load_dll("libwayland-client.so.0");
    if (!wl->api.module_core) {
        if (!quiet) log_error("Missing libwayland-client.so.0");
        return result_missing_library;
    }

    wl->api.wl.display_connect = (PFN_wl_display_connect)bedrock_get_proc_address(wl->api.module_core, "wl_display_connect");
    if (!wl->api.wl.display_connect) {
        if (!quiet) log_error("Missing libwayland-client entry point procedure");
        bedrock_close_dll(wl->api.module_core);
        return result_missing_procedures;
    }

    wl->display = wl->api.wl.display_connect(NULL);
    if (!wl->display) {
        if (!quiet) log_error("Can't connect to a Wayland display. Make sure that a Wayland display server is currently running.");
        bedrock_close_dll(wl->api.module_core);
        return result_missing_procedures;
    }

    usize reserve = sizeof(struct wayland_output) * 4 + 2048;
    wl->internal_allocator.data = allocate(alloc, reserve);
    wl->internal_allocator.size = reserve;

    /* a wayland display is valid, so we can now setup the hadopelagic struct */
    hadal->display = WAYLAND = wl;
    hadal->backend_name = "wayland";
    hadal->backend_id = hadal_backend_wayland;

    hadal->interface = (struct hadal_interface){
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
