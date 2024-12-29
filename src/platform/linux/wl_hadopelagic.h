#ifndef _AMW_HADOPELAGIC_WAYLAND_H
#define _AMW_HADOPELAGIC_WAYLAND_H

#include <lake/hadopelagic.h>
#include <lake/riven.h>

#include <wayland-client-core.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

struct wl_shm;
struct wl_output;
struct wl_display;
struct wl_proxy;
struct wl_interface;

typedef int32_t                         (*PFN_wl_display_flush)(struct wl_display *display);
typedef void                            (*PFN_wl_display_cancel_read)(struct wl_display *display);
typedef int32_t                         (*PFN_wl_display_dispatch_pending)(struct wl_display *display);
typedef int32_t                         (*PFN_wl_display_read_events)(struct wl_display *display);
typedef struct wl_display *             (*PFN_wl_display_connect)(const char *);
typedef void                            (*PFN_wl_display_disconnect)(struct wl_display *);
typedef int32_t                         (*PFN_wl_display_roundtrip)(struct wl_display *);
typedef int32_t                         (*PFN_wl_display_get_fd)(struct wl_display *);
typedef int32_t                         (*PFN_wl_display_prepare_read)(struct wl_display *);
typedef void                            (*PFN_wl_proxy_marshal)(struct wl_proxy *, uint32_t, ...);
typedef int32_t                         (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void(**)(void), void *);
typedef void                            (*PFN_wl_proxy_destroy)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor)(struct wl_proxy *, uint32_t, const struct wl_interface *, ...);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *, uint32_t, const struct wl_interface *, uint32_t, ...);
typedef void *                          (*PFN_wl_proxy_get_user_data)(struct wl_proxy *);
typedef void                            (*PFN_wl_proxy_set_user_data)(struct wl_proxy *, void *);
typedef void                            (*PFN_wl_proxy_set_tag)(struct wl_proxy *, const char * const *);
typedef const char * const *            (*PFN_wl_proxy_get_tag)(struct wl_proxy *);
typedef uint32_t                        (*PFN_wl_proxy_get_version)(struct wl_proxy *);
typedef struct wl_proxy *               (*PFN_wl_proxy_marshal_flags)(struct wl_proxy *, uint32_t, const struct wl_interface *, uint32_t, uint32_t, ...);
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
    uint32_t width;
    uint32_t height;
    uint32_t hotspot_x;
    uint32_t hotspot_y;
    uint32_t delay;
};

struct wl_cursor {
    uint32_t image_count;
    struct wl_cursor_image **images;
    char *name;
};

typedef struct wl_cursor_theme *        (*PFN_wl_cursor_theme_load)(const char *, int32_t, struct wl_shm *);
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
typedef int32_t                         (*PFN_xkb_keymap_key_repeats)(struct xkb_keymap *, xkb_keycode_t);
typedef int32_t                         (*PFN_xkb_keymap_key_get_syms_by_level)(struct xkb_keymap *, xkb_keycode_t, xkb_layout_index_t, xkb_level_index_t, const xkb_keysym_t **);
typedef struct xkb_state *              (*PFN_xkb_state_new)(struct xkb_keymap *);
typedef void                            (*PFN_xkb_state_unref)(struct xkb_state *);
typedef int32_t                         (*PFN_xkb_state_key_get_syms)(struct xkb_state *, xkb_keycode_t, const xkb_keysym_t **);
typedef enum xkb_state_component        (*PFN_xkb_state_update_mask)(struct xkb_state *, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
typedef xkb_layout_index_t              (*PFN_xkb_state_key_get_layout)(struct xkb_state *, xkb_keycode_t);
typedef int32_t                         (*PFN_xkb_state_mod_index_is_active)(struct xkb_state *, xkb_mod_index_t, enum xkb_state_component);
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

struct monitor_wayland {
    struct wl_output *output;
    uint32_t    name;
    int32_t     current_mode;
    int32_t     xpos, ypos, scale;
    uint32_t    width_mm, height_mm;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct window_wayland *window;
};

struct window_wayland {
    char       *title;
    at_uint32_t width, height;
    at_uint32_t fb_width, fb_height;
    uint32_t    minwidth, minheight;
    uint32_t    maxwidth, maxheight;
    uint32_t    numer, denom;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct monitor_wayland *monitor;

    struct wl_surface *surface;
    struct {
        struct xdg_surface  *surface;
        struct xdg_toplevel *toplevel;
    } xdg;

    struct {
        uint32_t width, height, flags;
    } pending;

    char *app_id;
};

struct hadopelagic_wayland {
    const char *tag;

    struct wl_display       *display;
    struct wl_registry      *registry;
    struct wl_shm           *shm;
    struct wl_seat          *seat;
    struct wl_compositor    *compositor;
    struct wl_subcompositor *subcompositor;
    struct wp_viewporter    *viewporter;
    struct xdg_wm_base      *shell;

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

    uint32_t serial;
};
extern struct hadopelagic_wayland *WAYLAND;

AMWAPI bool hadopelagic_wayland_entry_point(struct hadopelagic *hadal, uint32_t desired_id);
AMWAPI int32_t hadopelagic_wayland_init(struct hadopelagic *hadal);
AMWAPI void hadopelagic_wayland_fini(struct hadopelagic *hadal);

AMWAPI int32_t hadopelagic_wayland_create_window(struct hadopelagic *hadal, uint32_t width, uint32_t height);
AMWAPI void hadopelagic_wayland_destroy_window(struct hadopelagic *hadal);

AMWAPI void hadopelagic_wayland_get_window_size(const struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);
AMWAPI void hadopelagic_wayland_get_framebuffer_size(const struct hadopelagic *hadal, uint32_t *out_width, uint32_t *out_height);

AMWAPI void hadopelagic_wayland_show_window(struct hadopelagic *hadal);
AMWAPI void hadopelagic_wayland_hide_window(struct hadopelagic *hadal);

/* Protocols are generated with wayland-scanner, their sources are included in
 * the project repository: resources/wayland/<protocol>.xml.
 *
 * We need the WAYLAND global and some macro magic to redirect the procedures declared by 
 * wayland-client-core to be available in the generated protocols, when dynamically 
 * loading the pointers to the procedures at runtime. The global pointer also helps with 
 * keeping the backends clean, if for some reasons multiple wayland displays were to be 
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

#endif /* _AMW_HADAL_WAYLAND_H */
