#include <lake/bedrock/log.h>

#include "vk_platynova.h"
#include "../../hadal/hadopelagic.h"

#ifdef AMW_NATIVE_WAYLAND
VkResult create_wayland_surface(
        PFN_vkCreateWaylandSurfaceKHR proc, 
        VkInstance instance, 
        struct wl_display *display,
        struct wl_surface *wl_surface,
        const VkAllocationCallbacks *allocator, 
        VkSurfaceKHR *surface)
{
        VkWaylandSurfaceCreateInfoKHR wayland_surface_info = {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .display = display,
            .surface = wl_surface,
        };
        return proc(instance, &wayland_surface_info, allocator, surface);
}
#endif

VkResult create_headless_surface(
        PFN_vkCreateHeadlessSurfaceEXT proc, 
        VkInstance instance, 
        const VkAllocationCallbacks *allocator, 
        VkSurfaceKHR *surface)
{
    (void)proc;
    (void)instance;
    (void)allocator;
    (void)surface;
    return VK_SUCCESS;
}

VkResult vulkan_create_surface(
        struct vulkan_instance_api *api, 
        VkInstance instance, 
        struct hadal *hadal, 
        const VkAllocationCallbacks *allocator, 
        VkSurfaceKHR *surface)
{
    assert_debug(api && hadal && surface && instance != VK_NULL_HANDLE);

    *surface = VK_NULL_HANDLE;

    switch (hadal->api.api) {
#ifdef AMW_PLATFORM_WINDOWS
    case hadal_backend_win32:
        break;
#endif
#ifdef AMW_PLATFORM_MACOS
    case hadal_backend_cocoa:
        break;
#endif
#ifdef AMW_PLATFORM_IOS
    case hadal_backend_ios:
        break;
#endif
#ifdef AMW_NATIVE_ANDROID
    case hadal_backend_android:
        break;
#endif
#ifdef AMW_NATIVE_WAYLAND
    case hadal_backend_wayland:
        return create_wayland_surface(api->vkCreateWaylandSurfaceKHR, instance, hadal->wl.display, hadal->window.wl.surface, allocator, surface);
#endif
#ifdef AMW_NATIVE_XCB
    case hadal_backend_xcb:
        break;
#endif
#ifdef AMW_NATIVE_KMS
    case hadal_backend_kms:
        break;
#endif
    case hadal_backend_headless:
        return create_headless_surface(api->vkCreateHeadlessSurfaceEXT, instance, allocator, surface);
    default:
        break;
    }
    return VK_ERROR_FEATURE_NOT_PRESENT;
}
