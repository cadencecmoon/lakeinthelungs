#pragma once

#include <amw/pelagial.h>

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    /* to avoid including windows.h */
    #ifndef UNICODE
        #define UNICODE 1
    #endif
    #ifndef _UNICODE
        #define _UNICODE 1
    #endif
    #ifndef VC_EXTRALEAN
        #define VC_EXTRALEAN
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <vulkan/vk_platform.h>
    #include <vulkan/vulkan_core.h>

    typedef unsigned long                DWORD;
    typedef const wchar_t               *LPCWSTR;
    typedef void                        *HANDLE;
    typedef struct HINSTANCE__          *HINSTANCE;
    typedef struct HWND__               *HWND;
    typedef struct HMONITOR__           *HMONITOR;
    typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES;
    
    #include <vulkan/vulkan_win32.h>
    #if defined(VK_ENABLE_BETA_EXTENSIONS)
        #include <vulkan/vulkan_beta.h>
    #endif
#else
    #include <vulkan/vulkan.h>
#endif

s32 pelagial_vulkan_renderer_init(
    struct pelagic_ocean                   *restrict pelagial,
    const struct pelagic_ocean_create_info *restrict create_info);

void pelagial_vulkan_renderer_fini(void *restrict internal);

/** Collects bits for checking availability of Vulkan extensions and features that interest us. 
 *  These bits are only set once at initialization, represented as a 64-bit integer value.
 *  With our Vulkan renderer we target a minimum of Vulkan core 1.2 with dynamic rendering. */
enum vulkan_extensions {
    /* instance extensions */
    vulkan_ext_surface_bit                      = (1ull << 0),  /**< VK_KHR_surface */
    vulkan_ext_win32_surface_bit                = (1ull << 1),  /**< VK_KHR_win32_surface */
    vulkan_ext_metal_surface_bit                = (1ull << 2),  /**< VK_EXT_metal_surface */
    vulkan_ext_android_surface_bit              = (1ull << 3),  /**< VK_KHR_android_surface */
    vulkan_ext_wayland_surface_bit              = (1ull << 4),  /**< VK_KHR_wayland_surface */
    vulkan_ext_xcb_surface_bit                  = (1ull << 5),  /**< VK_KHR_xcb_surface */
    vulkan_ext_headless_surface_bit             = (1ull << 6),  /**< VK_KHR_headless_surface */
    vulkan_ext_display_bit                      = (1ull << 7),  /**< VK_KHR_display */
    /* instance extensions, debug builds only */
    vulkan_ext_debug_utils_bit                  = (1ull << 8),  /**< VK_EXT_debug_utils */
    vulkan_ext_layer_validation_bit             = (1ull << 9),  /**< VK_LAYER_KHRONOS_validation */
    
    /* device extensions */
    vulkan_ext_swapchain_bit                    = (1ull << 10), /**< VK_KHR_swapchain */
    vulkan_ext_device_fault_bit                 = (1ull << 11), /**< VK_EXT_device_fault */
    vulkan_ext_memory_budget_bit                = (1ull << 12), /**< VK_EXT_memory_budget */
    vulkan_ext_memory_priority_bit              = (1ull << 13), /**< VK_EXT_memory_priority */
    vulkan_ext_mesh_shader_bit                  = (1ull << 14), /**< VK_EXT_mesh_shader */
    vulkan_ext_fragment_shading_rate_bit        = (1ull << 15), /**< VK_KHR_fragment_shading_rate */
    vulkan_ext_deferred_host_operations_bit     = (1ull << 16), /**< VK_KHR_deferred_host_operations */
    vulkan_ext_acceleration_structure_bit       = (1ull << 17), /**< VK_KHR_acceleration_structure */
    vulkan_ext_pipeline_library_bit             = (1ull << 18), /**< VK_KHR_pipeline_library */
    vulkan_ext_ray_query_bit                    = (1ull << 19), /**< VK_KHR_ray_query */
    vulkan_ext_ray_tracing_pipeline_bit         = (1ull << 20), /**< VK_KHR_ray_tracing_pipeline */
    vulkan_ext_ray_tracing_maintenance1_bit     = (1ull << 21), /**< VK_KHR_ray_tracing_maintenance1 */
    vulkan_ext_video_queue_bit                  = (1ull << 22), /**< VK_KHR_video_queue */
    vulkan_ext_video_decode_queue_bit           = (1ull << 23), /**< VK_KHR_video_decode_queue */
    vulkan_ext_video_encode_queue_bit           = (1ull << 24), /**< VK_KHR_video_encode_queue */
    vulkan_ext_video_maintenance1_bit           = (1ull << 25), /**< VK_KHR_video_maintenance1 */
    /* AMD hardware */
    vulkan_ext_amd_device_coherent_memory_bit   = (1ull << 26), /**< VK_AMD_device_coherent_memory */
    /* core 1.4, for backwards compatibility */
    vulkan_ext_dynamic_rendering_local_read_bit = (1ull << 27), /**< VK_KHR_dynamic_rendering_local_read */
    vulkan_ext_maintenance5_bit                 = (1ull << 28), /**< VK_KHR_maintenance5 */
    /* core 1.3, for backwards compatibility */
    vulkan_ext_dynamic_rendering_bit            = (1ull << 29), /**< VK_KHR_dynamic_rendering */
    vulkan_ext_synchronization2_bit             = (1ull << 30), /**< VK_KHR_synchronization2 */
    vulkan_ext_maintenance4_bit                 = (1ull << 31), /**< VK_KHR_maintenance4 */
};
#define vulkan_ext_mask_raytracing \
    (vulkan_ext_deferred_host_operations_bit | \
     vulkan_ext_acceleration_structure_bit | \
     vulkan_ext_ray_query_bit)
#define vulkan_ext_mask_raytracing_pipeline \
    (vulkan_ext_mask_raytracing | \
     vulkan_ext_pipeline_library_bit | \
     vulkan_ext_ray_tracing_pipeline_bit)
#define vulkan_ext_mask_api14 \
    (vulkan_ext_dynamic_rendering_local_read_bit | \
     vulkan_ext_maintenance5_bit)
#define vulkan_ext_mask_api13 \
    (vulkan_ext_dynamic_rendering_bit | \
     vulkan_ext_synchronization2_bit | \
     vulkan_ext_maintenance4_bit)
#define vulkan_ext_mask_video \
    (vulkan_ext_synchronization2_bit | \
     vulkan_ext_video_queue_bit | \
     vulkan_ext_video_decode_queue_bit | \
     vulkan_ext_video_encode_queue_bit)
#define vulkan_ext_mask_required_device \
    (vulkan_ext_swapchain_bit | \
     vulkan_ext_dynamic_rendering_local_read_bit | \
     vulkan_ext_dynamic_rendering_bit | \
     vulkan_ext_synchronization2_bit)

/** Get a (very helpful I guess) message of a given Vulkan error code. */
AMWAPI attr_const
const char *vulkan_result_string(VkResult result);

#if !defined(NDEBUG)
    #define VERIFY_VK(x) { \
        VkResult res__ = (x); \
        if (res__ != VK_SUCCESS) { \
            log_error("Failed to assert VK_SUCCESS for: %s", #x); \
            log_error("The Vulkan error message: %s", vulkan_result_string(res__)); \
            assert_debug(!"VkResult assertion"); \
        } \
    }
#else
    #define VERIFY_VK(x) (void)(x)
#endif

/** Internal state of pelagic_ocean, the shared context of a renderer. Only one such 
 *  instance of a Vulkan backend can exist, this is verified from the entry point. */
struct vulkan_backend {
    /** An instance makes Vulkan functions available to us. It is used for driver calls,
     *  and holds information about the application before passing it to a logical device. */
    VkInstance                                                  instance;
    /** Used for logging messages and profiling with debug mode and validation layers enabled. */
    VkDebugUtilsMessengerEXT                                    debug_messenger;
    /** TODO (for now NULL) Host allocation callbacks implemented with riven. */
    VkAllocationCallbacks                                      *allocator;
    /** Bits for checking availability of Vulkan extensions that are of interest to us. */
    u64                                                         extensions;
    /** The shared rendering context owning this backend. */
    struct pelagic_ocean                                       *pelagial;
    /** The loaded driver library. */
    void                                                       *module;

    /* for retrieving procedures */
    PFN_vkGetInstanceProcAddr                                   vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr                                     vkGetDeviceProcAddr;

    /* global procedures */
    PFN_vkCreateInstance                                        vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties                  vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties                      vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceVersion                              vkEnumerateInstanceVersion;

    /* instance procedures */
    PFN_vkCreateDevice                                          vkCreateDevice;
    PFN_vkDestroyInstance                                       vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties                    vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices                              vkEnumeratePhysicalDevices;
    PFN_vkEnumeratePhysicalDeviceGroups                         vkEnumeratePhysicalDeviceGroups;
    PFN_vkGetPhysicalDeviceFeatures                             vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFeatures2                            vkGetPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceFormatProperties                     vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceFormatProperties2                    vkGetPhysicalDeviceFormatProperties2;
    PFN_vkGetPhysicalDeviceImageFormatProperties                vkGetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties2               vkGetPhysicalDeviceImageFormatProperties2;
    PFN_vkGetPhysicalDeviceMemoryProperties                     vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties2                    vkGetPhysicalDeviceMemoryProperties2;
    PFN_vkGetPhysicalDeviceProperties                           vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceProperties2                          vkGetPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties                vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2               vkGetPhysicalDeviceQueueFamilyProperties2;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties          vkGetPhysicalDeviceSparseImageFormatProperties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2         vkGetPhysicalDeviceSparseImageFormatProperties2;
    PFN_vkGetPhysicalDeviceToolProperties                       vkGetPhysicalDeviceToolProperties;

    /* surface */
    PFN_vkDestroySurfaceKHR                                     vkDestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR                    vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR               vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR                    vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR               vkGetPhysicalDeviceSurfacePresentModesKHR;

#if defined(VK_KHR_win32_surface)
    PFN_vkCreateWin32SurfaceKHR                                 vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR          vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* VK_KHR_win32_surface */

#if defined(VK_EXT_metal_surface)
    PFN_vkCreateMetalSurfaceEXT                                 vkCreateMetalSurfaceEXT;
#endif /* VK_EXT_metal_surface */

#if defined(VK_KHR_wayland_surface)
    PFN_vkCreateWaylandSurfaceKHR                               vkCreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR        vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* VK_KHR_wayland_surface */

#if defined(VK_KHR_xcb_surface)
    PFN_vkCreateXcbSurfaceKHR                                   vkCreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR            vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* VK_KHR_xcb_surface */

#if defined(VK_KHR_android_surface)
    PFN_vkCreateAndroidSurfaceKHR                               vkCreateAndroidSurfaceKHR;
#endif /* VK_KHR_android_surface */

#if defined(VK_EXT_headless_surface)
    PFN_vkCreateHeadlessSurfaceEXT                              vkCreateHeadlessSurfaceEXT;
#endif /* VK_EXT_headless_surface */

#if defined(VK_KHR_display)
    PFN_vkCreateDisplayModeKHR                                  vkCreateDisplayModeKHR;
    PFN_vkCreateDisplayPlaneSurfaceKHR                          vkCreateDisplayPlaneSurfaceKHR;
    PFN_vkGetDisplayModePropertiesKHR                           vkGetDisplayModePropertiesKHR;
    PFN_vkGetDisplayPlaneCapabilitiesKHR                        vkGetDisplayPlaneCapabilitiesKHR;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR                   vkGetDisplayPlaneSupportedDisplaysKHR;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR            vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR                 vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* VK_KHR_display */

    /* debug utils */
    PFN_vkSetDebugUtilsObjectNameEXT                            vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT                             vkSetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT                          vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT                            vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT                         vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT                            vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT                              vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT                           vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT                          vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                         vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT                            vkSubmitDebugUtilsMessageEXT;

    /* video coding */
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR                 vkGetPhysicalDeviceVideoCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR             vkGetPhysicalDeviceVideoFormatPropertiesKHR;
};

/** The device is a context of execution for our renderer, interfacing a GPU. */
struct vulkan_device {
    struct pelagic_ocean                                       *pelagial;
    /** A resource tag for the lifetime of this rendering device. */
    rivens_tag_t                                                tag;
    /** An unique id of the rendering device. */
    u32                                                         uid;
    /** How many instances of buffered state is in use. This value is mostly between 2-4. */
    u32                                                         frames_in_flight;

    /** The Vulkan context of a rendering device, using the given physical device. */
    VkDevice                                                    logical;
    /** A handle to the physical device used to create this rendering device. */
    VkPhysicalDevice                                            physical;
    /** Used to execute draw commands within a render pass, and for presenting to the window. */
    VkQueue                                                     graphics_queue;
    /** Used for dispatch commands on compute pipelines. */
    VkQueue                                                     compute_queue;
    /** Used for copy, updates and transfers of GPU resources. */
    VkQueue                                                     transfer_queue;
    /** Used for sparse binding resources, like huge virtual textures. */
    VkQueue                                                     sparse_queue;
    /** Used for video decoding, mostly H.264 format. */
    VkQueue                                                     decode_queue;
    /** Used for video encoding, mostly H.264 format. */
    VkQueue                                                     encode_queue;

    /* indices of queue families for queues listed above */
    u32                                                         graphics_queue_family_index;
    u32                                                         compute_queue_family_index;
    u32                                                         transfer_queue_family_index;
    u32                                                         sparse_queue_family_index;
    u32                                                         decode_queue_family_index;
    u32                                                         encode_queue_family_index;
    /** For convenience we put unique indices in an array. */
    u32                                                        *queue_family_indices;
    /** How many unique queue families are currently in use, very much device dependent. */
    u32                                                         queue_family_count;

    /** Surface capabilities of the physical device in use. */
    VkSurfaceCapabilitiesKHR                                    surface_capabilities;
    /** Information about hardware properties of the physical device. */
    VkPhysicalDeviceProperties2                                 properties2;
    VkPhysicalDeviceVulkan11Properties                          properties_11;
    VkPhysicalDeviceVulkan12Properties                          properties_12;
    VkPhysicalDeviceVulkan13Properties                          properties_13;
    VkPhysicalDeviceVulkan14Properties                          properties_14;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR          acceleration_structure_properties;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR             raytracing_pipeline_properties;
    VkPhysicalDeviceDescriptorIndexingProperties                descriptor_indexing_properties;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR            fragment_shading_rate_properties;
    VkPhysicalDeviceMeshShaderPropertiesEXT                     mesh_shader_properties;
    VkPhysicalDeviceMemoryProperties2                           memory_properties2;
    VkPhysicalDeviceMemoryBudgetPropertiesEXT                   memory_budget;

    /** Information about features supported by the physical device. */
    VkPhysicalDeviceFeatures2                                   features2;
    VkPhysicalDeviceVulkan11Features                            features_11;
    VkPhysicalDeviceVulkan12Features                            features_12;
    VkPhysicalDeviceVulkan13Features                            features_13;
    VkPhysicalDeviceVulkan14Features                            features_14;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR            acceleration_structure_features;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR               raytracing_pipeline_features;
    VkPhysicalDeviceRayQueryFeaturesKHR                         raytracing_query_features;
    VkPhysicalDeviceDescriptorIndexingFeatures                  descriptor_indexing_features;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR              fragment_shading_rate_features;
    VkPhysicalDeviceMeshShaderFeaturesEXT                       mesh_shader_features;

    /** Information about capabilities of accelerated H.264 video coding. */
    VkVideoDecodeH264ProfileInfoKHR                             decode_h264_profile;
    VkVideoDecodeH264CapabilitiesKHR                            decode_h264_capabilities;
    VkVideoEncodeH264ProfileInfoKHR                             encode_h264_profile;
    VkVideoEncodeH264CapabilitiesKHR                            encode_h264_capabilities;

    /** Information about capabilities of accelerated AV1 video coding. */
    VkVideoDecodeAV1ProfileInfoKHR                              decode_av1_profile;
    VkVideoDecodeAV1CapabilitiesKHR                             decode_av1_capabilities;
    VkVideoEncodeAV1ProfileInfoKHR                              encode_av1_profile;
    VkVideoEncodeAV1CapabilitiesKHR                             encode_av1_capabilities;

    struct vulkan_video_capability {
        VkVideoProfileInfoKHR                                   profile;
        VkVideoDecodeCapabilitiesKHR                            decode_capabilities;
        VkVideoEncodeCapabilitiesKHR                            encode_capabilities;
        VkVideoCapabilitiesKHR                                  video_capabilities;
    } video_h264_capability, video_av1_capability;

    /* core 1.0 */
	PFN_vkAllocateCommandBuffers                                vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets                                vkAllocateDescriptorSets;
	PFN_vkAllocateMemory                                        vkAllocateMemory;
	PFN_vkBeginCommandBuffer                                    vkBeginCommandBuffer;
	PFN_vkBindBufferMemory                                      vkBindBufferMemory;
	PFN_vkBindImageMemory                                       vkBindImageMemory;
	PFN_vkCmdBeginQuery                                         vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass                                    vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets                                 vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer                                    vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline                                       vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers                                  vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage                                          vkCmdBlitImage;
	PFN_vkCmdClearAttachments                                   vkCmdClearAttachments;
	PFN_vkCmdClearColorImage                                    vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage                             vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer                                         vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage                                  vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage                                          vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer                                  vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults                               vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch                                           vkCmdDispatch;
	PFN_vkCmdDispatchIndirect                                   vkCmdDispatchIndirect;
	PFN_vkCmdDraw                                               vkCmdDraw;
	PFN_vkCmdDrawIndexed                                        vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect                                vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect                                       vkCmdDrawIndirect;
	PFN_vkCmdEndQuery                                           vkCmdEndQuery;
	PFN_vkCmdEndRenderPass                                      vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands                                    vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer                                         vkCmdFillBuffer;
	PFN_vkCmdNextSubpass                                        vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier                                    vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants                                      vkCmdPushConstants;
	PFN_vkCmdResetEvent                                         vkCmdResetEvent;
	PFN_vkCmdResetQueryPool                                     vkCmdResetQueryPool;
	PFN_vkCmdResolveImage                                       vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants                                  vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias                                       vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds                                     vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent                                           vkCmdSetEvent;
	PFN_vkCmdSetLineWidth                                       vkCmdSetLineWidth;
	PFN_vkCmdSetScissor                                         vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask                              vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference                                vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask                                vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport                                        vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer                                       vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents                                         vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp                                     vkCmdWriteTimestamp;
	PFN_vkCreateBuffer                                          vkCreateBuffer;
	PFN_vkCreateBufferView                                      vkCreateBufferView;
	PFN_vkCreateCommandPool                                     vkCreateCommandPool;
	PFN_vkCreateComputePipelines                                vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool                                  vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout                             vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent                                           vkCreateEvent;
	PFN_vkCreateFence                                           vkCreateFence;
	PFN_vkCreateFramebuffer                                     vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines                               vkCreateGraphicsPipelines;
	PFN_vkCreateImage                                           vkCreateImage;
	PFN_vkCreateImageView                                       vkCreateImageView;
	PFN_vkCreatePipelineCache                                   vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout                                  vkCreatePipelineLayout;
	PFN_vkCreateQueryPool                                       vkCreateQueryPool;
	PFN_vkCreateRenderPass                                      vkCreateRenderPass;
	PFN_vkCreateSampler                                         vkCreateSampler;
	PFN_vkCreateSemaphore                                       vkCreateSemaphore;
	PFN_vkCreateShaderModule                                    vkCreateShaderModule;
	PFN_vkDestroyBuffer                                         vkDestroyBuffer;
	PFN_vkDestroyBufferView                                     vkDestroyBufferView;
	PFN_vkDestroyCommandPool                                    vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool                                 vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout                            vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice                                         vkDestroyDevice;
	PFN_vkDestroyEvent                                          vkDestroyEvent;
	PFN_vkDestroyFence                                          vkDestroyFence;
	PFN_vkDestroyFramebuffer                                    vkDestroyFramebuffer;
	PFN_vkDestroyImage                                          vkDestroyImage;
	PFN_vkDestroyImageView                                      vkDestroyImageView;
	PFN_vkDestroyPipeline                                       vkDestroyPipeline;
	PFN_vkDestroyPipelineCache                                  vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout                                 vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool                                      vkDestroyQueryPool;
	PFN_vkDestroyRenderPass                                     vkDestroyRenderPass;
	PFN_vkDestroySampler                                        vkDestroySampler;
	PFN_vkDestroySemaphore                                      vkDestroySemaphore;
	PFN_vkDestroyShaderModule                                   vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle                                        vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer                                      vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges                               vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers                                    vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets                                    vkFreeDescriptorSets;
	PFN_vkFreeMemory                                            vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements                           vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment                             vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue                                        vkGetDeviceQueue;
	PFN_vkGetEventStatus                                        vkGetEventStatus;
	PFN_vkGetFenceStatus                                        vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements                            vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements                      vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout                             vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData                                  vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults                                   vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity                              vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges                          vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory                                             vkMapMemory;
	PFN_vkMergePipelineCaches                                   vkMergePipelineCaches;
	PFN_vkQueueBindSparse                                       vkQueueBindSparse;
	PFN_vkQueueSubmit                                           vkQueueSubmit;
	PFN_vkQueueWaitIdle                                         vkQueueWaitIdle;
	PFN_vkResetCommandBuffer                                    vkResetCommandBuffer;
	PFN_vkResetCommandPool                                      vkResetCommandPool;
	PFN_vkResetDescriptorPool                                   vkResetDescriptorPool;
	PFN_vkResetEvent                                            vkResetEvent;
	PFN_vkResetFences                                           vkResetFences;
	PFN_vkSetEvent                                              vkSetEvent;
	PFN_vkUnmapMemory                                           vkUnmapMemory;
	PFN_vkUpdateDescriptorSets                                  vkUpdateDescriptorSets;
	PFN_vkWaitForFences                                         vkWaitForFences;

    /* core 1.1 */
    PFN_vkBindBufferMemory2                                     vkBindBufferMemory2;
    PFN_vkBindImageMemory2                                      vkBindImageMemory2;
    PFN_vkCmdDispatchBase                                       vkCmdDispatchBase;
    PFN_vkCmdSetDeviceMask                                      vkCmdSetDeviceMask;
    PFN_vkCreateDescriptorUpdateTemplate                        vkCreateDescriptorUpdateTemplate;
    PFN_vkCreateSamplerYcbcrConversion                          vkCreateSamplerYcbcrConversion;
    PFN_vkDestroyDescriptorUpdateTemplate                       vkDestroyDescriptorUpdateTemplate;
    PFN_vkDestroySamplerYcbcrConversion                         vkDestroySamplerYcbcrConversion;
    PFN_vkGetBufferMemoryRequirements2                          vkGetBufferMemoryRequirements2;
    PFN_vkGetDescriptorSetLayoutSupport                         vkGetDescriptorSetLayoutSupport;
    PFN_vkGetDeviceGroupPeerMemoryFeatures                      vkGetDeviceGroupPeerMemoryFeatures;
    PFN_vkGetDeviceQueue2                                       vkGetDeviceQueue2;
    PFN_vkGetImageMemoryRequirements2                           vkGetImageMemoryRequirements2;
    PFN_vkGetImageSparseMemoryRequirements2                     vkGetImageSparseMemoryRequirements2;
    PFN_vkTrimCommandPool                                       vkTrimCommandPool;
    PFN_vkUpdateDescriptorSetWithTemplate                       vkUpdateDescriptorSetWithTemplate;

    /* core 1.2 */
    PFN_vkCmdDrawIndexedIndirectCount                           vkCmdDrawIndexedIndirectCount;
    PFN_vkCmdDrawIndirectCount                                  vkCmdDrawIndirectCount;
    PFN_vkGetBufferDeviceAddress                                vkGetBufferDeviceAddress;
    PFN_vkGetBufferOpaqueCaptureAddress                         vkGetBufferOpaqueCaptureAddress;
    PFN_vkGetDeviceMemoryOpaqueCaptureAddress                   vkGetDeviceMemoryOpaqueCaptureAddress;
    PFN_vkGetSemaphoreCounterValue                              vkGetSemaphoreCounterValue;
    PFN_vkResetQueryPool                                        vkResetQueryPool;
    PFN_vkSignalSemaphore                                       vkSignalSemaphore;
    PFN_vkWaitSemaphores                                        vkWaitSemaphores;

    /* core 1.3 */
    PFN_vkCmdBeginRendering                                     vkCmdBeginRendering;
    PFN_vkCmdEndRendering                                       vkCmdEndRendering;
    PFN_vkCmdPipelineBarrier2                                   vkCmdPipelineBarrier2;
    PFN_vkCmdResetEvent2                                        vkCmdResetEvent2;
    PFN_vkCmdSetEvent2                                          vkCmdSetEvent2;
    PFN_vkCmdWaitEvents2                                        vkCmdWaitEvents2;
    PFN_vkCmdWriteTimestamp2                                    vkCmdWriteTimestamp2;
    PFN_vkQueueSubmit2                                          vkQueueSubmit2;
    PFN_vkGetDeviceBufferMemoryRequirements                     vkGetDeviceBufferMemoryRequirements;
    PFN_vkGetDeviceImageMemoryRequirements                      vkGetDeviceImageMemoryRequirements;
    PFN_vkGetDeviceImageSparseMemoryRequirements                vkGetDeviceImageSparseMemoryRequirements;

    /* core 1.4 */
    PFN_vkCmdBindIndexBuffer2                                   vkCmdBindIndexBuffer2;
    PFN_vkCmdSetRenderingAttachmentLocations                    vkCmdSetRenderingAttachmentLocations;
    PFN_vkCmdSetRenderingInputAttachmentIndices                 vkCmdSetRenderingInputAttachmentIndices;
    PFN_vkGetDeviceImageSubresourceLayout                       vkGetDeviceImageSubresourceLayout;
    PFN_vkGetImageSubresourceLayout2                            vkGetImageSubresourceLayout2;
    PFN_vkGetRenderingAreaGranularity                           vkGetRenderingAreaGranularity;

    /* swapchain */
    PFN_vkAcquireNextImageKHR                                   vkAcquireNextImageKHR;
    PFN_vkAcquireNextImage2KHR                                  vkAcquireNextImage2KHR;
    PFN_vkCreateSwapchainKHR                                    vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                                   vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                                 vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR                                       vkQueuePresentKHR;

    /* device fault */
    PFN_vkGetDeviceFaultInfoEXT                                 vkGetDeviceFaultInfoEXT;

    /* mesh shader */
    PFN_vkCmdDrawMeshTasksEXT                                   vkCmdDrawMeshTasksEXT;
    PFN_vkCmdDrawMeshTasksIndirectEXT                           vkCmdDrawMeshTasksIndirectEXT;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT                      vkCmdDrawMeshTasksIndirectCountEXT;

    /* deferred host operations */
    PFN_vkCreateDeferredOperationKHR                            vkCreateDeferredOperationKHR;
    PFN_vkDeferredOperationJoinKHR                              vkDeferredOperationJoinKHR;
    PFN_vkDestroyDeferredOperationKHR                           vkDestroyDeferredOperationKHR;
    PFN_vkGetDeferredOperationMaxConcurrencyKHR                 vkGetDeferredOperationMaxConcurrencyKHR;
    PFN_vkGetDeferredOperationResultKHR                         vkGetDeferredOperationResultKHR;

    /* acceleration structure */
    PFN_vkBuildAccelerationStructuresKHR                        vkBuildAccelerationStructuresKHR;
    PFN_vkCmdBuildAccelerationStructuresIndirectKHR             vkCmdBuildAccelerationStructuresIndirectKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR                     vkCmdBuildAccelerationStructuresKHR;
    PFN_vkCmdCopyAccelerationStructureKHR                       vkCmdCopyAccelerationStructureKHR;
    PFN_vkCmdCopyAccelerationStructureToMemoryKHR               vkCmdCopyAccelerationStructureToMemoryKHR;
    PFN_vkCmdCopyMemoryToAccelerationStructureKHR               vkCmdCopyMemoryToAccelerationStructureKHR;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR           vkCmdWriteAccelerationStructuresPropertiesKHR;
    PFN_vkCopyAccelerationStructureKHR                          vkCopyAccelerationStructureKHR;
    PFN_vkCopyAccelerationStructureToMemoryKHR                  vkCopyAccelerationStructureToMemoryKHR;
    PFN_vkCopyMemoryToAccelerationStructureKHR                  vkCopyMemoryToAccelerationStructureKHR;
    PFN_vkCreateAccelerationStructureKHR                        vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR                       vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR                 vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR              vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkGetDeviceAccelerationStructureCompatibilityKHR        vkGetDeviceAccelerationStructureCompatibilityKHR;
    PFN_vkWriteAccelerationStructuresPropertiesKHR              vkWriteAccelerationStructuresPropertiesKHR;

    /* ray tracing pipeline */
    PFN_vkCmdSetRayTracingPipelineStackSizeKHR                  vkCmdSetRayTracingPipelineStackSizeKHR;
    PFN_vkCmdTraceRaysIndirectKHR                               vkCmdTraceRaysIndirectKHR;
    PFN_vkCmdTraceRaysIndirect2KHR                              vkCmdTraceRaysIndirect2KHR; /* ray_tracing_maintenance1 */
    PFN_vkCmdTraceRaysKHR                                       vkCmdTraceRaysKHR;
    PFN_vkCreateRayTracingPipelinesKHR                          vkCreateRayTracingPipelinesKHR;
    PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR       vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupHandlesKHR                    vkGetRayTracingShaderGroupHandlesKHR;
    PFN_vkGetRayTracingShaderGroupStackSizeKHR                  vkGetRayTracingShaderGroupStackSizeKHR;

    /* video coding */
    PFN_vkBindVideoSessionMemoryKHR                             vkBindVideoSessionMemoryKHR;
    PFN_vkCmdBeginVideoCodingKHR                                vkCmdBeginVideoCodingKHR;
    PFN_vkCmdControlVideoCodingKHR                              vkCmdControlVideoCodingKHR;
    PFN_vkCmdDecodeVideoKHR                                     vkCmdDecodeVideoKHR;
    PFN_vkCmdEndVideoCodingKHR                                  vkCmdEndVideoCodingKHR;
    PFN_vkCreateVideoSessionKHR                                 vkCreateVideoSessionKHR;
    PFN_vkCreateVideoSessionParametersKHR                       vkCreateVideoSessionParametersKHR;
    PFN_vkDestroyVideoSessionKHR                                vkDestroyVideoSessionKHR;
    PFN_vkDestroyVideoSessionParametersKHR                      vkDestroyVideoSessionParametersKHR;
    PFN_vkGetVideoSessionMemoryRequirementsKHR                  vkGetVideoSessionMemoryRequirementsKHR;
    PFN_vkUpdateVideoSessionParametersKHR                       vkUpdateVideoSessionParametersKHR;
};

/** We use the swapchain as a way to present rendered images to the window surface. */
struct vulkan_swapchain {
    /** A swapchain created by the means of our display backend provided surface. */
    VkSwapchainKHR              sc;
    /** A surface for the renderer to interface with the system window. */
    VkSurfaceKHR                surface;
    /** The format of images, in pixels. */
    VkFormat                    format;
    /** Selected mode of image presentation. */
    VkPresentModeKHR            present_mode;
    /** An array of images held in the swapchain. */
    VkImage                     images[AMW_MAX_FRAMES_IN_FLIGHT];
    /** An image view for each image in the swapchain. */
    VkImageView                 image_views[AMW_MAX_FRAMES_IN_FLIGHT];
    /** Semaphores for synchronizing presentation with the device in control of the swapchain. */
    VkSemaphore                 image_available_sem[AMW_MAX_FRAMES_IN_FLIGHT];
    /** Number of images in the swapchain. */
    u32                         image_count;
    /** An unique id of the rendering device controling this swapchain. */
    u32                         uid;
};

/** Enums for intended memory usage. */
enum vulkan_memory_usage {
    vulkan_memory_usage_unknown = 0,           /**< No intended memory usage specified. */
    vulkan_memory_usage_lazily_allocated,      /**< Lazily allocated GPU memory, exists mostly on mobile platforms. Implies a dedicated allocation. */
    vulkan_memory_usage_auto,                  /**< Selects best memory type for GPU resources, figured out from the context of an allocation. */
    vulkan_memory_usage_auto_prefer_device,    /**< Selects best memory type for GPU resources, with preference for GPU (device) memory. */
    vulkan_memory_usage_auto_prefer_host,      /**< Selects best memory type for GPU resources, with preference for CPU (host) memory. */
};

enum vulkan_allocation_flags {
    /** Set this if the allocation should have its own device memory block. */
    vulkan_allocation_flag_dedicated_memory                = (1u << 0),
    /** Set this to only try suballocate from existing device memory, and never create new such blocks. */
    vulkan_allocation_flag_never_allocate                  = (1u << 1),
    /** Set this to use a memory that will be persistently mapped. */
    vulkan_allocation_flag_mapped                          = (1u << 2),
    /** Set this if the allocated memory will have aliasing resources. */
    vulkan_allocation_flag_can_alias                       = (1u << 3),
    /** Request possibility to map the allocation (either flag_mapped or later with map memory API calls).
     *  Declares that mapped memory will only be written sequentially, e.g. using 'memcpy()' or a loop
     *  writing byte-by-byte, never read, or accessed randomly from the CPU, so a memory type can be 
     *  selected that is uncached and write-combined. */
    vulkan_allocation_flag_host_access_sequential_write    = (1u << 4),
    /** Request possibility to map the allocation (either flag_mapped or later with map memory API calls).
     *  Declares that mapped memory can be read, written, and accessed in random order, so a host cached 
     *  memory type is preferred. */
    vulkan_allocation_flag_host_access_random              = (1u << 5),
};

struct vulkan_allocation_create_info {
    /** Flags of enum vulkan_allocation_flags */
    VkFlags                     flags;
    /** Defines for what purpose is this allocation. */
    enum vulkan_memory_usage    usage;
    /** Flags that must be set in a memory type chosen for an allocation. */
    VkMemoryPropertyFlags       required_flags;
    /** Flags that preferably should be set in a memory type chosen for an allocation. */
    VkMemoryPropertyFlags       preferred_flags;
    /** Bitmask containing one bit set for every memory type acceptable for this allocation.
     *  A value of 0 is equivalent to UINT32_MAX, so any valid memory type may be accepted. */
    u32                         memory_type_bits;
    /** The size of the memory allocation. If 0, we'll try to calculate it if possible. */
    VkDeviceSize                size;
};

/** Information necessary for subdividing large device memory allocations, 
 *  and the offset used for binding GPU resources like buffers and textures. */
struct vulkan_allocation {
    VkDeviceMemory              memory;                 /**< The actual memory allocation we access, can be NULL. */
    u32                         memory_type;            /**< Memory type index that this allocation was allocated from. */
    VkDeviceSize                size;                   /**< The size of this allocation, in bytes. */
    VkDeviceSize                offset;                 /**< The heap offset to the beginning of this allocation, in bytes. */
    void                       *mapped;                 /**< Pointer to the beginning of this allocation as mapped data, can be NULL. */
    VkBool32                    dedicated_allocation;   /**< Non-zero value if this is a dedicated GPU allocation. */
};

/** Helps to find memory_type_index, from given memory_type_bits and allocation_create_info.
 *  This will try to find a memory type that:
 *  - Is allowed by memory_type_bits.
 *  - Contains all the flags from allocation_create_info->required_flags. 
 *  - Matches intended usage.
 *  - Has as many flags from allocation_create_info->preferred_flags as possible. */
attr_nonnull_all
VkResult vulkan_find_memory_type_index(
    struct vulkan_device                       *device,
    u32                                         memory_type_bits,
    const struct vulkan_allocation_create_info *allocation_create_info,
    u32                                        *out_memory_type_index);

/** Helps to find memory_type_index, given VkImageCreateInfo and allocation_create_info. */
attr_nonnull_all
VkResult vulkan_find_memory_type_index_for_texture_info(
    struct vulkan_device                       *device,
    const VkImageCreateInfo                    *image_create_info,
    const struct vulkan_allocation_create_info *allocation_create_info,
    u32                                        *out_memory_type_index);

/** Helps to find memory_type_index, given VkBufferCreateInfo and allocation_create_info. */
attr_nonnull_all
VkResult vulkan_find_memory_type_index_for_buffer_info(
    struct vulkan_device                       *device,
    const VkBufferCreateInfo                   *buffer_create_info,
    const struct vulkan_allocation_create_info *allocation_request,
    u32                                        *out_memory_type_index);

/** Combines a Vulkan image object, with meta-data, the view and suballocation info. */
struct vulkan_texture {
    VkImage                     image;                  /**< The Vulkan handle of a texture resource. */
    VkImageView                 view;                   /**< A view onto the contents of this texture or NULL if no view was requested. */
    VkDeviceSize                size;                   /**< The minimum size of this individual image, in bytes. */
    VkDeviceSize                offset;                 /**< The offset of this image within the bound memory allocation, in bytes. */
    struct vulkan_allocation   *allocation;             /**< Information needed to access memory for this texture. */
};

/** Combines a Vulkan buffer object with suballocation info. */
struct vulkan_buffer {
    VkBuffer                    buffer;                 /**< The Vulkan handle of a buffer resource. */
    VkDeviceSize                size;                   /**< The minimum size of this buffer, in bytes. */
    VkDeviceSize                offset;                 /**< The offset of this buffer within the bound memory allocation, in bytes. */
    struct vulkan_allocation   *allocation;             /**< Information needed to access memory for this buffer. */
};

/** Computes the number of mipmap levels needed to get from a resource of the 
 *  given size to one texel. This is the max number of mipmapc that can be created. */
attr_inline u32 vulkan_get_mipmap_count_1d(u32 width) {
	s32 padded_width = (s32)(2 * width - 1);
	u32 mipmap_count = 0;
	while (padded_width > 0) {
		padded_width &= 0x7ffffffe;
		padded_width >>= 1;
		++mipmap_count;
	}
	return mipmap_count;
}

/** The maximum of 'get_mipmap_count_1d' for all given extents. */
attr_inline u32 vulkan_get_mipmap_count_3d(VkExtent3D *extent) {
	u32 counts[3] = {
		vulkan_get_mipmap_count_1d(extent->width),
		vulkan_get_mipmap_count_1d(extent->height),
		vulkan_get_mipmap_count_1d(extent->depth)
	};
	u32 result = counts[0];
	result = (result < counts[1]) ? counts[1] : result;
	result = (result < counts[2]) ? counts[2] : result;
	return result;
}
