#include "vk_xaku.h"
#ifdef XAKU_VULKAN

#include <amwe/math/bits.h>
#include <string.h> /* strcmp */

enum instance_extension_bits {
    instance_extension_khr_surface              = (1u << 0),    /**< VK_KHR_surface */
    instance_extension_khr_win32_surface        = (1u << 1),    /**< VK_KHR_win32_surface */
    instance_extension_ext_metal_surface        = (1u << 2),    /**< VK_EXT_metal_surface */
    instance_extension_khr_android_surface      = (1u << 3),    /**< VK_KHR_android_surface */
    instance_extension_khr_wayland_surface      = (1u << 4),    /**< VK_KHR_wayland_surface */
    instance_extension_khr_xcb_surface          = (1u << 5),    /**< VK_KHR_xcb_surface */
    instance_extension_khr_headless_surface     = (1u << 6),    /**< VK_KHR_headless_surface */
    instance_extension_khr_display              = (1u << 7),    /**< VK_KHR_display */
    instance_extension_ext_debug_utils          = (1u << 8),    /**< VK_EXT_debug_utils */
    instance_extension_count = 9,
    instance_extension_layer_validation         = (1u << 9),    /**< VK_LAYER_KHRONOS_validation */
};

static const char *g_instance_extension_names[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_EXT_metal_surface",
    "VK_KHR_android_surface",
    "VK_KHR_wayland_surface",
    "VK_KHR_xcb_surface",
    "VK_KHR_headless_surface",
    "VK_KHR_display",
    "VK_EXT_debug_utils",
};
bedrock_static_assert(lake_arraysize(g_instance_extension_names) == instance_extension_count, "The number of device extensions and their names count must match");

enum device_extension_bits {
    device_extension_khr_swapchain                             = (1ull << 0),  /**< VK_KHR_swapchain */
    device_extension_ext_device_fault                          = (1ull << 1),  /**< VK_EXT_device_fault */
    device_extension_ext_memory_budget                         = (1ull << 2),  /**< VK_EXT_memory_budget */
    device_extension_ext_memory_priority                       = (1ull << 3),  /**< VK_EXT_memory_priority */
    device_extension_ext_mesh_shader                           = (1ull << 4),  /**< VK_EXT_mesh_shader */
    device_extension_khr_fragment_shading_rate                 = (1ull << 5),  /**< VK_KHR_fragment_shading_rate */
    device_extension_khr_deferred_host_operations              = (1ull << 6),  /**< VK_KHR_deferred_host_operations */
    device_extension_khr_acceleration_structure                = (1ull << 7),  /**< VK_KHR_acceleration_structure */
    device_extension_khr_pipeline_library                      = (1ull << 8),  /**< VK_KHR_pipeline_library */
    device_extension_khr_ray_query                             = (1ull << 9),  /**< VK_KHR_ray_query */
    device_extension_khr_ray_tracing_pipeline                  = (1ull << 10), /**< VK_KHR_ray_tracing_pipeline */
    device_extension_khr_ray_tracing_maintenance1              = (1ull << 11), /**< VK_KHR_ray_tracing_maintenance1 */
    device_extension_khr_ray_tracing_position_fetch            = (1ull << 12), /**< VK_KHR_ray_tracing_position_fetch */
    device_extension_khr_video_queue                           = (1ull << 13), /**< VK_KHR_video_queue */
    device_extension_khr_video_decode_queue                    = (1ull << 14), /**< VK_KHR_video_decode_queue */
    device_extension_khr_video_decode_av1                      = (1ull << 15), /**< VK_KHR_video_decode_av1 */
    device_extension_khr_video_decode_h264                     = (1ull << 16), /**< VK_KHR_video_decode_h264 */
    device_extension_khr_video_maintenance1                    = (1ull << 17), /**< VK_KHR_video_maintenance1 */
    device_extension_ext_extended_dynamic_state3               = (1ull << 18), /**< VK_EXT_extended_dynamic_state3 */
    device_extension_ext_robustness2                           = (1ull << 19), /**< VK_EXT_robustness2 */
    device_extension_ext_shader_image_atomic_int64             = (1ull << 20), /**< VK_EXT_shader_image_atomic_int64 */
    device_extension_ext_shader_atomic_float                   = (1ull << 21), /**< VK_EXT_shader_atomic_float */
    device_extension_ext_conservative_rasterization            = (1ull << 22), /**< VK_EXT_conservative_rasterization */
    device_extension_ext_dynamic_rendering_unused_attachments  = (1ull << 23), /**< VK_EXT_dynamic_rendering_unused_attachments */
    /* NVIDIA hardware */
    device_extension_nv_ray_tracing_invocation_reorder         = (1ull << 24), /**< VK_NV_ray_tracing_invocation_reorder */
    /* AMD hardware */
    device_extension_amd_device_coherent_memory                = (1ull << 25), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    device_extension_khr_dynamic_rendering_local_read          = (1ull << 26), /**< VK_KHR_dynamic_rendering_local_read */
    device_extension_khr_maintenance5                          = (1ull << 27), /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    device_extension_khr_dynamic_rendering                     = (1ull << 28), /**< VK_KHR_dynamic_rendering */
    device_extension_khr_synchronization2                      = (1ull << 29), /**< VK_KHR_synchronization2 */
    device_extension_khr_maintenance4                          = (1ull << 30), /**< VK_KHR_maintenance4 */
    /** if this exceeds 64, we may need to split the values holding extension bits */
    device_extension_count = 31,
};

static const char *g_device_extension_names[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DEVICE_FAULT_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
    VK_EXT_MESH_SHADER_EXTENSION_NAME,
    VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
    VK_KHR_RAY_QUERY_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME,
    VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME,
    VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
    VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
    VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
    VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
    VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
    VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME,
    VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME,
    VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
    VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
};
bedrock_static_assert(lake_arraysize(g_device_extension_names) == device_extension_count, "The number of device extensions and their names count must match");

const char *vulkan_result_string(VkResult result)
{
    switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "host memory allocation has failed";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "device memory allocation has failed";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "initialization of an object could not be completed for implementation-specific reasons";
		case VK_ERROR_DEVICE_LOST:
			return "the logical or physical device has been lost";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "mapping of a memory object has failed";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "a requested layer is not present or could not be loaded";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "a requested extension is not supported";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "a requested feature is not supported";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "the requested version of Vulkan is not supported by the driver or is otherwise "
			       "incompatible for implementation-specific reasons";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "too many objects of the type have already been created";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "a requested format is not supported on this device";
		case VK_ERROR_FRAGMENTED_POOL:
			return "a pool allocation has failed due to fragmentation of the pool's memory";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "a pool memory allocation has failed";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "an external handle is not a valid handle of the specified type";
		case VK_ERROR_FRAGMENTATION:
			return "a descriptor pool creation has failed due to fragmentation";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "a buffer creation or memory allocation failed because the requested address is not available";
		case VK_PIPELINE_COMPILE_REQUIRED:
			return "a requested pipeline creation would have required compilation, but the application requested compilation to not be performed";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "a surface is no longer available";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "the requested window is already in use by Vulkan or another API in a manner which prevents it from being used again";
		case VK_SUBOPTIMAL_KHR:
			return "a swapchain no longer matches the surface properties exactly, but can still be used to present"
			       "to the surface successfully";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "a surface has changed in such a way that it is no longer compatible with the swapchain, "
			       "any further presentation requests using the swapchain will fail";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "the display used by a swapchain does not use the same presentable image layout, or is "
			       "incompatible in a way that prevents sharing an image";
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
			       "it did not have exlusive full-screen access";
		case VK_THREAD_IDLE_KHR:
			return "a deferred operation is not complete but there is currently no work for this thread to do at the time of this call";
		case VK_THREAD_DONE_KHR:
			return "a deferred operation is not complete but there is no work remaining to assign to additional threads";
		case VK_OPERATION_DEFERRED_KHR:
			return "a deferred operation was requested and at least some of the work was deferred";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "a deferred operation was requested and no operations were deferred";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "an image creation failed because internal resources required for compression are exhausted."
			       "this must only be returned when fixed-rate compression is requested";
        case VK_INCOMPLETE:
            return "incomplete";
        case VK_SUCCESS:
            return "success";
        default:
            return "an unknown error has occured";
    }
}

static bool query_extension(VkExtensionProperties *properties, u32 count, const char *ext)
{
    for (u32 i = 0; i < count; i++)
        if (!strcmp(properties[i].extensionName, ext))
            return true;
    return false;
}

static PFN_vkVoidFunction instance_proc_address(const struct xaku_encore *xaku, const char *procname)
{
    PFN_vkVoidFunction address = xaku->vkGetInstanceProcAddr(xaku->instance, procname);
    if (address == NULL)
        bedrock_log_warn("'xaku_vulkan' can't find Vulkan instance procedure '%s'.", procname);
    return address;
}

static PFN_vkVoidFunction device_proc_address(const struct xaku_device *device, const char *procname)
{
    PFN_vkVoidFunction address = device->xaku->vkGetDeviceProcAddr(device->logical, procname);
    if (address == NULL)
        bedrock_log_warn("'xaku_vulkan' can't find Vulkan device procedure '%s'.", procname);
    return address;
}

static bool load_instance_procedures(struct xaku_encore *xaku, u32 extension_bits)
{
    bedrock_assert_debug(xaku && xaku->vkGetInstanceProcAddr);

    xaku->vkCreateDevice = (PFN_vkCreateDevice)
        instance_proc_address(xaku, "vkCreateDevice");
    xaku->vkDestroyInstance = (PFN_vkDestroyInstance)
        instance_proc_address(xaku, "vkDestroyInstance");
    xaku->vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)
        instance_proc_address(xaku, "vkEnumerateDeviceExtensionProperties");
    xaku->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        instance_proc_address(xaku, "vkEnumeratePhysicalDevices");
    xaku->vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups)
        instance_proc_address(xaku, "vkEnumeratePhysicalDeviceGroups");
    xaku->vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)
        instance_proc_address(xaku, "vkGetDeviceProcAddr");
    xaku->vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)
        instance_proc_address(xaku, "vkGetPhysicalDeviceFeatures");
    xaku->vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceFeatures2");
    xaku->vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceFormatProperties");
    xaku->vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceFormatProperties2");
    xaku->vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceImageFormatProperties");
    xaku->vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceImageFormatProperties2");
    xaku->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceMemoryProperties");
    xaku->vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceMemoryProperties2");
    xaku->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceProperties");
    xaku->vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceProperties2");
    xaku->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceQueueFamilyProperties");
    xaku->vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceQueueFamilyProperties2");
    xaku->vkGetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceSparseImageFormatProperties");
    xaku->vkGetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2)
        instance_proc_address(xaku, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    xaku->vkGetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties)
        instance_proc_address(xaku, "vkGetPhysicalDeviceToolProperties");

    if (!xaku->vkCreateDevice ||
        !xaku->vkDestroyInstance ||
        !xaku->vkEnumerateDeviceExtensionProperties ||
        !xaku->vkEnumeratePhysicalDevices ||
        !xaku->vkEnumeratePhysicalDeviceGroups ||
        !xaku->vkGetDeviceProcAddr ||
        !xaku->vkGetPhysicalDeviceFeatures ||
        !xaku->vkGetPhysicalDeviceFeatures2 ||
        !xaku->vkGetPhysicalDeviceFormatProperties ||
        !xaku->vkGetPhysicalDeviceFormatProperties2 ||
        !xaku->vkGetPhysicalDeviceImageFormatProperties ||
        !xaku->vkGetPhysicalDeviceImageFormatProperties2 ||
        !xaku->vkGetPhysicalDeviceMemoryProperties ||
        !xaku->vkGetPhysicalDeviceMemoryProperties2 ||
        !xaku->vkGetPhysicalDeviceProperties ||
        !xaku->vkGetPhysicalDeviceProperties2 ||
        !xaku->vkGetPhysicalDeviceQueueFamilyProperties ||
        !xaku->vkGetPhysicalDeviceQueueFamilyProperties2 ||
        !xaku->vkGetPhysicalDeviceSparseImageFormatProperties ||
        !xaku->vkGetPhysicalDeviceSparseImageFormatProperties2 ||
        !xaku->vkGetPhysicalDeviceToolProperties)
        return false;

    /* VK_KHR_surface */
    if (extension_bits & instance_extension_khr_surface) {
        xaku->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)
            instance_proc_address(xaku, "vkDestroySurfaceKHR");
        xaku->vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
            instance_proc_address(xaku, "vkGetPhysicalDeviceSurfaceSupportKHR");
        xaku->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
            instance_proc_address(xaku, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        xaku->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
            instance_proc_address(xaku, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        xaku->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
            instance_proc_address(xaku, "vkGetPhysicalDeviceSurfacePresentModesKHR");

        if (!xaku->vkDestroySurfaceKHR ||
            !xaku->vkGetPhysicalDeviceSurfaceSupportKHR ||
            !xaku->vkGetPhysicalDeviceSurfaceCapabilitiesKHR ||
            !xaku->vkGetPhysicalDeviceSurfaceFormatsKHR ||
            !xaku->vkGetPhysicalDeviceSurfacePresentModesKHR)
            return false;
    }

    /* VK_EXT_debug_utils */
    if (extension_bits & instance_extension_ext_debug_utils) {
        xaku->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)
            instance_proc_address(xaku, "vkSetDebugUtilsObjectNameEXT");
        xaku->vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)
            instance_proc_address(xaku, "vkSetDebugUtilsObjectTagEXT");
        xaku->vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkQueueBeginDebugUtilsLabelEXT");
        xaku->vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkQueueEndDebugUtilsLabelEXT");
        xaku->vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkQueueInsertDebugUtilsLabelEXT");
        xaku->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkCmdBeginDebugUtilsLabelEXT");
        xaku->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkCmdEndDebugUtilsLabelEXT");
        xaku->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)
            instance_proc_address(xaku, "vkCmdInsertDebugUtilsLabelEXT");
        xaku->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
            instance_proc_address(xaku, "vkCreateDebugUtilsMessengerEXT");
        xaku->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
            instance_proc_address(xaku, "vkDestroyDebugUtilsMessengerEXT");
        xaku->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)
            instance_proc_address(xaku, "vkSubmitDebugUtilsMessageEXT");

        if (!xaku->vkSetDebugUtilsObjectNameEXT ||
            !xaku->vkSetDebugUtilsObjectTagEXT ||
            !xaku->vkQueueBeginDebugUtilsLabelEXT ||
            !xaku->vkQueueEndDebugUtilsLabelEXT ||
            !xaku->vkQueueInsertDebugUtilsLabelEXT ||
            !xaku->vkCmdBeginDebugUtilsLabelEXT ||
            !xaku->vkCmdEndDebugUtilsLabelEXT ||
            !xaku->vkCmdInsertDebugUtilsLabelEXT ||
            !xaku->vkCreateDebugUtilsMessengerEXT ||
            !xaku->vkDestroyDebugUtilsMessengerEXT ||
            !xaku->vkSubmitDebugUtilsMessageEXT)
            return false;
    }

    /* VK_KHR_video_queue, optional */
    xaku->vkGetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)
        instance_proc_address(xaku, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    xaku->vkGetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)
        instance_proc_address(xaku, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    xaku->vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR)
        instance_proc_address(xaku, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    return true;
}

static bool load_device_procedures(struct xaku_device *device, u64 extension_bits)
{
    bedrock_assert_debug(device && device->xaku->vkGetDeviceProcAddr);
    
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
        device_proc_address(device, "vkMergePipelineCaches");
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
    if (device->physical_details->xaku_properties.api_version >= VK_API_VERSION_1_3) {
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
    } else { /* backwards compatibility 1.3 */
        u64 required = device_extension_khr_dynamic_rendering | device_extension_khr_synchronization2 | device_extension_khr_maintenance4;
        if ((extension_bits & (required)) != required)
            return false;

        device->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)
            device_proc_address(device, "vkCmdBeginRenderingKHR");
        device->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)
            device_proc_address(device, "vkCmdEndRenderingKHR");
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
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)
            device_proc_address(device, "vkGetDeviceBufferMemoryRequirementsKHR");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)
            device_proc_address(device, "vkGetDeviceImageMemoryRequirementsKHR");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)
            device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    }
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
    
    /* core 1.4 */
    if (device->physical_details->xaku_properties.api_version >= VK_API_VERSION_1_4) {
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
    } else { /* backwards compatibility 1.4 */
        u64 required = device_extension_khr_dynamic_rendering_local_read | device_extension_khr_maintenance5;
        if ((extension_bits & (required)) != required)
            return false;

        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)
            device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
        device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)
            device_proc_address(device, "vkCmdSetRenderingAttachmentLocationsKHR");
        device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)
            device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndicesKHR");
        device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)
            device_proc_address(device, "vkGetDeviceImageSubresourceLayoutKHR");
        device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)
            device_proc_address(device, "vkGetImageSubresourceLayout2KHR");
        device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)
            device_proc_address(device, "vkGetRenderingAreaGranularityKHR");
    }
    if (!device->vkCmdBindIndexBuffer2 ||
        !device->vkCmdSetRenderingAttachmentLocations ||
        !device->vkCmdSetRenderingInputAttachmentIndices ||
        !device->vkGetDeviceImageSubresourceLayout ||
        !device->vkGetImageSubresourceLayout2 ||
        !device->vkGetRenderingAreaGranularity)
        return false;

    /* swapchain */
    if (extension_bits & device_extension_khr_swapchain) {
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
    if (extension_bits & device_extension_ext_device_fault) {
        device->vkGetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT)
            device_proc_address(device, "vkGetDeviceFaultInfoEXT");
        if (!device->vkGetDeviceFaultInfoEXT) return false;
    }

    /* dynamic state 3 */
    if (extension_bits & device_extension_ext_extended_dynamic_state3) {
        device->vkCmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT)
            device_proc_address(device, "vkCmdSetRasterizationSamplesEXT");
        if (!device->vkCmdSetRasterizationSamplesEXT) return false;
    }

    /* mesh shader */
    if (extension_bits & device_extension_ext_mesh_shader) {
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
    if (extension_bits & device_extension_khr_deferred_host_operations) {
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
    if (extension_bits & device_extension_khr_acceleration_structure) {
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
    if (extension_bits & device_extension_khr_ray_tracing_pipeline) {
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
    if (extension_bits & device_extension_khr_ray_tracing_maintenance1) {
        device->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)
            device_proc_address(device, "vkCmdTraceRaysIndirect2KHR");
        if (!device->vkCmdTraceRaysIndirect2KHR) return false;
    }

    /* video coding */
    if (extension_bits & (device_extension_khr_video_queue | device_extension_khr_video_decode_queue)) {
        device->vkBindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR)
            device_proc_address(device, "vkBindVideoSessionMemoryKHR");
        device->vkCmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR)
            device_proc_address(device, "vkCmdBeginVideoCodingKHR");
        device->vkCmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR)
            device_proc_address(device, "vkCmdControlVideoCodingKHR");
        device->vkCmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR)
            device_proc_address(device, "vkCmdDecodeVideoKHR");
        device->vkCmdEncodeVideoKHR = (PFN_vkCmdEncodeVideoKHR)
            device_proc_address(device, "vkCmdEncodeVideoKHR");
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
        device->vkGetEncodedVideoSessionParametersKHR = (PFN_vkGetEncodedVideoSessionParametersKHR)
            device_proc_address(device, "vkGetEncodedVideoSessionParametersKHR");
        device->vkGetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR)
            device_proc_address(device, "vkGetVideoSessionMemoryRequirementsKHR");
        device->vkUpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR)
            device_proc_address(device, "vkUpdateVideoSessionParametersKHR");

        if (!device->vkBindVideoSessionMemoryKHR ||
            !device->vkCmdBeginVideoCodingKHR ||
            !device->vkCmdControlVideoCodingKHR ||
            !device->vkCmdDecodeVideoKHR ||
            !device->vkCmdEncodeVideoKHR ||
            !device->vkCmdEndVideoCodingKHR ||
            !device->vkCreateVideoSessionKHR ||
            !device->vkCreateVideoSessionParametersKHR ||
            !device->vkDestroyVideoSessionKHR ||
            !device->vkDestroyVideoSessionParametersKHR ||
            !device->vkGetEncodedVideoSessionParametersKHR ||
            !device->vkGetVideoSessionMemoryRequirementsKHR ||
            !device->vkUpdateVideoSessionParametersKHR)
            return false;
    }
    return true;
}

extern enum lake_result vulkan_device_assembly_helper(struct xaku_device *device) 
{
    (void)device;
    if (load_device_procedures(device, 0))
        return lake_result_error_incompatible_driver;
    return lake_result_success;
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
            bedrock_log_debug("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            bedrock_log_verbose("%s", callbackdata->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            bedrock_log_warn("%s", callbackdata->pMessage);
            break;
        default:
            bedrock_log_error("%s", callbackdata->pMessage);
            bedrock_assert_debug(!"Vulkan validation error !!");
    }
    return VK_FALSE;
}

static VKAPI_ATTR void *VKAPI_CALL 
host_allocator_callback_malloc(
    void                       *pUserData, 
    usize                       size, 
    usize                       alignment, 
    VkSystemAllocationScope     allocationScope)
{
    (void)allocationScope;
    const struct riven_context *riven = (struct riven_context *)pUserData;
    return riven_malloc_aligned(riven->self, size, alignment);
}

static VKAPI_ATTR void *VKAPI_CALL 
host_allocator_callback_realloc(
    void                       *pUserData, 
    void                       *pOriginal, 
    usize                       size, 
    usize                       alignment, 
    VkSystemAllocationScope     allocationScope)
{
    (void)allocationScope;
    const struct riven_context *riven = (struct riven_context *)pUserData;
    return riven_realloc_aligned(riven->self, pOriginal, size, alignment);
}

static VKAPI_ATTR void VKAPI_CALL 
host_allocator_callback_free(void *pUserData, void *pMemory)
{
    const struct riven_context *riven = (struct riven_context *)pUserData;
    riven_free(riven->self, pMemory); 
}

struct physical_device_query_work {
    u64                             missing_features;
    u32                             index;
    const char                     *encore_name;
    const struct riven_context     *riven;
    const struct xaku_encore       *xaku;
    struct vulkan_physical_device   write;
};
static void LAKECALL physical_device_query(struct physical_device_query_work *work)
{
    const struct riven_context *riven = work->riven; /* only riven_tag_deferred is used */
    const struct xaku_encore *xaku = work->xaku;
    struct vulkan_physical_device *write = &work->write;

    /* VkVideoFormatPropertiesKHR *video_decode_format_properties; */
    /* VkVideoFormatPropertiesKHR *video_encode_format_properties; */
    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties *extension_properties;
    const char *device_name;

    u32 vk_api_version;
    u32 extension_count;
    /* u32 video_decode_format_count; */
    /* u32 video_encode_format_count; */
    u32 queue_family_count;

    work->missing_features = UINT64_MAX;
    xaku->vkGetPhysicalDeviceProperties(write->vk_physical_device, &write->properties2.properties);
    xaku->vkGetPhysicalDeviceQueueFamilyProperties2(write->vk_physical_device, &queue_family_count, NULL);
    VERIFY_VK(xaku->vkEnumerateDeviceExtensionProperties(write->vk_physical_device, NULL, &extension_count, NULL));
    device_name = write->properties2.properties.deviceName;
    vk_api_version = write->properties2.properties.apiVersion;

    /* early checks to invalidate faulty or old physical devices */
    if (vk_api_version < VK_API_VERSION_1_2) {
        bedrock_log_debug("'%s' physical device '%s' has too old drivers - found API version %u.%u.%u, we target atleast 1.2.X.",
                work->encore_name, device_name, (vk_api_version >> 22u), (vk_api_version >> 12u) & 0x3ffu, vk_api_version & 0xfffu);
        return;
    } else if (queue_family_count == 0) {
        bedrock_log_debug("'%s' physical device '%s' has no queue families.", work->encore_name, device_name);
        return;
    } else if (extension_count == 0) {
        bedrock_log_debug("'%s' physical device '%s' has zero Vulkan extensions.", work->encore_name, device_name);
        return;
    }
    { /* get scratch memory */
        /* TODO video coding */
        usize queue_family_video_properties_bytes = A8(lake_sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count);
        usize queue_family_properties2_bytes = A8(lake_sizeof(VkQueueFamilyProperties2) * queue_family_count);
        usize extension_properties_bytes = A8(lake_sizeof(VkExtensionProperties) * extension_count);
        usize total_bytes = 
            queue_family_video_properties_bytes +
            queue_family_properties2_bytes +
            extension_properties_bytes;

        usize o = 0;
        u8 *raw = (u8 *)riven_thalloc(riven->self, riven_tag_deferred, total_bytes, 8);
        bedrock_memset(raw, 0, total_bytes);

        queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)&raw[o];
        o += queue_family_video_properties_bytes;
        queue_family_properties2 = (VkQueueFamilyProperties2 *)&raw[o];
        o += queue_family_properties2_bytes;
        extension_properties = (VkExtensionProperties *)&raw[o];
        bedrock_assert_debug(o + extension_properties_bytes == total_bytes);
    }

    /* resolve queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        queue_family_properties2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queue_family_properties2[i].pNext = &queue_family_video_properties[i];
        queue_family_video_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        /* TODO video coding */
        write->queue_families[i].vk_index = UINT32_MAX;
    }
    xaku->vkGetPhysicalDeviceQueueFamilyProperties2(write->vk_physical_device, &queue_family_count, queue_family_properties2);
    for (u32 i = 0; i < queue_family_count; i++) {
        struct VkQueueFamilyProperties *properties = &queue_family_properties2[i].queueFamilyProperties;

        (void)properties;
        /* TODO video coding */
    }

    /* resolve extensions */
    for (u32 i = 0; i < device_extension_count; i++)
        if (query_extension(extension_properties, extension_count, g_device_extension_names[i]))
            write->vk_extension_bits |= (1ull << i);

    void *feature_chain = NULL;
    void *property_chain = NULL;

    write->timeline_semaphore_features.pNext = feature_chain;
    write->timeline_semaphore_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    feature_chain = (void *)&write->timeline_semaphore_features;
    write->timeline_semaphore_properties.pNext = property_chain;
    write->timeline_semaphore_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES;
    property_chain = (void *)&write->timeline_semaphore_properties;

    write->subgroup_size_control_features.pNext = feature_chain;
    write->subgroup_size_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES;
    feature_chain = (void *)&write->subgroup_size_control_features;
    write->subgroup_size_control_properties.pNext = property_chain;
    write->subgroup_size_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES;
    property_chain = (void *)&write->subgroup_size_control_properties;

    write->dynamic_rendering_features.pNext = feature_chain;
    write->dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    feature_chain = (void *)&write->dynamic_rendering_features;

    write->dynamic_rendering_local_read_features.pNext = feature_chain;
    write->dynamic_rendering_local_read_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
    feature_chain = (void *)&write->dynamic_rendering_local_read_features;

    if (write->vk_extension_bits & device_extension_ext_dynamic_rendering_unused_attachments)
    {
        write->dynamic_rendering_unused_attachments_features.pNext = feature_chain;
        write->dynamic_rendering_unused_attachments_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
        feature_chain = (void *)&write->dynamic_rendering_unused_attachments_features;
    }

    write->descriptor_indexing_properties.pNext = property_chain;
    write->descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    property_chain = (void *)&write->descriptor_indexing_properties;
    write->descriptor_indexing_features.pNext = feature_chain;
    write->descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    feature_chain = (void *)&write->descriptor_indexing_features;

    write->synchronization2_features.pNext = feature_chain;
    write->synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    feature_chain = (void *)&write->synchronization2_features;

    write->vulkan_memory_model_features.pNext = feature_chain;
    write->vulkan_memory_model_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES;
    feature_chain = (void *)&write->vulkan_memory_model_features;

    if (write->vk_extension_bits & device_extension_ext_extended_dynamic_state3)
    {
        write->extended_dynamic_state3_features.pNext = feature_chain;
        write->extended_dynamic_state3_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
        feature_chain = (void *)&write->extended_dynamic_state3_features;
    }

    if (write->vk_extension_bits & device_extension_ext_shader_image_atomic_int64)
    {
        write->shader_image_atomic_int64_features.pNext = feature_chain;
        write->shader_image_atomic_int64_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT;
        feature_chain = (void *)&write->shader_image_atomic_int64_features;
    }

    if (write->vk_extension_bits & device_extension_ext_shader_atomic_float)
    {
        write->shader_atomic_float_features.pNext = feature_chain;
        write->shader_atomic_float_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
        feature_chain = (void *)&write->shader_atomic_float_features;
    }

    write->shader_atomic_int64_features.pNext = feature_chain;
    write->shader_atomic_int64_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES;
    feature_chain = (void *)&write->shader_atomic_int64_features;

    write->shader_float16_int8_features.pNext = feature_chain;
    write->shader_float16_int8_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    feature_chain = (void *)&write->shader_float16_int8_features;

    write->bit16_storage_features.pNext = feature_chain;
    write->bit16_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
    feature_chain = (void *)&write->bit16_storage_features;

    write->bit8_storage_features.pNext = feature_chain;
    write->bit8_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES;
    feature_chain = (void *)&write->bit8_storage_features;

    if (write->vk_extension_bits & device_extension_ext_robustness2) 
    {
        write->robustness2_features.pNext = feature_chain;
        write->robustness2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
        feature_chain = (void *)&write->robustness2_features;
        write->robustness2_properties.pNext = property_chain;
        write->robustness2_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;
        property_chain = (void *)&write->robustness2_properties;
    }

    write->buffer_device_address_features.pNext = feature_chain;
    write->buffer_device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    feature_chain = (void *)&write->buffer_device_address_features;

    write->scalar_block_layout_features.pNext = feature_chain;
    write->scalar_block_layout_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES;
    feature_chain = (void *)&write->scalar_block_layout_features;

    write->variable_pointer_features.pNext = feature_chain;
    write->variable_pointer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES;
    feature_chain = (void *)&write->variable_pointer_features;

    write->host_query_reset_features.pNext = feature_chain;
    write->host_query_reset_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
    feature_chain = (void *)&write->host_query_reset_features;

    if (write->vk_extension_bits & device_extension_ext_mesh_shader) 
    {
        write->mesh_shader_features.pNext = feature_chain;
        write->mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
        feature_chain = (void *)&write->mesh_shader_features;
        write->mesh_shader_properties.pNext = property_chain;
        write->mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
        property_chain = (void *)&write->mesh_shader_properties;
    }

    if (write->vk_extension_bits & device_extension_khr_fragment_shading_rate) 
    {
        write->fragment_shading_rate_features.pNext = feature_chain;
        write->fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
        feature_chain = (void *)&write->fragment_shading_rate_features;
        write->fragment_shading_rate_properties.pNext = property_chain;
        write->fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
        property_chain = (void *)&write->fragment_shading_rate_properties;
    }

    if (write->vk_extension_bits & device_extension_khr_acceleration_structure)  
    {
        write->acceleration_structure_features.pNext = feature_chain;
        write->acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        feature_chain = (void *)&write->acceleration_structure_features;
        write->acceleration_structure_properties.pNext = property_chain;
        write->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        property_chain = (void *)&write->acceleration_structure_properties;
    }

    if (write->vk_extension_bits & device_extension_nv_ray_tracing_invocation_reorder)
    {
        write->ray_tracing_invocation_reorder_features.pNext = feature_chain;
        write->ray_tracing_invocation_reorder_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV;
        feature_chain = (void *)&write->ray_tracing_invocation_reorder_features;
        write->ray_tracing_invocation_reorder_properties.pNext = property_chain;
        write->ray_tracing_invocation_reorder_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_PROPERTIES_NV;
        property_chain = (void *)&write->ray_tracing_invocation_reorder_properties;
    }

    if (write->vk_extension_bits & device_extension_khr_ray_tracing_position_fetch)
    {
        write->ray_tracing_position_fetch_features.pNext = feature_chain;
        write->ray_tracing_position_fetch_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR;
        feature_chain = (void *)&write->ray_tracing_position_fetch_features;
    }

    if (write->vk_extension_bits & device_extension_khr_ray_tracing_pipeline)
    {
        write->ray_tracing_pipeline_features.pNext = feature_chain;
        write->ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        feature_chain = (void *)&write->ray_tracing_pipeline_features;
        write->ray_tracing_pipeline_properties.pNext = property_chain;
        write->ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        property_chain = (void *)&write->ray_tracing_pipeline_properties;
    }

    if (write->vk_extension_bits & device_extension_khr_ray_query)
    {
        write->ray_query_features.pNext = feature_chain;
        write->ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
        feature_chain = (void *)&write->ray_query_features;
    }

    write->properties2.pNext = property_chain;
    write->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    write->features2.pNext = feature_chain;
    write->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    if (write->vk_extension_bits & device_extension_ext_memory_budget) {
        write->memory_properties2.pNext = &write->memory_budget;
        write->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
        write->memory_budget.pNext = NULL;
    }
    write->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

    xaku->vkGetPhysicalDeviceFeatures2(write->vk_physical_device, &write->features2);
    xaku->vkGetPhysicalDeviceProperties2(write->vk_physical_device, &write->properties2);
    xaku->vkGetPhysicalDeviceMemoryProperties2(write->vk_physical_device, &write->memory_properties2);

    write->has_conservative_rasterization = write->vk_extension_bits & device_extension_ext_conservative_rasterization;
    write->has_swapchain = write->vk_extension_bits & device_extension_khr_swapchain;

    /* check for missing required features */
    /* TODO */

    /* calculate the score */
    /* TODO */

    bedrock_log_info("Physical device '%s' accepted at index %u with a total score of %lu points.", device_name, work->index, write->xaku_properties.total_score);
    work->missing_features = 0llu;
}

FN_XAKU_LIST_DEVICES_PROPERTIES(vulkan)
{
    if (properties) {
        bedrock_assert_debug(*property_count >= xaku->physical_device_count);

        for (u32 i = 0; i < xaku->physical_device_count; i++)
            properties[i] = &xaku->physical_devices[i].xaku_properties;
    }
    *property_count = xaku->physical_device_count;
}

struct xaku_encore *g_vulkan = NULL;

static void LAKECALL xaku_encore_zero_ref(struct xaku_encore *xaku)
{
    bedrock_assert_debug(xaku && lake_atomic_read(&xaku->interface.riven.refcnt) == 0);

    if (xaku->instance)
        xaku->vkDestroyInstance(xaku->instance, &xaku->allocator);
    if (xaku->module_vulkan)
        bedrock_module_close(xaku->module_vulkan);
    bedrock_zerop(xaku);
    g_vulkan = NULL;
}

FN_RIVEN_ENCORE(xaku, vulkan)
{
    const char *name = "xaku_vulkan";
    void *module_vulkan;
    u32 instance_api_version = 0;
    u32 layer_count = 0;
    u32 extension_count = 0;
    u32 extension_bits = 0;
    VkAllocationCallbacks allocator = {
        .pfnAllocation = host_allocator_callback_malloc,
        .pfnReallocation = host_allocator_callback_realloc,
        .pfnFree = host_allocator_callback_free,
        .pfnInternalAllocation = NULL,
        .pfnInternalFree = NULL,
        .pUserData = (void *)context,
    };

    /* we allow only one Vulkan backend instance to exist at a time */
    if (lake_unlikely(g_vulkan != NULL))
        return g_vulkan;

#if defined(LAKE_PLATFORM_WINDOWS)
    module_vulkan = bedrock_module_open("vulkan-1.dll");
#elif defined(LAKE_PLATFORM_APPLE)
    module_vulkan = bedrock_module_open("libvulkan.dylib");
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("libvulkan.1.dylib");
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a framework. App store rules for iOS 
     * strictly enforce no .dylib's. If they aren't found it just falls through. */
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("vulkan.framework/vulkan");
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("MoltenVK.framework/MoltenVK");
    /* Modern versions of MacOS don't search /usr/local/lib automatically contrary to what
     * man dlopen says. Vulkan SDK uses this as the system-wide installation location, so 
     * we're going to fallback to this if all else fails. */
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("/usr/local/lib/libvulkan.dylib");
#else
    module_vulkan = bedrock_module_open("libvulkan.so.1");
    if (!module_vulkan)
        module_vulkan = bedrock_module_open("libvulkan.so");
#endif
    if (!module_vulkan) {
        bedrock_log_debug("'%s' can't open the Vulkan drivers, ensure they are correclty installed and available via the system PATH", name);
        return NULL;
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)bedrock_get_proc_address(module_vulkan, "vkGetInstanceProcAddr");
    if (vkGetInstanceProcAddr == NULL) {
        bedrock_log_debug("'%s' can't get the address of vkGetInstanceProcAddr from Vulkan drivers.", name);
        bedrock_module_close(module_vulkan);
        return NULL;
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
        bedrock_log_debug("'%s' can't get addresses of global procedures from Vulkan drivers.", name);
        bedrock_module_close(module_vulkan);
        return NULL;
    }

    /* check the API version */
    vkEnumerateInstanceVersion(&instance_api_version);
    if (instance_api_version < VK_API_VERSION_1_2) {
        bedrock_log_debug("'%s' outdated drivers. We target a minimum of Vulkan 1.2 core."
                "Your drivers API version is %u.%u.%u, please update your drivers.",
                name, (instance_api_version >> 22u), (instance_api_version >> 12u) & 0x3ffu, (instance_api_version & 0xfffu));
        bedrock_module_close(module_vulkan);
        return NULL;
    }
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    /* check instance extensions */
    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
    if (!extension_count) {
        bedrock_log_debug("'%s' no instance extensions available, can't continue.", name);
        bedrock_module_close(module_vulkan);
        return NULL;
    }

    VkExtensionProperties *extension_properties = (VkExtensionProperties *)
        riven_thalloc(context->self, riven_tag_deferred, lake_sizeof(VkExtensionProperties) * extension_count, lake_alignof(VkExtensionProperties));
    VERIFY_VK(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_properties));

    /* check for extensions */
    for (u32 i = 0; i < instance_extension_count; i++)
        if (query_extension(extension_properties, extension_count, g_instance_extension_names[i]))
            extension_bits |= (1u << i);

    u32 o = 0; 
    extension_count = bits_popcnt_table_lookup((const u8 *)&extension_bits, lake_sizeof(extension_bits));
    const char **extensions = (const char **)
        riven_thalloc(context->self, riven_tag_deferred, lake_sizeof(const char *) * extension_count, lake_alignof(const char *));
    for (u32 i = 0; i < instance_extension_count; i++)
        if (extension_bits & (1u << i))
            extensions[o++] = g_instance_extension_names[i];
    bedrock_assert_debug(extension_count == o);

    VkApplicationInfo vk_app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = context->app->app_name,
        .applicationVersion = context->app->app_build_ver,
        .pEngineName = context->app->engine_name,
        .engineVersion = context->app->engine_build_ver,
        .apiVersion = instance_api_version,
    };
    VkInstanceCreateInfo vk_instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &vk_app_info,
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
        .enabledValidationFeatureCount = lake_arraysize(validation_feature_enable),
        .pEnabledValidationFeatures = validation_feature_enable,
        .disabledValidationFeatureCount = 0,
        .pDisabledValidationFeatures = NULL,
    };
    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    /* enable validation layers ? */
    if (context->app->enable_devtools && layer_count && extension_bits) {
        extension_bits |= instance_extension_layer_validation;
        vk_instance_info.pNext = &validation_features;
        vk_instance_info.enabledLayerCount = 1;
        vk_instance_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }

    /* create the instance */
    VkInstance vk_instance = VK_NULL_HANDLE;
    enum VkResult vk_result = vkCreateInstance(&vk_instance_info, &allocator, &vk_instance);
    if (vk_result != VK_SUCCESS) {
        bedrock_log_debug("'%s' creating a Vulkan device failed: %s.", name, vulkan_result_string(vk_result));
        bedrock_module_close(module_vulkan);
        return NULL;
    }

    /* assemble the interface */
    struct xaku_encore *xaku = (struct xaku_encore *)
        riven_thalloc(context->self, tag, lake_sizeof(struct xaku_encore), lake_alignof(struct xaku_encore));
    bedrock_zerop(xaku);

    /* write interface header */
    xaku->interface.riven.context = *context;
    xaku->interface.riven.tag = tag;
    xaku->interface.riven.name = name;
    xaku->interface.riven.zero_ref_callback = (PFN_riven_work)xaku_encore_zero_ref;
    g_vulkan = xaku;

    xaku->instance = vk_instance;
    xaku->allocator = allocator;
    xaku->vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    xaku->vkCreateInstance = vkCreateInstance;
    xaku->vkEnumerateInstanceVersion = vkEnumerateInstanceVersion;
    xaku->vkEnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
    xaku->vkEnumerateInstanceLayerProperties = vkEnumerateInstanceLayerProperties;
    xaku->module_vulkan = module_vulkan;
    xaku->api_version = instance_api_version;

    if (!load_instance_procedures(xaku, extension_bits)) {
        bedrock_log_debug("'%s' loading instance procedures failed.", name);
        xaku_encore_zero_ref(xaku);
        return NULL;
    }
    if (extension_bits & instance_extension_layer_validation) {
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
        if (xaku->vkCreateDebugUtilsMessengerEXT(xaku->instance, &messenger_info, &xaku->allocator, &xaku->debug_messenger) == VK_SUCCESS)
            bedrock_log_verbose("'%s' created Vulkan validation layers: %s.", name, validation_layers[0]);
    }

    /* query physical devices */
    vk_result = xaku->vkEnumeratePhysicalDevices(xaku->instance, &xaku->physical_device_count, NULL);
    if (xaku->physical_device_count == 0) {
        bedrock_log_debug("'%s' no physical devices are available to Vulkan.", name);
        xaku_encore_zero_ref(xaku);
        return NULL;
    }
    usize physical_devices_bytes = A8(lake_sizeof(VkPhysicalDevice) * xaku->physical_device_count);
    usize physical_device_query_work_bytes = A8(lake_sizeof(struct physical_device_query_work) * xaku->physical_device_count);
    usize physical_device_riven_bytes = A8(lake_sizeof(struct riven_work) * xaku->physical_device_count);
    usize physical_device_indices_bytes = A8(lake_sizeof(u32) * xaku->physical_device_count);
    usize physical_device_query_total_bytes = 
        physical_devices_bytes +
        physical_device_query_work_bytes +
        physical_device_riven_bytes +
        physical_device_indices_bytes;

    u8 *raw = (u8 *)riven_thalloc(context->self, riven_tag_deferred, physical_device_query_total_bytes, 8);
    bedrock_memset(raw, 0, physical_device_query_total_bytes);
    
    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)raw;
    o = physical_devices_bytes;
    struct physical_device_query_work *physical_device_query_work = (struct physical_device_query_work *)&raw[o];
    o += physical_device_query_work_bytes;
    struct riven_work *physical_device_riven = (struct riven_work *)&raw[o];
    o += physical_device_riven_bytes;
    u32 *physical_device_indices = (u32 *)&raw[o];
    bedrock_assert_debug(o + physical_device_indices_bytes == physical_device_query_total_bytes);
    VERIFY_VK(xaku->vkEnumeratePhysicalDevices(xaku->instance, &xaku->physical_device_count, physical_devices));

    /* execute the physical device query as a job per physical device */
    for (u32 i = 0; i < xaku->physical_device_count; i++) {
        physical_device_query_work[i].missing_features = UINT64_MAX;
        physical_device_query_work[i].index = i;
        physical_device_query_work[i].riven = context;
        physical_device_query_work[i].xaku = xaku;
        physical_device_query_work[i].encore_name = name;
        physical_device_query_work[i].write.vk_physical_device = physical_devices[i];
        physical_device_riven[i].procedure = (PFN_riven_work)physical_device_query;
        physical_device_riven[i].argument = (void *)&physical_device_query_work[i];
        physical_device_riven[i].name = "xaku_vulkan:encore:physical_device_query";
    }
    riven_split_work_and_unchain(context->self, physical_device_riven, xaku->physical_device_count);

    o = 0;
    /* collect indices of accepted physical devices */
    for (u32 i = 0; i < xaku->physical_device_count; i++)
        if (physical_device_query_work[i].missing_features == 0llu)
            physical_device_indices[o++] = i;
    if (o == 0) {
        bedrock_log_debug("'%s' physical device query invalidated all available physical devices (%u), can't continue.", name, xaku->physical_device_count);
        xaku_encore_zero_ref(xaku);
        return NULL;
    }
    /* sort indices by score */
    for (u32 i = 0, j, max_idx; i < o - 1; i++) {
        max_idx = i;
        for (j = i + 1; j < o; j++)
            if (physical_device_query_work[physical_device_indices[j]].write.xaku_properties.total_score >
                physical_device_query_work[physical_device_indices[max_idx]].write.xaku_properties.total_score) 
            {
                max_idx = j;
            }
        lake_xorswap(&physical_device_indices[max_idx], &physical_device_indices[i]);
    }
    /* copy the accepted physical devices */
    struct vulkan_physical_device *accepted_devices = (struct vulkan_physical_device *)
        riven_thalloc(context->self, tag, lake_sizeof(struct vulkan_physical_device) * o, lake_alignof(struct vulkan_physical_device));
    for (u32 i = 0; i < o; i++)
        bedrock_memcpy(&accepted_devices[i], &physical_device_query_work[physical_device_indices[i]].write, lake_sizeof(struct vulkan_physical_device));
    xaku->physical_devices = accepted_devices;
    xaku->physical_device_count = o;

    /* write interface procedures */
    xaku->interface.list_devices_properties = _xaku_vulkan_list_devices_properties;
    xaku->interface.device_assembly = _xaku_vulkan_device_assembly;
    xaku->interface.device_disassembly = _xaku_vulkan_device_disassembly;
    xaku->interface.device_queue_count = _xaku_vulkan_device_queue_count;
    xaku->interface.device_queue_wait_idle = _xaku_vulkan_device_queue_wait_idle;
    xaku->interface.device_wait_idle = _xaku_vulkan_device_wait_idle;
    xaku->interface.device_submit = _xaku_vulkan_device_submit;
    xaku->interface.device_present = _xaku_vulkan_device_present;
    xaku->interface.deferred_resource_disassembly = _xaku_vulkan_deferred_resource_disassembly;
    xaku->interface.memory_buffer_requirements = _xaku_vulkan_memory_buffer_requirements;
    xaku->interface.memory_texture_requirements = _xaku_vulkan_memory_texture_requirements;
    xaku->interface.memory_assembly = _xaku_vulkan_memory_assembly;
    xaku->interface.memory_disassembly = _xaku_vulkan_memory_disassembly;
    xaku->interface.query_pool_assembly = _xaku_vulkan_query_pool_assembly;
    xaku->interface.query_pool_disassembly = _xaku_vulkan_query_pool_disassembly;
    xaku->interface.swapchain_assembly = _xaku_vulkan_swapchain_assembly;
    xaku->interface.swapchain_disassembly = _xaku_vulkan_swapchain_disassembly;
    xaku->interface.compute_pipeline_assembly = _xaku_vulkan_compute_pipeline_assembly;
    xaku->interface.compute_pipeline_disassembly = _xaku_vulkan_compute_pipeline_disassembly;
    xaku->interface.raster_pipeline_assembly = _xaku_vulkan_raster_pipeline_assembly;
    xaku->interface.raster_pipeline_disassembly = _xaku_vulkan_raster_pipeline_disassembly;
    xaku->interface.ray_tracing_pipeline_assembly = _xaku_vulkan_ray_tracing_pipeline_assembly;
    xaku->interface.ray_tracing_pipeline_disassembly = _xaku_vulkan_ray_tracing_pipeline_disassembly;
    xaku->interface.command_recorder_assembly = _xaku_vulkan_command_recorder_assembly;
    xaku->interface.command_recorder_disassembly = _xaku_vulkan_command_recorder_disassembly;
    xaku->interface.create_buffer = _xaku_vulkan_create_buffer;
    xaku->interface.create_buffer_from_memory = _xaku_vulkan_create_buffer_from_memory;
    xaku->interface.create_texture = _xaku_vulkan_create_texture;
    xaku->interface.create_texture_from_memory = _xaku_vulkan_create_texture_from_memory;
    xaku->interface.create_texture_view = _xaku_vulkan_create_texture_view;
    xaku->interface.create_sampler = _xaku_vulkan_create_sampler;
    xaku->interface.create_tlas = _xaku_vulkan_create_tlas;
    xaku->interface.create_tlas_from_buffer = _xaku_vulkan_create_tlas_from_buffer;
    xaku->interface.create_blas = _xaku_vulkan_create_blas;
    xaku->interface.create_blas_from_buffer = _xaku_vulkan_create_blas_from_buffer;
    xaku->interface.get_buffer_assembly = _xaku_vulkan_get_buffer_assembly;
    xaku->interface.get_texture_assembly = _xaku_vulkan_get_texture_assembly;
    xaku->interface.get_texture_view_assembly = _xaku_vulkan_get_texture_view_assembly;
    xaku->interface.get_sampler_assembly = _xaku_vulkan_get_sampler_assembly;
    xaku->interface.get_tlas_build_sizes = _xaku_vulkan_get_tlas_build_sizes;
    xaku->interface.get_tlas_assembly = _xaku_vulkan_get_tlas_assembly;
    xaku->interface.get_blas_build_sizes = _xaku_vulkan_get_blas_build_sizes;
    xaku->interface.get_blas_assembly = _xaku_vulkan_get_blas_assembly;
    xaku->interface.destroy_buffer = _xaku_vulkan_destroy_buffer;
    xaku->interface.destroy_texture = _xaku_vulkan_destroy_texture;
    xaku->interface.destroy_texture_view = _xaku_vulkan_destroy_texture_view;
    xaku->interface.destroy_sampler = _xaku_vulkan_destroy_sampler;
    xaku->interface.destroy_tlas = _xaku_vulkan_destroy_tlas;
    xaku->interface.destroy_blas = _xaku_vulkan_destroy_blas;
    xaku->interface.cmd_copy_buffer_to_buffer = _xaku_vulkan_cmd_copy_buffer_to_buffer;
    xaku->interface.cmd_copy_buffer_to_texture = _xaku_vulkan_cmd_copy_buffer_to_texture;
    xaku->interface.cmd_copy_texture_to_buffer = _xaku_vulkan_cmd_copy_texture_to_buffer;
    xaku->interface.cmd_copy_texture_to_texture = _xaku_vulkan_cmd_copy_texture_to_texture;
    xaku->interface.cmd_blit_texture_to_texture = _xaku_vulkan_cmd_blit_texture_to_texture;
    xaku->interface.cmd_clear_buffer = _xaku_vulkan_cmd_clear_buffer;
    xaku->interface.cmd_clear_texture = _xaku_vulkan_cmd_clear_texture;
    xaku->interface.cmd_build_acceleration_structures = _xaku_vulkan_cmd_build_acceleration_structures;
    xaku->interface.cmd_destroy_buffer_deferred = _xaku_vulkan_cmd_destroy_buffer_deferred;
    xaku->interface.cmd_destroy_texture_deferred = _xaku_vulkan_cmd_destroy_texture_deferred;
    xaku->interface.cmd_destroy_texture_view_deferred = _xaku_vulkan_cmd_destroy_texture_view_deferred;
    xaku->interface.cmd_destroy_sampler_deferred = _xaku_vulkan_cmd_destroy_sampler_deferred;
    xaku->interface.cmd_destroy_tlas_deferred = _xaku_vulkan_cmd_destroy_tlas_deferred;
    xaku->interface.cmd_destroy_blas_deferred = _xaku_vulkan_cmd_destroy_blas_deferred;
    xaku->interface.cmd_push_constant = _xaku_vulkan_cmd_push_constant;
    xaku->interface.cmd_set_compute_pipeline = _xaku_vulkan_cmd_set_compute_pipeline;
    xaku->interface.cmd_set_raster_pipeline = _xaku_vulkan_cmd_set_raster_pipeline;
    xaku->interface.cmd_set_ray_tracing_pipeline = _xaku_vulkan_cmd_set_ray_tracing_pipeline;
    xaku->interface.cmd_set_viewport = _xaku_vulkan_cmd_set_viewport;
    xaku->interface.cmd_set_scissor = _xaku_vulkan_cmd_set_scissor;
    xaku->interface.cmd_set_depth_bias = _xaku_vulkan_cmd_set_depth_bias;
    xaku->interface.cmd_set_index_buffer = _xaku_vulkan_cmd_set_index_buffer;
    xaku->interface.cmd_begin_rendering = _xaku_vulkan_cmd_begin_rendering;
    xaku->interface.cmd_end_rendering = _xaku_vulkan_cmd_end_rendering;
    xaku->interface.cmd_dispatch = _xaku_vulkan_cmd_dispatch;
    xaku->interface.cmd_dispatch_indirect = _xaku_vulkan_cmd_dispatch_indirect;
    xaku->interface.cmd_draw = _xaku_vulkan_cmd_draw;
    xaku->interface.cmd_draw_indexed = _xaku_vulkan_cmd_draw_indexed;
    xaku->interface.cmd_draw_indirect = _xaku_vulkan_cmd_draw_indirect;
    xaku->interface.cmd_draw_indirect_count = _xaku_vulkan_cmd_draw_indirect_count;
    xaku->interface.cmd_mesh_tasks = _xaku_vulkan_cmd_mesh_tasks;
    xaku->interface.cmd_mesh_tasks_indirect = _xaku_vulkan_cmd_mesh_tasks_indirect;
    xaku->interface.cmd_mesh_tasks_indirect_count = _xaku_vulkan_cmd_mesh_tasks_indirect_count;
    xaku->interface.cmd_trace_rays = _xaku_vulkan_cmd_trace_rays;
    xaku->interface.cmd_trace_rays_indirect = _xaku_vulkan_cmd_trace_rays_indirect;
    xaku->interface.cmd_write_timestamps = _xaku_vulkan_cmd_write_timestamps;
    xaku->interface.cmd_reset_timestamps = _xaku_vulkan_cmd_reset_timestamps;
    xaku->interface.cmd_begin_label = _xaku_vulkan_cmd_begin_label;
    xaku->interface.cmd_end_label = _xaku_vulkan_cmd_end_label;
    xaku->interface.compile_command_list = _xaku_vulkan_compile_command_list;
    return xaku;
}

#endif /* XAKU_VULKAN */
