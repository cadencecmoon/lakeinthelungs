#ifndef _AMW_PLATYNOVA_H
#define _AMW_PLATYNOVA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#include <lake/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/* forward declarations */
struct hadal;
struct platinum;

/** Boolean flags describing the state of a logical GPU device. */
enum platynova_flags {
    platynova_flag_is_the_main_device               = (1u << 0),  /**< There should be only one main device per backend. */
    platynova_flag_used_for_presentation            = (1u << 1),  /**< This device controls the swapchain. */

    platynova_flag_hardware_raytracing_support      = (1u << 10), /**< The GPU has ray tracing cores and supports the related extensions. */
    platynova_flag_use_raytracing_accelerated       = (1u << 11), /**< Use accelerated implementations of ray tracing techniques, a GPU device must support them. */
    platynova_flag_use_raytracing_soft              = (1u << 12), /**< Use non-accelerated soft implementations of ray tracing techniques, will be less performant but supported by any GPU. */
};

/** An opaque context of a logical GPU device. Multiple devices can be explicitly used */
struct platynova;

AMWAPI int32_t platinum_construct_novas(
        struct platinum *plat, 
        int32_t preferred_main_idx, 
        uint32_t max_nova_count, 
        uint32_t thread_count);
AMWAPI void platinum_implode_novas(struct platinum *plat);

AMWAPI int32_t platinum_init(
        struct platinum *plat, 
        struct hadal *hadal, 
        const char *application_name,
        uint32_t application_version);
AMWAPI void platinum_fini(struct platinum *plat);

struct platinum {
    struct platynova  **novas;
    uint32_t            nova_count;

    /* Internal cast to a backend specific instance state: struct platinum_vulkan, etc. */
    void               *internal;
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PLATYNOVA_H */
