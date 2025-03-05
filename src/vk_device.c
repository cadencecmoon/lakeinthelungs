#include "vk_pelagial.h"

static PFN_vkVoidFunction device_proc_address(
    const struct vulkan_backend *vulkan, 
    const struct vulkan_device  *device, 
    const char                  *procname)
{
    PFN_vkVoidFunction address = vulkan->vkGetDeviceProcAddr(device->logical, procname);
    if (address == NULL)
        log_debug("Can't find a Vulkan device function of name '%s'", procname);
    return address;
}
