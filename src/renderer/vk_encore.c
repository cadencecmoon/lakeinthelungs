#include "vk_xaku.h"
#ifdef XAKU_VULKAN

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
    instance_extension_layer_validation         = (1u << 9),    /**< VK_LAYER_KHRONOS_validation */
};

FN_RIVEN_ENCORE(xaku, vulkan)
{
    (void)riven;
    (void)metadata;
    (void)riven_hints;
    (void)userdata;
    (void)tag;
    return NULL;
}

#endif /* XAKU_VULKAN */
