#include <lake/bedrock/os.h>

#include "vk_cobalt.h"

#if defined(AMW_PLATFORM_APPLE)
#include <stdlib.h> /* getenv, for MacOS */
#endif

static PFN_vkVoidFunction instance_proc_address(vulkan_cobalt *vk, VkInstance instance, const char *procname)
{
    PFN_vkVoidFunction address = vk->api.vkGetInstanceProcAddr(instance, procname);
    if (address == NULL) {
        log_debug("Can't find a Vulkan instance function of name '%s'", procname);
    }
    return address;
}

static PFN_vkVoidFunction device_proc_address(vulkan_cobalt *vk, VkDevice device, const char *procname)
{
    PFN_vkVoidFunction address = vk->api.vkGetDeviceProcAddr(device, procname);
    if (address == NULL) {
        log_debug("Can't find a Vulkan device function of name '%s'", procname);
    }
    return address;
}

b32 vulkan_open_driver(vulkan_cobalt *vk)
{
    vk->api.module = NULL;

#if defined(AMW_PLATFORM_WINDOWS)
    vk->api.module = bedrock_load_dll("vulkan-1.dll");
#elif defined(AMW_PLATFORM_APPLE)
    vk->api.module = bedrock_load_dll("libvulkan.dylib");
    if (!vk->api.module)
        vk->api.module = bedrock_load_dll("libvulkan.1.dylib");
    if (!vk->api.module)
        vk->api.module = bedrock_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a Framework. App store rules for iOS
     * strictly enforce no .dylib's. If they aren't found it just falls through */
    if (!vk->api.module)
        vk->api.module = bedrock_load_dll("vulkan.framework/vulkan");
    if (!vk->api.module)
        vk->api.module = bedrock_load_dll("MoltenVK.framework/MoltenVK");
    /* modern versions of macOS don't search /usr/local/lib automatically contrary to what man dlopen says
	 * Vulkan SDK uses this as the system-wide installation location, 
     * so we're going to fallback to this if all else fails */
    if (!api->module && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL)
        vk->api.module = bedrock_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    vk->api.module = bedrock_load_dll("libvulkan.so.1");
    if (!vk->api.module)
        vk->api.module = bedrock_load_dll("libvulkan.so");
#endif
    if (!vk->api.module) {
        log_error("Can't connect to a Vulkan driver library.");
        return false;
    }
    vk->api.vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)bedrock_get_proc_address(vk->api.module, "vkGetInstanceProcAddr");

    if (vk->api.vkGetInstanceProcAddr == NULL) {
        vulkan_close_driver(vk);
        log_error("Can't load vkGetInstanceProcAddr from Vulkan drivers.");
        return false;
    }

    vk->api.vkCreateInstance                        = (PFN_vkCreateInstance)(void *)instance_proc_address(vk, NULL, "vkCreateInstance");
    vk->api.vkEnumerateInstanceVersion              = (PFN_vkEnumerateInstanceVersion)(void *)instance_proc_address(vk, NULL, "vkEnumerateInstanceVersion");
    vk->api.vkEnumerateInstanceExtensionProperties  = (PFN_vkEnumerateInstanceExtensionProperties)(void *)instance_proc_address(vk, NULL, "vkEnumerateInstanceExtensionProperties");
    vk->api.vkEnumerateInstanceLayerProperties      = (PFN_vkEnumerateInstanceLayerProperties)(void *)instance_proc_address(vk, NULL, "vkEnumerateInstanceLayerProperties");

    if (!vk->api.vkCreateInstance ||
        !vk->api.vkEnumerateInstanceVersion ||
        !vk->api.vkEnumerateInstanceExtensionProperties ||
        !vk->api.vkEnumerateInstanceLayerProperties) 
    {
        vulkan_close_driver(vk);
        log_error("Can't load global function pointers from Vulkan drivers.");
        return false;
    }
    return true;
}

void vulkan_close_driver(vulkan_cobalt *vk)
{
    if (vk->api.module)
        bedrock_close_dll(vk->api.module);
    vk->api.vkGetDeviceProcAddr = NULL;
    vk->api.vkGetInstanceProcAddr = NULL;
    vk->api.module = NULL;
}

s32 vulkan_load_instance_api_procedures(vulkan_cobalt *vk)
{
    vk->api.vkCreateDevice = (PFN_vkCreateDevice)(void *)instance_proc_address(vk, vk->instance, "vkCreateDevice");
    vk->api.vkDestroyInstance = (PFN_vkDestroyInstance)(void *)instance_proc_address(vk, vk->instance, "vkDestroyInstance");
    vk->api.vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)(void *)instance_proc_address(vk, vk->instance, "vkEnumerateDeviceExtensionProperties");
    vk->api.vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)(void *)instance_proc_address(vk, vk->instance, "vkEnumeratePhysicalDeviceGroups");
    vk->api.vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)(void *)instance_proc_address(vk, vk->instance, "vkEnumeratePhysicalDevices");
    vk->api.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)(void *)instance_proc_address(vk, vk->instance, "vkGetDeviceProcAddr");
    vk->api.vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceFeatures");
    vk->api.vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceFeatures2");
    vk->api.vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceFormatProperties");
    vk->api.vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceFormatProperties2");
    vk->api.vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceImageFormatProperties");
    vk->api.vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceImageFormatProperties2");
    vk->api.vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceMemoryProperties");
    vk->api.vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceMemoryProperties2");
    vk->api.vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceProperties");
    vk->api.vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceProperties2");
    vk->api.vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vk->api.vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceQueueFamilyProperties2");

    if (!vk->api.vkCreateDevice ||
        !vk->api.vkDestroyInstance ||
        !vk->api.vkEnumerateDeviceExtensionProperties ||
        !vk->api.vkEnumeratePhysicalDeviceGroups ||
        !vk->api.vkEnumeratePhysicalDevices ||
        !vk->api.vkGetDeviceProcAddr ||
        !vk->api.vkGetPhysicalDeviceFeatures ||
        !vk->api.vkGetPhysicalDeviceFeatures2 ||
        !vk->api.vkGetPhysicalDeviceFormatProperties ||
        !vk->api.vkGetPhysicalDeviceFormatProperties2 ||
        !vk->api.vkGetPhysicalDeviceImageFormatProperties ||
        !vk->api.vkGetPhysicalDeviceImageFormatProperties2 ||
        !vk->api.vkGetPhysicalDeviceMemoryProperties ||
        !vk->api.vkGetPhysicalDeviceMemoryProperties2 ||
        !vk->api.vkGetPhysicalDeviceProperties ||
        !vk->api.vkGetPhysicalDeviceProperties2 ||
        !vk->api.vkGetPhysicalDeviceQueueFamilyProperties ||
        !vk->api.vkGetPhysicalDeviceQueueFamilyProperties2)
    {
        return result_error_missing_procedure;
    }
#if defined(VK_KHR_surface)
    if (vk->extensions & vulkan_extension_surface_bit) {
        vk->api.vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkDestroySurfaceKHR");
        vk->api.vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        vk->api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!vk->api.vkDestroySurfaceKHR ||
            !vk->api.vkGetPhysicalDeviceSurfaceSupportKHR ||
            !vk->api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !vk->api.vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !vk->api.vkGetPhysicalDeviceSurfacePresentModesKHR)
        {
            return result_error_missing_procedure; 
        }
    }
#endif /* VK_KHR_surface */
#if defined(VK_KHR_win32_surface) && defined(AMW_PLATFORM_WINDOWS)
    if (vk->extensions & vulkan_extension_win32_surface_bit) {
        vk->api.vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateWin32SurfaceKHR");
        vk->api.vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");

        if (!vk->api.vkCreateWin32SurfaceKHR || !vk->api.vkGetPhysicalDeviceWin32PresentationSupportKHR)
            return result_error_missing_procedure;
    }
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface) && defined(AMW_PLATFORM_APPLE)
    if (vk->extensions & vulkan_extension_metal_surface_bit) {
        vk->api.vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)(void *)instance_proc_address(vk, vk->instance, "vkCreateMetalSurfaceEXT");

        if (!vk->api.vkCreateMetalSurfaceEXT)
            return result_error_missing_procedure;
    }
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface) && defined(AMW_NATIVE_WAYLAND)
    if (vk->extensions & vulkan_extension_wayland_surface_bit) {
        vk->api.vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateWaylandSurfaceKHR");
        vk->api.vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");

        if (!vk->api.vkCreateWaylandSurfaceKHR || !vk->api.vkGetPhysicalDeviceWaylandPresentationSupportKHR)
            return result_error_missing_procedure;
    }
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface) && defined(AMW_NATIVE_XCB)
    if (vk->extensions & vulkan_extension_xcb_surface_bit) {
        vk->api.vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateXcbSurfaceKHR");
        vk->api.vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");

        if (!vk->api.vkCreateXcbSurfaceKHR || !vk->api.vkGetPhysicalDeviceXcbPresentationSupportKHR) 
            return result_error_missing_procedure;
    }
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface) && defined(AMW_PLATFORM_ANDROID)
    if (vk->extensions & vulkan_extension_android_surface_bit) {
        vk->api.vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateAndroidSurfaceKHR");

        if (!vk->api.vkCreateAndroidSurfaceKHR)
            return result_error_missing_procedure;
    }
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    if (vk->extensions & vulkan_extension_headless_surface_bit) {
        vk->api.vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT)(void *)instance_proc_address(vk, vk->instance, "vkCreateHeadlessSurfaceEXT");

        if (!vk->api.vkCreateHeadlessSurfaceEXT)
            return result_error_missing_procedure;
    }
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display)
    if (vk->extensions & vulkan_extension_display_bit) {
        vk->api.vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateDisplayModeKHR");
        vk->api.vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)(void *)instance_proc_address(vk, vk->instance, "vkCreateDisplayPlaneSurfaceKHR");
        vk->api.vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetDisplayModePropertiesKHR");
        vk->api.vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetDisplayPlaneCapabilitiesKHR");
        vk->api.vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
        vk->api.vkGetPhysicalDeviceDisplayPlanePropertiesKHR= (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
        vk->api.vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)(void *)instance_proc_address(vk, vk->instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");

        if (!vk->api.vkCreateDisplayModeKHR ||
            !vk->api.vkCreateDisplayPlaneSurfaceKHR ||
            !vk->api.vkGetDisplayModePropertiesKHR ||
            !vk->api.vkGetDisplayPlaneCapabilitiesKHR ||
            !vk->api.vkGetDisplayPlaneSupportedDisplaysKHR ||
            !vk->api.vkGetPhysicalDeviceDisplayPlanePropertiesKHR ||
            !vk->api.vkGetPhysicalDeviceDisplayPropertiesKHR)
        {
            return result_error_missing_procedure;
        }
    }
#endif /* VK_KHR_display */
#if defined(VK_EXT_debug_utils) && !defined(AMW_NDEBUG)
    if (vk->extensions & vulkan_extension_debug_utils_bit) {
        vk->api.vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)(void *)instance_proc_address(vk, vk->instance, "vkSetDebugUtilsObjectNameEXT");
        vk->api.vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)(void *)instance_proc_address(vk, vk->instance, "vkSetDebugUtilsObjectTagEXT");
        vk->api.vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkQueueBeginDebugUtilsLabelEXT");
        vk->api.vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkQueueEndDebugUtilsLabelEXT");
        vk->api.vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkQueueInsertDebugUtilsLabelEXT");
        vk->api.vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkCmdBeginDebugUtilsLabelEXT");
        vk->api.vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkCmdEndDebugUtilsLabelEXT");
        vk->api.vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)(void *)instance_proc_address(vk, vk->instance, "vkCmdInsertDebugUtilsLabelEXT");
        vk->api.vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)(void *)instance_proc_address(vk, vk->instance, "vkCreateDebugUtilsMessengerEXT");
        vk->api.vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)(void *)instance_proc_address(vk, vk->instance, "vkDestroyDebugUtilsMessengerEXT");
        vk->api.vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)(void *)instance_proc_address(vk, vk->instance, "vkSubmitDebugUtilsMessageEXT");

        if (!vk->api.vkSetDebugUtilsObjectNameEXT ||
            !vk->api.vkSetDebugUtilsObjectTagEXT ||
            !vk->api.vkQueueBeginDebugUtilsLabelEXT ||
            !vk->api.vkQueueEndDebugUtilsLabelEXT ||
            !vk->api.vkQueueInsertDebugUtilsLabelEXT ||
            !vk->api.vkCmdBeginDebugUtilsLabelEXT ||
            !vk->api.vkCmdEndDebugUtilsLabelEXT ||
            !vk->api.vkCmdInsertDebugUtilsLabelEXT ||
            !vk->api.vkCreateDebugUtilsMessengerEXT ||
            !vk->api.vkDestroyDebugUtilsMessengerEXT ||
            !vk->api.vkSubmitDebugUtilsMessageEXT) 
        {
            return result_error_missing_procedure;
        }
    }
#endif /* VK_EXT_debug_utils */
    return result_success;
}

s32 vulkan_load_device_api_procedures(
    vulkan_cobalt     *vk,
    vulkan_device_api *api,
    VkDevice           device,
    u32                device_api_version,
    u32                device_extensions)
{
    /* core 1.0 */
	api->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)(void *)device_proc_address(vk, device, "vkAllocateCommandBuffers");
	api->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)(void *)device_proc_address(vk, device, "vkAllocateDescriptorSets");
	api->vkAllocateMemory = (PFN_vkAllocateMemory)(void *)device_proc_address(vk, device, "vkAllocateMemory");
	api->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)(void *)device_proc_address(vk, device, "vkBeginCommandBuffer");
	api->vkBindBufferMemory = (PFN_vkBindBufferMemory)(void *)device_proc_address(vk, device, "vkBindBufferMemory");
	api->vkBindImageMemory = (PFN_vkBindImageMemory)(void *)device_proc_address(vk, device, "vkBindImageMemory");
	api->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)(void *)device_proc_address(vk, device, "vkCmdBeginQuery");
	api->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)(void *)device_proc_address(vk, device, "vkCmdBeginRenderPass");
	api->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)(void *)device_proc_address(vk, device, "vkCmdBindDescriptorSets");
	api->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)(void *)device_proc_address(vk, device, "vkCmdBindIndexBuffer");
	api->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)(void *)device_proc_address(vk, device, "vkCmdBindPipeline");
	api->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)(void *)device_proc_address(vk, device, "vkCmdBindVertexBuffers");
	api->vkCmdBlitImage = (PFN_vkCmdBlitImage)(void *)device_proc_address(vk, device, "vkCmdBlitImage");
	api->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)(void *)device_proc_address(vk, device, "vkCmdClearAttachments");
	api->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)(void *)device_proc_address(vk, device, "vkCmdClearColorImage");
	api->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)(void *)device_proc_address(vk, device, "vkCmdClearDepthStencilImage");
	api->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)(void *)device_proc_address(vk, device, "vkCmdCopyBuffer");
	api->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)(void *)device_proc_address(vk, device, "vkCmdCopyBufferToImage");
	api->vkCmdCopyImage = (PFN_vkCmdCopyImage)(void *)device_proc_address(vk, device, "vkCmdCopyImage");
	api->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)(void *)device_proc_address(vk, device, "vkCmdCopyImageToBuffer");
	api->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)(void *)device_proc_address(vk, device, "vkCmdCopyQueryPoolResults");
	api->vkCmdDispatch = (PFN_vkCmdDispatch)(void *)device_proc_address(vk, device, "vkCmdDispatch");
	api->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)(void *)device_proc_address(vk, device, "vkCmdDispatchIndirect");
	api->vkCmdDraw = (PFN_vkCmdDraw)(void *)device_proc_address(vk, device, "vkCmdDraw");
	api->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)(void *)device_proc_address(vk, device, "vkCmdDrawIndexed");
	api->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)(void *)device_proc_address(vk, device, "vkCmdDrawIndexedIndirect");
	api->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)(void *)device_proc_address(vk, device, "vkCmdDrawIndirect");
	api->vkCmdEndQuery = (PFN_vkCmdEndQuery)(void *)device_proc_address(vk, device, "vkCmdEndQuery");
	api->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)(void *)device_proc_address(vk, device, "vkCmdEndRenderPass");
	api->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)(void *)device_proc_address(vk, device, "vkCmdExecuteCommands");
	api->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)(void *)device_proc_address(vk, device, "vkCmdFillBuffer");
	api->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)(void *)device_proc_address(vk, device, "vkCmdNextSubpass");
	api->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)(void *)device_proc_address(vk, device, "vkCmdPipelineBarrier");
	api->vkCmdPushConstants = (PFN_vkCmdPushConstants)(void *)device_proc_address(vk, device, "vkCmdPushConstants");
	api->vkCmdResetEvent = (PFN_vkCmdResetEvent)(void *)device_proc_address(vk, device, "vkCmdResetEvent");
	api->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)(void *)device_proc_address(vk, device, "vkCmdResetQueryPool");
	api->vkCmdResolveImage = (PFN_vkCmdResolveImage)(void *)device_proc_address(vk, device, "vkCmdResolveImage");
	api->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)(void *)device_proc_address(vk, device, "vkCmdSetBlendConstants");
	api->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)(void *)device_proc_address(vk, device, "vkCmdSetDepthBias");
	api->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)(void *)device_proc_address(vk, device, "vkCmdSetDepthBounds");
	api->vkCmdSetEvent = (PFN_vkCmdSetEvent)(void *)device_proc_address(vk, device, "vkCmdSetEvent");
	api->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)(void *)device_proc_address(vk, device, "vkCmdSetLineWidth");
	api->vkCmdSetScissor = (PFN_vkCmdSetScissor)(void *)device_proc_address(vk, device, "vkCmdSetScissor");
	api->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)(void *)device_proc_address(vk, device, "vkCmdSetStencilCompareMask");
	api->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)(void *)device_proc_address(vk, device, "vkCmdSetStencilReference");
	api->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)(void *)device_proc_address(vk, device, "vkCmdSetStencilWriteMask");
	api->vkCmdSetViewport = (PFN_vkCmdSetViewport)(void *)device_proc_address(vk, device, "vkCmdSetViewport");
	api->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)(void *)device_proc_address(vk, device, "vkCmdUpdateBuffer");
	api->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)(void *)device_proc_address(vk, device, "vkCmdWaitEvents");
	api->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)(void *)device_proc_address(vk, device, "vkCmdWriteTimestamp");
	api->vkCreateBuffer = (PFN_vkCreateBuffer)(void *)device_proc_address(vk, device, "vkCreateBuffer");
	api->vkCreateBufferView = (PFN_vkCreateBufferView)(void *)device_proc_address(vk, device, "vkCreateBufferView");
	api->vkCreateCommandPool = (PFN_vkCreateCommandPool)(void *)device_proc_address(vk, device, "vkCreateCommandPool");
	api->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)(void *)device_proc_address(vk, device, "vkCreateComputePipelines");
	api->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)(void *)device_proc_address(vk, device, "vkCreateDescriptorPool");
	api->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)(void *)device_proc_address(vk, device, "vkCreateDescriptorSetLayout");
	api->vkCreateEvent = (PFN_vkCreateEvent)(void *)device_proc_address(vk, device, "vkCreateEvent");
	api->vkCreateFence = (PFN_vkCreateFence)(void *)device_proc_address(vk, device, "vkCreateFence");
	api->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)(void *)device_proc_address(vk, device, "vkCreateFramebuffer");
	api->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)(void *)device_proc_address(vk, device, "vkCreateGraphicsPipelines");
	api->vkCreateImage = (PFN_vkCreateImage)(void *)device_proc_address(vk, device, "vkCreateImage");
	api->vkCreateImageView = (PFN_vkCreateImageView)(void *)device_proc_address(vk, device, "vkCreateImageView");
	api->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)(void *)device_proc_address(vk, device, "vkCreatePipelineCache");
	api->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)(void *)device_proc_address(vk, device, "vkCreatePipelineLayout");
	api->vkCreateQueryPool = (PFN_vkCreateQueryPool)(void *)device_proc_address(vk, device, "vkCreateQueryPool");
	api->vkCreateRenderPass = (PFN_vkCreateRenderPass)(void *)device_proc_address(vk, device, "vkCreateRenderPass");
	api->vkCreateSampler = (PFN_vkCreateSampler)(void *)device_proc_address(vk, device, "vkCreateSampler");
	api->vkCreateSemaphore = (PFN_vkCreateSemaphore)(void *)device_proc_address(vk, device, "vkCreateSemaphore");
	api->vkCreateShaderModule = (PFN_vkCreateShaderModule)(void *)device_proc_address(vk, device, "vkCreateShaderModule");
	api->vkDestroyBuffer = (PFN_vkDestroyBuffer)(void *)device_proc_address(vk, device, "vkDestroyBuffer");
	api->vkDestroyBufferView = (PFN_vkDestroyBufferView)(void *)device_proc_address(vk, device, "vkDestroyBufferView");
	api->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)(void *)device_proc_address(vk, device, "vkDestroyCommandPool");
	api->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)(void *)device_proc_address(vk, device, "vkDestroyDescriptorPool");
	api->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)(void *)device_proc_address(vk, device, "vkDestroyDescriptorSetLayout");
	api->vkDestroyDevice = (PFN_vkDestroyDevice)(void *)device_proc_address(vk, device, "vkDestroyDevice");
	api->vkDestroyEvent = (PFN_vkDestroyEvent)(void *)device_proc_address(vk, device, "vkDestroyEvent");
	api->vkDestroyFence = (PFN_vkDestroyFence)(void *)device_proc_address(vk, device, "vkDestroyFence");
	api->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)(void *)device_proc_address(vk, device, "vkDestroyFramebuffer");
	api->vkDestroyImage = (PFN_vkDestroyImage)(void *)device_proc_address(vk, device, "vkDestroyImage");
	api->vkDestroyImageView = (PFN_vkDestroyImageView)(void *)device_proc_address(vk, device, "vkDestroyImageView");
	api->vkDestroyPipeline = (PFN_vkDestroyPipeline)(void *)device_proc_address(vk, device, "vkDestroyPipeline");
	api->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)(void *)device_proc_address(vk, device, "vkDestroyPipelineCache");
	api->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)(void *)device_proc_address(vk, device, "vkDestroyPipelineLayout");
	api->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)(void *)device_proc_address(vk, device, "vkDestroyQueryPool");
	api->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)(void *)device_proc_address(vk, device, "vkDestroyRenderPass");
	api->vkDestroySampler = (PFN_vkDestroySampler)(void *)device_proc_address(vk, device, "vkDestroySampler");
	api->vkDestroySemaphore = (PFN_vkDestroySemaphore)(void *)device_proc_address(vk, device, "vkDestroySemaphore");
	api->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)(void *)device_proc_address(vk, device, "vkDestroyShaderModule");
	api->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)(void *)device_proc_address(vk, device, "vkDeviceWaitIdle");
	api->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)(void *)device_proc_address(vk, device, "vkEndCommandBuffer");
	api->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)(void *)device_proc_address(vk, device, "vkFlushMappedMemoryRanges");
	api->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)(void *)device_proc_address(vk, device, "vkFreeCommandBuffers");
	api->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)(void *)device_proc_address(vk, device, "vkFreeDescriptorSets");
	api->vkFreeMemory = (PFN_vkFreeMemory)(void *)device_proc_address(vk, device, "vkFreeMemory");
	api->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)(void *)device_proc_address(vk, device, "vkGetBufferMemoryRequirements");
	api->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)(void *)device_proc_address(vk, device, "vkGetDeviceMemoryCommitment");
	api->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)(void *)device_proc_address(vk, device, "vkGetDeviceQueue");
	api->vkGetEventStatus = (PFN_vkGetEventStatus)(void *)device_proc_address(vk, device, "vkGetEventStatus");
	api->vkGetFenceStatus = (PFN_vkGetFenceStatus)(void *)device_proc_address(vk, device, "vkGetFenceStatus");
	api->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)(void *)device_proc_address(vk, device, "vkGetImageMemoryRequirements");
	api->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)(void *)device_proc_address(vk, device, "vkGetImageSparseMemoryRequirements");
	api->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)(void *)device_proc_address(vk, device, "vkGetImageSubresourceLayout");
	api->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)(void *)device_proc_address(vk, device, "vkGetPipelineCacheData");
	api->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)(void *)device_proc_address(vk, device, "vkGetQueryPoolResults");
	api->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)(void *)device_proc_address(vk, device, "vkGetRenderAreaGranularity");
	api->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)(void *)device_proc_address(vk, device, "vkInvalidateMappedMemoryRanges");
	api->vkMapMemory = (PFN_vkMapMemory)(void *)device_proc_address(vk, device, "vkMapMemory");
	api->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)(void *)device_proc_address(vk, device, "vkMergePipelineCaches");
	api->vkQueueBindSparse = (PFN_vkQueueBindSparse)(void *)device_proc_address(vk, device, "vkQueueBindSparse");
	api->vkQueueSubmit = (PFN_vkQueueSubmit)(void *)device_proc_address(vk, device, "vkQueueSubmit");
	api->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)(void *)device_proc_address(vk, device, "vkQueueWaitIdle");
	api->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)(void *)device_proc_address(vk, device, "vkResetCommandBuffer");
	api->vkResetCommandPool = (PFN_vkResetCommandPool)(void *)device_proc_address(vk, device, "vkResetCommandPool");
	api->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)(void *)device_proc_address(vk, device, "vkResetDescriptorPool");
	api->vkResetEvent = (PFN_vkResetEvent)(void *)device_proc_address(vk, device, "vkResetEvent");
	api->vkResetFences = (PFN_vkResetFences)(void *)device_proc_address(vk, device, "vkResetFences");
	api->vkSetEvent = (PFN_vkSetEvent)(void *)device_proc_address(vk, device, "vkSetEvent");
	api->vkUnmapMemory = (PFN_vkUnmapMemory)(void *)device_proc_address(vk, device, "vkUnmapMemory");
	api->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)(void *)device_proc_address(vk, device, "vkUpdateDescriptorSets");
	api->vkWaitForFences = (PFN_vkWaitForFences)(void *)device_proc_address(vk, device, "vkWaitForFences");

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
        return result_error_missing_procedure;
    }

    /* core 1.1 */
    api->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)(void *)device_proc_address(vk, device, "vkBindBufferMemory2");
    api->vkBindImageMemory2 = (PFN_vkBindImageMemory2)(void *)device_proc_address(vk, device, "vkBindImageMemory2");
    api->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)(void *)device_proc_address(vk, device, "vkCmdDispatchBase");
    api->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)(void *)device_proc_address(vk, device, "vkCmdSetDeviceMask");
    api->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)(void *)device_proc_address(vk, device, "vkCreateDescriptorUpdateTemplate");
    api->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)(void *)device_proc_address(vk, device, "vkCreateSamplerYcbcrConversion");
    api->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)(void *)device_proc_address(vk, device, "vkDestroyDescriptorUpdateTemplate");
    api->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)(void *)device_proc_address(vk, device, "vkDestroySamplerYcbcrConversion");
    api->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)(void *)device_proc_address(vk, device, "vkGetBufferMemoryRequirements2");
    api->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)(void *)device_proc_address(vk, device, "vkGetDescriptorSetLayoutSupport");
    api->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)(void *)device_proc_address(vk, device, "vkGetDeviceGroupPeerMemoryFeatures");
    api->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)(void *)device_proc_address(vk, device, "vkGetDeviceQueue2");
    api->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)(void *)device_proc_address(vk, device, "vkGetImageMemoryRequirements2");
    api->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)(void *)device_proc_address(vk, device, "vkGetImageSparseMemoryRequirements2");
    api->vkTrimCommandPool = (PFN_vkTrimCommandPool)(void *)device_proc_address(vk, device, "vkTrimCommandPool");
    api->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)(void *)device_proc_address(vk, device, "vkUpdateDescriptorSetWithTemplate");

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
        return result_error_missing_procedure;
    }

    /* core 1.2 */
    api->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)(void *)device_proc_address(vk, device, "vkCmdDrawIndexedIndirectCount");
    api->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)(void *)device_proc_address(vk, device, "vkCmdDrawIndirectCount");
    api->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)(void *)device_proc_address(vk, device, "vkGetBufferDeviceAddress");
    api->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)(void *)device_proc_address(vk, device, "vkGetBufferOpaqueCaptureAddress");
    api->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)(void *)device_proc_address(vk, device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    api->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)(void *)device_proc_address(vk, device, "vkGetSemaphoreCounterValue");
    api->vkResetQueryPool = (PFN_vkResetQueryPool)(void *)device_proc_address(vk, device, "vkResetQueryPool");
    api->vkSignalSemaphore = (PFN_vkSignalSemaphore)(void *)device_proc_address(vk, device, "vkSignalSemaphore");
    api->vkWaitSemaphores = (PFN_vkWaitSemaphores)(void *)device_proc_address(vk, device, "vkWaitSemaphores");

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
        log_error("Can't load Vulkan core 1.2 device procedures.");
        return result_error_missing_procedure;
    }

    /* core 1.3 */
    if (device_api_version >= VK_API_VERSION_1_3) {
        api->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)(void *)device_proc_address(vk, device, "vkCmdBeginRendering");
        api->vkCmdEndRendering = (PFN_vkCmdEndRendering)(void *)device_proc_address(vk, device, "vkCmdEndRendering");
    } else {
        if (device_extensions & vulkan_extension_dynamic_rendering_bit) {
            api->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)(void *)device_proc_address(vk, device, "vkCmdBeginRenderingKHR");
            api->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)(void *)device_proc_address(vk, device, "vkCmdEndRenderingKHR");
        }
    }
    if (!api->vkCmdBeginRendering || !api->vkCmdEndRendering) {
        log_error("Can't load Vulkan core 1.3 procedures nor required compatible extensions: \n"
                  "\tVK_KHR_dynamic_rendering - %s",
                  (api->vkCmdBeginRendering && api->vkCmdEndRendering) ? "TRUE" : "FALSE");
        return result_error_missing_procedure;
    }

#if defined(VK_KHR_swapchain)
    if (device_extensions & vulkan_extension_swapchain_bit) {
        api->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)(void *)device_proc_address(vk, device, "vkAcquireNextImageKHR");
        api->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)(void *)device_proc_address(vk, device, "vkAcquireNextImage2KHR");
        api->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)(void *)device_proc_address(vk, device, "vkCreateSwapchainKHR");
        api->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)(void *)device_proc_address(vk, device, "vkDestroySwapchainKHR");
        api->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)(void *)device_proc_address(vk, device, "vkGetSwapchainImagesKHR");
        api->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)(void *)device_proc_address(vk, device, "vkQueuePresentKHR");

        if (!api->vkAcquireNextImageKHR ||
            !api->vkAcquireNextImage2KHR ||
            !api->vkCreateSwapchainKHR ||
            !api->vkDestroySwapchainKHR ||
            !api->vkGetSwapchainImagesKHR ||
            !api->vkQueuePresentKHR)
        {
            return result_error_missing_procedure;
        }
    }
#endif /* VK_KHR_swapchain */
#if defined(VK_EXT_device_fault)
    if (device_extensions & vulkan_extension_device_fault_bit) {
        api->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)(void *)device_proc_address(vk, device, "vkGetDeviceFaultInfoEXT");

        if (!api->vkGetDeviceFaultInfoEXT)
            return result_error_missing_procedure;
    }
#endif /* VK_EXT_device_fault */
#if defined(VK_AMD_shader_info)
    if (device_extensions & vulkan_extension_amd_shader_info_bit) {
        api->vkGetShaderInfoAMD = (PFN_vkGetShaderInfoAMD)(void *)device_proc_address(vk, device, "vkGetShaderInfoAMD");

        if (!api->vkGetShaderInfoAMD)
            return result_error_missing_procedure;
    }
#endif /* VK_AMD_shader_info */
#if defined(VK_KHR_deferred_host_operations)
    if (device_extensions & vulkan_extension_deferred_host_operations_bit) {
        api->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)(void *)device_proc_address(vk, device, "vkCreateDeferredOperationKHR");
        api->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)(void *)device_proc_address(vk, device, "vkDeferredOperationJoinKHR");
        api->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)(void *)device_proc_address(vk, device, "vkDestroyDeferredOperationKHR");
        api->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)(void *)device_proc_address(vk, device, "vkGetDeferredOperationMaxConcurrencyKHR");
        api->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)(void *)device_proc_address(vk, device, "vkGetDeferredOperationResultKHR");

        if (!api->vkCreateDeferredOperationKHR ||
            !api->vkDeferredOperationJoinKHR ||
            !api->vkDestroyDeferredOperationKHR ||
            !api->vkGetDeferredOperationMaxConcurrencyKHR ||
            !api->vkGetDeferredOperationResultKHR)
        {
            return result_error_missing_procedure;
        }
    }
#endif /* VK_KHR_deferred_host_operations */
#if defined(VK_KHR_acceleration_structure)
    if (device_extensions & vulkan_extension_acceleration_structure_bit) {
        api->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)(void *)device_proc_address(vk, device, "vkBuildAccelerationStructuresKHR");
        api->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)(void *)device_proc_address(vk, device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        api->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)(void *)device_proc_address(vk, device, "vkCmdBuildAccelerationStructuresKHR");
        api->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkCmdCopyAccelerationStructureKHR");
        api->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)(void *)device_proc_address(vk, device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        api->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        api->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)(void *)device_proc_address(vk, device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        api->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkCopyAccelerationStructureKHR");
        api->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)(void *)device_proc_address(vk, device, "vkCopyAccelerationStructureToMemoryKHR");
        api->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkCopyMemoryToAccelerationStructureKHR");
        api->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkCreateAccelerationStructureKHR");
        api->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)(void *)device_proc_address(vk, device, "vkDestroyAccelerationStructureKHR");
        api->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)(void *)device_proc_address(vk, device, "vkGetAccelerationStructureBuildSizesKHR");
        api->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)(void *)device_proc_address(vk, device, "vkGetAccelerationStructureDeviceAddressKHR");
        api->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)(void *)device_proc_address(vk, device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        api->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)(void *)device_proc_address(vk, device, "vkWriteAccelerationStructuresPropertiesKHR");
    
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
            return result_error_missing_procedure;
        }
    }
#endif /* VK_KHR_acceleration_structure */
    return result_success;
}
