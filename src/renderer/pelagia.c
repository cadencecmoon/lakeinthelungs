/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/assert.h>
#include <lake/hadopelagic.h>
#include <lake/pelagia.h>

#include <string.h> /* strcmp */

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
static VKAPI_ATTR VkBool32 VKAPI_CALL 
debug_utils_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackdata,
        void *userdata)
{
    (void)userdata;
    (void)type;

    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        log_debug("%s", callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_trace("%s", callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("%s", callbackdata->pMessage);
        break;
    default:
        log_error("%s", callbackdata->pMessage);
        assertion(!"Vulkan validation error !!");
    }
    return VK_FALSE;
}

static void create_validation_layers(struct vulkan_backend *vk) 
{
    if (vk->messenger != VK_NULL_HANDLE)
        return;

    VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_utils_callback,
        .pUserData = vk, /* XXX maybe used later? */ 
    };

    if (vk->api.vkCreateDebugUtilsMessengerEXT(vk->instance, &messenger_info, NULL, &vk->messenger) != VK_SUCCESS) {
        vk->extensions &= ~(vulkan_extension_layer_validation_bit | vulkan_extension_debug_utils_bit);
        return;
    }
    log_trace("Created Vulkan validation layers.");
}

static void destroy_validation_layers(struct vulkan_backend *vk)
{
    if (vk->messenger == VK_NULL_HANDLE)
        return;

    vk->api.vkDestroyDebugUtilsMessengerEXT(vk->instance, vk->messenger, NULL);
    vk->messenger = VK_NULL_HANDLE;
    log_trace("Destroyed Vulkan validation layers.");
}
#endif /* validation layers */

static s32 create_instance(
    struct vulkan_backend  *vk,
    struct arena_allocator *temp_arena,
    const char             *application_name,
    u32                     application_version)
{
    u32 instance_version = 0, extension_count = 0, layer_count = 0;
    char **instance_extensions = NULL;

    vk->api.vkEnumerateInstanceVersion(&instance_version);
    vk->api.vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = application_name,
        .pEngineName = "A Moonlit Walk Engine",
        .engineVersion = AMW_VERSION,
        .applicationVersion = application_version,
        .apiVersion = instance_version,
    };

    VERIFY_VK(vk->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (extension_count > 0) {
        VkExtensionProperties *extension_props = (VkExtensionProperties *)arena_alloc(temp_arena, sizeof(VkExtensionProperties) * extension_count);
        VERIFY_VK(vk->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_props));

        u32 i;
        for (i = 0; i < extension_count; i++) {
            if (!strcmp(extension_props[i].extensionName, "VK_KHR_surface")) {
                vk->extensions |= vulkan_extension_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_win32_surface")) {
                vk->extensions |= vulkan_extension_win32_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_EXT_metal_surface")) {
                vk->extensions |= vulkan_extension_metal_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_android_surface")) {
                vk->extensions |= vulkan_extension_android_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_wayland_surface")) {
                vk->extensions |= vulkan_extension_wayland_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_xcb_surface")) {
                vk->extensions |= vulkan_extension_xcb_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_headless_surface")) {
                vk->extensions |= vulkan_extension_headless_surface_bit;
            } else if (!strcmp(extension_props[i].extensionName, "VK_KHR_display")) {
                vk->extensions |= vulkan_extension_display_bit;
#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
            } else if (!strcmp(extension_props[i].extensionName, "VK_EXT_debug_utils")) {
                vk->extensions |= vulkan_extension_debug_utils_bit;
#endif
            }
        }
        i = 0; extension_count = count_bits32(vk->extensions);

        instance_extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
        if (vk->extensions & vulkan_extension_surface_bit)
            instance_extensions[i++] = "VK_KHR_surface";
        if (vk->extensions & vulkan_extension_win32_surface_bit)
            instance_extensions[i++] = "VK_KHR_win32_surface";
        if (vk->extensions & vulkan_extension_metal_surface_bit)
            instance_extensions[i++] = "VK_EXT_metal_surface";
        if (vk->extensions & vulkan_extension_android_surface_bit)
            instance_extensions[i++] = "VK_KHR_android_surface";
        if (vk->extensions & vulkan_extension_wayland_surface_bit)
            instance_extensions[i++] = "VK_KHR_wayland_surface";
        if (vk->extensions & vulkan_extension_xcb_surface_bit)
            instance_extensions[i++] = "VK_KHR_xcb_surface";
        if (vk->extensions & vulkan_extension_headless_surface_bit)
            instance_extensions[i++] = "VK_KHR_headless_surface";
        if (vk->extensions & vulkan_extension_display_bit)
            instance_extensions[i++] = "VK_KHR_display";
        if (vk->extensions & vulkan_extension_debug_utils_bit)
            instance_extensions[i++] = "VK_EXT_debug_utils";
    } else {
        log_error("No Vulkan instance extensions for some reason.");
        return result_error_undefined; /* TODO */
    }

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (const char * const *)instance_extensions,
    };

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    VkValidationFeatureEnableEXT validation_feature_enable[] = {
        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    };
    VkValidationFeaturesEXT validation_features = {
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
        .pNext = NULL,
        .enabledValidationFeatureCount = arraysize(validation_feature_enable),
        .pEnabledValidationFeatures = validation_feature_enable,
        .disabledValidationFeatureCount = 0,
        .pDisabledValidationFeatures = NULL,
    };
    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    if (layer_count > 0 && vk->extensions & vulkan_extension_debug_utils_bit) {
        vk->extensions |= vulkan_extension_layer_validation_bit;
        instance_create_info.pNext = &validation_features;
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }
#endif /* validation layers */

    VERIFY_VK(vk->api.vkCreateInstance(&instance_create_info, NULL, &vk->instance));
    s32 res = vulkan_load_instance_api_procedures(&vk->api, vk->instance, vk->extensions);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    if (vk->extensions & vulkan_extension_layer_validation_bit && vk->api.vkCreateDebugUtilsMessengerEXT)    
        create_validation_layers(vk);
#endif /* validation layers */
    return res;
}

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

static const char *vendor_name_string(u32 vendor_id)
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

static void select_rendering_device(struct pelagia_renderer_init_work *work)
{
    struct pelagia         *pelagia       = work->pelagia;
    struct arena_allocator *scratch_arena = &pelagia->scratch_arena;

    u32 physical_device_count = 0;
    s32 preferred_device_idx = work->preferred_device_idx;

    VERIFY_VK(pelagia->vk.api.vkEnumeratePhysicalDevices(pelagia->vk.instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error("No physical GPU devices are available to Vulkan.");
        work->out_result = result_error_undefined; /* TODO */
        return;
    }
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)arena_alloc(scratch_arena, sizeof(VkPhysicalDevice) * physical_device_count);
    VERIFY_VK(pelagia->vk.api.vkEnumeratePhysicalDevices(pelagia->vk.instance, &physical_device_count, physical_devices));

    pelagia->device.physical = VK_NULL_HANDLE;
    preferred_device_idx = preferred_device_idx < (s32)physical_device_count ? preferred_device_idx : -1;

    struct region scratch = *pelagia->scratch_arena.end;
    for (u32 i = 0; i < physical_device_count && pelagia->device.physical == VK_NULL_HANDLE; i++) {
        VkPhysicalDeviceProperties physical_device_properties;
        VkQueueFamilyProperties *queue_family_properties;
        VkExtensionProperties *extension_properties;
        u32 queue_family_count = 0, extension_count = 0, extension_bits = 0;
        *pelagia->scratch_arena.end = scratch;

        /* too old drivers? */
        pelagia->vk.api.vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,1,0)) continue;

        /* no queue families? */
        pelagia->vk.api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, NULL);
        if (queue_family_count == 0) continue;

        queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(scratch_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        pelagia->vk.api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_family_properties);

        u32 graphics_family_idx = 0u, command_support = 0u;
        for (u32 j = 0; j < queue_family_count; j++) {
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                command_support |= (1u << 0);
            }
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT))
                command_support |= (1u << 1);
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT))
                command_support |= (1u << 2);
        }

        /* accept only devices with support for graphics, compute and transfer commands. */
        if ((command_support & 7) != 7) continue;

        /* no device extensions? */
        pelagia->vk.api.vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &extension_count, NULL);
        if (extension_count == 0) continue;

        /* pick this as a main device? */
        if (preferred_device_idx == (s32)i || (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && preferred_device_idx < 0) || physical_device_count == 1) {
            extension_properties = (VkExtensionProperties *)arena_alloc(scratch_arena, sizeof(VkExtensionProperties) * extension_count);
            pelagia->vk.api.vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &extension_count, extension_properties);

            for (u32 j = 0; j < extension_count; j++) {
                if (!strcmp(extension_properties[j].extensionName, "VK_KHR_swapchain")) {
                    extension_bits |= vulkan_extension_swapchain_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_device_fault")) {
                    extension_bits |= vulkan_extension_device_fault_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_budget")) {
                    extension_bits |= vulkan_extension_memory_budget_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_priority")) {
                    extension_bits |= vulkan_extension_memory_priority_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_AMD_device_coherent_memory")) {
                    extension_bits |= vulkan_extension_amd_device_coherent_memory_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_deferred_host_operations")) {
                    extension_bits |= vulkan_extension_deferred_host_operations_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_acceleration_structure")) {
                    extension_bits |= vulkan_extension_acceleration_structure_bit;
                } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_ray_query")) {
                    extension_bits |= vulkan_extension_ray_query_bit;
                }
                if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,4,0)) {
                    if (!strcmp(extension_properties[j].extensionName, "VK_KHR_dynamic_rendering_local_read")) {
                        extension_bits |= vulkan_extension_dynamic_rendering_local_read_bit;
                    }
                }
                if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,3,0)) {
                    if (!strcmp(extension_properties[j].extensionName, "VK_KHR_dynamic_rendering")) {
                        extension_bits |= vulkan_extension_dynamic_rendering_bit;
                    }
                }
            }

            VkBool32 presentation_supported = VK_FALSE;
            VkResult res = pelagia->vk.api.vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices[i], graphics_family_idx, pelagia->swapchain.surface, &presentation_supported);
            if (res != VK_SUCCESS) {
                log_warn("The selected rendering device does not support presentation to the window surface: %s.", vulkan_result_string(res));
                continue;
            }

            pelagia->device.physical = physical_devices[i];
            pelagia->device.graphics_queue_family_idx = graphics_family_idx;
            pelagia->device.extensions = extension_bits;
        }
    }
    *pelagia->scratch_arena.end = scratch;

    /* no device was found ? */
    if (pelagia->device.physical == VK_NULL_HANDLE) {
        log_error("Could not query a physical device that meets our requirements.");
        work->out_result = result_error_undefined; /* TODO */
        return;
    }

    /* decide on the workload buffering count */
    pelagia->vk.api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pelagia->device.physical, pelagia->swapchain.surface, &pelagia->swapchain.surface_capabilities);
    u32 min_image_count = pelagia->swapchain.surface_capabilities.minImageCount;
    u32 max_image_count = pelagia->swapchain.surface_capabilities.maxImageCount;
    u32 workload_buffering = max(min(work->target_workload_buffering, pelagia_workload_buffering_quad), pelagia_workload_buffering_dual);
    if (workload_buffering < min_image_count) workload_buffering = min_image_count;
    if (workload_buffering > max_image_count && max_image_count != 0) workload_buffering = max_image_count;
    log_debug("Swapchain %u min image count, %u max image count: requested %u-buffering, decided on %u-buffering.", 
        min_image_count, max_image_count, work->target_workload_buffering, workload_buffering);
    pelagia->workload_buffering = workload_buffering;
};

static s32 construct_rendering_device(
    struct pelagia *pelagia,
    u32             thread_count)
{
    struct vulkan_backend  *vk            = &pelagia->vk;
    struct vulkan_device   *device        = &pelagia->device;
    struct arena_allocator *scratch_arena = &pelagia->scratch_arena;

    u32 extension_count = 0, i;
    u32 queue_family_count = 0;
    u32 workload_buffering = pelagia->workload_buffering;
    VkQueueFamilyProperties *queue_family_properties;
    char **extensions = NULL;
    s32 result;

    vk->api.vkGetPhysicalDeviceProperties(device->physical, &device->physical_properties);
    vk->api.vkGetPhysicalDeviceFeatures(device->physical, &device->physical_features);
    vk->api.vkGetPhysicalDeviceMemoryProperties(device->physical, &device->memory_properties);

    /* we already made sure the GPU in question supports the command queues we need */
    vk->api.vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_family_count, NULL);
    queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(scratch_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
    vk->api.vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_family_count, queue_family_properties);

    /* query command queue families */
    for (i = 0; i < queue_family_count; i++) {
        /* we need a general graphics queue, don't be picky */
        if (device->graphics_queue == VK_NULL_HANDLE && queue_family_properties[i].queueCount > 0 
            && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
        {
            device->graphics_queue_family_idx = i;
        }
        /* ask for an async compute queue family */
        if (device->compute_queue_count == 0 && queue_family_properties[i].queueCount > 0
            && (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
        {
            device->compute_queue_count = queue_family_properties[i].queueCount;
            device->compute_queue_family_idx = i;
        }
        /* ask for an async transfer queue family */
        if (device->transfer_queue_count == 0 && queue_family_properties[i].queueCount > 0
            && (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            && ((queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
            && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
        {
            device->transfer_queue_count = queue_family_properties[i].queueCount;
            device->transfer_queue_family_idx = i;
        }
    }

    f32 queue_priorities[16];
    iamemset(queue_priorities, 1.0f, sizeof(f32) * 16);

    VkDeviceQueueCreateInfo queue_create_infos[3];
    for (i = 0; i < 3; i++) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].pNext = NULL;
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pQueuePriorities = queue_priorities;
        queue_create_infos[i].queueCount = 1;
        queue_create_infos[i].queueFamilyIndex = device->graphics_queue_family_idx;
    }

    queue_family_count = 3; /* we'll use this for how many different queue families should be used */
    if (device->compute_queue_count == 0) {
        if (queue_family_properties[device->graphics_queue_family_idx].queueCount > 1) {
            /* if the graphics queue families has multiple queues, let's grab them instead */
            device->compute_queue_count = max(thread_count >> 1, queue_family_properties[device->graphics_queue_family_idx].queueCount - 1);
            queue_create_infos[0].queueCount = 1 + device->compute_queue_count;
        } else {
            /* we'll have to use the graphics queue for compute commands */
            device->compute_queues = &device->graphics_queue;
        }
        /* graphics and compute will share the same command queue family */
        device->compute_queue_family_idx = device->graphics_queue_family_idx;
        queue_family_count--;
    }
    if (device->transfer_queue_count == 0) {
        device->transfer_queues = &device->graphics_queue;
        device->transfer_queue_family_idx = device->graphics_queue_family_idx;
        queue_family_count--;
    }

    /* resolve the queue family indices */
    if (queue_family_count == 3) {
        queue_create_infos[1].queueCount = device->compute_queue_count;
        queue_create_infos[1].queueFamilyIndex = device->compute_queue_family_idx;
        queue_create_infos[2].queueCount = device->transfer_queue_count;
        queue_create_infos[2].queueFamilyIndex = device->transfer_queue_family_idx;
    } else {
        if (device->compute_queue_count > 0) {
            queue_create_infos[1].queueCount = device->compute_queue_count;
            queue_create_infos[1].queueFamilyIndex = device->compute_queue_family_idx;
        } else if (device->transfer_queue_count > 0) {
            queue_create_infos[1].queueCount = device->transfer_queue_count;
            queue_create_infos[1].queueFamilyIndex = device->transfer_queue_family_idx;
        }
    }

    i = 0; extension_count = count_bits32(device->extensions);
    extensions = (char **)arena_alloc(scratch_arena, sizeof(char *) * extension_count);
    if (device->extensions & vulkan_extension_swapchain_bit)
        extensions[i++] = "VK_KHR_swapchain";
    if (device->extensions & vulkan_extension_device_fault_bit)
        extensions[i++] = "VK_EXT_device_fault";
    if (device->extensions & vulkan_extension_memory_budget_bit)
        extensions[i++] = "VK_EXT_memory_budget";
    if (device->extensions & vulkan_extension_memory_priority_bit)
        extensions[i++] = "VK_EXT_memory_priority";
    if (device->extensions & vulkan_extension_amd_device_coherent_memory_bit)
        extensions[i++] = "VK_AMD_device_coherent_memory";
    if (device->extensions & vulkan_extension_deferred_host_operations_bit)
        extensions[i++] = "VK_KHR_deferred_host_operations";
    if (device->extensions & vulkan_extension_acceleration_structure_bit)
        extensions[i++] = "VK_KHR_acceleration_structure";
    if (device->extensions & vulkan_extension_ray_query_bit)
        extensions[i++] = "VK_KHR_ray_query";
    if (device->extensions & vulkan_extension_dynamic_rendering_local_read_bit)
        extensions[i++] = "VK_KHR_dynamic_rendering_local_read";
    if (device->extensions & vulkan_extension_dynamic_rendering_bit)
        extensions[i++] = "VK_KHR_dynamic_rendering";

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
    VkPhysicalDeviceDynamicRenderingLocalReadFeatures dynamic_rendering_local_read_feats = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES_KHR,
        .pNext = ((device->extensions & vulkan_extension_mask_raytracing) 
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
        .vulkanMemoryModel = VK_TRUE,
        .vulkanMemoryModelDeviceScope = VK_TRUE,
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
        .queueCreateInfoCount = queue_family_count,
        .pQueueCreateInfos = queue_create_infos,
    };

    /* create the logical device */
    if (vk->api.vkCreateDevice(device->physical, &device_create_info, NULL, &device->logical) != VK_SUCCESS) {
        log_error("Can't create a Vulkan logical device.");
        return result_error_undefined; /* TODO */
    }
    result = vulkan_load_device_api_procedures(&vk->api, &device->api, device->logical, device->physical_properties.apiVersion, device->extensions);
    if (result != result_success) {
        log_error("Can't load the Vulkan API procedures for device idx.");
        return result;
    }

    /* grab the command queues */
    device->api.vkGetDeviceQueue(device->logical, device->graphics_queue_family_idx, 0, &device->graphics_queue);
    if (device->compute_queue_count > 0) {
        assert_debug(device->compute_queues == NULL);
        device->compute_queues = malloc(sizeof(VkQueue) * device->compute_queue_count);
        iamemset(device->compute_queues, 0u, sizeof(VkQueue) * device->compute_queue_count);
        for (i = (device->graphics_queue_family_idx == device->compute_queue_family_idx ? 1 : 0); i < device->compute_queue_count; i++)
            device->api.vkGetDeviceQueue(device->logical, device->compute_queue_family_idx, i, &device->compute_queues[i]);
    }
    if (device->transfer_queue_count > 0) {
        assert_debug(device->transfer_queues == NULL);
        device->transfer_queues = malloc(sizeof(VkQueue) * device->transfer_queue_count);
        iamemset(device->transfer_queues, 0u, sizeof(VkQueue) * device->transfer_queue_count);
        for (i = 0; i < device->transfer_queue_count; i++)
            device->api.vkGetDeviceQueue(device->logical, device->transfer_queue_family_idx, i, &device->transfer_queues[i]);
    }

#if AMW_DEBUG
    assert_debug(device->graphics_queue != VK_NULL_HANDLE);
    assert_debug(device->compute_queues[0] != VK_NULL_HANDLE);
    assert_debug(device->transfer_queues[0] != VK_NULL_HANDLE);
#endif /* AMW_DEBUG */

    /* create the command cools per queue family */
    device->queue_command_pool_count = workload_buffering * thread_count;
    device->raw_command_pool_count = queue_family_count * device->queue_command_pool_count;
    device->raw_command_pools = (VkCommandPool *)malloc(sizeof(VkCommandPool) * device->raw_command_pool_count);
    iamemset(device->raw_command_pools, 0u, sizeof(VkCommandPool) * device->raw_command_pool_count);

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = device->graphics_queue_family_idx,
    };

    device->graphics_command_pools = device->raw_command_pools;
    for (i = 0; i < device->queue_command_pool_count; i++)
        VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->graphics_command_pools[i]));

    if (device->compute_queue_family_idx != device->graphics_queue_family_idx) {
        device->compute_command_pools = &device->raw_command_pools[device->queue_command_pool_count];
        for (i = 0; i < device->queue_command_pool_count; i++)
            VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->compute_command_pools[i]));
    } else {
        device->compute_command_pools = device->graphics_command_pools;
    }

    if (device->transfer_queue_family_idx != device->graphics_queue_family_idx) {
        device->transfer_command_pools = &device->raw_command_pools[device->raw_command_pool_count - device->queue_command_pool_count];
        for (u32 j = 0; j < device->queue_command_pool_count; j++)
            VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->transfer_command_pools[j]));
    } else {
        device->transfer_command_pools = device->graphics_command_pools;
    }

#if AMW_DEBUG
    for (u32 j = 0; j < device->raw_command_pool_count; j++)
        assert_debug(device->raw_command_pools[j] != VK_NULL_HANDLE);
#endif /* AMW_DEBUG */

    log_info("Created a Vulkan rendering device:");
    log_info("              Name : %s", device->physical_properties.deviceName);
    log_info("              Type : %s, ID: %X", device_type_string(device->physical_properties.deviceType), device->physical_properties.deviceID);
    log_info("            Vendor : %s, ID: %X", vendor_name_string(device->physical_properties.vendorID), device->physical_properties.vendorID);
    log_info("       Api version : %u.%u.%u", (device->physical_properties.apiVersion >> 22U), (device->physical_properties.apiVersion >> 12U) & 0x3ffU, (device->physical_properties.apiVersion & 0xfffU));
    log_info("    Driver version : %u.%u.%u", (device->physical_properties.driverVersion >> 22U), (device->physical_properties.driverVersion >> 12U) & 0x3ffU, (device->physical_properties.driverVersion & 0xfffU));

    log_debug("Device created %d command queue families and %u command pools (%u per thread):", queue_family_count, device->raw_command_pool_count, workload_buffering);
    log_debug("    graphics queues : 1   graphics queue family idx : %d   command pools : %u", device->graphics_queue_family_idx, device->queue_command_pool_count);
    log_debug("     compute queues : %u    compute queue family idx : %d   command pools : %u",
            device->compute_queue_count, device->compute_queue_family_idx, device->compute_queue_count ? device->queue_command_pool_count : 0);
    log_debug("    transfer queues : %u   transfer queue family idx : %d   command pools : %u",
            device->transfer_queue_count, device->transfer_queue_family_idx, device->transfer_queue_count ? device->queue_command_pool_count : 0);

    /* ray tracing mesh support */
    if ((device->extensions & vulkan_extension_mask_raytracing) == vulkan_extension_mask_raytracing) {
        device->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 physical_device_properties2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
            .pNext = &device->acceleration_structure_properties,
        };
        vk->api.vkGetPhysicalDeviceProperties2(device->physical, &physical_device_properties2);
        at_fetch_or_relaxed(&pelagia->flags, pelagia_flag_accelerated_ray_tracing_supported);
        log_info("This device has hardware support for accelerated ray tracing.");
    }

    return result_success;
}

AMWAPI void pelagia_renderer_init(struct pelagia_renderer_init_work *work)
{
    if (!work) return;

    struct pelagia *pelagia = work->pelagia;
    struct riven   *riven   = work->riven;

    /* prepare the state, and fini will be called in case of errors. */
    struct pelagia_renderer_fini_work fini = { .pelagia = pelagia, .riven = riven };
    pelagia_renderer_fini(&fini);

    /* connect to vulkan */
    if (!vulkan_open_driver(&pelagia->vk.api)) {
        work->out_result = result_error_missing_shared_library;
        return;
    }

    arena_init(&pelagia->scratch_arena, 16 * 1024);
    struct region scratch = *pelagia->scratch_arena.end;
    s32 result;

    /* initialize the instance */
    result = create_instance(&pelagia->vk, &pelagia->scratch_arena, work->application_name, work->application_version);
    if (result != result_success) {
        log_error("Failed to create a Vulkan instance and load API procedures.");
        work->out_result = result;
        return;
    }
    *pelagia->scratch_arena.end = scratch;

    /* connect the renderer with the display backend */
    result = vulkan_create_swapchain_surface(&pelagia->vk, &pelagia->swapchain, work->hadal);
    if (result != result_success) {
        log_error("Failed to create a Vulkan surface for the '%s' display backend.", work->hadal->backend_name);
        work->out_result = result; 
        return;
    }

    /* query physical devices and decide which are to be used */
    select_rendering_device(work);
    if (work->out_result != result_success) return;
    result = construct_rendering_device(pelagia, work->thread_count);
    if (result != result_success) {
        log_error("Failed to create a logical Vulkan device.");
        work->out_result = result;
        return;
    }
    *pelagia->scratch_arena.end = scratch;
    at_fetch_or_relaxed(&pelagia->flags, pelagia_flag_initialized);

    struct pelagia_assemble_swapchain_work assemble_swapchain_work = {
        .pelagia = pelagia,
        .hadal = work->hadal,
        .use_vsync = work->enable_vsync,
        .out_result = result_success,
    };
    arena_init(&pelagia->swapchain.arena, 234);

    pelagia_assemble_swapchain(&assemble_swapchain_work);
    if (assemble_swapchain_work.out_result != result_success) {
        log_error("Failed to create a swapchain at initialization.");
        work->out_result = assemble_swapchain_work.out_result;
        return;
    }
    *pelagia->scratch_arena.end = scratch;

    struct rivens_tear assemble_tears[3];
    struct pelagia_assemble_render_targets_work assemble_render_targets = {
        .pelagia = pelagia,
        .dissasemble = false,
        .out_result = result_success,
    };
    struct pelagia_assemble_uniform_buffers_work assemble_uniform_buffers = {
        .pelagia = pelagia,
        .dissasemble = false,
        .out_result = result_success,
    };
    struct pelagia_assemble_shader_pipelines_work assemble_shader_pipelines = {
        .pelagia = pelagia,
        .riven = riven,
        .dissasemble = true,
    };

    s32 i = 0;
    pelagia_assemble_render_targets_tear__(&assemble_render_targets, &assemble_tears[i++]);
    pelagia_assemble_uniform_buffers_tear__(&assemble_uniform_buffers, &assemble_tears[i++]);
    pelagia_assemble_shader_pipelines_tear__(&assemble_shader_pipelines, &assemble_tears[i++]);
    riven_split_and_unchain(riven, assemble_tears, i);
    *pelagia->scratch_arena.end = scratch;
    i = 0;

    /* validate the work */
#define CHECKWORK(proc, idx) \
    if (assemble_##proc.out_result != result_success) { \
        log_error("Tear '%s' for primary device failed and returned %d.", assemble_tears[idx].name, assemble_##proc .out_result); \
        work->out_result = assemble_##proc.out_result; \
    }
    CHECKWORK(uniform_buffers, i++)
    CHECKWORK(render_targets, i++)
    CHECKWORK(shader_pipelines, i++)
#undef CHECKWORK
    if (work->out_result != result_success) return;
    *pelagia->scratch_arena.end = scratch;
}

AMWAPI void pelagia_renderer_fini(struct pelagia_renderer_fini_work *work)
{
    PELAGIA_WORK_INITIALIZED_OR_RETURN

    struct pelagia       *pelagia = work->pelagia;
    struct riven         *riven   = work->riven;
    struct vulkan_device *device = &pelagia->device;

    /* if no device was created, we can skip this step */
    if (device->logical != VK_NULL_HANDLE) {
        device->api.vkDeviceWaitIdle(device->logical);

        if (pelagia->swapchain.sc != VK_NULL_HANDLE) {
            vulkan_clear_swapchain(&pelagia->swapchain, device);
            device->api.vkDestroySwapchainKHR(device->logical, pelagia->swapchain.sc, NULL);
        }

        struct rivens_tear disassemble_tears[3];
        struct pelagia_assemble_render_targets_work disassemble_render_targets = {
            .pelagia = pelagia,
            .dissasemble = true,
        };
        struct pelagia_assemble_uniform_buffers_work disassemble_uniform_buffers = {
            .pelagia = pelagia,
            .dissasemble = true,
        };
        struct pelagia_assemble_shader_pipelines_work disassemble_shader_pipelines = {
            .pelagia = pelagia,
            .riven = riven,
            .dissasemble = true,
        };

        s32 i = 0;
        pelagia_assemble_render_targets_tear__(&disassemble_render_targets, &disassemble_tears[i++]);
        pelagia_assemble_uniform_buffers_tear__(&disassemble_uniform_buffers, &disassemble_tears[i++]);
        pelagia_assemble_shader_pipelines_tear__(&disassemble_shader_pipelines, &disassemble_tears[i++]);
        riven_split_and_unchain(riven, disassemble_tears, i);

        if (device->raw_command_pools) {
            for (u32 j = 0; j < device->raw_command_pool_count; j++)
                device->api.vkDestroyCommandPool(device->logical, device->raw_command_pools[j], NULL);
            free(device->raw_command_pools); /* TODO ia */
        }

        if (device->compute_queues && device->compute_queue_family_idx != device->graphics_queue_family_idx)
            free(device->compute_queues);
        if (device->transfer_queues && device->transfer_queue_family_idx != device->graphics_queue_family_idx)
            free(device->transfer_queues);

        log_info("Destroying a Vulkan rendering device: %s", device->physical_properties.deviceName);
        device->api.vkDestroyDevice(device->logical, NULL);
        iazerop(device);
    }

    if (pelagia->swapchain.surface != VK_NULL_HANDLE)
        pelagia->vk.api.vkDestroySurfaceKHR(pelagia->vk.instance, pelagia->swapchain.surface, NULL);
    arena_fini(&pelagia->swapchain.arena);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    destroy_validation_layers(&pelagia->vk);
#endif /* validation layers */

    if (pelagia->vk.instance)
        pelagia->vk.api.vkDestroyInstance(pelagia->vk.instance, NULL);
    arena_fini(&pelagia->scratch_arena);

    iazerop(pelagia);
}
