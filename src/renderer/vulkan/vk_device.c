#include "vk_cobalt.h"

AMWAPI s32 cobalt_vulkan_entry_point(cobalt *co, ipomoeaalba *ia)
{
    (void)ia; // TODO

    vulkan_instance_api api;
    iazero(api);

    if (vulkan_open_driver(&api) == false)
        return result_error_missing_shared_library;

    vulkan_backend *vk = (vulkan_backend *)malloc(sizeof(vulkan_backend));
    assert_debug(vk);

    iazerop(vk);
    vk->api = api;

    /* TODO */
    //vk->allocator = (VkAllocationCallbacks){};

    co->backend_api = cobalt_backend_api_vulkan;
    co->backend_name = "vulkan";
    co->renderer = (void *)vk;

    co->calls = (cobalt_calls){
        .renderer_init = cobalt_vulkan_renderer_init,
        .renderer_fini = cobalt_vulkan_renderer_fini,
        .create_swapchain_surface = cobalt_vulkan_create_swapchain_surface,
        .construct_devices = cobalt_vulkan_construct_devices,
        .destroy_devices = cobalt_vulkan_destroy_devices,
    };

    return result_success;
}

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
        log_debug("Vulkan: %s", callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_trace("Vulkan: %s", callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("Vulkan: %s", callbackdata->pMessage);
        break;
    default:
        log_error("Vulkan: %s", callbackdata->pMessage);
        assertion(!"Vulkan validation error !!");
    }
    return VK_FALSE;
}

static void create_validation_layers(vulkan_backend *vk) 
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

static void destroy_validation_layers(vulkan_backend *vk)
{
    if (vk->messenger == VK_NULL_HANDLE)
        return;

    vk->api.vkDestroyDebugUtilsMessengerEXT(vk->instance, vk->messenger, NULL);
    vk->messenger = VK_NULL_HANDLE;
    log_debug("Destroyed Vulkan validation layers.");
}
#endif /* validation layers */

AMWAPI s32 cobalt_vulkan_renderer_init(
    cobalt          *co, 
    ipomoeaalba     *ia, 
    hadopelagic     *hadal, 
    const char      *application_name, 
    u32              application_version, 
    arena_allocator *temp_arena)
{
    u32 i, instance_version = 0, extension_count = 0, layer_count = 0;
    char **instance_extensions = NULL;
    vulkan_backend *vk = (vulkan_backend *)co->renderer;

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
        return result_error_undefined; // TODO
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
    vulkan_load_instance_api_procedures(&vk->api, vk->instance, vk->extensions);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    if (vk->extensions & vulkan_extension_layer_validation_bit)    
        create_validation_layers(vk);
#endif /* validation layers */

    s32 res = cobalt_vulkan_create_swapchain_surface(co, hadal);
    if (res != result_success) {
        log_error("Failed to create a Vulkan surface for the '%s' display backend.", hadal->backend_name);
        cobalt_vulkan_renderer_fini(co);
        return res;
    }

    /* TODO */
    (void)ia;

    return result_success;
}

AMWAPI void cobalt_vulkan_renderer_fini(cobalt *co)
{
    if (co->devices)
        cobalt_vulkan_destroy_devices(co);
    if (co->renderer) {
        vulkan_backend *vk = (vulkan_backend *)co->renderer;

        if (vk->swapchain.surface)
            vk->api.vkDestroySurfaceKHR(vk->instance, vk->swapchain.surface, NULL);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
        destroy_validation_layers(vk);
#endif /* validation layers */

        if (vk->instance)
            vk->api.vkDestroyInstance(vk->instance, NULL);

        vulkan_close_driver(&vk->api);
        free(vk);
    }

    co->backend_api = 0;
    co->renderer = NULL;
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

AMWAPI s32 cobalt_vulkan_construct_devices(
    cobalt          *co,
    rivens_rift     *riven,
    thread_id       *threads,
    ssize            thread_count,
    s32              preferred_main_device_idx,
    s32              max_device_count,
    arena_allocator *temp_arena)
{
    u32 physical_device_count = 0;
    u32 device_count = 0;
    s32 main_device_idx = -1;
    if (!co->renderer) 
        return result_error_invalid_engine_context;

    vulkan_backend *vk = (vulkan_backend *)co->renderer;

    /* TODO */
    (void)riven;
    (void)threads;

    VERIFY_VK(vk->api.vkEnumeratePhysicalDevices(vk->instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error("No physical GPU devices are available to Vulkan.");
        return result_error_undefined; /* TODO */
    }
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)arena_alloc(temp_arena, sizeof(VkPhysicalDevice) * physical_device_count);
    VERIFY_VK(vk->api.vkEnumeratePhysicalDevices(vk->instance, &physical_device_count, physical_devices));

    preferred_main_device_idx = preferred_main_device_idx < (s32)physical_device_count ? preferred_main_device_idx : -1;
    max_device_count = min(max_device_count, (s32)physical_device_count);

    void *raw_helpers = arena_alloc(temp_arena, sizeof(s32) * 2 * physical_device_count);

    s32 *physical_device_indices = (s32 *)raw_helpers; 
    raw_helpers += sizeof(s32) * physical_device_count;
    u32 *extension_bits = (u32 *)raw_helpers;

    region scratch = *temp_arena->end;
    for (u32 i = 0; (i < physical_device_count) && (max_device_count > 0 ? max_device_count : true); i++) {
        VkPhysicalDeviceProperties physical_device_properties;
        VkQueueFamilyProperties *queue_family_properties;
        VkExtensionProperties *extension_properties;
        u32 queue_family_count = 0, extension_count = 0;

        physical_device_indices[i] = -1;
        extension_bits[i] = 0u;
        *temp_arena->end = scratch;

        /* too old drivers? */
        vk->api.vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
        if (physical_device_properties.apiVersion < VK_MAKE_API_VERSION(0,1,1,0)) continue;

        /* no device extensions? */
        vk->api.vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &extension_count, NULL);
        if (extension_count == 0) continue;

        /* no queue families? */
        vk->api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, NULL);
        if (queue_family_count == 0) continue;

        queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(temp_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        vk->api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_family_properties);

        u32 command_support = 0u;
        for (u32 j = 0; j < queue_family_count; j++) {
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                command_support |= (1u << 0);
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT))
                command_support |= (1u << 1);
            if (queue_family_properties[j].queueCount > 0 && (queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT))
                command_support |= (1u << 2);
        }

        /* accept only devices with support for graphics, compute and transfer commands. */
        if ((command_support & 7) != 7) continue;

        extension_properties = (VkExtensionProperties *)arena_alloc(temp_arena, sizeof(VkExtensionProperties) * extension_count);
        vk->api.vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &extension_count, extension_properties);

        for (u32 j = 0; j < extension_count; j++) {
            if (!strcmp(extension_properties[j].extensionName, "VK_KHR_swapchain")) {
                extension_bits[i] |= vulkan_extension_swapchain_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_device_fault")) {
                extension_bits[i] |= vulkan_extension_device_fault_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_budget")) {
                extension_bits[i] |= vulkan_extension_memory_budget_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_EXT_memory_priority")) {
                extension_bits[i] |= vulkan_extension_memory_priority_bit;
            } else if (!strcmp(extension_properties[j].extensionName, "VK_AMD_shader_info")) {
                extension_bits[i] |= vulkan_extension_amd_shader_info_bit;
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
        if (preferred_main_device_idx == (s32)i || (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
                && preferred_main_device_idx < 0 && main_device_idx < 0) || physical_device_count == 1) 
        {
            /* check if suitable for use as a main device */
            b32 viable_for_main = (extension_bits[i] & vulkan_extension_swapchain_bit);

            /* TODO check for presentation support */

            if (viable_for_main) {
                main_device_idx = i;
            } else { 
                if (preferred_main_device_idx >= 0 && preferred_main_device_idx == (s32)i)
                    preferred_main_device_idx = -1;
            }
            physical_device_indices[i] = i;
            device_count++;
        } else if ((max_device_count <= 0) || (main_device_idx >= 0 ? max_device_count > (s32)device_count : max_device_count > (s32)(device_count - 1))) {
            physical_device_indices[i] = i;
            device_count++;
        }
    }

    /* no device was found ? */
    if (device_count == 0) {
        log_error("Could not query a physical device that meets our requirements.");
        return result_error_undefined; /* TODO */
    } else if (main_device_idx < 0 || main_device_idx >= (s32)physical_device_count) {
        main_device_idx = 0;
    }

    vulkan_device *devices = (vulkan_device *)malloc(sizeof(vulkan_device) * device_count);
    iamemset(devices, 0u, sizeof(vulkan_device) * device_count);
    co->devices = (void *)devices;
    co->device_count = device_count;

    /* setup physical device handles */
    devices[0].physical = physical_devices[main_device_idx];
    devices[0].extensions = extension_bits[main_device_idx];
    main_device_idx = physical_device_indices[main_device_idx] = -1; /* we don't need that anymore */
    for (u32 i = 1; i < device_count; i++) {
        devices[i].physical = VK_NULL_HANDLE;
        for (u32 j = 0; j < physical_device_count && devices[i].physical == VK_NULL_HANDLE; j++) {
            s32 idx = physical_device_indices[j];
            if (idx < 0) continue;

            devices[i].physical = physical_devices[idx]; 
            devices[i].extensions = extension_bits[idx];
            physical_device_indices[j] = -1;
        }
    }

    /* setup per device vulkan objects */
    for (u32 i = 0; i < device_count; i++) {
        f32 queue_priorities[1] = { 0.0f };
        u32 bits = 0, queue_family_count = 0;
        VkQueueFamilyProperties *queue_family_properties;
        u32 extension_count = 0;
        char **extensions = NULL;

        vulkan_device *device = &devices[i];
        *temp_arena->end = scratch;

        vk->api.vkGetPhysicalDeviceProperties(devices[i].physical, &devices[i].physical_properties);
        vk->api.vkGetPhysicalDeviceFeatures(devices[i].physical, &devices[i].physical_features);
        vk->api.vkGetPhysicalDeviceMemoryProperties(devices[i].physical, &devices[i].memory_properties);

        log_info("\nCreating a Vulkan rendering device:\n"
                 "    Name: %s\n"
                 "    Type: %s, ID: %X\n"
                 "    Vendor: %s, ID: %X\n"
                 "    Api version: %u.%u.%u",
                 device->physical_properties.deviceName,
                 device_type_string(device->physical_properties.deviceType),
                 device->physical_properties.deviceID,
                 vendor_name_string(device->physical_properties.vendorID),
                 device->physical_properties.vendorID,
                 (device->physical_properties.apiVersion >> 22U),
                 (device->physical_properties.apiVersion >> 12U) & 0x3ffU,
                 (device->physical_properties.apiVersion & 0xfffU));

        /* we already made sure the GPU in question supports the command queues we need */
        vk->api.vkGetPhysicalDeviceQueueFamilyProperties(devices[i].physical, &queue_family_count, NULL);
        queue_family_properties = (VkQueueFamilyProperties *)arena_alloc(temp_arena, queue_family_count * sizeof(VkQueueFamilyProperties) * queue_family_count);
        vk->api.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_family_properties);

        for (u32 j = 0; j < queue_family_count; j++) {
            /* we need a general graphics queue, don't be picky */
            if (device->graphics_queue == VK_NULL_HANDLE && queue_family_properties[j].queueCount > 0 
                && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
            {
                device->graphics_queue_family_idx = j;
            }
            /* ask for an async compute queue family */
            if (device->compute_queue_count == 0 && queue_family_properties[j].queueCount > 0
                && (queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                && ((queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                device->compute_queue_count = queue_family_properties[j].queueCount;
                device->compute_queue_family_idx = j;
            }
            /* ask for async transfer queue family */
            if (device->transfer_queue_count == 0 && queue_family_properties[j].queueCount > 0
                && (queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
                && ((queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
                && ((queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                device->transfer_queue_count = queue_family_properties[j].queueCount;
                device->transfer_queue_family_idx = j;
            }
        }

        VkDeviceQueueCreateInfo queue_create_infos[3];
        for (u32 j = 0; j < 3; j++) {
            queue_create_infos[j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[j].pNext = NULL;
            queue_create_infos[j].flags = 0;
            queue_create_infos[j].pQueuePriorities = queue_priorities;
            queue_create_infos[j].queueCount = 1;
            queue_create_infos[j].queueFamilyIndex = device->graphics_queue_family_idx;
        }

        queue_family_count = 3; /* we'll use this for how many different queue families should be used */
        if (device->compute_queue_count == 0) {
            device->compute_queues = &device->graphics_queue;
            device->compute_queue_family_idx = device->graphics_queue_family_idx;
            queue_family_count--;
        }
        if (device->transfer_queue_count == 0) {
            device->transfer_queues = &device->graphics_queue;
            device->transfer_queue_family_idx = device->graphics_queue_family_idx;
            queue_family_count--;
        }

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

        extension_count = count_bits32(device->extensions);
        extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
        if (device->extensions & vulkan_extension_swapchain_bit)
            extensions[bits++] = "VK_KHR_swapchain";
        if (device->extensions & vulkan_extension_device_fault_bit)
            extensions[bits++] = "VK_EXT_device_fault";
        if (device->extensions & vulkan_extension_memory_budget_bit)
            extensions[bits++] = "VK_EXT_memory_budget";
        if (device->extensions & vulkan_extension_memory_priority_bit)
            extensions[bits++] = "VK_EXT_memory_priority";
        if (device->extensions & vulkan_extension_amd_shader_info_bit)
            extensions[bits++] = "VK_AMD_shader_info";
        if (device->extensions & vulkan_extension_deferred_host_operations_bit)
            extensions[bits++] = "VK_KHR_deferred_host_operations";
        if (device->extensions & vulkan_extension_acceleration_structure_bit)
            extensions[bits++] = "VK_KHR_acceleration_structure";
        if (device->extensions & vulkan_extension_ray_query_bit)
            extensions[bits++] = "VK_KHR_ray_query";
        if (device->extensions & vulkan_extension_dynamic_rendering_local_read_bit)
            extensions[bits++] = "VK_KHR_dynamic_rendering_local_read";
        if (device->extensions & vulkan_extension_dynamic_rendering_bit)
            extensions[bits++] = "VK_KHR_dynamic_rendering";

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
        if (vulkan_load_device_api_procedures(&vk->api, &device->api, device->logical, device->physical_properties.apiVersion, device->extensions)) {
            log_error("Can't load the Vulkan API procedures for a device.");
            return result_error_undefined; /* TODO */
        }

        /* grab the command queues */
        device->api.vkGetDeviceQueue(device->logical, device->graphics_queue_family_idx, 0, &device->graphics_queue);
        if (device->compute_queue_count > 0) {
            assert_debug(device->compute_queues == NULL);
            device->compute_queues = malloc(sizeof(VkQueue) * device->compute_queue_count);
            iamemset(device->compute_queues, 0u, sizeof(VkQueue) * device->compute_queue_count);
            for (u32 j = 0; j < device->compute_queue_count; j++)
                device->api.vkGetDeviceQueue(device->logical, device->compute_queue_family_idx, j, &device->compute_queues[j]);
        }
        if (device->transfer_queue_count > 0) {
            assert_debug(device->transfer_queues == NULL);
            device->transfer_queues = malloc(sizeof(VkQueue) * device->transfer_queue_count);
            iamemset(device->transfer_queues, 0u, sizeof(VkQueue) * device->transfer_queue_count);
            for (u32 j = 0; j < device->transfer_queue_count; j++)
                device->api.vkGetDeviceQueue(device->logical, device->transfer_queue_family_idx, j, &device->transfer_queues[j]);
        }

        assert_debug(device->graphics_queue);
        assert_debug(device->compute_queues);
        assert_debug(device->transfer_queues);

        /* create the command cools per queue family */
        device->queue_command_pool_count = VULKAN_MAX_FRAMES * thread_count;
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
        for (u32 j = 0; j < device->queue_command_pool_count; j++)
            VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->graphics_command_pools[j]));

        /* resolve the compute queue family, if present */
        if (device->compute_queue_count == 0) {
            device->compute_command_pools = device->graphics_command_pools;
        } else {
            device->compute_command_pools = &device->raw_command_pools[device->queue_command_pool_count];
            for (u32 j = 0; j < device->queue_command_pool_count; j++)
                VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->compute_command_pools[j]));
        }

        /* resolve the transfer queue family, if present */
        if (device->transfer_queue_count == 0) {
            device->transfer_command_pools = device->graphics_command_pools;
        } else {
            device->transfer_command_pools = &device->raw_command_pools[device->raw_command_pool_count - device->queue_command_pool_count];
            for (u32 j = 0; j < device->queue_command_pool_count; j++)
                VERIFY_VK(device->api.vkCreateCommandPool(device->logical, &command_pool_create_info, NULL, &device->transfer_command_pools[j]));
        }

#if AMW_DEBUG
        for (u32 j = 0; j < device->raw_command_pool_count; j++) {
            assert_debug(device->raw_command_pools[j] != VK_NULL_HANDLE);
        }
#endif

        log_info("Created %d unique command queue families and %lu command pools (shared between %lu threads):\n"
                  "    graphics queues : 1  graphics queue family idx : %d   command pools : %lu\n"
                  "    compute queues  : %lu  compute queue family idx  : %d   command pools : %lu\n"
                  "    transfer queues : %lu  transfer queue family idx : %d   command pools : %lu", 
                queue_family_count, device->raw_command_pool_count, thread_count,
                device->graphics_queue_family_idx, 
                device->queue_command_pool_count,
                device->compute_queue_count, 
                device->compute_queue_family_idx, 
                device->compute_queue_count ? device->queue_command_pool_count : 0,
                device->transfer_queue_count, 
                device->transfer_queue_family_idx,
                device->transfer_queue_count ? device->queue_command_pool_count : 0);

        /* ray tracing mesh support */
        if ((device->extensions & vulkan_extension_mask_raytracing) == vulkan_extension_mask_raytracing) {
            device->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
            VkPhysicalDeviceProperties2 physical_device_properties2 = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                .pNext = &device->acceleration_structure_properties,
            };
            vk->api.vkGetPhysicalDeviceProperties2(device->physical, &physical_device_properties2);
            log_info("This device has hardware support for accelerated ray tracing.");
        }
    }

    return result_success;
}

AMWAPI void cobalt_vulkan_destroy_devices(cobalt *co)
{
    if (!co->devices)
        return;

    vulkan_device *devices = (vulkan_device *)co->devices;

    for (u32 i = 0; i < co->device_count; i++) {
        vulkan_device *device = &devices[i];

        if (device->logical == VK_NULL_HANDLE)
            continue;

        device->api.vkDeviceWaitIdle(device->logical);

        if (device->raw_command_pools) {
            for (u32 j = 0; j < device->raw_command_pool_count; j++)
                device->api.vkDestroyCommandPool(device->logical, device->raw_command_pools[j], NULL);
            free(device->raw_command_pools);
        }

        if (device->compute_queue_count && device->compute_queues)
            free(device->compute_queues);
        if (device->transfer_queue_count && device->transfer_queues)
            free(device->transfer_queues);

        log_info("Destroying a Vulkan rendering device: %s", device->physical_properties.deviceName);
        device->api.vkDestroyDevice(device->logical, NULL);
    }

    free(devices);
    co->devices = NULL;
    co->device_count = 0;
    return;
}
