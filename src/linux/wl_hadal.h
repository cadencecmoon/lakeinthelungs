#pragma once

#include <amw/bedrock.h>
#include <amw/hadal.h>

FN_HADAL_WINDOW_CREATE(wayland);
FN_HADAL_WINDOW_DESTROY(wayland);
FN_HADAL_WINDOW_ATTACH_SWAPCHAIN(wayland);
FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(wayland);
#ifdef REZNOR_VULKAN
FN_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES(wayland);
FN_HADAL_VULKAN_SURFACE_CREATE(wayland);
FN_HADAL_VULKAN_PHYSICAL_DEVICE_PRESENTATION_SUPPORT(wayland);
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/timerfd.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

struct wl_interface;
struct wl_proxy;
struct wl_event_queue;
struct wl_display;
struct wl_surface;
struct wl_seat;

#include <wayland-client-core.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

typedef void (*PFN_wl_proxy_marshal)(struct wl_proxy *, u32, ...);
typedef struct wl_proxy *(*PFN_wl_proxy_create)(struct wl_proxy *, const struct wl_interface *);
typedef void (*PFN_wl_proxy_destroy)(struct wl_proxy *);
typedef s32 (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void(**)(void), void *);
typedef void (*PFN_wl_proxy_set_user_data)(struct wl_proxy *, void *);
typedef void *(*PFN_wl_proxy_get_user_data)(struct wl_proxy *);
typedef u32 (*PFN_wl_proxy_get_version)(struct wl_proxy *);
typedef u32 (*PFN_wl_proxy_get_id)(struct wl_proxy *);
typedef const char *(*PFN_wl_proxy_get_class)(struct wl_proxy *);
typedef void (*PFN_wl_proxy_set_queue)(struct wl_proxy *, struct wl_event_queue *);
typedef void *(*PFN_wl_proxy_create_wrapper)(void *);
typedef void (*PFN_wl_proxy_wrapper_destroy)(void *);
typedef struct wl_proxy *(*PFN_wl_proxy_marshal_constructor)(struct wl_proxy *, u32, const struct wl_interface *, ...);
typedef struct wl_proxy *(*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *, u32, const struct wl_interface *, u32, ...);
typedef struct wl_proxy *(*PFN_wl_proxy_marshal_flags)(struct wl_proxy *, u32, const struct wl_interface *, u32, u32, ...);
typedef struct wl_proxy *(*PFN_wl_proxy_marshal_array_flags)(struct wl_proxy *, u32, const struct wl_interface *, u32, u32, union wl_argument *);
typedef void (*PFN_wl_proxy_set_tag)(struct wl_proxy *, const char * const *);
typedef const char * const * (*PFN_wl_proxy_get_tag)(struct wl_proxy *);
#define wl_proxy_marshal                                    g_wayland->wl.proxy_marshal
#define wl_proxy_create                                     g_wayland->wl.proxy_create
#define wl_proxy_destroy                                    g_wayland->wl.proxy_destroy
#define wl_proxy_add_listener                               g_wayland->wl.proxy_add_listener
#define wl_proxy_set_user_data                              g_wayland->wl.proxy_set_user_data
#define wl_proxy_get_user_data                              g_wayland->wl.proxy_get_user_data
#define wl_proxy_get_version                                g_wayland->wl.proxy_get_version
#define wl_proxy_get_id                                     g_wayland->wl.proxy_get_id
#define wl_proxy_get_class                                  g_wayland->wl.proxy_get_class
#define wl_proxy_set_queue                                  g_wayland->wl.proxy_set_queue
#define wl_proxy_create_wrapper                             g_wayland->wl.proxy_create_wrapper
#define wl_proxy_wrapper_destroy                            g_wayland->wl.proxy_wrapper_destroy
#define wl_proxy_marshal_constructor                        g_wayland->wl.proxy_marshal_constructor
#define wl_proxy_marshal_constructor_versioned              g_wayland->wl.proxy_marshal_constructor_versioned
#define wl_proxy_marshal_flags                              g_wayland->wl.proxy_marshal_flags
#define wl_proxy_marshal_array_flags                        g_wayland->wl.proxy_marshal_array_flags
#define wl_proxy_set_tag                                    g_wayland->wl.proxy_set_tag
#define wl_proxy_get_tag                                    g_wayland->wl.proxy_get_tag

typedef struct wl_display *(*PFN_wl_display_connect)(const char *);
typedef struct wl_display *(*PFN_wl_display_connect_to_fd)(s32);
typedef void (*PFN_wl_display_disconnect)(struct wl_display *);
typedef s32 (*PFN_wl_display_get_fd)(struct wl_display *);
typedef s32 (*PFN_wl_display_dispatch)(struct wl_display *);
typedef s32 (*PFN_wl_display_dispatch_queue)(struct wl_display *, struct wl_event_queue *);
typedef s32 (*PFN_wl_display_dispatch_queue_pending)(struct wl_display *, struct wl_event_queue *);
typedef s32 (*PFN_wl_display_dispatch_pending)(struct wl_display *);
typedef s32 (*PFN_wl_display_prepare_read)(struct wl_display *);
typedef s32 (*PFN_wl_display_prepare_read_queue)(struct wl_display *, struct wl_event_queue *);
typedef s32 (*PFN_wl_display_read_events)(struct wl_display *);
typedef void (*PFN_wl_display_cancel_read)(struct wl_display *);
typedef s32 (*PFN_wl_display_get_error)(struct wl_display *);
typedef s32 (*PFN_wl_display_flush)(struct wl_display *);
typedef s32 (*PFN_wl_display_roundtrip)(struct wl_display *);
typedef struct wl_event_queue *(*PFN_wl_display_create_queue)(struct wl_display *);
typedef void (*PFN_wl_event_queue_destroy)(struct wl_event_queue *);
typedef void (*PFN_wl_log_set_handler_client)(wl_log_func_t);
typedef void (*PFN_wl_list_init)(struct wl_list *);
typedef void (*PFN_wl_list_insert)(struct wl_list *, struct wl_list *);
typedef void (*PFN_wl_list_remove)(struct wl_list *);
typedef s32 (*PFN_wl_list_length)(const struct wl_list *);
typedef s32 (*PFN_wl_list_empty)(const struct wl_list *);
typedef void (*PFN_wl_list_insert_list)(struct wl_list *, struct wl_list *);
#define wl_display_connect                                  g_wayland->wl.display_connect
#define wl_display_connect_to_fd                            g_wayland->wl.display_connect_to_fd
#define wl_display_disconnect                               g_wayland->wl.display_disconnect
#define wl_display_get_fd                                   g_wayland->wl.display_get_fd
#define wl_display_dispatch                                 g_wayland->wl.display_dispatch
#define wl_display_dispatch_queue                           g_wayland->wl.display_dispatch_queue
#define wl_display_dispatch_queue_pending                   g_wayland->wl.display_dispatch_queue_pending
#define wl_display_dispatch_pending                         g_wayland->wl.display_dispatch_pending
#define wl_display_prepare_read                             g_wayland->wl.display_prepare_read
#define wl_display_prepare_read_queue                       g_wayland->wl.display_prepare_read_queue
#define wl_display_read_events                              g_wayland->wl.display_read_events
#define wl_display_cancel_read                              g_wayland->wl.display_cancel_read
#define wl_display_get_error                                g_wayland->wl.display_get_error
#define wl_display_flush                                    g_wayland->wl.display_flush
#define wl_display_roundtrip                                g_wayland->wl.display_roundtrip
#define wl_display_create_queue                             g_wayland->wl.display_create_queue
#define wl_event_queue_destroy                              g_wayland->wl.event_queue_destroy
#define wl_log_set_handler_client                           g_wayland->wl.log_set_handler_client
#define wl_list_init                                        g_wayland->wl.list_init
#define wl_list_insert                                      g_wayland->wl.list_insert
#define wl_list_remove                                      g_wayland->wl.list_remove
#define wl_list_length                                      g_wayland->wl.list_length
#define wl_list_empty                                       g_wayland->wl.list_empty
#define wl_list_insert_list                                 g_wayland->wl.list_insert_list

struct wl_shm;
struct wl_output;

struct wl_cursor_image {
    uint32_t width;
    uint32_t height;
    uint32_t hotspot_x;
    uint32_t hotspot_y;
    uint32_t delay;
};
struct wl_cursor {
    unsigned int image_count;
    struct wl_cursor_image** images;
    char* name;
};

typedef struct wl_cursor_theme *(*PFN_wl_cursor_theme_load)(const char *, s32, struct wl_shm *);
typedef void (*PFN_wl_cursor_theme_destroy)(struct wl_cursor_theme *);
typedef struct wl_cursor *(*PFN_wl_cursor_theme_get_cursor)(struct wl_cursor_theme *, const char *);
typedef struct wl_buffer *(*PFN_wl_cursor_image_get_buffer)(struct wl_cursor_image *);
typedef s32 (*PFN_wl_cursor_frame)(struct wl_cursor *, u32);
#define wl_cursor_theme_load                                g_wayland->wl.cursor_theme_load
#define wl_cursor_theme_destroy                             g_wayland->wl.cursor_theme_destroy
#define wl_cursor_theme_get_cursor                          g_wayland->wl.cursor_theme_get_cursor
#define wl_cursor_image_get_buffer                          g_wayland->wl.cursor_image_get_buffer
#define wl_cursor_frame                                     g_wayland->wl.cursor_frame

typedef struct xkb_context *(*PFN_xkb_context_new)(enum xkb_context_flags);
typedef void (*PFN_xkb_context_unref)(struct xkb_context *);
typedef struct xkb_keymap *(*PFN_xkb_keymap_new_from_string)(struct xkb_context *, const char *, enum xkb_keymap_format, enum xkb_keymap_compile_flags);
typedef void (*PFN_xkb_keymap_unref)(struct xkb_keymap *);
typedef u32 (*PFN_xkb_keymap_mod_get_index)(struct xkb_keymap *, const char *);
typedef s32 (*PFN_xkb_keymap_key_repeats)(struct xkb_keymap *, xkb_keycode_t);
typedef void (*PFN_xkb_keymap_key_for_each)(struct xkb_keymap *, xkb_keymap_key_iter_t, void *);
typedef s32 (*PFN_xkb_keymap_key_get_syms_by_level)(struct xkb_keymap *, xkb_keycode_t, xkb_layout_index_t, xkb_layout_index_t, const xkb_keysym_t **);
typedef const char *(*PFN_xkb_keymap_layout_get_name)(struct xkb_keymap *, xkb_layout_index_t);
typedef s32 (*PFN_xkb_keysym_to_utf8)(xkb_keysym_t, char *, usize);
typedef u32 (*PFN_xkb_keysym_to_utf32)(xkb_keysym_t);
typedef struct xkb_state *(*PFN_xkb_state_new)(struct xkb_keymap *);
typedef void (*PFN_xkb_state_unref)(struct xkb_state *);
typedef s32 (*PFN_xkb_state_key_get_syms)(struct xkb_state *, xkb_keycode_t, const xkb_keysym_t **);
typedef xkb_layout_index_t (*PFN_xkb_state_key_get_layout)(struct xkb_state *, xkb_keycode_t);
typedef s32 (*PFN_xkb_state_mod_index_is_active)(struct xkb_state *, xkb_mod_index_t, enum xkb_state_component);
typedef enum xkb_state_component (*PFN_xkb_state_update_mask)(struct xkb_state *, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
typedef struct xkb_compose_table *(*PFN_xkb_compose_table_new_from_locale)(struct xkb_context *, const char *, enum xkb_compose_compile_flags);
typedef void (*PFN_xkb_compose_table_unref)(struct xkb_compose_table *);
typedef struct xkb_compose_state *(*PFN_xkb_compose_state_new)(struct xkb_compose_table *, enum xkb_compose_state_flags);
typedef void (*PFN_xkb_compose_state_reset)(struct xkb_compose_state *);
typedef void (*PFN_xkb_compose_state_unref)(struct xkb_compose_state *);
typedef enum xkb_compose_feed_result (*PFN_xkb_compose_state_feed)(struct xkb_compose_state *, xkb_keysym_t);
typedef enum xkb_compose_status (*PFN_xkb_compose_state_get_status)(struct xkb_compose_state *);
typedef xkb_keysym_t (*PFN_xkb_compose_state_get_one_sym)(struct xkb_compose_state *);
#define xkb_context_new                                     g_wayland->xkb.context_new
#define xkb_context_unref                                   g_wayland->xkb.context_unref
#define xkb_keymap_new_from_string                          g_wayland->xkb.keymap_new_from_string
#define xkb_keymap_unref                                    g_wayland->xkb.keymap_unref
#define xkb_keymap_mod_get_index                            g_wayland->xkb.keymap_mod_get_index
#define xkb_keymap_key_repeats                              g_wayland->xkb.keymap_key_repeats
#define xkb_keymap_key_for_each                             g_wayland->xkb.keymap_for_each
#define xkb_keymap_key_get_syms_by_level                    g_wayland->xkb.keymap_key_get_syms_by_level
#define xkb_keymap_layout_get_name                          g_wayland->xkb.keymap_layout_get_name
#define xkb_keysym_to_utf8                                  g_wayland->xkb.keysym_to_utf8
#define xkb_keysym_to_utf32                                 g_wayland->xkb.keysym_to_utf32
#define xkb_state_new                                       g_wayland->xkb.state_new
#define xkb_state_unref                                     g_wayland->xkb.state_unref
#define xkb_state_key_get_syms                              g_wayland->xkb.state_key_get_syms
#define xkb_state_key_get_layout                            g_wayland->xkb.state_key_get_layout
#define xkb_state_mod_index_is_active                       g_wayland->xkb.state_mod_index_is_active
#define xkb_state_update_mask                               g_wayland->xkb.state_update_mask
#define xkb_compose_table_new_from_locale                   g_wayland->xkb.compose_table_new_from_locale
#define xkb_compose_table_unref                             g_wayland->xkb.compose_table_unref
#define xkb_compose_state_new                               g_wayland->xkb.compose_state_new
#define xkb_compose_state_reset                             g_wayland->xkb.compose_state_reset
#define xkb_compose_state_feed                              g_wayland->xkb.compose_state_feed
#define xkb_compose_state_get_status                        g_wayland->xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym                       g_wayland->xkb.compose_state_get_one_sym

struct libdecor;
struct libdecor_frame;
struct libdecor_state;
struct libdecor_configuration;

enum libdecor_error
{
	LIBDECOR_ERROR_COMPOSITOR_INCOMPATIBLE,
	LIBDECOR_ERROR_INVALID_FRAME_CONFIGURATION,
};

enum libdecor_window_state
{
	LIBDECOR_WINDOW_STATE_NONE = 0,
	LIBDECOR_WINDOW_STATE_ACTIVE = 1,
	LIBDECOR_WINDOW_STATE_MAXIMIZED = 2,
	LIBDECOR_WINDOW_STATE_FULLSCREEN = 4,
	LIBDECOR_WINDOW_STATE_TILED_LEFT = 8,
	LIBDECOR_WINDOW_STATE_TILED_RIGHT = 16,
	LIBDECOR_WINDOW_STATE_TILED_TOP = 32,
	LIBDECOR_WINDOW_STATE_TILED_BOTTOM = 64
};

enum libdecor_capabilities
{
	LIBDECOR_ACTION_MOVE = 1,
	LIBDECOR_ACTION_RESIZE = 2,
	LIBDECOR_ACTION_MINIMIZE = 4,
	LIBDECOR_ACTION_FULLSCREEN = 8,
	LIBDECOR_ACTION_CLOSE = 16
};

struct libdecor_interface
{
	void (*error)(struct libdecor*,enum libdecor_error,const char*);
	void (*reserved0)(void);
	void (*reserved1)(void);
	void (*reserved2)(void);
	void (*reserved3)(void);
	void (*reserved4)(void);
	void (*reserved5)(void);
	void (*reserved6)(void);
	void (*reserved7)(void);
	void (*reserved8)(void);
	void (*reserved9)(void);
};

struct libdecor_frame_interface
{
	void (*configure)(struct libdecor_frame*,struct libdecor_configuration*,void*);
	void (*close)(struct libdecor_frame*,void*);
	void (*commit)(struct libdecor_frame*,void*);
	void (*dismiss_popup)(struct libdecor_frame*,const char*,void*);
	void (*reserved0)(void);
	void (*reserved1)(void);
	void (*reserved2)(void);
	void (*reserved3)(void);
	void (*reserved4)(void);
	void (*reserved5)(void);
	void (*reserved6)(void);
	void (*reserved7)(void);
	void (*reserved8)(void);
	void (*reserved9)(void);
};

enum libdecor_resize_edge {
	LIBDECOR_RESIZE_EDGE_NONE,
	LIBDECOR_RESIZE_EDGE_TOP,
	LIBDECOR_RESIZE_EDGE_BOTTOM,
	LIBDECOR_RESIZE_EDGE_LEFT,
	LIBDECOR_RESIZE_EDGE_TOP_LEFT,
	LIBDECOR_RESIZE_EDGE_BOTTOM_LEFT,
	LIBDECOR_RESIZE_EDGE_RIGHT,
	LIBDECOR_RESIZE_EDGE_TOP_RIGHT,
	LIBDECOR_RESIZE_EDGE_BOTTOM_RIGHT,
};

typedef struct libdecor *(*PFN_libdecor_new)(struct wl_display *, struct libdecor_interface *);
typedef void (*PFN_libdecor_unref)(struct libdecor *);
typedef struct libdecor_frame *(*PFN_libdecor_decorate)(struct libdecor *, struct wl_surface *, struct libdecor_frame_interface *, void *);
typedef void (*PFN_libdecor_frame_unref)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_title)(struct libdecor_frame *, const char *);
typedef void (*PFN_libdecor_frame_set_app_id)(struct libdecor_frame *, const char *);
typedef void (*PFN_libdecor_frame_set_max_content_size)(struct libdecor_frame *, s32, s32);
typedef void (*PFN_libdecor_frame_get_max_content_size)(const struct libdecor_frame *, s32 *, s32 *);
typedef void (*PFN_libdecor_frame_set_min_content_size)(struct libdecor_frame *, s32, s32);
typedef void (*PFN_libdecor_frame_get_min_content_size)(const struct libdecor_frame *, s32 *, s32 *);
typedef void (*PFN_libdecor_frame_resize)(struct libdecor_frame *, struct wl_seat *, u32, enum libdecor_resize_edge);
typedef void (*PFN_libdecor_frame_move)(struct libdecor_frame *, struct wl_seat *, u32);
typedef void (*PFN_libdecor_frame_commit)(struct libdecor_frame *, struct libdecor_state *, struct libdecor_configuration *);
typedef void (*PFN_libdecor_frame_set_minimized)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_maximized)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_unset_maximized)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_fullscreen)(struct libdecor_frame *, struct wl_output *);
typedef void (*PFN_libdecor_frame_unset_fullscreen)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_capabilities)(struct libdecor_frame *, enum libdecor_capabilities);
typedef void (*PFN_libdecor_frame_unset_capabilities)(struct libdecor_frame *, enum libdecor_capabilities);
typedef b32 (*PFN_libdecor_frame_has_capability)(struct libdecor_frame *, enum libdecor_capabilities);
typedef void (*PFN_libdecor_frame_set_visibility)(struct libdecor_frame *, b32);
typedef b32 (*PFN_libdecor_frame_is_visible)(struct libdecor_frame *);
typedef b32 (*PFN_libdecor_frame_is_floating)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_parent)(struct libdecor_frame *, struct libdecor_frame *);
typedef struct xdg_surface *(*PFN_libdecor_frame_get_xdg_surface)(struct libdecor_frame *);
typedef struct xdg_toplevel *(*PFN_libdecor_frame_get_xdg_toplevel)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_map)(struct libdecor_frame *);
typedef struct libdecor_state *(*PFN_libdecor_state_new)(s32, s32);
typedef void (*PFN_libdecor_state_free)(struct libdecor_state *);
typedef b32 (*PFN_libdecor_configuration_get_content_size)(struct libdecor_configuration *, struct libdecor_frame *, s32 *, s32 *);
typedef b32 (*PFN_libdecor_configuration_get_window_state)(struct libdecor_configuration *, enum libdecor_window_state *);
typedef s32 (*PFN_libdecor_dispatch)(struct libdecor *, s32);
#define libdecor_new                                        g_wayland->libdecor.new
#define libdecor_unref                                      g_wayland->libdecor.unref
#define libdecor_decorate                                   g_wayland->libdecor.decorate
#define libdecor_frame_unref                                g_wayland->libdecor.frame_unref
#define libdecor_frame_set_title                            g_wayland->libdecor.frame_set_title
#define libdecor_frame_set_app_id                           g_wayland->libdecor.frame_set_app_id
#define libdecor_frame_set_max_content_size                 g_wayland->libdecor.frame_set_max_content_size
#define libdecor_frame_get_max_content_size                 g_wayland->libdecor.frame_get_max_content_size
#define libdecor_frame_set_min_content_size                 g_wayland->libdecor.frame_set_min_content_size
#define libdecor_frame_get_min_content_size                 g_wayland->libdecor.frame_get_min_content_size
#define libdecor_frame_resize                               g_wayland->libdecor.frame_resize
#define libdecor_frame_move                                 g_wayland->libdecor.frame_move
#define libdecor_frame_commit                               g_wayland->libdecor.frame_commit
#define libdecor_frame_set_minimized                        g_wayland->libdecor.frame_set_minimized
#define libdecor_frame_set_maximized                        g_wayland->libdecor.frame_set_maximized
#define libdecor_frame_unset_maximized                      g_wayland->libdecor.frame_unset_maximized
#define libdecor_frame_set_fullscreen                       g_wayland->libdecor.frame_set_fullscreen
#define libdecor_frame_unset_fullscreen                     g_wayland->libdecor.frame_unset_fullscreen
#define libdecor_frame_set_capabilities                     g_wayland->libdecor.frame_set_capabilities
#define libdecor_frame_unset_capabilities                   g_wayland->libdecor.frame_unset_capabilities
#define libdecor_frame_has_capability                       g_wayland->libdecor.frame_has_capability
#define libdecor_frame_set_visibility                       g_wayland->libdecor.frame_set_visibility
#define libdecor_frame_is_visible                           g_wayland->libdecor.frame_is_visible
#define libdecor_frame_is_floating                          g_wayland->libdecor.frame_is_floating
#define libdecor_frame_set_parent                           g_wayland->libdecor.frame_set_parent
#define libdecor_frame_get_xdg_surface                      g_wayland->libdecor.frame_get_xdg_surface
#define libdecor_frame_get_xdg_toplevel                     g_wayland->libdecor.frame_get_xdg_toplevel
#define libdecor_frame_map                                  g_wayland->libdecor.frame_map
#define libdecor_state_new                                  g_wayland->libdecor.state_new
#define libdecor_state_free                                 g_wayland->libdecor.state_free
#define libdecor_configuration_get_content_size             g_wayland->libdecor.configuration_get_content_size
#define libdecor_configuration_get_window_state             g_wayland->libdecor.configuration_get_window_state
#define libdecor_dispatch                                   g_wayland->libdecor.dispatch

#if defined(REZNOR_VULKAN) && !defined(VULKAN_H_)
typedef u32 VkBool32;
typedef u32 VkFlags;
enum VkResult;

enum VkStructureType {
    VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR = 1000006000,
};
typedef VkFlags VkWaylandSurfaceCreateFlagsKHR;

struct VkWaylandSurfaceCreateInfoKHR {
    enum VkStructureType            sType;
    const void                     *pNext;
    VkWaylandSurfaceCreateFlagsKHR  flags;
    struct wl_display              *display;
    struct wl_surface              *surface;
};
typedef s32 (*PFN_vkCreateWaylandSurfaceKHR)(
    struct VkInstance_T *,
    const struct VkWaylandSurfaceCreateInfoKHR *,
    const struct VkAllocationCallbacks *, 
    struct VkSurfaceKHR_T **);

typedef VkBool32 (*PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(
    struct VkPhysicalDevice_T *, u32, struct wl_display *);
#endif /* REZNOR_VULKAN */

/* shared between linux display backends */
#include "joystick.h"

struct wayland_cursor_theme {
    struct wl_cursor_theme             *theme;
    usize                               size;
};

struct hadal_pen {
    struct hadal_pen_header             header;
};

struct hadal_touch {
    struct hadal_touch_header           header;
};

struct hadal_mouse {
    struct hadal_mouse_header           header;
};

struct hadal_keyboard {
    struct hadal_keyboard_header        header;
};

struct hadal_window {
    struct hadal_window_header          header;
    const char                         *tag;
    struct wl_surface                  *surface;

    union {
        struct {
            struct libdecor_frame      *frame;
            b32                         initial_config_seen;
        } libdecor;
        struct {
            struct xdg_surface         *surface;
            union {
                struct xdg_toplevel    *toplevel;
                struct {
                    struct xdg_popup       *popup;
                    struct xdg_positioner  *positioner;
                    u32                     parent_id;
                    struct hadal_window    *child;
                } popup;
            } roleobj;
            b32                         initial_config_seen;
        } xdg;
    } shell_surface;

    enum {
        WAYLAND_SURFACE_UNKNOWN = 0,
        WAYLAND_SURFACE_XDG_TOPLEVEL,
        WAYLAND_SURFACE_XDG_POPUP,
        WAYLAND_SURFACE_LIBDECOR
    } shell_surface_type;

    struct hadal_output   **outputs;
    u32                     outputs_count;

    u32                     floating_width, floating_height;
    u32                     fb_extent_width, fb_extent_height;
    u32                     window_width, window_height;
    u32                     toplevel_bounds_width, toplevel_bounds_height;
};

struct hadal_output {
    struct hadal_output_header                              header;
    struct wl_output                                       *output;
    struct zxdg_output_v1                                  *xdg_output;
};

/** The Wayland display backend implementation of the Hadal interface. */
struct hadal {
    struct hadal_interface                                  interface;

    struct wl_display                                      *display;
    struct wl_registry                                     *registry;
    struct wl_compositor                                   *compositor;
    struct wl_subcompositor                                *subcompositor;
    struct wl_shm                                          *shm;
    struct wl_pointer                                      *pointer;
    struct wl_seat                                         *seat;

    struct {
        struct xdg_wm_base                                 *xdg;
        struct libdecor                                    *libdecor;
    } shell;

    struct wayland_cursor_theme                            *cursor_themes;
    u32                                                     cursor_themes_count;

    s32                                                     cursor_timerfd;
    s32                                                     key_repeat_timerfd;
    s32                                                     key_repeat_rate;
    s32                                                     key_repeat_delay;
    s32                                                     key_repeat_scancode;
    s32                                                     relative_mouse_mode;

    struct xkb_context                                     *xkb_context;
    struct xkb_keymap                                      *xkb_keymap;
    struct xkb_state                                       *xkb_state;
    struct xkb_compose_state                               *xkb_compose_state;

    xkb_mod_index_t                                         xkb_control_idx;
    xkb_mod_index_t                                         xkb_alt_idx;
    xkb_mod_index_t                                         xkb_shift_idx;
    xkb_mod_index_t                                         xkb_super_idx;
    xkb_mod_index_t                                         xkb_caps_lock_idx;
    xkb_mod_index_t                                         xkb_num_lock_idx;
    u32                                                     xkb_modifiers;

    struct zwp_relative_pointer_manager_v1                 *relative_pointer_manager;
    struct zwp_pointer_constraints_v1                      *pointer_constraints;
    struct wp_cursor_shape_manager_v1                      *cursor_shape_manager;
    struct wl_data_device_manager                          *data_device_manager;
    struct zwp_primary_selection_device_manager_v1         *primary_selection_device_manager;
    struct zxdg_decoration_manager_v1                      *decoration_manager;
    struct zwp_keyboard_shortcuts_inhibit_manager_v1       *key_inhibitor_manager;
    struct zwp_idle_inhibit_manager_v1                     *idle_inhibit_manager;
    struct xdg_activation_v1                               *activation_manager;
    struct zwp_text_input_manager_v3                       *text_input_manager;
    struct xdg_toplevel_icon_manager_v1                    *xdg_toplevel_icon_manager;
    struct zxdg_output_manager_v1                          *xdg_output_manager;
    struct wp_viewporter                                   *viewporter;
    struct wp_fractional_scale_manager_v1                  *fractional_scale_manager;

    void                                                   *module_client;
    void                                                   *module_cursor;
    void                                                   *module_xkbcommon;
    void                                                   *module_libdecor;

    struct {
        PFN_wl_proxy_marshal                                proxy_marshal;
        PFN_wl_proxy_create                                 proxy_create;
        PFN_wl_proxy_destroy                                proxy_destroy;
        PFN_wl_proxy_add_listener                           proxy_add_listener;
        PFN_wl_proxy_set_user_data                          proxy_set_user_data;
        PFN_wl_proxy_get_user_data                          proxy_get_user_data;
        PFN_wl_proxy_get_version                            proxy_get_version;
        PFN_wl_proxy_get_id                                 proxy_get_id;
        PFN_wl_proxy_get_class                              proxy_get_class;
        PFN_wl_proxy_set_queue                              proxy_set_queue;
        PFN_wl_proxy_create_wrapper                         proxy_create_wrapper;
        PFN_wl_proxy_wrapper_destroy                        proxy_wrapper_destroy;
        PFN_wl_proxy_marshal_constructor                    proxy_marshal_constructor;
        PFN_wl_proxy_marshal_constructor_versioned          proxy_marshal_constructor_versioned;
        PFN_wl_proxy_marshal_flags                          proxy_marshal_flags;
        PFN_wl_proxy_marshal_array_flags                    proxy_marshal_array_flags;
        PFN_wl_proxy_set_tag                                proxy_set_tag;
        PFN_wl_proxy_get_tag                                proxy_get_tag;

        PFN_wl_display_connect                              display_connect;
        PFN_wl_display_connect_to_fd                        display_connect_to_fd;
        PFN_wl_display_disconnect                           display_disconnect;
        PFN_wl_display_get_fd                               display_get_fd;
        PFN_wl_display_dispatch                             display_dispatch;
        PFN_wl_display_dispatch_queue                       display_dispatch_queue;
        PFN_wl_display_dispatch_queue_pending               display_dispatch_queue_pending;
        PFN_wl_display_dispatch_pending                     display_dispatch_pending;
        PFN_wl_display_prepare_read                         display_prepare_read;
        PFN_wl_display_prepare_read_queue                   display_prepare_read_queue;
        PFN_wl_display_read_events                          display_read_events;
        PFN_wl_display_cancel_read                          display_cancel_read;
        PFN_wl_display_get_error                            display_get_error;
        PFN_wl_display_flush                                display_flush;
        PFN_wl_display_roundtrip                            display_roundtrip;
        PFN_wl_display_create_queue                         display_create_queue;
        PFN_wl_event_queue_destroy                          event_queue_destroy;
        PFN_wl_log_set_handler_client                       log_set_handler_client;
        PFN_wl_list_init                                    list_init;
        PFN_wl_list_insert                                  list_insert;
        PFN_wl_list_remove                                  list_remove;
        PFN_wl_list_length                                  list_length;
        PFN_wl_list_empty                                   list_empty;
        PFN_wl_list_insert_list                             list_insert_list;

        PFN_wl_cursor_theme_load                            cursor_theme_load;
        PFN_wl_cursor_theme_destroy                         cursor_theme_destroy;
        PFN_wl_cursor_theme_get_cursor                      cursor_theme_get_cursor;
        PFN_wl_cursor_image_get_buffer                      cursor_image_get_buffer;
        PFN_wl_cursor_frame                                 cursor_frame;
    } wl;

    struct {
        PFN_xkb_context_new                                 context_new;
        PFN_xkb_context_unref                               context_unref;
        PFN_xkb_keymap_new_from_string                      keymap_new_from_string;
        PFN_xkb_keymap_unref                                keymap_unref;
        PFN_xkb_keymap_mod_get_index                        keymap_mod_get_index;
        PFN_xkb_keymap_key_repeats                          keymap_key_repeats;
        PFN_xkb_keymap_key_for_each                         keymap_for_each;
        PFN_xkb_keymap_key_get_syms_by_level                keymap_key_get_syms_by_level;
        PFN_xkb_keymap_layout_get_name                      keymap_layout_get_name;
        PFN_xkb_keysym_to_utf8                              keysym_to_utf8;
        PFN_xkb_keysym_to_utf32                             keysym_to_utf32;
        PFN_xkb_state_new                                   state_new;
        PFN_xkb_state_unref                                 state_unref;
        PFN_xkb_state_key_get_syms                          state_key_get_syms;
        PFN_xkb_state_key_get_layout                        state_key_get_layout;
        PFN_xkb_state_mod_index_is_active                   state_mod_index_is_active;
        PFN_xkb_state_update_mask                           state_update_mask;
        PFN_xkb_compose_table_new_from_locale               compose_table_new_from_locale;
        PFN_xkb_compose_table_unref                         compose_table_unref;
        PFN_xkb_compose_state_new                           compose_state_new;
        PFN_xkb_compose_state_reset                         compose_state_reset;
        PFN_xkb_compose_state_feed                          compose_state_feed;
        PFN_xkb_compose_state_get_status                    compose_state_get_status;
        PFN_xkb_compose_state_get_one_sym                   compose_state_get_one_sym;
    } xkb;

    struct {
        PFN_libdecor_new                                    new;
        PFN_libdecor_unref                                  unref;
        PFN_libdecor_decorate                               decorate;
        PFN_libdecor_frame_unref                            frame_unref;
        PFN_libdecor_frame_set_title                        frame_set_title;
        PFN_libdecor_frame_set_app_id                       frame_set_app_id;
        PFN_libdecor_frame_set_max_content_size             frame_set_max_content_size;
        PFN_libdecor_frame_get_max_content_size             frame_get_max_content_size;
        PFN_libdecor_frame_set_min_content_size             frame_set_min_content_size;
        PFN_libdecor_frame_get_min_content_size             frame_get_min_content_size;
        PFN_libdecor_frame_resize                           frame_resize;
        PFN_libdecor_frame_move                             frame_move;
        PFN_libdecor_frame_commit                           frame_commit;
        PFN_libdecor_frame_set_minimized                    frame_set_minimized;
        PFN_libdecor_frame_set_maximized                    frame_set_maximized;
        PFN_libdecor_frame_unset_maximized                  frame_unset_maximized;
        PFN_libdecor_frame_set_fullscreen                   frame_set_fullscreen;
        PFN_libdecor_frame_unset_fullscreen                 frame_unset_fullscreen;
        PFN_libdecor_frame_set_capabilities                 frame_set_capabilities;
        PFN_libdecor_frame_unset_capabilities               frame_unset_capabilities;
        PFN_libdecor_frame_has_capability                   frame_has_capability;
        PFN_libdecor_frame_set_visibility                   frame_set_visibility;
        PFN_libdecor_frame_is_visible                       frame_is_visible;
        PFN_libdecor_frame_is_floating                      frame_is_floating;
        PFN_libdecor_frame_set_parent                       frame_set_parent;
        PFN_libdecor_frame_get_xdg_surface                  frame_get_xdg_surface;
        PFN_libdecor_frame_get_xdg_toplevel                 frame_get_xdg_toplevel;
        PFN_libdecor_frame_map                              frame_map;
        PFN_libdecor_state_new                              state_new;
        PFN_libdecor_state_free                             state_free;
        PFN_libdecor_configuration_get_content_size         configuration_get_content_size;
        PFN_libdecor_configuration_get_window_state         configuration_get_window_state;
        PFN_libdecor_dispatch                               dispatch;
    } libdecor;
#ifdef REZNOR_VULKAN
    struct {
        struct VkInstance_T                                    *instance;
        PFN_vkCreateWaylandSurfaceKHR                           vkCreateWaylandSurfaceKHR;
        PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR    vkGetPhysicalDeviceWaylandPresentationSupportKHR;
    } vulkan;
#endif /* REZNOR_VULKAN */
};

extern struct hadal *g_wayland;

/* Protocols are generated with wayland-scanner, their sources are included in
 * the project repository: wayland_protocols/<protocol>.xml. */
#include "wayland-protocol.h"
#include "cursor-shape-v1-protocol.h"
#include "fractional-scale-v1-protocol.h"
#include "idle-inhibit-unstable-v1-protocol.h"
#include "keyboard-shortcuts-inhibit-unstable-v1-protocol.h"
#include "pointer-constraints-unstable-v1-protocol.h"
#include "pointer-gestures-unstable-v1-protocol.h"
#include "primary-selection-unstable-v1-protocol.h"
#include "relative-pointer-unstable-v1-protocol.h"
#include "tablet-unstable-v2-protocol.h"
#include "text-input-unstable-v3-protocol.h"
#include "viewporter-protocol.h"
#include "xdg-activation-v1-protocol.h"
#include "xdg-decoration-unstable-v1-protocol.h"
#include "xdg-output-unstable-v1-protocol.h"
#include "xdg-shell-protocol.h"
#include "xdg-toplevel-icon-v1-protocol.h"
