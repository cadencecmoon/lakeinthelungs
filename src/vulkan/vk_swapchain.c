#include "vk_reznor.h"

#include <amw/hadal.h>

static VkResult swapchain_assembly(
    struct reznor_device                   *device, 
    struct reznor_swapchain                *swapchain, 
    VkSwapchainKHR                          old_swapchain,
    enum reznor_texture_format              preferred_format,
    b32                                     enable_vsync,
    b32                                     touch_surface)
{
    VkResult res;
    u32 image_count = device->header.frames_in_flight;
    u32 surface_format_count = 0;
    u32 present_mode_count = 0;

    struct reznor *reznor = device->header.reznor;
    struct riven *riven = reznor->interface.header.riven;
    VkPhysicalDevice physical = device->physical->device;

    struct hadal_window_header *window = (struct hadal_window_header *)swapchain->window;
    const struct hadal_interface *hadal = (const struct hadal_interface *)window->hadal;

    u32 flags = (atomic_load_explicit(&swapchain->header.flags, memory_order_relaxed) & 
        (reznor_swapchain_flag_is_valid | reznor_swapchain_flag_vsync_enabled | reznor_swapchain_flag_close_window_on_destroy));

    if (!touch_surface)
        goto surface_done;

    /* create a window surface */
    if (swapchain->surface != VK_NULL_HANDLE)
        reznor->vkDestroySurfaceKHR(reznor->instance, swapchain->surface, &device->host_allocator);
    swapchain->surface = VK_NULL_HANDLE;

    res = hadal->vulkan_surface_create(swapchain->window, &swapchain->surface, &device->host_allocator);
    if (res != VK_SUCCESS)
        return res;

    /* get capabilities of the surface */
    res = reznor->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, swapchain->surface, &swapchain->surface_capabilities);
    if (res != VK_SUCCESS)
        return res;

    /* get surface format count */
    res = reznor->vkGetPhysicalDeviceSurfaceFormatsKHR(physical, swapchain->surface, &surface_format_count, NULL);
    if (!surface_format_count) {
        log_debug("Vulkan surface of physical device '%s' and window '%s' has no surface formats available.",
            device->physical->properties2.properties.deviceName, window->title.ptr);
        return res != VK_SUCCESS ? res : VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    /* get present mode count */
    res = reznor->vkGetPhysicalDeviceSurfacePresentModesKHR(physical, swapchain->surface, &present_mode_count, NULL);
    if (!present_mode_count) {
        log_debug("Vulkan surface of physical device '%s' and window '%s' has no presentation modes.",
            device->physical->properties2.properties.deviceName, window->title.ptr);
        return res != VK_SUCCESS ? res : VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    usize surface_format_bytes = sizeof(VkSurfaceFormatKHR) * surface_format_count;
    usize present_mode_bytes = sizeof(VkPresentModeKHR) * present_mode_count;
    u8 *raw = (u8 *)riven_alloc(riven, riven_tag_deferred, surface_format_bytes + present_mode_bytes, 8);

    VkSurfaceFormatKHR *surface_formats = (VkSurfaceFormatKHR *)raw;
    res = reznor->vkGetPhysicalDeviceSurfaceFormatsKHR(physical, swapchain->surface, &surface_format_count, surface_formats);
    if (res != VK_SUCCESS)
        return res;

    VkPresentModeKHR *present_modes = (VkPresentModeKHR *)&raw[surface_format_bytes];
    res = reznor->vkGetPhysicalDeviceSurfacePresentModesKHR(physical, swapchain->surface, &present_mode_count, present_modes);
    if (res != VK_SUCCESS)
        return res;

    VkFormat image_formats[] = {
        vulkan_texture_format_translate(preferred_format),
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SRGB,
    };

    /* select an appropriate image format */
    swapchain->image_format = VK_FORMAT_UNDEFINED;

    if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
        swapchain->image_count = VK_FORMAT_B8G8R8A8_UNORM;
    for (u32 i = 0; i < arraysize(image_formats) && swapchain->image_format == VK_FORMAT_UNDEFINED; i++) {
        VkFormat format = image_formats[i];

        for (u32 j = 0; j < surface_format_count; j++) {
            if (surface_formats[j].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                continue;
            if (surface_formats[j].format == format) {
                swapchain->image_format = format;
                break;
            }
        }
    }

    /* select a present mode */
    swapchain->no_vsync_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && swapchain->no_vsync_present_mode == VK_PRESENT_MODE_FIFO_KHR)
            swapchain->no_vsync_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            swapchain->no_vsync_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    if (swapchain->no_vsync_present_mode == VK_PRESENT_MODE_FIFO_KHR)
        flags |= reznor_swapchain_flag_vsync_enabled;

    VkCompositeAlphaFlagsKHR composite_alpha_flags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    /* select a composite alpha */
    swapchain->composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    for (u32 i = 0; i < arraysize(composite_alpha_flags); i++) {
        if (swapchain->surface_capabilities.supportedCompositeAlpha & composite_alpha_flags[i]) {
            swapchain->composite_alpha = composite_alpha_flags[i];
            break;
        }
    }
surface_done:

    /* set the image usage */
    swapchain->image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (swapchain->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapchain->image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        flags |= reznor_swapchain_flag_screenshot_supported;
    }

    /* set the sharing mode (images are shared between queue families, not queues) */
    swapchain->sharing_mode = (device->physical->queue_family_count == 1 ||
        (device->physical->graphics_queue_family_index == device->physical->presentation_queue_family_index &&
        device->physical->transfer_queue_family_index == device->physical->presentation_queue_family_index))
            ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

    /* set the resolution */
    u32 fb_width, fb_height;
    hadal->window_acquire_framebuffer_extent(swapchain->window, &fb_width, &fb_height);

    swapchain->extent.width = (swapchain->surface_capabilities.currentExtent.width != UINT32_MAX) 
            ? swapchain->surface_capabilities.currentExtent.width : fb_width;
    swapchain->extent.height = (swapchain->surface_capabilities.currentExtent.height != UINT32_MAX) 
            ? swapchain->surface_capabilities.currentExtent.height : fb_height;

    /* set the image count */
    u32 min_image_count = swapchain->surface_capabilities.minImageCount;
    u32 max_image_count = swapchain->surface_capabilities.maxImageCount;
    if (image_count < min_image_count)
        image_count = min_image_count;
    if (max_image_count && image_count > max_image_count)
        image_count = max_image_count;

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = swapchain->surface,
        .minImageCount = image_count,
        .imageFormat = swapchain->image_format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = swapchain->extent,
        .imageArrayLayers = 1,
        .imageUsage = swapchain->image_usage,
        .imageSharingMode = swapchain->sharing_mode,
        .queueFamilyIndexCount = device->physical->queue_family_count,
        .pQueueFamilyIndices = device->physical->queue_family_indices,
        .preTransform = swapchain->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
            ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : swapchain->surface_capabilities.currentTransform,
        .compositeAlpha = swapchain->composite_alpha,
        .presentMode = enable_vsync ? VK_PRESENT_MODE_FIFO_KHR : swapchain->no_vsync_present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain,
    };

    /* create the swapchain */
    res = device->vkCreateSwapchainKHR(device->logical, &swapchain_info, &device->host_allocator, &swapchain->swapchain);
    if (res != VK_SUCCESS)
        return res;

    /* get swapchain images and create views */
    res = device->vkGetSwapchainImagesKHR(device->logical, swapchain->swapchain, &swapchain->image_count, swapchain->images);
    if (res != VK_SUCCESS)
        return res;

    VkImageViewCreateInfo color_image_view = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchain->image_format,
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

    for (u32 i = 0; i < swapchain->image_count; i++) {
        color_image_view.image = swapchain->images[i];

        /* create the views */
        res = device->vkCreateImageView(device->logical, &color_image_view, &device->host_allocator, &swapchain->image_views[i]);
        if (res != VK_SUCCESS)
            return res;
    }

    VkSemaphoreCreateInfo sem_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    for (u32 i = 0; i < device->header.frames_in_flight; i++) {
        /* create semaphores */
        if (swapchain->image_available_semaphores[i] == VK_NULL_HANDLE) {
            res = device->vkCreateSemaphore(device->logical, &sem_info, &device->host_allocator, &swapchain->image_available_semaphores[i]);
            if (res != VK_SUCCESS)
                return res;
        }
    }

    atomic_store_explicit(&swapchain->header.flags, flags, memory_order_release);
    window->swapchain = (struct reznor_resource_header *)swapchain;

    return VK_SUCCESS;
}

FN_REZNOR_ASSEMBLY(vulkan, swapchain) { 
    assert_debug(work && work->assembly.swapchain && work->memory.data && work->type == reznor_resource_type_swapchain && work->device);
    struct reznor_device *device = work->device;

    work->result = result_success;
    if (device->present_queue == NULL || !(device->header.features & reznor_device_feature_swapchain_support)) {
        log_error("Requested to create a Vulkan swapchain, but the device '%s' has no swapchain support.", 
            device->physical->properties2.properties.deviceName);
        work->result = result_error;
        return;
    }
#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->swapchain_count, work->assembly_count, memory_order_acquire);
#endif /* REZNOR_ENABLE_GPU_PROFILER */

    for (u32 i = 0; i < work->assembly_count; i++) {
        struct reznor_swapchain_assembly *assembly = &work->assembly.swapchain[i];
        struct reznor_swapchain *swapchain = (struct reznor_swapchain *)&work->memory.raw[sizeof(struct reznor_swapchain) * i];

        swapchain->header.debug_name = assembly->header.debug_name;
        swapchain->header.device = device;
        swapchain->header.type = reznor_resource_type_swapchain;
        swapchain->window = assembly->window;
        assert_debug(swapchain->window != NULL);

        VkResult res = swapchain_assembly(device, swapchain, NULL, assembly->preferred_format, assembly->enable_vsync, true);

        if (res != VK_SUCCESS) {
            struct hadal_window_header *window = (struct hadal_window_header *)assembly->window;

            log_error("Creating a Vulkan swapchain '%s' failed for device '%s' and window '%s': %s.", 
                swapchain->header.debug_name.ptr, device->physical->properties2.properties.deviceName, window->title.ptr, vulkan_result_string(res));

            _reznor_vulkan_swapchain_disassembly(swapchain);
            work->result = res;
            continue;
        }
        assembly->header.output.swapchain = swapchain;
    }
}

FN_REZNOR_TRY_RECREATE_SWAPCHAIN(vulkan)
{
    assert_debug(swapchain && swapchain->header.device);
    struct reznor_device *device = swapchain->header.device;
    u32 flags = atomic_load_explicit(&swapchain->header.flags, memory_order_acquire);
    assert_debug(flags & reznor_swapchain_flag_is_valid);

    device->vkDeviceWaitIdle(device->logical);

    VkSwapchainKHR old_swapchain = swapchain->swapchain;
    for (u32 i = 0; i < swapchain->image_count; i++) {
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->logical, swapchain->image_views[i], &device->host_allocator);
        swapchain->image_views[i] = VK_NULL_HANDLE;
    }
    swapchain->swapchain = VK_NULL_HANDLE;

    b32 enable_vsync = (flags & reznor_swapchain_flag_vsync_enabled) ? true : false;
    b32 touch_surface = (flags & reznor_swapchain_flag_surface_was_lost) ? true : false || !swapchain->surface;

    VkResult res = swapchain_assembly(device, swapchain, old_swapchain, reznor_texture_format_unknown, enable_vsync, touch_surface);
    if (res != VK_SUCCESS) {
        struct hadal_window_header *window = (struct hadal_window_header *)swapchain->window;

        log_error("Recreating a Vulkan swapchain '%s' failed for device '%s' and window '%s': %s.", 
            swapchain->header.debug_name.ptr, device->physical->properties2.properties.deviceName, window->title.ptr, vulkan_result_string(res));

        _reznor_vulkan_swapchain_disassembly(swapchain);
    }
    if (old_swapchain)
        device->vkDestroySwapchainKHR(device->logical, old_swapchain, &device->host_allocator);
}

FN_REZNOR_DISASSEMBLY(vulkan, swapchain) { 
    assert_debug(swapchain && swapchain->header.device);
    struct reznor_device *device = swapchain->header.device;
    if (!device)
        return;

    u32 flags = atomic_load_explicit(&swapchain->header.flags, memory_order_acquire);
    assert_debug(flags & reznor_swapchain_flag_is_valid);

    device->vkDeviceWaitIdle(device->logical);

    if (flags & reznor_swapchain_flag_close_window_on_destroy) {
        struct hadal_window_header *window = (struct hadal_window_header *)swapchain->window;
        if (window) {
            window->swapchain = NULL;
            atomic_fetch_or_explicit(&window->flags, hadal_window_flag_should_close, memory_order_release);
        }
    }
    swapchain->window = NULL;

    for (u32 i = 0; i < device->header.frames_in_flight; i++)
        if (swapchain->image_available_semaphores[i] != VK_NULL_HANDLE)
            device->vkDestroySemaphore(device->logical, swapchain->image_available_semaphores[i], &device->host_allocator);

    for (u32 i = 0; i < swapchain->image_count; i++)
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->logical, swapchain->image_views[i], &device->host_allocator);

    if (swapchain->swapchain != VK_NULL_HANDLE)
        device->vkDestroySwapchainKHR(device->logical, swapchain->swapchain, &device->host_allocator);
    if (swapchain->surface != VK_NULL_HANDLE)
        device->header.reznor->vkDestroySurfaceKHR(device->header.reznor->instance, swapchain->surface, NULL);
#if REZNOR_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->swapchain_count, 1u, memory_order_release);
#endif /* REZNOR_ENABLE_GPU_PROFILER */
    zerop(swapchain);
}
