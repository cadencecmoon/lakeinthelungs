#include <amw/bits.h>
#include <amw/hadal.h>
#include <amw/harridan.h>
#include <amw/log.h>
#include <amw/plugin.h>
#include <amw/riven.h>
#include <amw/random.h>

#include <stdio.h>  /* snprintf */
#include <string.h> /* strcmp */

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
            riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
        VERIFY_VK(harridan->api.vkEnumerateInstanceExtensionProperties(NULL, &extension_count, properties));

        u32 i = 0;
        for (; i < extension_count; i++) {
            if (!strcmp(properties[i].extensionName, ext_surface)) {
                extension_bits |= vk_extension_surface_bit;
#if defined(PLATFORM_WINDOWS)
            } else if (!strcmp(properties[i].extensionName, ext_win32_surface)) {
                extension_bits |= vk_extension_win32_surface_bit;
#elif defined(PLATFORM_APPLE)
            } else if (!strcmp(properties[i].extensionName, ext_metal_surface)) {
                extension_bits |= vk_extension_metal_surface_bit;
#elif defined(PLATFORM_ANDROID)
            } else if (!strcmp(properties[i].extensionName, ext_android_surface)) {
                extension_bits |= vk_extension_android_surface_bit;
#endif
#ifdef HADAL_WAYLAND
            } else if (!strcmp(properties[i].extensionName, ext_wayland_surface)) {
                extension_bits |= vk_extension_wayland_surface_bit;
#endif
#ifdef HADAL_XCB
            } else if (!strcmp(properties[i].extensionName, ext_xcb_surface)) {
                extension_bits |= vk_extension_xcb_surface_bit;
#endif
#ifdef HADAL_DRMKMS
            } else if (!strcmp(properties[i].extensionName, ext_display)) {
                extension_bits |= vk_extension_display_bit;
#endif
            } else if (!strcmp(properties[i].extensionName, ext_headless_surface)) {
                extension_bits |= vk_extension_headless_surface_bit;
            } else if (debug_utilities && !strcmp(properties[i].extensionName, ext_debug_utils)) {
                extension_bits |= vk_extension_debug_utils_bit;
            }
        }
        i = 0; extension_count = bits_popcnt_lookup((const u8 *)&extension_bits, sizeof(extension_bits));
        
        extensions = (const char **)riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(const char *) * extension_count, _Alignof(const char *));
        if (extension_bits & vk_extension_surface_bit)
            extensions[i++] = ext_surface;
#if defined(PLATFORM_WINDOWS)
        if (extension_bits & vk_extension_win32_surface_bit)
            extensions[i++] = ext_win32_surface;
#elif defined(PLATFORM_APPLE)
        if (extension_bits & vk_extension_metal_surface_bit)
            extensions[i++] = ext_metal_surface;
#elif defined(PLATFORM_ANDROID)
        if (extension_bits & vk_extension_android_surface_bit)
            extensions[i++] = ext_android_surface;
#endif
#ifdef HADAL_WAYLAND
        if (extension_bits & vk_extension_wayland_surface_bit)
            extensions[i++] = ext_wayland_surface;
#endif
#ifdef HADAL_XCB
        if (extension_bits & vk_extension_xcb_surface_bit)
            extensions[i++] = ext_xcb_surface;
#endif
#ifdef HADAL_DRMKMS
        if (extension_bits & vk_extension_display_bit)
            extensions[i++] = ext_display;
#endif
        if (extension_bits & vk_extension_headless_surface_bit)
            extensions[i++] = ext_headless_surface;
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

static b32 prepare_surface(
    struct harridan          *harridan,
    VkPhysicalDevice          physical,
    struct swapchain         *swapchain,
    VkSurfaceCapabilitiesKHR *surface_capabilities,
    VkFormat                 *image_format,
    VkPresentModeKHR         *present_mode)
{
    VkSurfaceFormatKHR *surface_formats;
    VkPresentModeKHR *present_modes;
    u32 surface_format_count = 0;
    u32 present_mode_count = 0;

    /* get surface capabilities */
    VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, swapchain->surface, surface_capabilities));

    /* get surface formats */
    VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceFormatsKHR(physical, swapchain->surface, &surface_format_count, NULL));
    surface_formats = (VkSurfaceFormatKHR *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkSurfaceFormatKHR) * surface_format_count, _Alignof(VkSurfaceFormatKHR));
    VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceFormatsKHR(physical, swapchain->surface, &surface_format_count, surface_formats));

    *image_format = VK_FORMAT_UNDEFINED;
    if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
        *image_format = VK_FORMAT_B8G8R8A8_UNORM;
    for (u32 i = 0; i < surface_format_count; i++) {
        if (surface_formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            continue;
        VkFormat format = surface_formats[i].format;
        if (format == VK_FORMAT_R8G8B8A8_UNORM ||
            format == VK_FORMAT_R8G8B8A8_SRGB ||
            format == VK_FORMAT_B8G8R8A8_UNORM ||
            format == VK_FORMAT_B8G8R8A8_SRGB ||
            format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 ||
            format == VK_FORMAT_A2R10G10B10_UNORM_PACK32)
        {
            *image_format = format;
        }
    }
    if (*image_format == VK_FORMAT_UNDEFINED) {
        /* for now only GPUs with presentation support interest us */
        log_error("Can't determine an appropriate surface format. Only R8G8B8A8, B8G8R8A8, A2R10G10B10 or A2B10G10R10 format will be accepted.");
        return false;
    }

    /* get present modes */
    VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfacePresentModesKHR(physical, swapchain->surface, &present_mode_count, NULL));
    present_modes = (VkPresentModeKHR *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkPresentModeKHR) * present_mode_count, _Alignof(VkPresentModeKHR));
    VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfacePresentModesKHR(physical, swapchain->surface, &present_mode_count, present_modes));

    VkPresentModeKHR no_vsync_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && no_vsync_mode == VK_PRESENT_MODE_FIFO_KHR)
            no_vsync_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            no_vsync_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    if (no_vsync_mode == VK_PRESENT_MODE_FIFO_KHR) {
        log_warn("Surface has no presentation mode without vertical synchronization, V-sync is forced.");
        harridan->flags |= harridan_flag_vsync_enabled;
    }
    *present_mode = no_vsync_mode;
    return true;
}

struct physical_device_info {
    /** For Vulkan API access. */
    const struct harridan                              *harridan;
    b32                                                 accepted;
    b32                                                 verbose;
    u32                                                 index;
    u64                                                 score;
    VkBool32                                            has_vk13;
    VkBool32                                            has_vk14;

    /** The physical device owning this structure. */
    u32                                                 flag_bits;
    u32                                                 feat_bits;
    u64                                                 extension_bits;
    VkPhysicalDevice                                    device;
    s32                                                 graphics_queue_family;
    s32                                                 compute_queue_family;
    s32                                                 transfer_queue_family;
    s32                                                 sparse_queue_family;
    s32                                                 decode_queue_family;

    u32                                                *queue_family_indices;
    u32                                                 queue_family_count;
    u32                                                 graphics_family_create_queue_count;
    u32                                                 compute_family_create_queue_count;

    /** Information for the swapchain. */
    VkBool32                                            presentation_support;
    VkFormat                                            swapchain_image_format;
    VkSurfaceCapabilitiesKHR                            surface_capabilities;
    VkSurfaceFormatKHR                                  surface_format;
    VkPresentModeKHR                                    present_mode;

    /** Information about hardware properties of the physical device. */
    VkPhysicalDeviceProperties2                         properties2;
    VkPhysicalDeviceVulkan11Properties                  properties_11;
    VkPhysicalDeviceVulkan12Properties                  properties_12;
    VkPhysicalDeviceVulkan13Properties                  properties_13;
    VkPhysicalDeviceVulkan14Properties                  properties_14;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR  acceleration_structure_properties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR     raytracing_pipeline_properties;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR    fragment_shading_rate_properties;
    VkPhysicalDeviceMeshShaderPropertiesEXT             mesh_shader_properties;
    VkPhysicalDeviceMemoryProperties2                   memory_properties2;
    VkPhysicalDeviceMemoryBudgetPropertiesEXT           memory_budget;

    /** Information about features supported by the physical device. */
    VkPhysicalDeviceFeatures2                           features2;
    VkPhysicalDeviceVulkan11Features                    features_11;
    VkPhysicalDeviceVulkan12Features                    features_12;
    VkPhysicalDeviceVulkan13Features                    features_13;
    VkPhysicalDeviceVulkan14Features                    features_14;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR    acceleration_structure_features;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR       raytracing_pipeline_features;
    VkPhysicalDeviceRayQueryFeaturesKHR                 raytracing_query_features;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR      fragment_shading_rate_features;
    VkPhysicalDeviceMeshShaderFeaturesEXT               mesh_shader_features;

    VkPhysicalDeviceDynamicRenderingFeatures            dynamic_rendering_features;
    VkPhysicalDeviceDynamicRenderingLocalReadFeatures   dynamic_rendering_local_read_features;
    VkPhysicalDeviceSynchronization2Features            synchronization2_features;

    /** Information about capabilities of accelerated video coding. */
    VkVideoDecodeH264ProfileInfoKHR                     decode_h264_profile;
    VkVideoDecodeH264CapabilitiesKHR                    decode_h264_capabilities;
    VkVideoProfileInfoKHR                               video_profile;
    VkVideoDecodeCapabilitiesKHR                        video_decode_capabilities;
    VkVideoCapabilitiesKHR                              video_capabilities;

    /* device extensions */
    const char *ext_swapchain;
    const char *ext_device_fault;
    const char *ext_memory_budget;
    const char *ext_memory_priority;
    const char *ext_mesh_shader;
    const char *ext_fragment_shading_rate;
    const char *ext_deferred_host_operations;
    const char *ext_acceleration_structure;
    const char *ext_pipeline_library;
    const char *ext_ray_query;
    const char *ext_raytracing_pipeline;
    const char *ext_raytracing_maintenance1;
    const char *ext_video_queue;
    const char *ext_video_decode_queue;
    const char *ext_amd_device_coherent_memory;

    /* backwards compatibility extensions */
    const char *ext_dynamic_rendering_local_read;
    const char *ext_dynamic_rendering;
    const char *ext_synchronization2;
    const char *ext_maintenance5;
    const char *ext_maintenance4;
};

/** All physical devices will be handled concurrently. */
static void populate_physical_device_information(struct physical_device_info *work)
{
    const struct harridan *harridan = work->harridan;
    VkPhysicalDevice device = work->device;

    VkVideoFormatPropertiesKHR *video_properties;
    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties   *extension_properties;

    u32 api_version = 0;
    u32 extension_count = 0;
    u32 video_property_count = 0;
    u32 queue_family_count = 0;

    work->accepted = false;

    assert_debug(device != VK_NULL_HANDLE);

    /* resolve driver versions */
    harridan->api.vkGetPhysicalDeviceProperties(device, &work->properties2.properties);
    api_version = work->properties2.properties.apiVersion;
    if (api_version < VK_API_VERSION_1_2) {
        if (work->verbose)
            log_warn("Physical device (%u) has too old drivers - found api version %u.%u.%u, we target atleast core 1.2.X.",
                     work->index, (api_version >> 22u), (api_version >> 12u) & 0x3ffu, api_version & 0xfffu);
        return;
    }
    work->has_vk13 = api_version >= VK_API_VERSION_1_3;
    work->has_vk14 = api_version >= VK_API_VERSION_1_4;
    work->score += (api_version & 0xfffu) * 10;

    if (work->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        work->feat_bits |= harridan_feat_is_discrete;
        work->score += 300;
    }

    work->mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
    work->fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    work->raytracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    work->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    work->properties_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES;
    work->properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    work->properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    work->properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    work->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    work->fragment_shading_rate_properties.pNext = &work->mesh_shader_properties;
    work->acceleration_structure_properties.pNext = &work->raytracing_pipeline_properties;
    work->properties_11.pNext = &work->acceleration_structure_properties;
    work->properties_12.pNext = &work->properties_11;
    work->properties_13.pNext = &work->properties_12;
    work->properties_14.pNext = &work->properties_13;
    work->properties2.pNext = work->has_vk14 ? &work->properties_14 
        : (work->has_vk13 ? (void *)&work->properties_13 : (void *)&work->properties_12);
    harridan->api.vkGetPhysicalDeviceProperties2(device, &work->properties2);

    work->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
    work->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    work->memory_properties2.pNext = &work->memory_budget;
    harridan->api.vkGetPhysicalDeviceMemoryProperties2(device, &work->memory_properties2);

    work->dynamic_rendering_local_read_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
    work->dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    work->synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    work->mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
    work->fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
    work->raytracing_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    work->raytracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    work->acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    work->features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    work->features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    work->features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    work->features_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    work->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    work->dynamic_rendering_features.pNext = &work->dynamic_rendering_local_read_features;
    work->synchronization2_features.pNext = &work->dynamic_rendering_features;
    work->mesh_shader_features.pNext = &work->synchronization2_features;
    work->fragment_shading_rate_features.pNext = &work->mesh_shader_features;
    work->raytracing_query_features.pNext = &work->fragment_shading_rate_features;
    work->raytracing_pipeline_features.pNext = &work->raytracing_query_features;
    work->acceleration_structure_features.pNext = &work->raytracing_pipeline_features;
    work->features_11.pNext = &work->acceleration_structure_features;
    work->features_12.pNext = &work->features_11;
    work->features_13.pNext = &work->features_12;
    work->features_14.pNext = &work->features_13;
    work->features2.pNext = work->has_vk14 ? &work->features_14 
        : (work->has_vk13 ? (void *)&work->features_13 : (void *)&work->features_12);
    harridan->api.vkGetPhysicalDeviceFeatures2(device, &work->features2);

    /* resolve queue families */
    harridan->api.vkGetPhysicalDeviceQueueFamilyProperties2(device, &queue_family_count, NULL);
    if (queue_family_count == 0) {
        if (work->verbose)
            log_warn("Physical device (%u) has no command queue families.", work->index);
        return;
    }
    work->queue_family_count = queue_family_count;
    queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count, _Alignof(VkQueueFamilyVideoPropertiesKHR));
    queue_family_properties2 = (VkQueueFamilyProperties2 *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkQueueFamilyProperties2) * queue_family_count, _Alignof(VkQueueFamilyProperties2));
    for (u32 i = 0; i < queue_family_count; i++) {
        queue_family_video_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        queue_family_video_properties[i].pNext = NULL; 
        queue_family_properties2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queue_family_properties2[i].pNext = &queue_family_video_properties[i];
    } 
    harridan->api.vkGetPhysicalDeviceQueueFamilyProperties2(device, &queue_family_count, queue_family_properties2);

    work->graphics_queue_family = -1;
    work->compute_queue_family  = -1;
    work->transfer_queue_family = -1;
    work->sparse_queue_family   = -1;
    work->decode_queue_family   = -1;

    u32 graphics_family_queue_count;
    VkQueueFlags graphics_family_flags;
    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_family_properties2[i].queueFamilyProperties.queueCount <= 0) continue;

        VkQueueFlags flags = queue_family_properties2[i].queueFamilyProperties.queueFlags;
        if (work->graphics_queue_family < 0 && (flags & VK_QUEUE_GRAPHICS_BIT)) {
            /* don't be picky about the graphics queue */
            work->graphics_queue_family = i;
            graphics_family_flags = flags;
            graphics_family_queue_count = queue_family_properties2[i].queueFamilyProperties.queueCount;
        } else if (work->compute_queue_family < 0 && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == VK_QUEUE_COMPUTE_BIT) {
            /* ask for an async compute queue family */
            work->feat_bits |= harridan_feat_async_compute;
            work->compute_queue_family = i;
        } else if (work->transfer_queue_family < 0 && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) == VK_QUEUE_TRANSFER_BIT) {
            /* ask for an async transfer queue family */
            work->feat_bits |= harridan_feat_async_transfer;
            work->transfer_queue_family = i;
        } else if (work->sparse_queue_family < 0 && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_SPARSE_BINDING_BIT)) == VK_QUEUE_SPARSE_BINDING_BIT) {
            /* ask for an async sparse binding queue family */
            work->feat_bits |= harridan_feat_async_sparse_binding;
            work->sparse_queue_family = i;
        } else if (work->decode_queue_family < 0 && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR)) == VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
            /* ask for an async video decode queue family */
            work->feat_bits |= harridan_feat_async_decode_video;
            work->decode_queue_family = i;
        }
    }

    if (work->graphics_queue_family < 0) {
        if (work->verbose)
            log_warn("Physical device (%u) has no graphics commands support.", work->index);
        return;
    }

    work->graphics_family_create_queue_count = 1;
    work->compute_family_create_queue_count = 1;
    work->queue_family_count = 5;
    work->queue_family_indices = (u32 *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(u32) * work->queue_family_count, _Alignof(u32));
    work->queue_family_indices[0] = work->graphics_queue_family;

    if (work->compute_queue_family < 0) {
        assert_debug(graphics_family_flags & VK_QUEUE_COMPUTE_BIT)
        work->compute_queue_family = work->graphics_queue_family;
        work->queue_family_count--;

        if (graphics_family_queue_count > work->graphics_family_create_queue_count) {
            work->feat_bits |= harridan_feat_async_compute;
            work->graphics_family_create_queue_count++;
        }
        work->compute_family_create_queue_count = 0;
    }
    if (work->transfer_queue_family < 0) {
        assert_debug(graphics_family_flags & VK_QUEUE_TRANSFER_BIT)
        work->transfer_queue_family = work->graphics_queue_family;
        work->queue_family_count--;

        if (graphics_family_queue_count > work->graphics_family_create_queue_count) {
            work->feat_bits |= harridan_feat_async_transfer;
            work->graphics_family_create_queue_count++;
        } else if (work->compute_queue_family != work->graphics_queue_family 
            && queue_family_properties2[work->compute_queue_family].queueFamilyProperties.queueCount > work->compute_family_create_queue_count) 
        {   /* we may have a seperate compute queue family we could use */
            work->feat_bits |= harridan_feat_async_transfer;
            work->transfer_queue_family = work->compute_queue_family;
            work->compute_family_create_queue_count++;
        }
    }
    if (work->sparse_queue_family < 0) {
        work->queue_family_count--;
        /* sparse binding is optional */
        if (graphics_family_flags & VK_QUEUE_SPARSE_BINDING_BIT) {
            work->sparse_queue_family = work->graphics_queue_family;
            if (graphics_family_queue_count > work->graphics_family_create_queue_count) {
                work->feat_bits |= harridan_feat_async_sparse_binding;
                work->graphics_family_create_queue_count++;
            }
        }
    }
    if (work->decode_queue_family < 0) {
        work->queue_family_count--;
        /* video decoding is optional */
        if (graphics_family_flags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
            work->decode_queue_family = work->graphics_queue_family;
            if (graphics_family_queue_count > work->graphics_family_create_queue_count) {
                work->feat_bits |= harridan_feat_async_decode_video;
                work->graphics_family_create_queue_count++;
            }
        }
    }

    u32 o = 1;
    if (work->compute_queue_family != work->graphics_queue_family)
        work->queue_family_indices[o++] = work->compute_queue_family;
    if (work->transfer_queue_family != work->graphics_queue_family && work->transfer_queue_family != work->compute_queue_family)
        work->queue_family_indices[o++] = work->transfer_queue_family;
    if (work->sparse_queue_family >= 0 && work->sparse_queue_family != work->graphics_queue_family)
        work->queue_family_indices[o++] = work->sparse_queue_family;
    if (work->decode_queue_family >= 0 && work->decode_queue_family != work->graphics_queue_family)
        work->queue_family_indices[o++] = work->decode_queue_family;

    /* resolve device extensions */
    harridan->api.vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    if (extension_count == 0) {
        if (work->verbose) 
            log_warn("Physical device (%u) has zero Vulkan extensions available.", work->index);
        return;
    }
    extension_properties = (VkExtensionProperties *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
    harridan->api.vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extension_properties);

    for (u32 i = 0; i < extension_count; i++) {
        if (!strcmp(extension_properties[i].extensionName, work->ext_swapchain)) {
            work->extension_bits |= vk_extension_swapchain_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_device_fault)) {
            work->extension_bits |= vk_extension_device_fault_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_memory_budget)) {
            work->extension_bits |= vk_extension_memory_budget_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_memory_priority)) {
            work->extension_bits |= vk_extension_memory_priority_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_mesh_shader)) {
            work->extension_bits |= vk_extension_mesh_shader_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_fragment_shading_rate)) {
            work->extension_bits |= vk_extension_fragment_shading_rate_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_deferred_host_operations)) {
            work->extension_bits |= vk_extension_deferred_host_operations_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_acceleration_structure)) {
            work->extension_bits |= vk_extension_acceleration_structure_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_pipeline_library)) {
            work->extension_bits |= vk_extension_pipeline_library_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_ray_query)) {
            work->extension_bits |= vk_extension_ray_query_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_raytracing_pipeline)) {
            work->extension_bits |= vk_extension_ray_tracing_pipeline_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_raytracing_maintenance1)) {
            work->extension_bits |= vk_extension_ray_tracing_maintenance1_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_video_queue)) {
            work->extension_bits |= vk_extension_video_queue_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_video_decode_queue)) {
            work->extension_bits |= vk_extension_video_decode_queue_bit;
        } else if (!strcmp(extension_properties[i].extensionName, work->ext_amd_device_coherent_memory)) {
            work->extension_bits |= vk_extension_amd_device_coherent_memory_bit;
        /* core 1.4 backwards compatibility */
        } else if (!work->has_vk14) {
            if (!strcmp(extension_properties[i].extensionName, work->ext_dynamic_rendering_local_read)) {
                work->extension_bits |= vk_extension_dynamic_rendering_local_read_bit;
            } else if (!strcmp(extension_properties[i].extensionName, work->ext_maintenance5)) {
                work->extension_bits |= vk_extension_maintenance5_bit;
            }
        /* core 1.3 backwards compatibility */
        } else if (!work->has_vk13) {
            if (!strcmp(extension_properties[i].extensionName, work->ext_dynamic_rendering)) {
                work->extension_bits |= vk_extension_dynamic_rendering_bit;
            } else if (!strcmp(extension_properties[i].extensionName, work->ext_synchronization2)) {
                work->extension_bits |= vk_extension_synchronization2_bit;
            } else if (!strcmp(extension_properties[i].extensionName, work->ext_maintenance4)) {
                work->extension_bits |= vk_extension_maintenance4_bit;
            }
        }
    }
    if (!(work->extension_bits & vk_extension_swapchain_bit)) {
        /* only GPUs with swapchain support interest us for now */
        if (work->verbose)
            log_warn("Physical device (%u) has no swapchain support.", work->index);
        return;
    }
    if (!work->has_vk14 && (work->extension_bits & vk_extension_mask_api14) != vk_extension_mask_api14) {
        if (work->verbose)
            log_warn("Physical device (%u) does not support required 1.4 core extensions (backwards compatibility):"
                     "\n\tdynamic_rendering_local_read : %b", work->index, work->dynamic_rendering_local_read_features.dynamicRenderingLocalRead);
        return;
    }
    if (!work->has_vk13 && (work->extension_bits & vk_extension_mask_api13) != vk_extension_mask_api13) {
        if (work->verbose)
            log_warn("Physical device (%u) does not support required 1.3 core extensions (backwards compatibility):"
                     "\n\tdynamic_rendering : %b\n\tsynchronization2 : %b", work->index, 
                     work->dynamic_rendering_features.dynamicRendering, work->synchronization2_features.synchronization2);
        return;
    }

    /* resolve presentation support */
    harridan->api.vkGetPhysicalDeviceSurfaceSupportKHR(device, work->graphics_queue_family, harridan->swapchain.surface, &work->presentation_support);
    if (work->presentation_support) {
        if (!prepare_surface(
            (struct harridan *)harridan, device, 
            (struct swapchain *)&harridan->swapchain, 
            &work->surface_capabilities, 
            &work->swapchain_image_format, 
            &work->present_mode)) 
        {
            if (work->verbose)
                log_warn("Physical devicxe (%u) can't be used with the given display surface.", work->index);
            return;
        }
        work->feat_bits |= harridan_feat_presentation_support;
    } else {
        /* for now only GPUs with presentation support interest us */
        if (work->verbose)
            log_warn("Physical device (%u) has no support for presenting to surface.", work->index);
        return;
    }

    /* resolve video coding */
    if (work->extension_bits & (vk_extension_video_queue_bit | vk_extension_video_decode_queue_bit) && work->decode_queue_family >= 0) {
        /* TODO */
        (void)video_properties;
        (void)video_property_count;
#if 0
        work->decode_h264_profile.sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR;
        work->video_profile.sType = VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
        work->video_profile.pNext = &work->decode_h264_profile;
        work->video_profile.videoCodecOperation = queue_family_video_properties[work->decode_queue_family].videoCodecOperations & VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR;
        work->video_profile.chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR;
        work->video_profile.chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;
        work->video_profile.lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;

        work->decode_h264_capabilities.sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR;
        work->video_decode_capabilities.sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR;
        work->video_decode_capabilities.pNext = &work->decode_h264_capabilities;
        work->video_capabilities.sType = VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR;
        work->video_capabilities.pNext = &work->video_decode_capabilities;
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceVideoCapabilitiesKHR(device, &work->video_profile, &work->video_capabilities));

        VkPhysicalDeviceVideoFormatInfoKHR video_format = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR,
            .pNext = NULL,
            .imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
        };

        VERIFY_VK(harridan->api.vkGetPhysicalDeviceVideoFormatPropertiesKHR(device, &video_format, &video_property_count, NULL));
        if (video_property_count) {
            video_properties = (VkVideoFormatPropertiesKHR *)
                riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkVideoFormatPropertiesKHR) * video_property_count, _Alignof(VkVideoFormatPropertiesKHR));
            VERIFY_VK(harridan->api.vkGetPhysicalDeviceVideoFormatPropertiesKHR(device, &video_format, &video_property_count, video_properties));

            work->feat_bits |= harridan_feat_h264_video_coding;
            work->score += 200;
        }
#endif
    }

    /* check VRAM */
    for (u32 i = 0; i < work->memory_properties2.memoryProperties.memoryHeapCount; i++)
        work->score += (25 * (work->memory_properties2.memoryProperties.memoryHeaps[i].size >> 28)); /* 256 MB -> 1 */

    /* TODO resolve optional features */

    /* we accept this physical device */
    work->accepted = true;

    if (!work->verbose)
        return;

    usize bufsize = 1024;
    usize offset = 0;
    char *buffer = (char *)riven_alloc(harridan->riven, rivens_tag_drifter, bufsize, 1);

    offset += snprintf(buffer + offset, bufsize - offset,
            "Physical device (%u) information:\n"
            "    Name           : %s\n"
            "    Type           : %s, ID: %X\n"
            "    Vendor         : %s, ID: %X\n"
            "    Driver Version : %u.%u.%u\n"
            "    API Version    : %u.%u.%u\n"
            "Memory heaps:\n", work->index,
            work->properties2.properties.deviceName,
            device_type_string(work->properties2.properties.deviceType),
            work->properties2.properties.deviceID,
            vendor_name_string(work->properties2.properties.vendorID),
            work->properties2.properties.vendorID,
            (work->properties2.properties.driverVersion >> 22u),
            (work->properties2.properties.driverVersion >> 12u) & 0x3ffu,
            work->properties2.properties.driverVersion & 0xfffu,
            (work->properties2.properties.apiVersion >> 22u),
            (work->properties2.properties.apiVersion >> 12u) & 0x3ffu,
            work->properties2.properties.apiVersion & 0xfffu);
    
    for (u32 i = 0; i < work->memory_properties2.memoryProperties.memoryHeapCount; i++) {
        usize size = work->memory_properties2.memoryProperties.memoryHeaps[i].size;
        usize flags = work->memory_properties2.memoryProperties.memoryHeaps[i].flags;

        offset += snprintf(buffer + offset, bufsize - offset, 
            "    Heap %u : %lu MB (%s)\n", i, (size >> 20), 
            (flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "device local" : "host visible");
    }

    buffer[min(offset, bufsize-1)] = '\0';
    log_info("%s", buffer);
}

static s32 create_rendering_device(
    struct harridan *harridan,
    s32              select_device_index,
    b32              verbose)
{
    u32 physical_device_count = 0;
    u32 physical_device_index = UINT32_MAX;
    VkPhysicalDevice *physical_devices;
    u64 best_score = 0;

    u32 o = 0, extension_count = 0;
    const char **extensions = NULL;
    f32 queue_priorities[5] = { 1.f, 1.f, 1.f, 1.f, 1.f };
    VkDeviceQueueCreateInfo queue_create_infos[5];

    VERIFY_VK(harridan->api.vkEnumeratePhysicalDevices(harridan->instance, &physical_device_count, NULL));
    if (!physical_device_count) {
        log_error("No GPUs are available to Vulkan.");
        return result_error;
    }

    physical_devices = (VkPhysicalDevice *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkPhysicalDevice) * physical_device_count, _Alignof(VkPhysicalDevice));
    VERIFY_VK(harridan->api.vkEnumeratePhysicalDevices(harridan->instance, &physical_device_count, physical_devices));

    struct physical_device_info *infos = (struct physical_device_info *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(struct physical_device_info) * physical_device_count, _Alignof(struct physical_device_info));
    memset(infos, 0, sizeof(struct physical_device_info) * physical_device_count);

    struct rivens_work *work = (struct rivens_work *)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(struct rivens_work) * physical_device_count, _Alignof(struct rivens_work));

    /* handle all physical devices concurrently */
    for (u32 i = 0; i < physical_device_count; i++) {
        struct physical_device_info *info = &infos[i];

        info->harridan = harridan;
        info->index = i;
        info->verbose = verbose;
        info->device = physical_devices[i];

        /* device extensions */
        info->ext_swapchain                     = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        info->ext_device_fault                  = VK_EXT_DEVICE_FAULT_EXTENSION_NAME;
        info->ext_memory_budget                 = VK_EXT_MEMORY_BUDGET_EXTENSION_NAME;
        info->ext_memory_priority               = VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME;
        info->ext_mesh_shader                   = VK_EXT_MESH_SHADER_EXTENSION_NAME;
        info->ext_fragment_shading_rate         = VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME;
        info->ext_deferred_host_operations      = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
        info->ext_acceleration_structure        = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME;
        info->ext_pipeline_library              = VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME;
        info->ext_ray_query                     = VK_KHR_RAY_QUERY_EXTENSION_NAME;
        info->ext_raytracing_pipeline           = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME;
        info->ext_raytracing_maintenance1       = VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME;
        info->ext_video_queue                   = VK_KHR_VIDEO_QUEUE_EXTENSION_NAME;
        info->ext_video_decode_queue            = VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME;
        info->ext_amd_device_coherent_memory    = VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME;

        /* backwards compatibility extensions */
        info->ext_dynamic_rendering_local_read  = VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME;
        info->ext_dynamic_rendering             = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
        info->ext_synchronization2              = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;
        info->ext_maintenance5                  = VK_KHR_MAINTENANCE_5_EXTENSION_NAME;
        info->ext_maintenance4                  = VK_KHR_MAINTENANCE_4_EXTENSION_NAME;

        /* prepare work info */
        work[i].procedure = (PFN_rivens_job)populate_physical_device_information;
        work[i].argument = (rivens_arg_t)info;
        work[i].name = "harridan:populate_physical_device_information";
    }
    riven_split_work_and_unchain(harridan->riven, work, physical_device_count);

    for (u32 i = 0; i < physical_device_count; i++) {
        if (!infos[i].accepted) continue;

        if (select_device_index >= 0 && select_device_index == (s32)i) {
            best_score = UINT32_MAX;
            physical_device_index = select_device_index;
        } else if (infos[i].score > best_score) {
            best_score = infos[i].score;
            physical_device_index = i;
        }
    }
    if (physical_device_index == UINT32_MAX) {
        log_error("Harridan: could not find an acceptable physical device..");
        return result_error;
    }
    struct physical_device_info *info = &infos[physical_device_index];
    harridan->physical = info->device;
    harridan->flags |= info->flag_bits;
    harridan->feats |= info->feat_bits;
    harridan->api.extensions |= info->extension_bits;
    if (info->has_vk14)
        harridan->api.extensions |= vk_extension_mask_api14;
    if (info->has_vk13)
        harridan->api.extensions |= vk_extension_mask_api13;

    harridan->graphics_queue_family = info->graphics_queue_family;
    harridan->compute_queue_family = info->compute_queue_family;
    harridan->transfer_queue_family = info->transfer_queue_family;
    harridan->sparse_queue_family = info->sparse_queue_family >= 0 ? (u32)info->sparse_queue_family : UINT32_MAX;
    harridan->decode_queue_family = info->decode_queue_family >= 0 ? (u32)info->decode_queue_family : UINT32_MAX;
    harridan->queue_family_count = info->queue_family_count;
    harridan->queue_family_indices = (u32 *)
        riven_alloc(harridan->riven, harridan->device_tag, sizeof(u32) * harridan->queue_family_count, _Alignof(u32));

    harridan->swapchain.format = info->swapchain_image_format;
    harridan->swapchain.surface_capabilities = info->surface_capabilities;
    harridan->swapchain.present_mode = info->present_mode;

    harridan->properties2 = info->properties2;
    harridan->properties_11 = info->properties_11;
    harridan->properties_12 = info->properties_12;
    harridan->properties_13 = info->properties_13;
    harridan->properties_14 = info->properties_14;
    harridan->acceleration_structure_properties = info->acceleration_structure_properties;
    harridan->raytracing_pipeline_properties = info->raytracing_pipeline_properties;
    harridan->fragment_shading_rate_properties = info->fragment_shading_rate_properties;
    harridan->mesh_shader_properties = info->mesh_shader_properties;
    harridan->memory_properties2 = info->memory_properties2;
    harridan->memory_budget = info->memory_budget;

    harridan->features2 = info->features2;
    harridan->features_11 = info->features_11;
    harridan->features_12 = info->features_12;
    harridan->features_13 = info->features_13;
    harridan->features_14 = info->features_14;
    harridan->acceleration_structure_features = info->acceleration_structure_features;
    harridan->raytracing_pipeline_features = info->raytracing_pipeline_features;
    harridan->raytracing_query_features = info->raytracing_query_features;
    harridan->mesh_shader_features = info->mesh_shader_features;

    harridan->decode_h264_profile = info->decode_h264_profile;
    harridan->decode_h264_capabilities = info->decode_h264_capabilities;
    harridan->video_profile = info->video_profile;
    harridan->video_decode_capabilities = info->video_decode_capabilities;
    harridan->video_capabilities = info->video_capabilities;

    extension_count = bits_popcnt_lookup((const u8 *)&info->extension_bits, sizeof(u64));
    extensions = (const char **)
        riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(const char *) * extension_count, _Alignof(const char *));
    if (info->extension_bits & vk_extension_swapchain_bit)
        extensions[o++] = info->ext_swapchain;
    if (info->extension_bits & vk_extension_device_fault_bit)
        extensions[o++] = info->ext_device_fault;
    if (info->extension_bits & vk_extension_memory_budget_bit)
        extensions[o++] = info->ext_memory_budget;
    if (info->extension_bits & vk_extension_memory_priority_bit)
        extensions[o++] = info->ext_memory_priority;
    if (info->extension_bits & vk_extension_mesh_shader_bit)
        extensions[o++] = info->ext_mesh_shader;
    if (info->extension_bits & vk_extension_fragment_shading_rate_bit)
        extensions[o++] = info->ext_fragment_shading_rate;
    if (info->extension_bits & vk_extension_deferred_host_operations_bit)
        extensions[o++] = info->ext_deferred_host_operations;
    if (info->extension_bits & vk_extension_acceleration_structure_bit)
        extensions[o++] = info->ext_acceleration_structure;
    if (info->extension_bits & vk_extension_pipeline_library_bit)
        extensions[o++] = info->ext_pipeline_library;
    if (info->extension_bits & vk_extension_ray_query_bit)
        extensions[o++] = info->ext_ray_query;
    if (info->extension_bits & vk_extension_ray_tracing_pipeline_bit)
        extensions[o++] = info->ext_raytracing_pipeline;
    if (info->extension_bits & vk_extension_ray_tracing_maintenance1_bit)
        extensions[o++] = info->ext_raytracing_maintenance1;
    if (info->extension_bits & vk_extension_video_queue_bit)
        extensions[o++] = info->ext_video_queue;
    if (info->extension_bits & vk_extension_video_decode_queue_bit)
        extensions[o++] = info->ext_video_decode_queue;
    if (info->extension_bits & vk_extension_amd_device_coherent_memory_bit)
        extensions[o++] = info->ext_amd_device_coherent_memory;
    if (info->extension_bits & vk_extension_dynamic_rendering_local_read_bit)
        extensions[o++] = info->ext_dynamic_rendering_local_read;
    if (info->extension_bits & vk_extension_dynamic_rendering_bit)
        extensions[o++] = info->ext_dynamic_rendering;
    if (info->extension_bits & vk_extension_synchronization2_bit)
        extensions[o++] = info->ext_synchronization2;
    if (info->extension_bits & vk_extension_maintenance5_bit)
        extensions[o++] = info->ext_maintenance5;
    if (info->extension_bits & vk_extension_maintenance4_bit)
        extensions[o++] = info->ext_maintenance4;
    assert_debug(o == extension_count);

    if (verbose) {
        char buffer[1024];
        usize bufsize = 1024;
        usize offset = snprintf(buffer, bufsize, "Vulkan device extensions enabled:");

        for (u32 i = 0; i < extension_count; i++)
            offset += snprintf(buffer + offset, bufsize - offset, "\n\t%s", extensions[i]);

        buffer[offset] = '\0';
        log_verbose("%s", buffer);
    }

    VkBool32 enable_dynamic_rendering = info->dynamic_rendering_features.dynamicRendering;
    VkBool32 enable_dynamic_local_read = info->dynamic_rendering_local_read_features.dynamicRenderingLocalRead;
    VkBool32 enable_synchronization2 = info->synchronization2_features.synchronization2;
    VkBool32 enable_mesh_shader = info->mesh_shader_features.meshShader;
    VkBool32 enable_fragment_shading = info->fragment_shading_rate_features.primitiveFragmentShadingRate;
    VkBool32 enable_acceleration_structure = info->acceleration_structure_features.accelerationStructure;
    VkBool32 enable_raytracing_pipeline = info->raytracing_pipeline_features.rayTracingPipeline;
    VkBool32 enable_rayquery = info->raytracing_query_features.rayQuery;
    info->features_11.pNext = NULL;

    struct feature_next {
        VkStructureType sType;
        void           *pNext;
    };
#define enable_feat(chain, structure) \
    if (enable_##chain) { next->pNext = &structure; enable_##chain = 0; continue; }

    for (struct feature_next *next = (struct feature_next *)&info->features_11; next; next = next->pNext) {
        next->pNext = NULL;
        if (!info->has_vk14) {
            enable_feat(dynamic_local_read, info->dynamic_rendering_local_read_features);
        }
        if (!info->has_vk13) {
            enable_feat(dynamic_rendering, info->dynamic_rendering_features);
            enable_feat(synchronization2, info->synchronization2_features);
        }
        enable_feat(mesh_shader, info->mesh_shader_features);
        enable_feat(fragment_shading, info->fragment_shading_rate_features);
        enable_feat(acceleration_structure, info->acceleration_structure_features);
        enable_feat(raytracing_pipeline, info->raytracing_pipeline_features);
        enable_feat(rayquery, info->raytracing_query_features);
    }
#undef enable_feat

    /* resolve queue families */
    for (u32 i = 0; i < info->queue_family_count; i++) {
        u32 family_index = info->queue_family_indices[i];
        harridan->queue_family_indices[i] = family_index;

        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].pNext = NULL;
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pQueuePriorities = queue_priorities;
        queue_create_infos[i].queueFamilyIndex = family_index;

        if (family_index == (u32)info->graphics_queue_family) {
            queue_create_infos[i].queueCount = info->graphics_family_create_queue_count;
        } else if (family_index == (u32)info->compute_queue_family && info->graphics_queue_family != info->compute_queue_family) {
            queue_create_infos[i].queueCount = info->compute_queue_family;
        } else {
            queue_create_infos[i].queueCount = 1;
        }
    }

    /* create the logical device */
    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = info->features2.pNext,
        .flags = 0,
        .pEnabledFeatures = &info->features2.features,
        .ppEnabledExtensionNames = (const char * const *)extensions,
        .enabledExtensionCount = extension_count,
        .ppEnabledLayerNames = NULL,
        .pQueueCreateInfos = queue_create_infos,
        .queueCreateInfoCount = info->queue_family_count,
    };
    VERIFY_VK(harridan->api.vkCreateDevice(harridan->physical, &device_info, NULL, &harridan->logical));

    if (!load_vulkan_device_procedures(&harridan->api, harridan->logical, info->properties2.properties.apiVersion)) {
        log_error("Harridan: can't load Vulkan procedures of the logical rendering device.");
        return result_error;
    }

    o = 0; 
    VkQueue command_queues[5];
    zeroa(command_queues);

    /* retrieve command queues */
    for (u32 i = 0; i < info->queue_family_count; i++) {
        u32 family_index = queue_create_infos[i].queueFamilyIndex;
        u32 queue_count = queue_create_infos[i].queueCount;

        for (u32 j = 0; j < queue_count; j++) {
            harridan->api.vkGetDeviceQueue(harridan->logical, family_index, j, &command_queues[o]);
            assert_debug(command_queues[o] != VK_NULL_HANDLE);
            o++;
        }
    }
    o = 0;
    harridan->graphics_queue = command_queues[o++];
    harridan->compute_queue  = command_queues[o++];
    harridan->transfer_queue = command_queues[o++];
    if (harridan->sparse_queue_family != UINT32_MAX)
        harridan->sparse_queue = command_queues[o++];
    if (harridan->decode_queue_family != UINT32_MAX)
        harridan->decode_queue = command_queues[o++];

    return result_success;
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
    s32 res = result_success;

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
    if (res != result_success) {
        log_error("Harridan: failed to create a Vulkan rendering device.");
        return res;
    }

    atomic_fetch_or_explicit(&harridan->flags, harridan_flag_device_is_valid, memory_order_relaxed);
    return result_success;
}

void harridan_fini(struct harridan *harridan)
{
    if (harridan->logical != VK_NULL_HANDLE) {
        harridan->api.vkDeviceWaitIdle(harridan->logical);
        harridan->api.vkDestroyDevice(harridan->logical, NULL);
    }

    if (harridan->swapchain.surface != VK_NULL_HANDLE)
        harridan->api.vkDestroySurfaceKHR(harridan->instance, harridan->swapchain.surface, NULL);

    if (harridan->api.extensions & vk_extension_layer_validation_bit)
        destroy_validation_layers(&harridan->api, harridan->instance, NULL, harridan->debug_messenger);

    if (harridan->instance)
        harridan->api.vkDestroyInstance(harridan->instance, NULL);
    close_vulkan_driver(&harridan->api);
    zerop(harridan);
}

static void reset_swapchain(
    const struct harridan *harridan,
    struct swapchain      *swapchain)
{
    for (u32 i = 0; i < swapchain->image_count; i++) {
        if (swapchain->image_views[i] != VK_NULL_HANDLE)
            harridan->api.vkDestroyImageView(harridan->logical, swapchain->image_views[i], NULL);
        swapchain->image_views[i] = VK_NULL_HANDLE;
        swapchain->images[i] = VK_NULL_HANDLE;
    }
    swapchain->extent = (VkExtent2D){0};
    swapchain->image_count = 0;
}

static s32 reconstruct_swapchain(
    struct harridan    *harridan,
    struct hadopelagic *hadal,
    struct swapchain   *swapchain,
    b32                 use_vsync)
{
    VkSwapchainKHR old_sc = swapchain->sc;
    u32 window_width, window_height;
    u32 write_flags = 0;
    s32 res = result_success;

    assert_debug(harridan->api.vkCreateSwapchainKHR);

    /* handle the surface if necessary */
    if (atomic_load_explicit(&harridan->flags, memory_order_relaxed) & harridan_flag_surface_lost || !swapchain->surface) {
        if (swapchain->surface)
            harridan->api.vkDestroySurfaceKHR(harridan->instance, swapchain->surface, NULL);

        res = hadal->interface.create_vulkan_surface(hadal, harridan, swapchain);
        if (res != result_success)
            return res;

        res = prepare_surface(harridan, harridan->physical, swapchain, &swapchain->surface_capabilities, &swapchain->format, &swapchain->present_mode);
        if (res != result_success)
            return res;
    }

    /* handle the presentation semaphores */
    if (swapchain->image_available_sem[0] == VK_NULL_HANDLE) {
        VkSemaphoreCreateInfo sem_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        for (u32 i = 0; i < min(swapchain->image_count, AMW_MAX_FRAMES_IN_FLIGHT); i++)
            VERIFY_VK(harridan->api.vkCreateSemaphore(harridan->logical, &sem_info, NULL, &swapchain->image_available_sem[i]));
    }

    /* resolve the resolution */
    window_width = atomic_load_explicit(&hadal->fb_width, memory_order_acquire);
    window_height = atomic_load_explicit(&hadal->fb_height, memory_order_acquire);
    swapchain->extent.width = (swapchain->surface_capabilities.currentExtent.width != 0xffffffff) ? swapchain->surface_capabilities.currentExtent.width : window_width;
    swapchain->extent.height = (swapchain->surface_capabilities.currentExtent.height != 0xffffffff) ? swapchain->surface_capabilities.currentExtent.height : window_height;

    /* find a supported composite alpha (one of these is guaranteed to be set) */
    VkCompositeAlphaFlagsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagsKHR composite_alpha_flags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (u32 i = 0; i < arraysize(composite_alpha_flags); i++) {
        if (swapchain->surface_capabilities.supportedCompositeAlpha & composite_alpha_flags[i]) {
            composite_alpha = composite_alpha_flags[i];
            break;
        }
    }

    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (swapchain->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        write_flags |= harridan_flag_screenshot_supported;
    }

    VkSharingMode sharing_mode = VK_SHARING_MODE_CONCURRENT;
    if (harridan->queue_family_count == 1 ||
        (harridan->graphics_queue_family == harridan->compute_queue_family &&
         harridan->graphics_queue_family == harridan->transfer_queue_family))
        sharing_mode = VK_SHARING_MODE_EXCLUSIVE;

    u32 min_image_count = swapchain->surface_capabilities.minImageCount;
    u32 max_image_count = swapchain->surface_capabilities.maxImageCount;
    u32 image_count = AMW_MAX_FRAMES_IN_FLIGHT;
    if (image_count < min_image_count)
        image_count = min_image_count;
    if (image_count > max_image_count && max_image_count != 0)
        image_count = max_image_count;

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = swapchain->surface,
        .minImageCount = image_count,
        .imageFormat = swapchain->format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = swapchain->extent,
        .imageArrayLayers = 1,
        .imageUsage = image_usage,
        .imageSharingMode = sharing_mode,
        .queueFamilyIndexCount = harridan->queue_family_count,
        .pQueueFamilyIndices = harridan->queue_family_indices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = composite_alpha,
        .presentMode = use_vsync ? VK_PRESENT_MODE_FIFO_KHR : swapchain->present_mode,
        .clipped = VK_FALSE,
        .oldSwapchain = old_sc,
    };
    VERIFY_VK(harridan->api.vkCreateSwapchainKHR(harridan->logical, &swapchain_info, NULL, &swapchain->sc));

    if (old_sc != VK_NULL_HANDLE)
        harridan->api.vkDestroySwapchainKHR(harridan->logical, old_sc, NULL);

    VERIFY_VK(harridan->api.vkGetSwapchainImagesKHR(harridan->logical, swapchain->sc, &swapchain->image_count, NULL));
    VERIFY_VK(harridan->api.vkGetSwapchainImagesKHR(harridan->logical, swapchain->sc, &swapchain->image_count, swapchain->images));
    for (u32 i = 0; i < swapchain->image_count; i++) {
        VkImageViewCreateInfo color_image_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.levelCount = 1,
            .subresourceRange.layerCount = 1,
        };
        VERIFY_VK(harridan->api.vkCreateImageView(harridan->logical, &color_image_view, NULL, &swapchain->image_views[i]));
    }

    if (use_vsync || swapchain->present_mode == VK_PRESENT_MODE_FIFO_KHR)
        write_flags |= harridan_flag_vsync_enabled;

    atomic_fetch_and_explicit(&hadal->flags, ~(hadal_flag_framebuffer_resized), memory_order_release);
    atomic_fetch_and_explicit(&harridan->flags, ~(harridan_flag_vsync_enabled | harridan_flag_screenshot_supported | harridan_flag_surface_lost), memory_order_release);
    atomic_fetch_or_explicit(&harridan->flags, write_flags, memory_order_release);
    return result_success;
}

void harridan_orchestrate_swapchain(struct harridan_orchestrate_swapchain_work *work)
{
    if (!work) return;

    struct harridan *harridan = work->harridan;
    struct hadopelagic *hadal = work->hadal;
    struct swapchain *swapchain = work->swapchain;

    if (!harridan || !hadal || !swapchain) {
        log_error("Harridan: incomplete work request for swapchain.");
        work->result = result_error;
        return;
    }

    switch (work->type) {
        case work_type_disassembly:
            harridan->api.vkDeviceWaitIdle(harridan->logical);
            for (u32 i = 0; i < min(swapchain->image_count, AMW_MAX_FRAMES_IN_FLIGHT); i++)
                if (swapchain->image_available_sem[i] != VK_NULL_HANDLE)
                    harridan->api.vkDestroySemaphore(harridan->logical, swapchain->image_available_sem[i], NULL);
            reset_swapchain(harridan, swapchain);
            if (swapchain->sc)
                harridan->api.vkDestroySwapchainKHR(harridan->logical, swapchain->sc, NULL);
            return;

        case work_type_assembly:
        case work_type_reassembly:
            work->result = reconstruct_swapchain(harridan, hadal, swapchain, work->use_vsync);
        default:
            return;
    }
}
