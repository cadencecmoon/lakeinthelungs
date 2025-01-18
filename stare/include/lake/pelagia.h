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

#include <lake/ipomoea.h>
#include <lake/hadopelagic.h>
#include <lake/riven.h>
#include <lake/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Boolean flags describing the state of the rendering context. */
enum pelagia_flags {
    pelagia_flag_initialized                        = (1u << 0),    /**< Set on a valid rendering backend flags value. */
    pelagia_flag_vsync_enabled                      = (1u << 1),    /**< Whether vertical synchronization is being used. */
    pelagia_flag_screenshot_supported               = (1u << 2),    /**< Set if the swapchain allows reading and saving the image data. */
    pelagia_flag_swapchain_surface_lost             = (1u << 3),    /**< True if the display backend failed and the surface must be recreated. */
    pelagia_flag_accelerated_ray_tracing_supported  = (1u << 10),   /**< Checks if the device supports accelerated ray tracing meshes - if not, ray tracing passes may only use software implementations instead. */
};

/** Acceptable values for the frames_buffering variable. Dual buffering is best 
 *  not used in parallel gameloops, triple buffering is the default. */
enum pelagia_workload_buffering {
    pelagia_workload_buffering_dual   = 2u,
    pelagia_workload_buffering_triple = 3u,
    pelagia_workload_buffering_quad   = 4u, /* AMW_MAX_WORKLOAD */
};

/** Different render targets used in render passes, except for swapchain images.
 *  The total amount of render targets is multiplied by each swapchain image. */
enum pelagia_render_target_type {
    pelagia_render_target_type_depth_buffer = 0u,
    pelagia_render_target_type_visibility_buffer,
    /** The total amount of render targets. */
    pelagia_render_target_type_count,
};

/** TODO */
struct pelagia_render_targets {
    u32 workload_count;
    void *targets_allocation; /* TODO */
    /** Array of all render targets available from this object. */
    union {
        struct {
            struct vulkan_texture depth_buffer;
            struct vulkan_texture visibility_buffer;
        };
        struct vulkan_texture textures[pelagia_render_target_type_count];
    } *targets;
};

/** TODO */
struct pelagia_uniform_buffers {
    struct vulkan_buffer *buffers;
};

/** The different render passes implemented with Vulkan. Used for a constant-time lookup
 *  of shader pipeline states and descriptor sets. */
enum pelagia_render_pass_type {
    /** The pass that produces the visibility buffer (the smallest conceivable G-buffer) 
     *  by rasterizing all geometry once. Outputs a primitive triangle index. */
    pelagia_render_pass_type_visibility = 0u,
    /** The pass that renders a screen filling triangle to perform deferred shading with polygonal lights. */
    pelagia_render_pass_type_lighting,
    /** Performs ray tracing using a top-level and bottom-level acceleration mesh. Requires hardware
     *  support for the VK_KHR_ray_query and VK_KHR_acceleration_structure extensions. Or, a software 
     *  implementation of a ray tracing pass with AABB structures can be used instead (mostly for testing
     *  on all kinds of hardware, that may not support the ray tracing extensions). */
    pelagia_render_pass_type_ray_tracing,
    /** A forward pass that draws from transparent and translucent materials. The forward pass
     *  mitigates the limitation of the lighting pass, that only renders opaque triangles. */
    pelagia_render_pass_type_translucency,
    /** Renders the user interface of debug tools or in-game UI. */
    pelagia_render_pass_type_interface,
    /** The total amount of render passes. */
    pelagia_render_pass_type_count,
};

/** The renderers execution context. */
struct pelagia {
    at_u32                   flags;                 /**< Any write-modify operations must be externally synchronized. */
    u32                      workload_buffering;    /**< The amount of frames we are buffering for, corresponds to the swapchain image count. */

    struct vulkan_backend    vk;                    /**< Holds backend state that may later be abstracted. This data may be shared between rendering devices. */
    struct vulkan_device     device;                /**< The rendering device, representing an execution context per GPU. For simplicity, we target one device. */
    struct vulkan_swapchain  swapchain;             /**< Holds images for presenting to the window. */
};

/** Members shared between all pelagia work structures. */
struct pelagia_work_result {
    s32                 out_result;                 /**< The result of this work. */
    struct pelagia     *pelagia;                    /**< Renderers state. */
};

/** The information needed to create a renderer and a collection of rendering devices. */
struct pelagia_renderer_init_work {
    s32                 out_result;                 /**< The result of this work. */
    struct pelagia     *pelagia;                    /**< The renderers state to be initialized. */
    struct ipomoea     *ia;                         /**< The memory backend allocator TODO. */
    struct hadopelagic *hadal;                      /**< The display backend */
    const char         *application_name;           /**< Application internal name. */
    u32                 application_version;        /**< Application internal version. */
    struct riven       *riven;                      /**< Some initialization work will be parallelized later. */
    thread_t           *threads;                    /**< To query the current thread idx. */
    u32                 thread_count;               /**< We must know how many worker threads may access the rendering state. */
    u32                 target_workload_buffering;  /**< Max swapchain images to run (clamped between 2 - 4). */
    s32                 preferred_device_idx;       /**< If a physical device of this index exists, it will be selected as the rendering device. */
    b32                 enable_vsync;               /**< Whether vertical synchronization should be enabled. */
};

/** The information needed to destroy the renderer. */
struct pelagia_renderer_fini_work {
    s32                 out_result;                 /**< The result of this work. */
    struct pelagia     *pelagia;                    /**< The renderers state to be finished. */
};

/** The information needed to create or recreate a swapchain. */
struct pelagia_assemble_swapchain_work {
    s32                 out_result;                 /**< The result of this work. */
    struct pelagia     *pelagia;                    /**< Holds the swapchain and a device that controls it. */
    struct hadopelagic *hadal;                      /**< The display backend will provide necessary window info, and expose native interfaces when needed. */
    b32                 use_vsync;                  /**< Whether to enable vertical synchronization. */
    b32                 dissasemble;                /**< If true, the assembly will only cleanup the swapchain state instead. */
};

/** TODO */
struct pelagia_assemble_uniform_buffers_work {
    s32                             out_result;         /**< The result of this work. */
    struct pelagia                 *pelagia;            /**< Uniform buffers will be built for the primary device, for each swapchain image. */
    struct pelagia_uniform_buffers *uniform_buffers;    /**< An array of uniform buffers to be assembled/dissasembled. */
    b32                             dissasemble;        /**< If true, the assembly will only cleanup the uniform buffers instead. */
};


/** TODO */
struct pelagia_assemble_render_targets_work {
    s32                            out_result;          /**< The result of this work. */
    struct pelagia                *pelagia;             /**< Render targets will be built for the primary device, for each swapchain image. */
    struct pelagia_render_targets *render_targets;      /**< An array of render targets to be assembled/dissasembled. */
    b32                            dissasemble;         /**< If true, the assembly will only cleanup the render target textures instead. */
};

/** TODO */
struct pelagia_assemble_shader_pipelines_work {
    s32                 out_result;                 /**< The result of this work. */
    struct pelagia     *pelagia;                    /**< Holds the backend and device for whom pipelines will be built. */
    struct riven       *riven;                      /**< Creating the pipelines can be parallelized. */
    b32                 dissasemble;                /**< If true, the assembly will only cleanup the pipeline state instead. */
};

/** Create a renderer. */
AMWAPI void pelagia_renderer_init(struct pelagia_renderer_init_work *work);

/** Destroy a renderer. */
AMWAPI void pelagia_renderer_fini(struct pelagia_renderer_fini_work *work);

/** Creates or recreates the swapchain, for a given display backend and main rendering device. */
AMWAPI void pelagia_assemble_swapchain(struct pelagia_assemble_swapchain_work *work);

/** Creates an instance of uniform buffers for each workload. */
AMWAPI void pelagia_assemble_uniform_buffers(struct pelagia_assemble_uniform_buffers_work *work);

/** Creates, updates or destroys render targets for each workload. */
AMWAPI void pelagia_assemble_render_targets(struct pelagia_assemble_render_targets_work *work);

/** Creates, recreates or destroys the defined render pass shader pipeline states. */
AMWAPI void pelagia_assemble_shader_pipelines(struct pelagia_assemble_shader_pipelines_work *work);

/** All renderer's work can be defined as tears, and at will parallelized. */
#define PELAGIA_RIVENS_TEAR(proc)                           \
    AMW_INLINE void pelagia_##proc##_tear__(                \
        struct pelagia_##proc##_work *work,                 \
        struct rivens_tear *tear)                           \
    {                                                       \
        tear->procedure = (PFN_riven_work)pelagia_##proc;   \
        tear->argument = work;                              \
        tear->name = "pelagia:" #proc;                      \
    }

/** Don't continue some work if the renderer state is invalid. A basic check for workloads below. */
#define PELAGIA_WORK_INITIALIZED_OR_RETURN \
    if (!work) { return; } else if (!work->pelagia || (at_read_relaxed(&work->pelagia->flags) & pelagia_flag_initialized) == 0) { work->out_result = result_error_invalid_engine_context; return; }

PELAGIA_RIVENS_TEAR(renderer_init);
PELAGIA_RIVENS_TEAR(renderer_fini);
PELAGIA_RIVENS_TEAR(assemble_swapchain);
PELAGIA_RIVENS_TEAR(assemble_uniform_buffers);
PELAGIA_RIVENS_TEAR(assemble_render_targets);
PELAGIA_RIVENS_TEAR(assemble_shader_pipelines);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PELAGIA_H */
