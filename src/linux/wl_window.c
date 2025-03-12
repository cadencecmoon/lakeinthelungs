#include "wl_hadal.h"

#include <amw/log.h>

void _hadal_wayland_acquire_framebuffer_extent(
    struct hadal *hadal, 
    u32          *width, 
    u32          *height)
{
    assert_debug(width && height);
    *width = atomic_load_explicit(&hadal->fb_width, memory_order_acquire);
    *height = atomic_load_explicit(&hadal->fb_height, memory_order_acquire);
}

#ifdef PELAGIA_VULKAN
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

s32 _hadal_wayland_vulkan_create_surface(
    const struct hadal                 *hadal,
    struct VkInstance_T                *instance,
    struct VkSurfaceKHR_T             **out_surface,
    const struct VkAllocationCallbacks *callbacks,
    PFN_vkGetInstanceProcAddr           vkGetInstanceProcAddr)
{
    assert_debug(instance && out_surface && vkGetInstanceProcAddr);

    PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)(void *)
        vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR");

    if (!vkCreateWaylandSurfaceKHR) {
        log_error("Can't get the vkCreateWaylandSurfaceKHR procedure.");
        return result_error;
    }

    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = hadal->display,
        .surface = hadal->surface,
    };

    /* VK_SUCCESS is a non-zero value so its fine enough */
    return vkCreateWaylandSurfaceKHR(instance, &surface_info, callbacks, out_surface);
}

b32 _hadal_wayland_vulkan_physical_device_presentation_support(
    const struct hadal                 *hadal,
    struct VkInstance_T                *instance,
    struct VkSurfaceKHR_T              *surface,
    struct VkPhysicalDevice_T          *physical_device,
    u32                                 queue_family,
    PFN_vkGetInstanceProcAddr           vkGetInstanceProcAddr)
{
    assert_debug(instance && vkGetInstanceProcAddr);

    (void)surface;

    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(void *)
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");

    if (!vkGetPhysicalDeviceWaylandPresentationSupportKHR) {
        log_error("Can't get the vkGetPhysicalDeviceWaylandPresentationSupportKHR procedure.");
        return false;
    }
    return (b32)vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, queue_family, hadal->display);
}
#endif
