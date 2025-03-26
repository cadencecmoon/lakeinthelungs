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
    (void)window;
    (void)config;
    (void)memory;
    return result_success;
}

FN_HADAL_WINDOW_DESTROY(wayland)
{
    (void)window;

}

FN_HADAL_WINDOW_ATTACH_SWAPCHAIN(wayland)
{
    (void)window;
    (void)swapchain;
}

FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(wayland)
{
    (void)window;
    (void)out_width;
    (void)out_height;
}

#ifdef REZNOR_VULKAN
FN_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES(wayland)
{
    assert_debug(hadal && instance && vkGetInstanceProcAddr);

    hadal->vulkan.instance = instance;
    hadal->vulkan.vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)(void *)vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR");
    hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(void *)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");

    if (!hadal->vulkan.vkCreateWaylandSurfaceKHR ||
        !hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR)
    {
        log_error("Wayland can't load surface-related Vulkan procedures.");
        return result_error;
    }
    return result_success;
}

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

FN_HADAL_VULKAN_PRESENTATION_SUPPORT(wayland)
{
    struct hadal *hadal = window->header.hadal;
    return (b32)hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, queue_family, hadal->display);
}
#endif /* REZNOR_VULKAN */
