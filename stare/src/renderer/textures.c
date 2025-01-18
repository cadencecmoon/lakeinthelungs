/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/pelagia.h>
#include <lake/vulkan.h>

void vulkan_print_texture_requests(
    const VkImageCreateInfo     *image_infos, 
    const VkImageViewCreateInfo *view_infos, 
    u32                          texture_count)
{
    log_debug("A description of each requested texture follows:");
    for (u32 i = 0; i < texture_count; i++) {
        const VkImageCreateInfo *image_info = &image_infos[i];
        u32 mip_map_count = image_info->mipLevels;
        if (mip_map_count == 0)
            mip_map_count = vulkan_get_mipmap_count_3d(image_info->extent);
        log_debug("    %u: %ux%ux%u, %u layers, %u mipmaps, format %d.",
            i, image_info->extent.width, image_info->extent.height, image_info->extent.depth,
            image_info->arrayLayers, mip_map_count, image_info->format);
    }
    (void)view_infos;
}

void vulkan_destroy_textures(
    struct vulkan_texture *textures, 
    struct vulkan_device  *device,
    struct vulkan_backend *vk)
{
    (void)vk;
    u32 texture_count = 0;

    if (!textures) return;

    for (u32 i = 0; i < texture_count; i++) {
        struct vulkan_texture *texture = &textures[i];
        if (texture->view) 
            device->api.vkDestroyImageView(device->logical, texture->view, NULL); /* TODO vk->allocator */
        if (texture->image)
            device->api.vkDestroyImage(device->logical, texture->image, NULL); /* TODO vk->allocator */
    }
    free(textures); /* TODO ia */
}

s32 vulkan_create_textures(
    struct vulkan_texture                 *textures,
    struct vulkan_device                  *device,
    u32                                    texture_count)
{
    (void)textures;
    (void)device;
    (void)texture_count;

    return result_error_undefined; /* TODO */
}

void pelagia_assemble_render_targets(struct pelagia_assemble_render_targets_work *work)
{
    PELAGIA_WORK_INITIALIZED_OR_RETURN

    struct pelagia                *pelagia        = work->pelagia;
    struct pelagia_render_targets *render_targets = work->render_targets;
    struct vulkan_device          *device         = &pelagia->device;
    struct vulkan_backend         *vk             = &pelagia->vk;
    u32                            workload_count = pelagia->workload_buffering;

    VkImageCreateInfo image_infos[pelagia_render_target_type_count];
    VkImageViewCreateInfo view_infos[pelagia_render_target_type_count];
    (void)image_infos;
    (void)view_infos;
    (void)workload_count;

    /* cleanup the render targets */
    //vulkan_destroy_textures(render_targets->textures, device, vk);
    if (work->dissasemble) return;

    /* setup default values */
#if 0
    u32 idx;
    for (idx = 0; idx < pelagia_render_target_type_count; idx++) {
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

    idx = pelagia_render_target_type_depth_buffer;
    requests[idx].image_info.format = VK_FORMAT_D32_SFLOAT;
    requests[idx].image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    requests[idx].view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    idx = pelagia_render_target_type_visibility_buffer;
    requests[idx].image_info.format = VK_FORMAT_R32_UINT;
    requests[idx].image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    requests[idx].view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    /* duplicate the image requests per swapchain image */
    struct vulkan_texture_request *all_requests = (struct vulkan_texture_request *)
        arena_alloc(scratch_arena, sizeof(requests) * workload_count);
    for (idx = 0; idx < workload_count; idx++)
        iamemcpy(all_requests + idx * workload_count, requests, sizeof(requests));
#endif

    /* TODO */

    work->out_result = result_error_undefined; /* TODO */
}
