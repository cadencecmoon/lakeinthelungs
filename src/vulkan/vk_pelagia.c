#include "vk_pelagia.h"

#include <amw/bits.h>
#include <amw/log.h>
#include <amw/process.h>

const char *vulkan_result_string(VkResult result)
{
    switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "host memory allocation has failed.";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "device memory allocation has failed.";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "initialization of an object could not be completed for implementation-specific reasons.";
		case VK_ERROR_DEVICE_LOST:
			return "the logical or physical device has been lost.";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "mapping of a memory object has failed.";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "a requested layer is not present or could not be loaded.";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "a requested extension is not supported.";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "a requested feature is not supported.";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "the requested version of Vulkan is not supported by the driver or is otherwise "
			       "incompatible for implementation-specific reasons.";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "too many objects of the type have already been created.";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "a requested format is not supported on this device.";
		case VK_ERROR_FRAGMENTED_POOL:
			return "a pool allocation has failed due to fragmentation of the pool's memory.";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "a pool memory allocation has failed.";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "an external handle is not a valid handle of the specified type.";
		case VK_ERROR_FRAGMENTATION:
			return "a descriptor pool creation has failed due to fragmentation.";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "a buffer creation or memory allocation failed because the requested address is not available.";
		case VK_PIPELINE_COMPILE_REQUIRED:
			return "a requested pipeline creation would have required compilation, but the application requested compilation to not be performed.";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "a surface is no longer available.";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "the requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
		case VK_SUBOPTIMAL_KHR:
			return "a swapchain no longer matches the surface properties exactly, but can still be used to present"
			       "to the surface successfully.";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "a surface has changed in such a way that it is no longer compatible with the swapchain, "
			       "any further presentation requests using the swapchain will fail.";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "the display used by a swapchain does not use the same presentable image layout, or is "
			       "incompatible in a way that prevents sharing an image.";
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:
			return "one or more shaders failed to compile or link";
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		case VK_ERROR_NOT_PERMITTED_KHR:
			return "VK_ERROR_NOT_PERMITTED_KHR";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			return "an operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as "
			       "it did not have exlusive full-screen access.";
		case VK_THREAD_IDLE_KHR:
			return "a deferred operation is not complete but there is currently no work for this thread to do at the time of this call.";
		case VK_THREAD_DONE_KHR:
			return "a deferred operation is not complete but there is no work remaining to assign to additional threads.";
		case VK_OPERATION_DEFERRED_KHR:
			return "a deferred operation was requested and at least some of the work was deferred.";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "a deferred operation was requested and no operations were deferred.";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "an image creation failed because internal resources required for compression are exhausted."
			       "this must only be returned when fixed-rate compression is requested.";
        default:
            return "an unknown error has occured.";
    }
}

b32 vulkan_query_extension(
    VkExtensionProperties *properties, 
    u32                    count, 
    const char            *ext)
{
    for (u32 i = 0; i < count; i++)
        if (!strcmp(properties[i].extensionName, ext))
            return true;
    return false;
}

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

static void create_validation_layers(struct pelagia *pelagia)
{
    if (pelagia->instance == VK_NULL_HANDLE || pelagia->debug_messenger) return;

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

    if (pelagia->vkCreateDebugUtilsMessengerEXT(pelagia->instance, &messenger_info, NULL, &pelagia->debug_messenger) != VK_SUCCESS)
        return;
    log_verbose("Created Vulkan validation layers.");
}

static PFN_vkVoidFunction instance_proc_address(
    const struct pelagia *pelagia, 
    const char           *procname)
{
    PFN_vkVoidFunction address = pelagia->vkGetInstanceProcAddr(pelagia->instance, procname);
    if (address == NULL)
        log_warn("Pelagia 'vulkan' can't find instance procedure of name '%s'.", procname);
    return address;
}

/** We allow only one Vulkan backend at a time. */
static struct pelagia *g_vk_pelagia = NULL;

static void vulkan_interface_fini(struct pelagia *pelagia)
{
    if (!pelagia) return;

    if (pelagia->debug_messenger) {
        pelagia->vkDestroyDebugUtilsMessengerEXT(pelagia->instance, pelagia->debug_messenger, NULL);
        pelagia->debug_messenger = VK_NULL_HANDLE;
        log_verbose("Destroyed Vulkan validation layers.");
    }

    if (pelagia->instance)
        pelagia->vkDestroyInstance(pelagia->instance, NULL);
    if (pelagia->module)
        process_close_dll(pelagia->module);
    zerop(pelagia);
    g_vk_pelagia = NULL;
}

RIVEN_ENCORE(pelagia, vulkan) {
    assert_debug(create_info->header.interface && *create_info->header.interface == NULL);

    const char *fmt = "'pelagia_encore_vulkan' %s";
    void *module = NULL;
    u32 instance_version = 0, layer_count = 0;
    u32 extension_count = 0;
    u32 extension_bits = 0;
    const char **extensions = NULL;

    /* we allow only one vulkan backend at a time, so the interface will be shared */
    if (UNLIKELY(g_vk_pelagia != NULL)) {
        *create_info->header.interface = (riven_argument_t)g_vk_pelagia;
        return;
    }

#if defined(PLATFORM_WINDOWS)
    module = process_load_dll("vulkan-1.dll");
#elif defined(PLATFORM_APPLE)
    module = process_load_dll("libvulkan.dylib");
    if (!module)
        module = process_load_dll("libvulkan.1.dylib");
    if (!module)
        module = process_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (!module)
        module = process_load_dll("vulkan.framework/vulkan");
    if (!module)
        module = process_load_dll("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (!module)
        module = process_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    module = process_load_dll("libvulkan.so.1");
    if (!module)
        module = process_load_dll("libvulkan.so");
#endif
    if (!module) {
        log_error(fmt, "can't open the Vulkan drivers, ensure they are correclty installed and available via the system PATH");
        return;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)process_get_address(module, "vkGetInstanceProcAddr");
    if (vkGetInstanceProcAddr == NULL) {
        process_close_dll(module);
        log_error(fmt, "can't get the address of vkGetInstanceProcAddr from Vulkan drivers");
        return;
    }

    PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");

    if (!vkCreateInstance || !vkEnumerateInstanceVersion || !vkEnumerateInstanceExtensionProperties || !vkEnumerateInstanceLayerProperties) {
        process_close_dll(module);
        log_error(fmt, "can't get addresses of global procedures from Vulkan drivers");
        return;
    }

    /* check the API version */
    vkEnumerateInstanceVersion(&instance_version);
    if (instance_version < VK_API_VERSION_1_2) {
        process_close_dll(module);
        log_error(fmt, "outdated drivers");
        log_info("We target a minimum of Vulkan 1.2 core. Your drivers API version is %u.%u.%u, please update your drivers.",
            (instance_version >> 22u), (instance_version >> 12u) & 0x3ffu, (instance_version & 0xfffu));
        return;
    }
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (!extension_count) {
        process_close_dll(module);
        log_error(fmt, "no instance extensions available, we can't continue");
        return;
    }

    /* allocate the interface */
    struct pelagia *pelagia = (struct pelagia *)
        riven_alloc(create_info->header.riven, create_info->header.tag, sizeof(struct pelagia), _Alignof(struct pelagia));
    zerop(pelagia);

    pelagia->module = module;
    pelagia->vkCreateInstance = vkCreateInstance;
    pelagia->vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    pelagia->vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    pelagia->vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    pelagia->vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;

    VkExtensionProperties *extension_properties = (VkExtensionProperties *)
        riven_alloc(create_info->header.riven, riven_tag_deferred, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_properties));

    /* query instance extensions */
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_surface_bit;
#if defined(PLATFORM_WINDOWS)
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_win32_surface_bit;
#elif defined(PLATFORM_APPLE)
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_METAL_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_metal_surface_bit;
#elif defined(PLATFORM_ANDROID)
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_android_surface_bit;
#endif
#ifdef HADAL_WAYLAND
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_wayland_surface_bit;
#endif
#ifdef HADAL_XCB
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_XCB_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_xcb_surface_bit;
#endif
#ifdef HADAL_DRMKMS
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DISPLAY_EXTENSION_NAME))
        extension_bits |= vulkan_ext_display_bit;
#endif
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME))
        extension_bits |= vulkan_ext_headless_surface_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        extension_bits |= vulkan_ext_debug_utils_bit;

    u32 o = 0;
    extension_count = bits_popcnt_lookup((const u8 *)&extension_bits, sizeof(extension_bits));
    extensions = (const char **)
        riven_alloc(create_info->header.riven, riven_tag_deferred, sizeof(const char *) * extension_count, _Alignof(const char *));
    if (extension_bits & vulkan_ext_surface_bit)
        extensions[o++] = VK_KHR_SURFACE_EXTENSION_NAME;
#if defined(PLATFORM_WINDOWS)
    if (extension_bits & vulkan_ext_win32_surface_bit)
        extensions[o++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(PLATFORM_APPLE)
    if (extension_bits & vulkan_ext_metal_surface_bit)
        extensions[o++] = VK_EXT_METAL_SURFACE_EXTENSION_NAME;
#elif defined(PLATFORM_ANDROID)
    if (extension_bits & vulkan_ext_android_surface_bit)
        extensions[o++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_WAYLAND
    if (extension_bits & vulkan_ext_wayland_surface_bit)
        extensions[o++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_XCB
    if (extension_bits & vulkan_ext_xcb_surface_bit)
        extensions[o++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_DRMKMS
    if (extension_bits & vulkan_ext_display_bit)
        extensions[o++] = VK_KHR_DISPLAY_EXTENSION_NAME;
#endif
    if (extension_bits & vulkan_ext_headless_surface_bit)
        extensions[o++] = VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME;
    if (extension_bits & vulkan_ext_debug_utils_bit)
        extensions[o++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = create_info->header.metadata->game_name.ptr,
        .applicationVersion = create_info->header.metadata->game_build_version,
        .pEngineName = create_info->header.metadata->engine_name.ptr,
        .engineVersion = create_info->header.metadata->engine_build_version,
        .apiVersion = instance_version,
    };

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
    if (layer_count && extension_count & vulkan_ext_debug_utils_bit) {
        extension_bits |= vulkan_ext_layer_validation_bit;
        instance_info.pNext = &validation_features;
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }
    VERIFY_VK(vkCreateInstance(&instance_info, NULL, &pelagia->instance));

    /* core instance procedures */
    pelagia->vkCreateDevice = (PFN_vkCreateDevice)instance_proc_address(pelagia, "vkCreateDevice");
    pelagia->vkDestroyInstance = (PFN_vkDestroyInstance)instance_proc_address(pelagia, "vkDestroyInstance");
    pelagia->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)instance_proc_address(pelagia, "vkEnumerateDeviceExtensionProperties");
    pelagia->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)instance_proc_address(pelagia, "vkEnumeratePhysicalDeviceGroups");
    pelagia->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)instance_proc_address(pelagia, "vkEnumeratePhysicalDevices");
    pelagia->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)instance_proc_address(pelagia, "vkGetDeviceProcAddr");
    pelagia->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)instance_proc_address(pelagia, "vkGetPhysicalDeviceFeatures");
    pelagia->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)instance_proc_address(pelagia, "vkGetPhysicalDeviceFeatures2");
    pelagia->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)instance_proc_address(pelagia, "vkGetPhysicalDeviceFormatProperties");
    pelagia->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)instance_proc_address(pelagia, "vkGetPhysicalDeviceFormatProperties2");
    pelagia->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)instance_proc_address(pelagia, "vkGetPhysicalDeviceImageFormatProperties");
    pelagia->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)instance_proc_address(pelagia, "vkGetPhysicalDeviceImageFormatProperties2");
    pelagia->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instance_proc_address(pelagia, "vkGetPhysicalDeviceMemoryProperties");
    pelagia->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)instance_proc_address(pelagia, "vkGetPhysicalDeviceMemoryProperties2");
    pelagia->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)instance_proc_address(pelagia, "vkGetPhysicalDeviceProperties");
    pelagia->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)instance_proc_address(pelagia, "vkGetPhysicalDeviceProperties2");
    pelagia->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)instance_proc_address(pelagia, "vkGetPhysicalDeviceQueueFamilyProperties");
    pelagia->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)instance_proc_address(pelagia, "vkGetPhysicalDeviceQueueFamilyProperties2");

    /* ensure there are any physical devices available */
    u32 physical_device_count = 0;
    VERIFY_VK(pelagia->vkEnumeratePhysicalDevices(pelagia->instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error(fmt, "no physical devices are available to Vulkan");
        return;
    }

    /* surface */
    pelagia->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instance_proc_address(pelagia, "vkDestroySurfaceKHR");
    pelagia->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceSurfaceSupportKHR");
    pelagia->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    pelagia->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    pelagia->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceSurfacePresentModesKHR");

    /* debug utils */
    pelagia->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)instance_proc_address(pelagia, "vkSetDebugUtilsObjectNameEXT");
    pelagia->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)instance_proc_address(pelagia, "vkSetDebugUtilsObjectTagEXT");
    pelagia->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkQueueBeginDebugUtilsLabelEXT");
    pelagia->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkQueueEndDebugUtilsLabelEXT");
    pelagia->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkQueueInsertDebugUtilsLabelEXT");
    pelagia->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkCmdBeginDebugUtilsLabelEXT");
    pelagia->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkCmdEndDebugUtilsLabelEXT");
    pelagia->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)instance_proc_address(pelagia, "vkCmdInsertDebugUtilsLabelEXT");
    pelagia->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)instance_proc_address(pelagia, "vkCreateDebugUtilsMessengerEXT");
    pelagia->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)instance_proc_address(pelagia, "vkDestroyDebugUtilsMessengerEXT");
    pelagia->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)instance_proc_address(pelagia, "vkSubmitDebugUtilsMessageEXT");

    /* video coding */
    pelagia->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    pelagia->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)instance_proc_address(pelagia, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");

    if (extension_bits & vulkan_ext_layer_validation_bit)
        create_validation_layers(pelagia);

    /* write the interface ;3 */
    pelagia->interface = (struct pelagia_interface){
        .header = {
            .name = str_init("pelagia_vulkan"),
            .riven = create_info->header.riven,
            .tag = create_info->header.tag,
            .fini = (PFN_riven_work)vulkan_interface_fini,
        },
#define WRITE_PFN(fn) \
        .fn = _pelagia_vulkan_##fn,
        WRITE_PFN(query_physical_devices)
        WRITE_PFN(create_device)
        WRITE_PFN(destroy_device)
        WRITE_PFN(allocate_device_memory)
        WRITE_PFN(free_device_memory)
        WRITE_PFN(create_buffers)
        WRITE_PFN(create_textures)
        WRITE_PFN(create_samplers)
        WRITE_PFN(create_shaders)
        WRITE_PFN(create_pipeline_layouts)
        WRITE_PFN(create_graphics_pipelines)
        WRITE_PFN(create_compute_pipelines)
        WRITE_PFN(create_raytracing_pipelines)
        WRITE_PFN(create_shader_binding_tables)
        WRITE_PFN(create_command_buffers)
        WRITE_PFN(create_descriptor_sets)
        WRITE_PFN(create_query_pools)
        WRITE_PFN(create_swapchains)
        WRITE_PFN(create_bottom_levels)
        WRITE_PFN(create_top_levels)
        WRITE_PFN(destroy_resources)
#undef WRITE_PFN
    };
    *create_info->header.interface = (riven_argument_t)pelagia;
    log_verbose("'%s' interface write.", pelagia->interface.header.name.ptr);
}

/* TODO move them into a gpu_allocator source file or some shit */
void _pelagia_vulkan_allocate_device_memory(struct pelagia_device_memory_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_free_device_memory(struct pelagia_device_memory *memory) { (void)memory; }

/* TODO i'll handle the resources later */
void _pelagia_vulkan_create_buffers(struct pelagia_buffers_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_textures(struct pelagia_textures_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_samplers(struct pelagia_samplers_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_shaders(struct pelagia_shaders_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_pipeline_layouts(struct pelagia_pipeline_layouts_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_graphics_pipelines(struct pelagia_graphics_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_compute_pipelines(struct pelagia_compute_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_raytracing_pipelines(struct pelagia_raytracing_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_shader_binding_tables(struct pelagia_shader_binding_tables_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_command_buffers(struct pelagia_command_buffers_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_descriptor_sets(struct pelagia_descriptor_sets_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_query_pools(struct pelagia_query_pools_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_swapchains(struct pelagia_swapchains_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_bottom_levels(struct pelagia_bottom_levels_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_top_levels(struct pelagia_top_levels_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_destroy_resources(struct pelagia_destroy_resources_work *work) { (void)work; }
