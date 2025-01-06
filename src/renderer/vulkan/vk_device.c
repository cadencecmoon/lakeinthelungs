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
    int32_t p, v;
    const char *typestr;

    (void)userdata;

    p = (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0 ? 1 : 0;
    v = (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0 ? 1 : 0;

    if (p && !v) typestr = "(performance)";
    else if (p &&  v) typestr = "(performance and validation)";
    else if (v) typestr = "(validation)";
    else typestr = "";

    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        log_info("Vulkan: %s %s", typestr, callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_debug("Vulkan: %s %s", typestr, callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("Vulkan: %s %s", typestr, callbackdata->pMessage);
        break;
    default:
        log_error("Vulkan: %s %s", typestr, callbackdata->pMessage);
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

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = application_name,
        .pEngineName = "A Moonlit Walk Engine",
        .engineVersion = AMW_VERSION,
        .applicationVersion = application_version,
        .apiVersion = instance_version,
    };

    vk->api.vkEnumerateInstanceVersion(&instance_version);
    vk->api.vkEnumerateInstanceLayerProperties(&layer_count, NULL);

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
    co->device_count = 0;
    co->devices = NULL;

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
    u32 main_device_idx = 0;
    if (!co->renderer) 
        return result_error_invalid_engine_context;
    return result_success;

    vulkan_backend *vk = (vulkan_backend *)co->renderer;

    VERIFY_VK(vk->api.vkEnumeratePhysicalDevices(vk->instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error("No physical GPU devices are available to Vulkan.");
        return result_error_undefined; /* TODO */
    }
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)arena_alloc(temp_arena, sizeof(VkPhysicalDevice) * physical_device_count);
    VERIFY_VK(vk->api.vkEnumeratePhysicalDevices(vk->instance, &physical_device_count, physical_devices));

    preferred_main_device_idx = preferred_main_device_idx < (s32)physical_device_count ? preferred_main_device_idx : -1;
    max_device_count = min(max_device_count, (s32)physical_device_count);

    void *raw_helpers = arena_alloc(temp_arena, sizeof(u32) * 2 * physical_device_count);

    u32 *physical_device_indices = (u32 *)raw_helpers; 
    raw_helpers += sizeof(s32) * physical_device_count;
    u32 *extension_bits = (u32 *)raw_helpers;

    region scratch = *temp_arena->end;
    for (u32 i = 0; (i < physical_device_count) && (max_device_count > 0 ? max_device_count : true); i++) {
        VkPhysicalDeviceProperties physical_device_properties;
        VkQueueFamilyProperties *queue_family_properties;
        VkExtensionProperties *extension_properties;
        u32 queue_family_count = 0, extension_count = 0;

        physical_device_indices[i] = UINT32_MAX;
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

        /* don't be picky */
        b32 has_best_properties = physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    }

    return result_success;
}

AMWAPI void cobalt_vulkan_destroy_devices(cobalt *co)
{
    if (!co->renderer || !co->devices) return;

    return;
}
