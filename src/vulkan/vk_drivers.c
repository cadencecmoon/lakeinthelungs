/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include "vk_backend.h"

#include <amw/common/assert.h>
#include <amw/hadal.h>

#if defined(AMW_PLATFORM_APPLE)
#include <stdlib.h> /* getenv, for MacOS */
#endif
#include <string.h> /* strcmp */

static PFN_vkVoidFunction get_vulkan_instance_proc_address(i32 *out_error, struct vulkan_api *vulkan, VkInstance instance, const char *procname)
{
    assert_debug(vulkan && vulkan->module && vulkan->vkGetInstanceProcAddr);

    PFN_vkVoidFunction address = vulkan->vkGetInstanceProcAddr(instance, procname);
    if (address == NULL) {
        log_error("Can't find a Vulkan function of name '%s'", procname);
        *out_error = result_error_proc_address_dll;
    }

    return address;
}

static PFN_vkVoidFunction get_vulkan_device_proc_address(i32 *out_error, struct vulkan_api *vulkan, VkDevice device, const char *procname)
{
    assert_debug(vulkan && vulkan->module && vulkan->vkGetDeviceProcAddr);

    PFN_vkVoidFunction address = vulkan->vkGetDeviceProcAddr(device, procname);
    if (address == NULL) {
        log_error("Can't find a Vulkan function of name '%s'", procname);
        *out_error = result_error_proc_address_dll;
    }

    return address;
}

bool vulkan_open_driver(struct vulkan_api *vulkan)
{
    assert_debug(vulkan);

#if defined(AMW_PLATFORM_WINDOWS)
    vulkan->module = hadal_load_dll("vulkan-1.dll");
#elif defined(AMW_PLATFORM_APPLE)
    vulkan->module = hadal_load_dll("libvulkan.dylib");
    if (!vulkan->module)
        vulkan->module = hadal_load_dll("libvulkan.1.dylib");
    if (!vulkan->module)
        vulkan->module = hadal_load_dll("libMoltenVK.dylib");
    /* Add support for using Vulkan and MoltenVK in a Framework. App store rules for iOS
     * strictly enforce no .dylib's. If they aren't found it just falls through */
    if (!vulkan->module)
        vulkan->module = hadal_load_dll("vulkan.framework/vulkan");
    if (!vulkan->module)
        vulkan->module = hadal_load_dll("MoltenVK.framework/MoltenVK");
    /* modern versions of macOS don't search /usr/local/lib automatically contrary to what man dlopen says
	 * Vulkan SDK uses this as the system-wide installation location, 
     * so we're going to fallback to this if all else fails */
    if (!vulkan->module && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL)
        vulkan->module = hadal_load_dll("/usr/local/lib/libvulkan.dylib");
#else
    vulkan->module = hadal_load_dll("libvulkan.so.1");
    if (!vulkan->module)
        vulkan->module = hadal_load_dll("libvulkan.so");
#endif
    if (!vulkan->module) {
        log_error("Can't connect to a Vulkan driver library.");
        return false;
    }
    vulkan->vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)hadal_get_proc_address(vulkan->module, "vkGetInstanceProcAddr");

    if (vulkan->vkGetInstanceProcAddr == NULL) {
        vulkan_close_driver(vulkan);
        log_error("Can't load vkGetInstanceProcAddr from Vulkan drivers.");
        return false;
    }

    i32 error = result_success;
    vulkan->vkCreateInstance                        = (PFN_vkCreateInstance)(void *)get_vulkan_instance_proc_address(&error, vulkan, NULL, "vkCreateInstance");
    vulkan->vkEnumerateInstanceVersion              = (PFN_vkEnumerateInstanceVersion)(void *)get_vulkan_instance_proc_address(&error, vulkan, NULL, "vkEnumerateInstanceVersion");
    vulkan->vkEnumerateInstanceExtensionProperties  = (PFN_vkEnumerateInstanceExtensionProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, NULL, "vkEnumerateInstanceExtensionProperties");
    vulkan->vkEnumerateInstanceLayerProperties      = (PFN_vkEnumerateInstanceLayerProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, NULL, "vkEnumerateInstanceLayerProperties");

    if (error != result_success) {
        vulkan_close_driver(vulkan);
        log_error("Can't load global function pointers from Vulkan drivers.");
        return false;
    }

    return true;
}

void vulkan_close_driver(struct vulkan_api *vulkan)
{
    assert_debug(vulkan);

    if (vulkan->module)
        hadal_close_dll(vulkan->module);

    vulkan->vkGetDeviceProcAddr = NULL;
    vulkan->vkGetInstanceProcAddr = NULL;
    vulkan->module = NULL;
}

bool vulkan_has_extension(const VkExtensionProperties *ext, u32 count, const char *name)
{
    for (u32 i = 0; i < count; i++) {
        if (!strcmp(ext[i].extensionName, name)) {
            return true;
        }
    }
    return false;
}

i32 vulkan_load_instance_pointers(struct vulkan_api *vulkan, VkInstance instance)
{
    if (!vulkan || !vulkan->vkGetInstanceProcAddr)
        return result_error_invalid_context;

    i32 error = result_success;
    vulkan->vkCreateDevice                                          = (PFN_vkCreateDevice)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateDevice");
    vulkan->vkDestroyInstance                                       = (PFN_vkDestroyInstance)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkDestroyInstance");
    vulkan->vkEnumerateDeviceExtensionProperties                    = (PFN_vkEnumerateDeviceExtensionProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkEnumerateDeviceExtensionProperties");
    vulkan->vkEnumeratePhysicalDevices                              = (PFN_vkEnumeratePhysicalDevices)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkEnumeratePhysicalDevices");
    vulkan->vkGetDeviceProcAddr                                     = (PFN_vkGetDeviceProcAddr)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetDeviceProcAddr");
    vulkan->vkGetPhysicalDeviceFeatures                             = (PFN_vkGetPhysicalDeviceFeatures)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceFeatures");
    vulkan->vkGetPhysicalDeviceFeatures2                            = (PFN_vkGetPhysicalDeviceFeatures2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceFeatures2");
    vulkan->vkGetPhysicalDeviceFormatProperties                     = (PFN_vkGetPhysicalDeviceFormatProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceFormatProperties");
    vulkan->vkGetPhysicalDeviceFormatProperties2                    = (PFN_vkGetPhysicalDeviceFormatProperties2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceFormatProperties2");
    vulkan->vkGetPhysicalDeviceImageFormatProperties                = (PFN_vkGetPhysicalDeviceImageFormatProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceImageFormatProperties");
    vulkan->vkGetPhysicalDeviceImageFormatProperties2               = (PFN_vkGetPhysicalDeviceImageFormatProperties2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceImageFormatProperties2");
    vulkan->vkGetPhysicalDeviceMemoryProperties                     = (PFN_vkGetPhysicalDeviceMemoryProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceMemoryProperties");
    vulkan->vkGetPhysicalDeviceMemoryProperties2                    = (PFN_vkGetPhysicalDeviceMemoryProperties2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceMemoryProperties2");
    vulkan->vkGetPhysicalDeviceProperties                           = (PFN_vkGetPhysicalDeviceProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceProperties");
    vulkan->vkGetPhysicalDeviceProperties2                          = (PFN_vkGetPhysicalDeviceProperties2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceProperties2");
    vulkan->vkGetPhysicalDeviceQueueFamilyProperties                = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vulkan->vkGetPhysicalDeviceQueueFamilyProperties2               = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceQueueFamilyProperties2");
    if (error != result_success) {
        log_error("Can't load core Vulkan instance functions.");
        return error;
    }
#if defined(VK_KHR_surface)
    if (vulkan->extensions & vulkan_extension_surface_bit) {
        vulkan->vkDestroySurfaceKHR                                 = (PFN_vkDestroySurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateDevice");
        vulkan->vkGetPhysicalDeviceSurfaceSupportKHR                = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        vulkan->vkGetPhysicalDeviceSurfaceCapabilitiesKHR           = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        vulkan->vkGetPhysicalDeviceSurfaceFormatsKHR                = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        vulkan->vkGetPhysicalDeviceSurfacePresentModesKHR           = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_surface Vulkan instance functions.");
            return error;
        }
    }
#else
    #error VK_KHR_surface is required.
#endif /* VK_KHR_surface */
#if defined(VK_KHR_win32_surface)
    if (vulkan->extensions & vulkan_extension_win32_surface_bit) {
        vulkan->vkCreateWin32SurfaceKHR                             = (PFN_vkCreateWin32SurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateWin32SurfaceKHR");
        vulkan->vkGetPhysicalDeviceWin32PresentationSupportKHR      = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_win32_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_win32_surface */
#if defined(VK_EXT_metal_surface)
    if (vulkan->extensions & vulkan_extension_metal_surface_bit) {
        vulkan->vkCreateMetalSurfaceEXT                             = (PFN_vkCreateMetalSurfaceEXT)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateMetalSurfaceEXT");
        if (error != result_success) {
            log_error("Can't load VK_EXT_metal_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_metal_surface */
#if defined(VK_KHR_wayland_surface) && defined(AMW_NATIVE_WAYLAND)
    if (vulkan->extensions & vulkan_extension_wayland_surface_bit) {
        vulkan->vkCreateWaylandSurfaceKHR                           = (PFN_vkCreateWaylandSurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateWaylandSurfaceKHR");
        vulkan->vkGetPhysicalDeviceWaylandPresentationSupportKHR    = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_wayland_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_wayland_surface */
#if defined(VK_KHR_xcb_surface) && defined(AMW_NATIVE_XCB)
    if (vulkan->extensions & vulkan_extension_xcb_surface_bit) {
        vulkan->vkCreateXcbSurfaceKHR                               = (PFN_vkCreateXcbSurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateXcbSurfaceKHR");
        vulkan->vkGetPhysicalDeviceXcbPresentationSupportKHR        = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_xcb_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_xcb_surface */
#if defined(VK_KHR_android_surface)
    if (vulkan->extensions & vulkan_extension_android_surface_bit) {
        vulkan->vkCreateAndroidSurfaceKHR                           = (PFN_vkCreateAndroidSurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateAndroidSurfaceKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_android_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_android_surface */
#if defined(VK_EXT_headless_surface)
    if (vulkan->extensions & vulkan_extension_headless_surface_bit) {
        vulkan->vkCreateHeadlessSurfaceEXT                          = (PFN_vkCreateHeadlessSurfaceEXT)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateHeadlessSurfaceEXT");
        if (error != result_success) {
            log_error("Can't load VK_EXT_headless_surface Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_EXT_headless_surface */
#if defined(VK_KHR_display) && defined(AMW_NATIVE_KMS)
    if (vulkan->extensions & vulkan_extension_display_bit) {
        vulkan->vkCreateDisplayModeKHR                               = (PFN_vkCreateDisplayModeKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateDisplayModeKHR");
        vulkan->vkCreateDisplayPlaneSurfaceKHR                       = (PFN_vkCreateDisplayPlaneSurfaceKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkCreateDisplayPlaneSurfaceKHR");
        vulkan->vkGetDisplayModePropertiesKHR                        = (PFN_vkGetDisplayModePropertiesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetDisplayModePropertiesKHR");
        vulkan->vkGetDisplayPlaneCapabilitiesKHR                     = (PFN_vkGetDisplayPlaneCapabilitiesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetDisplayPlaneCapabilitiesKHR");
        vulkan->vkGetDisplayPlaneSupportedDisplaysKHR                = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetDisplayPlaneSupportedDisplaysKHR");
        vulkan->vkGetPhysicalDeviceDisplayPlanePropertiesKHR         = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
        vulkan->vkGetPhysicalDeviceDisplayPropertiesKHR              = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetPhysicalDeviceDisplayPropertiesKHR");
        if (error != result_success) {
            log_error("Can't load VK_KHR_display Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_KHR_display */
#if defined(VK_EXT_acquire_drm_display) && defined(AMW_NATIVE_KMS)
    if (vulkan->extensions & vulkan_extension_acquire_drm_display_bit) {
        vulkan->vkAcquireDrmDisplayEXT                              = (PFN_vkAcquireDrmDisplayEXT)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkAcquireDrmDisplayEXT");
        vulkan->vkGetDrmDisplayEXT                                  = (PFN_vkGetDrmDisplayEXT)(void *)get_vulkan_instance_proc_address(&error, vulkan, instance, "vkGetDrmDisplayEXT");
        if (error != result_success) {
            log_error("Can't load VK_EXT_acquire_drm_display Vulkan instance functions.");
            return error;
        }
    }
#endif /* VK_EXT_acquire_drm_display */
    return error;
}

i32 vulkan_load_device_pointers(struct vulkan_api *vulkan, VkDevice device)
{
    if (!vulkan || !vulkan->vkGetDeviceProcAddr)
        return result_error_invalid_context;

    i32 error = result_success;

    return error;
}
