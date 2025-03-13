#include "vk_pelagia.h"

#include <amw/bits.h>
#include <amw/hadal.h>
#include <amw/log.h>
#include <amw/process.h>

#ifndef VULKAN_VALIDATE_REQUIRED_EXTENSIONS
#define VULKAN_VALIDATE_REQUIRED_EXTENSIONS 1
#endif

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

VkFormat vulkan_translate_texture_format(enum pelagia_texture_format format)
{
    switch (format) {
#define TEX_FORMAT(_type) \
        case pelagia_texture_format_##_type: return 

        TEX_FORMAT(r1_unorm) VK_FORMAT_UNDEFINED;
        TEX_FORMAT(a8_unorm) VK_FORMAT_A8_UNORM;
        /* depth formats */
        TEX_FORMAT(d16_unorm) VK_FORMAT_D16_UNORM;
        TEX_FORMAT(d16s8_unorm) VK_FORMAT_D16_UNORM_S8_UINT;
        TEX_FORMAT(d24s8_unorm) VK_FORMAT_D24_UNORM_S8_UINT;
        TEX_FORMAT(d32_float) VK_FORMAT_D32_SFLOAT;
        TEX_FORMAT(d0s8_unorm) VK_FORMAT_S8_UINT;
        /* 8-bit, block size 1 byte, 1x1x1 extent, 1 texel/block */
        TEX_FORMAT(r8_snorm) VK_FORMAT_R8_SNORM;
        TEX_FORMAT(r8_unorm) VK_FORMAT_R8_UNORM;
        TEX_FORMAT(r8_sint) VK_FORMAT_R8_SINT;
        TEX_FORMAT(r8_uint) VK_FORMAT_R8_UINT;
        /* 16-bit, block size 2 bytes, 1x1x1 extent, 1 texel/block */
        TEX_FORMAT(r16_float) VK_FORMAT_R16_SFLOAT;
        TEX_FORMAT(r16_snorm) VK_FORMAT_R16_SNORM;
        TEX_FORMAT(r16_unorm) VK_FORMAT_R16_UNORM;
        TEX_FORMAT(r16_sint) VK_FORMAT_R16_SINT;
        TEX_FORMAT(r16_uint) VK_FORMAT_R16_UINT;
        TEX_FORMAT(r8g8_snorm) VK_FORMAT_R8G8_SNORM;
        TEX_FORMAT(r8g8_unorm) VK_FORMAT_R8G8_UNORM;
        TEX_FORMAT(r8g8_sint) VK_FORMAT_R8G8_SINT;
        TEX_FORMAT(r8g8_uint) VK_FORMAT_R8G8_UINT;
        /* 24-bit, block size 3 bytes, 1x1x1 extent, 1 texel/block */
        TEX_FORMAT(r8g8b8_snorm) VK_FORMAT_R8G8B8_SNORM;
        TEX_FORMAT(r8g8b8_unorm) VK_FORMAT_R8G8B8_UNORM;
        TEX_FORMAT(r8g8b8_sint) VK_FORMAT_R8G8B8_SINT;
        TEX_FORMAT(r8g8b8_uint) VK_FORMAT_R8G8B8_UINT;
        /* 32-bit, block size 4 bytes, 1x1x1 extent, 1 texel/block */
        TEX_FORMAT(r32_float) VK_FORMAT_R32_SFLOAT;
        TEX_FORMAT(r32_sint) VK_FORMAT_R32_SINT;
        TEX_FORMAT(r32_uint) VK_FORMAT_R32_UINT;
        TEX_FORMAT(r16g16_float) VK_FORMAT_R16G16_SFLOAT;
        TEX_FORMAT(r16g16_snorm) VK_FORMAT_R16G16_SNORM;
        TEX_FORMAT(r16g16_unorm) VK_FORMAT_R16G16_UNORM;
        TEX_FORMAT(r16g16_sint) VK_FORMAT_R16G16_SINT;
        TEX_FORMAT(r16g16_uint) VK_FORMAT_R16G16_UINT;
        TEX_FORMAT(r8g8b8a8_snorm) VK_FORMAT_R8G8B8A8_SNORM;
        TEX_FORMAT(r8g8b8a8_unorm) VK_FORMAT_R8G8B8A8_UNORM;
        TEX_FORMAT(r8g8b8a8_sint) VK_FORMAT_R8G8B8A8_SINT;
        TEX_FORMAT(r8g8b8a8_uint) VK_FORMAT_R8G8B8A8_UINT;
        TEX_FORMAT(b8g8r8a8_snorm) VK_FORMAT_B8G8R8A8_SNORM;
        TEX_FORMAT(b8g8r8a8_unorm) VK_FORMAT_B8G8R8A8_SNORM;
        TEX_FORMAT(b8g8r8a8_sint) VK_FORMAT_B8G8R8A8_SINT;
        TEX_FORMAT(b8g8r8a8_uint) VK_FORMAT_B8G8R8A8_UINT;
        TEX_FORMAT(r10g10b10a2) VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        TEX_FORMAT(b10g10r10a2) VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        /* 48-bit, block size 6 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r16g16b16_float) VK_FORMAT_R16G16B16_SFLOAT;
        TEX_FORMAT(r16g16b16_snorm) VK_FORMAT_R16G16B16_SNORM;
        TEX_FORMAT(r16g16b16_unorm) VK_FORMAT_R16G16B16_UNORM;
        TEX_FORMAT(r16g16b16_sint) VK_FORMAT_R16G16B16_SINT;
        TEX_FORMAT(r16g16b16_uint) VK_FORMAT_R16G16B16_UINT;
        /* 64-bit, block size 8 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r64_float) VK_FORMAT_R64_SFLOAT;
        TEX_FORMAT(r64_sint) VK_FORMAT_R64_SINT;
        TEX_FORMAT(r64_uint) VK_FORMAT_R64_UINT;
        TEX_FORMAT(r32g32_float) VK_FORMAT_R32G32_SFLOAT;
        TEX_FORMAT(r32g32_sint) VK_FORMAT_R32G32_SINT;
        TEX_FORMAT(r32g32_uint) VK_FORMAT_R32G32_UINT;
        TEX_FORMAT(r16g16b16a16_float) VK_FORMAT_R16G16B16A16_SFLOAT;
        TEX_FORMAT(r16g16b16a16_snorm) VK_FORMAT_R16G16B16A16_SNORM;
        TEX_FORMAT(r16g16b16a16_unorm) VK_FORMAT_R16G16B16A16_UNORM;
        TEX_FORMAT(r16g16b16a16_sint) VK_FORMAT_R16G16B16A16_SINT;
        TEX_FORMAT(r16g16b16a16_uint) VK_FORMAT_R16G16B16A16_UINT;
        /* 96-bit, block size 12 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r32g32b32_float) VK_FORMAT_R32G32B32_SFLOAT;
        TEX_FORMAT(r32g32b32_sint) VK_FORMAT_R32G32B32_SINT;
        TEX_FORMAT(r32g32b32_uint) VK_FORMAT_R32G32B32_UINT;
        /* 128-bit, block size 16 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r32g32b32a32_float) VK_FORMAT_R32G32B32A32_SFLOAT;
        TEX_FORMAT(r32g32b32a32_sint) VK_FORMAT_R32G32B32A32_SINT;
        TEX_FORMAT(r32g32b32a32_uint) VK_FORMAT_R32G32B32A32_UINT;
        TEX_FORMAT(r64g64_float) VK_FORMAT_R64G64_SFLOAT;
        TEX_FORMAT(r64g64_sint) VK_FORMAT_R64G64_SINT;
        TEX_FORMAT(r64g64_uint) VK_FORMAT_R64G64_UINT;
        /* 192-bit, block size 24 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r64g64b64_float) VK_FORMAT_R64G64B64_SFLOAT;
        TEX_FORMAT(r64g64b64_sint) VK_FORMAT_R64G64B64_SINT;
        TEX_FORMAT(r64g64b64_uint) VK_FORMAT_R64G64B64_UINT;
        /* 256-bit, block size 32 bytes, 1x1x1 block extent, 1 texel/block */
        TEX_FORMAT(r64g64b64a64_float) VK_FORMAT_R64G64B64A64_SFLOAT;
        TEX_FORMAT(r64g64b64a64_sint) VK_FORMAT_R64G64B64A64_SINT;
        TEX_FORMAT(r64g64b64a64_uint) VK_FORMAT_R64G64B64A64_UINT;
        /* compressed texture formats */
        TEX_FORMAT(BC1) VK_FORMAT_BC1_RGB_SRGB_BLOCK;
        TEX_FORMAT(BC2) VK_FORMAT_BC2_SRGB_BLOCK;
        TEX_FORMAT(BC3) VK_FORMAT_BC3_SRGB_BLOCK;
        TEX_FORMAT(BC4) VK_FORMAT_BC4_SNORM_BLOCK;
        TEX_FORMAT(BC5) VK_FORMAT_BC5_SNORM_BLOCK;
        TEX_FORMAT(BC6H) VK_FORMAT_BC6H_SFLOAT_BLOCK;
        TEX_FORMAT(BC7) VK_FORMAT_BC7_SRGB_BLOCK;
        TEX_FORMAT(ETC1) VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        TEX_FORMAT(ETC2) VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        TEX_FORMAT(ETC2A) VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        TEX_FORMAT(ETC2A1) VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        TEX_FORMAT(PTC12) VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
        TEX_FORMAT(PTC14) VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
        TEX_FORMAT(PTC22) VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
        TEX_FORMAT(PTC24) VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
        TEX_FORMAT(ASTC4x4) VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        TEX_FORMAT(ASTC5x4) VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
        TEX_FORMAT(ASTC5x5) VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        TEX_FORMAT(ASTC6x5) VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
        TEX_FORMAT(ASTC6x6) VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        TEX_FORMAT(ASTC8x5) VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
        TEX_FORMAT(ASTC8x6) VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
        TEX_FORMAT(ASTC8x8) VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        TEX_FORMAT(ASTC10x5) VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
        TEX_FORMAT(ASTC10x6) VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
        TEX_FORMAT(ASTC10x8) VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
        TEX_FORMAT(ASTC10x10) VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        TEX_FORMAT(ASTC12x10) VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
        TEX_FORMAT(ASTC12x12) VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
#undef TEX_FORMAT
        default:
            return VK_FORMAT_UNDEFINED;
    };
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
        log_warn("'pelagia_vulkan' can't find instance procedure of name '%s'.", procname);
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

struct physical_device_query_work {
    struct work_header              header;
    const struct pelagia           *pelagia;
    const struct hadal             *hadal;
    struct vulkan_physical_device  *device;
};

static void physical_device_query(struct physical_device_query_work *work)
{
    struct vulkan_physical_device *physical = work->device;
    assert_debug(physical->device);

    const struct pelagia *pelagia = work->pelagia;
    VkVideoFormatPropertiesKHR *video_format_properties;
    VkQueueFamilyVideoPropertiesKHR *queue_family_video_properties;
    VkQueueFamilyProperties2 *queue_family_properties2;
    VkExtensionProperties *extension_properties;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    usize log_buffer_size = 1500;
    usize log_offset = 0;
    char *log_buffer;

    u32 api_version = 0;
    u32 extension_count = 0;
    u32 video_format_properties_count = 0;
    u32 queue_family_count = 0;

    pelagia->vkGetPhysicalDeviceProperties(physical->device, &physical->properties2.properties);
    struct str formated_err = str_null;
    riven_format_string(pelagia->interface.header.riven, riven_tag_deferred, &formated_err, "Vulkan physical device '%s'", physical->properties2.properties.deviceName);
    const char *err = formated_err.ptr;

    /* check driver version */
    api_version = physical->properties2.properties.apiVersion;
    if (api_version < VK_API_VERSION_1_2) {
        log_warn("DISCARD: %s too old drivers - found API version %u.%u.%u, we target atleast 1.2.X.",
            err, (api_version >> 22u), (api_version >> 12u) & 0x3ffu, api_version & 0xfffu);
        return;
    }
    b32 has_vk13 = api_version >= VK_API_VERSION_1_3;
    b32 has_vk14 = api_version >= VK_API_VERSION_1_4;
    physical->score += (api_version & 0xfffu) * 15;

    /* check presence of queue families */
    pelagia->vkGetPhysicalDeviceQueueFamilyProperties2(physical->device, &queue_family_count, NULL);
    if (queue_family_count == 0) {
        log_warn("DISCARD: %s has no queue families.", err);
        return;
    }

    /* check presence of device extensions */
    VERIFY_VK(pelagia->vkEnumerateDeviceExtensionProperties(physical->device, NULL, &extension_count, NULL));
    if (extension_count == 0) {
        log_warn("DISCARD: %s has zero Vulkan extensions.", err);
        return;
    }

    if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        physical->score += 1500;
    else if (physical->properties2.properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        physical->score += 450;

    physical->properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    physical->properties_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES;
    physical->properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    physical->properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    physical->properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    physical->acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    physical->raytracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    physical->descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    physical->fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    physical->mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;

    physical->properties2.pNext = has_vk14 ? &physical->properties_14 : (has_vk13 ? (void *)&physical->properties_13 : (void *)&physical->properties_12);
    physical->properties_14.pNext = &physical->properties_13;
    physical->properties_13.pNext = &physical->properties_12;
    physical->properties_12.pNext = &physical->properties_11;
    physical->properties_11.pNext = &physical->acceleration_structure_properties;
    physical->acceleration_structure_properties.pNext = &physical->raytracing_pipeline_properties;
    physical->raytracing_pipeline_properties.pNext = &physical->descriptor_indexing_properties;
    physical->descriptor_indexing_properties.pNext = &physical->fragment_shading_rate_properties;
    physical->fragment_shading_rate_properties.pNext = &physical->mesh_shader_properties;

    physical->memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physical->memory_properties2.pNext = &physical->memory_budget;
    physical->memory_budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

    physical->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical->features_14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    physical->features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    physical->features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    physical->features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    physical->acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    physical->raytracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    physical->ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    physical->descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    physical->dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    physical->dynamic_rendering_local_read_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES;
    physical->synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    physical->fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
    physical->mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    physical->features2.pNext = has_vk14 ? &physical->features_14 : (has_vk13 ? (void *)&physical->features_13 : (void *)&physical->properties_12);
    physical->features_14.pNext = &physical->features_13;
    physical->features_13.pNext = &physical->features_12;
    physical->features_12.pNext = &physical->features_11;
    physical->features_11.pNext = &physical->acceleration_structure_features;
    physical->acceleration_structure_features.pNext = &physical->raytracing_pipeline_features;
    physical->raytracing_pipeline_features.pNext = &physical->ray_query_features;
    physical->ray_query_features.pNext = &physical->descriptor_indexing_features;
    physical->descriptor_indexing_features.pNext = &physical->dynamic_rendering_features;
    physical->dynamic_rendering_features.pNext = &physical->dynamic_rendering_local_read_features;
    physical->dynamic_rendering_local_read_features.pNext = &physical->synchronization2_features;
    physical->synchronization2_features.pNext = &physical->fragment_shading_rate_features;
    physical->fragment_shading_rate_features.pNext = &physical->mesh_shader_features;

    pelagia->vkGetPhysicalDeviceProperties2(physical->device, &physical->properties2);
    pelagia->vkGetPhysicalDeviceMemoryProperties2(physical->device, &physical->memory_properties2);
    pelagia->vkGetPhysicalDeviceFeatures2(physical->device, &physical->features2);

    /* if the display is present, we will make an effort to check for presentation support */
    if (work->hadal) {
        /* let's create a temporary surface we can work with */
        struct hadal_interface *interface = (struct hadal_interface *)work->hadal;
        VkResult res = interface->vulkan_create_surface(work->hadal, pelagia->instance, &surface, NULL, pelagia->vkGetInstanceProcAddr);

        if (res != VK_SUCCESS || !surface) {
            log_warn("%s creating a temporary Vulkan surface failed: %s. Can't check for presentation support.", err, vulkan_result_string(res));
            surface = VK_NULL_HANDLE;
        }
    }

    /* accelerated video capabilities are optional */
    VkPhysicalDeviceVideoFormatInfoKHR video_format = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR,
        .pNext = NULL,
        .imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR,
    };
    VERIFY_VK(pelagia->vkGetPhysicalDeviceVideoFormatPropertiesKHR(physical->device, &video_format, &video_format_properties_count, NULL));

    { /* calculate scratch memory requirements */
        usize video_format_properties_bytes = A16(sizeof(VkVideoFormatPropertiesKHR) * video_format_properties_count);
        usize queue_family_video_properties_bytes = A16(sizeof(VkQueueFamilyVideoPropertiesKHR) * queue_family_count);
        usize queue_family_properties_bytes = A16(sizeof(VkQueueFamilyProperties2) * queue_family_count);
        usize extension_properties_bytes = A16(sizeof(VkExtensionProperties) * extension_count);
        usize total_bytes = 
            log_buffer_size +
            video_format_properties_bytes +
            queue_family_video_properties_bytes +
            queue_family_properties_bytes +
            extension_properties_bytes;

        u8 *raw = (u8 *)riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, total_bytes, 16);
        memset(raw, 0, total_bytes);

        usize o = 0;
        log_buffer = (char *)&raw[o];
        o += log_buffer_size;
        video_format_properties = (VkVideoFormatPropertiesKHR *)&raw[o];
        o += video_format_properties_bytes;
        queue_family_video_properties = (VkQueueFamilyVideoPropertiesKHR *)&raw[o];
        o += queue_family_video_properties_bytes;
        queue_family_properties2 = (VkQueueFamilyProperties2 *)&raw[o];
        o += queue_family_properties_bytes;
        extension_properties = (VkExtensionProperties *)&raw[o];
    }
    /* prepare the log buffer */
    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
            "%s accepted (internal index %u):\n"
            "    Type             %-10s id %X\n"
            "    Vendor           %-10s id %X\n"
            "    Driver version   %u.%u.%u\n"
            "    API version      %u.%u.%u\n",
            err, work->header.index, 
            device_type_string(physical->properties2.properties.deviceType), physical->properties2.properties.deviceID,
            vendor_name_string(physical->properties2.properties.vendorID), physical->properties2.properties.vendorID,
            (physical->properties2.properties.driverVersion >> 22u),
            (physical->properties2.properties.driverVersion >> 12u) & 0x3ffu,
            physical->properties2.properties.driverVersion & 0xfffu,
            (physical->properties2.properties.apiVersion >> 22u),
            (physical->properties2.properties.apiVersion >> 12u) & 0x3ffu,
            physical->properties2.properties.apiVersion & 0xfffu);

    /* check for VRAM */
    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, "Memory heaps (%u):\n", physical->memory_properties2.memoryProperties.memoryHeapCount);
    for (u32 i = 0; i < physical->memory_properties2.memoryProperties.memoryHeapCount; i++) {
        usize size = (physical->memory_properties2.memoryProperties.memoryHeaps[i].size);
        usize flags = (physical->memory_properties2.memoryProperties.memoryHeaps[i].flags);

        b32 device_local = flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
        physical->score += (device_local ? 42 : 21) * (size >> 28); /* 256 MB -> 1 */
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                "    Heap %u   %6lu MB (%s)\n", i, (size >> 20), device_local ? "device local" : "host visible");
        if (device_local)
            physical->total_vram += size;
    }
    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
            "Total VRAM available %lu MB.\n", physical->total_vram >> 20);

    /* resolve queue families */
    for (u32 i = 0; i < queue_family_count; i++) {
        queue_family_video_properties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        queue_family_video_properties[i].pNext = NULL;
        queue_family_properties2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queue_family_properties2[i].pNext = &queue_family_video_properties[i];
    }
    pelagia->vkGetPhysicalDeviceQueueFamilyProperties2(physical->device, &queue_family_count, queue_family_properties2);
    
    physical->graphics_queue_family_index = UINT32_MAX;
    physical->compute_queue_family_index = UINT32_MAX;
    physical->transfer_queue_family_index = UINT32_MAX;
    physical->sparse_binding_queue_family_index = UINT32_MAX;
    physical->video_decode_queue_family_index = UINT32_MAX;
    physical->presentation_queue_family_index = UINT32_MAX;

    /* search for specialized queue families */
    VkQueueFlags graphics_queue_flags;
    for (u32 i = 0; i < queue_family_count; i++) {
        s32 queue_count = queue_family_properties2[i].queueFamilyProperties.queueCount;
        if (queue_count <= 0) 
            continue;

        VkQueueFlags flags = queue_family_properties2[i].queueFamilyProperties.queueFlags;
        if (physical->graphics_queue_family_index == UINT32_MAX && (flags & VK_QUEUE_GRAPHICS_BIT)) {
            /* don't be picky about the graphics queue */
            physical->graphics_queue_family_queue_count = queue_count;
            physical->graphics_queue_family_index = i;
            graphics_queue_flags = flags;
        } else if (physical->compute_queue_family_index == UINT32_MAX && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == VK_QUEUE_COMPUTE_BIT) {
            physical->has_async_compute = true;
            physical->async_compute_queue_family_queue_count = queue_count;
            physical->compute_queue_family_index = i;
        } else if (physical->transfer_queue_family_index == UINT32_MAX && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) == VK_QUEUE_TRANSFER_BIT) {
            physical->has_async_transfer = true;
            physical->transfer_queue_family_index = i;
        } else if (physical->sparse_binding_queue_family_index == UINT32_MAX && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_SPARSE_BINDING_BIT)) == VK_QUEUE_SPARSE_BINDING_BIT) {
            physical->has_async_sparse_binding = true;
            physical->sparse_binding_queue_family_index = i;
        } else if (physical->video_decode_queue_family_index == UINT32_MAX && (flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR)) == VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
            physical->has_async_video = true;
            physical->video_decode_queue_family_index = i;
        }
    }
    /* just for validation */
    if (physical->graphics_queue_family_index == UINT32_MAX) {
        log_warn("%s does not support graphics operations.", err);
        return;
    } else if (surface) {
        /* search for a queue family that will support presentation */
        struct hadal_interface *interface = (struct hadal_interface *)work->hadal;

        for (u32 i = 0; i < queue_family_count; i++) {
            /* pick the last queue family that supports presentation */
            if (interface->vulkan_physical_device_presentation_support(work->hadal, pelagia->instance, surface, physical->device, i, pelagia->vkGetInstanceProcAddr))
                physical->presentation_queue_family_index = i;
        }
        if (physical->presentation_queue_family_index != UINT32_MAX) {
            physical->presentation_support = true;
            physical->score += 200;
            log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                    "Device has presentation support using display '%s'.\n", interface->header.name.ptr);
        }
        pelagia->vkDestroySurfaceKHR(pelagia->instance, surface, NULL);
    }

    u32 graphics_family_queue_count = 1;
    u32 compute_family_queue_count = 1;

    u32 o = 0;
    physical->queue_family_indices[o++] = physical->graphics_queue_family_index;

    /* compute queue */
    if (physical->compute_queue_family_index == UINT32_MAX) {
        physical->compute_queue_family_index = physical->graphics_queue_family_index;
        if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
            graphics_family_queue_count++;
            physical->has_async_compute = true;
        }
        physical->async_compute_queue_family_queue_count = compute_family_queue_count = 0;
    } else {
        physical->queue_family_indices[o++] = physical->compute_queue_family_index;
    }
    /* transfer queue */
    if (physical->transfer_queue_family_index == UINT32_MAX) {
        if (compute_family_queue_count) {
            physical->transfer_queue_family_index = physical->compute_queue_family_index;
            if (physical->async_compute_queue_family_queue_count > compute_family_queue_count) {
                compute_family_queue_count++;
                physical->has_async_transfer = true;
            }
        } else {
            physical->transfer_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_transfer = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->transfer_queue_family_index;
    }
    /* sparse binding queue */
    if (physical->sparse_binding_queue_family_index == UINT32_MAX) {
        /* sparse binding is optional */
        if (graphics_queue_flags & VK_QUEUE_SPARSE_BINDING_BIT) {
            physical->sparse_binding_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_sparse_binding = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->sparse_binding_queue_family_index;
    }
    /* video decode queue */
    if (physical->video_decode_queue_family_index == UINT32_MAX) {
        /* video is optional */
        if (graphics_queue_flags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
            physical->video_decode_queue_family_index = physical->graphics_queue_family_index;
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_video = true;
            }
        }
    } else {
        physical->queue_family_indices[o++] = physical->video_decode_queue_family_index;
    }
    /* presentation queue, only if presentation supported */
    if (physical->presentation_support) {
        if (physical->presentation_queue_family_index == physical->graphics_queue_family_index) {
            if (physical->graphics_queue_family_queue_count > graphics_family_queue_count) {
                graphics_family_queue_count++;
                physical->has_async_present = true;
            }
        } else if (physical->presentation_queue_family_index == physical->compute_queue_family_index) {
            if (physical->async_compute_queue_family_queue_count > compute_family_queue_count) {
                compute_family_queue_count++;
                physical->has_async_present = true;
            }
        } else {
            assert_debug(!"spec validation");
        }
    }
    physical->graphics_queue_family_queue_count = graphics_family_queue_count;
    physical->async_compute_queue_family_queue_count = compute_family_queue_count;
    physical->queue_family_count = o;

    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
            "Specialized command queues:\n"
            "    Graphics          %11s, %u family index\n",
            "Yes", physical->graphics_queue_family_index);
    { /* log queues */
        /* in order: compute, transfer, sparse binding, video, present */
        b32 queue_supported[5] = {
            true, true, 
            physical->sparse_binding_queue_family_index != UINT32_MAX,
            physical->video_decode_queue_family_index != UINT32_MAX,
            physical->presentation_support,
        };
        u32 queue_families[5] = {
            physical->compute_queue_family_index,
            physical->transfer_queue_family_index,
            physical->sparse_binding_queue_family_index,
            physical->video_decode_queue_family_index,
            physical->presentation_queue_family_index,
        };
        b32 queue_async[5] = {
            physical->has_async_compute,
            physical->has_async_transfer,
            physical->has_async_sparse_binding,
            physical->has_async_video,
            physical->has_async_present,
        };
        const char *queue_names[5] = { "Compute", "Transfer", "Sparse binding", "Video", "Present"};

        for (u32 i = 0; i < 5; i++) {
            log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
                    "    %-17s %11s", queue_names[i], queue_supported[i] ? (queue_async[i] ? "Async" : "Overloaded") : "No");
            if (queue_supported[i])
                log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, ", %u family index", queue_families[i]);
            log_buffer[log_offset++] = '\n';
        }
    }

    b32 valid_ext = true;
    /* resolve extensions (we require dynamic_rendering, dynamic_rendering_local_read and synchronization2) */
    VERIFY_VK(pelagia->vkEnumerateDeviceExtensionProperties(physical->device, NULL, &extension_count, extension_properties));
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_swapchain_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_DEVICE_FAULT_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_device_fault_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_memory_budget_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_memory_priority_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_EXT_MESH_SHADER_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_mesh_shader_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_fragment_shading_rate_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_deferred_host_operations_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_acceleration_structure_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_pipeline_library_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_QUERY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_ray_query_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_raytracing_pipeline_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_raytracing_maintenance1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_queue_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_queue_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_av1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_decode_h264_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_video_maintenance1_bit;
    if (vulkan_query_extension(extension_properties, extension_count, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
        physical->extension_bits |= vulkan_ext_amd_device_coherent_memory_bit;
    /* for backwards compatiblity */
    if (!has_vk14) {
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_dynamic_rendering_local_read_bit;
        } else if (!physical->dynamic_rendering_local_read_features.dynamicRenderingLocalRead) {
            log_warn("%s does not support a required extension: %s.", err, VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 100;
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_MAINTENANCE_5_EXTENSION_NAME))
            physical->extension_bits |= vulkan_ext_maintenance5_bit;
    }
    if (!has_vk13) {
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_dynamic_rendering_bit;
        } else if (!physical->dynamic_rendering_features.dynamicRendering) {
            log_warn("%s does not support a required extension: %s.", err, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 100;
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
            physical->extension_bits |= vulkan_ext_synchronization2_bit;
        } else if (!physical->synchronization2_features.synchronization2) {
            log_warn("%s does not support a required extension: %s.", err, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
            valid_ext = false;
            physical->score -= 100;
        }
        if (vulkan_query_extension(extension_properties, extension_count, VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
            physical->extension_bits |= vulkan_ext_maintenance4_bit;
    }
#if VULKAN_VALIDATE_REQUIRED_EXTENSIONS
    if (!valid_ext) {
        log_warn("DISCARD: %s is not suitable for our rendering work (lack of core extensions support).", err);
        return;
    }
#endif
    /* check for the swapchain */
    if (!(physical->extension_bits & vulkan_ext_swapchain_bit)) {
        log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset, 
                "Device has no swapchain support%s", physical->presentation_support ? ", presentation is not possible." : ".");
        physical->presentation_queue_family_index = UINT32_MAX;
        physical->presentation_support = false;
        physical->score -= 200;
    }

    /* resolve video capabilities */
    VERIFY_VK(pelagia->vkGetPhysicalDeviceVideoFormatPropertiesKHR(physical->device, &video_format, &video_format_properties_count, video_format_properties));
    /* TODO video AV1 H.264 decode */

    /* add features to score */
    b32 has_swapchain = physical->presentation_support;
    b32 has_sparse_binding = physical->features2.features.sparseBinding;
    b32 has_dynamic_local_read = (physical->features_14.dynamicRenderingLocalRead || physical->dynamic_rendering_local_read_features.dynamicRenderingLocalRead); 
    b32 has_dynamic_rendering = (physical->features_13.dynamicRendering || physical->dynamic_rendering_features.dynamicRendering); 
    b32 has_synchronization2 = (physical->features_13.synchronization2 || physical->synchronization2_features.synchronization2);
    b32 has_descriptor_indexing = (physical->features_12.descriptorIndexing);
    b32 has_fragment_shading_rate = physical->fragment_shading_rate_features.primitiveFragmentShadingRate;
    b32 has_acceleration_structures = physical->acceleration_structure_features.accelerationStructure + physical->acceleration_structure_features.accelerationStructureHostCommands;
    b32 has_raytracing = physical->raytracing_pipeline_features.rayTracingPipeline + physical->raytracing_pipeline_features.rayTracingPipelineTraceRaysIndirect;
    b32 has_ray_query = physical->ray_query_features.rayQuery;
    b32 has_mesh_shader = physical->mesh_shader_features.meshShader + physical->mesh_shader_features.multiviewMeshShader + physical->mesh_shader_features.primitiveFragmentShadingRateMeshShader;
    b32 has_video = physical->has_async_video + ((physical->extension_bits & vulkan_ext_video_decode_av1_bit) ? 1 : 0) + ((physical->extension_bits & vulkan_ext_video_decode_h264_bit) ? 1 : 0);
    b32 has_mix_vk10 = physical->features2.features.sampleRateShading + physical->features2.features.fragmentStoresAndAtomics + physical->features2.features.samplerAnisotropy
        + physical->features2.features.multiDrawIndirect + physical->features2.features.multiViewport + physical->features2.features.occlusionQueryPrecise + physical->features2.features.pipelineStatisticsQuery;
    b32 has_mix_vk11 = physical->features_11.multiview + physical->features_11.uniformAndStorageBuffer16BitAccess;
    b32 has_mix_vk12 = physical->features_12.drawIndirectCount + physical->features_12.hostQueryReset;
    b32 has_mix_vk13 = physical->features_13.maintenance4 + physical->features_13.computeFullSubgroups;
    b32 has_mix_vk14 = physical->features_14.pushDescriptor + physical->features_14.maintenance5 + physical->features_14.maintenance6;

    u32 has_features =
        has_swapchain +
        has_sparse_binding +
        has_dynamic_local_read +
        has_dynamic_rendering +
        has_synchronization2 +
        has_descriptor_indexing +
        has_fragment_shading_rate +
        has_acceleration_structures +
        has_raytracing +
        has_ray_query +
        has_mesh_shader +
        has_video +
        has_mix_vk10 +
        has_mix_vk11 +
        has_mix_vk12 +
        has_mix_vk13 +
        has_mix_vk14;
    physical->score += 51 * has_features;

    if (has_swapchain)
        physical->pelagia_features |= pelagia_feature_swapchain;
    if (has_sparse_binding)
        physical->pelagia_features |= pelagia_feature_sparse_binding;
    if (has_acceleration_structures)
        physical->pelagia_features |= pelagia_feature_acceleration_structures;
    if (has_raytracing)
        physical->pelagia_features |= pelagia_feature_raytracing_pipeline;
    if (has_ray_query)
        physical->pelagia_features |= pelagia_feature_ray_query;
    if (has_mesh_shader)
        physical->pelagia_features |= pelagia_feature_mesh_shader;
    if (physical->extension_bits & vulkan_ext_video_decode_av1_bit)
        physical->pelagia_features |= pelagia_feature_decode_av1;
    if (physical->extension_bits & vulkan_ext_video_decode_h264_bit)
        physical->pelagia_features |= pelagia_feature_decode_h264;

#define CHECKBOOLEAN(bo) (bo > 0 ? "Yes" : "No")
    log_offset += snprintf(log_buffer + log_offset, log_buffer_size - log_offset,
        "Optional GPU features:\n"
        "    Multi viewport            %s\n"
        "    Descriptor indexing       %s\n"
        "    Fragment shading rate     %s\n"
        "    Acceleration structures   %s\n"
        "    Raytracing pipelines      %s\n"
        "    Ray queries               %s\n"
        "    Mesh shaders              %s\n"
        "    Sparse binding            %s\n"
        "    Video AV1 decode          %s\n"
        "    Video H.264 decode        %s\n"
        "Scored a total of %u points from the query.",
        CHECKBOOLEAN(physical->features2.features.multiViewport),
        CHECKBOOLEAN(has_descriptor_indexing),
        CHECKBOOLEAN(has_fragment_shading_rate),
        CHECKBOOLEAN(has_acceleration_structures),
        CHECKBOOLEAN(has_raytracing),
        CHECKBOOLEAN(has_ray_query),
        CHECKBOOLEAN(has_mesh_shader),
        CHECKBOOLEAN(has_sparse_binding),
        CHECKBOOLEAN((physical->extension_bits & vulkan_ext_video_decode_av1_bit)),
        CHECKBOOLEAN((physical->extension_bits & vulkan_ext_video_decode_h264_bit)),
        physical->score);
#undef CHECKBOOLEAN

    log_buffer[log_offset] = '\0';
    log_info("%s", log_buffer);

    /* validate the device ;3 */
    work->header.result = result_success;
}

s32 _pelagia_vulkan_query_physical_devices(
    struct pelagia                      *pelagia, 
    const struct hadal                  *hadal,
    u32                                 *out_device_count, 
    struct pelagia_physical_device_info *out_devices)
{
    assert_debug(pelagia && out_device_count);

    if (!pelagia->physical_device_count) {
        u32 physical_device_count = 0;
        VERIFY_VK(pelagia->vkEnumeratePhysicalDevices(pelagia->instance, &physical_device_count, NULL));
        assert_debug(physical_device_count); /* we checked the devices before at encore */

        usize handle_bytes = A16(sizeof(VkPhysicalDevice) * physical_device_count);
        usize physical_device_bytes = A16(sizeof(struct vulkan_physical_device) * physical_device_count); 
        usize riven_work_bytes = A16(sizeof(struct riven_work) * physical_device_count); 
        usize query_work_bytes = A16(sizeof(struct physical_device_query_work) * physical_device_count);
        usize index_bytes = A16(sizeof(u32) * physical_device_count);
        usize total_bytes = handle_bytes + physical_device_bytes + riven_work_bytes + query_work_bytes + index_bytes;

        u8 *raw = (u8 *)riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, total_bytes, 16);
        memset(raw, 0, total_bytes);

        usize o = 0;
        VkPhysicalDevice *handles = (VkPhysicalDevice *)&raw[o];
        VERIFY_VK(pelagia->vkEnumeratePhysicalDevices(pelagia->instance, &physical_device_count, handles));
        o += handle_bytes;
        struct vulkan_physical_device *physical_devices = (struct vulkan_physical_device *)&raw[o]; 
        o += physical_device_bytes;
        struct riven_work *work = (struct riven_work *)&raw[o];
        o += riven_work_bytes;
        struct physical_device_query_work *query_work = (struct physical_device_query_work *)&raw[o];
        o += query_work_bytes;
        u32 *indices = (u32 *)&raw[o];
        assert_debug(o + index_bytes == total_bytes);

        for (u32 i = 0; i < physical_device_count; i++) {
            physical_devices[i].device = handles[i];
            work[i].name = str_init("vulkan:physical_device_query");
            work[i].argument = &query_work[i];
            work[i].procedure = (PFN_riven_work)physical_device_query;
            query_work[i].header.result = result_error;
            query_work[i].header.index = i;
            query_work[i].pelagia = pelagia;
            query_work[i].hadal = hadal;
            query_work[i].device = &physical_devices[i];
        }
        riven_split_work_and_unchain(pelagia->interface.header.riven, work, physical_device_count);

        o = 0;
        /* collect indices */
        for (u32 i = 0; i < physical_device_count; i++) {
            if (query_work[i].header.result == result_success) 
                indices[o] = i; 
            o++;
        }
        if (o == 0) {
            log_error("Vulkan device query processed %u physical devices, and invalidated all of them. Can't continue.", physical_device_count);
            return result_error;
        }

        /* write the accepted physical devices into the backend state */
        struct vulkan_physical_device *accepted = (struct vulkan_physical_device *)
            riven_alloc(pelagia->interface.header.riven, pelagia->interface.header.tag, sizeof(struct vulkan_physical_device) * o, _Alignof(struct vulkan_physical_device));
        for (u32 i = 0; i < o; i++)
            memcpy(&accepted[i], &physical_devices[indices[i]], sizeof(struct vulkan_physical_device));
        pelagia->physical_devices = accepted;
        pelagia->physical_device_count = o;

        if (!out_devices) {
            *out_device_count = pelagia->physical_device_count;
            return result_success;
        }
        assert_debug(*out_device_count >= pelagia->physical_device_count);
    }
    assert_debug(out_devices);

    /* write the physical device info */
    for (u32 i = 0; i < pelagia->physical_device_count; i++) {
        struct pelagia_physical_device_info *out = &out_devices[i];
        const struct vulkan_physical_device *info = &pelagia->physical_devices[i]; 

        out->name.ptr = info->properties2.properties.deviceName;
        out->name.length = strlen(info->properties2.properties.deviceName);
        out->device_type = info->properties2.properties.deviceType;
        out->vendor_id = info->properties2.properties.vendorID;
        out->features = info->pelagia_features;
        out->api_version = info->properties2.properties.apiVersion;
        out->total_vram = info->total_vram;
        out->score = info->score;
        out->presentation = info->presentation_support;
        out->index = i;
    }

    *out_device_count = pelagia->physical_device_count;
    return result_success;
}

static PFN_vkVoidFunction device_proc_address(
    const struct pelagia_device *device, 
    const char                  *procname)
{
    PFN_vkVoidFunction address = device->pelagia->vkGetDeviceProcAddr(device->logical, procname);
    if (address == NULL)
        log_warn("'pelagia_vulkan' can't find device procedure of name '%s'.", procname);
    return address;
}

void _pelagia_vulkan_create_device(struct pelagia_device_create_info *create_info)
{
    assert_debug(create_info && create_info->pelagia && create_info->physical_device && create_info->write_device);

    const struct pelagia *pelagia = create_info->pelagia;
    const struct vulkan_physical_device *physical = &pelagia->physical_devices[create_info->physical_device->index];

    create_info->allocation.size = sizeof(struct pelagia_device);
    create_info->allocation.alignment = _Alignof(struct pelagia_device);
    *create_info->write_device = NULL;

    if (create_info->allocation.tag == riven_tag_invalid && !create_info->allocation.memory) {
        assert_debug(create_info->work_header.result != result_allocation_query);
        create_info->work_header.result = result_allocation_query;
        return;
    }
    create_info->work_header.result = result_error;

    f32 queue_priorities[6] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };
    VkDeviceQueueCreateInfo queue_infos[VULKAN_QUEUE_FAMILY_INDICES_COUNT];

    for (u32 i = 0; i < physical->queue_family_count; i++) {
        u32 family = physical->queue_family_indices[i];
        queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_infos[i].pNext = NULL;
        queue_infos[i].flags = 0;
        queue_infos[i].pQueuePriorities = queue_priorities;
        queue_infos[i].queueFamilyIndex = family;
        queue_infos[i].queueCount = 1;
        if (family == physical->graphics_queue_family_index) {
            queue_infos[i].queueCount = physical->graphics_queue_family_queue_count;
        } else if (family == physical->compute_queue_family_index) {
            queue_infos[i].queueCount = physical->async_compute_queue_family_queue_count;
        }
    }

    u32 o = 0;
    u32 extension_count = bits_popcnt_lookup((const u8 *)&physical->extension_bits, sizeof(u64));
    const char **extensions = riven_alloc(pelagia->interface.header.riven, riven_tag_deferred, sizeof(const char *) * extension_count, _Alignof(const char *));
    if (physical->extension_bits & vulkan_ext_swapchain_bit)
        extensions[o++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_device_fault_bit)
        extensions[o++] = VK_EXT_DEVICE_FAULT_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_memory_budget_bit)
        extensions[o++] = VK_EXT_MEMORY_BUDGET_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_memory_priority_bit)
        extensions[o++] = VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_mesh_shader_bit)
        extensions[o++] = VK_EXT_MESH_SHADER_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_fragment_shading_rate_bit)
        extensions[o++] = VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_deferred_host_operations_bit)
        extensions[o++] = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_acceleration_structure_bit)
        extensions[o++] = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_pipeline_library_bit)
        extensions[o++] = VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_ray_query_bit)
        extensions[o++] = VK_KHR_RAY_QUERY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_raytracing_pipeline_bit)
        extensions[o++] = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_raytracing_maintenance1_bit)
        extensions[o++] = VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_queue_bit)
        extensions[o++] = VK_KHR_VIDEO_QUEUE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_queue_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_av1_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_AV1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_decode_h264_bit)
        extensions[o++] = VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_video_maintenance1_bit)
        extensions[o++] = VK_KHR_VIDEO_MAINTENANCE_1_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_amd_device_coherent_memory_bit)
        extensions[o++] = VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_dynamic_rendering_local_read_bit)
        extensions[o++] = VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_dynamic_rendering_bit)
        extensions[o++] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_synchronization2_bit)
        extensions[o++] = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_maintenance5_bit)
        extensions[o++] = VK_KHR_MAINTENANCE_5_EXTENSION_NAME;
    if (physical->extension_bits & vulkan_ext_maintenance4_bit)
        extensions[o++] = VK_KHR_MAINTENANCE_4_EXTENSION_NAME;
    assert_debug(extension_count == o);

    struct next_feat {
        VkStructureType sType;
        void           *pNext;
    } *feat = (struct next_feat *)&physical->features_11;

#define ENABLE_FEATURE(_feature) \
    { feat->pNext = (void *)&physical->_feature; feat = feat->pNext; feat->pNext = NULL; }

    ENABLE_FEATURE(features_12);
    if (physical->properties2.properties.apiVersion >= VK_API_VERSION_1_3) {
        ENABLE_FEATURE(features_13) 
    } else {
        if (physical->dynamic_rendering_features.dynamicRendering) 
            ENABLE_FEATURE(dynamic_rendering_features);
        if (physical->synchronization2_features.synchronization2) 
            ENABLE_FEATURE(synchronization2_features);
    }
    if (physical->properties2.properties.apiVersion >= VK_API_VERSION_1_4) {
        ENABLE_FEATURE(features_14)
    } else {
        if (physical->dynamic_rendering_local_read_features.dynamicRenderingLocalRead) 
            ENABLE_FEATURE(dynamic_rendering_local_read_features);
    }
    if (physical->fragment_shading_rate_features.primitiveFragmentShadingRate)
        ENABLE_FEATURE(fragment_shading_rate_features);
    if (physical->pelagia_features & pelagia_feature_mesh_shader)
        ENABLE_FEATURE(mesh_shader_features);
    if (physical->pelagia_features & pelagia_feature_ray_query)
        ENABLE_FEATURE(ray_query_features);
    if (physical->pelagia_features & pelagia_feature_raytracing_pipeline)
        ENABLE_FEATURE(raytracing_pipeline_features);
    if (physical->pelagia_features & pelagia_feature_acceleration_structures)
        ENABLE_FEATURE(acceleration_structure_features)
#undef ENABLE_FEATURE

    VkDevice logical;
    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = physical->features2.pNext,
        .flags = 0,
        .pEnabledFeatures = &physical->features2.features,
        .ppEnabledExtensionNames = (const char * const *)extensions,
        .enabledExtensionCount = extension_count,
        .ppEnabledLayerNames = NULL,
        .pQueueCreateInfos = queue_infos,
        .queueCreateInfoCount = physical->queue_family_count,
    };
    VERIFY_VK(pelagia->vkCreateDevice(physical->device, &device_info, NULL, &logical));

    struct pelagia_device *device = NULL;
    /* allocate the device state */
    if (create_info->allocation.tag) {
        device = (struct pelagia_device *)
            riven_alloc(pelagia->interface.header.riven, create_info->allocation.tag, sizeof(struct pelagia_device), _Alignof(struct pelagia_device));
    } else {
        assert_debug(create_info->allocation.memory);
        device = (struct pelagia_device *)create_info->allocation.memory;
    }
    zerop(device);

    device->pelagia = pelagia;
    device->logical = logical;
    device->physical = physical;
    device->frames_in_flight = max(1, min(create_info->frames_in_flight, PELAGIA_MAX_FRAMES_IN_FLIGHT));

    /* core 1.0 */
	device->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)device_proc_address(device, "vkAllocateCommandBuffers");
	device->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)device_proc_address(device, "vkAllocateDescriptorSets");
	device->vkAllocateMemory = (PFN_vkAllocateMemory)device_proc_address(device, "vkAllocateMemory");
	device->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)device_proc_address(device, "vkBeginCommandBuffer");
	device->vkBindBufferMemory = (PFN_vkBindBufferMemory)device_proc_address(device, "vkBindBufferMemory");
	device->vkBindImageMemory = (PFN_vkBindImageMemory)device_proc_address(device, "vkBindImageMemory");
	device->vkCmdBeginQuery = (PFN_vkCmdBeginQuery)device_proc_address(device, "vkCmdBeginQuery");
	device->vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)device_proc_address(device, "vkCmdBeginRenderPass");
	device->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)device_proc_address(device, "vkCmdBindDescriptorSets");
	device->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)device_proc_address(device, "vkCmdBindIndexBuffer");
	device->vkCmdBindPipeline = (PFN_vkCmdBindPipeline)device_proc_address(device, "vkCmdBindPipeline");
	device->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)device_proc_address(device, "vkCmdBindVertexBuffers");
	device->vkCmdBlitImage = (PFN_vkCmdBlitImage)device_proc_address(device, "vkCmdBlitImage");
	device->vkCmdClearAttachments = (PFN_vkCmdClearAttachments)device_proc_address(device, "vkCmdClearAttachments");
	device->vkCmdClearColorImage = (PFN_vkCmdClearColorImage)device_proc_address(device, "vkCmdClearColorImage");
	device->vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)device_proc_address(device, "vkCmdClearDepthStencilImage");
	device->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)device_proc_address(device, "vkCmdCopyBuffer");
	device->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)device_proc_address(device, "vkCmdCopyBufferToImage");
	device->vkCmdCopyImage = (PFN_vkCmdCopyImage)device_proc_address(device, "vkCmdCopyImage");
	device->vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)device_proc_address(device, "vkCmdCopyImageToBuffer");
	device->vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)device_proc_address(device, "vkCmdCopyQueryPoolResults");
	device->vkCmdDispatch = (PFN_vkCmdDispatch)device_proc_address(device, "vkCmdDispatch");
	device->vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)device_proc_address(device, "vkCmdDispatchIndirect");
	device->vkCmdDraw = (PFN_vkCmdDraw)device_proc_address(device, "vkCmdDraw");
	device->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)device_proc_address(device, "vkCmdDrawIndexed");
	device->vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)device_proc_address(device, "vkCmdDrawIndexedIndirect");
	device->vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)device_proc_address(device, "vkCmdDrawIndirect");
	device->vkCmdEndQuery = (PFN_vkCmdEndQuery)device_proc_address(device, "vkCmdEndQuery");
	device->vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)device_proc_address(device, "vkCmdEndRenderPass");
	device->vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)device_proc_address(device, "vkCmdExecuteCommands");
	device->vkCmdFillBuffer = (PFN_vkCmdFillBuffer)device_proc_address(device, "vkCmdFillBuffer");
	device->vkCmdNextSubpass = (PFN_vkCmdNextSubpass)device_proc_address(device, "vkCmdNextSubpass");
	device->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)device_proc_address(device, "vkCmdPipelineBarrier");
	device->vkCmdPushConstants = (PFN_vkCmdPushConstants)device_proc_address(device, "vkCmdPushConstants");
	device->vkCmdResetEvent = (PFN_vkCmdResetEvent)device_proc_address(device, "vkCmdResetEvent");
	device->vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)device_proc_address(device, "vkCmdResetQueryPool");
	device->vkCmdResolveImage = (PFN_vkCmdResolveImage)device_proc_address(device, "vkCmdResolveImage");
	device->vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)device_proc_address(device, "vkCmdSetBlendConstants");
	device->vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)device_proc_address(device, "vkCmdSetDepthBias");
	device->vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)device_proc_address(device, "vkCmdSetDepthBounds");
	device->vkCmdSetEvent = (PFN_vkCmdSetEvent)device_proc_address(device, "vkCmdSetEvent");
	device->vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)device_proc_address(device, "vkCmdSetLineWidth");
	device->vkCmdSetScissor = (PFN_vkCmdSetScissor)device_proc_address(device, "vkCmdSetScissor");
	device->vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)device_proc_address(device, "vkCmdSetStencilCompareMask");
	device->vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)device_proc_address(device, "vkCmdSetStencilReference");
	device->vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)device_proc_address(device, "vkCmdSetStencilWriteMask");
	device->vkCmdSetViewport = (PFN_vkCmdSetViewport)device_proc_address(device, "vkCmdSetViewport");
	device->vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)device_proc_address(device, "vkCmdUpdateBuffer");
	device->vkCmdWaitEvents = (PFN_vkCmdWaitEvents)device_proc_address(device, "vkCmdWaitEvents");
	device->vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)device_proc_address(device, "vkCmdWriteTimestamp");
	device->vkCreateBuffer = (PFN_vkCreateBuffer)device_proc_address(device, "vkCreateBuffer");
	device->vkCreateBufferView = (PFN_vkCreateBufferView)device_proc_address(device, "vkCreateBufferView");
	device->vkCreateCommandPool = (PFN_vkCreateCommandPool)device_proc_address(device, "vkCreateCommandPool");
	device->vkCreateComputePipelines = (PFN_vkCreateComputePipelines)device_proc_address(device, "vkCreateComputePipelines");
	device->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)device_proc_address(device, "vkCreateDescriptorPool");
	device->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)device_proc_address(device, "vkCreateDescriptorSetLayout");
	device->vkCreateEvent = (PFN_vkCreateEvent)device_proc_address(device, "vkCreateEvent");
	device->vkCreateFence = (PFN_vkCreateFence)device_proc_address(device, "vkCreateFence");
	device->vkCreateFramebuffer = (PFN_vkCreateFramebuffer)device_proc_address(device, "vkCreateFramebuffer");
	device->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)device_proc_address(device, "vkCreateGraphicsPipelines");
	device->vkCreateImage = (PFN_vkCreateImage)device_proc_address(device, "vkCreateImage");
	device->vkCreateImageView = (PFN_vkCreateImageView)device_proc_address(device, "vkCreateImageView");
	device->vkCreatePipelineCache = (PFN_vkCreatePipelineCache)device_proc_address(device, "vkCreatePipelineCache");
	device->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)device_proc_address(device, "vkCreatePipelineLayout");
	device->vkCreateQueryPool = (PFN_vkCreateQueryPool)device_proc_address(device, "vkCreateQueryPool");
	device->vkCreateRenderPass = (PFN_vkCreateRenderPass)device_proc_address(device, "vkCreateRenderPass");
	device->vkCreateSampler = (PFN_vkCreateSampler)device_proc_address(device, "vkCreateSampler");
	device->vkCreateSemaphore = (PFN_vkCreateSemaphore)device_proc_address(device, "vkCreateSemaphore");
	device->vkCreateShaderModule = (PFN_vkCreateShaderModule)device_proc_address(device, "vkCreateShaderModule");
	device->vkDestroyBuffer = (PFN_vkDestroyBuffer)device_proc_address(device, "vkDestroyBuffer");
	device->vkDestroyBufferView = (PFN_vkDestroyBufferView)device_proc_address(device, "vkDestroyBufferView");
	device->vkDestroyCommandPool = (PFN_vkDestroyCommandPool)device_proc_address(device, "vkDestroyCommandPool");
	device->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)device_proc_address(device, "vkDestroyDescriptorPool");
	device->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)device_proc_address(device, "vkDestroyDescriptorSetLayout");
	device->vkDestroyDevice = (PFN_vkDestroyDevice)device_proc_address(device, "vkDestroyDevice");
	device->vkDestroyEvent = (PFN_vkDestroyEvent)device_proc_address(device, "vkDestroyEvent");
	device->vkDestroyFence = (PFN_vkDestroyFence)device_proc_address(device, "vkDestroyFence");
	device->vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)device_proc_address(device, "vkDestroyFramebuffer");
	device->vkDestroyImage = (PFN_vkDestroyImage)device_proc_address(device, "vkDestroyImage");
	device->vkDestroyImageView = (PFN_vkDestroyImageView)device_proc_address(device, "vkDestroyImageView");
	device->vkDestroyPipeline = (PFN_vkDestroyPipeline)device_proc_address(device, "vkDestroyPipeline");
	device->vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)device_proc_address(device, "vkDestroyPipelineCache");
	device->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)device_proc_address(device, "vkDestroyPipelineLayout");
	device->vkDestroyQueryPool = (PFN_vkDestroyQueryPool)device_proc_address(device, "vkDestroyQueryPool");
	device->vkDestroyRenderPass = (PFN_vkDestroyRenderPass)device_proc_address(device, "vkDestroyRenderPass");
	device->vkDestroySampler = (PFN_vkDestroySampler)device_proc_address(device, "vkDestroySampler");
	device->vkDestroySemaphore = (PFN_vkDestroySemaphore)device_proc_address(device, "vkDestroySemaphore");
	device->vkDestroyShaderModule = (PFN_vkDestroyShaderModule)device_proc_address(device, "vkDestroyShaderModule");
	device->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)device_proc_address(device, "vkDeviceWaitIdle");
	device->vkEndCommandBuffer = (PFN_vkEndCommandBuffer)device_proc_address(device, "vkEndCommandBuffer");
	device->vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)device_proc_address(device, "vkFlushMappedMemoryRanges");
	device->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)device_proc_address(device, "vkFreeCommandBuffers");
	device->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)device_proc_address(device, "vkFreeDescriptorSets");
	device->vkFreeMemory = (PFN_vkFreeMemory)device_proc_address(device, "vkFreeMemory");
	device->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)device_proc_address(device, "vkGetBufferMemoryRequirements");
	device->vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)device_proc_address(device, "vkGetDeviceMemoryCommitment");
	device->vkGetDeviceQueue = (PFN_vkGetDeviceQueue)device_proc_address(device, "vkGetDeviceQueue");
	device->vkGetEventStatus = (PFN_vkGetEventStatus)device_proc_address(device, "vkGetEventStatus");
	device->vkGetFenceStatus = (PFN_vkGetFenceStatus)device_proc_address(device, "vkGetFenceStatus");
	device->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)device_proc_address(device, "vkGetImageMemoryRequirements");
	device->vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)device_proc_address(device, "vkGetImageSparseMemoryRequirements");
	device->vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)device_proc_address(device, "vkGetImageSubresourceLayout");
	device->vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)device_proc_address(device, "vkGetPipelineCacheData");
	device->vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)device_proc_address(device, "vkGetQueryPoolResults");
	device->vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)device_proc_address(device, "vkGetRenderAreaGranularity");
	device->vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)device_proc_address(device, "vkInvalidateMappedMemoryRanges");
	device->vkMapMemory = (PFN_vkMapMemory)device_proc_address(device, "vkMapMemory");
	device->vkMergePipelineCaches = (PFN_vkMergePipelineCaches)device_proc_address(device, "vkMergePipelineCaches");
	device->vkQueueBindSparse = (PFN_vkQueueBindSparse)device_proc_address(device, "vkQueueBindSparse");
	device->vkQueueSubmit = (PFN_vkQueueSubmit)device_proc_address(device, "vkQueueSubmit");
	device->vkQueueWaitIdle = (PFN_vkQueueWaitIdle)device_proc_address(device, "vkQueueWaitIdle");
	device->vkResetCommandBuffer = (PFN_vkResetCommandBuffer)device_proc_address(device, "vkResetCommandBuffer");
	device->vkResetCommandPool = (PFN_vkResetCommandPool)device_proc_address(device, "vkResetCommandPool");
	device->vkResetDescriptorPool = (PFN_vkResetDescriptorPool)device_proc_address(device, "vkResetDescriptorPool");
	device->vkResetEvent = (PFN_vkResetEvent)device_proc_address(device, "vkResetEvent");
	device->vkResetFences = (PFN_vkResetFences)device_proc_address(device, "vkResetFences");
	device->vkSetEvent = (PFN_vkSetEvent)device_proc_address(device, "vkSetEvent");
	device->vkUnmapMemory = (PFN_vkUnmapMemory)device_proc_address(device, "vkUnmapMemory");
	device->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)device_proc_address(device, "vkUpdateDescriptorSets");
	device->vkWaitForFences = (PFN_vkWaitForFences)device_proc_address(device, "vkWaitForFences");

    /* core 1.1 */
    device->vkBindBufferMemory2 = (PFN_vkBindBufferMemory2)device_proc_address(device, "vkBindBufferMemory2");
    device->vkBindImageMemory2 = (PFN_vkBindImageMemory2)device_proc_address(device, "vkBindImageMemory2");
    device->vkCmdDispatchBase = (PFN_vkCmdDispatchBase)device_proc_address(device, "vkCmdDispatchBase");
    device->vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask)device_proc_address(device, "vkCmdSetDeviceMask");
    device->vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate)device_proc_address(device, "vkCreateDescriptorUpdateTemplate");
    device->vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion)device_proc_address(device, "vkCreateSamplerYcbcrConversion");
    device->vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate)device_proc_address(device, "vkDestroyDescriptorUpdateTemplate");
    device->vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion)device_proc_address(device, "vkDestroySamplerYcbcrConversion");
    device->vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2)device_proc_address(device, "vkGetBufferMemoryRequirements2");
    device->vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport)device_proc_address(device, "vkGetDescriptorSetLayoutSupport");
    device->vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures)device_proc_address(device, "vkGetDeviceGroupPeerMemoryFeatures");
    device->vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2)device_proc_address(device, "vkGetDeviceQueue2");
    device->vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2)device_proc_address(device, "vkGetImageMemoryRequirements2");
    device->vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2)device_proc_address(device, "vkGetImageSparseMemoryRequirements2");
    device->vkTrimCommandPool = (PFN_vkTrimCommandPool)device_proc_address(device, "vkTrimCommandPool");
    device->vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate)device_proc_address(device, "vkUpdateDescriptorSetWithTemplate");

    /* core 1.2 */
    device->vkCmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount)device_proc_address(device, "vkCmdDrawIndexedIndirectCount");
    device->vkCmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount)device_proc_address(device, "vkCmdDrawIndirectCount");
    device->vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)device_proc_address(device, "vkGetBufferDeviceAddress");
    device->vkGetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress)device_proc_address(device, "vkGetBufferOpaqueCaptureAddress");
    device->vkGetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)device_proc_address(device, "vkGetDeviceMemoryOpaqueCaptureAddress");
    device->vkGetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue)device_proc_address(device, "vkGetSemaphoreCounterValue");
    device->vkResetQueryPool = (PFN_vkResetQueryPool)device_proc_address(device, "vkResetQueryPool");
    device->vkSignalSemaphore = (PFN_vkSignalSemaphore)device_proc_address(device, "vkSignalSemaphore");
    device->vkWaitSemaphores = (PFN_vkWaitSemaphores)device_proc_address(device, "vkWaitSemaphores");

    /* core 1.3 */
    if (physical->properties2.properties.apiVersion >= VK_API_VERSION_1_3) {
        device->vkCmdBeginRendering = (PFN_vkCmdBeginRendering)device_proc_address(device, "vkCmdBeginRendering");
        device->vkCmdEndRendering = (PFN_vkCmdEndRendering)device_proc_address(device, "vkCmdEndRendering");
        device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2)device_proc_address(device, "vkCmdPipelineBarrier2");
        device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2)device_proc_address(device, "vkCmdResetEvent2");
        device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2)device_proc_address(device, "vkCmdSetEvent2");
        device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2)device_proc_address(device, "vkCmdWaitEvents2");
        device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2)device_proc_address(device, "vkCmdWriteTimestamp2");
        device->vkQueueSubmit2 = (PFN_vkQueueSubmit2)device_proc_address(device, "vkQueueSubmit2");
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements)device_proc_address(device, "vkGetDeviceBufferMemoryRequirements");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements)device_proc_address(device, "vkGetDeviceImageMemoryRequirements");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements)device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirements");
    } else {
        device->vkCmdBeginRendering = (PFN_vkCmdBeginRenderingKHR)device_proc_address(device, "vkCmdBeginRenderingKHR");
        device->vkCmdEndRendering = (PFN_vkCmdEndRenderingKHR)device_proc_address(device, "vkCmdEndRenderingKHR");
        device->vkCmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2KHR)device_proc_address(device, "vkCmdPipelineBarrier2KHR");
        device->vkCmdResetEvent2 = (PFN_vkCmdResetEvent2KHR)device_proc_address(device, "vkCmdResetEvent2KHR");
        device->vkCmdSetEvent2 = (PFN_vkCmdSetEvent2KHR)device_proc_address(device, "vkCmdSetEvent2KHR");
        device->vkCmdWaitEvents2 = (PFN_vkCmdWaitEvents2KHR)device_proc_address(device, "vkCmdWaitEvents2KHR");
        device->vkCmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2KHR)device_proc_address(device, "vkCmdWriteTimestamp2KHR");
        device->vkQueueSubmit2 = (PFN_vkQueueSubmit2KHR)device_proc_address(device, "vkQueueSubmit2KHR");

        /* maintenance */
        device->vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)device_proc_address(device, "vkGetDeviceBufferMemoryRequirementsKHR");
        device->vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirementsKHR)device_proc_address(device, "vkGetDeviceImageMemoryRequirementsKHR");
        device->vkGetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)device_proc_address(device, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    }

    /* core 1.4 */
    if (physical->properties2.properties.apiVersion >= VK_API_VERSION_1_4) {
        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2)device_proc_address(device, "vkCmdBindIndexBuffer2");
        device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations)device_proc_address(device, "vkCmdSetRenderingAttachmentLocations");
        device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices)device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndices");
        device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout)device_proc_address(device, "vkCmdBindIndexBuffer2");
        device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2)device_proc_address(device, "vkGetImageSubresourceLayout2");
        device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity)device_proc_address(device, "vkGetRenderingAreaGranularity");
    } else {
        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
        device->vkCmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)device_proc_address(device, "vkCmdSetRenderingAttachmentLocationsKHR");
        device->vkCmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)device_proc_address(device, "vkCmdSetRenderingInputAttachmentIndicesKHR");

        /* maintenance 5 */
        device->vkCmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2KHR)device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
        device->vkGetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayoutKHR)device_proc_address(device, "vkCmdBindIndexBuffer2KHR");
        device->vkGetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2KHR)device_proc_address(device, "vkGetImageSubresourceLayout2KHR");
        device->vkGetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularityKHR)device_proc_address(device, "vkGetRenderingAreaGranularityKHR");
    }

    /* swapchain */
    if (physical->extension_bits & vulkan_ext_swapchain_bit) {
        device->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)device_proc_address(device, "vkAcquireNextImageKHR");
        device->vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR)device_proc_address(device, "vkAcquireNextImage2KHR");
        device->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device_proc_address(device, "vkCreateSwapchainKHR");
        device->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device_proc_address(device, "vkDestroySwapchainKHR");
        device->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device_proc_address(device, "vkGetSwapchainImagesKHR");
        device->vkQueuePresentKHR = (PFN_vkQueuePresentKHR)device_proc_address(device, "vkQueuePresentKHR");
    }
    /* mesh shader */
    if (physical->extension_bits & vulkan_ext_mesh_shader_bit) {
        device->vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)device_proc_address(device, "vkCmdDrawMeshTasksEXT");
        device->vkCmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT)device_proc_address(device, "vkCmdDrawMeshTasksIndirectEXT");
        device->vkCmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)device_proc_address(device, "vkCmdDrawMeshTasksIndirectCountEXT");
    }
    /* deferred host operations */
    if (physical->extension_bits & vulkan_ext_deferred_host_operations_bit) {
        device->vkCreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR)device_proc_address(device, "vkCreateDeferredOperationKHR");
        device->vkDeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR)device_proc_address(device, "vkDeferredOperationJoinKHR");
        device->vkDestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR)device_proc_address(device, "vkDestroyDeferredOperationKHR");
        device->vkGetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)device_proc_address(device, "vkGetDeferredOperationMaxConcurrencyKHR");
        device->vkGetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR)device_proc_address(device, "vkGetDeferredOperationResultKHR");
    }
    /* acceleration structure */
    if (physical->extension_bits & vulkan_ext_acceleration_structure_bit) {
        device->vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)device_proc_address(device, "vkBuildAccelerationStructuresKHR");
        device->vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)device_proc_address(device, "vkCmdBuildAccelerationStructuresIndirectKHR");
        device->vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)device_proc_address(device, "vkCmdBuildAccelerationStructuresKHR");
        device->vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)device_proc_address(device, "vkCmdCopyAccelerationStructureKHR");
        device->vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)device_proc_address(device, "vkCmdCopyAccelerationStructureToMemoryKHR");
        device->vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)device_proc_address(device, "vkCmdCopyMemoryToAccelerationStructureKHR");
        device->vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)device_proc_address(device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
        device->vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)device_proc_address(device, "vkCopyAccelerationStructureKHR");
        device->vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)device_proc_address(device, "vkCopyAccelerationStructureToMemoryKHR");
        device->vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)device_proc_address(device, "vkCopyMemoryToAccelerationStructureKHR");
        device->vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)device_proc_address(device, "vkCreateAccelerationStructureKHR");
        device->vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)device_proc_address(device, "vkDestroyAccelerationStructureKHR");
        device->vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)device_proc_address(device, "vkGetAccelerationStructureBuildSizesKHR");
        device->vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)device_proc_address(device, "vkGetAccelerationStructureDeviceAddressKHR");
        device->vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)device_proc_address(device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
        device->vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)device_proc_address(device, "vkWriteAccelerationStructuresPropertiesKHR");
    }
    /* ray tracing pipelines */
    if (physical->extension_bits & vulkan_ext_raytracing_pipeline_bit) {
        device->vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)device_proc_address(device, "vkCmdSetRayTracingPipelineStackSizeKHR");
        device->vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)device_proc_address(device, "vkCmdTraceRaysIndirectKHR");
        device->vkCmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR)device_proc_address(device, "vkCmdTraceRaysIndirect2KHR");
        device->vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)device_proc_address(device, "vkCmdTraceRaysKHR");
        device->vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)device_proc_address(device, "vkCreateRayTracingPipelinesKHR");
        device->vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)device_proc_address(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)device_proc_address(device, "vkGetRayTracingShaderGroupHandlesKHR");
        device->vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)device_proc_address(device, "vkGetRayTracingShaderGroupStackSizeKHR");
    }
    /* video coding */
    if (physical->extension_bits & (vulkan_ext_video_queue_bit | vulkan_ext_video_decode_queue_bit)) {
        device->vkBindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR)device_proc_address(device, "vkBindVideoSessionMemoryKHR");
        device->vkCmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR)device_proc_address(device, "vkCmdBeginVideoCodingKHR");
        device->vkCmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR)device_proc_address(device, "vkCmdControlVideoCodingKHR");
        device->vkCmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR)device_proc_address(device, "vkCmdDecodeVideoKHR");
        device->vkCmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR)device_proc_address(device, "vkCmdEndVideoCodingKHR");
        device->vkCreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR)device_proc_address(device, "vkCreateVideoSessionKHR");
        device->vkCreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR)device_proc_address(device, "vkCreateVideoSessionParametersKHR");
        device->vkDestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR)device_proc_address(device, "vkDestroyVideoSessionKHR");
        device->vkDestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR)device_proc_address(device, "vkDestroyVideoSessionParametersKHR");
        device->vkGetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR)device_proc_address(device, "vkGetVideoSessionMemoryRequirementsKHR");
        device->vkUpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR)device_proc_address(device, "vkUpdateVideoSessionParametersKHR");
    }

    /* get device queues */
    for (u32 i = 0; i < physical->queue_family_count; i++) {
        u32 family = queue_infos[i].queueFamilyIndex;
        u32 count = queue_infos[i].queueCount;

        for (u32 j = 0; j < count; j++) {
            VkQueue queue = VK_NULL_HANDLE;
            device->vkGetDeviceQueue(logical, family, j, &queue);
            if (device->graphics_queue == VK_NULL_HANDLE && family == physical->graphics_queue_family_index) {
                device->graphics_queue = queue;
            } else if (device->compute_queue == VK_NULL_HANDLE && family == physical->compute_queue_family_index) {
                device->compute_queue = queue;
            } else if (device->transfer_queue == VK_NULL_HANDLE && family == physical->transfer_queue_family_index) {
                device->transfer_queue = queue;
            } else if (device->sparse_binding_queue == VK_NULL_HANDLE && family == physical->sparse_binding_queue_family_index) {
                device->sparse_binding_queue = queue; 
            } else if (device->video_decode_queue == VK_NULL_HANDLE && family == physical->video_decode_queue_family_index) {
                device->video_decode_queue = queue;
            } else if (device->presentation_queue == VK_NULL_HANDLE && family == physical->presentation_queue_family_index) {
                device->presentation_queue = queue;
            }
        }
    }
    /* overload non-async queues */
    if (device->compute_queue == VK_NULL_HANDLE) {
        assert_debug(!physical->has_async_compute);
        device->compute_queue = device->graphics_queue;
    }
    if (device->transfer_queue == VK_NULL_HANDLE) {
        assert_debug(!physical->has_async_transfer);
        device->transfer_queue = device->graphics_queue;
    }
    if (device->sparse_binding_queue == VK_NULL_HANDLE && physical->sparse_binding_queue_family_index == physical->graphics_queue_family_index) {
        assert_debug(!physical->has_async_sparse_binding);
        device->sparse_binding_queue = device->graphics_queue;
    }
    if (device->video_decode_queue == VK_NULL_HANDLE && physical->video_decode_queue_family_index == physical->graphics_queue_family_index) {
        assert_debug(!physical->has_async_video);
        device->video_decode_queue = device->graphics_queue;
    }
    if (device->presentation_queue == VK_NULL_HANDLE && physical->presentation_queue_family_index == physical->graphics_queue_family_index) {
        assert_debug(!physical->has_async_present);
        device->presentation_queue = device->graphics_queue;
    }

    /* write results */
    create_info->work_header.result = result_success;
    *create_info->write_device = device;

    log_debug("Created rendering device: %s.", physical->properties2.properties.deviceName);
}

void _pelagia_vulkan_destroy_device(struct pelagia_device *device)
{
    if (device->logical == VK_NULL_HANDLE) return;

    device->vkDeviceWaitIdle(device->logical);
#if PELAGIA_ENABLE_GPU_PROFILER
    assert_debug(atomic_load_explicit(&device->device_memory_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->buffer_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->texture_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->sampler_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->pipeline_layout_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->graphics_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->compute_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->raytracing_pipeline_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->descriptor_set_layout_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->descriptor_pool_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->swapchain_count, memory_order_relaxed) == 0);
    assert_debug(atomic_load_explicit(&device->query_pool_count, memory_order_relaxed) == 0);
#endif
    device->vkDestroyDevice(device->logical, NULL);
    log_debug("Destroyed rendering device: %s.", device->physical->properties2.properties.deviceName);
}

void _pelagia_vulkan_destroy_resources(struct pelagia_destroy_resources_work *work) 
{
    for (u32 i = 0; i < work->resource_count; i++) {
        union pelagia_resource resource = work->resources[i];

        switch(resource.header->type) {
#define DESTROY(__type) \
            case pelagia_resource_type_##__type: \
                vulkan_destroy_##__type(resource.__type); break;
            DESTROY(device_memory)
            DESTROY(buffer)
            DESTROY(texture)
            DESTROY(sampler)
            DESTROY(shader)
            DESTROY(pipeline_layout)
            DESTROY(graphics_pipeline)
            DESTROY(compute_pipeline)
            DESTROY(raytracing_pipeline)
            DESTROY(shader_binding_table)
            DESTROY(descriptor_set)
            DESTROY(query_pool)
            DESTROY(swapchain)
            DESTROY(bottom_level)
            DESTROY(top_level)
#undef DESTROY
            case pelagia_resource_type_invalid:
                log_warn("'pelagia_vulkan' asked to destroy an invalid type of GPU resource.");
                break;
            default:
                log_warn("'pelagia_vulkan' asked to destroy a resource of unknown type: %u.", resource.header->type);
                break;
        }
    }
}

/* FIXME stubs */
void _pelagia_vulkan_create_device_memory(struct pelagia_device_memory_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_buffers(struct pelagia_buffers_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_textures(struct pelagia_textures_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_samplers(struct pelagia_samplers_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_shaders(struct pelagia_shaders_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_pipeline_layouts(struct pelagia_pipeline_layouts_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_graphics_pipelines(struct pelagia_graphics_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_compute_pipelines(struct pelagia_compute_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_raytracing_pipelines(struct pelagia_raytracing_pipelines_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_shader_binding_tables(struct pelagia_shader_binding_tables_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_descriptor_sets(struct pelagia_descriptor_sets_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_query_pools(struct pelagia_query_pools_create_info *create_info) { (void)create_info; }

void _pelagia_vulkan_create_bottom_levels(struct pelagia_bottom_levels_create_info *create_info) { (void)create_info; }
void _pelagia_vulkan_create_top_levels(struct pelagia_top_levels_create_info *create_info) { (void)create_info; }

/* TODO macro for destroy stubs */
#define DESTROY_FN(__type) \
    void vulkan_destroy_##__type(struct pelagia_##__type *__type)

DESTROY_FN(device_memory) {
    struct pelagia_device *device = device_memory->header.device;

    zerop(device_memory);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->device_memory_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(buffer) {
    struct pelagia_device *device = buffer->header.device;

    zerop(buffer);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->buffer_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(texture) {
    struct pelagia_device *device = texture->header.device;

    zerop(texture);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->texture_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(sampler) {
    struct pelagia_device *device = sampler->header.device;

    zerop(sampler);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->sampler_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(shader) {
    zerop(shader);
}

DESTROY_FN(pipeline_layout) {
    struct pelagia_device *device = pipeline_layout->header.device;

    zerop(pipeline_layout);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->pipeline_layout_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(graphics_pipeline) {
    struct pelagia_device *device = graphics_pipeline->header.device;

    zerop(graphics_pipeline);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->graphics_pipeline_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(compute_pipeline) {
    struct pelagia_device *device = compute_pipeline->header.device;

    zerop(compute_pipeline);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->compute_pipeline_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(raytracing_pipeline) {
    struct pelagia_device *device = raytracing_pipeline->header.device;

    zerop(raytracing_pipeline);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->raytracing_pipeline_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(shader_binding_table) {
    zerop(shader_binding_table);
}

DESTROY_FN(descriptor_set) {
    zerop(descriptor_set);
}

DESTROY_FN(query_pool) {
    struct pelagia_device *device = query_pool->header.device;

    zerop(query_pool);
#if PELAGIA_ENABLE_GPU_PROFILER
    atomic_fetch_sub_explicit(&device->query_pool_count, 1u, memory_order_release);
#endif
}

DESTROY_FN(bottom_level) { 
    zerop(bottom_level); 
}

DESTROY_FN(top_level) {
    zerop(top_level);
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
        /* device API */
        WRITE_PFN(query_physical_devices)
        WRITE_PFN(create_device)
        WRITE_PFN(destroy_device)
        /* resources API */
        WRITE_PFN(create_device_memory)
        WRITE_PFN(create_buffers)
        WRITE_PFN(create_textures)
        WRITE_PFN(create_samplers)
        WRITE_PFN(create_shaders)
        WRITE_PFN(create_pipeline_layouts)
        WRITE_PFN(create_graphics_pipelines)
        WRITE_PFN(create_compute_pipelines)
        WRITE_PFN(create_raytracing_pipelines)
        WRITE_PFN(create_shader_binding_tables)
        WRITE_PFN(create_descriptor_sets)
        WRITE_PFN(create_query_pools)
        WRITE_PFN(create_swapchain)
        WRITE_PFN(create_bottom_levels)
        WRITE_PFN(create_top_levels)
        WRITE_PFN(destroy_resources)
        /* commands API */
        /* TODO */
#undef WRITE_PFN
    };
    *create_info->header.interface = (riven_argument_t)pelagia;
    log_verbose("'%s' interface write.", pelagia->interface.header.name.ptr);
}
