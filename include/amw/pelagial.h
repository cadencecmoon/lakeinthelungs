#pragma once

#include <amw/riven.h>
#include <amw/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PELAGIAL_MAX_DEVICES
#define PELAGIAL_MAX_DEVICES 16
#endif

#ifndef PELAGIAL_MAX_FRAMES_IN_FLIGHT
#define PELAGIAL_MAX_FRAMES_IN_FLIGHT 4
#endif

/* forward declarations */
struct hadopelagic;
struct pelagic_ocean;
struct pelagic_ocean_interface;
struct pelagic_ocean_create_info;

/* opaque handles */
typedef void *pelagic_physical_device_t;
typedef void *pelagic_device_t;
typedef void *pelagic_swapchain_t;

/** Numeric values to distinguish between the available rendering backends.
 *
 *  Possible rendering backends may be devkitPro Deco3D, GNM, 
 *  and other low-level, either free or proprietary APIs.
 *
 *  APIs like OpenGL, OpenGL|ES were not really targeted with 
 *  the design of this renderer, mostly because of deferred and multithreading. */
enum pelagic_backend_id {
    pelagic_backend_invalid = 0u,
    pelagic_backend_d3d12,
    pelagic_backend_metal,
    pelagic_backend_vulkan,
    pelagic_backend_webgpu,
    pelagic_backend_null,
    pelagic_backend_id_count,
};

/** Arguments of the entry point to the rendering backend. */
#define _PELAGIAL_ENTRY_POINT_ARGS                                \
    struct pelagic_ocean_interface         *restrict interface,   \
    const struct pelagic_ocean_create_info *restrict create_info, \
    b32                                              verbose

/** Defines an entry point for the rendering backend. Verbose will enable log messages, including errors. */
typedef s32 attr_nonnull_all (*PFN_pelagial_entry_point)(_PELAGIAL_ENTRY_POINT_ARGS);

/** Declares a pelagial entry point implementation. */
#define PELAGIAL_ENTRY_POINT(name) \
    s32 attr_nonnull_all pelagial_##name##_entry_point(_PELAGIAL_ENTRY_POINT_ARGS)

AMWAPI PELAGIAL_ENTRY_POINT(d3d12);
AMWAPI PELAGIAL_ENTRY_POINT(metal);
AMWAPI PELAGIAL_ENTRY_POINT(vulkan);
AMWAPI PELAGIAL_ENTRY_POINT(webgpu);
AMWAPI PELAGIAL_ENTRY_POINT(null);

/** Picks the first valid entry point from the available above. */
AMWAPI s32 pelagial_entry_point(_PELAGIAL_ENTRY_POINT_ARGS);

/** Initializes the rendering backend. */
typedef s32 (*PFN_pelagial_renderer_init)(
    struct pelagic_ocean                   *restrict pelagial,
    const struct pelagic_ocean_create_info *restrict create_info);

/** Cleanups the rendering backend. */
typedef void (*PFN_pelagial_renderer_fini)(void *restrict internal);

/** Procedures implemented by the rendering backend. */
struct pelagic_ocean_interface {
    u32                         id;         /**< The numeric identifier of the rendering backend. */
    const char                 *name;       /**< A readable name of the rendering backend, for logging. */
    void                       *internal;   /**< The rendering backend. */

    PFN_pelagial_renderer_init  renderer_init;
    PFN_pelagial_renderer_fini  renderer_fini;
};

/** Flags controlling the renderer. */
enum pelagic_flags {
    pelagic_flag_renderer_is_valid  = (1u << 0), /**< Always true for a valid rendering backend. */
    pelagic_flag_surface_lost       = (1u << 1), /**< Set on display errors. */
    pelagic_flag_vsync_enabled      = (1u << 2), /**< Whether vertical synchronization is in use. */
};

/** Holds instance state, interfaces and devices of the rendering backend.
 *
 *  Procedures implemented by every rendering backend, divided into three types:
 *  - backend API, used to query physical devices, create logical devices, control state; 
 *  - resource API, used to create resources owned by a GPU on device memory (textures, buffers, etc.);
 *  - command API, used to record rendering commands to be executed on the GPU; */
struct pelagic_ocean {
    at_u32                         flags;
    struct pelagic_ocean_interface interface;

    rivens_tag_t                   tag;    /**< The lifetime of this system. */
    struct rivens                 *riven; 

    /** We register all available physical devices upfront, rendering devices 
     *  can be created from referencing a particular physical device index.
     *  Multiple devices can be created from one physical device (virtual mGPU)
     *  for testing purposes, in practice, max one device per physical is used. */
    pelagic_physical_device_t     *physical_devices;
    /** How many physical devices are available to use. */
    u32                            physical_device_count;

    /** Rendering devices are opaque handles and created from a backend. 
     *  A device can be used to manage GPU resources and execute GPU commands. */
    pelagic_device_t               devices[PELAGIAL_MAX_DEVICES];
    /** How many devices are currently in use. */
    u32                            device_count;

    /** We support only one window, so one swapchain is enough. */
    pelagic_swapchain_t            swapchain;
};

/** Information needed to construct a rendering backend. */
struct pelagic_ocean_create_info {
    struct hadopelagic     *hadal;
    struct rivens          *riven;
    rivens_tag_t            tag;
    const char             *app_name;
    u32                     app_version;
    const char             *engine_name;
    u32                     engine_version;
    b32                     enable_debug_utilities;
};

/** Initialize the backend instance for the renderer. If an entry point fails, the next 
 *  entry point in the given array is used as a fallback, until initialized successfully 
 *  or until there are no more entry points requested to try. */
AMWAPI attr_nonnull_all
struct pelagic_ocean *pelagial_init(
    const PFN_pelagial_entry_point         *entries,
    u32                                     entry_count,
    const struct pelagic_ocean_create_info *create_info,
    b32                                     verbose);

/** Destroy the rendering backend bound to this renderer.  */
AMWAPI attr_nonnull_all
void pelagial_fini(struct pelagic_ocean *pelagial);

struct pelagic_work_touch_swapchain {
    WORK_STRUCTURE_HEADER
    pelagic_device_t        device;         /**< A swapchain is bound to a device, only one can control it. */
    pelagic_swapchain_t     swapchain;      /**< The swapchain to be touched. */
    struct hadopelagic     *hadal;          /**< We need to interface with the display backend. */
    b32                     use_vsync;      /**< Whether vertical synchronization is to be used. */
};

/** Control a swapchain state. */
AMWAPI attr_nonnull_all
void pelagial_touch_swapchain(struct pelagic_work_touch_swapchain *restrict work);

#define PELAGIAL_JOB_DECL(fn)                               \
    attr_inline void pelagial_##fn##_job__(                 \
        struct pelagic_work_##fn *work,                     \
        struct rivens_work *context)                        \
    {                                                       \
        context->procedure = (PFN_rivens_job)pelagial_##fn; \
        context->argument = (rivens_arg_t)work;             \
        context->name = "pelagial:" #fn;                    \
    }

PELAGIAL_JOB_DECL(touch_swapchain)

#ifdef __cplusplus
}
#endif
