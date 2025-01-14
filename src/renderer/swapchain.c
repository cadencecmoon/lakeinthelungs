/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/vulkan.h>
#include <lake/pelagia.h>

#ifdef AMW_PLATFORM_WINDOW
#endif /* AMW_PLATFORM_WINDOW */

#ifdef AMW_PLATFORM_APPLE
#endif /* AMW_PLATFORM_APPLE */

#ifdef AMW_PLATFORM_ANDROID
#endif /* AMW_PLATFORM_ANDROID */

#ifdef AMW_NATIVE_WAYLAND
static s32 create_surface_wayland(
    struct vulkan_backend   *vk, 
    struct vulkan_swapchain *swapchain,
    struct wl_display       *display, 
    struct wl_surface       *surface)
{
    VkWaylandSurfaceCreateInfoKHR wayland_surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = display,
        .surface = surface,
    };
    VkResult res = vk->api.vkCreateWaylandSurfaceKHR(vk->instance, &wayland_surface_info, NULL, &swapchain->surface);
    if (res != VK_SUCCESS)
        return result_error_undefined; /* TODO */
    return result_success;
}
#endif /* AMW_NATIVE_WAYLAND */

#ifdef AMW_NATIVE_XCB
#endif /* AMW_NATIVE_XCB */

#ifdef AMW_NATIVE_DRM
#endif /* AMW_NATIVE_DRM */

AMWAPI s32 vulkan_create_swapchain_surface(
    struct vulkan_backend   *vk,
    struct vulkan_swapchain *swapchain, 
    struct hadopelagic      *hadal)
{
    if (vk->instance == VK_NULL_HANDLE || !hadal->calls.expose_native_window)
        return result_error_invalid_engine_context;
    swapchain->surface = VK_NULL_HANDLE;

    void *display, *window;
    hadal->calls.expose_native_window(hadal, &display, &window);

    if (display == NULL || window == NULL) {
        log_error("Can't expose a native window using the display backend '%s'.\n"
                  "\t(XXX this is an invalid state in Wayland, but may not be in other platforms...)", hadal->backend_name);
        return result_error_invalid_engine_context;
    }

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
        return create_surface_wayland(vk, swapchain, (struct wl_display *)display, (struct wl_surface *)window);
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
    return result_error_undefined; /* TODO */
}

AMWAPI void vulkan_clear_swapchain(
    struct vulkan_swapchain *swapchain,
    struct vulkan_device    *primary_device)
{
    for (u32 i = 0; i < swapchain->image_count; i++)
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            primary_device->api.vkDestroyImageView(primary_device->logical, swapchain->image_views[i], NULL);
    arena_reset(&swapchain->arena);

    struct vulkan_swapchain cleared = {
        .sc = swapchain->sc,
        .surface = swapchain->surface,
        .surface_capabilities = swapchain->surface_capabilities,
        .arena = swapchain->arena,
    };
    *swapchain = cleared;
}

AMWAPI void pelagia_assemble_swapchain(struct pelagia_assemble_swapchain_work *work)
{
    if (!work) {
        return;
    } else if (!work->pelagia || (at_read_relaxed(&work->pelagia->flags) & pelagia_flag_initialized) == 0) {
        work->out_result = result_error_invalid_engine_context;
        return;
    }

    struct pelagia     *pelagia         = work->pelagia;
    struct hadopelagic *hadal           = work->hadal;
    b32                 use_vsync       = work->use_vsync; 
    b32                 create_surface  = false;

    struct vulkan_backend   *vk             = &pelagia->vk;
    struct vulkan_swapchain *swapchain      = &pelagia->swapchain;
    struct vulkan_device    *primary_device = &pelagia->devices[0];

    /* TODO work->surface_lost ? */
    if (work->surface_lost) {
        log_warn("The swapchain surface was lost. Trying to recreate the surface...");
        if (swapchain->surface)
            vk->api.vkDestroySurfaceKHR(vk->instance, swapchain->surface, NULL);
        create_surface = true;
    } else if (swapchain->surface == NULL) {
        create_surface = true;
    }
    if (create_surface && vulkan_create_swapchain_surface(vk, swapchain, hadal) != result_success) {
        log_error("Can't create a Vulkan surface from the %s display backend. Consider opening a fallback or headless display to continue.", hadal->backend_name);
        work->out_result = result_error_no_fallback;
        return;
    }

    VkSwapchainKHR old_sc = swapchain->sc;
    u32 window_width = 0, window_height = 0;
    u32 set_pelagia_flags = 0;

    if (old_sc != VK_NULL_HANDLE)
        vulkan_clear_swapchain(swapchain, primary_device);

    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR(primary_device->physical, swapchain->surface, &swapchain->surface_format_count, NULL));
    swapchain->surface_formats = (VkSurfaceFormatKHR *)arena_alloc(&swapchain->arena, sizeof(VkSurfaceFormatKHR) * swapchain->surface_format_count);
    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR(primary_device->physical, swapchain->surface, &swapchain->surface_format_count, swapchain->surface_formats));

    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR(primary_device->physical, swapchain->surface, &swapchain->present_mode_count, NULL));
    swapchain->present_modes = (VkPresentModeKHR *)arena_alloc(&swapchain->arena, sizeof(VkPresentModeKHR) * swapchain->present_mode_count);
    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR(primary_device->physical, swapchain->surface, &swapchain->present_mode_count, swapchain->present_modes));

    swapchain->format = VK_FORMAT_UNDEFINED;
    if (swapchain->surface_format_count == 1 && swapchain->surface_formats[0].format == VK_FORMAT_UNDEFINED)
        swapchain->format = VK_FORMAT_B8G8R8A8_UNORM;
    for (u32 i = 0; i < swapchain->surface_format_count; i++) {
        if (swapchain->surface_formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            continue;
        VkFormat format = swapchain->surface_formats[i].format;
        if (format == VK_FORMAT_R8G8B8A8_UNORM ||
            format == VK_FORMAT_R8G8B8A8_SRGB ||
            format == VK_FORMAT_B8G8R8A8_UNORM ||
            format == VK_FORMAT_B8G8R8A8_SRGB ||
            format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 ||
            format == VK_FORMAT_A2R10G10B10_UNORM_PACK32) 
        {
            swapchain->format = format;
        }
    }
    if (swapchain->format == VK_FORMAT_UNDEFINED) {
        log_error("Unable to determine an appropriate surface format. Only R8G8B8A8, B8G8R8A8, A2R10G10B10 or A2B10G10R10 formats are supported.");
        if (old_sc != VK_NULL_HANDLE)
            primary_device->api.vkDestroySwapchainKHR(primary_device->logical, old_sc, NULL);
        vulkan_clear_swapchain(swapchain, primary_device);
        work->out_result = result_error_undefined; /* TODO */
        return;
    }

    hadal_framebuffer_size(hadal, &window_width, &window_height);
    swapchain->extent.width = (swapchain->surface_capabilities.currentExtent.width != 0xFFFFFFFF) ? swapchain->surface_capabilities.currentExtent.width : window_width;
    swapchain->extent.height = (swapchain->surface_capabilities.currentExtent.height != 0xFFFFFFFF) ? swapchain->surface_capabilities.currentExtent.height : window_height;
    if (window_width != swapchain->extent.width || window_height != swapchain->extent.height) {
        log_trace("The swapchain resolution is %ux%u.", swapchain->extent.width, swapchain->extent.height);
    }

    /* find a supported composite alpha mode (one of these is guaranteed to be set) */
    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR composite_alpha_flags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (u32 i = 0; i < arraysize(composite_alpha_flags); i++) {
        if (swapchain->surface_capabilities.supportedCompositeAlpha & composite_alpha_flags[i]) {
            composite_alpha = composite_alpha_flags[i];
            break;
        }
    }
    VkPresentModeKHR no_vsync_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < swapchain->present_mode_count; i++) {
        if (swapchain->present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && no_vsync_present_mode == VK_PRESENT_MODE_FIFO_KHR)
            no_vsync_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (swapchain->present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            no_vsync_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    if (no_vsync_present_mode == VK_PRESENT_MODE_FIFO_KHR && !use_vsync) {
        log_warn("No presentation mode without vertical synchronization is available. Enabling v-sync instead.");
    }

    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (swapchain->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        set_pelagia_flags |= pelagia_flag_screenshot_supported;
    }

    VkSharingMode sharing_mode = VK_SHARING_MODE_CONCURRENT;
    u32 queue_family_count = 3;
    u32 queue_family_indices[3] = {
        primary_device->graphics_queue_family_idx,
        primary_device->compute_queue_family_idx,
        primary_device->transfer_queue_family_idx,
    };
    if (primary_device->compute_queue_family_idx != primary_device->graphics_queue_family_idx 
            && primary_device->transfer_queue_family_idx == primary_device->graphics_queue_family_idx) {
        queue_family_count = 2;
    } else if (primary_device->compute_queue_family_idx == primary_device->graphics_queue_family_idx 
            && primary_device->transfer_queue_family_idx != primary_device->graphics_queue_family_idx) 
    {
        queue_family_indices[1] = primary_device->transfer_queue_family_idx;
        queue_family_count = 2;
    } else if (primary_device->compute_queue_count == 0 && primary_device->transfer_queue_count == 0) {
        sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
        queue_family_count = 1;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = swapchain->surface,
        .minImageCount = pelagia->frames_buffering,
        .imageFormat = swapchain->format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = swapchain->extent,
        .imageArrayLayers = 1,
        .imageUsage = image_usage,
        .imageSharingMode = sharing_mode,
        .queueFamilyIndexCount = queue_family_count,
        .pQueueFamilyIndices = queue_family_indices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = composite_alpha,
        .presentMode = use_vsync ? VK_PRESENT_MODE_FIFO_KHR : no_vsync_present_mode,
        .clipped = VK_FALSE,
        .oldSwapchain = old_sc,
    };
    VkResult result = primary_device->api.vkCreateSwapchainKHR(primary_device->logical, &swapchain_create_info, NULL, &swapchain->sc);

    if (old_sc != VK_NULL_HANDLE)
        primary_device->api.vkDestroySwapchainKHR(primary_device->logical, old_sc, NULL);
    if (result != VK_SUCCESS) {
        log_error("Failed to construct a swap chain: %s", vulkan_result_string(result));
        vulkan_clear_swapchain(swapchain, primary_device);
        work->out_result = result_error_undefined; /* TODO */
        return;
    }

    VERIFY_VK(primary_device->api.vkGetSwapchainImagesKHR(primary_device->logical, swapchain->sc, &swapchain->image_count, NULL));
    swapchain->images = (VkImage *)arena_alloc(&swapchain->arena, sizeof(VkImage) * swapchain->image_count);
    VERIFY_VK(primary_device->api.vkGetSwapchainImagesKHR(primary_device->logical, swapchain->sc, &swapchain->image_count, swapchain->images));
    swapchain->image_views = (VkImageView *)arena_alloc(&swapchain->arena, sizeof(VkImageView) * swapchain->image_count);
    for (u32 i = 0; i < swapchain->image_count; i++) {
        VkImageViewCreateInfo color_image_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.levelCount = 1,
            .subresourceRange.layerCount = 1,
        };
        result = primary_device->api.vkCreateImageView(primary_device->logical, &color_image_view, NULL, &swapchain->image_views[i]);
        if (result != VK_SUCCESS) {
            log_error("Failed to create a view onto swapchain image: %s", vulkan_result_string(result));
            vulkan_clear_swapchain(swapchain, primary_device);
            work->out_result = result_error_undefined; /* TODO */
            return;
        }
    }

#if AMW_DEBUG
    if (swapchain->image_count != pelagia->frames_buffering) 
        log_warn("Swapchain image count (%u) is different than frame buffering (%u).", swapchain->image_count, pelagia->frames_buffering);
#endif /* AMW_DEBUG */

    if (use_vsync || no_vsync_present_mode == VK_PRESENT_MODE_FIFO_KHR)
        set_pelagia_flags |= pelagia_flag_vsync_enabled;

    at_fetch_and_explicit(&work->hadal->flags, ~hadal_flag_recreate_swapchain, memory_model_seq_cst);
    at_fetch_and_explicit(&work->pelagia->flags, ~(pelagia_flag_vsync_enabled | pelagia_flag_screenshot_supported), memory_model_seq_cst);
    at_fetch_or_explicit(&work->pelagia->flags, set_pelagia_flags, memory_model_seq_cst);
    work->out_result = result_success;
}

