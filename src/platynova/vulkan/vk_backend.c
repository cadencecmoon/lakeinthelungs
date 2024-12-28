#include <lake/bedrock/log.h>
#include <lake/hadal.h>
#include <lake/ipomoea.h>

#include "vk_platynova.h"

#include <string.h> /* strcmp */

int32_t _platinum_vulkan_entry_point(struct platinum *plat)
{
    (void)plat;

    struct vulkan_instance_api api;
    if (!vulkan_open_driver(&api)) {
        log_error("Can't load the Vulkan drivers. This can be either because they are not installed, or because the shared Vulkan library is not added to the system path.");
        return result_error_undefined; /* TODO */
    }

    struct platinum_vulkan *plat_vk = (struct platinum_vulkan *)malloc(sizeof(struct platinum_vulkan));
    assert_debug(plat_vk);

    iazerop(plat_vk);
    plat_vk->api.module = api.module;
    plat_vk->api.vkGetInstanceProcAddr = api.vkGetInstanceProcAddr;
    plat_vk->api.vkCreateInstance = api.vkCreateInstance;
    plat_vk->api.vkEnumerateInstanceVersion = api.vkEnumerateInstanceVersion;
    plat_vk->api.vkEnumerateInstanceExtensionProperties = api.vkEnumerateInstanceExtensionProperties;
    plat_vk->api.vkEnumerateInstanceLayerProperties = api.vkEnumerateInstanceLayerProperties;
    plat->internal_plat = (void *)plat_vk;

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
        log_info("%s %s", typestr, callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_debug("%s %s", typestr, callbackdata->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("%s %s", typestr, callbackdata->pMessage);
        break;
    default:
        log_error("%s %s", typestr, callbackdata->pMessage);
        assertion(!"Vulkan Validation !!");
    }
    return VK_FALSE;
}

static void create_validation_layers(struct platinum_vulkan *plat_vk) 
{
    if (plat_vk->messenger != VK_NULL_HANDLE)
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
        .pUserData = plat_vk, /* XXX maybe used later? */ 
    };

    if (plat_vk->api.vkCreateDebugUtilsMessengerEXT(plat_vk->instance, &messenger_info, NULL, &plat_vk->messenger) != VK_SUCCESS) {
        plat_vk->extensions &= ~(vulkan_extension_layer_validation_bit | vulkan_extension_debug_utils_bit);
        return;
    }
    log_debug("Created Vulkan validation layers.");
}

static void destroy_validation_layers(struct platinum_vulkan *plat_vk)
{
    if (plat_vk->messenger == VK_NULL_HANDLE)
        return;

    plat_vk->api.vkDestroyDebugUtilsMessengerEXT(plat_vk->instance, plat_vk->messenger, NULL);
    plat_vk->messenger = VK_NULL_HANDLE;
    log_debug("Destroyed Vulkan validation layers.");
}
#endif /* validation layers */

int32_t _platinum_vulkan_init(
        void *internal_plat, 
        struct hadal *hadal, 
        const char *application_name, 
        uint32_t application_version,
        struct arena_allocator *temp_arena)
{
    uint32_t instance_version = 0, extension_count = 0, layer_count = 0;
    char **instance_extensions = NULL;
    struct platinum_vulkan *plat_vk = (struct platinum_vulkan *)internal_plat;

    plat_vk->api.vkEnumerateInstanceVersion(&instance_version);
    plat_vk->api.vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = application_name,
        .pEngineName = "A Moonlit Walk Engine",
        .engineVersion = AMW_VERSION,
        .applicationVersion = application_version,
        .apiVersion = instance_version,
    };

    VERIFY_VK(plat_vk->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));

    if (extension_count > 0) {
        VkExtensionProperties *extensions = (VkExtensionProperties *)arena_alloc(temp_arena, sizeof(VkExtensionProperties) * extension_count);
        VERIFY_VK(plat_vk->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions));

        uint32_t i;
        for (i = 0; i < extension_count; i++) {
            if (!strcmp(extensions[i].extensionName, "VK_KHR_surface")) {
                plat_vk->extensions |= vulkan_extension_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_win32_surface")) {
                plat_vk->extensions |= vulkan_extension_win32_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_EXT_metal_surface")) {
                plat_vk->extensions |= vulkan_extension_metal_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_android_surface")) {
                plat_vk->extensions |= vulkan_extension_android_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_wayland_surface")) {
                plat_vk->extensions |= vulkan_extension_wayland_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_xcb_surface")) {
                plat_vk->extensions |= vulkan_extension_xcb_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_headless_surface")) {
                plat_vk->extensions |= vulkan_extension_headless_surface_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_KHR_display")) {
                plat_vk->extensions |= vulkan_extension_display_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_EXT_direct_mode_display")) {
                plat_vk->extensions |= vulkan_extension_direct_mode_display_bit;
            } else if (!strcmp(extensions[i].extensionName, "VK_EXT_acquire_drm_display")) {
                plat_vk->extensions |= vulkan_extension_acquire_drm_display_bit;
#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
            } else if (!strcmp(extensions[i].extensionName, "VK_EXT_debug_utils")) {
                plat_vk->extensions |= vulkan_extension_debug_utils_bit;
#endif
            }
        }
        i = 0; extension_count = count_bits32(plat_vk->extensions);

        instance_extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
        if (plat_vk->extensions & vulkan_extension_surface_bit)
            instance_extensions[i++] = "VK_KHR_surface";
        if (plat_vk->extensions & vulkan_extension_win32_surface_bit)
            instance_extensions[i++] = "VK_KHR_win32_surface";
        if (plat_vk->extensions & vulkan_extension_metal_surface_bit)
            instance_extensions[i++] = "VK_EXT_metal_surface";
        if (plat_vk->extensions & vulkan_extension_android_surface_bit)
            instance_extensions[i++] = "VK_KHR_android_surface";
        if (plat_vk->extensions & vulkan_extension_wayland_surface_bit)
            instance_extensions[i++] = "VK_KHR_wayland_surface";
        if (plat_vk->extensions & vulkan_extension_xcb_surface_bit)
            instance_extensions[i++] = "VK_KHR_xcb_surface";
        if (plat_vk->extensions & vulkan_extension_headless_surface_bit)
            instance_extensions[i++] = "VK_KHR_headless_surface";
        if (plat_vk->extensions & vulkan_extension_display_bit)
            instance_extensions[i++] = "VK_KHR_display";
        if (plat_vk->extensions & vulkan_extension_direct_mode_display_bit)
            instance_extensions[i++] = "VK_EXT_direct_mode_display";
        if (plat_vk->extensions & vulkan_extension_acquire_drm_display_bit)
            instance_extensions[i++] = "VK_EXT_acquire_drm_display";
        if (plat_vk->extensions & vulkan_extension_debug_utils_bit)
            instance_extensions[i++] = "VK_EXT_debug_utils";

        log_debug("Enabled Vulkan instance extensions:");
        for (i = 0; i < extension_count; i++) {
            log_debug("  %3d %s", i, instance_extensions[i]);
        }
    } else {
        log_error("No Vulkan instance extensions were found.");
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
    log_debug("%p, %p", validation_layers, validation_layers[0]);

    if (layer_count > 0 && plat_vk->extensions & vulkan_extension_debug_utils_bit) {
        plat_vk->extensions |= vulkan_extension_layer_validation_bit;
        instance_create_info.pNext = &validation_features;
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }
#endif /* validation layers */

    VERIFY_VK(plat_vk->api.vkCreateInstance(&instance_create_info, NULL, &plat_vk->instance));
    vulkan_instance_api_load_procedures(&plat_vk->api, plat_vk->instance, &plat_vk->extensions);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    if (plat_vk->extensions & vulkan_extension_layer_validation_bit)    
        create_validation_layers(plat_vk);
#endif /* validation layers */

    VERIFY_VK(vulkan_create_surface(&plat_vk->api, plat_vk->instance, hadal, NULL, &plat_vk->swapchain.surface));

    return result_success;
}

void _platinum_vulkan_fini(void *internal)
{
    struct platinum_vulkan *plat_vk = (struct platinum_vulkan *)internal;

    if (plat_vk->swapchain.surface)
        plat_vk->api.vkDestroySurfaceKHR(plat_vk->instance, plat_vk->swapchain.surface, NULL);

#if !defined(AMW_NDEBUG) && defined(VK_EXT_debug_utils)
    destroy_validation_layers(plat_vk);
#endif /* validation layers */

    vulkan_close_driver(&plat_vk->api);
    free(plat_vk);
}
