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

    /* core instance */
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

    /* surface */
    api->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instance_proc_address(api, instance, "vkDestroySurfaceKHR");
    api->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    api->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    api->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    api->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

#if defined(VK_KHR_win32_surface)
    api->vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)instance_proc_address(api, instance, "vkCreateWin32SurfaceKHR");
    api->vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif /* VK_KHR_win32_surface */

#if defined(VK_EXT_metal_surface)
    api->vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)instance_proc_address(api, instance, "vkCreateMetalSurfaceEXT");
#endif /* VK_EXT_metal_surface */

#if defined(VK_KHR_wayland_surface)
    api->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)instance_proc_address(api, instance, "vkCreateWaylandSurfaceKHR");
    api->vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif /* VK_KHR_wayland_surface */

#if defined(VK_KHR_xcb_surface)
    api->vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)instance_proc_address(api, instance, "vkCreateXcbSurfaceKHR");
    api->vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif /* VK_KHR_xcb_surface */

#if defined(VK_KHR_android_surface)
    api->vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)instance_proc_address(api, instance, "vkCreateAndroidSurfaceKHR");
#endif /* VK_KHR_android_surface */

#if defined(VK_EXT_headless_surface)
    api->vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT)instance_proc_address(api, instance, "vkCreateHeadlessSurfaceEXT");
#endif /* VK_EXT_headless_surface */

#if defined(VK_KHR_display)
    api->vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR)instance_proc_address(api, instance, "vkCreateDisplayModeKHR");
    api->vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)instance_proc_address(api, instance, "vkCreateDisplayPlaneSurfaceKHR");
    api->vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR)instance_proc_address(api, instance, "vkGetDisplayModePropertiesKHR");
    api->vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR)instance_proc_address(api, instance, "vkGetDisplayPlaneCapabilitiesKHR");
    api->vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)instance_proc_address(api, instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
    api->vkGetPhysicalDeviceDisplayPlanePropertiesKHR= (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    api->vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");
#endif /* VK_KHR_display */

    /* debug utils */
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

    /* video coding */
    api->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    api->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)instance_proc_address(api, instance, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");

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

    /* core 1.3 */
    if (api_version >= VK_API_VERSION_1_3) {
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

        /* maintenance */
        api->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceBufferMemoryRequirementsKHR");
        api->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceImageMemoryRequirementsKHR");
        api->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)device_proc_address(api, device, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    }

    /* core 1.4 */
    if (api_version >= VK_API_VERSION_1_4) {
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

        /* maintenance 5 */
        api->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)device_proc_address(api, device, "vkCmdBindIndexBuffer2KHR");
        api->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)device_proc_address(api, device, "vkCmdBindIndexBuffer2KHR");
        api->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)device_proc_address(api, device, "vkGetImageSubresourceLayout2KHR");
        api->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)device_proc_address(api, device, "vkGetRenderingAreaGranularityKHR");
    }

    /* swapchain */
    api->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)device_proc_address(api, device, "vkAcquireNextImageKHR");
    api->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)device_proc_address(api, device, "vkAcquireNextImage2KHR");
    api->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device_proc_address(api, device, "vkCreateSwapchainKHR");
    api->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device_proc_address(api, device, "vkDestroySwapchainKHR");
    api->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device_proc_address(api, device, "vkGetSwapchainImagesKHR");
    api->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)device_proc_address(api, device, "vkQueuePresentKHR");

    /* device fault */
    if (api->extensions & vk_extension_device_fault_bit) {
        api->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)device_proc_address(api, device, "vkGetDeviceFaultInfoEXT");

        if (!api->vkGetDeviceFaultInfoEXT)
            return false;
    }

    /* mesh shader */
    api->vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksEXT");
    api->vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksIndirectEXT");
    api->vkCmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)device_proc_address(api, device, "vkCmdDrawMeshTasksIndirectCountEXT");

    /* deferred host operations */
    api->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)device_proc_address(api, device, "vkCreateDeferredOperationKHR");
    api->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)device_proc_address(api, device, "vkDeferredOperationJoinKHR");
    api->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)device_proc_address(api, device, "vkDestroyDeferredOperationKHR");
    api->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)device_proc_address(api, device, "vkGetDeferredOperationMaxConcurrencyKHR");
    api->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)device_proc_address(api, device, "vkGetDeferredOperationResultKHR");

    /* acceleration structure */
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
    
    /* ray tracing pipeline */
    api->vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)device_proc_address(api, device, "vkCmdSetRayTracingPipelineStackSizeKHR");
    api->vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)device_proc_address(api, device, "vkCmdTraceRaysIndirectKHR");
    api->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)device_proc_address(api, device, "vkCmdTraceRaysIndirect2KHR");
    api->vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)device_proc_address(api, device, "vkCmdTraceRaysKHR");
    api->vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)device_proc_address(api, device, "vkCreateRayTracingPipelinesKHR");
    api->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)device_proc_address(api, device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    api->vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)device_proc_address(api, device, "vkGetRayTracingShaderGroupHandlesKHR");
    api->vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)device_proc_address(api, device, "vkGetRayTracingShaderGroupStackSizeKHR");

    /* video coding */
    api->vkBindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR)device_proc_address(api, device, "vkBindVideoSessionMemoryKHR");
    api->vkCmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR)device_proc_address(api, device, "vkCmdBeginVideoCodingKHR");
    api->vkCmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR)device_proc_address(api, device, "vkCmdControlVideoCodingKHR");
    api->vkCmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR)device_proc_address(api, device, "vkCmdDecodeVideoKHR");
    api->vkCmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR)device_proc_address(api, device, "vkCmdEndVideoCodingKHR");
    api->vkCreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR)device_proc_address(api, device, "vkCreateVideoSessionKHR");
    api->vkCreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR)device_proc_address(api, device, "vkCreateVideoSessionParametersKHR");
    api->vkDestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR)device_proc_address(api, device, "vkDestroyVideoSessionKHR");
    api->vkDestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR)device_proc_address(api, device, "vkDestroyVideoSessionParametersKHR");
    api->vkGetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR)device_proc_address(api, device, "vkGetVideoSessionMemoryRequirementsKHR");
    api->vkUpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR)device_proc_address(api, device, "vkUpdateVideoSessionParametersKHR");

    return true;
}

