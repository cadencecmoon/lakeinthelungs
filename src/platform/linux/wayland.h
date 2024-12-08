/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_INTERNAL_WAYLAND_H
#define _AMW_INTERNAL_WAYLAND_H

#include <amw/hadal.h>
#include <amw/riven.h>

#ifdef AMW_NATIVE_WAYLAND
#include <wayland-client-core.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

struct wl_shm;
struct wl_output;

struct wl_cursor_image {
    u32 width;
    u32 height;
    u32 hotspot_x;
    u32 hotspot_y;
    u32 delay;
};

struct wl_cursor {
    u32 image_count;
    struct wl_cursor_image **images;
    char *name;
};

typedef struct wl_cursor_theme *        (*PFN_wl_cursor_theme_load)(const char *, i32, struct wl_shm *);
typedef void                            (*PFN_wl_cursor_theme_destroy)(struct wl_cursor_theme *);
typedef struct wl_cursor *              (*PFN_wl_cursor_theme_get_cursor)(struct wl_cursor_theme *, const char *);
typedef struct wl_buffer *              (*PFN_wl_cursor_image_get_buffer)(struct wl_cursor_image *);
typedef i32                             (*PFN_wl_display_flush)(struct wl_display *display);
typedef void                            (*PFN_wl_display_cancel_read)(struct wl_display *display);
typedef i32                             (*PFN_wl_display_dispatch_pending)(struct wl_display *display);
typedef i32                             (*PFN_wl_display_read_events)(struct wl_display *display);
typedef struct wl_display *             (*PFN_wl_display_connect)(const char *);
typedef void                            (*PFN_wl_display_disconnect)(struct wl_display *);
typedef i32                             (*PFN_wl_display_roundtrip)(struct wl_display *);
typedef i32                             (*PFN_wl_display_get_fd)(struct wl_display *);
typedef i32                             (*PFN_wl_display_prepare_read)(struct wl_display *);
typedef void                            (*PFN_wl_proxy_marshal)(struct wl_proxy *, u32, ...);
typedef i32                             (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void(**)(void), void *);
typedef void                            (*PFN_wl_proxy_destroy)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor)(struct wl_proxy *, u32, const struct wl_interface *, ...);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *, u32, const struct wl_interface *, u32, ...);
typedef void *                          (*PFN_wl_proxy_get_user_data)(struct wl_proxy *);
typedef void                            (*PFN_wl_proxy_set_user_data)(struct wl_proxy *, void *);
typedef void                            (*PFN_wl_proxy_set_tag)(struct wl_proxy *, const char * const *);
typedef const char * const *            (*PFN_wl_proxy_get_tag)(struct wl_proxy *);
typedef u32                             (*PFN_wl_proxy_get_version)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_flags)(struct wl_proxy *, u32, const struct wl_interface *, u32, u32, ...);
#define wl_cursor_theme_load HADAL.api.wl.cursor_theme_load
#define wl_cursor_theme_destroy HADAL.api.wl.cursor_theme_destroy
#define wl_cursor_theme_get_cursor HADAL.api.wl.cursor_theme_get_cursor
#define wl_cursor_image_get_buffer HADAL.api.wl.cursor_image_get_buffer
#define wl_display_flush HADAL.api.wl.display_flush
#define wl_display_cancel_read HADAL.api.wl.display_cancel_read
#define wl_display_dispatch_pending HADAL.api.wl.display_dispatch_pending
#define wl_display_read_events HADAL.api.wl.display_read_events
#define wl_display_connect HADAL.api.wl.display_connect
#define wl_display_disconnect HADAL.api.wl.display_disconnect
#define wl_display_roundtrip HADAL.api.wl.display_roundtrip
#define wl_display_get_fd HADAL.api.wl.display_get_fd
#define wl_display_prepare_read HADAL.api.wl.display_prepare_read
#define wl_proxy_marshal HADAL.api.wl.proxy_marshal
#define wl_proxy_add_listener HADAL.api.wl.proxy_add_listener
#define wl_proxy_destroy HADAL.api.wl.proxy_destroy
#define wl_proxy_marshal_constructor HADAL.api.wl.proxy_marshal_constructor
#define wl_proxy_marshal_constructor_versioned HADAL.api.wl.proxy_marshal_constructor_versioned
#define wl_proxy_get_user_data HADAL.api.wl.proxy_get_user_data
#define wl_proxy_set_user_data HADAL.api.wl.proxy_set_user_data
#define wl_proxy_set_tag HADAL.api.wl.proxy_set_tag
#define wl_proxy_get_tag HADAL.api.wl.proxy_get_tag
#define wl_proxy_get_version HADAL.api.wl.proxy_get_version
#define wl_proxy_marshal_flags HADAL.api.wl.proxy_marshal_flags

struct hadopelagic_api_wl {
    void *module_core;
    void *module_cursor;

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
};

typedef struct xkb_context *            (*PFN_xkb_context_new)(enum xkb_context_flags);
typedef void                            (*PFN_xkb_context_unref)(struct xkb_context *);
typedef struct xkb_keymap *             (*PFN_xkb_keymap_new_from_string)(struct xkb_context *, const char *, enum xkb_keymap_format, enum xkb_keymap_compile_flags);
typedef void                            (*PFN_xkb_keymap_unref)(struct xkb_keymap *);
typedef xkb_mod_index_t                 (*PFN_xkb_keymap_mod_get_index)(struct xkb_keymap *, const char *);
typedef i32                             (*PFN_xkb_keymap_key_repeats)(struct xkb_keymap *, xkb_keycode_t);
typedef i32                             (*PFN_xkb_keymap_key_get_syms_by_level)(struct xkb_keymap *, xkb_keycode_t, xkb_layout_index_t, xkb_level_index_t, const xkb_keysym_t **);
typedef struct xkb_state *              (*PFN_xkb_state_new)(struct xkb_keymap *);
typedef void                            (*PFN_xkb_state_unref)(struct xkb_state *);
typedef i32                             (*PFN_xkb_state_key_get_syms)(struct xkb_state *, xkb_keycode_t, const xkb_keysym_t **);
typedef enum xkb_state_component        (*PFN_xkb_state_update_mask)(struct xkb_state *, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
typedef xkb_layout_index_t              (*PFN_xkb_state_key_get_layout)(struct xkb_state *, xkb_keycode_t);
typedef i32                             (*PFN_xkb_state_mod_index_is_active)(struct xkb_state *, xkb_mod_index_t, enum xkb_state_component);
typedef struct xkb_compose_table *      (*PFN_xkb_compose_table_new_from_locale)(struct xkb_context *, const char *, enum xkb_compose_compile_flags);
typedef void                            (*PFN_xkb_compose_table_unref)(struct xkb_compose_table *);
typedef struct xkb_compose_state *      (*PFN_xkb_compose_state_new)(struct xkb_compose_table *, enum xkb_compose_state_flags);
typedef void                            (*PFN_xkb_compose_state_unref)(struct xkb_compose_state *);
typedef enum xkb_compose_feed_result    (*PFN_xkb_compose_state_feed)(struct xkb_compose_state *, xkb_keysym_t);
typedef enum xkb_compose_status         (*PFN_xkb_compose_state_get_status)(struct xkb_compose_state *);
typedef xkb_keysym_t                    (*PFN_xkb_compose_state_get_one_sym)(struct xkb_compose_state *);
#define xkb_context_new HADAL.api.xkb.context_new
#define xkb_context_unref HADAL.api.xkb.context_unref
#define xkb_keymap_new_from_string HADAL.api.xkb.keymap_new_from_string
#define xkb_keymap_unref HADAL.api.xkb.keymap_unref
#define xkb_keymap_mod_get_index HADAL.api.xkb.keymap_mod_get_index
#define xkb_keymap_key_repeats HADAL.api.xkb.keymap_key_repeats
#define xkb_keymap_key_get_syms_by_level HADAL.api.xkb.keymap_key_get_syms_by_level
#define xkb_state_new HADAL.api.xkb.state_new
#define xkb_state_unref HADAL.api.xkb.state_unref
#define xkb_state_key_get_syms HADAL.api.xkb.state_key_get_syms
#define xkb_state_update_mask HADAL.api.xkb.state_update_mask
#define xkb_state_key_get_layout HADAL.api.xkb.state_key_get_layout
#define xkb_state_mod_index_is_active HADAL.api.xkb.state_mod_index_is_active
#define xkb_compose_table_new_from_locale HADAL.api.xkb.compose_table_new_from_locale
#define xkb_compose_table_unref HADAL.api.xkb.compose_table_unref
#define xkb_compose_state_new HADAL.api.xkb.compose_state_new
#define xkb_compose_state_unref HADAL.api.xkb.compose_state_unref
#define xkb_compose_state_feed HADAL.api.xkb.compose_state_feed
#define xkb_compose_state_get_status HADAL.api.xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym HADAL.api.xkb.compose_state_get_one_sym

struct hadopelagic_api_xkb {
    void *module;

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
};

struct monitor_wayland {
    struct wl_output *output;
    u32 name;
    i32 current_mode;
    i32 xpos, ypos, scale;
};

struct window_wayland {
    at_u32_t width, height;
    at_u32_t fb_width, fb_height;

    struct wl_surface *surface;

    struct {
        struct xdg_surface  *surface;
        struct xdg_toplevel *toplevel;
    } xdg;

    struct {
        u32 width, height, flags;
    } pending;

    char *app_id;
};

struct hadal_wayland {
    const char *tag;

    struct wl_display       *display;
    struct wl_registry      *registry;
    struct wl_shm           *shm;
    struct wl_seat          *seat;
    struct wl_compositor    *compositor;
    struct wl_subcompositor *subcompositor;
    struct wp_viewporter    *viewporter;
    struct xdg_wm_base      *shell;

    u32 serial;
};

struct window;

extern bool _hadal_wayland_connect(u32 desired_api);
extern i32  _hadal_wayland_init(void);
extern void _hadal_wayland_fini(void);

extern i32  _hadal_wayland_window_create(struct window *window, u32 width, u32 height);
extern void _hadal_wayland_window_destroy(struct window *window);

extern void _hadal_wayland_get_window_size(const struct window *window, u32 *out_width, u32 *out_height);
extern void _hadal_wayland_get_framebuffer_size(const struct window *window, u32 *out_width, u32 *out_height);

extern void _hadal_wayland_visible_show(struct window *window);
extern void _hadal_wayland_visible_hide(struct window *window);

    #define HADOPELAGIC_WAYLAND_MONITOR_STATE   struct monitor_wayland wl;
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE    struct window_wayland wl;
    #define HADOPELAGIC_WAYLAND_API_WL_STATE    struct hadopelagic_api_wl wl;
    #define HADOPELAGIC_WAYLAND_API_XKB_STATE   struct hadopelagic_api_xkb xkb;
    #define HADOPELAGIC_WAYLAND_PLATFORM_STATE  struct hadal_wayland wl;
#else
    #define HADOPELAGIC_WAYLAND_MONITOR_STATE
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE
    #define HADOPELAGIC_WAYLAND_API_WL_STATE
    #define HADOPELAGIC_WAYLAND_API_XKB_STATE
    #define HADOPELAGIC_WAYLAND_PLATFORM_STATE
#endif /* AMW_NATIVE_WAYLAND */
#endif /* _AMW_INTERNAL_WAYLAND_H */
