#include <amw/bits.h>
#include <amw/hadal.h>
#include <amw/harridan.h>
#include <amw/log.h>
#include <amw/riven.h>
#include <amw/random.h>

#include <stdio.h>  /* snprintf */
#include <string.h> /* strcmp */

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_utils_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackdata,
    void                                       *userdata)
{
    (void)userdata; (void)type;

    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            log_debug("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            log_verbose("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            log_warn("%s", callbackdata->pMessage);
            break;
        default:
            log_error("%s", callbackdata->pMessage);
            assert_debug(!"Vulkan validation error !!");
    }
    return VK_FALSE;
}

static void create_validation_layers(
    struct vk_api            *api,
    VkInstance                instance,
    VkAllocationCallbacks    *callbacks,
    VkDebugUtilsMessengerEXT *messenger)
{
    if (instance == VK_NULL_HANDLE || *messenger != VK_NULL_HANDLE) return;

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
        .pUserData = NULL,
    };

    if (api->vkCreateDebugUtilsMessengerEXT(instance, &messenger_info, callbacks, messenger) != VK_SUCCESS) {
        api->extensions &= ~vk_extension_layer_validation_bit;
        return;
    }
    log_debug("Created Vulkan validation layers.");
}

static void destroy_validation_layers(
    struct vk_api            *api,
    VkInstance                instance,
    VkAllocationCallbacks    *callbacks,
    VkDebugUtilsMessengerEXT  messenger)
{
    if (instance == VK_NULL_HANDLE || messenger == VK_NULL_HANDLE) return;
    
    api->vkDestroyDebugUtilsMessengerEXT(instance, messenger, callbacks);
    messenger = VK_NULL_HANDLE;
    log_debug("Destroyed Vulkan validation layers.");
}

#if 0
/** https://docs.vulkan.org/spec/latest/chapters/memory.html#PFN_vkAllocationFunction
 *
 *  typedef enum VkSystemAllocationScope {
 *      VK_SYSTEM_ALLOCATION_SCOPE_COMMAND = 0,
 *      VK_SYSTEM_ALLOCATION_SCOPE_OBJECT = 1,
 *      VK_SYSTEM_ALLOCATION_SCOPE_CACHE = 2,
 *      VK_SYSTEM_ALLOCATION_SCOPE_DEVICE = 3,
 *      VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE = 4,
 *  } VkSystemAllocationScope;
 *
 *  VK_SYSTEM_ALLOCATION_SCOPE_COMMAND specifies that the allocation is scoped 
 *  to the duration of the Vulkan command.

 *  VK_SYSTEM_ALLOCATION_SCOPE_OBJECT specifies that the allocation is scoped 
 *  to the lifetime of the Vulkan object that is being created or used.

 *  VK_SYSTEM_ALLOCATION_SCOPE_CACHE specifies that the allocation is scoped 
 *  to the lifetime of a VkPipelineCache or VkValidationCacheEXT object.

 *  VK_SYSTEM_ALLOCATION_SCOPE_DEVICE specifies that the allocation is scoped 
 *  to the lifetime of the Vulkan device.

 *  VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE specifies that the allocation is scoped 
 *  to the lifetime of the Vulkan instance.
 */
static void *allocation_function(
    void                   *harridan_raw,
    usize                   size,
    usize                   alignment,
    VkSystemAllocationScope allocation_scope)
{
    /* TODO */
}

static void *reallocation_function(
    void                   *harridan_raw,
    void                   *original,
    usize                   size,
    usize                   alignment,
    VkSystemAllocationScope allocation_scope)
{
    /* TODO */
}

static void free_function(
    void                   *harridan_raw,
    void                   *memory)
{
    /* TODO */
}
#endif

static const char *device_type_string(VkPhysicalDeviceType type)
{
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "integrated";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "discrete";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "virtual";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "cpu";
        default:                                     return "unknown";
    }
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
        default:     return "unknown";
    }
}

static s32 create_instance(
    struct harridan        *harridan,
    const char             *app_name,
    u32                     app_version,
    const char             *engine_name,
    u32                     engine_version,
    b32                     verbose,
    b32                     debug_utilities)
{
    u32 instance_version = 0, layer_count = 0;
    u32 extension_count = 0;
    u64 extension_bits = 0;
    const char **extensions = NULL;

    /* instance extensions */
    const char *ext_surface             = "VK_KHR_surface";
    const char *ext_win32_surface       = "VK_KHR_win32_surface";
    const char *ext_metal_surface       = "VK_EXT_metal_surface";
    const char *ext_android_surface     = "VK_KHR_android_surface";
    const char *ext_wayland_surface     = "VK_KHR_wayland_surface";
    const char *ext_xcb_surface         = "VK_KHR_xcb_surface";
    const char *ext_headless_surface    = "VK_EXT_headless_surface";
    const char *ext_display             = "VK_KHR_display";
    const char *ext_debug_utils         = "VK_EXT_debug_utils";

    harridan->api.vkEnumerateInstanceVersion(&instance_version);
    harridan->api.vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (instance_version < VK_MAKE_API_VERSION(0,1,2,0)) {
        log_error("Harridan: a minimum viable Vulkan version is 1.2 core."
                  "Your instance API version is %u.%u.%u, please update your drivers.", 
                  (instance_version >> 22u), (instance_version > 12u) & 0x3ffu, (instance_version & 0xfffu));
        return result_error;
    }

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = app_name,
        .applicationVersion = app_version,
        .pEngineName = engine_name,
        .engineVersion = engine_version,
        .apiVersion = instance_version,
    };

    VERIFY_VK(harridan->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (extension_count) {
        VkExtensionProperties *properties = (VkExtensionProperties *)
            riven_alloc(harridan->riven, rivens_tag_rendering, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
        VERIFY_VK(harridan->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, properties));

        u32 i = 0;
        for (; i < extension_count; i++) {
            if (!strcmp(properties[i].extensionName, ext_surface)) {
                extension_bits |= vk_extension_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_win32_surface)) {
                extension_bits |= vk_extension_win32_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_metal_surface)) {
                extension_bits |= vk_extension_metal_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_android_surface)) {
                extension_bits |= vk_extension_android_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_wayland_surface)) {
                extension_bits |= vk_extension_wayland_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_xcb_surface)) {
                extension_bits |= vk_extension_xcb_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_headless_surface)) {
                extension_bits |= vk_extension_headless_surface_bit;
            } else if (!strcmp(properties[i].extensionName, ext_display)) {
                extension_bits |= vk_extension_display_bit;
            } else if (debug_utilities && !strcmp(properties[i].extensionName, ext_debug_utils)) {
                extension_bits |= vk_extension_debug_utils_bit;
            }
        }
        i = 0; extension_count = bits_popcnt_lookup((const u8 *)&extension_bits, sizeof(extension_bits));

        extensions = (const char **)riven_alloc(harridan->riven, rivens_tag_rendering, sizeof(const char *) * extension_count, _Alignof(const char *));
        if (extension_bits & vk_extension_surface_bit)
            extensions[i++] = ext_surface;
        if (extension_bits & vk_extension_win32_surface_bit)
            extensions[i++] = ext_win32_surface;
        if (extension_bits & vk_extension_metal_surface_bit)
            extensions[i++] = ext_metal_surface;
        if (extension_bits & vk_extension_android_surface_bit)
            extensions[i++] = ext_android_surface;
        if (extension_bits & vk_extension_wayland_surface_bit)
            extensions[i++] = ext_wayland_surface;
        if (extension_bits & vk_extension_xcb_surface_bit)
            extensions[i++] = ext_xcb_surface;
        if (extension_bits & vk_extension_headless_surface_bit)
            extensions[i++] = ext_headless_surface;
        if (extension_bits & vk_extension_display_bit)
            extensions[i++] = ext_display;
        if (extension_bits & vk_extension_debug_utils_bit)
            extensions[i++] = ext_debug_utils;

        if (verbose) {
            char buffer[256];
            usize size = 256;
            usize offset = snprintf(buffer, size, "Vulkan instance extensions enabled:");

            for (i = 0; i < extension_count; i++)
                offset += snprintf(buffer + offset, size - offset, "\n\t%s", extensions[i]);

            buffer[offset] = '\0';
            log_verbose("%s", buffer);
        }

    } else {
        log_error("No Vulkan instance extensions available. Can't continue.");
        return result_error;
    }

    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (const char * const *)extensions,
    };

    VkValidationFeatureEnableEXT validation_feature_enable[] = {
        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
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
        "VK_LAYER_KHRONOS_validation",
    };

    /* use validation layers if requested */
    if (layer_count && extension_bits & vk_extension_debug_utils_bit) {
        extension_bits |= vk_extension_layer_validation_bit;
        instance_info.pNext = &validation_features;
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = (const char * const *)validation_layers;
        atomic_fetch_or_explicit(&harridan->flags, harridan_flag_debug_utils, memory_order_relaxed);
    }
    harridan->api.extensions |= extension_bits;

    VERIFY_VK(harridan->api.vkCreateInstance(&instance_info, NULL, &harridan->instance));
    if (!load_vulkan_instance_procedures(&harridan->api, harridan->instance)) {
        log_error("Loading Vulkan instance procedures failed.");
        return result_error;
    }

    if (extension_bits & vk_extension_layer_validation_bit)
        create_validation_layers(&harridan->api, harridan->instance, NULL, &harridan->debug_messenger);
    return result_success;
}

static s32 select_physical_device(void);

static s32 create_rendering_device(
    struct harridan *harridan,
    s32              select_device_index,
    b32              verbose)
{
    /* device extensions */
    const char *ext_swapchain                       = "VK_KHR_swapchain";
    const char *ext_device_fault                    = "VK_EXT_device_fault";
    const char *ext_memory_budget                   = "VK_EXT_memory_budget";
    const char *ext_memory_priority                 = "VK_EXT_memory_priorty";
    const char *ext_mesh_shader                     = "VK_EXT_mesh_shader";
    const char *ext_fragment_shading_rate           = "VK_KHR_fragment_shading_rate";
    const char *ext_deferred_host_operations        = "VK_KHR_deferred_host_operations";
    const char *ext_acceleration_structure          = "VK_KHR_acceleration_structure";
    const char *ext_pipeline_library                = "VK_KHR_pipeline_library";
    const char *ext_ray_query                       = "VK_KHR_ray_query";
    const char *ext_ratracing_pipeline              = "VK_KHR_ray_tracing_pipeline";
    const char *ext_ratracing_maintenance1          = "VK_KHR_ray_tracing_maintenance1";
    const char *ext_video_queue                     = "VK_KHR_video_queue";
    const char *ext_video_encode_queue              = "VK_KHR_video_encode_queue";
    const char *ext_video_decode_queue              = "VK_KHR_video_decode_queue";
    const char *ext_amd_device_coherent_memory      = "VK_AMD_device_coherent_memory";

    /* backwards compatibility extensions */
    const char *ext_dynamic_rendering_local_read    = "VK_KHR_dynamic_rendering_local_read";
    const char *ext_dynamic_rendering               = "VK_KHR_dynamic_rendering";
    const char *ext_synchronization2                = "VK_KHR_synchronization2";
    const char *ext_maintenance5                    = "VK_KHR_maintenance5";
    const char *ext_maintenance4                    = "VK_KHR_maintenance4";

    return 0;
}

s32 harridan_init(
    struct harridan        *harridan,
    struct hadopelagic     *hadal,
    struct rivens          *riven,
    rivens_tag_t            tag,
    const char             *app_name,
    u32                     app_version,
    const char             *engine_name,
    u32                     engine_version,
    s32                     select_device_index,
    b32                     verbose,
    b32                     debug_utilities)
{
    s32 res = 0;

    assert_debug(hadal->interface.create_vulkan_surface);

    if (atomic_load_explicit(&harridan->flags, memory_order_relaxed) & harridan_flag_device_is_valid) {
        log_warn("Harridan: the rendering device is already initialized.");
        return result_success;
    }

    harridan->riven = riven;
    harridan->device_tag = tag;
    harridan->render_state_tag = 0xFFF4321; /* TODO random */
    if (!open_vulkan_driver(&harridan->api, verbose || debug_utilities)) {
        log_error("Harridan: ensure the Vulkan drivers are correctly installed on your system.");
        return result_error;
    }

    res = create_instance(harridan, app_name, app_version, engine_name, engine_version, verbose, debug_utilities);
    if (res != result_success) {
        log_error("Harridan: failed to create a Vulkan instance.");
        return res;
    }

    res = hadal->interface.create_vulkan_surface(hadal, harridan, &harridan->swapchain);
    if (res != result_success) {
        log_error("Harridan: failed to create a Vulkan surface from the display backend: %u id, %s.", 
            hadal->interface.id, hadal->interface.name.ptr);
        return res;
    }

    res = create_rendering_device(harridan, select_device_index, verbose);

    atomic_fetch_or_explicit(&harridan->flags, harridan_flag_device_is_valid, memory_order_relaxed);
    return result_success;
}

void harridan_fini(struct harridan *harridan)
{
    if (harridan->swapchain.surface != VK_NULL_HANDLE)
        harridan->api.vkDestroySurfaceKHR(harridan->instance, harridan->swapchain.surface, NULL);

    if (harridan->api.extensions & vk_extension_layer_validation_bit)
        destroy_validation_layers(&harridan->api, harridan->instance, NULL, harridan->debug_messenger);

    if (harridan->instance)
        harridan->api.vkDestroyInstance(harridan->instance, NULL);
    close_vulkan_driver(&harridan->api);
    zerop(harridan);
}
