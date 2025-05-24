#pragma once

/** @file hadal/hadal_adapter.h
 *  @brief Display and XR runtime backend.
 *
 *  TODO docs
 */
#include "../lake/inthelungs.h"

/** The platform-specific display backend. */
LAKE_DECL_HANDLE(hadal_adapter);
/** Represents a display output, maps directly to hardware. A backend without displays is headless. */
LAKE_DECL_HANDLE(hadal_display);
/** Represents a system window with a surface we can draw to. Main context of interaction with the display. */
LAKE_DECL_HANDLE(hadal_window);
/** Represents a keyboard device. */
LAKE_DECL_HANDLE(hadal_keyboard);
/** Represents a mouse or pointer device. */
LAKE_DECL_HANDLE(hadal_mouse);
/** Represents a joystick, these are raw inputs of a game controller. The `hadal_gamepad` maps to the joystick. */
LAKE_DECL_HANDLE(hadal_joystick);
/** Represents a touch device, especially important on mobile platforms. */
LAKE_DECL_HANDLE(hadal_touch);
/** Represents a pen device. */
LAKE_DECL_HANDLE(hadal_pen);
/** The platform-specific VR/XR runtime backend. */
LAKE_DECL_HANDLE(hadalxr_adapter);
/** Represents a frame of reference between virtual space and real-world locations. */
LAKE_DECL_HANDLE(hadalxr_space);
/** Represents an individual action when retrieving action data, creating spaces, or sending haptic events. */
LAKE_DECL_HANDLE(hadalxr_action);
/** Represents a collection of actions that can be logically coupled with the game context. */
LAKE_DECL_HANDLE(hadalxr_action_set);

#ifdef MOON_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, const char *))(void);

/** Allows the display backend to query procedures it needs for the display manager to support Vulkan. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_vulkan_write_instance)(hadal_adapter hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr);
#define FN_HADAL_VULKAN_WRITE_INSTANCE(backend) \
    LAKE_NODISCARD bool LAKECALL _hadal_##backend##_vulkan_write_instance(hadal_adapter hadal, struct VkInstance_T *vk_instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr)

/** Returns whether a queue family in the physical device is capable of presenting to a display. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_hadal_vulkan_presentation_support)(hadal_adapter hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family);
#define FN_HADAL_VULKAN_PRESENTATION_SUPPORT(backend) \
    LAKE_NODISCARD bool LAKECALL _hadal_##backend##_vulkan_presentation_support(hadal_adapter hadal, struct VkPhysicalDevice_T *vk_physical_device, u32 queue_family)

/** Create a surface that a Vulkan renderer may draw to. */
typedef LAKE_NODISCARD lake_result (LAKECALL *PFN_hadal_vulkan_create_surface)(hadal_window window, const struct VkAllocationCallbacks *callbacks, struct VkSurfaceKHR_T **out_vk_surface);
#define FN_HADAL_VULKAN_CREATE_SURFACE(ENCORE) \
    LAKE_NODISCARD lake_result LAKECALL _hadal_##ENCORE##_vulkan_create_surface(hadal_window window, const struct VkAllocationCallbacks *callbacks, struct VkSurfaceKHR_T **out_vk_surface)
#endif /* MOON_VULKAN */

/** Details to assemble a display backend. */
typedef struct {
    lake_app_desc const *app_desc;
} hadal_adapter_assembly;

/** Details to assemble a XR runtime backend. */
typedef struct {
    lake_app_desc const *app_desc;
} hadalxr_adapter_assembly;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(LAKE_PLATFORM_WINDOWS)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_win32, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_WINDOWS */
#if defined(LAKE_PLATFORM_MACOS)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_cocoa, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_MACOS */
#if defined(LAKE_PLATFORM_IOS)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_uikit, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_IOS */
#if defined(LAKE_PLATFORM_ANDROID)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_android, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_ANDROID */
#if defined(LAKE_PLATFORM_EMSCRIPTEN)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_emscripten, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_EMSCRIPTEN */
#ifdef LAKE_PLATFORM_WAYLAND
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_wayland, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_WAYLAND */
#ifdef LAKE_PLATFORM_XCB
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_xcb, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_XCB */
#ifdef LAKE_PLATFORM_KMS_DRM
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_kms, hadal_adapter_assembly, hadal_adapter);
#endif /* LAKE_PLATFORM_KMS_DRM */

/** A headless display adapter, used for debug testing and spec validation. */
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadal_adapter_assembly_headless, hadal_adapter_assembly, hadal_adapter);

/** Returns an array of native display adapters, with a predefined priority. */
LAKE_NODISCARD LAKE_NONNULL_ALL
LAKEAPI PFN_lake_interface_assembly *LAKECALL
hadal_assembly_list_of_native_adapters(
    u32 *out_assembly_count, 
    bool include_fallback);

#if defined(HADAL_OPENXR)
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadalxr_adapter_assembly_openxr, hadalxr_adapter_assembly, hadalxr_adapter);
#endif /* HADAL_OPENXR */

/** A headless XR runtime adapter, used for debug testing and spec validation. */
LAKEAPI FN_LAKE_INTERFACE_ASSEMBLY(hadalxr_adapter_assembly_headless, hadalxr_adapter_assembly, hadalxr_adapter);

/** Returns an array of available XR runtime adapters, with a predefined priority. */
LAKE_NODISCARD LAKE_NONNULL_ALL
LAKEAPI PFN_lake_interface_assembly *LAKECALL
hadalxr_assembly_list_of_runtime_adapters(
    u32 *out_assembly_count, 
    bool include_fallback);

#ifdef __cplusplus
}
#endif /* __cplusplus */
