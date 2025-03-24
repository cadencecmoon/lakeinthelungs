#include "wl_hadal.h"

#include <amw/log.h>

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
        .display =hadal->display,
        .surface = window->surface,
    };

    /* VK_SUCCESS is a non-zero value so its fine enough */
    return hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.instance, &surface_info, callbacks, out_surface);
}

FN_HADAL_VULKAN_PHYSICAL_DEVICE_PRESENTATION_SUPPORT(wayland)
{
    struct hadal *hadal = window->header.hadal;
    return (b32)hadal->vulkan.vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, queue_family, hadal->display);
}
#endif /* REZNOR_VULKAN */
