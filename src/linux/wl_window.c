#include "wl_hadal.h"

#include <amw/log.h>

#if 0
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
#endif

FN_HADAL_WINDOW_CREATE(wayland)
{
    (void)hadal;
    (void)create_info;
    (void)memory;
    (void)out_window;
    return result_success;
}

FN_HADAL_WINDOW_DESTROY(wayland)
{
    (void)window;

}

FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(wayland)
{
    (void)window;
    (void)out_width;
    (void)out_height;
}

#ifdef REZNOR_VULKAN
FN_HADAL_VULKAN_SURFACE_CREATE(wayland)
{
    struct hadal *hadal = window->header.hadal;
    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = hadal->display,
        .surface = window->surface,
    };

    /* VK_SUCCESS is a non-zero value so its fine enough */
    return hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.instance, &surface_info, callbacks, out_surface);
}
#endif /* REZNOR_VULKAN */
