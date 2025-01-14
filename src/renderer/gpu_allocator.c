#include <lake/vulkan.h>

AMWAPI s32 vulkan_find_memory_type(
    u32                             *out_type_idx, 
    VkPhysicalDeviceMemoryProperties memory_properties, 
    u32                              memory_type_bits,
    VkMemoryPropertyFlags            property_mask)
{
    for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (memory_type_bits & (1 << i)) {
            if ((memory_properties.memoryTypes[i].propertyFlags & property_mask) == property_mask) {
                *out_type_idx = i;
                return 0;
            }
        }
    }
    return 1;
}
