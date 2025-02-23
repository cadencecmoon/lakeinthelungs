#include <amw/harridan.h>
#include <amw/plugin.h>
#include <amw/log.h>

static PFN_vkVoidFunction instance_proc_address(struct vk_api *api, VkInstance instance, const char *procname)
{
    PFN_vkVoidFunction address = api->vkGetInstanceProcAddr(instance, procname);
    if (address == NULL)
        log_debug("Can't find a Vulkan instance function of name '%s'", procname);
    return address;
}

static PFN_vkVoidFunction device_proc_address(struct vk_api *api, VkDevice device, const char *procname)
{
    PFN_vkVoidFunction address = api->vkGetDeviceProcAddr(device, procname);
    if (address == NULL)
        log_debug("Can't find a Vulkan device function of name '%s'", procname);
    return address;
}

b32 open_vulkan_driver(struct vk_api *api, b32 verbose)
{
    api->module = NULL;

#if defined(PLATFORM_WINDOWS)
    api->module = plugin_load_dll("vulkan-1.dll");
#elif defined(PLATFORM_APPLE)
    api->module = plugin_load_dll("libvulkan.dylib");
    if (!api->module)
        api->module = plugin_load_dll("libvulkan.1.dylib");
    if (!api->module)
        api->module = plugin_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (!api->module)
        api->module = plugin_load_dll("vulkan.framework/vulkan");
    if (!api->module)
        api->module = plugin_load_dll("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (!api->module)
        api->module = plugin_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    api->module = plugin_load_dll("libvulkan.so.1");
    if (!api->module)
        api->module = plugin_load_dll("libvulkan.so");
#endif
    if (!api->module) {
        if (verbose)
            log_error("Can't load any Vulkan drivers. Ensure they are correclty installed and available via the system PATH.");
        return false;
    }

    api->vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)plugin_get_proc_address(api->module, "vkGetInstanceProcAddr");
    if (api->vkGetInstanceProcAddr == NULL) {
        close_vulkan_driver(api);
        if (verbose)
            log_error("Can't load vkGetInstanceProcAddr from Vulkan drivers.");
        return false;
    }

    api->vkCreateInstance = (PFN_vkCreateInstance)instance_proc_address(api, NULL, "vkCreateInstance");
    api->vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)instance_proc_address(api, NULL, "vkEnumerateInstanceVersion");
    api->vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)instance_proc_address(api, NULL, "vkEnumerateInstanceExtensionProperties");
    api->vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)instance_proc_address(api, NULL, "vkEnumerateInstanceLayerProperties");

    if (!api->vkCreateInstance ||
        !api->vkEnumerateInstanceVersion ||
        !api->vkEnumerateInstanceExtensionProperties ||
        !api->vkEnumerateInstanceLayerProperties) 
    {
        close_vulkan_driver(api);
        if (verbose)
            log_error("Can't load global function pointers from Vulkan drivers.");
        return false;
    }
    return true;
}

void close_vulkan_driver(struct vk_api *api)
{
    if (api->module)
        plugin_close_dll(api->module);
    api->vkGetDeviceProcAddr = NULL;
    api->vkGetInstanceProcAddr = NULL;
    api->module = NULL;
}

b32 load_vulkan_instance_procedures(struct vk_api *api, VkInstance instance)
{
    assert_debug(api->module && api->vkGetInstanceProcAddr);

    /* core procedures */
    api->vkCreateDevice = (PFN_vkCreateDevice)instance_proc_address(api, instance, "vkCreateDevice");
    api->vkDestroyInstance = (PFN_vkDestroyInstance)instance_proc_address(api, instance, "vkDestroyInstance");
    api->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)instance_proc_address(api, instance, "vkEnumerateDeviceExtensionProperties");
    api->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)instance_proc_address(api, instance, "vkEnumeratePhysicalDeviceGroups");
    api->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)instance_proc_address(api, instance, "vkEnumeratePhysicalDevices");
    api->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)instance_proc_address(api, instance, "vkGetDeviceProcAddr");
    api->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)instance_proc_address(api, instance, "vkGetPhysicalDeviceFeatures");
    api->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)instance_proc_address(api, instance, "vkGetPhysicalDeviceFeatures2");
    api->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)instance_proc_address(api, instance, "vkGetPhysicalDeviceFormatProperties");
    api->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)instance_proc_address(api, instance, "vkGetPhysicalDeviceFormatProperties2");
    api->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)instance_proc_address(api, instance, "vkGetPhysicalDeviceImageFormatProperties");
    api->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)instance_proc_address(api, instance, "vkGetPhysicalDeviceImageFormatProperties2");
    api->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instance_proc_address(api, instance, "vkGetPhysicalDeviceMemoryProperties");
    api->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)instance_proc_address(api, instance, "vkGetPhysicalDeviceMemoryProperties2");
    api->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)instance_proc_address(api, instance, "vkGetPhysicalDeviceProperties");
    api->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)instance_proc_address(api, instance, "vkGetPhysicalDeviceProperties2");
    api->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)instance_proc_address(api, instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    api->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)instance_proc_address(api, instance, "vkGetPhysicalDeviceQueueFamilyProperties2");

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
        return false;

    /* surface */
    if (api->extensions & vk_extension_surface_bit) {
        api->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instance_proc_address(api, instance, "vkDestroySurfaceKHR");
        api->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        api->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        api->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        api->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!api->vkDestroySurfaceKHR ||
            !api->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !api->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !api->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !api->vkGetPhysicalDeviceSurfacePresentModesKHR)
            return false; 
    }
#if defined(VK_KHR_win32_surface)
    if (api->extensions & vk_extension_win32_surface_bit) {
        api->vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)instance_proc_address(api, instance, "vkCreateWin32SurfaceKHR");
        api->vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");

        if (!api->vkCreateWin32SurfaceKHR || 
            !api->vkGetPhysicalDeviceWin32PresentationSupportKHR)
            return false;
    }
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface)
    if (api->extensions & vk_extension_metal_surface_bit) {
        api->vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)instance_proc_address(api, instance, "vkCreateMetalSurfaceEXT");

        if (!api->vkCreateMetalSurfaceEXT)
            return false;
    }
#endif /* VK_EXT_metal_surface */
#if defined(VK_KHR_wayland_surface)
    if (api->extensions & vk_extension_wayland_surface_bit) {
        api->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)instance_proc_address(api, instance, "vkCreateWaylandSurfaceKHR");
        api->vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");

        if (!api->vkCreateWaylandSurfaceKHR || 
            !api->vkGetPhysicalDeviceWaylandPresentationSupportKHR)
            return false;
    }
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface)
    if (api->extensions & vk_extension_xcb_surface_bit) {
        api->vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)instance_proc_address(api, instance, "vkCreateXcbSurfaceKHR");
        api->vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");

        if (!api->vkCreateXcbSurfaceKHR || 
            !api->vkGetPhysicalDeviceXcbPresentationSupportKHR) 
            return false;
    }
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface)
    if (api->extensions & vk_extension_android_surface_bit) {
        api->vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)instance_proc_address(api, instance, "vkCreateAndroidSurfaceKHR");

        if (!api->vkCreateAndroidSurfaceKHR)
            return false;
    }
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    if (api->extensions & vk_extension_headless_surface_bit) {
        api->vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT)instance_proc_address(api, instance, "vkCreateHeadlessSurfaceEXT");

        if (!api->vkCreateHeadlessSurfaceEXT)
            return false;
    }
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display)
    if (api->extensions & vk_extension_display_bit) {
        api->vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)instance_proc_address(api, instance, "vkCreateDisplayModeKHR");
        api->vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)instance_proc_address(api, instance, "vkCreateDisplayPlaneSurfaceKHR");
        api->vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)instance_proc_address(api, instance, "vkGetDisplayModePropertiesKHR");
        api->vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)instance_proc_address(api, instance, "vkGetDisplayPlaneCapabilitiesKHR");
        api->vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)instance_proc_address(api, instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
        api->vkGetPhysicalDeviceDisplayPlanePropertiesKHR= (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
        api->vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");

        if (!api->vkCreateDisplayModeKHR ||
            !api->vkCreateDisplayPlaneSurfaceKHR ||
            !api->vkGetDisplayModePropertiesKHR ||
            !api->vkGetDisplayPlaneCapabilitiesKHR ||
            !api->vkGetDisplayPlaneSupportedDisplaysKHR ||
            !api->vkGetPhysicalDeviceDisplayPlanePropertiesKHR ||
            !api->vkGetPhysicalDeviceDisplayPropertiesKHR)
            return false;
    }
#endif /* VK_KHR_display */

    /* debug utils */
    if (api->extensions & vk_extension_debug_utils_bit) {
        api->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)instance_proc_address(api, instance, "vkSetDebugUtilsObjectNameEXT");
        api->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)instance_proc_address(api, instance, "vkSetDebugUtilsObjectTagEXT");
        api->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkQueueBeginDebugUtilsLabelEXT");
        api->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkQueueEndDebugUtilsLabelEXT");
        api->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkQueueInsertDebugUtilsLabelEXT");
        api->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkCmdBeginDebugUtilsLabelEXT");
        api->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkCmdEndDebugUtilsLabelEXT");
        api->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)instance_proc_address(api, instance, "vkCmdInsertDebugUtilsLabelEXT");
        api->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)instance_proc_address(api, instance, "vkCreateDebugUtilsMessengerEXT");
        api->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)instance_proc_address(api, instance, "vkDestroyDebugUtilsMessengerEXT");
        api->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)instance_proc_address(api, instance, "vkSubmitDebugUtilsMessageEXT");

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
            return false;
    }
    return true;
}

b32 load_vulkan_device_procedures(struct vk_api *api, VkDevice device, u32 api_version)
{
    assert_debug(api->module && api->vkGetDeviceProcAddr);

    /* core 1.0 */
	api->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)device_proc_address(api, device, "vkAllocateCommandBuffers");
	api->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)device_proc_address(api, device, "vkAllocateDescriptorSets");
	api->vkAllocateMemory = (PFN_vkAllocateMemory)device_proc_address(api, device, "vkAllocateMemory");
	api->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)device_proc_address(api, device, "vkBeginCommandBuffer");
	api->vkBindBufferMemory = (PFN_vkBindBufferMemory)device_proc_address(api, device, "vkBindBufferMemory");
	api->vkBindImageMemory = (PFN_vkBindImageMemory)device_proc_address(api, device, "vkBindImageMemory");
	api->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)device_proc_address(api, device, "vkCmdBeginQuery");
	api->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)device_proc_address(api, device, "vkCmdBeginRenderPass");
	api->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)device_proc_address(api, device, "vkCmdBindDescriptorSets");
	api->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)device_proc_address(api, device, "vkCmdBindIndexBuffer");
	api->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)device_proc_address(api, device, "vkCmdBindPipeline");
	api->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)device_proc_address(api, device, "vkCmdBindVertexBuffers");
	api->vkCmdBlitImage = (PFN_vkCmdBlitImage)device_proc_address(api, device, "vkCmdBlitImage");
	api->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)device_proc_address(api, device, "vkCmdClearAttachments");
	api->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)device_proc_address(api, device, "vkCmdClearColorImage");
	api->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)device_proc_address(api, device, "vkCmdClearDepthStencilImage");
	api->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)device_proc_address(api, device, "vkCmdCopyBuffer");
	api->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)device_proc_address(api, device, "vkCmdCopyBufferToImage");
	api->vkCmdCopyImage = (PFN_vkCmdCopyImage)device_proc_address(api, device, "vkCmdCopyImage");
	api->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)device_proc_address(api, device, "vkCmdCopyImageToBuffer");
	api->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)device_proc_address(api, device, "vkCmdCopyQueryPoolResults");
	api->vkCmdDispatch = (PFN_vkCmdDispatch)device_proc_address(api, device, "vkCmdDispatch");
	api->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)device_proc_address(api, device, "vkCmdDispatchIndirect");
	api->vkCmdDraw = (PFN_vkCmdDraw)device_proc_address(api, device, "vkCmdDraw");
	api->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)device_proc_address(api, device, "vkCmdDrawIndexed");
	api->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)device_proc_address(api, device, "vkCmdDrawIndexedIndirect");
	api->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)device_proc_address(api, device, "vkCmdDrawIndirect");
	api->vkCmdEndQuery = (PFN_vkCmdEndQuery)device_proc_address(api, device, "vkCmdEndQuery");
	api->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)device_proc_address(api, device, "vkCmdEndRenderPass");
	api->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)device_proc_address(api, device, "vkCmdExecuteCommands");
	api->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)device_proc_address(api, device, "vkCmdFillBuffer");
	api->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)device_proc_address(api, device, "vkCmdNextSubpass");
	api->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)device_proc_address(api, device, "vkCmdPipelineBarrier");
	api->vkCmdPushConstants = (PFN_vkCmdPushConstants)device_proc_address(api, device, "vkCmdPushConstants");
	api->vkCmdResetEvent = (PFN_vkCmdResetEvent)device_proc_address(api, device, "vkCmdResetEvent");
	api->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)device_proc_address(api, device, "vkCmdResetQueryPool");
	api->vkCmdResolveImage = (PFN_vkCmdResolveImage)device_proc_address(api, device, "vkCmdResolveImage");
	api->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)device_proc_address(api, device, "vkCmdSetBlendConstants");
	api->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)device_proc_address(api, device, "vkCmdSetDepthBias");
	api->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)device_proc_address(api, device, "vkCmdSetDepthBounds");
	api->vkCmdSetEvent = (PFN_vkCmdSetEvent)device_proc_address(api, device, "vkCmdSetEvent");
	api->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)device_proc_address(api, device, "vkCmdSetLineWidth");
	api->vkCmdSetScissor = (PFN_vkCmdSetScissor)device_proc_address(api, device, "vkCmdSetScissor");
	api->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)device_proc_address(api, device, "vkCmdSetStencilCompareMask");
	api->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)device_proc_address(api, device, "vkCmdSetStencilReference");
	api->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)device_proc_address(api, device, "vkCmdSetStencilWriteMask");
	api->vkCmdSetViewport = (PFN_vkCmdSetViewport)device_proc_address(api, device, "vkCmdSetViewport");
	api->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)device_proc_address(api, device, "vkCmdUpdateBuffer");
	api->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)device_proc_address(api, device, "vkCmdWaitEvents");
	api->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)device_proc_address(api, device, "vkCmdWriteTimestamp");
	api->vkCreateBuffer = (PFN_vkCreateBuffer)device_proc_address(api, device, "vkCreateBuffer");
	api->vkCreateBufferView = (PFN_vkCreateBufferView)device_proc_address(api, device, "vkCreateBufferView");
	api->vkCreateCommandPool = (PFN_vkCreateCommandPool)device_proc_address(api, device, "vkCreateCommandPool");
	api->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)device_proc_address(api, device, "vkCreateComputePipelines");
	api->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)device_proc_address(api, device, "vkCreateDescriptorPool");
	api->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)device_proc_address(api, device, "vkCreateDescriptorSetLayout");
	api->vkCreateEvent = (PFN_vkCreateEvent)device_proc_address(api, device, "vkCreateEvent");
	api->vkCreateFence = (PFN_vkCreateFence)device_proc_address(api, device, "vkCreateFence");
	api->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)device_proc_address(api, device, "vkCreateFramebuffer");
	api->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)device_proc_address(api, device, "vkCreateGraphicsPipelines");
	api->vkCreateImage = (PFN_vkCreateImage)device_proc_address(api, device, "vkCreateImage");
	api->vkCreateImageView = (PFN_vkCreateImageView)device_proc_address(api, device, "vkCreateImageView");
	api->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)device_proc_address(api, device, "vkCreatePipelineCache");
	api->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)device_proc_address(api, device, "vkCreatePipelineLayout");
	api->vkCreateQueryPool = (PFN_vkCreateQueryPool)device_proc_address(api, device, "vkCreateQueryPool");
	api->vkCreateRenderPass = (PFN_vkCreateRenderPass)device_proc_address(api, device, "vkCreateRenderPass");
	api->vkCreateSampler = (PFN_vkCreateSampler)device_proc_address(api, device, "vkCreateSampler");
	api->vkCreateSemaphore = (PFN_vkCreateSemaphore)device_proc_address(api, device, "vkCreateSemaphore");
	api->vkCreateShaderModule = (PFN_vkCreateShaderModule)device_proc_address(api, device, "vkCreateShaderModule");
	api->vkDestroyBuffer = (PFN_vkDestroyBuffer)device_proc_address(api, device, "vkDestroyBuffer");
	api->vkDestroyBufferView = (PFN_vkDestroyBufferView)device_proc_address(api, device, "vkDestroyBufferView");
	api->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)device_proc_address(api, device, "vkDestroyCommandPool");
	api->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)device_proc_address(api, device, "vkDestroyDescriptorPool");
	api->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)device_proc_address(api, device, "vkDestroyDescriptorSetLayout");
	api->vkDestroyDevice = (PFN_vkDestroyDevice)device_proc_address(api, device, "vkDestroyDevice");
	api->vkDestroyEvent = (PFN_vkDestroyEvent)device_proc_address(api, device, "vkDestroyEvent");
	api->vkDestroyFence = (PFN_vkDestroyFence)device_proc_address(api, device, "vkDestroyFence");
	api->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)device_proc_address(api, device, "vkDestroyFramebuffer");
	api->vkDestroyImage = (PFN_vkDestroyImage)device_proc_address(api, device, "vkDestroyImage");
	api->vkDestroyImageView = (PFN_vkDestroyImageView)device_proc_address(api, device, "vkDestroyImageView");
	api->vkDestroyPipeline = (PFN_vkDestroyPipeline)device_proc_address(api, device, "vkDestroyPipeline");
	api->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)device_proc_address(api, device, "vkDestroyPipelineCache");
	api->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)device_proc_address(api, device, "vkDestroyPipelineLayout");
	api->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)device_proc_address(api, device, "vkDestroyQueryPool");
	api->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)device_proc_address(api, device, "vkDestroyRenderPass");
	api->vkDestroySampler = (PFN_vkDestroySampler)device_proc_address(api, device, "vkDestroySampler");
	api->vkDestroySemaphore = (PFN_vkDestroySemaphore)device_proc_address(api, device, "vkDestroySemaphore");
	api->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)device_proc_address(api, device, "vkDestroyShaderModule");
	api->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)device_proc_address(api, device, "vkDeviceWaitIdle");
	api->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)device_proc_address(api, device, "vkEndCommandBuffer");
	api->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)device_proc_address(api, device, "vkFlushMappedMemoryRanges");
	api->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)device_proc_address(api, device, "vkFreeCommandBuffers");
	api->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)device_proc_address(api, device, "vkFreeDescriptorSets");
	api->vkFreeMemory = (PFN_vkFreeMemory)device_proc_address(api, device, "vkFreeMemory");
	api->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)device_proc_address(api, device, "vkGetBufferMemoryRequirements");
	api->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)device_proc_address(api, device, "vkGetDeviceMemoryCommitment");
	api->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)device_proc_address(api, device, "vkGetDeviceQueue");
	api->vkGetEventStatus = (PFN_vkGetEventStatus)device_proc_address(api, device, "vkGetEventStatus");
	api->vkGetFenceStatus = (PFN_vkGetFenceStatus)device_proc_address(api, device, "vkGetFenceStatus");
	api->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)device_proc_address(api, device, "vkGetImageMemoryRequirements");
	api->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)device_proc_address(api, device, "vkGetImageSparseMemoryRequirements");
	api->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)device_proc_address(api, device, "vkGetImageSubresourceLayout");
	api->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)device_proc_address(api, device, "vkGetPipelineCacheData");
	api->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)device_proc_address(api, device, "vkGetQueryPoolResults");
	api->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)device_proc_address(api, device, "vkGetRenderAreaGranularity");
	api->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)device_proc_address(api, device, "vkInvalidateMappedMemoryRanges");
	api->vkMapMemory = (PFN_vkMapMemory)device_proc_address(api, device, "vkMapMemory");
	api->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)device_proc_address(api, device, "vkMergePipelineCaches");
	api->vkQueueBindSparse = (PFN_vkQueueBindSparse)device_proc_address(api, device, "vkQueueBindSparse");
	api->vkQueueSubmit = (PFN_vkQueueSubmit)device_proc_address(api, device, "vkQueueSubmit");
	api->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)device_proc_address(api, device, "vkQueueWaitIdle");
	api->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)device_proc_address(api, device, "vkResetCommandBuffer");
	api->vkResetCommandPool = (PFN_vkResetCommandPool)device_proc_address(api, device, "vkResetCommandPool");
	api->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)device_proc_address(api, device, "vkResetDescriptorPool");
	api->vkResetEvent = (PFN_vkResetEvent)device_proc_address(api, device, "vkResetEvent");
	api->vkResetFences = (PFN_vkResetFences)device_proc_address(api, device, "vkResetFences");
	api->vkSetEvent = (PFN_vkSetEvent)device_proc_address(api, device, "vkSetEvent");
	api->vkUnmapMemory = (PFN_vkUnmapMemory)device_proc_address(api, device, "vkUnmapMemory");
	api->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)device_proc_address(api, device, "vkUpdateDescriptorSets");
	api->vkWaitForFences = (PFN_vkWaitForFences)device_proc_address(api, device, "vkWaitForFences");

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
        return false;

    /* core 1.1 */
    api->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)device_proc_address(api, device, "vkBindBufferMemory2");
    api->vkBindImageMemory2 = (PFN_vkBindImageMemory2)device_proc_address(api, device, "vkBindImageMemory2");
    api->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)device_proc_address(api, device, "vkCmdDispatchBase");
    api->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)device_proc_address(api, device, "vkCmdSetDeviceMask");
    api->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)device_proc_address(api, device, "vkCreateDescriptorUpdateTemplate");
    api->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)device_proc_address(api, device, "vkCreateSamplerYcbcrConversion");
    api->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)device_proc_address(api, device, "vkDestroyDescriptorUpdateTemplate");
    api->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)device_proc_address(api, device, "vkDestroySamplerYcbcrConversion");
    api->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)device_proc_address(api, device, "vkGetBufferMemoryRequirements2");
    api->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)device_proc_address(api, device, "vkGetDescriptorSetLayoutSupport");
    api->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)device_proc_address(api, device, "vkGetDeviceGroupPeerMemoryFeatures");
    api->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)device_proc_address(api, device, "vkGetDeviceQueue2");
    api->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)device_proc_address(api, device, "vkGetImageMemoryRequirements2");
    api->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)device_proc_address(api, device, "vkGetImageSparseMemoryRequirements2");
    api->vkTrimCommandPool = (PFN_vkTrimCommandPool)device_proc_address(api, device, "vkTrimCommandPool");
    api->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)device_proc_address(api, device, "vkUpdateDescriptorSetWithTemplate");

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
        return false;

    /* core 1.2 */
    api->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)device_proc_address(api, device, "vkCmdDrawIndexedIndirectCount");
    api->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)device_proc_address(api, device, "vkCmdDrawIndirectCount");
    api->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)device_proc_address(api, device, "vkGetBufferDeviceAddress");
    api->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)device_proc_address(api, device, "vkGetBufferOpaqueCaptureAddress");
    api->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)device_proc_address(api, device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    api->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)device_proc_address(api, device, "vkGetSemaphoreCounterValue");
    api->vkResetQueryPool = (PFN_vkResetQueryPool)device_proc_address(api, device, "vkResetQueryPool");
    api->vkSignalSemaphore = (PFN_vkSignalSemaphore)device_proc_address(api, device, "vkSignalSemaphore");
    api->vkWaitSemaphores = (PFN_vkWaitSemaphores)device_proc_address(api, device, "vkWaitSemaphores");

    if (!api->vkCmdDrawIndexedIndirectCount ||
        !api->vkCmdDrawIndirectCount ||
        !api->vkGetBufferDeviceAddress ||
        !api->vkGetBufferOpaqueCaptureAddress ||
        !api->vkGetDeviceMemoryOpaqueCaptureAddress ||
        !api->vkGetSemaphoreCounterValue ||
        !api->vkResetQueryPool ||
        !api->vkSignalSemaphore ||
        !api->vkWaitSemaphores)
        return false;

    /* core 1.3 */
    if (api_version >= VK_MAKE_API_VERSION(0,1,3,0)) {
        api->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)device_proc_address(api, device, "vkCmdBeginRendering");
        api->vkCmdEndRendering = (PFN_vkCmdEndRendering)device_proc_address(api, device, "vkCmdEndRendering");
        api->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2)device_proc_address(api, device, "vkCmdPipelineBarrier2");
        api->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2)device_proc_address(api, device, "vkCmdResetEvent2");
        api->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2)device_proc_address(api, device, "vkCmdSetEvent2");
        api->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2)device_proc_address(api, device, "vkCmdWaitEvents2");
        api->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2)device_proc_address(api, device, "vkCmdWriteTimestamp2");
        api->vkQueueSubmit2 = (PFN_vkQueueSubmit2)device_proc_address(api, device, "vkQueueSubmit2");
        api->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements)device_proc_address(api, device, "vkGetDeviceBufferMemoryRequirements");
        api->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements)device_proc_address(api, device, "vkGetDeviceImageMemoryRequirements");
        api->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements)device_proc_address(api, device, "vkGetDeviceImageSparseMemoryRequirements");
    } else {
        api->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)device_proc_address(api, device, "vkCmdBeginRenderingKHR");
        api->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)device_proc_address(api, device, "vkCmdEndRenderingKHR");
        api->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2KHR)device_proc_address(api, device, "vkCmdPipelineBarrier2KHR");
        api->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2KHR)device_proc_address(api, device, "vkCmdResetEvent2KHR");
        api->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2KHR)device_proc_address(api, device, "vkCmdSetEvent2KHR");
        api->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2KHR)device_proc_address(api, device, "vkCmdWaitEvents2KHR");
        api->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2KHR)device_proc_address(api, device, "vkCmdWriteTimestamp2KHR");
        api->vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)device_proc_address(api, device, "vkQueueSubmit2KHR");

        /* optional */
        if (api->extensions & vk_extension_maintenance4_bit) {
            api->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceBufferMemoryRequirementsKHR");
            api->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceImageMemoryRequirementsKHR");
            api->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceImageSparseMemoryRequirementsKHR");
        }
    }
    if (!api->vkCmdBeginRendering || 
        !api->vkCmdEndRendering ||
        !api->vkCmdPipelineBarrier2 ||
        !api->vkCmdResetEvent2 ||
        !api->vkCmdSetEvent2 ||
        !api->vkCmdWaitEvents2 ||
        !api->vkCmdWriteTimestamp2 ||
        !api->vkQueueSubmit2)
        return false;

    /* core 1.4 */
    if (api_version >= VK_MAKE_API_VERSION(0,1,4,0)) {
        api->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2)device_proc_address(api, device, "vkCmdBindIndexBuffer2");
        api->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations)device_proc_address(api, device, "vkCmdSetRenderingAttachmentLocations");
        api->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices)device_proc_address(api, device, "vkCmdSetRenderingInputAttachmentIndices");
        api->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout)device_proc_address(api, device, "vkCmdBindIndexBuffer2");
        api->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2)device_proc_address(api, device, "vkGetImageSubresourceLayout2");
        api->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity)device_proc_address(api, device, "vkGetRenderingAreaGranularity");
    } else {
        api->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)device_proc_address(api, device, "vkCmdBindIndexBuffer2KHR");
        api->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)device_proc_address(api, device, "vkCmdSetRenderingAttachmentLocationsKHR");
        api->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)device_proc_address(api, device, "vkCmdSetRenderingInputAttachmentIndicesKHR");

        /* optional */
        if (api->extensions & vk_extension_maintenance5_bit) {
            api->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)device_proc_address(api, device, "vkCmdBindIndexBuffer2KHR");
            api->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)device_proc_address(api, device, "vkCmdBindIndexBuffer2KHR");
            api->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)device_proc_address(api, device, "vkGetImageSubresourceLayout2KHR");
            api->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)device_proc_address(api, device, "vkGetRenderingAreaGranularityKHR");
        }
    }
    if (!api->vkCmdBindIndexBuffer2 ||
        !api->vkCmdSetRenderingAttachmentLocations ||
        !api->vkCmdSetRenderingInputAttachmentIndices)
        return false;

    /* swapchain */
    if (api->extensions & vk_extension_swapchain_bit) {
        api->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)device_proc_address(api, device, "vkAcquireNextImageKHR");
        api->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)device_proc_address(api, device, "vkAcquireNextImage2KHR");
        api->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device_proc_address(api, device, "vkCreateSwapchainKHR");
        api->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device_proc_address(api, device, "vkDestroySwapchainKHR");
        api->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device_proc_address(api, device, "vkGetSwapchainImagesKHR");
        api->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)device_proc_address(api, device, "vkQueuePresentKHR");

        if (!api->vkAcquireNextImageKHR ||
            !api->vkAcquireNextImage2KHR ||
            !api->vkCreateSwapchainKHR ||
            !api->vkDestroySwapchainKHR ||
            !api->vkGetSwapchainImagesKHR ||
            !api->vkQueuePresentKHR)
            return false;
    }

    /* device fault */
    if (api->extensions & vk_extension_device_fault_bit) {
        api->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)device_proc_address(api, device, "vkGetDeviceFaultInfoEXT");

        if (!api->vkGetDeviceFaultInfoEXT)
            return false;
    }

    /* mesh shader */
    if (api->extensions & vk_extension_mesh_shader_bit) {
        api->vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksEXT");
        api->vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksIndirectEXT");
        api->vkCmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksIndirectCountEXT");

        if (!api->vkCmdDrawMeshTasksEXT ||
            !api->vkCmdDrawMeshTasksIndirectEXT ||
            !api->vkCmdDrawMeshTasksIndirectCountEXT)
            return false;
    }

    /* deferred host operations */
    if (api->extensions & vk_extension_deferred_host_operations_bit) {
        api->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)device_proc_address(api, device, "vkCreateDeferredOperationKHR");
        api->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)device_proc_address(api, device, "vkDeferredOperationJoinKHR");
        api->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)device_proc_address(api, device, "vkDestroyDeferredOperationKHR");
        api->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)device_proc_address(api, device, "vkGetDeferredOperationMaxConcurrencyKHR");
        api->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)device_proc_address(api, device, "vkGetDeferredOperationResultKHR");

        if (!api->vkCreateDeferredOperationKHR ||
            !api->vkDeferredOperationJoinKHR ||
            !api->vkDestroyDeferredOperationKHR ||
            !api->vkGetDeferredOperationMaxConcurrencyKHR ||
            !api->vkGetDeferredOperationResultKHR)
            return false;
    }

    /* acceleration structure */
    if (api->extensions & vk_extension_acceleration_structure_bit) {
        api->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)device_proc_address(api, device, "vkBuildAccelerationStructuresKHR");
        api->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)device_proc_address(api, device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        api->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)device_proc_address(api, device, "vkCmdBuildAccelerationStructuresKHR");
        api->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)device_proc_address(api, device, "vkCmdCopyAccelerationStructureKHR");
        api->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)device_proc_address(api, device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        api->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)device_proc_address(api, device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        api->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)device_proc_address(api, device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        api->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)device_proc_address(api, device, "vkCopyAccelerationStructureKHR");
        api->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)device_proc_address(api, device, "vkCopyAccelerationStructureToMemoryKHR");
        api->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)device_proc_address(api, device, "vkCopyMemoryToAccelerationStructureKHR");
        api->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)device_proc_address(api, device, "vkCreateAccelerationStructureKHR");
        api->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)device_proc_address(api, device, "vkDestroyAccelerationStructureKHR");
        api->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)device_proc_address(api, device, "vkGetAccelerationStructureBuildSizesKHR");
        api->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)device_proc_address(api, device, "vkGetAccelerationStructureDeviceAddressKHR");
        api->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)device_proc_address(api, device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        api->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)device_proc_address(api, device, "vkWriteAccelerationStructuresPropertiesKHR");
    
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
            return false;
    }

    /* ray tracing pipeline */
    if (api->extensions & vk_extension_ray_tracing_pipeline_bit) {
        api->vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)device_proc_address(api, device, "vkCmdSetRayTracingPipelineStackSizeKHR");
        api->vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)device_proc_address(api, device, "vkCmdTraceRaysIndirectKHR");
        api->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)device_proc_address(api, device, "vkCmdTraceRaysIndirect2KHR");
        api->vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)device_proc_address(api, device, "vkCmdTraceRaysKHR");
        api->vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)device_proc_address(api, device, "vkCreateRayTracingPipelinesKHR");
        api->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)device_proc_address(api, device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
        api->vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)device_proc_address(api, device, "vkGetRayTracingShaderGroupHandlesKHR");
        api->vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)device_proc_address(api, device, "vkGetRayTracingShaderGroupStackSizeKHR");

        if (!api->vkCmdSetRayTracingPipelineStackSizeKHR ||
            !api->vkCmdTraceRaysIndirectKHR ||
            !api->vkCmdTraceRaysKHR ||
            !api->vkCreateRayTracingPipelinesKHR ||
            !api->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR ||
            !api->vkGetRayTracingShaderGroupHandlesKHR ||
            !api->vkGetRayTracingShaderGroupStackSizeKHR)
            return false;
    }
    return true;
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
