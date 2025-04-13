#include "vk_xaku.h"
#ifdef XAKU_VULKAN

enum extension_bits {
    extension_surface_bit               = (1u << 0),    /**< VK_KHR_surface */
    extension_win32_surface_bit         = (1u << 1),    /**< VK_KHR_win32_surface */
    extension_metal_surface_bit         = (1u << 2),    /**< VK_EXT_metal_surface */
    extension_android_surface_bit       = (1u << 3),    /**< VK_KHR_android_surface */
    extension_wayland_surface_bit       = (1u << 4),    /**< VK_KHR_wayland_surface */
    extension_xcb_surface_bit           = (1u << 5),    /**< VK_KHR_xcb_surface */
    extension_headless_surface_bit      = (1u << 6),    /**< VK_KHR_headless_surface */
    extension_display_bit               = (1u << 7),    /**< VK_KHR_display */
    extension_debug_utils_bit           = (1u << 8),    /**< VK_EXT_debug_utils */
    extension_layer_validation_bit      = (1u << 9),    /**< VK_LAYER_KHRONOS_validation */
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
