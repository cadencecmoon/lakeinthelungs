#ifndef _INTERNAL_RENDERER_H
#define _INTERNAL_RENDERER_H

#include <lake/platynova.h>

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk_platynova.h"
#endif

struct platynova {
    struct platinum    *plat;

    at_uint32_t         flags;

    void               *internal;
};

#endif /* _INTERNAL_RENDERER_H */
