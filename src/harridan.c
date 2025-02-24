#include <amw/bits.h>
#include <amw/hadal.h>
#include <amw/harridan.h>
#include <amw/log.h>
#include <amw/riven.h>
#include <amw/random.h>

#include <stdio.h>  /* snprintf */
#include <string.h> /* strcmp */

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
    VkSurfaceFormatKHR *surface_formats;
    VkPresentModeKHR *present_modes;

    u32 api_version = 0;
    u32 extension_count = 0;
    u32 video_property_count = 0;
    u32 queue_family_count = 0;
    u32 surface_format_count = 0;
    u32 present_mode_count = 0;

    work->accepted = false;

    assert_debug(device != VK_NULL_HANDLE);

    /* resolve driver versions */
    harridan->api.vkGetPhysicalDeviceProperties(device, &work->properties2.properties);
    api_version = work->properties2.properties.apiVersion;
    if (api_version < VK_API_VERSION_1_2) {
        if (work->verbose)
            log_debug("Physical device (%u) has too old drivers - found api version %u.%u.%u, we target atleast core 1.2.X.",
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
            log_debug("Physical device (%u) has no command queue families.", work->index);
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
            log_debug("Physical device (%u) has no graphics commands support.", work->index);
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
            log_debug("Physical device (%u) has zero Vulkan extensions available.", work->index);
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
            log_debug("Physical device (%u) has no swapchain support.", work->index);
        return;
    }
    if (!work->has_vk14 && (work->extension_bits & vk_extension_mask_api14) != vk_extension_mask_api14) {
        if (work->verbose)
            log_debug("Physical device (%u) does not support required 1.4 core extensions (backwards compatibility):"
                      "\n\tdynamic_rendering_local_read : %b", work->index, work->dynamic_rendering_local_read_features.dynamicRenderingLocalRead);
        return;
    }
    if (!work->has_vk13 && (work->extension_bits & vk_extension_mask_api13) != vk_extension_mask_api13) {
        if (work->verbose)
            log_debug("Physical device (%u) does not support required 1.3 core extensions (backwards compatibility):"
                      "\n\tdynamic_rendering : %b\n\tsynchronization2 : %b", work->index, 
                      work->dynamic_rendering_features.dynamicRendering, work->synchronization2_features.synchronization2);
        return;
    }

    /* resolve presentation support */
    harridan->api.vkGetPhysicalDeviceSurfaceSupportKHR(device, work->graphics_queue_family, harridan->swapchain.surface, &work->presentation_support);
    if (work->presentation_support) {
        /* get surface capabilities */
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(work->device, harridan->swapchain.surface, &work->surface_capabilities));

        /* get surface formats */
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceFormatsKHR(work->device, harridan->swapchain.surface, &surface_format_count, NULL));
        surface_formats = (VkSurfaceFormatKHR *)
            riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkSurfaceFormatKHR) * surface_format_count, _Alignof(VkSurfaceFormatKHR));
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfaceFormatsKHR(work->device, harridan->swapchain.surface, &surface_format_count, surface_formats));

        work->swapchain_image_format = VK_FORMAT_UNDEFINED;
        if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
            work->swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
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
                work->swapchain_image_format = format;
            }
        }
        if (work->swapchain_image_format == VK_FORMAT_UNDEFINED) {
            /* for now only GPUs with presentation support interest us */
            if (work->verbose)
                log_debug("Physical device (%u) can't determine an appropriate surface format." 
                          "Only R8G8B8A8, B8G8R8A8, A2R10G10B10 or A2B10G10R10 format will be accepted.", work->index);
            return;
        }

        /* get present modes */
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfacePresentModesKHR(work->device, harridan->swapchain.surface, &present_mode_count, NULL));
        present_modes = (VkPresentModeKHR *)
            riven_alloc(harridan->riven, rivens_tag_drifter, sizeof(VkPresentModeKHR) * present_mode_count, _Alignof(VkPresentModeKHR));
        VERIFY_VK(harridan->api.vkGetPhysicalDeviceSurfacePresentModesKHR(work->device, harridan->swapchain.surface, &present_mode_count, present_modes));

        VkPresentModeKHR no_vsync_mode = VK_PRESENT_MODE_FIFO_KHR;
        for (u32 i = 0; i < present_mode_count; i++) {
            if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR && no_vsync_mode == VK_PRESENT_MODE_FIFO_KHR)
                no_vsync_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                no_vsync_mode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
        if (no_vsync_mode == VK_PRESENT_MODE_FIFO_KHR) {
            if (work->verbose)
                log_verbose("Physical device (%u) has no presentation mode without vertical synchronization, V-sync is forced.", work->index);
            work->flag_bits |= harridan_flag_vsync_enabled;
        }
        work->present_mode = no_vsync_mode;
        work->feat_bits |= harridan_feat_presentation_support;
    } else {
        /* for now only GPUs with presentation support interest us */
        if (work->verbose)
            log_debug("Physical device (%u) has no support for presenting to surface.", work->index);
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
            "    Heap %u :    %lu MB (%s)\n", i, (size >> 20), 
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
    f32 queue_priorities[6] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };
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
    harridan->swapchain.surface_format = info->surface_format;
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
    if (harridan->logical != VK_NULL_HANDLE)
        harridan->api.vkDestroyDevice(harridan->logical, NULL);

    if (harridan->swapchain.surface != VK_NULL_HANDLE)
        harridan->api.vkDestroySurfaceKHR(harridan->instance, harridan->swapchain.surface, NULL);

    if (harridan->api.extensions & vk_extension_layer_validation_bit)
        destroy_validation_layers(&harridan->api, harridan->instance, NULL, harridan->debug_messenger);

    if (harridan->instance)
        harridan->api.vkDestroyInstance(harridan->instance, NULL);
    close_vulkan_driver(&harridan->api);
    zerop(harridan);
}
