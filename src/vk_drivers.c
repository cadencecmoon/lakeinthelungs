#include <amw/a_moonlit_walk.h>

#include "vk_reznor.h"

/* XXX malloc for callbacks, until i figure out how to make use of Riven for realloc/free */
#include <stdlib.h>

static struct reznor *g_vulkan = NULL;

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


static PFN_vkVoidFunction instance_proc_address(const struct reznor *reznor, const char *procname)
{
    PFN_vkVoidFunction address = reznor->vkGetInstanceProcAddr(reznor->instance, procname);
    if (address == NULL)
        log_warn("Can't find Vulkan instance procedure '%s'.", procname);
    return address;
}

static PFN_vkVoidFunction device_proc_address(const struct reznor_device *device, const char *procname)
{
    PFN_vkVoidFunction address = device->header.reznor->vkGetDeviceProcAddr(device->logical, procname);
    if (address == NULL)
        log_warn("Can't find Vulkan device procedure '%s'.", procname);
    return address;
}

b32 vulkan_load_instance_procedures(struct reznor *reznor, u32 api_version, u32 extension_bits)
{
    assert_debug(reznor && reznor->vkGetInstanceProcAddr);

    reznor->vkCreateDevice = (PFN_vkCreateDevice)
        instance_proc_address(reznor, "vkCreateDevice");
    reznor->vkDestroyInstance = (PFN_vkDestroyInstance)
        instance_proc_address(reznor, "vkDestroyInstance");
    reznor->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)
        instance_proc_address(reznor, "vkEnumerateDeviceExtensionProperties");
    reznor->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        instance_proc_address(reznor, "vkEnumeratePhysicalDevices");
    reznor->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)
        instance_proc_address(reznor, "vkEnumeratePhysicalDeviceGroups");
    reznor->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)
        instance_proc_address(reznor, "vkGetPhysicalDeviceFeatures");
    reznor->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceFeatures2");
    reznor->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceFormatProperties");
    reznor->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceFormatProperties2");
    reznor->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceImageFormatProperties");
    reznor->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceImageFormatProperties2");
    reznor->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceMemoryProperties");
    reznor->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceMemoryProperties2");
    reznor->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceProperties");
    reznor->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceProperties2");
    reznor->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceQueueFamilyProperties");
    reznor->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceQueueFamilyProperties2");
    reznor->vkGetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceSparseImageFormatProperties");
    reznor->vkGetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2)
        instance_proc_address(reznor, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    reznor->vkGetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties)
        instance_proc_address(reznor, "vkGetPhysicalDeviceToolProperties");

    if (!reznor->vkCreateDevice ||
        !reznor->vkDestroyInstance ||
        !reznor->vkEnumerateDeviceExtensionProperties ||
        !reznor->vkEnumeratePhysicalDevices ||
        !reznor->vkEnumeratePhysicalDeviceGroups ||
        !reznor->vkGetPhysicalDeviceFeatures ||
        !reznor->vkGetPhysicalDeviceFeatures2 ||
        !reznor->vkGetPhysicalDeviceFormatProperties ||
        !reznor->vkGetPhysicalDeviceFormatProperties2 ||
        !reznor->vkGetPhysicalDeviceImageFormatProperties ||
        !reznor->vkGetPhysicalDeviceImageFormatProperties2 ||
        !reznor->vkGetPhysicalDeviceMemoryProperties ||
        !reznor->vkGetPhysicalDeviceMemoryProperties2 ||
        !reznor->vkGetPhysicalDeviceProperties ||
        !reznor->vkGetPhysicalDeviceProperties2 ||
        !reznor->vkGetPhysicalDeviceQueueFamilyProperties ||
        !reznor->vkGetPhysicalDeviceQueueFamilyProperties2 ||
        !reznor->vkGetPhysicalDeviceSparseImageFormatProperties ||
        !reznor->vkGetPhysicalDeviceSparseImageFormatProperties2 ||
        !reznor->vkGetPhysicalDeviceToolProperties)
        return false;

    /* VK_KHR_surface */
    if (extension_bits & vulkan_ext_surface_bit) {

        reznor->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)
            instance_proc_address(reznor, "vkDestroySurfaceKHR");
        reznor->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceSurfaceSupportKHR");
        reznor->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        reznor->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        reznor->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!reznor->vkDestroySurfaceKHR ||
            !reznor->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !reznor->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !reznor->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !reznor->vkGetPhysicalDeviceSurfacePresentModesKHR)
            return false;
    }

    /* VK_EXT_debug_utils */
    if (extension_bits & vulkan_ext_debug_utils_bit) {

        reznor->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            instance_proc_address(reznor, "vkSetDebugUtilsObjectNameEXT");
        reznor->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)
            instance_proc_address(reznor, "vkSetDebugUtilsObjectTagEXT");
        reznor->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkQueueBeginDebugUtilsLabelEXT");
        reznor->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkQueueEndDebugUtilsLabelEXT");
        reznor->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkQueueInsertDebugUtilsLabelEXT");
        reznor->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkCmdBeginDebugUtilsLabelEXT");
        reznor->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkCmdEndDebugUtilsLabelEXT");
        reznor->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)
            instance_proc_address(reznor, "vkCmdInsertDebugUtilsLabelEXT");
        reznor->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
            instance_proc_address(reznor, "vkCreateDebugUtilsMessengerEXT");
        reznor->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
            instance_proc_address(reznor, "vkDestroyDebugUtilsMessengerEXT");
        reznor->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)
            instance_proc_address(reznor, "vkSubmitDebugUtilsMessageEXT");

        if (!reznor->vkSetDebugUtilsObjectNameEXT ||
            !reznor->vkSetDebugUtilsObjectTagEXT ||
            !reznor->vkQueueBeginDebugUtilsLabelEXT ||
            !reznor->vkQueueEndDebugUtilsLabelEXT ||
            !reznor->vkQueueInsertDebugUtilsLabelEXT ||
            !reznor->vkCmdBeginDebugUtilsLabelEXT ||
            !reznor->vkCmdEndDebugUtilsLabelEXT ||
            !reznor->vkCmdInsertDebugUtilsLabelEXT ||
            !reznor->vkCreateDebugUtilsMessengerEXT ||
            !reznor->vkDestroyDebugUtilsMessengerEXT ||
            !reznor->vkSubmitDebugUtilsMessageEXT)
            return false;
    }

    /* VK_KHR_video_queue, a device extension */
    if (api_version >= VK_API_VERSION_1_3 || (extension_bits & vulkan_ext_synchronization2_bit)) {

        reznor->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
        reznor->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)
            instance_proc_address(reznor, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");

        if (!reznor->vkGetPhysicalDeviceVideoCapabilitiesKHR || !reznor->vkGetPhysicalDeviceVideoFormatPropertiesKHR)
            return false;
    }

    return true;
}

b32 vulkan_load_device_procedures(struct reznor_device *device, u32 api_version, u64 extension_bits)
{
    assert_debug(device && device->header.reznor->vkGetDeviceProcAddr);

    /* core 1.0 */
	device->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
        device_proc_address(device, "vkAllocateCommandBuffers");
	device->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)
        device_proc_address(device, "vkAllocateDescriptorSets");
	device->vkAllocateMemory = (PFN_vkAllocateMemory)
        device_proc_address(device, "vkAllocateMemory");
	device->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)
        device_proc_address(device, "vkBeginCommandBuffer");
	device->vkBindBufferMemory = (PFN_vkBindBufferMemory)
        device_proc_address(device, "vkBindBufferMemory");
	device->vkBindImageMemory = (PFN_vkBindImageMemory)
        device_proc_address(device, "vkBindImageMemory");
	device->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)
        device_proc_address(device, "vkCmdBeginQuery");
	device->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)
        device_proc_address(device, "vkCmdBeginRenderPass");
	device->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)
        device_proc_address(device, "vkCmdBindDescriptorSets");
	device->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)
        device_proc_address(device, "vkCmdBindIndexBuffer");
	device->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)
        device_proc_address(device, "vkCmdBindPipeline");
	device->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)
        device_proc_address(device, "vkCmdBindVertexBuffers");
	device->vkCmdBlitImage = (PFN_vkCmdBlitImage)
        device_proc_address(device, "vkCmdBlitImage");
	device->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)
        device_proc_address(device, "vkCmdClearAttachments");
	device->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)
        device_proc_address(device, "vkCmdClearColorImage");
	device->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)
        device_proc_address(device, "vkCmdClearDepthStencilImage");
	device->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)
        device_proc_address(device, "vkCmdCopyBuffer");
	device->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)
        device_proc_address(device, "vkCmdCopyBufferToImage");
	device->vkCmdCopyImage = (PFN_vkCmdCopyImage)
        device_proc_address(device, "vkCmdCopyImage");
	device->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)
        device_proc_address(device, "vkCmdCopyImageToBuffer");
	device->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)
        device_proc_address(device, "vkCmdCopyQueryPoolResults");
	device->vkCmdDispatch = (PFN_vkCmdDispatch)
        device_proc_address(device, "vkCmdDispatch");
	device->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)
        device_proc_address(device, "vkCmdDispatchIndirect");
	device->vkCmdDraw = (PFN_vkCmdDraw)
        device_proc_address(device, "vkCmdDraw");
	device->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)
        device_proc_address(device, "vkCmdDrawIndexed");
	device->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)
        device_proc_address(device, "vkCmdDrawIndexedIndirect");
	device->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)
        device_proc_address(device, "vkCmdDrawIndirect");
	device->vkCmdEndQuery = (PFN_vkCmdEndQuery)
        device_proc_address(device, "vkCmdEndQuery");
	device->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)
        device_proc_address(device, "vkCmdEndRenderPass");
	device->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)
        device_proc_address(device, "vkCmdExecuteCommands");
	device->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)
        device_proc_address(device, "vkCmdFillBuffer");
	device->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)
        device_proc_address(device, "vkCmdNextSubpass");
	device->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
        device_proc_address(device, "vkCmdPipelineBarrier");
	device->vkCmdPushConstants = (PFN_vkCmdPushConstants)
        device_proc_address(device, "vkCmdPushConstants");
	device->vkCmdResetEvent = (PFN_vkCmdResetEvent)
        device_proc_address(device, "vkCmdResetEvent");
	device->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)
        device_proc_address(device, "vkCmdResetQueryPool");
	device->vkCmdResolveImage = (PFN_vkCmdResolveImage)
        device_proc_address(device, "vkCmdResolveImage");
	device->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)
        device_proc_address(device, "vkCmdSetBlendConstants");
	device->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)
        device_proc_address(device, "vkCmdSetDepthBias");
	device->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)
        device_proc_address(device, "vkCmdSetDepthBounds");
	device->vkCmdSetEvent = (PFN_vkCmdSetEvent)
        device_proc_address(device, "vkCmdSetEvent");
	device->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)
        device_proc_address(device, "vkCmdSetLineWidth");
	device->vkCmdSetScissor = (PFN_vkCmdSetScissor)
        device_proc_address(device, "vkCmdSetScissor");
	device->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)
        device_proc_address(device, "vkCmdSetStencilCompareMask");
	device->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)
        device_proc_address(device, "vkCmdSetStencilReference");
	device->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)
        device_proc_address(device, "vkCmdSetStencilWriteMask");
	device->vkCmdSetViewport = (PFN_vkCmdSetViewport)
        device_proc_address(device, "vkCmdSetViewport");
	device->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)
        device_proc_address(device, "vkCmdUpdateBuffer");
	device->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)
        device_proc_address(device, "vkCmdWaitEvents");
	device->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)
        device_proc_address(device, "vkCmdWriteTimestamp");
	device->vkCreateBuffer = (PFN_vkCreateBuffer)
        device_proc_address(device, "vkCreateBuffer");
	device->vkCreateBufferView = (PFN_vkCreateBufferView)
        device_proc_address(device, "vkCreateBufferView");
	device->vkCreateCommandPool = (PFN_vkCreateCommandPool)
        device_proc_address(device, "vkCreateCommandPool");
	device->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)
        device_proc_address(device, "vkCreateComputePipelines");
	device->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)
        device_proc_address(device, "vkCreateDescriptorPool");
	device->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)
        device_proc_address(device, "vkCreateDescriptorSetLayout");
	device->vkCreateEvent = (PFN_vkCreateEvent)
        device_proc_address(device, "vkCreateEvent");
	device->vkCreateFence = (PFN_vkCreateFence)
        device_proc_address(device, "vkCreateFence");
	device->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)
        device_proc_address(device, "vkCreateFramebuffer");
	device->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)
        device_proc_address(device, "vkCreateGraphicsPipelines");
	device->vkCreateImage = (PFN_vkCreateImage)
        device_proc_address(device, "vkCreateImage");
	device->vkCreateImageView = (PFN_vkCreateImageView)
        device_proc_address(device, "vkCreateImageView");
	device->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)
        device_proc_address(device, "vkCreatePipelineCache");
	device->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)
        device_proc_address(device, "vkCreatePipelineLayout");
	device->vkCreateQueryPool = (PFN_vkCreateQueryPool)
        device_proc_address(device, "vkCreateQueryPool");
	device->vkCreateRenderPass = (PFN_vkCreateRenderPass)
        device_proc_address(device, "vkCreateRenderPass");
	device->vkCreateSampler = (PFN_vkCreateSampler)
        device_proc_address(device, "vkCreateSampler");
	device->vkCreateSemaphore = (PFN_vkCreateSemaphore)
        device_proc_address(device, "vkCreateSemaphore");
	device->vkCreateShaderModule = (PFN_vkCreateShaderModule)
        device_proc_address(device, "vkCreateShaderModule");
	device->vkDestroyBuffer = (PFN_vkDestroyBuffer)
        device_proc_address(device, "vkDestroyBuffer");
	device->vkDestroyBufferView = (PFN_vkDestroyBufferView)
        device_proc_address(device, "vkDestroyBufferView");
	device->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)
        device_proc_address(device, "vkDestroyCommandPool");
	device->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)
        device_proc_address(device, "vkDestroyDescriptorPool");
	device->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)
        device_proc_address(device, "vkDestroyDescriptorSetLayout");
	device->vkDestroyDevice = (PFN_vkDestroyDevice)
        device_proc_address(device, "vkDestroyDevice");
	device->vkDestroyEvent = (PFN_vkDestroyEvent)
        device_proc_address(device, "vkDestroyEvent");
	device->vkDestroyFence = (PFN_vkDestroyFence)
        device_proc_address(device, "vkDestroyFence");
	device->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)
        device_proc_address(device, "vkDestroyFramebuffer");
	device->vkDestroyImage = (PFN_vkDestroyImage)
        device_proc_address(device, "vkDestroyImage");
	device->vkDestroyImageView = (PFN_vkDestroyImageView)
        device_proc_address(device, "vkDestroyImageView");
	device->vkDestroyPipeline = (PFN_vkDestroyPipeline)
        device_proc_address(device, "vkDestroyPipeline");
	device->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)
        device_proc_address(device, "vkDestroyPipelineCache");
	device->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)
        device_proc_address(device, "vkDestroyPipelineLayout");
	device->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)
        device_proc_address(device, "vkDestroyQueryPool");
	device->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)
        device_proc_address(device, "vkDestroyRenderPass");
	device->vkDestroySampler = (PFN_vkDestroySampler)
        device_proc_address(device, "vkDestroySampler");
	device->vkDestroySemaphore = (PFN_vkDestroySemaphore)
        device_proc_address(device, "vkDestroySemaphore");
	device->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)
        device_proc_address(device, "vkDestroyShaderModule");
	device->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)
        device_proc_address(device, "vkDeviceWaitIdle");
	device->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)
        device_proc_address(device, "vkEndCommandBuffer");
	device->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)
        device_proc_address(device, "vkFlushMappedMemoryRanges");
	device->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)
        device_proc_address(device, "vkFreeCommandBuffers");
	device->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)
        device_proc_address(device, "vkFreeDescriptorSets");
	device->vkFreeMemory = (PFN_vkFreeMemory)
        device_proc_address(device, "vkFreeMemory");
	device->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)
        device_proc_address(device, "vkGetBufferMemoryRequirements");
	device->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)
        device_proc_address(device, "vkGetDeviceMemoryCommitment");
	device->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)
        device_proc_address(device, "vkGetDeviceQueue");
	device->vkGetEventStatus = (PFN_vkGetEventStatus)
        device_proc_address(device, "vkGetEventStatus");
	device->vkGetFenceStatus = (PFN_vkGetFenceStatus)
        device_proc_address(device, "vkGetFenceStatus");
	device->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)
        device_proc_address(device, "vkGetImageMemoryRequirements");
	device->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)
        device_proc_address(device, "vkGetImageSparseMemoryRequirements");
	device->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)
        device_proc_address(device, "vkGetImageSubresourceLayout");
	device->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)
        device_proc_address(device, "vkGetPipelineCacheData");
	device->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)
        device_proc_address(device, "vkGetQueryPoolResults");
	device->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)
        device_proc_address(device, "vkGetRenderAreaGranularity");
	device->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)
        device_proc_address(device, "vkInvalidateMappedMemoryRanges");
	device->vkMapMemory = (PFN_vkMapMemory)
        device_proc_address(device, "vkMapMemory");
	device->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)
        device_proc_address(device, "vkvkMergePipelineCaches");
	device->vkQueueBindSparse = (PFN_vkQueueBindSparse)
        device_proc_address(device, "vkQueueBindSparse");
	device->vkQueueSubmit = (PFN_vkQueueSubmit)
        device_proc_address(device, "vkQueueSubmit");
	device->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)
        device_proc_address(device, "vkQueueWaitIdle");
	device->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)
        device_proc_address(device, "vkResetCommandBuffer");
	device->vkResetCommandPool = (PFN_vkResetCommandPool)
        device_proc_address(device, "vkResetCommandPool");
	device->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)
        device_proc_address(device, "vkResetDescriptorPool");
	device->vkResetEvent = (PFN_vkResetEvent)
        device_proc_address(device, "vkResetEvent");
	device->vkResetFences = (PFN_vkResetFences)
        device_proc_address(device, "vkResetFences");
	device->vkSetEvent = (PFN_vkSetEvent)
        device_proc_address(device, "vkSetEvent");
	device->vkUnmapMemory = (PFN_vkUnmapMemory)
        device_proc_address(device, "vkUnmapMemory");
	device->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)
        device_proc_address(device, "vkUpdateDescriptorSets");
	device->vkWaitForFences = (PFN_vkWaitForFences)
        device_proc_address(device, "vkWaitForFences");

	if (!device->vkAllocateCommandBuffers ||
	    !device->vkAllocateDescriptorSets ||
	    !device->vkAllocateMemory ||
	    !device->vkBeginCommandBuffer ||
	    !device->vkBindBufferMemory ||
	    !device->vkBindImageMemory ||
	    !device->vkCmdBeginQuery ||
	    !device->vkCmdBeginRenderPass ||
	    !device->vkCmdBindDescriptorSets ||
	    !device->vkCmdBindIndexBuffer ||
	    !device->vkCmdBindPipeline ||
	    !device->vkCmdBindVertexBuffers ||
	    !device->vkCmdBlitImage ||
	    !device->vkCmdClearAttachments ||
	    !device->vkCmdClearColorImage ||
	    !device->vkCmdClearDepthStencilImage ||
	    !device->vkCmdCopyBuffer ||
	    !device->vkCmdCopyBufferToImage ||
	    !device->vkCmdCopyImage ||
	    !device->vkCmdCopyImageToBuffer ||
	    !device->vkCmdCopyQueryPoolResults ||
	    !device->vkCmdDispatch ||
	    !device->vkCmdDispatchIndirect ||
	    !device->vkCmdDraw ||
	    !device->vkCmdDrawIndexed ||
	    !device->vkCmdDrawIndexedIndirect ||
	    !device->vkCmdDrawIndirect ||
	    !device->vkCmdEndQuery ||
	    !device->vkCmdEndRenderPass ||
	    !device->vkCmdExecuteCommands ||
	    !device->vkCmdFillBuffer ||
	    !device->vkCmdNextSubpass ||
	    !device->vkCmdPipelineBarrier ||
	    !device->vkCmdPushConstants ||
	    !device->vkCmdResetEvent ||
	    !device->vkCmdResetQueryPool ||
	    !device->vkCmdResolveImage ||
	    !device->vkCmdSetBlendConstants ||
	    !device->vkCmdSetDepthBias ||
	    !device->vkCmdSetDepthBounds ||
	    !device->vkCmdSetEvent ||
	    !device->vkCmdSetLineWidth ||
	    !device->vkCmdSetScissor ||
	    !device->vkCmdSetStencilCompareMask ||
	    !device->vkCmdSetStencilReference ||
	    !device->vkCmdSetStencilWriteMask ||
	    !device->vkCmdSetViewport ||
	    !device->vkCmdUpdateBuffer ||
	    !device->vkCmdWaitEvents ||
	    !device->vkCmdWriteTimestamp ||
	    !device->vkCreateBuffer ||
	    !device->vkCreateBufferView ||
	    !device->vkCreateCommandPool ||
	    !device->vkCreateComputePipelines ||
	    !device->vkCreateDescriptorPool ||
	    !device->vkCreateDescriptorSetLayout ||
	    !device->vkCreateEvent ||
	    !device->vkCreateFence ||
	    !device->vkCreateFramebuffer ||
	    !device->vkCreateGraphicsPipelines ||
	    !device->vkCreateImage ||
	    !device->vkCreateImageView ||
	    !device->vkCreatePipelineCache ||
	    !device->vkCreatePipelineLayout ||
	    !device->vkCreateQueryPool ||
	    !device->vkCreateRenderPass ||
	    !device->vkCreateSampler ||
	    !device->vkCreateSemaphore ||
	    !device->vkCreateShaderModule ||
	    !device->vkDestroyBuffer ||
	    !device->vkDestroyBufferView ||
	    !device->vkDestroyCommandPool ||
	    !device->vkDestroyDescriptorPool ||
	    !device->vkDestroyDescriptorSetLayout ||
	    !device->vkDestroyDevice ||
	    !device->vkDestroyEvent ||
	    !device->vkDestroyFence ||
	    !device->vkDestroyFramebuffer ||
	    !device->vkDestroyImage ||
	    !device->vkDestroyImageView ||
	    !device->vkDestroyPipeline ||
	    !device->vkDestroyPipelineCache ||
	    !device->vkDestroyPipelineLayout ||
	    !device->vkDestroyQueryPool ||
	    !device->vkDestroyRenderPass ||
	    !device->vkDestroySampler ||
	    !device->vkDestroySemaphore ||
	    !device->vkDestroyShaderModule ||
	    !device->vkDeviceWaitIdle ||
	    !device->vkEndCommandBuffer ||
	    !device->vkFlushMappedMemoryRanges ||
	    !device->vkFreeCommandBuffers ||
	    !device->vkFreeDescriptorSets ||
	    !device->vkFreeMemory ||
	    !device->vkGetBufferMemoryRequirements ||
	    !device->vkGetDeviceMemoryCommitment ||
	    !device->vkGetDeviceQueue ||
	    !device->vkGetEventStatus ||
	    !device->vkGetFenceStatus ||
	    !device->vkGetImageMemoryRequirements ||
	    !device->vkGetImageSparseMemoryRequirements ||
	    !device->vkGetImageSubresourceLayout ||
	    !device->vkGetPipelineCacheData ||
	    !device->vkGetQueryPoolResults ||
	    !device->vkGetRenderAreaGranularity ||
	    !device->vkInvalidateMappedMemoryRanges ||
	    !device->vkMapMemory ||
	    !device->vkMergePipelineCaches ||
	    !device->vkQueueBindSparse ||
	    !device->vkQueueSubmit ||
	    !device->vkQueueWaitIdle ||
	    !device->vkResetCommandBuffer ||
	    !device->vkResetCommandPool ||
	    !device->vkResetDescriptorPool ||
	    !device->vkResetEvent ||
	    !device->vkResetFences ||
	    !device->vkSetEvent ||
	    !device->vkUnmapMemory ||
	    !device->vkUpdateDescriptorSets ||
	    !device->vkWaitForFences)
        return false;

    /* core 1.1 */
    device->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)
        device_proc_address(device, "vkBindBufferMemory2");
    device->vkBindImageMemory2 = (PFN_vkBindImageMemory2)
        device_proc_address(device, "vkBindImageMemory2");
    device->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)
        device_proc_address(device, "vkCmdDispatchBase");
    device->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)
        device_proc_address(device, "vkCmdSetDeviceMask");
    device->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)
        device_proc_address(device, "vkCreateDescriptorUpdateTemplate");
    device->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)
        device_proc_address(device, "vkCreateSamplerYcbcrConversion");
    device->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)
        device_proc_address(device, "vkDestroyDescriptorUpdateTemplate");
    device->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)
        device_proc_address(device, "vkDestroySamplerYcbcrConversion");
    device->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)
        device_proc_address(device, "vkGetBufferMemoryRequirements2");
    device->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)
        device_proc_address(device, "vkGetDescriptorSetLayoutSupport");
    device->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)
        device_proc_address(device, "vkGetDeviceGroupPeerMemoryFeatures");
    device->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)
        device_proc_address(device, "vkGetDeviceQueue2");
    device->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)
        device_proc_address(device, "vkGetImageMemoryRequirements2");
    device->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)
        device_proc_address(device, "vkGetImageSparseMemoryRequirements2");
    device->vkTrimCommandPool = (PFN_vkTrimCommandPool)
        device_proc_address(device, "vkTrimCommandPool");
    device->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)
        device_proc_address(device, "vkUpdateDescriptorSetWithTemplate");

    if (!device->vkBindBufferMemory2 ||
        !device->vkBindImageMemory2 ||
        !device->vkCmdDispatchBase ||
        !device->vkCmdSetDeviceMask ||
        !device->vkCreateDescriptorUpdateTemplate ||
        !device->vkCreateSamplerYcbcrConversion ||
        !device->vkDestroyDescriptorUpdateTemplate ||
        !device->vkDestroySamplerYcbcrConversion ||
        !device->vkGetBufferMemoryRequirements2 ||
        !device->vkGetDescriptorSetLayoutSupport ||
        !device->vkGetDeviceGroupPeerMemoryFeatures ||
        !device->vkGetDeviceQueue2 ||
        !device->vkGetImageMemoryRequirements2 ||
        !device->vkGetImageSparseMemoryRequirements2 ||
        !device->vkTrimCommandPool ||
        !device->vkUpdateDescriptorSetWithTemplate)
        return false;

    /* core 1.2 */
    device->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)
        device_proc_address(device, "vkCmdDrawIndexedIndirectCount");
    device->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)
        device_proc_address(device, "vkCmdDrawIndirectCount");
    device->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)
        device_proc_address(device, "vkGetBufferDeviceAddress");
    device->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)
        device_proc_address(device, "vkGetBufferOpaqueCaptureAddress");
    device->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)
        device_proc_address(device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    device->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)
        device_proc_address(device, "vkGetSemaphoreCounterValue");
    device->vkResetQueryPool = (PFN_vkResetQueryPool)
        device_proc_address(device, "vkResetQueryPool");
    device->vkSignalSemaphore = (PFN_vkSignalSemaphore)
        device_proc_address(device, "vkSignalSemaphore");
    device->vkWaitSemaphores = (PFN_vkWaitSemaphores)
        device_proc_address(device, "vkWaitSemaphores");

    if (!device->vkCmdDrawIndexedIndirectCount ||
        !device->vkCmdDrawIndirectCount ||
        !device->vkGetBufferDeviceAddress ||
        !device->vkGetBufferOpaqueCaptureAddress ||
        !device->vkGetDeviceMemoryOpaqueCaptureAddress ||
        !device->vkGetSemaphoreCounterValue ||
        !device->vkResetQueryPool ||
        !device->vkSignalSemaphore ||
        !device->vkWaitSemaphores)
        return false;
    
    /* core 1.3 */
    if (api_version >= VK_API_VERSION_1_3) {

        device->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)
            device_proc_address(device, "vkCmdBeginRendering");
        device->vkCmdEndRendering = (PFN_vkCmdEndRendering)
            device_proc_address(device, "vkCmdEndRendering");
        device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2)
            device_proc_address(device, "vkCmdPipelineBarrier2");
        device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2)
            device_proc_address(device, "vkCmdResetEvent2");
        device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2)
            device_proc_address(device, "vkCmdSetEvent2");
        device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2)
            device_proc_address(device, "vkCmdWaitEvents2");
        device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2)
            device_proc_address(device, "vkCmdWriteTimestamp2");
        device->vkQueueSubmit2 = (PFN_vkQueueSubmit2)
            device_proc_address(device, "vkQueueSubmit2");
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements)
            device_proc_address(device, "vkGetDeviceBufferMemoryRequirements");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements)
            device_proc_address(device, "vkGetDeviceImageMemoryRequirements");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements)
            device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirements");
    
        if (!device->vkCmdBeginRendering ||
            !device->vkCmdEndRendering ||
            !device->vkCmdPipelineBarrier2 ||
            !device->vkCmdResetEvent2 ||
            !device->vkCmdSetEvent2 ||
            !device->vkCmdWaitEvents2 ||
            !device->vkCmdWriteTimestamp2 ||
            !device->vkQueueSubmit2 ||
            !device->vkGetDeviceBufferMemoryRequirements ||
            !device->vkGetDeviceImageMemoryRequirements ||
            !device->vkGetDeviceImageSparseMemoryRequirements)
            return false;

    } else { /* backwards compatibility 1.3 */

        if (extension_bits & vulkan_ext_dynamic_rendering_bit) {
            device->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)
                device_proc_address(device, "vkCmdBeginRenderingKHR");
            device->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)
                device_proc_address(device, "vkCmdEndRenderingKHR");

            if (!device->vkCmdBeginRendering || !device->vkCmdEndRendering)
                return false;
        }
        if (extension_bits & vulkan_ext_synchronization2_bit) {
            device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2KHR)
                device_proc_address(device, "vkCmdPipelineBarrier2KHR");
            device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2KHR)
                device_proc_address(device, "vkCmdResetEvent2KHR");
            device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2KHR)
                device_proc_address(device, "vkCmdSetEvent2KHR");
            device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2KHR)
                device_proc_address(device, "vkCmdWaitEvents2KHR");
            device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2KHR)
                device_proc_address(device, "vkCmdWriteTimestamp2KHR");
            device->vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)
                device_proc_address(device, "vkQueueSubmit2KHR");

            if (!device->vkCmdPipelineBarrier2 ||
                !device->vkCmdResetEvent2 ||
                !device->vkCmdSetEvent2 ||
                !device->vkCmdWaitEvents2 ||
                !device->vkCmdWriteTimestamp2 ||
                !device->vkQueueSubmit2)
                return false;
        }
        if (extension_bits & vulkan_ext_maintenance4_bit) {
            device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)
                device_proc_address(device, "vkGetDeviceBufferMemoryRequirementsKHR");
            device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)
                device_proc_address(device, "vkGetDeviceImageMemoryRequirementsKHR");
            device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)
                device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirementsKHR");

            if (!device->vkGetDeviceBufferMemoryRequirements ||
                !device->vkGetDeviceImageMemoryRequirements ||
                !device->vkGetDeviceImageSparseMemoryRequirements)
                return false;
        }
    }
    
    /* core 1.4 */
    if (api_version >= VK_API_VERSION_1_4) {

        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2)
            device_proc_address(device, "vkCmdBindIndexBuffer2");
        device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations)
            device_proc_address(device, "vkCmdSetRenderingAttachmentLocations");
        device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices)
            device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndices");
        device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout)
            device_proc_address(device, "vkGetDeviceImageSubresourceLayout");
        device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2)
            device_proc_address(device, "vkGetImageSubresourceLayout2");
        device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity)
            device_proc_address(device, "vkGetRenderingAreaGranularity");

        if (!device->vkCmdBindIndexBuffer2 ||
            !device->vkCmdSetRenderingAttachmentLocations ||
            !device->vkCmdSetRenderingInputAttachmentIndices ||
            !device->vkGetDeviceImageSubresourceLayout ||
            !device->vkGetImageSubresourceLayout2 ||
            !device->vkGetRenderingAreaGranularity)
            return false;

    } else { /* backwards compatibility 1.4 */

        if (extension_bits & vulkan_ext_dynamic_rendering_local_read_bit) {
            device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)
                device_proc_address(device, "vkCmdSetRenderingAttachmentLocationsKHR");
            device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)
                device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndicesKHR");

            if (!device->vkCmdSetRenderingAttachmentLocations || !device->vkCmdSetRenderingInputAttachmentIndices)
                return false;
        }
        if (extension_bits & vulkan_ext_maintenance5_bit) {
            device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)
                device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
            device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)
                device_proc_address(device, "vkGetDeviceImageSubresourceLayoutKHR");
            device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)
                device_proc_address(device, "vkGetImageSubresourceLayout2KHR");
            device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)
                device_proc_address(device, "vkGetRenderingAreaGranularityKHR");

            if (!device->vkCmdBindIndexBuffer2 ||
                !device->vkGetDeviceImageSubresourceLayout ||
                !device->vkGetImageSubresourceLayout2 ||
                !device->vkGetRenderingAreaGranularity)
                return false;
        }
    }

    /* swapchain */
    if (extension_bits & vulkan_ext_swapchain_bit) {

        device->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)
            device_proc_address(device, "vkAcquireNextImageKHR");
        device->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)
            device_proc_address(device, "vkAcquireNextImage2KHR");
        device->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)
            device_proc_address(device, "vkCreateSwapchainKHR");
        device->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)
            device_proc_address(device, "vkDestroySwapchainKHR");
        device->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
            device_proc_address(device, "vkGetSwapchainImagesKHR");
        device->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)
            device_proc_address(device, "vkQueuePresentKHR");

        if (!device->vkAcquireNextImageKHR ||
            !device->vkAcquireNextImage2KHR ||
            !device->vkCreateSwapchainKHR ||
            !device->vkDestroySwapchainKHR ||
            !device->vkGetSwapchainImagesKHR ||
            !device->vkQueuePresentKHR)
            return false;
    }

    /* device fault */
    if (extension_bits & vulkan_ext_device_fault_bit) {

        device->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)
            device_proc_address(device, "vkGetDeviceFaultInfoEXT");
        
        if (!device->vkGetDeviceFaultInfoEXT)
            return false;
    }

    /* mesh shader */
    if (extension_bits & vulkan_ext_mesh_shader_bit) {

        device->vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)
            device_proc_address(device, "vkCmdDrawMeshTasksEXT");
        device->vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT)
            device_proc_address(device, "vkCmdDrawMeshTasksIndirectEXT");
        device->vkCmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)
            device_proc_address(device, "vkCmdDrawMeshTasksIndirectCountEXT");

        if (!device->vkCmdDrawMeshTasksEXT ||
            !device->vkCmdDrawMeshTasksIndirectEXT ||
            !device->vkCmdDrawMeshTasksIndirectCountEXT)
            return false;
    }

    /* deferred host operations */
    if (extension_bits & vulkan_ext_deferred_host_operations_bit) {

        device->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)
            device_proc_address(device, "vkCreateDeferredOperationKHR");
        device->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)
            device_proc_address(device, "vkDeferredOperationJoinKHR");
        device->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)
            device_proc_address(device, "vkDestroyDeferredOperationKHR");
        device->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)
            device_proc_address(device, "vkGetDeferredOperationMaxConcurrencyKHR");
        device->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)
            device_proc_address(device, "vkGetDeferredOperationResultKHR");

        if (!device->vkCreateDeferredOperationKHR ||
            !device->vkDeferredOperationJoinKHR ||
            !device->vkDestroyDeferredOperationKHR ||
            !device->vkGetDeferredOperationMaxConcurrencyKHR ||
            !device->vkGetDeferredOperationResultKHR)
            return false;
    }

    /* acceleration structure */
    if (extension_bits & vulkan_ext_acceleration_structure_bit) {

        device->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)
            device_proc_address(device, "vkBuildAccelerationStructuresKHR");
        device->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)
            device_proc_address(device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        device->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)
            device_proc_address(device, "vkCmdBuildAccelerationStructuresKHR");
        device->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)
            device_proc_address(device, "vkCmdCopyAccelerationStructureKHR");
        device->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)
            device_proc_address(device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        device->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)
            device_proc_address(device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        device->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)
            device_proc_address(device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        device->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)
            device_proc_address(device, "vkCopyAccelerationStructureKHR");
        device->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)
            device_proc_address(device, "vkCopyAccelerationStructureToMemoryKHR");
        device->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)
            device_proc_address(device, "vkCopyMemoryToAccelerationStructureKHR");
        device->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)
            device_proc_address(device, "vkCreateAccelerationStructureKHR");
        device->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)
            device_proc_address(device, "vkDestroyAccelerationStructureKHR");
        device->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)
            device_proc_address(device, "vkGetAccelerationStructureBuildSizesKHR");
        device->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)
            device_proc_address(device, "vkGetAccelerationStructureDeviceAddressKHR");
        device->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)
            device_proc_address(device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        device->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)
            device_proc_address(device, "vkWriteAccelerationStructuresPropertiesKHR");

        if (!device->vkBuildAccelerationStructuresKHR ||
            !device->vkCmdBuildAccelerationStructuresIndirectKHR ||
            !device->vkCmdBuildAccelerationStructuresKHR ||
            !device->vkCmdCopyAccelerationStructureKHR ||
            !device->vkCmdCopyAccelerationStructureToMemoryKHR ||
            !device->vkCmdCopyMemoryToAccelerationStructureKHR ||
            !device->vkCmdWriteAccelerationStructuresPropertiesKHR ||
            !device->vkCopyAccelerationStructureKHR ||
            !device->vkCopyAccelerationStructureToMemoryKHR ||
            !device->vkCopyMemoryToAccelerationStructureKHR ||
            !device->vkCreateAccelerationStructureKHR ||
            !device->vkDestroyAccelerationStructureKHR ||
            !device->vkGetAccelerationStructureBuildSizesKHR ||
            !device->vkGetAccelerationStructureDeviceAddressKHR ||
            !device->vkGetDeviceAccelerationStructureCompatibilityKHR ||
            !device->vkWriteAccelerationStructuresPropertiesKHR)
            return false;
    }

    /* raytracing pipeline */
    if (extension_bits & vulkan_ext_raytracing_pipeline_bit) {

        device->vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)
            device_proc_address(device, "vkCmdSetRayTracingPipelineStackSizeKHR");
        device->vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)
            device_proc_address(device, "vkCmdTraceRaysIndirectKHR");
        device->vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)
            device_proc_address(device, "vkCmdTraceRaysKHR");
        device->vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)
            device_proc_address(device, "vkCreateRayTracingPipelinesKHR");
        device->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)
            device_proc_address(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)
            device_proc_address(device, "vkGetRayTracingShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)
            device_proc_address(device, "vkGetRayTracingShaderGroupStackSizeKHR");

        if (!device->vkCmdSetRayTracingPipelineStackSizeKHR ||
            !device->vkCmdTraceRaysIndirectKHR ||
            !device->vkCmdTraceRaysKHR ||
            !device->vkCreateRayTracingPipelinesKHR ||
            !device->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR ||
            !device->vkGetRayTracingShaderGroupHandlesKHR ||
            !device->vkGetRayTracingShaderGroupStackSizeKHR)
            return false;
    }

    /* raytracing maintenance */
    if (extension_bits & vulkan_ext_raytracing_maintenance1_bit) {

        device->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)
            device_proc_address(device, "vkCmdTraceRaysIndirect2KHR");

        if (!device->vkCmdTraceRaysIndirect2KHR)
            return false;
    }

    /* video coding */
    if (extension_bits & (vulkan_ext_video_queue_bit | vulkan_ext_video_decode_queue_bit)) {

        device->vkBindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR)
            device_proc_address(device, "vkBindVideoSessionMemoryKHR");
        device->vkCmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR)
            device_proc_address(device, "vkCmdBeginVideoCodingKHR");
        device->vkCmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR)
            device_proc_address(device, "vkCmdControlVideoCodingKHR");
        device->vkCmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR)
            device_proc_address(device, "vkCmdDecodeVideoKHR");
        device->vkCmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR)
            device_proc_address(device, "vkCmdEndVideoCodingKHR");
        device->vkCreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR)
            device_proc_address(device, "vkCreateVideoSessionKHR");
        device->vkCreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR)
            device_proc_address(device, "vkCreateVideoSessionParametersKHR");
        device->vkDestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR)
            device_proc_address(device, "vkDestroyVideoSessionKHR");
        device->vkDestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR)
            device_proc_address(device, "vkDestroyVideoSessionParametersKHR");
        device->vkGetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR)
            device_proc_address(device, "vkGetVideoSessionMemoryRequirementsKHR");
        device->vkUpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR)
            device_proc_address(device, "vkUpdateVideoSessionParametersKHR");

        if (!device->vkBindVideoSessionMemoryKHR ||
            !device->vkCmdBeginVideoCodingKHR ||
            !device->vkCmdControlVideoCodingKHR ||
            !device->vkCmdDecodeVideoKHR ||
            !device->vkCmdEndVideoCodingKHR ||
            !device->vkCreateVideoSessionKHR ||
            !device->vkCreateVideoSessionParametersKHR ||
            !device->vkDestroyVideoSessionKHR ||
            !device->vkDestroyVideoSessionParametersKHR ||
            !device->vkGetVideoSessionMemoryRequirementsKHR ||
            !device->vkUpdateVideoSessionParametersKHR)
            return false;
    }

    return true;
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

static void create_validation_layers(struct reznor *reznor)
{
    if (reznor->instance == VK_NULL_HANDLE || reznor->debug_messenger) 
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
        .pUserData = NULL,
    };

    if (reznor->vkCreateDebugUtilsMessengerEXT(reznor->instance, &messenger_info, &reznor->allocator, &reznor->debug_messenger) == VK_SUCCESS)
        log_verbose("Created Vulkan validation layers.");
}

struct allocation_header {
    void *outer; /**< Unaligned pointer returned by malloc(). */
    usize size;  /**< Original size requested in the allocation call. */
};

static void *fn_allocation(
    void                       *pUserData, 
    usize                       size, 
    usize                       alignment, 
    VkSystemAllocationScope     allocationScope)
{
    (void)pUserData;
    (void)allocationScope;

    assert_debug(alignment != 0 && !(alignment & (alignment - 1)));

    /* the spec requires a return value of NULL when size is 0 */
    if (size == 0) 
        return NULL;

    void *outer = malloc(alignment + sizeof(struct allocation_header) + size);
    if (!outer)
        return NULL;

    /* add enough space for the header, then round up to alignment */
    uptr inner = ((uptr)outer + sizeof(struct allocation_header) + alignment - 1) & ~(alignment - 1);

    struct allocation_header header = {
        .outer = outer,
        .size = size,
    };

    /* store the header just before inner */
    memcpy((void *)(inner - sizeof(struct allocation_header)), &header, sizeof(struct allocation_header));

    return (void *)inner;
}

static void fn_free(void *pUserData, void *pMemory)
{
    (void)pUserData;

    uptr inner = (uptr)pMemory;

    struct allocation_header header;
    memcpy(&header, (void *)(inner - sizeof(struct allocation_header)), sizeof(struct allocation_header));

    free(header.outer);
}

static void *fn_reallocation(
    void                       *pUserData, 
    void                       *pOriginal, 
    usize                       size, 
    usize                       alignment, 
    VkSystemAllocationScope     allocationScope)
{
    (void)pUserData;
    (void)allocationScope;

    assert_debug(alignment != 0 && !(alignment & (alignment - 1)));

    if (pOriginal == NULL)
        return fn_allocation(pUserData, size, alignment, allocationScope);

    if (size == 0) {
        fn_free(pUserData, pOriginal);
        return NULL;
    }

    uptr inner = (uptr)pOriginal;

    /* header of the original allocation */
    struct allocation_header header;
    memcpy(&header, (void *)(inner - sizeof(struct allocation_header)), sizeof(struct allocation_header));

    /* If we can be certain that realloc will return a correctly-aligned pointer (which typically 
     * means alignment <= alignof(double)) then it's most efficiently to simply use that.
     *
     * Otherwise, we have no choice but to allocate a fresh buffer and copy the data across.
     * We can't speculatively try a realloc and hope that it just shrinks the buffer and preserves 
     * alignment - the problem is that if realloc breaks the alignment, and we need to fall back 
     * to the fresh-buffer-and-copy method, but the fresh allocation fails, we will have already 
     * freed the original buffer (in realloc). We can only legally return NULL if we guarantee
     * the original buffer is still valid. */
    static const usize min_realloc_alignment = _Alignof(double);

    if (alignment <= min_realloc_alignment) {
        void *new_outer = realloc(header.outer, alignment + sizeof(struct allocation_header) + size);
        if (!new_outer)
            return NULL;

        /* verify realloc returned the alignment we expected */
        assert_debug(((uptr)new_outer & (alignment - 1)) == 0);

        /* realloc already copied the inner contents, we just need to update the header */
        uptr new_inner = ((uptr)new_outer + sizeof(struct allocation_header) + alignment - 1) & ~(alignment - 1);

        /* double-check */
        assert_debug(new_inner - inner == (uptr)new_outer - (uptr)header.outer);

        header.outer = new_outer;
        header.size = size;

        /* store the updated header */
        memcpy((void *)(new_inner - sizeof(struct allocation_header)), &header, sizeof(struct allocation_header));

        return (void *)new_inner;
    } else {
        /* get a totally new aligned buffer */
        void *new_inner = fn_allocation(pUserData, size, alignment, allocationScope);
        if (!new_inner)
            return NULL;

        /* copy the inner buffer */
        memcpy(new_inner, (void *)inner, min(size, header.size));

        /* release the original buffer */
        free(header.outer);

        return new_inner;
    }
}

static void vulkan_interface_fini(struct reznor *reznor)
{
    if (!reznor) return;

    if (reznor->module)
        process_close_dll(reznor->module);
    zerop(reznor);
    g_vulkan = NULL;
}

void vulkan_write_allocation_callbacks(VkAllocationCallbacks *allocator, const char *userdata)
{
    allocator->pfnAllocation = fn_allocation;
    allocator->pfnReallocation = fn_reallocation;
    allocator->pfnFree = fn_free;
    allocator->pfnInternalAllocation = NULL;
    allocator->pfnInternalFree = NULL;
    allocator->pUserData = (void *)userdata;
};

RIVEN_ENCORE(reznor, vulkan)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    const char *fn = "'reznor_vulkan'";
    u32 instance_version = 0;
    u32 extension_bits = 0;
    u32 extension_count = 0;
    u32 layer_count = 0;
    u32 physical_device_count = 0;
    void *module_vulkan = NULL;

    /* only one vulkan backend at a time is allowed, so the interface will be shared */
    if (UNLIKELY(g_vulkan != NULL)) {
        *encore->header.interface = (riven_argument_t)g_vulkan;
        return;
    }
#if defined(PLATFORM_WINDOWS)
    module_vulkan = process_load_dll("vulkan-1.dll");
#elif defined(PLATFORM_APPLE)
    module_vulkan = process_load_dll("libvulkan.dylib");
    if (!module_vulkan)
        module_vulkan = process_load_dll("libvulkan.1.dylib");
    if (!module_vulkan)
        module_vulkan = process_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (!module_vulkan)
        module_vulkan = process_load_dll("vulkan.framework/vulkan");
    if (!module_vulkan)
        module_vulkan = process_load_dll("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (!module_vulkan)
        module_vulkan = process_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    module_vulkan = process_load_dll("libvulkan.so.1");
    if (!module_vulkan)
        module_vulkan = process_load_dll("libvulkan.so");
#endif
    if (!module_vulkan) {
        log_error("%s can't open the Vulkan drivers, ensure they are correclty installed and available via the system PATH", fn);
        return;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)process_get_address(module_vulkan, "vkGetInstanceProcAddr");
    if (vkGetInstanceProcAddr == NULL) {
        log_error("%s can't get the address of vkGetInstanceProcAddr from Vulkan drivers.", fn);
        process_close_dll(module_vulkan);
        return;
    }

    PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)
        vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)
        vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)
        vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");

    if (!vkCreateInstance || 
        !vkEnumerateInstanceVersion || 
        !vkEnumerateInstanceExtensionProperties || 
        !vkEnumerateInstanceLayerProperties) 
    {
        log_error("%s can't get addresses of global procedures from Vulkan drivers.", fn);
        process_close_dll(module_vulkan);
        return;
    }

    /* check the API version */
    vkEnumerateInstanceVersion(&instance_version);
    if (instance_version < VK_API_VERSION_1_2) {
        log_error("%s outdated drivers. We target a minimum of Vulkan 1.2 core."
                  "Your drivers API version is %u.%u.%u, please update your drivers.",
                  fn, (instance_version >> 22u), (instance_version >> 12u) & 0x3ffu, (instance_version & 0xfffu));
        process_close_dll(module_vulkan);
        return;
    }
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    /* check instance extensions */
    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (!extension_count) {
        log_error("%s no instance extensions available, can't continue.", fn);
        process_close_dll(module_vulkan);
        return;
    }

    VkExtensionProperties *extension_properties = (VkExtensionProperties *)
        riven_alloc(encore->header.riven, riven_tag_deferred, sizeof(VkExtensionProperties) * extension_count, _Alignof(VkExtensionProperties));
    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_properties));

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
    if (encore->debug_utils && vulkan_query_extension(extension_properties, extension_count, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        extension_bits |= vulkan_ext_debug_utils_bit;

    u32 o = 0;
    extension_count = bits_popcnt_lookup((const u8 *)&extension_bits, sizeof(extension_bits));
    const char **extensions = (const char **)
        riven_alloc(encore->header.riven, riven_tag_deferred, sizeof(const char *) * extension_count, _Alignof(const char *));
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
    assert_debug(extension_count == o);

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = encore->header.metadata->game_name.ptr,
        .applicationVersion = encore->header.metadata->game_build_version,
        .pEngineName = encore->header.metadata->engine_name.ptr,
        .engineVersion = encore->header.metadata->engine_build_version,
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

    if (layer_count && extension_count & vulkan_ext_debug_utils_bit) {
        extension_bits |= vulkan_ext_layer_validation_bit;
        instance_info.pNext = &validation_features;
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }
    struct reznor *reznor = (struct reznor *)
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct reznor), _Alignof(struct reznor));
    zerop(reznor);

    reznor->vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    reznor->vkCreateInstance = vkCreateInstance;
    reznor->vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;
    reznor->vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    reznor->vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    reznor->module = module_vulkan;
    reznor->extension_bits = extension_bits;

    /* write the interface header */
    reznor->interface.header.name = str_init("reznor_vulkan");
    reznor->interface.header.riven = encore->header.riven;
    reznor->interface.header.tag = encore->header.tag;
    reznor->interface.header.fini = (PFN_riven_work)vulkan_interface_fini;
    g_vulkan = reznor;

    vulkan_write_allocation_callbacks(&reznor->allocator, reznor->interface.header.name.ptr);

    VERIFY_VK(vkCreateInstance(&instance_info, &reznor->allocator, &reznor->instance));

    if (!vulkan_load_instance_procedures(reznor, instance_version, extension_bits)) {
        vulkan_interface_fini(reznor);
        return;
    };

    if (extension_bits & vulkan_ext_layer_validation_bit)
        create_validation_layers(reznor);

    /* query physical devices */
    VERIFY_VK(reznor->vkEnumeratePhysicalDevices(reznor->instance, &physical_device_count, NULL));
    if (physical_device_count == 0) {
        log_error("%s no physical devices are available to Vulkan.", fn);
        vulkan_interface_fini(reznor);
        return;
    }
    usize physical_devices_bytes = A8(sizeof(VkPhysicalDevice) * physical_device_count);
    usize query_work_bytes = A8(sizeof(struct vulkan_physical_device_query_work) * physical_device_count);
    usize riven_work_bytes = A8(sizeof(struct riven_work) * physical_device_count);
    usize indices_bytes = A8(sizeof(u32) * physical_device_count);
    usize total_bytes = physical_devices_bytes + query_work_bytes + riven_work_bytes + indices_bytes;

    u8 *raw = (u8 *)riven_alloc(encore->header.riven, riven_tag_deferred, total_bytes, 8);
    memset(raw, 0, total_bytes);

    o = 0;
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)&raw[o];
    o += physical_devices_bytes;
    struct vulkan_physical_device_query_work *query_work = (struct vulkan_physical_device_query_work *)&raw[o];
    o += query_work_bytes;
    struct riven_work *riven_work = (struct riven_work *)&raw[o];
    o += riven_work_bytes;
    u32 *indices = (u32 *)&raw[o];
    assert_debug(o + indices_bytes == total_bytes);

    /* prepare the physical device query */
    VERIFY_VK(reznor->vkEnumeratePhysicalDevices(reznor->instance, &physical_device_count, physical_devices));

    for (u32 i = 0; i < physical_device_count; i++) {
        query_work[i].header.index = i;
        query_work[i].header.result = result_error;
        query_work[i].physical.device = physical_devices[i];
        query_work[i].reznor = reznor;
        riven_work[i].procedure = (PFN_riven_work)vulkan_physical_device_query;
        riven_work[i].argument = (riven_argument_t)&query_work[i];
        riven_work[i].name = str_init("reznor_vulkan:physical_device_query");
    }
    riven_split_work_and_unchain(encore->header.riven, riven_work, physical_device_count);

    o = 0;
    /* collect indices of accepted physical devices */
    for (u32 i = 0; i < physical_device_count; i++)
        if (query_work[i].header.result == result_success)
            indices[o++] = i;
    if (o == 0) {
        log_error("%s physical device query invalidated all of %u physical devices available to Vulkan.", fn, physical_device_count);
        vulkan_interface_fini(reznor);
        return;
    }

    /* copy the accepted physical devices */
    struct vulkan_physical_device *devices = (struct vulkan_physical_device *)
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct vulkan_physical_device), _Alignof(struct vulkan_physical_device));
    for (u32 i = 0; i < o; i++)
        memcpy(&devices[i], &query_work[indices[i]].physical, sizeof(struct vulkan_physical_device));
    reznor->physical_devices = devices;
    reznor->physical_device_count = o;

    /* write the interface */
    reznor->interface.device_query = _reznor_vulkan_device_query;
    reznor->interface.device_create = _reznor_vulkan_device_create;
    reznor->interface.device_destroy = _reznor_vulkan_device_destroy;
    reznor->interface.device_memory_assembly = _reznor_vulkan_device_memory_assembly;
    reznor->interface.buffer_assembly = _reznor_vulkan_buffer_assembly;
    reznor->interface.texture_assembly = _reznor_vulkan_texture_assembly;
    reznor->interface.sampler_assembly = _reznor_vulkan_sampler_assembly;
    reznor->interface.descriptor_set_layout_assembly = _reznor_vulkan_descriptor_set_layout_assembly;
    reznor->interface.descriptor_set_assembly = _reznor_vulkan_descriptor_set_assembly;
    reznor->interface.pipeline_layout_assembly = _reznor_vulkan_pipeline_layout_assembly;
    reznor->interface.graphics_pipeline_assembly = _reznor_vulkan_graphics_pipeline_assembly;
    reznor->interface.compute_pipeline_assembly = _reznor_vulkan_compute_pipeline_assembly;
    reznor->interface.raytracing_pipeline_assembly = _reznor_vulkan_raytracing_pipeline_assembly;
    reznor->interface.shader_binding_table_assembly = _reznor_vulkan_shader_binding_table_assembly;
    reznor->interface.bottom_level_assembly = _reznor_vulkan_bottom_level_assembly;
    reznor->interface.top_level_assembly = _reznor_vulkan_top_level_assembly;
    reznor->interface.query_pool_assembly = _reznor_vulkan_query_pool_assembly;
    reznor->interface.swapchain_assembly = _reznor_vulkan_swapchain_assembly;
    reznor->interface.swapchain_try_recreate = _reznor_vulkan_swapchain_try_recreate;
    reznor->interface.swapchain_next_image = _reznor_vulkan_swapchain_next_image;
    reznor->interface.disassembly = _reznor_vulkan_disassembly;
    reznor->interface.frame_begin = _reznor_vulkan_frame_begin;
    reznor->interface.frame_submit = _reznor_vulkan_frame_submit;
    reznor->interface.command_draw = _reznor_vulkan_command_draw;
    reznor->interface.command_draw_indexed = _reznor_vulkan_command_draw_indexed;
    reznor->interface.command_draw_indexed_indirect = _reznor_vulkan_command_draw_indexed_indirect;
    reznor->interface.command_draw_indirect = _reznor_vulkan_command_draw_indirect;
    reznor->interface.command_dispatch = _reznor_vulkan_command_dispatch;
    reznor->interface.command_dispatch_indirect = _reznor_vulkan_command_dispatch_indirect;
    reznor->interface.command_copy_buffer = _reznor_vulkan_command_copy_buffer;
    reznor->interface.command_copy_texture = _reznor_vulkan_command_copy_texture;
    reznor->interface.command_begin_render_pass = _reznor_vulkan_command_begin_render_pass;
    reznor->interface.command_end_render_pass = _reznor_vulkan_command_end_render_pass;

    *encore->header.interface = (riven_argument_t)reznor;
    log_verbose("'%s' interface write.", reznor->interface.header.name.ptr);
}
