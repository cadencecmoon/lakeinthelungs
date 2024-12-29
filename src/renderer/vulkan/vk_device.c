#include <lake/bedrock/log.h>

#include "vk_platynova.h"

static const char *device_type_string(VkPhysicalDeviceType type)
{
#define PDTYPESTR(r) case VK_ ##r: return "VK_"#r;
    switch (type) {
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_OTHER)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_CPU)
        default: return "<unknown>";
    }
#undef PDTYPESTR
    return "UNKNOWN DEVICE";
}

static const char *vendor_name_string(uint32_t vendor_id)
{
    switch (vendor_id) {
        case 0x1002: return "AMD";
        case 0x1010: return "ImgTec";
        case 0x106B: return "Apple";
        case 0x10DE: return "NVIDIA";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8086: return "Intel";
        default: return "<unknown>";
    }
}

void _platinum_vulkan_select_physical_devices(
        void *internal_plat,
        void **out_internal_novas, 
        struct arena_allocator *temp_arena,
        uint32_t *out_gpu_count, 
        uint32_t max_gpu_count, 
        int32_t preferred_main_gpu)
{
    uint32_t pd_count = 0, nova_count = 0;
    int32_t main_gpu_idx = -1;
    struct platinum_vulkan *plat_vk = (struct platinum_vulkan *)internal_plat;

    VERIFY_VK(plat_vk->api.vkEnumeratePhysicalDevices(plat_vk->instance, &pd_count, NULL));
    if (pd_count == 0)
        return;
    VkPhysicalDevice *pds = (VkPhysicalDevice *)arena_alloc(temp_arena, sizeof(VkPhysicalDevice) * pd_count);
    VERIFY_VK(plat_vk->api.vkEnumeratePhysicalDevices(plat_vk->instance, &pd_count, pds));

    preferred_main_gpu = preferred_main_gpu < (int32_t)pd_count ? preferred_main_gpu : -1;
    max_gpu_count = min(max_gpu_count, pd_count);

    /* pd_idx (or -1) */
    int32_t *indices = (int32_t *)arena_alloc(temp_arena, sizeof(int32_t) * pd_count);
    uint32_t *extension_bits = (uint32_t *)arena_alloc(temp_arena, sizeof(uint32_t) * pd_count);

    for (int32_t i = 0; (i < (int32_t)pd_count) && (max_gpu_count == 0 ? true : max_gpu_count); i++) {
        VkPhysicalDeviceProperties pd_props;
        VkQueueFamilyProperties *queue_families;
        VkExtensionProperties *extensions;
        uint32_t queue_family_count = 0, extension_count = 0;

        indices[i] = -1;
        plat_vk->api.vkGetPhysicalDeviceProperties(pds[i], &pd_props);

        /* too old drivers? */
        if (pd_props.apiVersion < VK_MAKE_API_VERSION(0,1,1,0))
            continue;

        plat_vk->api.vkEnumerateDeviceExtensionProperties(pds[i], NULL, &extension_count, NULL);
        /* no extensions at all? */
        if (extension_count == 0)
            continue;

        plat_vk->api.vkGetPhysicalDeviceQueueFamilyProperties(pds[i], &queue_family_count, NULL);
        /* no queue properties? */
        if (queue_family_count == 0)
            continue;

        queue_families = (VkQueueFamilyProperties *)arena_alloc(temp_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        plat_vk->api.vkGetPhysicalDeviceQueueFamilyProperties(pds[i], &queue_family_count, queue_families);

        /* don't be picky */
        bool best_properties = pd_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        bool has_graphics = 0, has_compute = 0, has_transfer = 0;

        /* device is OK if there is at least one queue with VK_QUEUE_GRAPHICS_BIT and one queue with VK_QUEUE_COMPUTE_BIT set */
        for (uint32_t j = 0; j < queue_family_count; j++) {
            if (!has_graphics && queue_families[j].queueCount > 0 && (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                has_graphics = true;
            if (!has_compute && queue_families[j].queueCount > 0 && (queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT))
                has_compute = true;
            if (!has_transfer && queue_families[j].queueCount > 0 && (queue_families[j].queueFlags & VK_QUEUE_TRANSFER_BIT))
                has_transfer = true;
        }

        /* accept only devices who have support for drawing, compute and transfer */
        if (!has_graphics || !has_compute || !has_transfer)
            continue;

        extensions = (VkExtensionProperties *)arena_alloc(temp_arena, sizeof(VkExtensionProperties) * extension_count);
        plat_vk->api.vkEnumerateDeviceExtensionProperties(pds[i], NULL, &extension_count, extensions);

        for (uint32_t j = 0; j < extension_count; j++) {
            if (!strcmp(extensions[j].extensionName, "VK_KHR_swapchain")) {
                extension_bits[i] |= vulkan_extension_swapchain_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_KHR_display_swapchain")) {
                extension_bits[i] |= vulkan_extension_display_swapchain_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_EXT_device_fault")) {
                extension_bits[i] |= vulkan_extension_device_fault_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_EXT_memory_budget")) {
                extension_bits[i] |= vulkan_extension_memory_budget_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_EXT_memory_priority")) {
                extension_bits[i] |= vulkan_extension_memory_priority_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_AMD_shader_info")) {
                extension_bits[i] |= vulkan_extension_amd_shader_info_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_KHR_deferred_host_operations")) {
                extension_bits[i] |= vulkan_extension_deferred_host_operations_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_KHR_acceleration_structure")) {
                extension_bits[i] |= vulkan_extension_acceleration_structure_bit;
            } else if (!strcmp(extensions[j].extensionName, "VK_KHR_ray_query")) {
                extension_bits[i] |= vulkan_extension_ray_query_bit;
            }

            /* 1.4 core backwards compatibility */
            if (pd_props.apiVersion < VK_MAKE_API_VERSION(0,1,4,0)) {
                if (!strcmp(extensions[j].extensionName, "VK_KHR_dynamic_rendering_local_read")) {
                    extension_bits[i] |= vulkan_extension_dynamic_rendering_local_read_bit;
                }
            }
            /* 1.3 core backwards compatibility */
            if (pd_props.apiVersion < VK_MAKE_API_VERSION(0,1,3,0)) {
                if (!strcmp(extensions[j].extensionName, "VK_KHR_dynamic_rendering")) {
                    extension_bits[i] |= vulkan_extension_dynamic_rendering_bit;
                }
            }
            /* 1.2 core backwards compatibility */
            if (pd_props.apiVersion < VK_MAKE_API_VERSION(0,1,2,0)) {
                /* TODO */
            }
        }

        /* check if suitable for use as a main device */
        if (preferred_main_gpu == i || (best_properties && preferred_main_gpu < 0 && main_gpu_idx < 0) || pd_count == 1) {
            /* VkBool32 present_supported;
             * VERIFY_VK(plat_vk->api.vkGetPhysicalDeviceSurfaceSupportKHR(pds[i], has_graphics, plat_vk->swapchain.surface, &present_supported))
             * FIXME probably should check for presentation with the current hadal backend uh */

            indices[i] = main_gpu_idx = i;
            nova_count++;
        } else if (pd_count > 1 && max_gpu_count > 1) {
            indices[i] = i;
            nova_count++;
        }
    }

    /* no main gpu was selected for some reason? */
    if ((main_gpu_idx == -1) || nova_count == 0) {
        log_error("Could not query a physical device that serves our requirements.");
        return;
    }

    struct platynova_vulkan *novas_vk = (struct platynova_vulkan *)malloc(sizeof(struct platynova_vulkan) * nova_count);
    iamemset(novas_vk, 0u, sizeof(struct platynova_vulkan) * nova_count);

    /* assign the physical devices that will be used */
    for (uint32_t i = 0; i < nova_count; i++) {
        novas_vk[i].physical = VK_NULL_HANDLE;
        for (uint32_t j = 0; j < pd_count && novas_vk[i].physical == VK_NULL_HANDLE; j++) {
            int32_t idx = indices[(i == 0) ? main_gpu_idx : (int32_t)j];
            if (idx == -1) continue;

            novas_vk[i].physical = pds[idx];
            novas_vk[i].extensions = extension_bits[idx];
            indices[(i == 0) ? main_gpu_idx : (int32_t)j] = -1;
        }
        plat_vk->api.vkGetPhysicalDeviceProperties(pds[i], &novas_vk[i].physical_properties);
        plat_vk->api.vkGetPhysicalDeviceFeatures(pds[i], &novas_vk[i].physical_features);
        plat_vk->api.vkGetPhysicalDeviceMemoryProperties(pds[i], &novas_vk[i].memory_properties);
    }

    *out_gpu_count = nova_count;
    *out_internal_novas = (void *)novas_vk;
}

int32_t _platinum_vulkan_create_devices(
        void *internal_plat,
        void *internal_novas,
        struct arena_allocator *temp_arena,
        uint32_t nova_count,
        uint32_t thread_count)
{
    struct platinum_vulkan *plat_vk = (struct platinum_vulkan *)internal_plat;
    struct platynova_vulkan *novas_vk = (struct platynova_vulkan *)internal_novas;

    int32_t result = result_success;
    uint32_t pd_count = 0;
    uint32_t i;

    VERIFY_VK(plat_vk->api.vkEnumeratePhysicalDevices(plat_vk->instance, &pd_count, NULL));

    for (i = 0; i < nova_count; i++) {
        float queue_priorities[1] = { 0.0f };
        uint32_t j, queue_family_count = 0;
        uint32_t queue_create_info_count = 3;
        uint32_t first_compute_queue_idx = 0;
        uint32_t first_transfer_queue_idx = 0;
        uint32_t extension_count = 0;
        char **extensions = NULL;

        struct platynova_vulkan *nova = &novas_vk[i];

        log_info("\nCreating a rendering device (platynova) for GPU:\n"
                 "    Device name: %s\n"
                 "    Device type: %s, ID: %X\n"
                 "    Vendor: %s, ID: %X\n"
                 "    Api version: %u.%u.%u",
                 nova->physical_properties.deviceName,
                 device_type_string(nova->physical_properties.deviceType),
                 nova->physical_properties.deviceID,
                 vendor_name_string(nova->physical_properties.vendorID),
                 nova->physical_properties.vendorID,
                 (nova->physical_properties.apiVersion >> 22U),
                 (nova->physical_properties.apiVersion >> 12U) & 0x3ffU,
                 (nova->physical_properties.apiVersion & 0xfffU));

        arena_reset(temp_arena);
        VkPhysicalDevice *pds = (VkPhysicalDevice *)arena_alloc(temp_arena, sizeof(VkPhysicalDevice) * pd_count);
        VERIFY_VK(plat_vk->api.vkEnumeratePhysicalDevices(plat_vk->instance, &pd_count, pds));

        /* we already made sure the GPU in question supports the command queues we need */
        plat_vk->api.vkGetPhysicalDeviceQueueFamilyProperties(nova->physical, &queue_family_count, NULL);
        VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties *)arena_alloc(temp_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        plat_vk->api.vkGetPhysicalDeviceQueueFamilyProperties(nova->physical, &queue_family_count, queue_families);

        for (uint32_t j = 0; j < queue_family_count; j++) {
            /* ask for one independent graphics queue, don't be picky */
            if (nova->graphics_queue_count == 0 && queue_families[j].queueCount > 0 && (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                nova->graphics_queue_count = 1;
                nova->graphics_queue_family_idx = j;
            }

            /* ask for independent compute queue(s) */
            if (nova->compute_queue_count == 0 && queue_families[j].queueCount > 0 
                && (queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                && (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
            {
                nova->compute_queue_count = queue_families[j].queueCount;
                nova->compute_queue_family_idx = j;
            }

            /* ask for independent transfer queue(s) */
            if (nova->transfer_queue_count == 0 && queue_families[j].queueCount > 0 
                && (queue_families[j].queueFlags & VK_QUEUE_TRANSFER_BIT) 
                && (queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0
                && (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) 
            {
                nova->transfer_queue_count = min(queue_families[j].queueCount, 2);
                nova->transfer_queue_family_idx = j;

            /* otherwise ask for compute + transfer queues(s) */
            } else if (nova->compute_queue_count != 0 
                && nova->transfer_queue_count == 0 && queue_families[j].queueCount > 0 
                && (queue_families[j].queueFlags & VK_QUEUE_TRANSFER_BIT) 
                && (queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                && (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) 
            {
                nova->transfer_queue_count = 1;
                nova->transfer_queue_family_idx = j;

                /* my gpu (RX 7600) has a compute+transfer family for example.. */
                if (queue_families[j].queueCount > 1) {
                    nova->compute_queue_count = queue_families[j].queueCount - 1;
                    first_transfer_queue_idx = nova->compute_queue_count - 1; /* implied that compute family is different from graphics */
                } else {
                    nova->compute_queue_count = 0;
                }
            }
        }

        /* resolve compute queues, if no asynchronous compute families exist */
        if (nova->compute_queue_count == 0) {
            uint32_t idx = nova->graphics_queue_family_idx;
            uint32_t count = queue_families[idx].queueCount;
            nova->compute_queue_family_idx = idx;
            if (count > 1) {
                nova->compute_queue_count = count - 1;
                first_compute_queue_idx = nova->graphics_queue_count;
            } else {
                /* share the only available graphics + compute queue */
                nova->compute_queue_count = 1;
                first_compute_queue_idx = 0;
            }
        }

        /* resolve transfer queues, if no asynchronous transfer families exist */
        if (nova->transfer_queue_count == 0) {
            uint32_t count = queue_families[nova->transfer_queue_family_idx].queueCount;
            nova->transfer_queue_count = 1;
            nova->transfer_queue_family_idx = nova->compute_queue_family_idx;

            if (count > 2) {
                nova->compute_queue_count--;
                first_transfer_queue_idx = first_compute_queue_idx + nova->compute_queue_count;
            } else if (count > 1) {
                first_transfer_queue_idx = first_compute_queue_idx; /* if sharing the graphics queue, will be 0 */
            }
        }
        uint32_t queue_count = 0;

        /* resolve what queues are necessary to be created */
        VkDeviceQueueCreateInfo queue_create_infos[3];
        for (j = 0; j < 3; j++) {
            queue_create_infos[j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[j].pNext = NULL;
            queue_create_infos[j].flags = 0;
            queue_create_infos[j].pQueuePriorities = queue_priorities;
            queue_create_infos[j].queueCount = nova->graphics_queue_count;
            queue_create_infos[j].queueFamilyIndex = nova->graphics_queue_family_idx;
        }
        queue_create_infos[1].queueCount = nova->compute_queue_count;
        queue_create_infos[1].queueFamilyIndex = nova->compute_queue_family_idx;
        queue_create_infos[2].queueCount = nova->transfer_queue_count;
        queue_create_infos[2].queueFamilyIndex = nova->transfer_queue_family_idx;

        /* separate compute + transfer family */
        if (nova->transfer_queue_family_idx == nova->compute_queue_family_idx
            && nova->transfer_queue_family_idx != nova->graphics_queue_family_idx)
        {
            queue_count = min(queue_families[nova->compute_queue_family_idx].queueCount, 
                nova->compute_queue_count + nova->transfer_queue_count);
            queue_create_infos[1].queueCount = queue_count;
            queue_count += nova->graphics_queue_count;
            queue_create_info_count = 2;
        /* separate compute family */
        } else if (nova->transfer_queue_family_idx != nova->compute_queue_family_idx
            && nova->transfer_queue_family_idx == nova->graphics_queue_family_idx)
        {
            queue_count = min(queue_families[nova->transfer_queue_family_idx].queueCount, 
                nova->graphics_queue_count + nova->transfer_queue_count);
            queue_create_infos[0].queueCount = queue_count;
            queue_count += nova->compute_queue_count;
            queue_create_info_count = 2;
        /* one queue family that supports everything */
        } else if (nova->transfer_queue_family_idx == nova->compute_queue_family_idx
            && nova->transfer_queue_family_idx == nova->graphics_queue_family_idx
            && nova->compute_queue_family_idx == nova->graphics_queue_family_idx)
        {
            queue_count = min(queue_families[nova->graphics_queue_family_idx].queueCount, 
                nova->graphics_queue_count + nova->compute_queue_count + nova->transfer_queue_count);
            queue_create_infos[0].queueCount = queue_count;
            queue_create_info_count = 1;
        }

        /* validate state */
        assert_debug(nova->graphics_queue_count > 0);
        assert_debug(nova->compute_queue_count > 0);
        assert_debug(nova->transfer_queue_count > 0);

        /* enable the needed extensions */
        j = 0; extension_count = count_bits32(nova->extensions);
        extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
        if (nova->extensions & vulkan_extension_swapchain_bit)
            extensions[j++] = "VK_KHR_swapchain";
        if (nova->extensions & vulkan_extension_display_swapchain_bit)
            extensions[j++] = "VK_KHR_display_swapchain";
        if (nova->extensions & vulkan_extension_device_fault_bit)
            extensions[j++] = "VK_EXT_device_fault";
        if (nova->extensions & vulkan_extension_memory_budget_bit)
            extensions[j++] = "VK_EXT_memory_budget";
        if (nova->extensions & vulkan_extension_memory_priority_bit)
            extensions[j++] = "VK_EXT_memory_priority";
        if (nova->extensions & vulkan_extension_amd_shader_info_bit)
            extensions[j++] = "VK_AMD_shader_info";
        if (nova->extensions & vulkan_extension_deferred_host_operations_bit)
            extensions[j++] = "VK_KHR_deferred_host_operations";
        if (nova->extensions & vulkan_extension_acceleration_structure_bit)
            extensions[j++] = "VK_KHR_acceleration_structure";
        if (nova->extensions & vulkan_extension_ray_query_bit)
            extensions[j++] = "VK_KHR_ray_query";
        if (nova->extensions & vulkan_extension_dynamic_rendering_local_read_bit)
            extensions[j++] = "VK_KHR_dynamic_rendering_local_read";
        if (nova->extensions & vulkan_extension_dynamic_rendering_bit)
            extensions[j++] = "VK_KHR_dynamic_rendering";

        VkPhysicalDeviceFeatures physical_device_feats = {
            .shaderSampledImageArrayDynamicIndexing = VK_TRUE,
            .samplerAnisotropy = VK_TRUE,
            .fragmentStoresAndAtomics = VK_TRUE,
            .vertexPipelineStoresAndAtomics = VK_TRUE,
            .shaderInt64 = VK_TRUE,
        };
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_feats = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
            .pNext = NULL,
            .accelerationStructure = VK_TRUE,
        };
        VkPhysicalDeviceRayQueryFeaturesKHR ray_query_feats = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
            .pNext = &acceleration_structure_feats,
            .rayQuery = VK_TRUE,
        };
        VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR dynamic_rendering_local_read_feats = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES_KHR,
            .pNext = ((nova->extensions & vulkan_extension_mask_raytracing) 
                    == vulkan_extension_mask_raytracing) ? &ray_query_feats : NULL,
            .dynamicRenderingLocalRead = VK_TRUE,
        };
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feats = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = &dynamic_rendering_local_read_feats,
            .dynamicRendering = VK_TRUE,
        };
        VkPhysicalDeviceVulkan12Features vk12_feats = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &dynamic_rendering_feats,
            .descriptorIndexing = VK_TRUE,
            .uniformAndStorageBuffer8BitAccess = VK_TRUE,
            .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
            .bufferDeviceAddress = VK_TRUE,
            .timelineSemaphore = VK_TRUE,
        };
        VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &vk12_feats,
            .flags = 0,
            .pEnabledFeatures = &physical_device_feats,
            .ppEnabledExtensionNames = (const char * const *)extensions,
            .enabledExtensionCount = extension_count,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = NULL,
            .queueCreateInfoCount = queue_create_info_count,
            .pQueueCreateInfos = queue_create_infos,
        };

        /* continue on error, for the validation layers to seek errors with other devices */
        if (plat_vk->api.vkCreateDevice(nova->physical, &device_create_info, NULL, &nova->logical) != VK_SUCCESS) {
            log_error("Failed to create a Vulkan logical device !!");
            result = result_error_undefined; /* TODO */
            continue;
        }
        if ((result = vulkan_device_api_load_procedures(&plat_vk->api, &nova->api, nova->logical, nova->physical_properties.apiVersion, &nova->extensions)) != result_success)
            continue;

        /* grab the command queues */
        nova->graphics_queues = (VkQueue *)malloc(sizeof(VkQueue) * nova->graphics_queue_count);
        iamemset(nova->graphics_queues, 0u, sizeof(VkQueue) * nova->graphics_queue_count);
        for (j = 0; j < nova->graphics_queue_count; j++)
            nova->api.vkGetDeviceQueue(nova->logical, nova->graphics_queue_family_idx, j, &nova->graphics_queues[j]);
        nova->compute_queues = (VkQueue *)malloc(sizeof(VkQueue) * nova->compute_queue_count);
        iamemset(nova->compute_queues, 0u, sizeof(VkQueue) * nova->compute_queue_count);
        for (j = first_compute_queue_idx; j < nova->compute_queue_count; j++)
            nova->api.vkGetDeviceQueue(nova->logical, nova->compute_queue_family_idx, j, &nova->compute_queues[j]);
        nova->transfer_queues = (VkQueue *)malloc(sizeof(VkQueue) * nova->transfer_queue_count);
        iamemset(nova->transfer_queues, 0u, sizeof(VkQueue) * nova->transfer_queue_count);
        for (j = first_transfer_queue_idx; j < nova->transfer_queue_count; j++)
            nova->api.vkGetDeviceQueue(nova->logical, nova->transfer_queue_family_idx, j, &nova->transfer_queues[j]);

        /* we use 3 (PLATYNOVA_MAX_IMAGES) command pools (or 1 command pool for transfer)
         * per worker thread, per graphics/compute queue, per device */
        uint32_t graphics_command_pool_count = nova->graphics_queue_count * PLATYNOVA_MAX_IMAGES * thread_count;
        uint32_t compute_command_pool_count = nova->compute_queue_count * PLATYNOVA_MAX_IMAGES * thread_count;
        uint32_t transfer_command_pool_count = nova->transfer_queue_count * thread_count;

        /* create the command pools */
        nova->graphics_command_pools = (VkCommandPool *)malloc(sizeof(VkCommandPool) * graphics_command_pool_count);
        iamemset(nova->graphics_command_pools, 0u, sizeof(VkCommandPool) * graphics_command_pool_count);
        nova->compute_command_pools = (VkCommandPool *)malloc(sizeof(VkCommandPool) * compute_command_pool_count);
        iamemset(nova->compute_command_pools, 0u, sizeof(VkCommandPool) * compute_command_pool_count);
        nova->transfer_command_pools = (VkCommandPool *)malloc(sizeof(VkCommandPool) * transfer_command_pool_count);
        iamemset(nova->transfer_command_pools, 0u, sizeof(VkCommandPool) * transfer_command_pool_count);

        VkCommandPoolCreateInfo command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
        };
        command_pool_create_info.queueFamilyIndex = nova->graphics_queue_family_idx;
        for (j = 0; j < graphics_command_pool_count; j++)
            VERIFY_VK(nova->api.vkCreateCommandPool(nova->logical, &command_pool_create_info, NULL, &nova->graphics_command_pools[j]));
        command_pool_create_info.queueFamilyIndex = nova->compute_queue_family_idx;
        for (j = 0; j < compute_command_pool_count; j++)
            VERIFY_VK(nova->api.vkCreateCommandPool(nova->logical, &command_pool_create_info, NULL, &nova->compute_command_pools[j]));
        command_pool_create_info.queueFamilyIndex = nova->transfer_queue_family_idx;
        for (j = 0; j < transfer_command_pool_count; j++) {
            VERIFY_VK(nova->api.vkCreateCommandPool(nova->logical, &command_pool_create_info, NULL, &nova->transfer_command_pools[j]));
        }

        log_debug("Created %d unique command queues and %d command pools:\n"
                  "    graphics queues : %2d  graphics queue family idx : %d   command pools : %d\n"
                  "    compute queues  : %2d  compute queue family idx  : %d   command pools : %d\n"
                  "    transfer queues : %2d  transfer queue family idx : %d   command pools : %d", 
                queue_count, graphics_command_pool_count + compute_command_pool_count + transfer_command_pool_count,
                nova->graphics_queue_count, 
                nova->graphics_queue_family_idx, 
                graphics_command_pool_count,
                nova->compute_queue_count, 
                nova->compute_queue_family_idx, 
                compute_command_pool_count,
                nova->transfer_queue_count, 
                nova->transfer_queue_family_idx,
                transfer_command_pool_count);

#if defined(VK_KHR_acceleration_structure)
        /* retrieve acceleration structure properties if raytracing supported */
        if ((nova->extensions & vulkan_extension_mask_raytracing) == vulkan_extension_mask_raytracing) {
            nova->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
            VkPhysicalDeviceProperties2 device_properties2 = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
                .pNext = &nova->acceleration_structure_properties,
            };
            plat_vk->api.vkGetPhysicalDeviceProperties2(nova->physical, &device_properties2);
            log_info("This GPU supports hardware accelerated ray tracing.");
        }
#endif
    }

    if (result != result_success) {
        log_error("An error occured when creating rendering devices.");
        _platinum_vulkan_destroy_devices(internal_novas, nova_count, thread_count);
    }

    return result;
}

void _platinum_vulkan_destroy_devices(
        void *internal_novas,
        uint32_t nova_count,
        uint32_t thread_count)
{
    struct platynova_vulkan *novas_vk = (struct platynova_vulkan *)internal_novas;

    for (uint32_t i = 0; i < nova_count; i++) {
        struct platynova_vulkan *nova = &novas_vk[i];

        if (nova->logical == VK_NULL_HANDLE)
            continue;

        nova->api.vkDeviceWaitIdle(nova->logical);

        if (nova->graphics_command_pools) {
            uint32_t graphics_command_pool_count = nova->graphics_queue_count * PLATYNOVA_MAX_IMAGES * thread_count;
            uint32_t compute_command_pool_count = nova->compute_queue_count * PLATYNOVA_MAX_IMAGES * thread_count;
            uint32_t transfer_command_pool_count = nova->transfer_queue_count * thread_count;

            for (uint32_t j = 0; j < graphics_command_pool_count; j++)
                nova->api.vkDestroyCommandPool(nova->logical, nova->graphics_command_pools[j], NULL);
            for (uint32_t j = 0; j < compute_command_pool_count; j++)
                nova->api.vkDestroyCommandPool(nova->logical, nova->compute_command_pools[j], NULL);
            for (uint32_t j = 0; j < transfer_command_pool_count; j++)
                nova->api.vkDestroyCommandPool(nova->logical, nova->transfer_command_pools[j], NULL);

            /* the first graphics command pool points to the beginning of the command pool array */
            free(nova->graphics_command_pools);
            free(nova->compute_command_pools);
            free(nova->transfer_command_pools);
        }

        if (nova->graphics_queues) {
            /* the first graphics queue points to the beginning of the queue array */
            free(nova->graphics_queues);
            free(nova->compute_queues);
            free(nova->transfer_queues);
        }

        log_info("Destroying a rendering device (platynova) for GPU: %s", nova->physical_properties.deviceName);
        nova->api.vkDestroyDevice(nova->logical, NULL);
    }
}

void _platynova_vulkan_setup_internal_device(
        struct platynova *nova,
        void *internal_novas,
        uint32_t nova_idx,
        bool use_raytracing)
{
    struct platynova_vulkan *novas_vk = (struct platynova_vulkan *)internal_novas;

    nova->internal_nova = &novas_vk[nova_idx];

    if ((novas_vk[nova_idx].extensions & vulkan_extension_mask_raytracing) == vulkan_extension_mask_raytracing) {
        at_fetch_or_relaxed(&nova->flags, platynova_flag_hardware_raytracing_support);
        if (use_raytracing)
            at_fetch_or_relaxed(&nova->plat->flags, platinum_flag_use_raytracing_accelerated);
    } else if (use_raytracing) {
        at_fetch_or_relaxed(&nova->plat->flags, platinum_flag_use_raytracing_soft);
    }
}
