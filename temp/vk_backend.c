#include "vk_pelagial.h"

#include <amw/bits.h>
#include <amw/log.h>
#include <amw/plugin.h>

#include <stdio.h>  /* snprintf */
#include <string.h> /* strcmp */

static struct vulkan_backend *g_vulkan = NULL;

PELAGIAL_ENTRY_POINT(vulkan)
{
    void *module = NULL;
    struct string name = string_init("vulkan");
    u32 id = pelagic_backend_vulkan;
    u32 instance_version = 0;

    interface->id = id;
    interface->name = name;

#if defined(PLATFORM_WINDOWS)
    module = plugin_load_dll("vulkan-1.dll");
#elif defined(PLATFORM_APPLE)
    module = plugin_load_dll("libvulkan.dylib");
    if (!module)
        module = plugin_load_dll("libvulkan.1.dylib");
    if (!module)
        module = plugin_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (!module)
        module = plugin_load_dll("vulkan.framework/vulkan");
    if (!module)
        module = plugin_load_dll("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (!module)
        module = plugin_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    module = plugin_load_dll("libvulkan.so.1");
    if (!module)
        module = plugin_load_dll("libvulkan.so");
#endif
    if (!module) {
        if (verbose)
            log_error("Can't open the Vulkan drivers. Ensure they are correclty installed and available via the system PATH.");
        return result_error;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)plugin_get_proc_address(module, "vkGetInstanceProcAddr");
    if (vkGetInstanceProcAddr == NULL) {
        plugin_close_dll(module);
        if (verbose)
            log_error("Can't retrieve address of vkGetInstanceProcAddr from Vulkan drivers.");
        return result_error;
    }

    PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");

    if (!vkCreateInstance || !vkEnumerateInstanceVersion || !vkEnumerateInstanceExtensionProperties || !vkEnumerateInstanceLayerProperties) {
        plugin_close_dll(module);
        if (verbose)
            log_error("Can't retrieve addresses of global procedures from Vulkan drivers.");
        return result_error;
    }

    vkEnumerateInstanceVersion(&instance_version);
    if (instance_version < VK_API_VERSION_1_2) {
        plugin_close_dll(module);
        if (verbose)
            log_error("A minimum viable Vulkan driver version is API 1.2 core."
                "Your API version is %u.%u.%u, please update your drivers.",
                (instance_version >> 22u), (instance_version >> 12u) & 0x3ffu, (instance_version & 0xfffu));
        return result_error;
    }

    struct vulkan_backend *vulkan = (struct vulkan_backend *)
        riven_alloc(create_info->riven, create_info->tag, sizeof(struct vulkan_backend), _Alignof(struct vulkan_backend));
    zerop(vulkan);

    vulkan->module = module;
    vulkan->vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkan->vkCreateInstance = vkCreateInstance;
    vulkan->vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;
    vulkan->vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    vulkan->vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    g_vulkan = vulkan;

    *interface = (struct pelagic_ocean_interface){
        .id = id,
        .name = name,
        .internal = vulkan,
        .renderer_init = pelagial_vulkan_renderer_init,
        .renderer_fini = pelagial_vulkan_renderer_fini,
    };
    return result_success;
}

const char *vulkan_result_string(VkResult result)
{
    switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "Host memory allocation has failed.";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "Device memory allocation has failed.";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "Initialization of an object could not be completed for implementation-specific reasons.";
		case VK_ERROR_DEVICE_LOST:
			return "The logical or physical device has been lost.";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "Mapping of a memory object has failed.";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "A requested layer is not present or could not be loaded.";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "A requested extension is not supported.";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "A requested feature is not supported.";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "The requested version of Vulkan is not supported by the driver or is otherwise "
			       "incompatible for implementation-specific reasons.";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "Too many objects of the type have already been created.";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "A requested format is not supported on this device.";
		case VK_ERROR_FRAGMENTED_POOL:
			return "A pool allocation has failed due to fragmentation of the pool's memory.";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "A pool memory allocation has failed.";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "An external handle is not a valid handle of the specified type.";
		case VK_ERROR_FRAGMENTATION:
			return "A descriptor pool creation has failed due to fragmentation.";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "A buffer creation or memory allocation failed because the requested address is not available.";
		case VK_PIPELINE_COMPILE_REQUIRED:
			return "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed.";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "A surface is no longer available.";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
		case VK_SUBOPTIMAL_KHR:
			return "A swapchain no longer matches the surface properties exactly, but can still be used to present"
			       "to the surface successfully.";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "A surface has changed in such a way that it is no longer compatible with the swapchain, "
			       "any further presentation requests using the swapchain will fail.";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "The display used by a swapchain does not use the same presentable image layout, or is "
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
			return "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as "
			       "it did not have exlusive full-screen access.";
		case VK_THREAD_IDLE_KHR:
			return "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call.";
		case VK_THREAD_DONE_KHR:
			return "A deferred operation is not complete but there is no work remaining to assign to additional threads.";
		case VK_OPERATION_DEFERRED_KHR:
			return "A deferred operation was requested and at least some of the work was deferred.";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "A deferred operation was requested and no operations were deferred.";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "An image creation failed because internal resources required for compression are exhausted."
			       "This must only be returned when fixed-rate compression is requested.";
        default:
            return "An unknown error has occured.";
    };
}

static PFN_vkVoidFunction instance_proc_address(
    const struct vulkan_backend *vulkan, 
    const char                  *procname)
{
    PFN_vkVoidFunction address = vulkan->vkGetInstanceProcAddr(vulkan->instance, procname);
    if (address == NULL)
        log_debug("Can't find a Vulkan instance function of name '%s'.", procname);
    return address;
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

static void create_validation_layers(struct vulkan_backend *vulkan)
{
    if (vulkan->instance == VK_NULL_HANDLE || vulkan->debug_messenger) return;

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

    if (vulkan->vkCreateDebugUtilsMessengerEXT(vulkan->instance, &messenger_info, vulkan->allocator, &vulkan->debug_messenger) != VK_SUCCESS) {
        vulkan->extensions &= ~vulkan_ext_layer_validation_bit;
        return;
    }
    log_verbose("Created Vulkan validation layers.");
}

static void destroy_validation_layers(struct vulkan_backend *vulkan)
{
    if (vulkan->instance == VK_NULL_HANDLE || vulkan->debug_messenger == VK_NULL_HANDLE) return;
    
    vulkan->vkDestroyDebugUtilsMessengerEXT(vulkan->instance, vulkan->debug_messenger, vulkan->allocator);
    vulkan->debug_messenger = VK_NULL_HANDLE;
    log_verbose("Destroyed Vulkan validation layers.");
}

s32 pelagial_vulkan_renderer_init(
    struct pelagic_ocean                   *restrict pelagial,
    const struct pelagic_ocean_create_info *restrict create_info)
{
    u32 instance_version = 0;
    u32 layer_count = 0;
    u32 extension_count = 0;
    const char **extensions = NULL;

    /* instance extensions */
    const char *ext_surface             = VK_KHR_SURFACE_EXTENSION_NAME;
#if defined(PLATFORM_WINDOWS)
    const char *ext_win32_surface       = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(PLATFORM_APPLE)
    const char *ext_metal_surface       = VK_EXT_METAL_SURFACE_EXTENSION_NAME;
#elif defined(PLATFORM_ANDROID)
    const char *ext_android_surface     = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_WAYLAND
    const char *ext_wayland_surface     = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_XCB
    const char *ext_xcb_surface         = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif
#ifdef HADAL_DRMKMS
    const char *ext_display             = VK_KHR_DISPLAY_EXTENSION_NAME;
#endif
    const char *ext_headless_surface    = VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME;
    const char *ext_debug_utils         = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    struct vulkan_backend *vulkan = (struct vulkan_backend *)pelagial->interface.internal;
    assert_debug(vulkan && vulkan->module);
    vulkan->pelagial = pelagial;

    vulkan->vkEnumerateInstanceVersion(&instance_version);
    vulkan->vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = create_info->app_name,
        .applicationVersion = create_info->app_version,
        .pEngineName = create_info->engine_name,
        .engineVersion = create_info->engine_version,
        .apiVersion = instance_version,
    };

    VERIFY_VK(vulkan->vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (extension_count) {
        VkExtensionProperties *properties = (VkExtensionProperties *)
            riven_alloc(create_info->riven, rivens_tag_drifter, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
        VERIFY_VK(vulkan->vkEnumerateInstanceExtensionProperties(NULL, &extension_count, properties));

        u32 i;
        for (i = 0; i < extension_count; i++) {
            if (!strcmp(properties[i].extensionName, ext_surface)) {
                vulkan->extensions |= vulkan_ext_surface_bit;
#if defined(PLATFORM_WINDOWS)
            } else if (!strcmp(properties[i].extensionName, ext_win32_surface)) {
                vulkan->extensions |= vulkan_ext_win32_surface_bit;
#endif
#if defined(PLATFORM_APPLE)
            } else if (!strcmp(properties[i].extensionName, ext_metal_surface)) {
                vulkan->extensions |= vulkan_ext_metal_surface_bit;
#endif
#if defined(PLATFORM_ANDROID)
            } else if (!strcmp(properties[i].extensionName, ext_android_surface)) {
                vulkan->extensions |= vulkan_ext_android_surface_bit;
#endif
#ifdef HADAL_WAYLAND
            } else if (!strcmp(properties[i].extensionName, ext_wayland_surface)) {
                vulkan->extensions |= vulkan_ext_wayland_surface_bit;
#endif
#ifdef HADAL_XCB
            } else if (!strcmp(properties[i].extensionName, ext_xcb_surface)) {
                vulkan->extensions |= vulkan_ext_xcb_surface_bit;
#endif
#ifdef HADAL_DRMKMS
            } else if (!strcmp(properties[i].extensionName, ext_display)) {
                vulkan->extensions |= vulkan_ext_display_bit;
#endif
            } else if (!strcmp(properties[i].extensionName, ext_headless_surface)) {
                vulkan->extensions |= vulkan_ext_headless_surface_bit;
            } else if (create_info->enable_debug_utilities && !strcmp(properties[i].extensionName, ext_debug_utils)) {
                vulkan->extensions |= vulkan_ext_debug_utils_bit;
            }
        }
        i = 0; 
        extension_count = bits_popcnt_lookup((const u8 *)&vulkan->extensions, sizeof(vulkan->extensions));
        extensions = (const char **)riven_alloc(create_info->riven, rivens_tag_drifter, sizeof(const char *) * extension_count, _Alignof(const char *));

        if (vulkan->extensions & vulkan_ext_surface_bit)
            extensions[i++] = ext_surface;
#if defined(PLATFORM_WINDOWS)
        if (vulkan->extensions & vulkan_ext_win32_surface_bit)
            extensions[i++] = ext_win32_surface;
#endif
#if defined(PLATFORM_APPLE)
        if (vulkan->extensions & vulkan_ext_metal_surface_bit)
            extensions[i++] = ext_metal_surface;
#endif
#if defined(PLATFORM_ANDROID)
        if (vulkan->extensions & vulkan_ext_android_surface_bit)
            extensions[i++] = ext_android_surface;
#endif
#ifdef HADAL_WAYLAND
        if (vulkan->extensions & vulkan_ext_wayland_surface_bit)
            extensions[i++] = ext_wayland_surface;
#endif
#ifdef HADAL_XCB
        if (vulkan->extensions & vulkan_ext_xcb_surface_bit)
            extensions[i++] = ext_xcb_surface;
#endif
#ifdef HADAL_DRMKMS
        if (vulkan->extensions & vulkan_ext_display_bit)
            extensions[i++] = ext_display;
#endif
        if (vulkan->extensions & vulkan_ext_headless_surface_bit)
            extensions[i++] = ext_headless_surface;
        if (vulkan->extensions & vulkan_ext_debug_utils_bit)
            extensions[i++] = ext_debug_utils;
    } else {
        log_error("No Vulkan instance extensions are available, not a valid state.");
        return result_error;
    }

    /* TODO setup VkAllocationCallbacks structure:
    vulkan->allocator = (VkAllocationCallbacks *)riven_alloc(create_info->riven, create_info->tag, sizeof(VkAllocationCallbacks), _Alignof(VkAllocationCallbacks));
    vulkan->allocator->...
     */

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

    /* enable validation layers if debug utils were requested */
    if (layer_count && vulkan->extensions & vulkan_ext_debug_utils_bit) {
        vulkan->extensions |= vulkan_ext_layer_validation_bit;
        instance_info.pNext = &validation_features;
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }
    VERIFY_VK(vulkan->vkCreateInstance(&instance_info, vulkan->allocator, &vulkan->instance));

    /* core instance procedures */
    vulkan->vkCreateDevice = (PFN_vkCreateDevice)instance_proc_address(vulkan, "vkCreateDevice");
    vulkan->vkDestroyInstance = (PFN_vkDestroyInstance)instance_proc_address(vulkan, "vkDestroyInstance");
    vulkan->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)instance_proc_address(vulkan, "vkEnumerateDeviceExtensionProperties");
    vulkan->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)instance_proc_address(vulkan, "vkEnumeratePhysicalDeviceGroups");
    vulkan->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)instance_proc_address(vulkan, "vkEnumeratePhysicalDevices");
    vulkan->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)instance_proc_address(vulkan, "vkGetDeviceProcAddr");
    vulkan->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)instance_proc_address(vulkan, "vkGetPhysicalDeviceFeatures");
    vulkan->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)instance_proc_address(vulkan, "vkGetPhysicalDeviceFeatures2");
    vulkan->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)instance_proc_address(vulkan, "vkGetPhysicalDeviceFormatProperties");
    vulkan->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)instance_proc_address(vulkan, "vkGetPhysicalDeviceFormatProperties2");
    vulkan->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)instance_proc_address(vulkan, "vkGetPhysicalDeviceImageFormatProperties");
    vulkan->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)instance_proc_address(vulkan, "vkGetPhysicalDeviceImageFormatProperties2");
    vulkan->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instance_proc_address(vulkan, "vkGetPhysicalDeviceMemoryProperties");
    vulkan->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)instance_proc_address(vulkan, "vkGetPhysicalDeviceMemoryProperties2");
    vulkan->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)instance_proc_address(vulkan, "vkGetPhysicalDeviceProperties");
    vulkan->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)instance_proc_address(vulkan, "vkGetPhysicalDeviceProperties2");
    vulkan->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)instance_proc_address(vulkan, "vkGetPhysicalDeviceQueueFamilyProperties");
    vulkan->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)instance_proc_address(vulkan, "vkGetPhysicalDeviceQueueFamilyProperties2");

    /* surface */
    vulkan->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instance_proc_address(vulkan, "vkDestroySurfaceKHR");
    vulkan->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceSurfaceSupportKHR");
    vulkan->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vulkan->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    vulkan->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceSurfacePresentModesKHR");

#if defined(VK_KHR_win32_surface)
    vulkan->vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)instance_proc_address(vulkan, "vkCreateWin32SurfaceKHR");
    vulkan->vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif /* VK_KHR_win32_surface */

#if defined(VK_EXT_metal_surface)
    vulkan->vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)instance_proc_address(vulkan, "vkCreateMetalSurfaceEXT");
#endif /* VK_EXT_metal_surface */

#if defined(VK_KHR_wayland_surface)
    vulkan->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)instance_proc_address(vulkan, "vkCreateWaylandSurfaceKHR");
    vulkan->vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif /* VK_KHR_wayland_surface */

#if defined(VK_KHR_xcb_surface)
    vulkan->vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)instance_proc_address(vulkan, "vkCreateXcbSurfaceKHR");
    vulkan->vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif /* VK_KHR_xcb_surface */

#if defined(VK_KHR_android_surface)
    vulkan->vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)instance_proc_address(vulkan, "vkCreateAndroidSurfaceKHR");
#endif /* VK_KHR_android_surface */

#if defined(VK_EXT_headless_surface)
    vulkan->vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT)instance_proc_address(vulkan, "vkCreateHeadlessSurfaceEXT");
#endif /* VK_EXT_headless_surface */

#if defined(VK_KHR_display)
    vulkan->vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)instance_proc_address(vulkan, "vkCreateDisplayModeKHR");
    vulkan->vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)instance_proc_address(vulkan, "vkCreateDisplayPlaneSurfaceKHR");
    vulkan->vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)instance_proc_address(vulkan, "vkGetDisplayModePropertiesKHR");
    vulkan->vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)instance_proc_address(vulkan, "vkGetDisplayPlaneCapabilitiesKHR");
    vulkan->vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)instance_proc_address(vulkan, "vkGetDisplayPlaneSupportedDisplaysKHR");
    vulkan->vkGetPhysicalDeviceDisplayPlanePropertiesKHR= (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    vulkan->vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceDisplayPropertiesKHR");
#endif /* VK_KHR_display */

    /* debug utils */
    vulkan->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)instance_proc_address(vulkan, "vkSetDebugUtilsObjectNameEXT");
    vulkan->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)instance_proc_address(vulkan, "vkSetDebugUtilsObjectTagEXT");
    vulkan->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkQueueBeginDebugUtilsLabelEXT");
    vulkan->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkQueueEndDebugUtilsLabelEXT");
    vulkan->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkQueueInsertDebugUtilsLabelEXT");
    vulkan->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkCmdBeginDebugUtilsLabelEXT");
    vulkan->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkCmdEndDebugUtilsLabelEXT");
    vulkan->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)instance_proc_address(vulkan, "vkCmdInsertDebugUtilsLabelEXT");
    vulkan->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)instance_proc_address(vulkan, "vkCreateDebugUtilsMessengerEXT");
    vulkan->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)instance_proc_address(vulkan, "vkDestroyDebugUtilsMessengerEXT");
    vulkan->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)instance_proc_address(vulkan, "vkSubmitDebugUtilsMessageEXT");

    /* video coding */
    vulkan->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    vulkan->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)instance_proc_address(vulkan, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");

    if (vulkan->extensions & vulkan_ext_layer_validation_bit)
        create_validation_layers(vulkan);

    atomic_fetch_or_explicit(&pelagial->flags, pelagic_flag_renderer_is_valid, memory_order_release);
    return result_success;
}

void pelagial_vulkan_renderer_fini(void *restrict internal)
{
    struct vulkan_backend *vulkan = (struct vulkan_backend *)internal;
    assert_debug(vulkan);

    if (vulkan->extensions & vulkan_ext_layer_validation_bit)
        destroy_validation_layers(vulkan);

    if (vulkan->instance)
        vulkan->vkDestroyInstance(vulkan->instance, vulkan->allocator);

    if (vulkan->module)
        plugin_close_dll(vulkan->module);

    zerop(vulkan);
    g_vulkan = NULL;
}
