/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/assert.h>
#include <lake/hadopelagic.h>
#include <lake/pelagia.h>

#include <string.h> /* memcpy */

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
    log_debug("Created Vulkan validation layers.");
}

static void destroy_validation_layers(struct vulkan_backend *vk)
{
    if (vk->messenger == VK_NULL_HANDLE)
        return;

    vk->api.vkDestroyDebugUtilsMessengerEXT(vk->instance, vk->messenger, NULL);
    vk->messenger = VK_NULL_HANDLE;
    log_debug("Destroyed Vulkan validation layers.");
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

static void select_rendering_devices(struct pelagia_renderer_init_work *work)
{
    struct pelagia *pelagia = work->pelagia;

    u32 physical_device_count = 0;
    u32 device_count = 0;
    u32 virtual_device_count = max(0, work->virtual_device_count);
    s32 max_device_count = work->max_physical_device_count;
    s32 preferred_primary_device_idx = work->preferred_primary_device_idx;
    s32 primary_device_idx = -1;
    u32 primary_device_graphics_queue_family_idx = 0;

    VERIFY_VK(pelagia->vk.api.vkEnumeratePhysicalDevices(pelagia->vk.instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error("No physical GPU devices are available to Vulkan.");
        work->out_result = result_error_undefined; /* TODO */
        return;
    }
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)arena_alloc(&pelagia->scratch_arena, sizeof(VkPhysicalDevice) * physical_device_count);
    VERIFY_VK(pelagia->vk.api.vkEnumeratePhysicalDevices(pelagia->vk.instance, &physical_device_count, physical_devices));

    preferred_primary_device_idx = preferred_primary_device_idx < (s32)physical_device_count ? preferred_primary_device_idx : -1;
    max_device_count = min(max_device_count, (s32)physical_device_count);
    
    s32 *physical_device_indices = (s32 *)arena_alloc(&pelagia->scratch_arena, sizeof(s32) * 2 * physical_device_count);
    u32 *extension_bits = (u32 *)physical_device_indices + sizeof(s32) * physical_device_count;

    struct region scratch = *pelagia->scratch_arena.end;
    for (u32 i = 0; (i < physical_device_count) && (max_device_count > 0 ? max_device_count : true); i++) {
        VkPhysicalDeviceProperties physical_device_properties;
        VkQueueFamilyProperties *queue_family_properties;
        VkExtensionProperties *extension_properties;
        u32 queue_family_count = 0, extension_count = 0;

        physical_device_indices[i] = -1;
        extension_bits[i] = 0u;
        *pelagia->scratch_arena.end = scratch;

        /* too old drivers? */
        pelagia->vk.api.vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,1,0)) continue;

        /* no queue families? */
        pelagia->vk.api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, NULL);
        if (queue_family_count == 0) continue;

        queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(&pelagia->scratch_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        pelagia->vk.api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_family_properties);

        u32 graphics_family_idx = 0u, command_support = 0u;
        for (u32 j = 0; j < queue_family_count; j++) {
            if (graphics_family_idx == 0 && queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                graphics_family_idx = j;
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

        extension_properties = (VkExtensionProperties *)arena_alloc(&pelagia->scratch_arena, sizeof(VkExtensionProperties) * extension_count);
        pelagia->vk.api.vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &extension_count, extension_properties);

        for (u32 j = 0; j < extension_count; j++) {
            if (!strcmp(extension_properties[j].extensionName, "VK_KHR_swapchain")) {
                extension_bits[i] |= vulkan_extension_swapchain_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_device_fault")) {
                extension_bits[i] |= vulkan_extension_device_fault_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_budget")) {
                extension_bits[i] |= vulkan_extension_memory_budget_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_priority")) {
                extension_bits[i] |= vulkan_extension_memory_priority_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_AMD_device_coherent_memory")) {
                extension_bits[i] |= vulkan_extension_amd_device_coherent_memory_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_deferred_host_operations")) {
                extension_bits[i] |= vulkan_extension_deferred_host_operations_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_acceleration_structure")) {
                extension_bits[i] |= vulkan_extension_acceleration_structure_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_KHR_ray_query")) {
                extension_bits[i] |= vulkan_extension_ray_query_bit;
            }
            if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,4,0)) {
                if (!strcmp(extension_properties[j].extensionName, "VK_KHR_dynamic_rendering_local_read")) {
                    extension_bits[i] |= vulkan_extension_dynamic_rendering_local_read_bit;
                }
            }
            if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,3,0)) {
                if (!strcmp(extension_properties[j].extensionName, "VK_KHR_dynamic_rendering")) {
                    extension_bits[i] |= vulkan_extension_dynamic_rendering_bit;
                }
            }
        }

        /* check if suitable for use as a main device */
        if (preferred_primary_device_idx == (s32)i || (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
                && preferred_primary_device_idx < 0 && primary_device_idx < 0) || physical_device_count == 1) 
        {
            /* check if suitable for use as a primary device */
            if (extension_bits[i] & vulkan_extension_swapchain_bit) {
                primary_device_idx = i;
                primary_device_graphics_queue_family_idx = graphics_family_idx;
            } else if (preferred_primary_device_idx >= 0 && preferred_primary_device_idx == (s32)i)
                preferred_primary_device_idx = -1;
            physical_device_indices[i] = i;
            device_count++;
        } else if ((max_device_count <= 0) || (primary_device_idx >= 0 ? max_device_count > (s32)device_count : max_device_count > (s32)(device_count - 1))) {
            physical_device_indices[i] = i;
            device_count++;
        }
    }
    *pelagia->scratch_arena.end = scratch;

    /* no device was found ? */
    if (device_count == 0) {
        log_error("Could not query a single physical device that meets our requirements.");
        work->out_result = result_error_undefined; /* TODO */
        return;
    } else if (primary_device_idx < 0 || primary_device_idx >= (s32)physical_device_count) {
        primary_device_idx = 0;
    }

    /* add the virtual device count */
    device_count += virtual_device_count;

    /* decide on the frames buffering count */
    VkBool32 presentation_supported = VK_FALSE;
    if (pelagia->vk.api.vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_devices[primary_device_idx], 
            primary_device_graphics_queue_family_idx, 
            pelagia->swapchain.surface, 
            &presentation_supported) != VK_SUCCESS) 
    {
        log_error("The primary device does not support presenting to the window.");
        work->out_result = result_error_undefined; /* TODO */
    }

    pelagia->vk.api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_devices[primary_device_idx], pelagia->swapchain.surface, &pelagia->swapchain.surface_capabilities);
    u32 min_image_count = pelagia->swapchain.surface_capabilities.minImageCount;
    u32 max_image_count = pelagia->swapchain.surface_capabilities.maxImageCount;
    u32 frames_buffering = max(min(work->max_frames_buffering, pelagia_frames_quad_buffering), pelagia_frames_dual_buffering);
    if (frames_buffering < min_image_count) frames_buffering = min_image_count;
    if (frames_buffering > max_image_count && max_image_count != 0) frames_buffering = max_image_count;
    log_info("Swapchain %u min image count, %u max image count: requested %u-buffering, but decided on %u-buffering.", 
        min_image_count, max_image_count, work->max_frames_buffering, frames_buffering);
    pelagia->frames_buffering = frames_buffering;

    struct vulkan_device *devices = (struct vulkan_device *)malloc(sizeof(struct vulkan_device) * device_count);
    iamemset(devices, 0u, sizeof(struct vulkan_device) * device_count);
    pelagia->devices = devices;
    pelagia->device_count = device_count;

    /* resolve physical device indices */
    devices[0].physical = physical_devices[primary_device_idx];
    devices[0].extensions = extension_bits[primary_device_idx];
    devices[0].flags |= (pelagia_device_flag_primary | pelagia_device_flag_is_valid);
    primary_device_idx = physical_device_indices[primary_device_idx] = -1; /* we don't need that anymore */
    for (u32 i = 1; i < device_count; i++) {
        if (i >= device_count - virtual_device_count) {
            devices[i].physical = devices[0].physical;
            devices[i].extensions = devices[0].extensions;
            devices[i].flags |= (pelagia_device_flag_virtual | pelagia_device_flag_is_valid);
            continue;
        }
        devices[i].physical = VK_NULL_HANDLE;
        for (u32 j = 0; j < physical_device_count && devices[i].physical == VK_NULL_HANDLE; j++) {
            s32 idx = physical_device_indices[j];
            if (idx < 0) continue;

            devices[i].physical = physical_devices[idx]; 
            devices[i].extensions = extension_bits[idx];
            devices[i].flags |= pelagia_device_flag_is_valid;
            physical_device_indices[j] = -1;
        }
    }
};

struct construct_device_work {
    struct vulkan_backend  *vk;
    struct vulkan_device   *device;
    struct arena_allocator *temp_arena;
    u32                     frames_buffering;
    u32                     thread_count;
    u32                     device_idx;
    s32                     out_result;
};

static void construct_device(struct construct_device_work *work) 
{
    u32 extension_count = 0, i;
    u32 queue_family_count = 0;
    VkQueueFamilyProperties *queue_family_properties;
    char **extensions = NULL;

    struct vulkan_backend  *vk               = work->vk;
    struct vulkan_device   *device           = work->device;
    struct arena_allocator *temp_arena       = work->temp_arena;
    u32                     frames_buffering = work->frames_buffering;
    u32                     thread_count     = work->thread_count;
    u32                     device_idx       = work->device_idx;

    vk->api.vkGetPhysicalDeviceProperties(device->physical, &device->physical_properties);
    vk->api.vkGetPhysicalDeviceFeatures(device->physical, &device->physical_features);
    vk->api.vkGetPhysicalDeviceMemoryProperties(device->physical, &device->memory_properties);

    /* we already made sure the GPU in question supports the command queues we need */
    vk->api.vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_family_count, NULL);
    queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(temp_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
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
    extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
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
        log_error("Can't create a Vulkan logical device idx %d.", device_idx);
        work->out_result = result_error_undefined; /* TODO */
        return;
    }
    work->out_result = vulkan_load_device_api_procedures(&vk->api, &device->api, device->logical, device->physical_properties.apiVersion, device->extensions);
    if (work->out_result != result_success) {
        log_error("Can't load the Vulkan API procedures for device idx %d.", device_idx);
        return;
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
    device->queue_command_pool_count = frames_buffering * thread_count;
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

    log_info("Created a Vulkan rendering device, idx %d:", device_idx);
    log_info("              Name : %s", device->physical_properties.deviceName);
    log_info("              Type : %s, ID: %X", device_type_string(device->physical_properties.deviceType), device->physical_properties.deviceID);
    log_info("            Vendor : %s, ID: %X", vendor_name_string(device->physical_properties.vendorID), device->physical_properties.vendorID);
    log_info("       Api version : %u.%u.%u", (device->physical_properties.apiVersion >> 22U), (device->physical_properties.apiVersion >> 12U) & 0x3ffU, (device->physical_properties.apiVersion & 0xfffU));
    log_info("    Driver version : %u.%u.%u", (device->physical_properties.driverVersion >> 22U), (device->physical_properties.driverVersion >> 12U) & 0x3ffU, (device->physical_properties.driverVersion & 0xfffU));

    log_info("Device idx %d created %d command queue families and %u command pools (%u per thread):", device_idx, queue_family_count, device->raw_command_pool_count, frames_buffering);
    log_info("    graphics queues : 1   graphics queue family idx : %d   command pools : %u", device->graphics_queue_family_idx, device->queue_command_pool_count);
    log_info("     compute queues : %u    compute queue family idx : %d   command pools : %u",
            device->compute_queue_count, device->compute_queue_family_idx, device->compute_queue_count ? device->queue_command_pool_count : 0);
    log_info("    transfer queues : %u   transfer queue family idx : %d   command pools : %u",
            device->transfer_queue_count, device->transfer_queue_family_idx, device->transfer_queue_count ? device->queue_command_pool_count : 0);

    /* ray tracing mesh support */
    if ((device->extensions & vulkan_extension_mask_raytracing) == vulkan_extension_mask_raytracing) {
        device->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 physical_device_properties2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
            .pNext = &device->acceleration_structure_properties,
        };
        vk->api.vkGetPhysicalDeviceProperties2(device->physical, &physical_device_properties2);
        device->flags |= pelagia_device_flag_accelerated_ray_tracing_supported;
        log_info("This device has hardware support for accelerated ray tracing.");
    }
}

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

static s32 create_swapchain_surface(
    struct vulkan_backend   *vk,
    struct vulkan_swapchain *swapchain, 
    struct hadopelagic      *hadal)
{
    if (vk->instance == VK_NULL_HANDLE || !hadal->calls.expose_native_window)
        return result_error_invalid_engine_context;
    swapchain->surface = VK_NULL_HANDLE;

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
    return result_error_invalid_engine_context;
}

static void clear_swapchain(
    struct vulkan_swapchain *swapchain,
    struct vulkan_device    *primary)
{
    for (u32 i = 0; i < swapchain->image_count; i++)
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            primary->api.vkDestroyImageView(primary->logical, swapchain->image_views[i], NULL);
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
    assert_debug(work && work->pelagia);
    assert_debug(work->pelagia->devices);

    struct pelagia     *pelagia   = work->pelagia;
    struct hadopelagic *hadal     = work->hadal;
    b32                 use_vsync = work->use_vsync; 

    struct vulkan_backend   *vk        = &pelagia->vk;
    struct vulkan_swapchain *swapchain = &pelagia->swapchain;
    struct vulkan_device    *primary   = &pelagia->devices[0];

    assert_debug(primary->flags & pelagia_device_flag_primary);

    /* TODO work->surface_lost ? */

    VkSwapchainKHR old_sc = swapchain->sc;
    u32 window_width = 0, window_height = 0;
    u32 set_pelagia_flags = 0;

    if (old_sc != VK_NULL_HANDLE)
        clear_swapchain(swapchain, primary);

    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR(primary->physical, swapchain->surface, &swapchain->surface_format_count, NULL));
    swapchain->surface_formats = (VkSurfaceFormatKHR *)arena_alloc(&swapchain->arena, sizeof(VkSurfaceFormatKHR) * swapchain->surface_format_count);
    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR(primary->physical, swapchain->surface, &swapchain->surface_format_count, swapchain->surface_formats));

    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR(primary->physical, swapchain->surface, &swapchain->present_mode_count, NULL));
    swapchain->present_modes = (VkPresentModeKHR *)arena_alloc(&swapchain->arena, sizeof(VkPresentModeKHR) * swapchain->present_mode_count);
    VERIFY_VK(vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR(primary->physical, swapchain->surface, &swapchain->present_mode_count, swapchain->present_modes));

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
            primary->api.vkDestroySwapchainKHR(primary->logical, old_sc, NULL);
        clear_swapchain(swapchain, primary);
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
        primary->graphics_queue_family_idx,
        primary->compute_queue_family_idx,
        primary->transfer_queue_family_idx,
    };
    if (primary->compute_queue_family_idx != primary->graphics_queue_family_idx 
            && primary->transfer_queue_family_idx == primary->graphics_queue_family_idx) {
        queue_family_count = 2;
    } else if (primary->compute_queue_family_idx == primary->graphics_queue_family_idx 
            && primary->transfer_queue_family_idx != primary->graphics_queue_family_idx) 
    {
        queue_family_indices[1] = primary->transfer_queue_family_idx;
        queue_family_count = 2;
    } else if (primary->compute_queue_count == 0 && primary->transfer_queue_count == 0) {
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
    VkResult result = primary->api.vkCreateSwapchainKHR(primary->logical, &swapchain_create_info, NULL, &swapchain->sc);

    if (old_sc != VK_NULL_HANDLE)
        primary->api.vkDestroySwapchainKHR(primary->logical, old_sc, NULL);
    if (result != VK_SUCCESS) {
        log_error("Failed to construct a swap chain: %s", vulkan_result_string(result));
        clear_swapchain(swapchain, primary);
        work->out_result = result_error_undefined; /* TODO */
        return;
    }

    VERIFY_VK(primary->api.vkGetSwapchainImagesKHR(primary->logical, swapchain->sc, &swapchain->image_count, NULL));
    swapchain->images = (VkImage *)arena_alloc(&swapchain->arena, sizeof(VkImage) * swapchain->image_count);
    VERIFY_VK(primary->api.vkGetSwapchainImagesKHR(primary->logical, swapchain->sc, &swapchain->image_count, swapchain->images));
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
        result = primary->api.vkCreateImageView(primary->logical, &color_image_view, NULL, &swapchain->image_views[i]);
        if (result != VK_SUCCESS) {
            log_error("Failed to create a view onto swapchain image: %s", vulkan_result_string(result));
            clear_swapchain(swapchain, primary);
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

AMWAPI void pelagia_renderer_init(struct pelagia_renderer_init_work *work)
{
    if (!work) return;

    s32 result;
    struct pelagia *pelagia = work->pelagia;
    struct riven   *riven   = work->riven;

    /* prepare the state, and fini will be called in case of errors. */
    struct pelagia_renderer_fini_work fini = { pelagia, riven };
    pelagia_renderer_fini(&fini);

    /* connect to vulkan */
    if (!vulkan_open_driver(&pelagia->vk.api)) {
        work->out_result = result_error_missing_shared_library;
        return;
    }

    arena_init(&pelagia->scratch_arena, 16 * 1024);
    struct region scratch = *pelagia->scratch_arena.end;

    /* initialize the instance */
    result = create_instance(&pelagia->vk, &pelagia->scratch_arena, work->application_name, work->application_version);
    if (result != result_success) {
        log_error("Failed to create a Vulkan instance and load API procedures.");
        work->out_result = result;
        return;
    }
    *pelagia->scratch_arena.end = scratch;

    /* connect the renderer with the display backend */
    result = create_swapchain_surface(&pelagia->vk, &pelagia->swapchain, work->hadal);
    if (result != result_success) {
        log_error("Failed to create a Vulkan surface for the '%s' display backend.", work->hadal->backend_name);
        work->out_result = result; 
        return;
    }

    /* query physical devices and decide which are to be used */
    select_rendering_devices(work);
    if (work->out_result != result_success) return;

    struct construct_device_work device_work = {
        .vk = &pelagia->vk,
        .temp_arena = &pelagia->scratch_arena,
        .frames_buffering = pelagia->frames_buffering,
        .thread_count = work->thread_count,
        .out_result = result_success,
    };

    /* create the logical rendering devices */
    for (u32 i = 0; i < pelagia->device_count; i++) {
        *pelagia->scratch_arena.end = scratch;
        device_work.device = &pelagia->devices[i];
        device_work.device_idx = i;
        construct_device(&device_work);

        if (device_work.out_result != result_success) {
            log_error("Creating device of idx '%d' failed, aborting.", i);
            work->out_result = device_work.out_result;
            return;
        }
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
    arena_reset(&pelagia->scratch_arena);

    struct rivens_tear assemble_tears[2];
    struct pelagia_assemble_render_targets_work assemble_render_targets = {
        .pelagia = pelagia,
        .dissasemble = false,
    };
    struct pelagia_assemble_uniform_buffers_work assemble_uniform_buffers = {
        .pelagia = pelagia,
        .dissasemble = false,
    };
    pelagia_assemble_render_targets_tear__(&assemble_render_targets, &assemble_tears[0]);
    pelagia_assemble_uniform_buffers_tear__(&assemble_uniform_buffers, &assemble_tears[1]);
    riven_split_and_unchain(riven, assemble_tears, 2);

    /* validate the work */
#define CHECKWORK(proc, idx) \
    if (assemble_##proc.out_result != result_success) { \
        log_error("Tear '%s' for primary device failed and returned %d.", assemble_tears[idx].name, assemble_##proc.out_result); \
        work->out_result = assemble_##proc.out_result; \
    }
    CHECKWORK(uniform_buffers, 0)
    CHECKWORK(render_targets, 1)
#undef CHECKWORK
    if (work->out_result != result_success) return;

    /* a tear per device */
    struct rivens_tear *device_tears = (struct rivens_tear *)arena_alloc(&pelagia->scratch_arena, sizeof(struct rivens_tear) * pelagia->device_count);

    /* A pipeline state is device-specific. We will handle one device at a time,
     * and assemble the pipelines for all available devices. */
    struct pelagia_assemble_render_pass_pipelines_work *assemble_render_pass_pipelines = (struct pelagia_assemble_render_pass_pipelines_work *)
        arena_alloc(&pelagia->scratch_arena, sizeof(struct pelagia_assemble_render_pass_pipelines_work) * pelagia->device_count);

    /* the pipelines can be assembled concurrently, so we do all devices at a time */
    for (u32 i = 0; i < pelagia->device_count; i++) {
        assemble_render_pass_pipelines[i] = (struct pelagia_assemble_render_pass_pipelines_work){
            .pelagia = pelagia,
            .riven = riven,
            .render_pass_type_mask = UINT64_MAX, /* assemble all pipelines */
            .device_idx = i,
            .out_result = result_success,
        };
        pelagia_assemble_render_pass_pipelines_tear__(&assemble_render_pass_pipelines[i], &device_tears[i]);
    }
    riven_split_and_unchain(riven, device_tears, pelagia->device_count);

    /* check work for errors */
    for (u32 i = 0; i < pelagia->device_count; i++) {
        if (assemble_render_pass_pipelines[i].out_result != result_success) {
            log_error("Failed to assemble all shader pipelines at initialization.");
            work->out_result = assemble_render_pass_pipelines[i].out_result;
        }
    }
}

AMWAPI void pelagia_renderer_fini(struct pelagia_renderer_fini_work *work)
{
    if (!work) return;

    struct pelagia *pelagia = work->pelagia;
    struct riven   *riven   = work->riven;
    if (!pelagia) return;

    /* handle individual devices in reverse order */
    for (s32 i = pelagia->device_count-1; i >= 0; i--) {
        struct vulkan_device *device = &pelagia->devices[i];

        /* if no device was created, we can skip this step */
        if (device->logical == VK_NULL_HANDLE) continue;

        device->api.vkDeviceWaitIdle(device->logical);

        struct pelagia_assemble_render_pass_pipelines_work dissasemble_pipelines_work = {
            .pelagia = pelagia,
            .riven = riven,
            .render_pass_type_mask = UINT64_MAX, /* dissasemble all pipelines */
            .dissasemble = true,
            .device_idx = i,
        };
        pelagia_assemble_render_pass_pipelines(&dissasemble_pipelines_work);

        if (at_read_relaxed(&device->flags) & pelagia_device_flag_primary) {
            if (pelagia->swapchain.sc != VK_NULL_HANDLE) {
                clear_swapchain(&pelagia->swapchain, device);
                device->api.vkDestroySwapchainKHR(device->logical, pelagia->swapchain.sc, NULL);
            }
            struct rivens_tear disassemble_tears[2];
            struct pelagia_assemble_render_targets_work disassemble_render_targets = {
                .pelagia = pelagia,
                .dissasemble = true,
            };
            struct pelagia_assemble_uniform_buffers_work disassemble_uniform_buffers = {
                .pelagia = pelagia,
                .dissasemble = true,
            };
            pelagia_assemble_render_targets_tear__(&disassemble_render_targets, &disassemble_tears[0]);
            pelagia_assemble_uniform_buffers_tear__(&disassemble_uniform_buffers, &disassemble_tears[1]);
            riven_split_and_unchain(riven, disassemble_tears, 2);
        } else {
            /* XXX handle secondary devices */
        }

        if (device->raw_command_pools) {
            for (u32 j = 0; j < device->raw_command_pool_count; j++)
                device->api.vkDestroyCommandPool(device->logical, device->raw_command_pools[j], NULL);
            free(device->raw_command_pools); /* TODO ia */
        }

        if (device->compute_queues && device->compute_queue_family_idx != device->graphics_queue_family_idx)
            free(device->compute_queues);
        if (device->transfer_queues && device->transfer_queue_family_idx != device->graphics_queue_family_idx)
            free(device->transfer_queues);

        log_info("Destroying a Vulkan rendering device idx %u: %s", i, device->physical_properties.deviceName);
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
