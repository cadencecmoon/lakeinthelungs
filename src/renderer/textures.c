#include <lake/pelagia.h>
#include <lake/vulkan.h>

AMWAPI void vulkan_print_texture_requests(const struct vulkan_texture_request *texture_requests, u32 texture_count)
{
    log_debug("A description of each requested texture follows:");
    for (u32 i = 0; i < texture_count; i++) {
        const VkImageCreateInfo *image_info = &texture_requests[i].image_info;
        u32 mip_map_count = image_info->mipLevels;
        if (mip_map_count == 0)
            mip_map_count = vulkan_get_mipmap_count_3d(image_info->extent);
        log_debug("    %u: %ux%ux%u, %u layers, %u mipmaps, format %d.",
            i, image_info->extent.width, image_info->extent.height, image_info->extent.depth,
            image_info->arrayLayers, mip_map_count, image_info->format);
    }
}

AMWAPI void vulkan_destroy_textures(struct vulkan_textures *textures, struct vulkan_device *device)
{
    for (u32 i = 0; i < textures->image_count; i++) {
        struct vulkan_texture *texture = &textures->textures[i];
        if (texture->view) 
            device->api.vkDestroyImageView(device->logical, texture->view, NULL); /* TODO vk->allocator */
        if (texture->image)
            device->api.vkDestroyImage(device->logical, texture->image, NULL); /* TODO vk->allocator */
    }
    free(textures->textures); /* TODO ia */

    /* free the memory allocations */
    /* TODO gpu allocator */

    /* clear the object */
    iazerop(textures);
}

AMWAPI s32 vulkan_create_textures(
    struct vulkan_textures                *textures,
    struct vulkan_device                  *device,
    const struct vulkan_texture_request   *texture_requests,
    u32                                    texture_count,
    const struct vulkan_allocation_request allocation_request)
{
    (void)textures;
    (void)device;
    (void)texture_requests;
    (void)texture_count;
    (void)allocation_request;

    return result_error_undefined; /* TODO */
}

AMWAPI void pelagia_assemble_render_targets(struct pelagia_assemble_render_targets_work *work)
{
    if (!work) {
        return;
    } else if (!work->pelagia || at_read_relaxed(&work->pelagia->flags) & pelagia_flag_initialized) {
        work->out_result = result_error_invalid_engine_context;
        return;
    }

    struct pelagia         *pelagia        = work->pelagia;
    struct vulkan_textures *render_targets = &pelagia->render_targets;
    struct vulkan_device   *primary_device = &pelagia->devices[0];
    struct arena_allocator *scratch_arena  = &pelagia->scratch_arena;
    u32                     image_count    = pelagia->frames_buffering;

    struct vulkan_texture_request requests[render_target_type_count];
    u32 idx;

    if (image_count <= 1) {
        log_error("Trying to create render targets, %u is not a valid swapchain image count.", image_count);
        work->out_result = result_error_invalid_engine_context;
        return;
    } else if ((at_read_relaxed(&primary_device->flags) & (pelagia_device_flag_primary | pelagia_device_flag_is_valid))) {
        s32 flags = at_read_relaxed(&primary_device->flags);
        log_error("Trying to create render targets, can't validate flags (primary:%u | is_valid:%u) of the expected primary device (idx 0).",
            flags & pelagia_device_flag_primary ? 1 : 0, flags & pelagia_device_flag_is_valid ? 1 : 0);
        work->out_result = result_error_invalid_engine_context;
        return;
    }

    /* cleanup the render targets */
    vulkan_destroy_textures(render_targets, primary_device);
    if (work->dissasemble) return;

    /* setup default values */
    for (idx = 0; idx < render_target_type_count; idx++) {
        requests[idx].image_info = (VkImageCreateInfo){
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .extent = {pelagia->swapchain.extent.width, pelagia->swapchain.extent.height, 1},
            .mipLevels = 1, 
            .arrayLayers = 1,
            .samples = 1,
        };
        requests[idx].view_info = (VkImageViewCreateInfo){
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
        };
    }

    idx = render_target_type_depth_buffer;
    requests[idx].image_info.format = VK_FORMAT_D32_SFLOAT;
    requests[idx].image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    requests[idx].view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    idx = render_target_type_visibility_buffer;
    requests[idx].image_info.format = VK_FORMAT_R32_UINT;
    requests[idx].image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    requests[idx].view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    /* duplicate the image requests per swapchain image */
    struct vulkan_texture_request *all_requests = (struct vulkan_texture_request *)
        arena_alloc(scratch_arena, sizeof(requests) * image_count);
    for (idx = 0; idx < image_count; idx++)
        memcpy(all_requests + idx * image_count, requests, sizeof(requests));

    work->out_result = result_error_undefined; /* TODO */
}
