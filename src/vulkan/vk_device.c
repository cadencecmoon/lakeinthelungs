#include "vk_pelagia.h"

#include <amw/log.h>

struct physical_device_query_work {
    struct work_header              header;
    const struct pelagia           *pelagia;
    const struct hadal             *hadal;
    struct vulkan_physical_device  *device;
};

static void physical_device_query(struct physical_device_query_work *work)
{
    struct vulkan_physical_device *physical = work->device;
    assert_debug(physical->device);

    const struct pelagia *pelagia = work->pelagia;
    VkSurfaceFormatKHR *surface_formats;
    VkPresentModeKHR *present_modes;
    VkVideoFormatPropertiesKHR *video_format_properties;
    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties *extension_properties;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    return;

    u32 api_version = 0;
    u32 surface_format_count = 0;
    u32 present_mode_count = 0;
    u32 extension_count = 0;
    u32 video_format_properties_count = 0;
    u32 queue_family_count = 0;
    b32 has_vk13 = api_version >= VK_API_VERSION_1_3;
    b32 has_vk14 = api_version >= VK_API_VERSION_1_4;

    pelagia->vkGetPhysicalDeviceProperties(physical->device, &physical->properties2.properties);
    struct str formated_err = str_null;
    riven_format_string(pelagia->interface.header.riven, riven_tag_deferred, &formated_err, "Vulkan physical device '%s'", physical->properties2.properties.deviceName);
    const char *err = formated_err.ptr;

    /* resolve driver versions */
    api_version = physical->properties2.properties.apiVersion;
    if (api_version < VK_API_VERSION_1_2) {
        log_error("%s too old drivers - found API version %u.%u.%u, we target atleast 1.2.X.",
            err, (api_version >> 22u), (api_version >> 12u) & 0x3ffu, api_version & 0xfffu);
        return;
    }

    /* resolve presence of queue families */
    pelagia->vkGetPhysicalDeviceQueueFamilyProperties2(physical->device, &queue_family_count, NULL);
    if (queue_family_count == 0) {
        log_error("%s has no queue families.", err);
        return;
    }

    /* resolve presence of device extensions */
    VERIFY_VK(pelagia->vkEnumerateDeviceExtensionProperties(physical->device, NULL, &extension_count, NULL));
    if (extension_count == 0) {
        log_error("%s has zero Vulkan extensions.", err);
        return;
    }

    if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        physical->score += 1000;
    else if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        physical->score += 250;

    physical->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    physical->properties_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES;
    physical->properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    physical->properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    physical->properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    physical->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    physical->raytracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    physical->descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    physical->fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    physical->mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;

    physical->properties2.pNext = has_vk14 ? &physical->properties_14 : (has_vk13 ? (void *)&physical->properties_13 : (void *)&physical->properties_12);
    physical->properties_14.pNext = &physical->properties_13;
    physical->properties_13.pNext = &physical->properties_12;
    physical->properties_12.pNext = &physical->properties_11;
    physical->properties_11.pNext = &physical->acceleration_structure_properties;
    physical->acceleration_structure_properties.pNext = &physical->raytracing_pipeline_properties;
    physical->raytracing_pipeline_properties.pNext = &physical->descriptor_indexing_properties;
    physical->descriptor_indexing_properties.pNext = &physical->fragment_shading_rate_properties;
    physical->fragment_shading_rate_properties.pNext = &physical->mesh_shader_properties;

    physical->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physical->memory_properties2.pNext = &physical->memory_budget;
    physical->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

    physical->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical->features_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    physical->features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    physical->features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    physical->features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    physical->acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    physical->raytracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    physical->ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    physical->descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    physical->dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    physical->dynamic_rendering_local_read_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
    physical->synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    physical->fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
    physical->mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    physical->features2.pNext = has_vk14 ? &physical->features_14 : (has_vk13 ? (void *)&physical->features_13 : (void *)&physical->properties_12);
    physical->features_14.pNext = &physical->features_13;
    physical->features_13.pNext = &physical->features_12;
    physical->features_12.pNext = &physical->features_11;
    physical->features_11.pNext = &physical->acceleration_structure_features;
    physical->acceleration_structure_features.pNext = &physical->raytracing_pipeline_features;
    physical->raytracing_pipeline_features.pNext = &physical->ray_query_features;
    physical->ray_query_features.pNext = &physical->descriptor_indexing_features;
    physical->descriptor_indexing_features.pNext = &physical->dynamic_rendering_features;
    physical->dynamic_rendering_features.pNext = &physical->dynamic_rendering_local_read_features;
    physical->dynamic_rendering_local_read_features.pNext = &physical->synchronization2_features;
    physical->synchronization2_features.pNext = &physical->fragment_shading_rate_features;
    physical->fragment_shading_rate_features.pNext = &physical->mesh_shader_features;

    pelagia->vkGetPhysicalDeviceProperties2(physical->device, &physical->properties2);
    pelagia->vkGetPhysicalDeviceMemoryProperties2(physical->device, &physical->memory_properties2);
    pelagia->vkGetPhysicalDeviceFeatures2(physical->device, &physical->features2);

    /* if the display is present, we will make an effort to check for presentation support */
    if (work->hadal) {
        /* let's create a temporary surface we can work with */
        /* TODO */

        VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical->device, surface, &physical->surface_capabilities));
        VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfaceFormatsKHR(physical->device, surface, &surface_format_count, NULL));
        VERIFY_VK(pelagia->vkGetPhysicalDeviceSurfacePresentModesKHR(physical->device, surface, &present_mode_count, NULL));
    }
    VkPhysicalDeviceVideoFormatInfoKHR video_format = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR,
        .pNext = NULL,
        .imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR,
    };
    VERIFY_VK(pelagia->vkGetPhysicalDeviceVideoFormatPropertiesKHR(physical->device, &video_format, &video_format_properties_count, NULL));

    usize surface_formats_bytes = A16(sizeof(VkSurfaceFormatKHR) * surface_format_count);
    usize present_modes_bytes = A16(sizeof(VkPresentModeKHR) * present_mode_count);
    usize video_format_properties_bytes = A16(sizeof(VkVideoFormatPropertiesKHR) * video_format_properties_count);
    usize queue_family_video_properties_bytes = A16(sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count);
    usize queue_family_properties_bytes = A16(sizeof(VkQueueFamilyProperties2) * queue_family_count);
    usize extension_properties_bytes = A16(sizeof(VkExtensionProperties) * extension_count);
    usize total_bytes = 
        surface_formats_bytes +
        present_modes_bytes +
        video_format_properties_bytes +
        queue_family_video_properties_bytes +
        queue_family_properties_bytes +
        extension_properties_bytes;

    u8 *raw = (u8 *)riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, total_bytes, 16);
    memset(raw, 0, total_bytes);

    usize o = 0;
    surface_formats = (VkSurfaceFormatKHR *)&raw[o];
    o += surface_formats_bytes;
    present_modes = (VkPresentModeKHR *)&raw[o];
    o += present_modes_bytes;
    video_format_properties = (VkVideoFormatPropertiesKHR *)&raw[o];
    o += video_format_properties_bytes;
    queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)&raw[o];
    o += queue_family_properties_bytes;
    queue_family_properties2 = (VkQueueFamilyProperties2 *)&raw[o];
    o += queue_family_properties_bytes;
    extension_properties = (VkExtensionProperties *)&raw[o];
    assert_debug(o + extension_properties_bytes == total_bytes);

    /* validate the device ;3 */
    work->header.result = result_success;
}

s32 _pelagia_vulkan_query_physical_devices(
    struct pelagia                      *pelagia, 
    const struct hadal                  *hadal,
    u32                                 *out_device_count, 
    struct pelagia_physical_device_info *out_devices)
{
    assert_debug(pelagia && out_device_count);

    if (!pelagia->physical_device_count) {
        u32 physical_device_count = 0;
        VERIFY_VK(pelagia->vkEnumeratePhysicalDevices(pelagia->instance, &physical_device_count, NULL));
        assert_debug(physical_device_count); /* we checked the devices before at encore */

        usize handle_bytes = A16(sizeof(VkPhysicalDevice) * physical_device_count);
        usize physical_device_bytes = A16(sizeof(struct vulkan_physical_device) * physical_device_count); 
        usize riven_work_bytes = A16(sizeof(struct riven_work) * physical_device_count); 
        usize query_work_bytes = A16(sizeof(struct physical_device_query_work) * physical_device_count);
        usize index_bytes = A16(sizeof(u32) * physical_device_count);
        usize total_bytes = handle_bytes + physical_device_bytes + riven_work_bytes + query_work_bytes + index_bytes;

        u8 *raw = (u8 *)riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, total_bytes, 16);
        memset(raw, 0, total_bytes);

        usize o = 0;
        VkPhysicalDevice *handles = (VkPhysicalDevice *)&raw[o];
        VERIFY_VK(pelagia->vkEnumeratePhysicalDevices(pelagia->instance, &physical_device_count, handles));
        o += handle_bytes;
        struct vulkan_physical_device *physical_devices = (struct vulkan_physical_device *)&raw[o]; 
        o += physical_device_bytes;
        struct riven_work *work = (struct riven_work *)&raw[o];
        o += riven_work_bytes;
        struct physical_device_query_work *query_work = (struct physical_device_query_work *)&raw[o];
        o += query_work_bytes;
        u32 *indices = (u32 *)&raw[o];
        assert_debug(o + index_bytes == total_bytes);

        for (u32 i = 0; i < physical_device_count; i++) {
            physical_devices[i].device = handles[i];
            work[i].name = str_init("vulkan:physical_device_query");
            work[i].argument = &query_work[i];
            work[i].procedure = (PFN_riven_work)physical_device_query;
            query_work[i].header.result = result_error;
            query_work[i].header.index = i;
            query_work[i].pelagia = pelagia;
            query_work[i].hadal = hadal;
            query_work[i].device = &physical_devices[i];
        }
        riven_split_work_and_unchain(pelagia->interface.header.riven, work, physical_device_count);

        o = 0;
        /* collect indices */
        for (u32 i = 0; i < pelagia->physical_device_count; i++)
            if (query_work[i].header.result == result_success) indices[++o] = i; 
        if (o == 0) {
            log_error("Vulkan device query processed %u physical devices, and invalidated all of them. Can't continue.", physical_device_count);
            return result_error;
        }

        /* write the accepted physical devices into the backend state */
        struct vulkan_physical_device *accepted = (struct vulkan_physical_device *)
            riven_alloc(pelagia->interface.header.riven, pelagia->interface.header.tag, sizeof(struct vulkan_physical_device) * o, _Alignof(struct vulkan_physical_device));
        for (u32 i = 0; i < o; i++)
            memcpy(&accepted[i], &physical_devices[indices[i]], sizeof(struct vulkan_physical_device));

        if (!out_devices) {
            *out_device_count = pelagia->physical_device_count = o;
            return result_success;
        }
        assert_debug(*out_device_count >= pelagia->physical_device_count);
    }
    assert_debug(out_devices);

    /* write the physical device info */
    for (u32 i = 0; i < pelagia->physical_device_count; i++) {
        struct pelagia_physical_device_info *out = &out_devices[i];
        const struct vulkan_physical_device *info = &pelagia->physical_devices[i]; 

        out->name.ptr = info->properties2.properties.deviceName;
        out->name.length = strnlen(info->properties2.properties.deviceName, 256);
        out->device_type = info->properties2.properties.deviceType;
        out->vendor_id = info->properties2.properties.vendorID;
        out->api_version = info->properties2.properties.apiVersion;
        out->total_vram = info->total_vram;
        out->score = info->score;
        out->presentation = info->presentation_support;
    }

    *out_device_count = pelagia->physical_device_count;
    return result_error;
}

void _pelagia_vulkan_create_device(struct pelagia_device_create_info *create_info)
{
    log_warn("create device lol");
    (void)create_info;
}

void _pelagia_vulkan_destroy_device(struct pelagia_device *device)
{
    (void)device;
}
