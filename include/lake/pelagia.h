/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_PELAGIA_H
#define _AMW_PELAGIA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/assert.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/os.h>

#include <lake/datastructures/arena_allocator.h>
#include <lake/datastructures/render_graph.h>

#include <lake/ipomoeaalba.h>
#include <lake/hadopelagic.h>
#include <lake/riven.h>
#include <lake/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Boolean flags describing the state of an explicit rendering device.
 *  The device state is hold internally and API-dependent, but we can use these flags 
 *  to either control or read the state of the device. It's internally represented by 
 *  an atomic uint64_t value, bits from 0 to 39 are reserved, flags from 40 to 63 bits 
 *  are for internal use and as such are backend-dependent. */
enum pelagia_device_flags {
    pelagia_device_flag_is_valid                            = (1llu << 0), /**< Set on a valid rendering device flags value. */
    pelagia_device_flag_primary                             = (1llu << 1),  /**< True for only one device, it's responsible for controlling the rendering and shared internal constructs, like the swapchain. */
    pelagia_device_flag_virtual                             = (1llu << 2),  /**< True if it's a virtual logical device, created from the same primary physical device. Used for testing mGPU setups on single GPU PCs. */

    pelagia_device_flag_accelerated_ray_tracing_supported   = (1llu << 10), /**< Checks if the device supports accelerated ray tracing meshes - if not, ray tracing passes may only use software implementations instead. */
}; 

/** Boolean flags describing the state of the rendering context. */
enum pelagia_flags {
    pelagia_flag_initialized                                = (1u << 0),  /**< Set on a valid rendering backend flags value. */
    pelagia_flag_vsync_enabled                              = (1u << 1),  /**< Whether vertical synchronization is being used. */
    pelagia_flag_screenshot_supported                       = (1u << 2),  /**< Set if the swapchain allows reading and saving the image data. */
};

/** Acceptable values for the frames_buffering variable. Dual buffering is best 
 *  not used in parallel gameloops, triple buffering is the default. */
enum pelagia_frames_buffering {
    pelagia_frames_dual_buffering   = 2,
    pelagia_frames_triple_buffering = 3,
    pelagia_frames_quad_buffering   = 4,
};

/** The renderer. */
struct pelagia {
    at_u32                   flags;             /**< Any write-modify operations must be externally synchronized. */
    u32                      frames_buffering;  /**< The amount of frames we are buffering for, corresponds to the swapchain image count. */

    struct vulkan_backend    vk;                /**< A rendering context shared between devices. */
    struct vulkan_device    *devices;           /**< An array of rendering devices, representing an execution context per GPU. The device of idx 0 is always the primary GPU. */
    u32                      device_count;      /**< How many devices are available in the renderer. This value will not change during the lifetime of a rendering backend. */

    struct vulkan_swapchain  swapchain;         /**< Holds images for presenting to the window, controlled by the primary device. */
    struct vulkan_images    *render_targets;    /**< Render target type per frame, shared between devices. */
    struct vulkan_buffers   *constant_buffers;  /**< Buffers holding frame constants per frame, shared between devices. */

    struct arena_allocator   arena;  /**< XXX A memory pool for all image count dependent dynamic state, and for initialization. */
};

/** The information needed to create a renderer and a collection of rendering devices. */
struct pelagia_renderer_init_work {
    struct pelagia     *pelagia;                /**< The renderers state to be initialized. */
    struct ipomoeaalba *ia;                     /**< The memory backend allocator TODO. */
    struct hadopelagic *hadal;                  /**< The display backend */
    const char         *application_name;       /**< Application internal name. */
    u32                 application_version;    /**< Application internal version. */
    struct riven       *riven;                  /**< Some initialization work will be parallelized later. */
    thread_id          *threads;                /**< To query the current thread idx. */
    u32                 thread_count;           /**< We must know how many worker threads may access the rendering state. */
    u32                 max_frames_buffering;   /**< Max swapchain images to run (clamped between 2 - 4). */

    s32                 preferred_primary_device_idx;   /**< If a physical device of this index exists, it will be selected as the primary rendering device. */
    s32                 max_physical_device_count;      /**< A limit to how many rendering devices may be created. If 0 or less, there is no limit. */
    s32                 virtual_device_count;           /**< Added to the total max device count, creates logical devices from a selected primary device. Useful only for testing mGPU features on single GPU setups. */

    b32                 enable_vsync;           /**< Whether vertical synchronization should be enabled. */
    s32                 out_result;             /**< The result of this work. */
};

/** Create a renderer. */
AMWAPI void pelagia_renderer_init(struct pelagia_renderer_init_work *work);

/** The information needed to destroy the renderer. */
struct pelagia_renderer_fini_work {
    struct pelagia *pelagia;
};

/** Destroy a renderer. */
AMWAPI void pelagia_renderer_fini(struct pelagia_renderer_fini_work *work);

/** An argument holding work for creating/recreating a swapchain. */
struct pelagia_reconstruct_swapchain_work {
    struct pelagia     *pelagia;        /**< Holds the swapchain and a device that controls it. */
    struct hadopelagic *hadal;          /**< The display backend will provide necessary window info. */
    b32                 use_vsync;      /**< Whether to enable vertical synchronization. */
    s32                 out_result;     /**< A return code, check for errors. */
};

/** Creates or recreates the swapchain, for a given display backend and main rendering device. */
AMWAPI void pelagia_reconstruct_swapchain(struct pelagia_reconstruct_swapchain_work *work);

/** An argument for defining the render passes to be recreated. The pipelines are constructed
 *  for all devices in the provided pelagia structure. */
struct pelagia_construct_shader_pipelines_work {
    struct pelagia     *pelagia;        /**< Holds the devices for whom pipelines will be built. */
    struct riven       *riven;          /**< Creating the pipelines can be parallelized. */
    s32                 out_result;     /**< A return code, check for errors. */
};

/** Creates or recreates the defined shader pipelines. */
AMWAPI void pelagia_construct_shader_pipelines(struct pelagia_construct_shader_pipelines_work *work);

struct pelagia_construct_render_targets_work {
    struct pelagia     *pelagia;        /**< Holds the devices for whom pipelines will be built. */
    struct riven       *riven;          /**< Creating the pipelines can be parallelized. */
    s32                 out_result;     /**< A return code, check for errors. */
};

/** Create render targets for all the internal render passes. */
AMWAPI void pelagia_construct_render_targets(struct pelagia_construct_render_targets_work *work);

/** All renderer's work can be defined as tears, and at will parallelized. */
#define PELAGIA_RIVENS_TEAR(proc) \
    AMW_INLINE void pelagia_##proc##_tear__( \
        struct pelagia_##proc##_work *work, \
        struct rivens_tear *tear) \
    { \
        tear->procedure = (PFN_riven_work)pelagia_##proc; \
        tear->argument = work; \
        tear->name = "pelagia:" #proc; \
    }

PELAGIA_RIVENS_TEAR(renderer_init);
PELAGIA_RIVENS_TEAR(renderer_fini);
PELAGIA_RIVENS_TEAR(reconstruct_swapchain);
PELAGIA_RIVENS_TEAR(construct_shader_pipelines);
PELAGIA_RIVENS_TEAR(construct_render_targets);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PELAGIA_H */
