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

AMWAPI void vulkan_destroy_textures(struct vulkan_textures *textures, const struct vulkan_device *device)
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
