#include "vk_pelagia.h"

#include <amw/hadal.h>

void _pelagia_vulkan_create_swapchains(struct pelagia_swapchains_create_info *create_info) 
{ 
    struct pelagia_device *device = create_info->device;
    const struct pelagia *pelagia = device->pelagia;

    assert_debug(device && create_info->windows && create_info->write);

    if ((device->physical->extension_bits & vulkan_ext_swapchain_bit) == 0) {
        log_error("Can't create a swapchain - device '%s' does not the swapchain extension.", device->physical->properties2.properties.deviceName);
        create_info->work_header.result = result_error;
        return;
    }
    assert_debug(create_info->write_count);
    assert_debug(device->vkCreateSwapchainKHR);

    /* write the memory requirements, return if query */
    create_info->allocation.size = sizeof(struct pelagia_swapchain) * create_info->write_count;
    create_info->allocation.alignment = _Alignof(struct pelagia_swapchain);
    *create_info->write = NULL;

    if (create_info->allocation.tag == riven_tag_invalid && !create_info->allocation.memory) {
        assert_debug(create_info->work_header.result != result_allocation_query);
        create_info->work_header.result = result_allocation_query;
        return;
    }
    create_info->work_header.result = result_error;

    /* create the swapchains one at a time */
    for (u32 index = 0; index < create_info->write_count; index++) {
        const struct hadal_window *window = (const struct hadal_window *)create_info->windows[index];

    }

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSwapchainKHR sc = VK_NULL_HANDLE;

    VkSurfaceFormatKHR *surface_formats = NULL;
    u32 surface_format_count = 0;
    VkFormat image_format = VK_FORMAT_UNDEFINED;

    VkPresentModeKHR *present_modes = NULL;
    u32 present_mode_count = 0;
    VkPresentModeKHR no_vsync = VK_PRESENT_MODE_FIFO_KHR;

    /* create the surface */
    VkResult res = hadal->vulkan_create_surface((const struct hadal *)hadal, pelagia->instance, &surface, NULL, device->pelagia->vkGetInstanceProcAddr);

    if (res != VK_SUCCESS || !surface) {
        log_error("Creating a swapchain '%s' surface failed: %s.", hadal->header.name.ptr, vulkan_result_string(res));
        return;
    }
    VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical->device, surface, &surface_capabilities));
    
    res = pelagia->vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical->device, surface, &surface_format_count, NULL);
    if (!surface_format_count) {
        log_error("Can't acquire swapchain '%s' surface formats: %s.", hadal->header.name.ptr, vulkan_result_string(res));
        pelagia->vkDestroySurfaceKHR(pelagia->instance, surface, NULL);
        return;
    }
    res = pelagia->vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical->device, surface, &present_mode_count, NULL);
    if (!present_mode_count) {
        log_error("Can't acquire present modes for a swapchain '%s' surface: %s.", hadal->header.name.ptr, vulkan_result_string(res));
        pelagia->vkDestroySurfaceKHR(pelagia->instance, surface, NULL);
        return;
    }

    usize surface_format_bytes = sizeof(VkSurfaceFormatKHR) * surface_format_count;
    usize present_mode_bytes = sizeof(VkPresentModeKHR) * present_mode_count;
    u8 *raw = (u8 *)riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, surface_format_bytes + present_mode_bytes, 4);
    surface_formats = (VkSurfaceFormatKHR *)raw;
    present_modes = (VkPresentModeKHR *)&raw[surface_format_bytes];

    VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical->device, surface, &surface_format_count, surface_formats));
    VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical->device, surface, &present_mode_count, present_modes));

    VkFormat image_formats[] = {
        vulkan_translate_texture_format(create_info->preferred_format),
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SRGB,
    };

    /* select an appropriate image format */
    if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
        image_format = VK_FORMAT_B8G8R8A8_UNORM;
    for (u32 i = 0; i < arraysize(image_formats) && image_format == VK_FORMAT_UNDEFINED; i++) {
        VkFormat format = image_formats[i];

        for (u32 j = 0; j < surface_format_count; j++) {
            if (surface_formats[j].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                continue;
            if (surface_formats[j].format == format) {
                image_format = format;
                break;
            }
        }
    }

    /* select a present mode */
    for (u32 i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && no_vsync == VK_PRESENT_MODE_FIFO_KHR)
            no_vsync = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            no_vsync = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    if (no_vsync == VK_PRESENT_MODE_FIFO_KHR && !create_info->enable_vsync)
        log_debug("For '%s' swapchain present mode V-sync is forced.", device->physical->properties2.properties.deviceName);

    /* set the resolution */
    u32 fb_width = 0;
    u32 fb_height = 0;
    hadal->acquire_framebuffer_extent((struct hadal *)hadal, &fb_width, &fb_height);
    VkExtent2D extent = {
        (surface_capabilities.currentExtent.width != UINT32_MAX) ? surface_capabilities.currentExtent.width : fb_width,
        (surface_capabilities.currentExtent.height != UINT32_MAX) ? surface_capabilities.currentExtent.height : fb_height,
    };

    /* find a supported composite alpha (one of these is guaranteed to be set) */
    VkCompositeAlphaFlagsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagsKHR composite_alpha_flags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (u32 i = 0; i < arraysize(composite_alpha_flags); i++) {
        if (surface_capabilities.supportedCompositeAlpha & composite_alpha_flags[i]) {
            composite_alpha = composite_alpha_flags[i];
            break;
        }
    }

    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        /* TODO screenshot is supported */
    }
    VkSharingMode sharing_mode = VK_SHARING_MODE_CONCURRENT;
    if (device->physical->queue_family_count == 1 || 
       (device->physical->graphics_queue_family_index == device->physical->presentation_queue_family_index && 
        device->physical->transfer_queue_family_index == device->physical->presentation_queue_family_index)) 
    {
        sharing_mode = VK_SHARING_MODE_EXCLUSIVE; 
    }

    u32 min_image_count = surface_capabilities.minImageCount;
    u32 max_image_count = surface_capabilities.maxImageCount;
    u32 image_count = device->frames_in_flight;
    if (image_count < min_image_count)
        image_count = min(min_image_count, PELAGIA_MAX_SWAPCHAIN_IMAGES);
    if (max_image_count && image_count > max_image_count)
        image_count = max_image_count;

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = image_format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = image_usage,
        .imageSharingMode = sharing_mode,
        .queueFamilyIndexCount = device->physical->queue_family_count,
        .pQueueFamilyIndices = device->physical->queue_family_indices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = composite_alpha,
        .presentMode = create_info->enable_vsync ? VK_PRESENT_MODE_FIFO_KHR : no_vsync,
        .clipped = VK_FALSE,
        .oldSwapchain = NULL,
    };

    res = device->vkCreateSwapchainKHR(device->logical, &swapchain_info, NULL, &sc);
    if (res != VK_SUCCESS) {
        log_error("Failed to create a Vulkan swapchain: %s.", vulkan_result_string(res));
        pelagia->vkDestroySurfaceKHR(pelagia->instance, surface, NULL);
        return;
    }

    /* allocate the swapchain */
    if (!create_info->allocation.memory)
        create_info->allocation.memory = riven_alloc(pelagia->interface.header.riven, create_info->allocation.tag, create_info->allocation.size, create_info->allocation.alignment);
    struct pelagia_swapchain *swapchain = (struct pelagia_swapchain *)create_info->allocation.memory;
    zerop(swapchain);

    swapchain->header = (struct pelagia_resource_header){
        .device = device,
        .type = pelagia_resource_type_swapchain,
        .flags = 0,
    };
    swapchain->window = window;
    swapchain->sc = sc;
    swapchain->surface = surface;
    swapchain->no_vsync_present_mode = no_vsync;
    swapchain->composite_alpha = composite_alpha;
    swapchain->extent = extent;
    swapchain->sharing_mode = sharing_mode;
    swapchain->image_usage = image_usage;
    swapchain->image_format = image_format;
    swapchain->image_count = image_count;
    swapchain->presentation_interval = create_info->presentation_interval;

    /* get swapchain images and create views */
    VERIFY_VK(device->vkGetSwapchainImagesKHR(device->logical, sc, &swapchain->image_count, swapchain->images));
    for (u32 i = 0; i < swapchain->image_count; i++) {
        VkImageViewCreateInfo color_image_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->images[i],
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
        VERIFY_VK(device->vkCreateImageView(device->logical, &color_image_view, NULL, &swapchain->image_views[i]));
    }

    /* create image available semaphores */
    VkSemaphoreCreateInfo sem_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    for (u32 i = 0; i < device->frames_in_flight; i++)
        VERIFY_VK(device->vkCreateSemaphore(device->logical, &sem_info, NULL, &swapchain->image_available_sem[i]));

    /* write results */
    create_info->work_header.result = result_success;
    *create_info->write = swapchain;
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_add_explicit(&device->swapchain_count, 1, memory_order_acquire);
#endif
}

void vulkan_destroy_swapchain(struct pelagia_swapchain *swapchain) {
    struct pelagia_device *device = swapchain->header.device;

    device->vkQueueWaitIdle(device->presentation_queue);

    for (u32 i = 0; i < device->frames_in_flight; i++)
        if (swapchain->image_available_sem[i] != VK_NULL_HANDLE)
            device->vkDestroySemaphore(device->logical, swapchain->image_available_sem[i], NULL);

    for (u32 i = 0; i < swapchain->image_count; i++)
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            device->vkDestroyImageView(device->logical, swapchain->image_views[i], NULL);

    if (swapchain->sc != VK_NULL_HANDLE)
        device->vkDestroySwapchainKHR(device->logical, swapchain->sc, NULL);
    if (swapchain->surface != VK_NULL_HANDLE)
        device->pelagia->vkDestroySurfaceKHR(device->pelagia->instance, swapchain->surface, NULL);
    zerop(swapchain);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->swapchain_count, 1u, memory_order_release);
#endif
}
