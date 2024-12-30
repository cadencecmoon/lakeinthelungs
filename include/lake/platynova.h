#ifndef _AMW_PLATYNOVA_H
#define _AMW_PLATYNOVA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#include <lake/hadopelagic.h>
#include <lake/ipomoeaalba.h>
#include <lake/riven.h>

#include <lake/datastructures/arena_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLATYNOVA_MAX_IMAGES 3

/* forward declarations */
struct platinum;
struct platynova;

/** Entry point to the Platinum rendering backend. 
 *  @return True if loading the backend module and procedures was successful.
 *
 *  Available rendering backends in A Moonlit Walk are: 
 *  - Vulkan 1.3
 *  - TODO Direct3D 12 Ultimate ?
 *  - TODO Metal 3 ?
 *  - TODO WebGPU ?
 *  - TODO Proxy ?
 *  - Mock (no api, validation and debugging) */
typedef int32_t (AMWAPIENTRY *PFN_platinum_entry_point)(struct platinum *plat, struct ipomoeaalba *ia);

#ifndef AMW_NO_PROTOTYPES
AMWAPI int32_t AMWAPIENTRY platinum_vulkan_entry_point(struct platinum *plat, struct ipomoeaalba *ia);
AMWAPI int32_t AMWAPIENTRY platinum_mock_entry_point(struct platinum *plat, struct ipomoeaalba *ia);
/** Select the first appropriate rendering backend supported by this build. */
AMWAPI int32_t AMWAPIENTRY platinum_entry_point(struct platinum *plat, struct ipomoeaalba *ia);
#endif /* AMW_NO_PROTOTYPES */

/** Initialize the internal rendering backend. */
typedef int32_t (AMWAPIENTRY *PFN_platinum_init)(struct platinum *plat, struct ipomoeaalba *ia, struct hadopelagic *hadal, const char *application_name, uint32_t application_version, struct arena_allocator *temp_arena);

/** Finalize the internal rendering backend. */
typedef void    (AMWAPIENTRY *PFN_platinum_fini)(struct platinum *plat);

/** Boolean flags describing the state of the rendering context. */
enum platinum_flags {
    platinum_flag_initialized                   = (1u << 0),  /** Set on all valid render backends. */

    platinum_flag_vsync_enabled                 = (1u << 1),  /** Whether to use screen vertical synchronization. */
    platinum_flag_use_raytracing_accelerated    = (1u << 11), /**< Use accelerated implementations of ray tracing techniques, a GPU device must support them. */
    platinum_flag_use_raytracing_soft           = (1u << 12), /**< Use non-accelerated soft implementations of ray tracing techniques instead, will be less performant but supported by any GPU. */

    platinum_flag_recreate_swapchain            = (1u << 16), /** If set, the swapchain will be recreated next iteration. */
};

struct platinum {
    at_uint32_t         flags;

    struct {
        PFN_platinum_init init;
        PFN_platinum_fini fini;
    } calls;

    struct platynova   *novas;
    uint32_t            nova_count;
    uint32_t            thread_count;

    void               *internal_plat;  /**< Internal cast to a backend specific instance state: struct platinum_vulkan, etc. */
    void               *internal_novas; /**< Internal cast to an array of backend specific device state: struct platynova_vulkan[], etc. */
};

#ifndef AMW_NO_PROTOTYPES

AMWAPI int32_t AMWAPIENTRY 
platinum_init(
        PFN_platinum_entry_point entry_point__,
        struct platinum *plat, 
        struct ipomoeaalba *ia,
        struct hadopelagic *hadal, 
        const char *application_name,
        uint32_t application_version,
        uint32_t thread_count);

AMWAPI void AMWAPIENTRY 
platinum_fini(struct platinum *plat);

AMWAPI int32_t AMWAPIENTRY 
platinum_construct_novas(
        struct platinum *plat, 
        struct ipomoeaalba *ia,
        struct riven *riven,
        int32_t preferred_main_idx, 
        uint32_t max_nova_count,
        bool use_raytracing);

AMWAPI void AMWAPIENTRY 
platinum_implode_novas(struct platinum *plat);

#endif /* AMW_NO_PROTOTYPES */

/** Boolean flags describing the state of a logical GPU device. */
enum platynova_flags {
    platynova_flag_is_the_main_device               = (1u << 0),  /**< There should be only one main device per backend, this device will control the rendering logic. */

    platynova_flag_hardware_raytracing_support      = (1u << 10), /**< The GPU has ray tracing cores and supports the related extensions. */
};

/** An context of a logical GPU device. Multiple devices can be created and explicitly used. */
struct platynova {
    struct platinum    *plat;

    at_uint32_t         flags;

    void               *internal_nova;
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PLATYNOVA_H */
