#include <amwe/hadal.h>
#ifdef HADAL_WAYLAND

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h> /* strcmp */

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
typedef s32 (*PFN_wl_proxy_add_listener)(struct wl_proxy *, void (**)(void), void *);
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
#define xkb_keymap_key_for_each                             g_wayland->xkb.keymap_key_for_each
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
#define xkb_compose_state_unref                             g_wayland->xkb.compose_state_unref
#define xkb_compose_state_feed                              g_wayland->xkb.compose_state_feed
#define xkb_compose_state_get_status                        g_wayland->xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym                       g_wayland->xkb.compose_state_get_one_sym

#ifdef HADAL_WAYLAND_LIBDECOR
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
typedef s32 (*PFN_libdecor_get_fd)(struct libdecor *);
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
typedef bool (*PFN_libdecor_frame_has_capability)(struct libdecor_frame *, enum libdecor_capabilities);
typedef void (*PFN_libdecor_frame_set_visibility)(struct libdecor_frame *, bool);
typedef bool (*PFN_libdecor_frame_is_visible)(struct libdecor_frame *);
typedef bool (*PFN_libdecor_frame_is_floating)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_set_parent)(struct libdecor_frame *, struct libdecor_frame *);
typedef struct xdg_surface *(*PFN_libdecor_frame_get_xdg_surface)(struct libdecor_frame *);
typedef struct xdg_toplevel *(*PFN_libdecor_frame_get_xdg_toplevel)(struct libdecor_frame *);
typedef void (*PFN_libdecor_frame_map)(struct libdecor_frame *);
typedef struct libdecor_state *(*PFN_libdecor_state_new)(s32, s32);
typedef void (*PFN_libdecor_state_free)(struct libdecor_state *);
typedef bool (*PFN_libdecor_configuration_get_content_size)(struct libdecor_configuration *, struct libdecor_frame *, s32 *, s32 *);
typedef bool (*PFN_libdecor_configuration_get_window_state)(struct libdecor_configuration *, enum libdecor_window_state *);
typedef s32 (*PFN_libdecor_dispatch)(struct libdecor *, s32);
#define libdecor_new                                        g_wayland->libdecor.new
#define libdecor_unref                                      g_wayland->libdecor.unref
#define libdecor_get_fd                                     g_wayland->libdecor.get_fd
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
#endif /* HADAL_WAYLAND_LIBDECOR */

#ifdef XAKU_VULKAN
typedef u32 VkBool32;
typedef u32 VkFlags;

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
#endif /* XAKU_VULKAN */

struct wayland_cursor_theme {
    struct wl_cursor_theme             *theme;
    usize                               size;
};

struct hadal_window {
    HADAL_INTERFACE_WINDOW_HEADER
    atomic_u32                          flags;
    const char                         *tag;
    struct wl_surface                  *surface;

    union {
#ifdef HADAL_WAYLAND_LIBDECOR
        struct {
            struct libdecor_frame          *frame;
            bool                            initial_config_seen;
        } libdecor;
#endif /* HADAL_WAYLAND_LIBDECOR */
        struct {
            struct xdg_surface             *surface;
            union {
                struct xdg_toplevel        *toplevel;
                struct {
                    struct xdg_popup       *popup;
                    struct xdg_positioner  *positioner;
                    u32                     parent_id;
                    struct hadal_window    *child;
                } popup;
            } roleobj;
            bool                            initial_config_seen;
        } xdg;
    } shell_surface;

    enum {
        WAYLAND_SURFACE_UNKNOWN = 0,
        WAYLAND_SURFACE_XDG_TOPLEVEL,
        WAYLAND_SURFACE_XDG_POPUP,
#ifdef HADAL_WAYLAND_LIBDECOR
        WAYLAND_SURFACE_LIBDECOR
#endif /* HADAL_WAYLAND_LIBDECOR */
    } shell_surface_type;
};

struct hadal_monitor {
    HADAL_INTERFACE_MONITOR_HEADER
    struct wl_output                   *output;
    struct zxdg_output_v1              *xdg_output;
};

struct hadal_encore {
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
#ifdef HADAL_WAYLAND_LIBDECOR
        struct libdecor                                    *libdecor;
#endif /* HADAL_WAYLAND_LIBDECOR */
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
        PFN_xkb_keymap_key_for_each                         keymap_key_for_each;
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
        PFN_xkb_compose_state_unref                         compose_state_unref;
        PFN_xkb_compose_state_feed                          compose_state_feed;
        PFN_xkb_compose_state_get_status                    compose_state_get_status;
        PFN_xkb_compose_state_get_one_sym                   compose_state_get_one_sym;
    } xkb;

#ifdef HADAL_WAYLAND_LIBDECOR
    void                                                   *module_libdecor;
    struct {
        PFN_libdecor_new                                    new;
        PFN_libdecor_unref                                  unref;
        PFN_libdecor_get_fd                                 get_fd;
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
#endif /* HADAL_WAYLAND_LIBDECOR */

#ifdef XAKU_VULKAN
    struct {
        struct VkInstance_T                                    *instance;
        PFN_vkCreateWaylandSurfaceKHR                           vkCreateWaylandSurfaceKHR;
        PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR    vkGetPhysicalDeviceWaylandPresentationSupportKHR;
    } vulkan;
#endif /* XAKU_VULKAN */
};

static struct hadal_encore *g_wayland = NULL;

/* Protocols are generated with wayland-scanner, their sources are included in
 * the project repository: wayland_protocols/<protocol>.xml. */
#include "wayland-protocol.h"
#include "alpha-modifier-v1-protocol.h"
#include "cursor-shape-v1-protocol.h"
#include "color-management-v1-protocol.h"
#include "fractional-scale-v1-protocol.h"
#include "frog-color-management-v1-protocol.h"
#include "idle-inhibit-unstable-v1-protocol.h"
#include "input-timestamps-unstable-v1-protocol.h"
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
#include "xdg-dialog-v1-protocol.h"
#include "xdg-foreign-unstable-v2-protocol.h"
#include "xdg-output-unstable-v1-protocol.h"
#include "xdg-shell-protocol.h"
#include "xdg-toplevel-icon-v1-protocol.h"

#if 0 // TODO
static const char *g_wayland_monitor_tag = "hadal_monitor";
static const char *g_wayland_surface_tag = "hadal_window";

static void wayland_register_surface(struct wl_surface *surface)
{
    wl_proxy_set_tag((struct wl_proxy *)surface, &g_wayland_surface_tag);
}

static void wayland_register_output(struct wl_output *output) 
{
    wl_proxy_set_tag((struct wl_proxy *)output, &g_wayland_monitor_tag);
}

static bool wayland_own_surface(struct wl_surface *surface)
{
    return wl_proxy_get_tag((struct wl_proxy *)surface) == &g_wayland_surface_tag;
}

static bool wayland_own_output(struct wl_output *output)
{
    return wl_proxy_get_tag((struct wl_proxy *)output) == &g_wayland_monitor_tag;
}

static void resize_framebuffer(struct hadal_window *window)
{
    (void)window;
    /* TODO framebuffer scaling */
    //lake_atomic_store_explicit(&window->fb_width, window->window_width, lake_memory_model_release);
    //lake_atomic_store_explicit(&window->fb_height, window->window_height, lake_memory_model_release);

    /* TODO swapchain callback */
}

static bool resize_window(struct hadal_window *window, s32 width, s32 height)
{
    width = lake_max(width, 1);
    height = lake_max(height, 1);

    if ((u32)width == window->window_width && (u32)height == window->window_height)
        return false;

    window->window_width = width;
    window->window_height = height;

    resize_framebuffer(window);

    return true;
}

static void handle_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *output)
{
    /* unused */
    (void)surface;
    (void)data; // window

    if (!wayland_own_output(output))
        return;
}

static void handle_surface_leave(
    void              *data,
    struct wl_surface *surface,
    struct wl_output  *output)
{
    /* unused */
    (void)surface;
    (void)data; // window

    if (!wayland_own_output(output))
        return;
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
};
#endif

#ifdef HADAL_WAYLAND_LIBDECOR
static bool create_window_shell_libdecor_frame(struct hadal_window *window)
{
    (void)window;
    return false;
}
#endif /* HADAL_WAYLAND_LIBDECOR */

static void handle_xdg_surface_configure(
    void               *data,
    struct xdg_surface *xdg_surface,
    u32                 serial)
{
    struct hadal_window *window = (struct hadal_window *)data;
      
    xdg_surface_ack_configure(xdg_surface, serial);

    /* TODO */
    (void)window;
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void handle_xdg_toplevel_close(
    void                *data,
    struct xdg_toplevel *xdg_toplevel)
{
    /* unused */
    (void)xdg_toplevel;

    struct hadal_window *window = (struct hadal_window *)data;
    lake_atomic_or_explicit(&window->flags, hadal_window_flag_should_close, lake_memory_model_release);
}

static void handle_xdg_toplevel_configure(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height,
    struct wl_array     *states)
{
    struct hadal_window *window = (struct hadal_window *)data;
    u32 *state;

    u32 flags = 0u;

    wl_array_for_each(state, states)
    {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            flags |= hadal_window_flag_maximized;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            flags |= hadal_window_flag_fullscreen;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            flags |= hadal_window_flag_shell_activated;
            break;
        }
    }

    /* TODO */
    (void)xdg_toplevel;
    (void)width;
    (void)height;
    (void)flags;
    (void)window;
}

static void handle_xdg_toplevel_configure_bounds(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height)
{
    (void)data;
    (void)xdg_toplevel;
    (void)width;
    (void)height;
}

static void handle_xdg_toplevel_wm_capabilities(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    struct wl_array     *array)
{
    (void)data;
    (void)xdg_toplevel;
    (void)array;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .close = handle_xdg_toplevel_close,
    .configure = handle_xdg_toplevel_configure,
    .configure_bounds = handle_xdg_toplevel_configure_bounds,
    .wm_capabilities = handle_xdg_toplevel_wm_capabilities,
};

static bool create_window_shell_xdg_toplevel(struct hadal_window *window)
{
    struct hadal_encore *hadal = window->hadal;
    bedrock_assert_debug(hadal->shell.xdg);

    window->shell_surface.xdg.surface = xdg_wm_base_get_xdg_surface(hadal->shell.xdg, window->surface);
    if (!window->shell_surface.xdg.surface) {
        bedrock_log_error("Wayland: failed to create xdg-surface for window of title '%s'.", window->title);
        return false;
    }
    xdg_surface_add_listener(window->shell_surface.xdg.surface, &xdg_surface_listener, window);

    window->shell_surface.xdg.roleobj.toplevel = xdg_surface_get_toplevel(window->shell_surface.xdg.surface);
    if (!window->shell_surface.xdg.roleobj.toplevel) {
        bedrock_log_error("Wayland: failed to create xdg-toplevel for window of title '%s'.", window->title);
        return false;
    }
    xdg_toplevel_add_listener(window->shell_surface.xdg.roleobj.toplevel, &xdg_toplevel_listener, window);

    xdg_toplevel_set_title(window->shell_surface.xdg.roleobj.toplevel, window->title);

    wl_surface_commit(window->surface);
    wl_display_roundtrip(hadal->display);
    return true;
}

static bool create_window_shell_xdg_popup(struct hadal_window *window)
{
    /* TODO */
    (void)window;
    return false;
}

static bool create_window_shell_objects(struct hadal_window *window)
{
    if (window->shell_surface.xdg.roleobj.toplevel || 
#ifdef HADAL_WAYLAND_LIBDECOR
        window->shell_surface.libdecor.frame || 
#endif /* HADAL_WAYLAND_LIBDECOR */
        window->shell_surface.xdg.roleobj.popup.popup)
        return true;

    switch (window->shell_surface_type) {
#ifdef HADAL_WAYLAND_LIBDECOR
    case WAYLAND_SURFACE_LIBDECOR: 
        return create_window_shell_libdecor_frame(window);
#endif /* HADAL_WAYLAND_LIBDECOR */
    case WAYLAND_SURFACE_XDG_TOPLEVEL: 
        return create_window_shell_xdg_toplevel(window);
    case WAYLAND_SURFACE_XDG_POPUP: 
        return create_window_shell_xdg_popup(window);
    case WAYLAND_SURFACE_UNKNOWN: 
        return false;
    }
    lake_unreachable;
}

static void destroy_window_shell_objects(struct hadal_window *window)
{

#ifdef HADAL_WAYLAND_LIBDECOR
    if (window->shell_surface_type == WAYLAND_SURFACE_LIBDECOR && window->shell_surface.libdecor.frame) {
        libdecor_frame_unref(window->shell_surface.libdecor.frame);
        window->shell_surface.libdecor.frame = NULL;
    }
#endif /* HADAL_WAYLAND_LIBDECOR */

    if (window->shell_surface_type != WAYLAND_SURFACE_UNKNOWN) {
        if (window->shell_surface_type == WAYLAND_SURFACE_XDG_TOPLEVEL && window->shell_surface.xdg.roleobj.toplevel) {
            xdg_toplevel_destroy(window->shell_surface.xdg.roleobj.toplevel);
            window->shell_surface.xdg.roleobj.toplevel = NULL;
        } else if (window->shell_surface.xdg.roleobj.popup.popup) {
            xdg_popup_destroy(window->shell_surface.xdg.roleobj.popup.popup);
            window->shell_surface.xdg.roleobj.popup.popup = NULL;
            window->shell_surface.xdg.roleobj.popup.positioner = NULL;
            window->shell_surface.xdg.roleobj.popup.child = NULL;
            window->shell_surface.xdg.roleobj.popup.parent_id = 0u;
        }

        if (window->shell_surface.xdg.surface)
            xdg_surface_destroy(window->shell_surface.xdg.surface);
        window->shell_surface.xdg.surface = NULL;
    }
}

static FN_HADAL_WINDOW_ASSEMBLY(wayland)
{
    (void)hadal;
    (void)assembly;
    (void)out_window;
    return lake_result_max_enum;
}

static FN_HADAL_WINDOW_DISASSEMBLY(wayland)
{
    destroy_window_shell_objects(window);

    if (window->surface)
        wl_surface_destroy(window->surface);

    bedrock_zerop(window);
}

static FN_HADAL_WINDOW_VISIBILITY(wayland)
{
    u32 flags = lake_atomic_read_explicit(&window->flags, lake_memory_model_relaxed);

    if ((flags & hadal_window_flag_visible) != (visible ? hadal_window_flag_visible : 0)) {
        if (visible && !create_window_shell_objects(window)) {
            /* an error occured when creating the shell objects */
            return flags;
        } else {
            destroy_window_shell_objects(window);
            wl_surface_attach(window->surface, NULL, 0, 0);
            wl_surface_commit(window->surface);
        }
        return lake_atomic_xor_explicit(&window->flags, hadal_window_flag_visible, lake_memory_model_release);
    }
    /* no modify/write operation was done */
    return flags;
}

#ifdef XAKU_VULKAN
#include <amwe/xaku.h>

static FN_HADAL_VULKAN_WRITE_INSTANCE(wayland)
{
    if (!hadal|| !instance || !vkGetInstanceProcAddr)
        return false;

    hadal->vulkan.instance = instance;
    hadal->vulkan.vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)
        (void *)vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR");
    hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)
        (void *)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    if (!hadal->vulkan.vkCreateWaylandSurfaceKHR || !hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR)
    {
        hadal->vulkan.instance = NULL;
        hadal->vulkan.vkCreateWaylandSurfaceKHR = NULL;
        hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = NULL;
        return false;
    }
    return true;
}

static FN_HADAL_VULKAN_PRESENTATION_SUPPORT(wayland)
{
    bedrock_assert_debug(hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR);
    return (bool)hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, queue_family_index, hadal->display);
}

static FN_HADAL_VULKAN_SURFACE_CREATE(wayland)
{
    bedrock_assert_debug(lake_atomic_read_explicit(&window->flags, lake_memory_model_relaxed) 
        & (hadal_window_flag_vulkan | hadal_window_flag_is_valid));

    struct hadal_encore *hadal = window->hadal;
    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = hadal->display,
        .surface = window->surface,
    };
    s32 result = hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.instance, &surface_info, callbacks, out_surface);

    if (result == -1)
        return lake_result_error_out_of_host_memory;
    else if (result == -2)
        return lake_result_error_out_of_device_memory;
    return lake_result_success;
}
#endif /* XAKU_VULKAN */

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
    struct hadal_encore *hadal = (struct hadal_encore *)data;

    if (!strcmp(interface, "wl_compositor")) {
        hadal->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, lake_min(3, version));

    } else if (!strcmp(interface, "wl_subcompositor")) {
        hadal->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);

    } else if (!strcmp(interface, "wl_shm")) {
        hadal->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);

    } else if (!strcmp(interface, "wl_output")) {
        bedrock_log_debug("Add wayland output: %u ver. %u", name, version);

    } else if (!strcmp(interface, "wl_seat")) {
        if (!hadal->seat) {
            hadal->seat = wl_registry_bind(registry, name, &wl_seat_interface, lake_min(4, version));
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

static bool load_symbols(struct hadal_encore *hadal, const char *name)
{
    /* should be already loaded */
    bedrock_assert_debug(hadal->wl.display_connect);

    void *module_client = hadal->module_client;
    void *module_cursor = hadal->module_cursor;
    void *module_xkbcommon = hadal->module_xkbcommon;

    hadal->wl.proxy_marshal = (PFN_wl_proxy_marshal)
        bedrock_get_proc_address(module_client, "wl_proxy_marshal");
    hadal->wl.proxy_create = (PFN_wl_proxy_create)
        bedrock_get_proc_address(module_client, "wl_proxy_create");
    hadal->wl.proxy_destroy = (PFN_wl_proxy_destroy)
        bedrock_get_proc_address(module_client, "wl_proxy_destroy");
    hadal->wl.proxy_add_listener = (PFN_wl_proxy_add_listener)
        bedrock_get_proc_address(module_client, "wl_proxy_add_listener");
    hadal->wl.proxy_set_user_data = (PFN_wl_proxy_set_user_data)
        bedrock_get_proc_address(module_client, "wl_proxy_set_user_data");
    hadal->wl.proxy_get_user_data = (PFN_wl_proxy_get_user_data)
        bedrock_get_proc_address(module_client, "wl_proxy_get_user_data");
    hadal->wl.proxy_get_version = (PFN_wl_proxy_get_version)
        bedrock_get_proc_address(module_client, "wl_proxy_get_version");
    hadal->wl.proxy_get_id = (PFN_wl_proxy_get_id)
        bedrock_get_proc_address(module_client, "wl_proxy_get_id");
    hadal->wl.proxy_get_class = (PFN_wl_proxy_get_class)
        bedrock_get_proc_address(module_client, "wl_proxy_get_class");
    hadal->wl.proxy_set_queue = (PFN_wl_proxy_set_queue)
        bedrock_get_proc_address(module_client, "wl_proxy_set_queue");
    hadal->wl.proxy_create_wrapper = (PFN_wl_proxy_create_wrapper)
        bedrock_get_proc_address(module_client, "wl_proxy_create_wrapper");
    hadal->wl.proxy_wrapper_destroy = (PFN_wl_proxy_wrapper_destroy)
        bedrock_get_proc_address(module_client, "wl_proxy_wrapper_destroy");
    hadal->wl.proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)
        bedrock_get_proc_address(module_client, "wl_proxy_marshal_constructor");
    hadal->wl.proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)
        bedrock_get_proc_address(module_client, "wl_proxy_marshal_constructor_versioned");
    hadal->wl.proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)
        bedrock_get_proc_address(module_client, "wl_proxy_marshal_flags");
    hadal->wl.proxy_marshal_array_flags = (PFN_wl_proxy_marshal_array_flags)
        bedrock_get_proc_address(module_client, "wl_proxy_marshal_array_flags");
    hadal->wl.proxy_set_tag = (PFN_wl_proxy_set_tag)
        bedrock_get_proc_address(module_client, "wl_proxy_set_tag");
    hadal->wl.proxy_get_tag = (PFN_wl_proxy_get_tag)
        bedrock_get_proc_address(module_client, "wl_proxy_get_tag");
    hadal->wl.display_connect_to_fd = (PFN_wl_display_connect_to_fd)
        bedrock_get_proc_address(module_client, "wl_display_connect_to_fd");
    hadal->wl.display_disconnect = (PFN_wl_display_disconnect)
        bedrock_get_proc_address(module_client, "wl_display_disconnect");
    hadal->wl.display_get_fd = (PFN_wl_display_get_fd)
        bedrock_get_proc_address(module_client, "wl_display_get_fd");
    hadal->wl.display_dispatch = (PFN_wl_display_dispatch)
        bedrock_get_proc_address(module_client, "wl_display_dispatch");
    hadal->wl.display_dispatch_queue = (PFN_wl_display_dispatch_queue)
        bedrock_get_proc_address(module_client, "wl_display_dispatch_queue");
    hadal->wl.display_dispatch_queue_pending = (PFN_wl_display_dispatch_queue_pending)
        bedrock_get_proc_address(module_client, "wl_display_dispatch_queue_pending");
    hadal->wl.display_dispatch_pending = (PFN_wl_display_dispatch_pending)
        bedrock_get_proc_address(module_client, "wl_display_dispatch_pending");
    hadal->wl.display_prepare_read = (PFN_wl_display_prepare_read)
        bedrock_get_proc_address(module_client, "wl_display_prepare_read");
    hadal->wl.display_prepare_read_queue = (PFN_wl_display_prepare_read_queue)
        bedrock_get_proc_address(module_client, "wl_display_prepare_read_queue");
    hadal->wl.display_read_events = (PFN_wl_display_read_events)
        bedrock_get_proc_address(module_client, "wl_display_read_events");
    hadal->wl.display_cancel_read = (PFN_wl_display_cancel_read)
        bedrock_get_proc_address(module_client, "wl_display_cancel_read");
    hadal->wl.display_get_error = (PFN_wl_display_get_error)
        bedrock_get_proc_address(module_client, "wl_display_get_error");
    hadal->wl.display_flush = (PFN_wl_display_flush)
        bedrock_get_proc_address(module_client, "wl_display_flush");
    hadal->wl.display_roundtrip = (PFN_wl_display_roundtrip)
        bedrock_get_proc_address(module_client, "wl_display_roundtrip");
    hadal->wl.display_create_queue = (PFN_wl_display_create_queue)
        bedrock_get_proc_address(module_client, "wl_display_create_queue");
    hadal->wl.event_queue_destroy = (PFN_wl_event_queue_destroy)
        bedrock_get_proc_address(module_client, "wl_event_queue_destroy");
    hadal->wl.log_set_handler_client = (PFN_wl_log_set_handler_client)
        bedrock_get_proc_address(module_client, "wl_log_set_handler_client");
    hadal->wl.list_init = (PFN_wl_list_init)
        bedrock_get_proc_address(module_client, "wl_list_init");
    hadal->wl.list_insert = (PFN_wl_list_insert)
        bedrock_get_proc_address(module_client, "wl_list_insert");
    hadal->wl.list_remove = (PFN_wl_list_remove)
        bedrock_get_proc_address(module_client, "wl_list_remove");
    hadal->wl.list_length = (PFN_wl_list_length)
        bedrock_get_proc_address(module_client, "wl_list_length");
    hadal->wl.list_empty = (PFN_wl_list_empty)
        bedrock_get_proc_address(module_client, "wl_list_empty");
    hadal->wl.list_insert_list = (PFN_wl_list_insert_list)
        bedrock_get_proc_address(module_client, "wl_list_insert_list");

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
        bedrock_log_error("'%s' can't load libwayland-client procedures, can't continue.", name);
        return false;
    }

    hadal->wl.cursor_theme_load = (PFN_wl_cursor_theme_load)
        bedrock_get_proc_address(module_cursor, "wl_cursor_theme_load");
    hadal->wl.cursor_theme_destroy = (PFN_wl_cursor_theme_destroy)
        bedrock_get_proc_address(module_cursor, "wl_cursor_theme_destroy");
    hadal->wl.cursor_theme_get_cursor = (PFN_wl_cursor_theme_get_cursor)
        bedrock_get_proc_address(module_cursor, "wl_cursor_theme_get_cursor");
    hadal->wl.cursor_image_get_buffer = (PFN_wl_cursor_image_get_buffer)
        bedrock_get_proc_address(module_cursor, "wl_cursor_image_get_buffer");
    hadal->wl.cursor_frame = (PFN_wl_cursor_frame)
        bedrock_get_proc_address(module_cursor, "wl_cursor_frame");

    if (!hadal->wl.cursor_theme_load ||
        !hadal->wl.cursor_theme_destroy ||
        !hadal->wl.cursor_theme_get_cursor ||
        !hadal->wl.cursor_image_get_buffer ||
        !hadal->wl.cursor_frame)
    {
        bedrock_log_error("'%s' can't load libwayland-cursor procedures, can't continue.", name);
        return false;
    }

    hadal->xkb.context_new = (PFN_xkb_context_new)
        bedrock_get_proc_address(module_xkbcommon, "xkb_context_new");
    hadal->xkb.context_unref = (PFN_xkb_context_unref)
        bedrock_get_proc_address(module_xkbcommon, "xkb_context_unref");
    hadal->xkb.keymap_new_from_string = (PFN_xkb_keymap_new_from_string)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_new_from_string");
    hadal->xkb.keymap_unref = (PFN_xkb_keymap_unref)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_unref");
    hadal->xkb.keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_mod_get_index");
    hadal->xkb.keymap_key_repeats = (PFN_xkb_keymap_key_repeats)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_key_repeats");
    hadal->xkb.keymap_key_for_each = (PFN_xkb_keymap_key_for_each)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_key_for_each");
    hadal->xkb.keymap_key_get_syms_by_level = (PFN_xkb_keymap_key_get_syms_by_level)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_key_get_syms_by_level");
    hadal->xkb.keymap_layout_get_name = (PFN_xkb_keymap_layout_get_name)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keymap_layout_get_name");
    hadal->xkb.keysym_to_utf8 = (PFN_xkb_keysym_to_utf8)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keysym_to_utf8");
    hadal->xkb.keysym_to_utf32 = (PFN_xkb_keysym_to_utf32)
        bedrock_get_proc_address(module_xkbcommon, "xkb_keysym_to_utf32");
    hadal->xkb.state_new = (PFN_xkb_state_new)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_new");
    hadal->xkb.state_unref = (PFN_xkb_state_unref)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_unref");
    hadal->xkb.state_key_get_syms = (PFN_xkb_state_key_get_syms)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_key_get_syms");
    hadal->xkb.state_key_get_layout = (PFN_xkb_state_key_get_layout)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_key_get_layout");
    hadal->xkb.state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_mod_index_is_active");
    hadal->xkb.state_update_mask = (PFN_xkb_state_update_mask)
        bedrock_get_proc_address(module_xkbcommon, "xkb_state_update_mask");
    hadal->xkb.compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_table_new_from_locale");
    hadal->xkb.compose_table_unref = (PFN_xkb_compose_table_unref)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_table_unref");
    hadal->xkb.compose_state_new = (PFN_xkb_compose_state_new)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_state_new");
    hadal->xkb.compose_state_reset = (PFN_xkb_compose_state_reset)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_state_reset");
    hadal->xkb.compose_state_feed = (PFN_xkb_compose_state_feed)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_state_feed");
    hadal->xkb.compose_state_get_status = (PFN_xkb_compose_state_get_status)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_state_get_status");
    hadal->xkb.compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)
        bedrock_get_proc_address(module_xkbcommon, "xkb_compose_state_get_one_sym");

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
        bedrock_log_error("'%s' can't load libxkbcommon procedures, can't continue.", name);
        return false;
    }

    /* optional */
#ifdef HADAL_WAYLAND_LIBDECOR
    void *module_libdecor = hadal->module_libdecor;

    if (!module_libdecor) return true;

    hadal->libdecor.new = (PFN_libdecor_new)
        bedrock_get_proc_address(module_libdecor, "libdecor_new");
    hadal->libdecor.unref = (PFN_libdecor_unref)
        bedrock_get_proc_address(module_libdecor, "libdecor_unref");
    hadal->libdecor.get_fd = (PFN_libdecor_get_fd)
        bedrock_get_proc_address(module_libdecor, "libdecor_get_fd");
    hadal->libdecor.decorate = (PFN_libdecor_decorate)
        bedrock_get_proc_address(module_libdecor, "libdecor_decorate");
    hadal->libdecor.frame_unref = (PFN_libdecor_frame_unref)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_unref");
    hadal->libdecor.frame_set_title = (PFN_libdecor_frame_set_title)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_title");
    hadal->libdecor.frame_set_app_id = (PFN_libdecor_frame_set_app_id)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_app_id");
    hadal->libdecor.frame_set_max_content_size = (PFN_libdecor_frame_set_max_content_size)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_max_content_size");
    hadal->libdecor.frame_get_max_content_size = (PFN_libdecor_frame_get_max_content_size)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_get_max_content_size");
    hadal->libdecor.frame_set_min_content_size = (PFN_libdecor_frame_set_min_content_size)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_min_content_size");
    hadal->libdecor.frame_get_min_content_size = (PFN_libdecor_frame_get_min_content_size)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_get_min_content_size");
    hadal->libdecor.frame_resize = (PFN_libdecor_frame_resize)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_resize");
    hadal->libdecor.frame_move = (PFN_libdecor_frame_move)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_move");
    hadal->libdecor.frame_commit = (PFN_libdecor_frame_commit)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_commit");
    hadal->libdecor.frame_set_minimized = (PFN_libdecor_frame_set_minimized)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_minimized");
    hadal->libdecor.frame_set_maximized = (PFN_libdecor_frame_set_maximized)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_maximized");
    hadal->libdecor.frame_unset_maximized = (PFN_libdecor_frame_unset_maximized)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_unset_maximized");
    hadal->libdecor.frame_set_fullscreen = (PFN_libdecor_frame_set_fullscreen)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_fullscreen");
    hadal->libdecor.frame_unset_fullscreen = (PFN_libdecor_frame_unset_fullscreen)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_unset_fullscreen");
    hadal->libdecor.frame_set_capabilities = (PFN_libdecor_frame_set_capabilities)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_capabilities");
    hadal->libdecor.frame_unset_capabilities = (PFN_libdecor_frame_unset_capabilities)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_unset_capabilities");
    hadal->libdecor.frame_has_capability = (PFN_libdecor_frame_has_capability)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_has_capability");
    hadal->libdecor.frame_set_visibility = (PFN_libdecor_frame_set_visibility)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_visibility");
    hadal->libdecor.frame_is_visible = (PFN_libdecor_frame_is_visible)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_is_visible");
    hadal->libdecor.frame_is_floating = (PFN_libdecor_frame_is_floating)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_is_floating");
    hadal->libdecor.frame_set_parent = (PFN_libdecor_frame_set_parent)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_set_parent");
    hadal->libdecor.frame_get_xdg_surface = (PFN_libdecor_frame_get_xdg_surface)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_get_xdg_surface");
    hadal->libdecor.frame_get_xdg_toplevel = (PFN_libdecor_frame_get_xdg_toplevel)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_get_xdg_toplevel");
    hadal->libdecor.frame_map = (PFN_libdecor_frame_map)
        bedrock_get_proc_address(module_libdecor, "libdecor_frame_map");
    hadal->libdecor.state_new = (PFN_libdecor_state_new)
        bedrock_get_proc_address(module_libdecor, "libdecor_state_new");
    hadal->libdecor.state_free = (PFN_libdecor_state_free)
        bedrock_get_proc_address(module_libdecor, "libdecor_state_free");
    hadal->libdecor.configuration_get_content_size = (PFN_libdecor_configuration_get_content_size)
        bedrock_get_proc_address(module_libdecor, "libdecor_configuration_get_content_size");
    hadal->libdecor.configuration_get_window_state = (PFN_libdecor_configuration_get_window_state)
        bedrock_get_proc_address(module_libdecor, "libdecor_configuration_get_window_state");
    hadal->libdecor.dispatch = (PFN_libdecor_dispatch)
        bedrock_get_proc_address(module_libdecor, "libdecor_dispatch");

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
        bedrock_log_error("'%s' can't load libdecor procedures, this functionality will be disabled.", name);
        bedrock_module_close(module_libdecor);
        hadal->module_libdecor = NULL;
    }
#endif /* HADAL_WAYLAND_LIBDECOR */
    return true;
}

static void create_key_tables(struct hadal_encore *hadal)
{
    bedrock_memset(hadal->interface.keycodes, -1, lake_sizeof(hadal->interface.keycodes));
    bedrock_memset(hadal->interface.scancodes, -1, lake_sizeof(hadal->interface.scancodes));

    hadal->interface.keycodes[KEY_GRAVE]      = hadal_keycode_grave_accent;
    hadal->interface.keycodes[KEY_1]          = hadal_keycode_1;
    hadal->interface.keycodes[KEY_2]          = hadal_keycode_2;
    hadal->interface.keycodes[KEY_3]          = hadal_keycode_3;
    hadal->interface.keycodes[KEY_4]          = hadal_keycode_4;
    hadal->interface.keycodes[KEY_5]          = hadal_keycode_5;
    hadal->interface.keycodes[KEY_6]          = hadal_keycode_6;
    hadal->interface.keycodes[KEY_7]          = hadal_keycode_7;
    hadal->interface.keycodes[KEY_8]          = hadal_keycode_8;
    hadal->interface.keycodes[KEY_9]          = hadal_keycode_9;
    hadal->interface.keycodes[KEY_0]          = hadal_keycode_0;
    hadal->interface.keycodes[KEY_SPACE]      = hadal_keycode_space;
    hadal->interface.keycodes[KEY_MINUS]      = hadal_keycode_minus;
    hadal->interface.keycodes[KEY_EQUAL]      = hadal_keycode_equal;
    hadal->interface.keycodes[KEY_Q]          = hadal_keycode_q;
    hadal->interface.keycodes[KEY_W]          = hadal_keycode_w;
    hadal->interface.keycodes[KEY_E]          = hadal_keycode_e;
    hadal->interface.keycodes[KEY_R]          = hadal_keycode_r;
    hadal->interface.keycodes[KEY_T]          = hadal_keycode_t;
    hadal->interface.keycodes[KEY_Y]          = hadal_keycode_y;
    hadal->interface.keycodes[KEY_U]          = hadal_keycode_u;
    hadal->interface.keycodes[KEY_I]          = hadal_keycode_i;
    hadal->interface.keycodes[KEY_O]          = hadal_keycode_o;
    hadal->interface.keycodes[KEY_P]          = hadal_keycode_p;
    hadal->interface.keycodes[KEY_LEFTBRACE]  = hadal_keycode_left_bracket;
    hadal->interface.keycodes[KEY_RIGHTBRACE] = hadal_keycode_right_bracket;
    hadal->interface.keycodes[KEY_A]          = hadal_keycode_a;
    hadal->interface.keycodes[KEY_S]          = hadal_keycode_s;
    hadal->interface.keycodes[KEY_D]          = hadal_keycode_d;
    hadal->interface.keycodes[KEY_F]          = hadal_keycode_f;
    hadal->interface.keycodes[KEY_G]          = hadal_keycode_g;
    hadal->interface.keycodes[KEY_H]          = hadal_keycode_h;
    hadal->interface.keycodes[KEY_J]          = hadal_keycode_j;
    hadal->interface.keycodes[KEY_K]          = hadal_keycode_k;
    hadal->interface.keycodes[KEY_L]          = hadal_keycode_l;
    hadal->interface.keycodes[KEY_SEMICOLON]  = hadal_keycode_semicolon;
    hadal->interface.keycodes[KEY_APOSTROPHE] = hadal_keycode_apostrophe;
    hadal->interface.keycodes[KEY_Z]          = hadal_keycode_z;
    hadal->interface.keycodes[KEY_X]          = hadal_keycode_x;
    hadal->interface.keycodes[KEY_C]          = hadal_keycode_c;
    hadal->interface.keycodes[KEY_V]          = hadal_keycode_v;
    hadal->interface.keycodes[KEY_B]          = hadal_keycode_b;
    hadal->interface.keycodes[KEY_N]          = hadal_keycode_n;
    hadal->interface.keycodes[KEY_M]          = hadal_keycode_m;
    hadal->interface.keycodes[KEY_COMMA]      = hadal_keycode_comma;
    hadal->interface.keycodes[KEY_DOT]        = hadal_keycode_period;
    hadal->interface.keycodes[KEY_SLASH]      = hadal_keycode_slash;
    hadal->interface.keycodes[KEY_BACKSLASH]  = hadal_keycode_backslash;
    hadal->interface.keycodes[KEY_ESC]        = hadal_keycode_escape;
    hadal->interface.keycodes[KEY_TAB]        = hadal_keycode_tab;
    hadal->interface.keycodes[KEY_LEFTSHIFT]  = hadal_keycode_left_shift;
    hadal->interface.keycodes[KEY_RIGHTSHIFT] = hadal_keycode_right_shift;
    hadal->interface.keycodes[KEY_LEFTCTRL]   = hadal_keycode_left_control;
    hadal->interface.keycodes[KEY_RIGHTCTRL]  = hadal_keycode_right_control;
    hadal->interface.keycodes[KEY_LEFTALT]    = hadal_keycode_left_alt;
    hadal->interface.keycodes[KEY_RIGHTALT]   = hadal_keycode_right_alt;
    hadal->interface.keycodes[KEY_LEFTMETA]   = hadal_keycode_left_super;
    hadal->interface.keycodes[KEY_RIGHTMETA]  = hadal_keycode_right_super;
    hadal->interface.keycodes[KEY_COMPOSE]    = hadal_keycode_menu;
    hadal->interface.keycodes[KEY_NUMLOCK]    = hadal_keycode_num_lock;
    hadal->interface.keycodes[KEY_CAPSLOCK]   = hadal_keycode_caps_lock;
    hadal->interface.keycodes[KEY_PRINT]      = hadal_keycode_print_screen;
    hadal->interface.keycodes[KEY_SCROLLLOCK] = hadal_keycode_scroll_lock;
    hadal->interface.keycodes[KEY_PAUSE]      = hadal_keycode_pause;
    hadal->interface.keycodes[KEY_DELETE]     = hadal_keycode_delete;
    hadal->interface.keycodes[KEY_BACKSPACE]  = hadal_keycode_backspace;
    hadal->interface.keycodes[KEY_ENTER]      = hadal_keycode_enter;
    hadal->interface.keycodes[KEY_HOME]       = hadal_keycode_home;
    hadal->interface.keycodes[KEY_END]        = hadal_keycode_end;
    hadal->interface.keycodes[KEY_PAGEUP]     = hadal_keycode_page_up;
    hadal->interface.keycodes[KEY_PAGEDOWN]   = hadal_keycode_page_down;
    hadal->interface.keycodes[KEY_INSERT]     = hadal_keycode_insert;
    hadal->interface.keycodes[KEY_LEFT]       = hadal_keycode_left;
    hadal->interface.keycodes[KEY_RIGHT]      = hadal_keycode_right;
    hadal->interface.keycodes[KEY_DOWN]       = hadal_keycode_down;
    hadal->interface.keycodes[KEY_UP]         = hadal_keycode_up;
    hadal->interface.keycodes[KEY_F1]         = hadal_keycode_f1;
    hadal->interface.keycodes[KEY_F2]         = hadal_keycode_f2;
    hadal->interface.keycodes[KEY_F3]         = hadal_keycode_f3;
    hadal->interface.keycodes[KEY_F4]         = hadal_keycode_f4;
    hadal->interface.keycodes[KEY_F5]         = hadal_keycode_f5;
    hadal->interface.keycodes[KEY_F6]         = hadal_keycode_f6;
    hadal->interface.keycodes[KEY_F7]         = hadal_keycode_f7;
    hadal->interface.keycodes[KEY_F8]         = hadal_keycode_f8;
    hadal->interface.keycodes[KEY_F9]         = hadal_keycode_f9;
    hadal->interface.keycodes[KEY_F10]        = hadal_keycode_f10;
    hadal->interface.keycodes[KEY_F11]        = hadal_keycode_f11;
    hadal->interface.keycodes[KEY_F12]        = hadal_keycode_f12;
    hadal->interface.keycodes[KEY_F13]        = hadal_keycode_f13;
    hadal->interface.keycodes[KEY_F14]        = hadal_keycode_f14;
    hadal->interface.keycodes[KEY_F15]        = hadal_keycode_f15;
    hadal->interface.keycodes[KEY_F16]        = hadal_keycode_f16;
    hadal->interface.keycodes[KEY_F17]        = hadal_keycode_f17;
    hadal->interface.keycodes[KEY_F18]        = hadal_keycode_f18;
    hadal->interface.keycodes[KEY_F19]        = hadal_keycode_f19;
    hadal->interface.keycodes[KEY_F20]        = hadal_keycode_f20;
    hadal->interface.keycodes[KEY_F21]        = hadal_keycode_f21;
    hadal->interface.keycodes[KEY_F22]        = hadal_keycode_f22;
    hadal->interface.keycodes[KEY_F23]        = hadal_keycode_f23;
    hadal->interface.keycodes[KEY_F24]        = hadal_keycode_f24;
    hadal->interface.keycodes[KEY_KPSLASH]    = hadal_keycode_kp_divide;
    hadal->interface.keycodes[KEY_KPASTERISK] = hadal_keycode_kp_multiply;
    hadal->interface.keycodes[KEY_KPMINUS]    = hadal_keycode_kp_subtract;
    hadal->interface.keycodes[KEY_KPPLUS]     = hadal_keycode_kp_add;
    hadal->interface.keycodes[KEY_KP0]        = hadal_keycode_kp_0;
    hadal->interface.keycodes[KEY_KP1]        = hadal_keycode_kp_1;
    hadal->interface.keycodes[KEY_KP2]        = hadal_keycode_kp_2;
    hadal->interface.keycodes[KEY_KP3]        = hadal_keycode_kp_3;
    hadal->interface.keycodes[KEY_KP4]        = hadal_keycode_kp_4;
    hadal->interface.keycodes[KEY_KP5]        = hadal_keycode_kp_5;
    hadal->interface.keycodes[KEY_KP6]        = hadal_keycode_kp_6;
    hadal->interface.keycodes[KEY_KP7]        = hadal_keycode_kp_7;
    hadal->interface.keycodes[KEY_KP8]        = hadal_keycode_kp_8;
    hadal->interface.keycodes[KEY_KP9]        = hadal_keycode_kp_9;
    hadal->interface.keycodes[KEY_KPDOT]      = hadal_keycode_kp_decimal;
    hadal->interface.keycodes[KEY_KPEQUAL]    = hadal_keycode_kp_equal;
    hadal->interface.keycodes[KEY_KPENTER]    = hadal_keycode_kp_enter;
    hadal->interface.keycodes[KEY_102ND]      = hadal_keycode_world_2;

    for (s32 scancode = 0; scancode < 256; scancode++)
        if (hadal->interface.keycodes[scancode] > 0)
            hadal->interface.scancodes[hadal->interface.keycodes[scancode]] = scancode;
}

static void hadal_encore_zero_ref(struct hadal_encore *hadal)
{
    if (!hadal)
        return;

#ifdef HADAL_WAYLAND_LIBDECOR
    if (hadal->shell.libdecor)
        libdecor_unref(hadal->shell.libdecor);
    if (hadal->module_libdecor)
        bedrock_module_close(hadal->module_libdecor);
#endif /* HADAL_WAYLAND_LIBDECOR */

    if (hadal->xkb_compose_state)
        xkb_compose_state_unref(hadal->xkb_compose_state);
    if (hadal->xkb_keymap)
        xkb_keymap_unref(hadal->xkb_keymap);
    if (hadal->xkb_state)
        xkb_state_unref(hadal->xkb_state);
    if (hadal->xkb_context)
        xkb_context_unref(hadal->xkb_context);
    if (hadal->module_xkbcommon)
        bedrock_module_close(hadal->module_xkbcommon);

    for (u32 i = 0; i < hadal->cursor_themes_count; i++)
        wl_cursor_theme_destroy(hadal->cursor_themes[i].theme);
    if (hadal->cursor_shape_manager)
        wp_cursor_shape_manager_v1_destroy(hadal->cursor_shape_manager);
    if (hadal->module_cursor)
        bedrock_module_close(hadal->module_cursor);

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
    if (hadal->viewporter)
        wp_viewporter_destroy(hadal->viewporter);
    if (hadal->decoration_manager)
        zxdg_decoration_manager_v1_destroy(hadal->decoration_manager);
    if (hadal->primary_selection_device_manager)
        zwp_primary_selection_device_manager_v1_destroy(hadal->primary_selection_device_manager);
    if (hadal->data_device_manager)
        wl_data_device_manager_destroy(hadal->data_device_manager);
    if (hadal->relative_pointer_manager)
        zwp_relative_pointer_manager_v1_destroy(hadal->relative_pointer_manager);
    if (hadal->pointer_constraints)
        zwp_pointer_constraints_v1_destroy(hadal->pointer_constraints);
    if (hadal->idle_inhibit_manager)
        zwp_idle_inhibit_manager_v1_destroy(hadal->idle_inhibit_manager);
    if (hadal->activation_manager)
        xdg_activation_v1_destroy(hadal->activation_manager);
    if (hadal->fractional_scale_manager)
        wp_fractional_scale_manager_v1_destroy(hadal->fractional_scale_manager);
    if (hadal->registry)
        wl_registry_destroy(hadal->registry);
    if (hadal->display) {
        hadal->wl.display_flush(hadal->display);
        hadal->wl.display_disconnect(hadal->display);
    }
    if (hadal->module_client)
        bedrock_module_close(hadal->module_client);

    bedrock_zerop(hadal);
    g_wayland = NULL;
}

FN_RIVEN_ENCORE(hadal, wayland)
{
    const char *name = "hadal_wayland";

    /* we allow only one Wayland backend instance to exist at a time */
    if (lake_unlikely(g_wayland != NULL))
        return g_wayland;

    void *module_client = bedrock_module_open("libwayland-client.so.0");
    if (!module_client)
        bedrock_log_debug("'%s' libwayland-client.so.0 is missing.", name);

    void *module_cursor = bedrock_module_open("libwayland-cursor.so.0");
    if (!module_cursor)
        bedrock_log_debug("'%s' libwayland-cursor.so.0 is missing.", name);

    void *module_xkbcommon = bedrock_module_open("libxkbcommon.so.0");
    if (!module_xkbcommon)
        bedrock_log_debug("'%s' libxkbcommon.so.0 is missing.", name);

#ifdef HADAL_WAYLAND_LIBDECOR
    /* optional */
    void *module_libdecor = bedrock_module_open("libdecor-0.so.0");
    if (!module_libdecor)
        bedrock_log_debug("'%s' optional libdecor.so.0 is missing, this feature will be disabled.", name);
#endif /* HADAL_WAYLAND_LIBDECOR */

    if (!module_client || !module_cursor || !module_xkbcommon) {
disconnect:
#ifdef HADAL_WAYLAND_LIBDECOR
        bedrock_module_close(module_libdecor);
#endif /* HADAL_WAYLAND_LIBDECOR */
        bedrock_module_close(module_xkbcommon);
        bedrock_module_close(module_cursor);
        bedrock_module_close(module_client);
        hadal_encore_zero_ref(g_wayland);
        return NULL;
    }

    PFN_wl_display_connect display_connect = (PFN_wl_display_connect)
        bedrock_get_proc_address(module_client, "wl_display_connect");
    if (!display_connect) {
        bedrock_log_debug("'%s' can't load 'wl_display_connect'.", name);
        goto disconnect;
    }

    struct wl_display *display = display_connect(NULL);
    if (!display)  {
        bedrock_log_debug("'%s' can't connect to a Wayland display, make sure you're running a Wayland compositor.", name);
        goto disconnect;
    }

    struct hadal_encore *hadal = (struct hadal_encore *)
        riven_thalloc(context->self, tag, lake_sizeof(struct hadal_encore), lake_alignof(struct hadal_encore));
    bedrock_zerop(hadal);

    hadal->display = display;
    hadal->wl.display_connect = display_connect;
    hadal->module_client = module_client;
    hadal->module_cursor = module_cursor;
    hadal->module_xkbcommon = module_xkbcommon;
#ifdef HADAL_WAYLAND_LIBDECOR
    hadal->module_libdecor = module_libdecor;
#endif /* HADAL_WAYLAND_LIBDECOR */

    /* write the header */
    hadal->interface.riven.context = *context;
    hadal->interface.riven.tag = tag;
    hadal->interface.riven.name = name;
    hadal->interface.riven.zero_ref_callback = (PFN_riven_work)hadal_encore_zero_ref;
    g_wayland = hadal;

    if (!load_symbols(hadal, name))
        goto disconnect;

    hadal->key_repeat_timerfd = -1;
    hadal->cursor_timerfd = -1;

    hadal->registry = wl_display_get_registry(hadal->display);
    wl_registry_add_listener(hadal->registry, &registry_listener, hadal);
    create_key_tables(hadal);

    hadal->xkb_context = xkb_context_new(0);
    if (!hadal->xkb_context) {
        bedrock_log_debug("'%s' failed to initialize XKB context.", name);
        goto disconnect;
    }

    /* sync to get all registry objects */
    wl_display_roundtrip(hadal->display);

    /* sync to get initial output events */
    wl_display_roundtrip(hadal->display);

#ifdef HADAL_WAYLAND_LIBDECOR
    if (module_libdecor) {
        //wayland->shell.libdecor = libdecor_new(display, &libdecor_interface);
        if (hadal->shell.libdecor) {
            /* perform an initial dispatch and flush to get the init started */
            //libdecor_dispatch(wayland->shell.libdecor, 0);

            /* TODO sync point callback? */
        }
    }
#endif /* HADAL_WAYLAND_LIBDECOR */

    /* write the interface */
    hadal->interface.window_assembly = _hadal_wayland_window_assembly;
    hadal->interface.window_disassembly = _hadal_wayland_window_disassembly;
    hadal->interface.window_visibility = _hadal_wayland_window_visibility;
#ifdef XAKU_VULKAN
    hadal->interface.vulkan_write_instance = _hadal_wayland_vulkan_write_instance;
    hadal->interface.vulkan_presentation_support = _hadal_wayland_vulkan_presentation_support;
    hadal->interface.vulkan_surface_create = _hadal_wayland_vulkan_surface_create;
#endif /* XAKU_VULKAN */
    return hadal;
}

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
#endif /* HADAL_WAYLAND */
