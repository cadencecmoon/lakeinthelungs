/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/hadopelagic.h>

#include "vk_cobalt.h"

#ifdef AMW_PLATFORM_WINDOW
#endif /* AMW_PLATFORM_WINDOW */

#ifdef AMW_PLATFORM_APPLE
#endif /* AMW_PLATFORM_APPLE */

#ifdef AMW_PLATFORM_ANDROID
#endif /* AMW_PLATFORM_ANDROID */

#ifdef AMW_NATIVE_WAYLAND
static s32 create_surface_wayland(struct vulkan_backend *vk, struct wl_display *display, struct wl_surface *surface)
{
    VkWaylandSurfaceCreateInfoKHR wayland_surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = display,
        .surface = surface,
    };
    // TODO &vk->allocator
    VkResult res = vk->api.vkCreateWaylandSurfaceKHR(vk->instance, &wayland_surface_info, NULL, &vk->swapchain.surface);
    if (res != VK_SUCCESS)
        return result_error_undefined; /* TODO */
    return result_success;
}
#endif /* AMW_NATIVE_WAYLAND */

#ifdef AMW_NATIVE_XCB
#endif /* AMW_NATIVE_XCB */

#ifdef AMW_NATIVE_DRM
#endif /* AMW_NATIVE_DRM */

AMWAPI s32 cobalt_vulkan_create_swapchain_surface(struct cobalt *cobalt, struct hadopelagic *hadal)
{
    struct vulkan_backend *vk = (struct vulkan_backend *)cobalt->backend;

    if (vk->instance == VK_NULL_HANDLE || !hadal->calls.expose_native_window)
        return result_error_invalid_engine_context;
    vk->swapchain.surface = VK_NULL_HANDLE;

    void *display, *window;
    hadal->calls.expose_native_window(hadal, &display, &window);

    switch (hadal->backend_api) {
#ifdef AMW_PLATFORM_WINDOW
    case hadal_backend_api_win32: 
        break;
#endif /* AMW_PLATFORM_WINDOW */
#ifdef AMW_PLATFORM_APPLE
    case hadal_backend_api_cocoa:
    case hadal_backend_api_ios:
        break;
#endif /* AMW_PLATFORM_APPLE */
#ifdef AMW_PLATFORM_ANDROID
    case hadal_backend_api_android:
        break;
#endif /* AMW_PLATFORM_ANDROID */
#ifdef AMW_NATIVE_WAYLAND
    case hadal_backend_api_wayland:
        return create_surface_wayland(vk, (struct wl_display *)display, (struct wl_surface *)window);
#endif /* AMW_NATIVE_WAYLAND */
#ifdef AMW_NATIVE_XCB
    case hadal_backend_api_xcb:
        break;
#endif /* AMW_NATIVE_XCB */
#ifdef AMW_NATIVE_DRM
    case hadal_backend_api_drm:
        break;
#endif /* AMW_NATIVE_DRM */
    case hadal_backend_api_headless:
        break;
    default:
        break;
    }
    return result_error_invalid_engine_context;
}
