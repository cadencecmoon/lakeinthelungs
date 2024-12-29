#include <lake/bedrock/log.h>
#include <lake/hadal.h>

#include "vk_platynova.h"

#if defined(AMW_PLATFORM_APPLE)
#include <stdlib.h> /* getenv, for MacOS */
#endif

static PFN_vkVoidFunction instance_proc_address(struct vulkan_instance_api *api, VkInstance instance, const char *procname)
{
    PFN_vkVoidFunction address = api->vkGetInstanceProcAddr(instance, procname);
    if (address == NULL) {
        log_debug("Can't find a Vulkan instance function of name '%s'", procname);
    }
    return address;
}

static PFN_vkVoidFunction device_proc_address(struct vulkan_instance_api *instance_api, VkDevice device, const char *procname)
{
    PFN_vkVoidFunction address = instance_api->vkGetDeviceProcAddr(device, procname);
    if (address == NULL) {
        log_debug("Can't find a Vulkan device function of name '%s'", procname);
    }
    return address;
}

bool vulkan_open_driver(struct vulkan_instance_api *api)
{
    assert_debug(api);
    api->module = NULL;

#if defined(AMW_PLATFORM_WINDOWS)
    api->module = hadal_load_dll("vulkan-1.dll");
#elif defined(AMW_PLATFORM_APPLE)
    api->module = hadal_load_dll("libvulkan.dylib");
    if (!api->module)
        api->module = hadal_load_dll("libvulkan.1.dylib");
    if (!api->module)
        api->module = hadal_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a Framework. App store rules for iOS
     * strictly enforce no .dylib's. If they aren't found it just falls through */
    if (!api->module)
        api->module = hadal_load_dll("vulkan.framework/vulkan");
    if (!api->module)
        api->module = hadal_load_dll("MoltenVK.framework/MoltenVK");
    /* modern versions of macOS don't search /usr/local/lib automatically contrary to what man dlopen says
	 * Vulkan SDK uses this as the system-wide installation location, 
     * so we're going to fallback to this if all else fails */
    if (!api->module && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL)
        api->module = hadal_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    api->module = hadal_load_dll("libvulkan.so.1");
    if (!api->module)
        api->module = hadal_load_dll("libvulkan.so");
#endif
    if (!api->module) {
        log_error("Can't connect to a Vulkan driver library.");
        return false;
    }
    api->vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)hadal_get_proc_address(api->module, "vkGetInstanceProcAddr");

    if (api->vkGetInstanceProcAddr == NULL) {
        vulkan_close_driver(api);
        log_error("Can't load vkGetInstanceProcAddr from Vulkan drivers.");
        return false;
    }

    api->vkCreateInstance                        = (PFN_vkCreateInstance)(void *)instance_proc_address(api, NULL, "vkCreateInstance");
    api->vkEnumerateInstanceVersion              = (PFN_vkEnumerateInstanceVersion)(void *)instance_proc_address(api, NULL, "vkEnumerateInstanceVersion");
    api->vkEnumerateInstanceExtensionProperties  = (PFN_vkEnumerateInstanceExtensionProperties)(void *)instance_proc_address(api, NULL, "vkEnumerateInstanceExtensionProperties");
    api->vkEnumerateInstanceLayerProperties      = (PFN_vkEnumerateInstanceLayerProperties)(void *)instance_proc_address(api, NULL, "vkEnumerateInstanceLayerProperties");

    if (!api->vkCreateInstance ||
        !api->vkEnumerateInstanceVersion ||
        !api->vkEnumerateInstanceExtensionProperties ||
        !api->vkEnumerateInstanceLayerProperties) 
    {
        vulkan_close_driver(api);
        log_error("Can't load global function pointers from Vulkan drivers.");
        return false;
    }

    return true;
}

void vulkan_close_driver(struct vulkan_instance_api *api)
{
    assert_debug(api);

    if (api->module)
        hadal_close_dll(api->module);

    api->vkGetDeviceProcAddr = NULL;
    api->vkGetInstanceProcAddr = NULL;
    api->module = NULL;
}

int32_t vulkan_instance_api_load_procedures(
        struct vulkan_instance_api *api, 
        VkInstance instance, 
        uint32_t *instance_extension_bits)
{
    if (!api || !api->vkGetInstanceProcAddr || !instance_extension_bits)
        return result_error_invalid_engine_context;

    int32_t out = result_success;

    api->vkCreateDevice = (PFN_vkCreateDevice)(void *)instance_proc_address(api, instance, "vkCreateDevice");
    api->vkDestroyInstance = (PFN_vkDestroyInstance)(void *)instance_proc_address(api, instance, "vkDestroyInstance");
    api->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)(void *)instance_proc_address(api, instance, "vkEnumerateDeviceExtensionProperties");
    api->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)(void *)instance_proc_address(api, instance, "vkEnumeratePhysicalDeviceGroups");
    api->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)(void *)instance_proc_address(api, instance, "vkEnumeratePhysicalDevices");
    api->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)(void *)instance_proc_address(api, instance, "vkGetDeviceProcAddr");
    api->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceFeatures");
    api->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceFeatures2");
    api->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceFormatProperties");
    api->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceFormatProperties2");
    api->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceImageFormatProperties");
    api->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceImageFormatProperties2");
    api->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceMemoryProperties");
    api->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceMemoryProperties2");
    api->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceProperties");
    api->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceProperties2");
    api->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    api->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceQueueFamilyProperties2");

    if (!api->vkCreateDevice ||
        !api->vkDestroyInstance ||
        !api->vkEnumerateDeviceExtensionProperties ||
        !api->vkEnumeratePhysicalDeviceGroups ||
        !api->vkEnumeratePhysicalDevices ||
        !api->vkGetDeviceProcAddr ||
        !api->vkGetPhysicalDeviceFeatures ||
        !api->vkGetPhysicalDeviceFeatures2 ||
        !api->vkGetPhysicalDeviceFormatProperties ||
        !api->vkGetPhysicalDeviceFormatProperties2 ||
        !api->vkGetPhysicalDeviceImageFormatProperties ||
        !api->vkGetPhysicalDeviceImageFormatProperties2 ||
        !api->vkGetPhysicalDeviceMemoryProperties ||
        !api->vkGetPhysicalDeviceMemoryProperties2 ||
        !api->vkGetPhysicalDeviceProperties ||
        !api->vkGetPhysicalDeviceProperties2 ||
        !api->vkGetPhysicalDeviceQueueFamilyProperties ||
        !api->vkGetPhysicalDeviceQueueFamilyProperties2)
    {
        log_error("Can't load core Vulkan instance functions.");
        return result_error_undefined; // TODO
    }
#if defined(VK_KHR_surface)
    if (*instance_extension_bits & vulkan_extension_surface_bit) {
        api->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(void *)instance_proc_address(api, instance, "vkDestroySurfaceKHR");
        api->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        api->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        api->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        api->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!api->vkDestroySurfaceKHR ||
            !api->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !api->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !api->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !api->vkGetPhysicalDeviceSurfacePresentModesKHR)
        {
            log_error("Can't load VK_KHR_surface extension procedures. This extension is required.");
            return result_error_undefined; // TODO
        }
    } else {
        log_error("The extension VK_KHR_surface is not present. This extension is required.");
        return result_error_undefined; // TODO
    }
#endif /* VK_KHR_surface */
#if defined(VK_KHR_win32_surface) && defined(AMW_PLATFORM_WINDOWS)
    if (*instance_extension_bits & vulkan_extension_win32_surface_bit) {
        api->vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(void *)instance_proc_address(api, instance, "vkCreateWin32SurfaceKHR");
        api->vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");

        if (!api->vkCreateWin32SurfaceKHR || 
            !api->vkGetPhysicalDeviceWin32PresentationSupportKHR)
        {
            log_warn("Can't load VK_KHR_win32_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_win32_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface) && defined(AMW_PLATFORM_APPLE)
    if (*instance_extension_bits & vulkan_extension_metal_surface_bit) {
        api->vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)(void *)instance_proc_address(api, instance, "vkCreateMetalSurfaceEXT");

        if (!api->vkCreateMetalSurfaceEXT) {
            log_warn("Can't load VK_KHR_metal_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_metal_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface) && defined(AMW_NATIVE_WAYLAND)
    if (*instance_extension_bits & vulkan_extension_wayland_surface_bit) {
        api->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)(void *)instance_proc_address(api, instance, "vkCreateWaylandSurfaceKHR");
        api->vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");

        if (!api->vkCreateWaylandSurfaceKHR || 
            !api->vkGetPhysicalDeviceWaylandPresentationSupportKHR) 
        {
            log_warn("Can't load VK_KHR_wayland_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_wayland_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface) && defined(AMW_NATIVE_XCB)
    if (*instance_extension_bits & vulkan_extension_xcb_surface_bit) {
        api->vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)(void *)instance_proc_address(api, instance, "vkCreateXcbSurfaceKHR");
        api->vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");

        if (!api->vkCreateXcbSurfaceKHR || 
            !api->vkGetPhysicalDeviceXcbPresentationSupportKHR) 
        {
            log_warn("Can't load VK_KHR_xcb_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_xcb_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface) && defined(AMW_PLATFORM_ANDROID)
    if (*instance_extension_bits & vulkan_extension_android_surface_bit) {
        api->vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)(void *)instance_proc_address(api, instance, "vkCreateAndroidSurfaceKHR");

        if (!api->vkCreateAndroidSurfaceKHR) {
            log_warn("Can't load VK_KHR_android_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_android_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    if (*instance_extension_bits & vulkan_extension_headless_surface_bit) {
        api->vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT)(void *)instance_proc_address(api, instance, "vkCreateHeadlessSurfaceEXT");

        if (!api->vkCreateHeadlessSurfaceEXT) {
            log_warn("Can't load VK_EXT_headless_surface extension procedures.");
            *instance_extension_bits &= ~(vulkan_extension_headless_surface_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display) && defined(AMW_NATIVE_KMS)
    if (*instance_extension_bits & vulkan_extension_display_bit) {
        api->vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)(void *)instance_proc_address(api, instance, "vkCreateDisplayModeKHR");
        api->vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)(void *)instance_proc_address(api, instance, "vkCreateDisplayPlaneSurfaceKHR");
        api->vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)(void *)instance_proc_address(api, instance, "vkGetDisplayModePropertiesKHR");
        api->vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)(void *)instance_proc_address(api, instance, "vkGetDisplayPlaneCapabilitiesKHR");
        api->vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)(void *)instance_proc_address(api, instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
        api->vkGetPhysicalDeviceDisplayPlanePropertiesKHR= (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
        api->vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)(void *)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");

        if (!api->vkCreateDisplayModeKHR ||
            !api->vkCreateDisplayPlaneSurfaceKHR ||
            !api->vkGetDisplayModePropertiesKHR ||
            !api->vkGetDisplayPlaneCapabilitiesKHR ||
            !api->vkGetDisplayPlaneSupportedDisplaysKHR ||
            !api->vkGetPhysicalDeviceDisplayPlanePropertiesKHR ||
            !api->vkGetPhysicalDeviceDisplayPropertiesKHR)
        {
            log_warn("Can't load VK_KHR_display extension procedures. DRM/KMS is not available.");
            *instance_extension_bits &= ~(vulkan_extension_display_bit | vulkan_extension_acquire_drm_display_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_display */
#if defined(VK_EXT_direct_mode_display) && defined(AMW_NATIVE_KMS)
    if (*instance_extension_bits & vulkan_extension_direct_mode_display_bit) {
        api->vkReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT)(void *)instance_proc_address(api, instance, "vkReleaseDisplayEXT");
        
        if (!api->vkReleaseDisplayEXT) {
            log_warn("Can't load VK_EXT_direct_mode_display extension procedure. DRM/KMS may be limited");
            *instance_extension_bits &= ~(vulkan_extension_acquire_drm_display_bit | vulkan_extension_direct_mode_display_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_EXT_direct_mode_display */
#if defined(VK_EXT_acquire_drm_display) && defined(AMW_NATIVE_KMS)
    if (*instance_extension_bits & vulkan_extension_acquire_drm_display_bit) {
        api->vkAcquireDrmDisplayEXT = (PFN_vkAcquireDrmDisplayEXT)(void *)instance_proc_address(api, instance, "vkAcquireDrmDisplayEXT");
        api->vkGetDrmDisplayEXT = (PFN_vkGetDrmDisplayEXT)(void *)instance_proc_address(api, instance, "vkGetDrmDisplayEXT");

        if (!api->vkAcquireDrmDisplayEXT || 
            !api->vkGetDrmDisplayEXT) 
        {
            log_warn("Can't load VK_EXT_acquire_drm_display extension procedures. DRM/KMS may be limited.");
            *instance_extension_bits &= ~(vulkan_extension_acquire_drm_display_bit | vulkan_extension_direct_mode_display_bit);
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_EXT_acquire_drm_display */
#if defined(VK_EXT_debug_utils) && !defined(AMW_NDEBUG)
    if (*instance_extension_bits & vulkan_extension_debug_utils_bit) {
        api->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(void *)instance_proc_address(api, instance, "vkSetDebugUtilsObjectNameEXT");
        api->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)(void *)instance_proc_address(api, instance, "vkSetDebugUtilsObjectTagEXT");
        api->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkQueueBeginDebugUtilsLabelEXT");
        api->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkQueueEndDebugUtilsLabelEXT");
        api->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkQueueInsertDebugUtilsLabelEXT");
        api->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkCmdBeginDebugUtilsLabelEXT");
        api->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkCmdEndDebugUtilsLabelEXT");
        api->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)(void *)instance_proc_address(api, instance, "vkCmdInsertDebugUtilsLabelEXT");
        api->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)(void *)instance_proc_address(api, instance, "vkCreateDebugUtilsMessengerEXT");
        api->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)(void *)instance_proc_address(api, instance, "vkDestroyDebugUtilsMessengerEXT");
        api->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)(void *)instance_proc_address(api, instance, "vkSubmitDebugUtilsMessageEXT");

        if (!api->vkSetDebugUtilsObjectNameEXT ||
            !api->vkSetDebugUtilsObjectTagEXT ||
            !api->vkQueueBeginDebugUtilsLabelEXT ||
            !api->vkQueueEndDebugUtilsLabelEXT ||
            !api->vkQueueInsertDebugUtilsLabelEXT ||
            !api->vkCmdBeginDebugUtilsLabelEXT ||
            !api->vkCmdEndDebugUtilsLabelEXT ||
            !api->vkCmdInsertDebugUtilsLabelEXT ||
            !api->vkCreateDebugUtilsMessengerEXT ||
            !api->vkDestroyDebugUtilsMessengerEXT ||
            !api->vkSubmitDebugUtilsMessageEXT) 
        {
            log_debug("Can't load VK_EXT_debug_utils extension procedures, validation layers will be disabled.");
            *instance_extension_bits &= ~(vulkan_extension_debug_utils_bit | vulkan_extension_layer_validation_bit);
            out = result_error_undefined; // TODO
        }
    } else { /* without debug utils no need to create the validation layers */
        *instance_extension_bits &= ~(vulkan_extension_layer_validation_bit);
    }
#endif /* VK_EXT_debug_utils */
    return out;
}

int32_t vulkan_device_api_load_procedures(
        struct vulkan_instance_api *instance_api, 
        struct vulkan_device_api *api, 
        VkDevice device, 
        uint32_t drivers_version,
        uint32_t *device_extension_bits)
{
    if (!api || !instance_api || !instance_api->vkGetDeviceProcAddr || !device_extension_bits)
        return result_error_invalid_engine_context;

    int32_t out = result_success;

    /* core 1.0 */
	api->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)(void *)device_proc_address(instance_api, device, "vkAllocateCommandBuffers");
	api->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)(void *)device_proc_address(instance_api, device, "vkAllocateDescriptorSets");
	api->vkAllocateMemory = (PFN_vkAllocateMemory)(void *)device_proc_address(instance_api, device, "vkAllocateMemory");
	api->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)(void *)device_proc_address(instance_api, device, "vkBeginCommandBuffer");
	api->vkBindBufferMemory = (PFN_vkBindBufferMemory)(void *)device_proc_address(instance_api, device, "vkBindBufferMemory");
	api->vkBindImageMemory = (PFN_vkBindImageMemory)(void *)device_proc_address(instance_api, device, "vkBindImageMemory");
	api->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)(void *)device_proc_address(instance_api, device, "vkCmdBeginQuery");
	api->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)(void *)device_proc_address(instance_api, device, "vkCmdBeginRenderPass");
	api->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)(void *)device_proc_address(instance_api, device, "vkCmdBindDescriptorSets");
	api->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)(void *)device_proc_address(instance_api, device, "vkCmdBindIndexBuffer");
	api->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)(void *)device_proc_address(instance_api, device, "vkCmdBindPipeline");
	api->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)(void *)device_proc_address(instance_api, device, "vkCmdBindVertexBuffers");
	api->vkCmdBlitImage = (PFN_vkCmdBlitImage)(void *)device_proc_address(instance_api, device, "vkCmdBlitImage");
	api->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)(void *)device_proc_address(instance_api, device, "vkCmdClearAttachments");
	api->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)(void *)device_proc_address(instance_api, device, "vkCmdClearColorImage");
	api->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)(void *)device_proc_address(instance_api, device, "vkCmdClearDepthStencilImage");
	api->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)(void *)device_proc_address(instance_api, device, "vkCmdCopyBuffer");
	api->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)(void *)device_proc_address(instance_api, device, "vkCmdCopyBufferToImage");
	api->vkCmdCopyImage = (PFN_vkCmdCopyImage)(void *)device_proc_address(instance_api, device, "vkCmdCopyImage");
	api->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)(void *)device_proc_address(instance_api, device, "vkCmdCopyImageToBuffer");
	api->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)(void *)device_proc_address(instance_api, device, "vkCmdCopyQueryPoolResults");
	api->vkCmdDispatch = (PFN_vkCmdDispatch)(void *)device_proc_address(instance_api, device, "vkCmdDispatch");
	api->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)(void *)device_proc_address(instance_api, device, "vkCmdDispatchIndirect");
	api->vkCmdDraw = (PFN_vkCmdDraw)(void *)device_proc_address(instance_api, device, "vkCmdDraw");
	api->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)(void *)device_proc_address(instance_api, device, "vkCmdDrawIndexed");
	api->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)(void *)device_proc_address(instance_api, device, "vkCmdDrawIndexedIndirect");
	api->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)(void *)device_proc_address(instance_api, device, "vkCmdDrawIndirect");
	api->vkCmdEndQuery = (PFN_vkCmdEndQuery)(void *)device_proc_address(instance_api, device, "vkCmdEndQuery");
	api->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)(void *)device_proc_address(instance_api, device, "vkCmdEndRenderPass");
	api->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)(void *)device_proc_address(instance_api, device, "vkCmdExecuteCommands");
	api->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)(void *)device_proc_address(instance_api, device, "vkCmdFillBuffer");
	api->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)(void *)device_proc_address(instance_api, device, "vkCmdNextSubpass");
	api->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)(void *)device_proc_address(instance_api, device, "vkCmdPipelineBarrier");
	api->vkCmdPushConstants = (PFN_vkCmdPushConstants)(void *)device_proc_address(instance_api, device, "vkCmdPushConstants");
	api->vkCmdResetEvent = (PFN_vkCmdResetEvent)(void *)device_proc_address(instance_api, device, "vkCmdResetEvent");
	api->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)(void *)device_proc_address(instance_api, device, "vkCmdResetQueryPool");
	api->vkCmdResolveImage = (PFN_vkCmdResolveImage)(void *)device_proc_address(instance_api, device, "vkCmdResolveImage");
	api->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)(void *)device_proc_address(instance_api, device, "vkCmdSetBlendConstants");
	api->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)(void *)device_proc_address(instance_api, device, "vkCmdSetDepthBias");
	api->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)(void *)device_proc_address(instance_api, device, "vkCmdSetDepthBounds");
	api->vkCmdSetEvent = (PFN_vkCmdSetEvent)(void *)device_proc_address(instance_api, device, "vkCmdSetEvent");
	api->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)(void *)device_proc_address(instance_api, device, "vkCmdSetLineWidth");
	api->vkCmdSetScissor = (PFN_vkCmdSetScissor)(void *)device_proc_address(instance_api, device, "vkCmdSetScissor");
	api->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)(void *)device_proc_address(instance_api, device, "vkCmdSetStencilCompareMask");
	api->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)(void *)device_proc_address(instance_api, device, "vkCmdSetStencilReference");
	api->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)(void *)device_proc_address(instance_api, device, "vkCmdSetStencilWriteMask");
	api->vkCmdSetViewport = (PFN_vkCmdSetViewport)(void *)device_proc_address(instance_api, device, "vkCmdSetViewport");
	api->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)(void *)device_proc_address(instance_api, device, "vkCmdUpdateBuffer");
	api->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)(void *)device_proc_address(instance_api, device, "vkCmdWaitEvents");
	api->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)(void *)device_proc_address(instance_api, device, "vkCmdWriteTimestamp");
	api->vkCreateBuffer = (PFN_vkCreateBuffer)(void *)device_proc_address(instance_api, device, "vkCreateBuffer");
	api->vkCreateBufferView = (PFN_vkCreateBufferView)(void *)device_proc_address(instance_api, device, "vkCreateBufferView");
	api->vkCreateCommandPool = (PFN_vkCreateCommandPool)(void *)device_proc_address(instance_api, device, "vkCreateCommandPool");
	api->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(void *)device_proc_address(instance_api, device, "vkCreateComputePipelines");
	api->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(void *)device_proc_address(instance_api, device, "vkCreateDescriptorPool");
	api->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)(void *)device_proc_address(instance_api, device, "vkCreateDescriptorSetLayout");
	api->vkCreateEvent = (PFN_vkCreateEvent)(void *)device_proc_address(instance_api, device, "vkCreateEvent");
	api->vkCreateFence = (PFN_vkCreateFence)(void *)device_proc_address(instance_api, device, "vkCreateFence");
	api->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)(void *)device_proc_address(instance_api, device, "vkCreateFramebuffer");
	api->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(void *)device_proc_address(instance_api, device, "vkCreateGraphicsPipelines");
	api->vkCreateImage = (PFN_vkCreateImage)(void *)device_proc_address(instance_api, device, "vkCreateImage");
	api->vkCreateImageView = (PFN_vkCreateImageView)(void *)device_proc_address(instance_api, device, "vkCreateImageView");
	api->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)(void *)device_proc_address(instance_api, device, "vkCreatePipelineCache");
	api->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(void *)device_proc_address(instance_api, device, "vkCreatePipelineLayout");
	api->vkCreateQueryPool = (PFN_vkCreateQueryPool)(void *)device_proc_address(instance_api, device, "vkCreateQueryPool");
	api->vkCreateRenderPass = (PFN_vkCreateRenderPass)(void *)device_proc_address(instance_api, device, "vkCreateRenderPass");
	api->vkCreateSampler = (PFN_vkCreateSampler)(void *)device_proc_address(instance_api, device, "vkCreateSampler");
	api->vkCreateSemaphore = (PFN_vkCreateSemaphore)(void *)device_proc_address(instance_api, device, "vkCreateSemaphore");
	api->vkCreateShaderModule = (PFN_vkCreateShaderModule)(void *)device_proc_address(instance_api, device, "vkCreateShaderModule");
	api->vkDestroyBuffer = (PFN_vkDestroyBuffer)(void *)device_proc_address(instance_api, device, "vkDestroyBuffer");
	api->vkDestroyBufferView = (PFN_vkDestroyBufferView)(void *)device_proc_address(instance_api, device, "vkDestroyBufferView");
	api->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(void *)device_proc_address(instance_api, device, "vkDestroyCommandPool");
	api->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(void *)device_proc_address(instance_api, device, "vkDestroyDescriptorPool");
	api->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)(void *)device_proc_address(instance_api, device, "vkDestroyDescriptorSetLayout");
	api->vkDestroyDevice = (PFN_vkDestroyDevice)(void *)device_proc_address(instance_api, device, "vkDestroyDevice");
	api->vkDestroyEvent = (PFN_vkDestroyEvent)(void *)device_proc_address(instance_api, device, "vkDestroyEvent");
	api->vkDestroyFence = (PFN_vkDestroyFence)(void *)device_proc_address(instance_api, device, "vkDestroyFence");
	api->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)(void *)device_proc_address(instance_api, device, "vkDestroyFramebuffer");
	api->vkDestroyImage = (PFN_vkDestroyImage)(void *)device_proc_address(instance_api, device, "vkDestroyImage");
	api->vkDestroyImageView = (PFN_vkDestroyImageView)(void *)device_proc_address(instance_api, device, "vkDestroyImageView");
	api->vkDestroyPipeline = (PFN_vkDestroyPipeline)(void *)device_proc_address(instance_api, device, "vkDestroyPipeline");
	api->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)(void *)device_proc_address(instance_api, device, "vkDestroyPipelineCache");
	api->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(void *)device_proc_address(instance_api, device, "vkDestroyPipelineLayout");
	api->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)(void *)device_proc_address(instance_api, device, "vkDestroyQueryPool");
	api->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(void *)device_proc_address(instance_api, device, "vkDestroyRenderPass");
	api->vkDestroySampler = (PFN_vkDestroySampler)(void *)device_proc_address(instance_api, device, "vkDestroySampler");
	api->vkDestroySemaphore = (PFN_vkDestroySemaphore)(void *)device_proc_address(instance_api, device, "vkDestroySemaphore");
	api->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(void *)device_proc_address(instance_api, device, "vkDestroyShaderModule");
	api->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)(void *)device_proc_address(instance_api, device, "vkDeviceWaitIdle");
	api->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)(void *)device_proc_address(instance_api, device, "vkEndCommandBuffer");
	api->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)(void *)device_proc_address(instance_api, device, "vkFlushMappedMemoryRanges");
	api->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)(void *)device_proc_address(instance_api, device, "vkFreeCommandBuffers");
	api->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)(void *)device_proc_address(instance_api, device, "vkFreeDescriptorSets");
	api->vkFreeMemory = (PFN_vkFreeMemory)(void *)device_proc_address(instance_api, device, "vkFreeMemory");
	api->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)(void *)device_proc_address(instance_api, device, "vkGetBufferMemoryRequirements");
	api->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)(void *)device_proc_address(instance_api, device, "vkGetDeviceMemoryCommitment");
	api->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)(void *)device_proc_address(instance_api, device, "vkGetDeviceQueue");
	api->vkGetEventStatus = (PFN_vkGetEventStatus)(void *)device_proc_address(instance_api, device, "vkGetEventStatus");
	api->vkGetFenceStatus = (PFN_vkGetFenceStatus)(void *)device_proc_address(instance_api, device, "vkGetFenceStatus");
	api->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)(void *)device_proc_address(instance_api, device, "vkGetImageMemoryRequirements");
	api->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)(void *)device_proc_address(instance_api, device, "vkGetImageSparseMemoryRequirements");
	api->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)(void *)device_proc_address(instance_api, device, "vkGetImageSubresourceLayout");
	api->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)(void *)device_proc_address(instance_api, device, "vkGetPipelineCacheData");
	api->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)(void *)device_proc_address(instance_api, device, "vkGetQueryPoolResults");
	api->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)(void *)device_proc_address(instance_api, device, "vkGetRenderAreaGranularity");
	api->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)(void *)device_proc_address(instance_api, device, "vkInvalidateMappedMemoryRanges");
	api->vkMapMemory = (PFN_vkMapMemory)(void *)device_proc_address(instance_api, device, "vkMapMemory");
	api->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)(void *)device_proc_address(instance_api, device, "vkMergePipelineCaches");
	api->vkQueueBindSparse = (PFN_vkQueueBindSparse)(void *)device_proc_address(instance_api, device, "vkQueueBindSparse");
	api->vkQueueSubmit = (PFN_vkQueueSubmit)(void *)device_proc_address(instance_api, device, "vkQueueSubmit");
	api->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)(void *)device_proc_address(instance_api, device, "vkQueueWaitIdle");
	api->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)(void *)device_proc_address(instance_api, device, "vkResetCommandBuffer");
	api->vkResetCommandPool = (PFN_vkResetCommandPool)(void *)device_proc_address(instance_api, device, "vkResetCommandPool");
	api->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)(void *)device_proc_address(instance_api, device, "vkResetDescriptorPool");
	api->vkResetEvent = (PFN_vkResetEvent)(void *)device_proc_address(instance_api, device, "vkResetEvent");
	api->vkResetFences = (PFN_vkResetFences)(void *)device_proc_address(instance_api, device, "vkResetFences");
	api->vkSetEvent = (PFN_vkSetEvent)(void *)device_proc_address(instance_api, device, "vkSetEvent");
	api->vkUnmapMemory = (PFN_vkUnmapMemory)(void *)device_proc_address(instance_api, device, "vkUnmapMemory");
	api->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)(void *)device_proc_address(instance_api, device, "vkUpdateDescriptorSets");
	api->vkWaitForFences = (PFN_vkWaitForFences)(void *)device_proc_address(instance_api, device, "vkWaitForFences");

    if (!api->vkAllocateCommandBuffers ||
        !api->vkAllocateDescriptorSets ||
        !api->vkAllocateMemory ||
        !api->vkBeginCommandBuffer ||
        !api->vkBindBufferMemory ||
        !api->vkBindImageMemory ||
        !api->vkCmdBeginQuery ||
        !api->vkCmdBeginRenderPass ||
        !api->vkCmdBindDescriptorSets ||
        !api->vkCmdBindIndexBuffer ||
        !api->vkCmdBindPipeline ||
        !api->vkCmdBindVertexBuffers ||
        !api->vkCmdBlitImage ||
        !api->vkCmdClearAttachments ||
        !api->vkCmdClearColorImage ||
        !api->vkCmdClearDepthStencilImage ||
        !api->vkCmdCopyBuffer ||
        !api->vkCmdCopyBufferToImage ||
        !api->vkCmdCopyImage ||
        !api->vkCmdCopyImageToBuffer ||
        !api->vkCmdCopyQueryPoolResults ||
        !api->vkCmdDispatch ||
        !api->vkCmdDispatchIndirect ||
        !api->vkCmdDraw ||
        !api->vkCmdDrawIndexed ||
        !api->vkCmdDrawIndexedIndirect ||
        !api->vkCmdDrawIndirect ||
        !api->vkCmdEndQuery ||
        !api->vkCmdEndRenderPass ||
        !api->vkCmdExecuteCommands ||
        !api->vkCmdFillBuffer ||
        !api->vkCmdNextSubpass ||
        !api->vkCmdPipelineBarrier ||
        !api->vkCmdPushConstants ||
        !api->vkCmdResetEvent ||
        !api->vkCmdResetQueryPool ||
        !api->vkCmdResolveImage ||
        !api->vkCmdSetBlendConstants ||
        !api->vkCmdSetDepthBias ||
        !api->vkCmdSetDepthBounds ||
        !api->vkCmdSetEvent ||
        !api->vkCmdSetLineWidth ||
        !api->vkCmdSetScissor ||
        !api->vkCmdSetStencilCompareMask ||
        !api->vkCmdSetStencilReference ||
        !api->vkCmdSetStencilWriteMask ||
        !api->vkCmdSetViewport ||
        !api->vkCmdUpdateBuffer ||
        !api->vkCmdWaitEvents ||
        !api->vkCmdWriteTimestamp ||
        !api->vkCreateBuffer ||
        !api->vkCreateBufferView ||
        !api->vkCreateCommandPool ||
        !api->vkCreateComputePipelines ||
        !api->vkCreateDescriptorPool ||
        !api->vkCreateDescriptorSetLayout ||
        !api->vkCreateEvent ||
        !api->vkCreateFence ||
        !api->vkCreateFramebuffer ||
        !api->vkCreateGraphicsPipelines ||
        !api->vkCreateImage ||
        !api->vkCreateImageView ||
        !api->vkCreatePipelineCache ||
        !api->vkCreatePipelineLayout ||
        !api->vkCreateQueryPool ||
        !api->vkCreateRenderPass ||
        !api->vkCreateSampler ||
        !api->vkCreateSemaphore ||
        !api->vkCreateShaderModule ||
        !api->vkDestroyBuffer ||
        !api->vkDestroyBufferView ||
        !api->vkDestroyCommandPool ||
        !api->vkDestroyDescriptorPool ||
        !api->vkDestroyDescriptorSetLayout ||
        !api->vkDestroyDevice ||
        !api->vkDestroyEvent ||
        !api->vkDestroyFence ||
        !api->vkDestroyFramebuffer ||
        !api->vkDestroyImage ||
        !api->vkDestroyImageView ||
        !api->vkDestroyPipeline ||
        !api->vkDestroyPipelineCache ||
        !api->vkDestroyPipelineLayout ||
        !api->vkDestroyQueryPool ||
        !api->vkDestroyRenderPass ||
        !api->vkDestroySampler ||
        !api->vkDestroySemaphore ||
        !api->vkDestroyShaderModule ||
        !api->vkDeviceWaitIdle ||
        !api->vkEndCommandBuffer ||
        !api->vkFlushMappedMemoryRanges ||
        !api->vkFreeCommandBuffers ||
        !api->vkFreeDescriptorSets ||
        !api->vkFreeMemory ||
        !api->vkGetBufferMemoryRequirements ||
        !api->vkGetDeviceMemoryCommitment ||
        !api->vkGetDeviceQueue ||
        !api->vkGetEventStatus ||
        !api->vkGetFenceStatus ||
        !api->vkGetImageMemoryRequirements ||
        !api->vkGetImageSparseMemoryRequirements ||
        !api->vkGetImageSubresourceLayout ||
        !api->vkGetPipelineCacheData ||
        !api->vkGetQueryPoolResults ||
        !api->vkGetRenderAreaGranularity ||
        !api->vkInvalidateMappedMemoryRanges ||
        !api->vkMapMemory ||
        !api->vkMergePipelineCaches ||
        !api->vkQueueBindSparse ||
        !api->vkQueueSubmit ||
        !api->vkQueueWaitIdle ||
        !api->vkResetCommandBuffer ||
        !api->vkResetCommandPool ||
        !api->vkResetDescriptorPool ||
        !api->vkResetEvent ||
        !api->vkResetFences ||
        !api->vkSetEvent ||
        !api->vkUnmapMemory ||
        !api->vkUpdateDescriptorSets ||
        !api->vkWaitForFences)
    {
        log_error("Can't load Vulkan core 1.0 device procedures.");
        return result_error_undefined; // TODO
    }
    
    /* core 1.1 */
    api->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)(void *)device_proc_address(instance_api, device, "vkBindBufferMemory2");
    api->vkBindImageMemory2 = (PFN_vkBindImageMemory2)(void *)device_proc_address(instance_api, device, "vkBindImageMemory2");
    api->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)(void *)device_proc_address(instance_api, device, "vkCmdDispatchBase");
    api->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)(void *)device_proc_address(instance_api, device, "vkCmdSetDeviceMask");
    api->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)(void *)device_proc_address(instance_api, device, "vkCreateDescriptorUpdateTemplate");
    api->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)(void *)device_proc_address(instance_api, device, "vkCreateSamplerYcbcrConversion");
    api->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)(void *)device_proc_address(instance_api, device, "vkDestroyDescriptorUpdateTemplate");
    api->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)(void *)device_proc_address(instance_api, device, "vkDestroySamplerYcbcrConversion");
    api->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)(void *)device_proc_address(instance_api, device, "vkGetBufferMemoryRequirements2");
    api->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)(void *)device_proc_address(instance_api, device, "vkGetDescriptorSetLayoutSupport");
    api->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)(void *)device_proc_address(instance_api, device, "vkGetDeviceGroupPeerMemoryFeatures");
    api->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)(void *)device_proc_address(instance_api, device, "vkGetDeviceQueue2");
    api->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)(void *)device_proc_address(instance_api, device, "vkGetImageMemoryRequirements2");
    api->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)(void *)device_proc_address(instance_api, device, "vkGetImageSparseMemoryRequirements2");
    api->vkTrimCommandPool = (PFN_vkTrimCommandPool)(void *)device_proc_address(instance_api, device, "vkTrimCommandPool");
    api->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)(void *)device_proc_address(instance_api, device, "vkUpdateDescriptorSetWithTemplate");

    if (!api->vkBindBufferMemory2 ||
        !api->vkBindImageMemory2 ||
        !api->vkCmdDispatchBase ||
        !api->vkCmdSetDeviceMask ||
        !api->vkCreateDescriptorUpdateTemplate ||
        !api->vkCreateSamplerYcbcrConversion ||
        !api->vkDestroyDescriptorUpdateTemplate ||
        !api->vkDestroySamplerYcbcrConversion ||
        !api->vkGetBufferMemoryRequirements2 ||
        !api->vkGetDescriptorSetLayoutSupport ||
        !api->vkGetDeviceGroupPeerMemoryFeatures ||
        !api->vkGetDeviceQueue2 ||
        !api->vkGetImageMemoryRequirements2 ||
        !api->vkGetImageSparseMemoryRequirements2 ||
        !api->vkTrimCommandPool ||
        !api->vkUpdateDescriptorSetWithTemplate)
    {
        log_error("Can't load Vulkan core 1.1 device procedures.");
        return result_error_undefined; // TODO
    }

    /* core 1.2 */
    api->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)(void *)device_proc_address(instance_api, device, "vkCmdDrawIndexedIndirectCount");
    api->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)(void *)device_proc_address(instance_api, device, "vkCmdDrawIndirectCount");
    api->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)(void *)device_proc_address(instance_api, device, "vkGetBufferDeviceAddress");
    api->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)(void *)device_proc_address(instance_api, device, "vkGetBufferOpaqueCaptureAddress");
    api->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)(void *)device_proc_address(instance_api, device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    api->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)(void *)device_proc_address(instance_api, device, "vkGetSemaphoreCounterValue");
    api->vkResetQueryPool = (PFN_vkResetQueryPool)(void *)device_proc_address(instance_api, device, "vkResetQueryPool");
    api->vkSignalSemaphore = (PFN_vkSignalSemaphore)(void *)device_proc_address(instance_api, device, "vkSignalSemaphore");
    api->vkWaitSemaphores = (PFN_vkWaitSemaphores)(void *)device_proc_address(instance_api, device, "vkWaitSemaphores");

    if (!api->vkCmdDrawIndexedIndirectCount ||
        !api->vkCmdDrawIndirectCount ||
        !api->vkGetBufferDeviceAddress ||
        !api->vkGetBufferOpaqueCaptureAddress ||
        !api->vkGetDeviceMemoryOpaqueCaptureAddress ||
        !api->vkGetSemaphoreCounterValue ||
        !api->vkResetQueryPool ||
        !api->vkSignalSemaphore ||
        !api->vkWaitSemaphores)
    {
        log_error("Can't load Vulkan core 1.2 device procedures. This is the minimal target Vulkan version.");
        return result_error_undefined; // TODO
    }

#if defined(VK_VERSION_1_3)
    if (drivers_version >= VK_API_VERSION_1_3) {
        api->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)(void *)device_proc_address(instance_api, device, "vkCmdBeginRendering");
        api->vkCmdEndRendering = (PFN_vkCmdEndRendering)(void *)device_proc_address(instance_api, device, "vkCmdEndRendering");
    } else
#endif
    {
#if defined(VK_KHR_dynamic_rendering)
        if (*device_extension_bits & vulkan_extension_dynamic_rendering_bit) {
            api->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)(void *)device_proc_address(instance_api, device, "vkCmdBeginRenderingKHR");
            api->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)(void *)device_proc_address(instance_api, device, "vkCmdEndRenderingKHR");
        }
#endif
    } /* core 1.3 */
    if (!api->vkCmdBeginRendering ||
        !api->vkCmdEndRendering)
    {
        log_error("Can't load Vulkan core 1.3 procedures nor required compatible extensions: \n"
                  "\tVK_KHR_dynamic_rendering - %s",
                  (api->vkCmdBeginRendering && api->vkCmdEndRendering) ? "TRUE" : "FALSE");
        return result_error_undefined; // TODO
    }

#if defined(VK_VERSION_1_4)
    if (drivers_version >= VK_API_VERSION_1_4) {
        api->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations)(void *)device_proc_address(instance_api, device, "vkCmdSetRenderingAttachmentLocations");
        api->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices)(void *)device_proc_address(instance_api, device, "vkCmdSetRenderingInputAttachmentIndices");
    } else
#endif
    {
#if defined(VK_KHR_dynamic_rendering_local_read)
        if (*device_extension_bits & vulkan_extension_dynamic_rendering_local_read_bit) {
            api->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)(void *)device_proc_address(instance_api, device, "vkCmdSetRenderingAttachmentLocationsKHR");
            api->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)(void *)device_proc_address(instance_api, device, "vkCmdSetRenderingInputAttachmentIndicesKHR");
        }
#endif
    } /* core 1.4 */
    if (!api->vkCmdSetRenderingAttachmentLocations ||
        !api->vkCmdSetRenderingInputAttachmentIndices)
    {
        log_error("Can't load Vulkan core 1.4 procedures nor required compatible extensions: \n"
                  "\tVK_KHR_dynamic_rendering_local_read - %s",
                  (api->vkCmdSetRenderingAttachmentLocations && api->vkCmdSetRenderingInputAttachmentIndices) ? "TRUE" : "FALSE");
        return result_error_undefined; // TODO
    }

#if defined(VK_KHR_swapchain)
    if (*device_extension_bits & vulkan_extension_swapchain_bit) {
        api->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(void *)device_proc_address(instance_api, device, "vkAcquireNextImageKHR");
        api->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)(void *)device_proc_address(instance_api, device, "vkAcquireNextImage2KHR");
        api->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(void *)device_proc_address(instance_api, device, "vkCreateSwapchainKHR");
        api->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(void *)device_proc_address(instance_api, device, "vkDestroySwapchainKHR");
        api->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(void *)device_proc_address(instance_api, device, "vkGetSwapchainImagesKHR");
        api->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(void *)device_proc_address(instance_api, device, "vkQueuePresentKHR");

        if (!api->vkAcquireNextImageKHR ||
            !api->vkAcquireNextImage2KHR ||
            !api->vkCreateSwapchainKHR ||
            !api->vkDestroySwapchainKHR ||
            !api->vkGetSwapchainImagesKHR ||
            !api->vkQueuePresentKHR)
        {
            log_error("Can't load VK_KHR_swapchain extension procedures. This extension is required.");
            return result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_swapchain */
#if defined(VK_KHR_display_swapchain) && defined(AMW_NATIVE_KMS)
    if (*device_extension_bits & vulkan_extension_display_swapchain_bit) {
        api->vkCreateSharedSwapchainsKHR = (PFN_vkCreateSharedSwapchainsKHR)(void *)device_proc_address(instance_api, device, "vkCreateSharedSwapchainsKHR");

        if (!api->vkCreateSharedSwapchainsKHR) {
            log_warn("Can't load vkCreateSharedSwapchainsKHR from the VK_KHR_display_swapchain extension.");
            *device_extension_bits &= ~vulkan_extension_display_swapchain_bit;
        }
    }
#endif /* VK_KHR_display_swapchain */
#if defined(VK_EXT_device_fault)
    if (*device_extension_bits & vulkan_extension_device_fault_bit) {
        api->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)(void *)device_proc_address(instance_api, device, "vkGetDeviceFaultInfoEXT");

        if (!api->vkGetDeviceFaultInfoEXT) {
            log_warn("Can't load vkGetDeviceFaultInfoEXT from the VK_EXT_device_fault extension.");
            *device_extension_bits &= ~vulkan_extension_device_fault_bit;
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_EXT_device_fault */
#if defined(VK_AMD_shader_info)
    if (*device_extension_bits & vulkan_extension_amd_shader_info_bit) {
        api->vkGetShaderInfoAMD = (PFN_vkGetShaderInfoAMD)(void *)device_proc_address(instance_api, device, "vkGetShaderInfoAMD");

        if (!api->vkGetShaderInfoAMD) {
            log_warn("Can't load vkGetShaderInfoAMD from the VK_AMD_shader_info extension.");
            *device_extension_bits &= ~vulkan_extension_amd_shader_info_bit;
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_AMD_shader_info */
#if defined(VK_KHR_deferred_host_operations)
    if (*device_extension_bits & vulkan_extension_deferred_host_operations_bit) {
        api->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)(void *)device_proc_address(instance_api, device, "vkCreateDeferredOperationKHR");
        api->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)(void *)device_proc_address(instance_api, device, "vkDeferredOperationJoinKHR");
        api->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)(void *)device_proc_address(instance_api, device, "vkDestroyDeferredOperationKHR");
        api->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)(void *)device_proc_address(instance_api, device, "vkGetDeferredOperationMaxConcurrencyKHR");
        api->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)(void *)device_proc_address(instance_api, device, "vkGetDeferredOperationResultKHR");

        if (!api->vkCreateDeferredOperationKHR ||
            !api->vkDeferredOperationJoinKHR ||
            !api->vkDestroyDeferredOperationKHR ||
            !api->vkGetDeferredOperationMaxConcurrencyKHR ||
            !api->vkGetDeferredOperationResultKHR)
        {
            log_warn("Can't load VK_KHR_deferred_host_operations extension procedures. Raytracing GPU acceleration is unavailable");
            *device_extension_bits &= ~vulkan_extension_deferred_host_operations_bit;
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_deferred_host_operations */
#if defined(VK_KHR_acceleration_structure)
    if (*device_extension_bits & vulkan_extension_acceleration_structure_bit) {
        api->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)(void *)device_proc_address(instance_api, device, "vkBuildAccelerationStructuresKHR");
        api->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)(void *)device_proc_address(instance_api, device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        api->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)(void *)device_proc_address(instance_api, device, "vkCmdBuildAccelerationStructuresKHR");
        api->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkCmdCopyAccelerationStructureKHR");
        api->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)(void *)device_proc_address(instance_api, device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        api->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        api->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)(void *)device_proc_address(instance_api, device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        api->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkCopyAccelerationStructureKHR");
        api->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)(void *)device_proc_address(instance_api, device, "vkCopyAccelerationStructureToMemoryKHR");
        api->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkCopyMemoryToAccelerationStructureKHR");
        api->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkCreateAccelerationStructureKHR");
        api->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)(void *)device_proc_address(instance_api, device, "vkDestroyAccelerationStructureKHR");
        api->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)(void *)device_proc_address(instance_api, device, "vkGetAccelerationStructureBuildSizesKHR");
        api->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)(void *)device_proc_address(instance_api, device, "vkGetAccelerationStructureDeviceAddressKHR");
        api->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)(void *)device_proc_address(instance_api, device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        api->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)(void *)device_proc_address(instance_api, device, "vkWriteAccelerationStructuresPropertiesKHR");
    
        if (!api->vkBuildAccelerationStructuresKHR ||
            !api->vkCmdBuildAccelerationStructuresIndirectKHR ||
            !api->vkCmdBuildAccelerationStructuresKHR ||
            !api->vkCmdCopyAccelerationStructureKHR ||
            !api->vkCmdCopyAccelerationStructureToMemoryKHR ||
            !api->vkCmdCopyMemoryToAccelerationStructureKHR ||
            !api->vkCmdWriteAccelerationStructuresPropertiesKHR ||
            !api->vkCopyAccelerationStructureKHR ||
            !api->vkCopyAccelerationStructureToMemoryKHR ||
            !api->vkCopyMemoryToAccelerationStructureKHR ||
            !api->vkCreateAccelerationStructureKHR ||
            !api->vkDestroyAccelerationStructureKHR ||
            !api->vkGetAccelerationStructureBuildSizesKHR ||
            !api->vkGetAccelerationStructureDeviceAddressKHR ||
            !api->vkGetDeviceAccelerationStructureCompatibilityKHR ||
            !api->vkWriteAccelerationStructuresPropertiesKHR)
        {
            log_warn("Can't load VK_KHR_acceleration_structure extension procedures. Raytracing GPU acceleration is unavailable");
            *device_extension_bits &= ~vulkan_extension_acceleration_structure_bit;
            out = result_error_undefined; // TODO
        }
    }
#endif /* VK_KHR_acceleration_structure */
    return out;
}
