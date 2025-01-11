/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_PELAGIA_H
#define _AMW_PELAGIA_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/os.h>

#include <lake/datastructures/arena_allocator.h>

#include <lake/ipomoeaalba.h>
#include <lake/hadopelagic.h>
#include <lake/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/* forward declarations */
struct pelagia;

/** Types of command queues used in the rendering command stream. */
enum pelagia_command_queue_type {
    pelagia_command_queue_type_none      = 0x00,
    pelagia_command_queue_type_graphics  = 0x01,
    pelagia_command_queue_type_compute   = 0x02,
    pelagia_command_queue_type_transfer  = 0x04,
    pelagia_command_queue_type_all       = 0x07,
};

/** Entry point to the rendering backend. 
 *  @return True if loading the backend module and procedures was successful. */
typedef s32 (*PFN_pelagia_entry_point)(struct pelagia *pelagia, struct ipomoeaalba *ia);

/** Defines ID's of supported rendering backends. */
enum pelagia_backend_api {
    pelagia_backend_api_vulkan = 0,
    pelagia_backend_api_d3dx12,      // TODO reserved id
    pelagia_backend_api_metal3,      // TODO reserved id
    pelagia_backend_api_webgpu,      // TODO reserved id
    pelagia_backend_api_proxy,       // TODO reserved id
    pelagia_backend_api_mock,
};

AMWAPI s32 pelagia_vulkan_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);
AMWAPI s32 pelagia_mock_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);
/** Select the first appropriate rendering backend supported by this build. */
AMWAPI s32 pelagia_entry_point(struct pelagia *pelagia, struct ipomoeaalba *ia);

/** Initialize the internal rendering backend. */
typedef s32 (*PFN_pelagia_renderer_init)(
    struct pelagia         *pelagia, 
    struct ipomoeaalba     *ia, 
    struct hadopelagic     *hadal, 
    const char             *application_name, 
    u32                     application_version, 
    struct arena_allocator *temp_arena);

/** Finalize the internal rendering backend. */
typedef void (*PFN_pelagia_renderer_fini)(struct pelagia *pelagia);

/** Create new rendering devices from physical GPUs for this backend. */
typedef s32 (*PFN_pelagia_construct_devices)(
    struct pelagia         *pelagia,
    struct riven           *riven,
    thread_id              *threads,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    struct arena_allocator *temp_arena);

/** Destroy all existing rendering devices for this backend. */
typedef void (*PFN_pelagia_destroy_devices)(struct pelagia *pelagia);

/** Using the display backend, create a swapchain surface we can draw to. */
typedef s32 (*PFN_pelagia_create_swapchain_surface)(struct pelagia *pelagia, struct hadopelagic *hadal);

/** An argument holding work for creating/recreating a swapchain. */
struct pelagia_construct_swapchain_work {
    struct pelagia     *pelagia;         /**< Holds the swapchain and a device that controls it. */
    struct hadopelagic *hadal;          /**< The display backend will provide necessary window info. */
    struct ipomoeaalba *ia;             /**< Needed for initialization of the swapchain arena, can be NULL'ed afterwards */
    b32                 use_vsync;      /**< Whether to enable vertical synchronization. */
    s32                 out_result;     /**< A return code, check for errors. */
};

/** Creates or recreates the swapchain, for a given display backend and main rendering device. */
RIVENS_TEAR_PFN(PFN_pelagia_construct_swapchain_tear, struct pelagia_construct_swapchain_work *work);

/** Internal calls to be implemented by a rendering backend and assigned from the entry point. */
struct pelagia_calls {
    PFN_pelagia_renderer_init            renderer_init;
    PFN_pelagia_renderer_fini            renderer_fini;
    PFN_pelagia_construct_devices        construct_devices;
    PFN_pelagia_destroy_devices          destroy_devices;
    PFN_pelagia_create_swapchain_surface create_swapchain_surface;
    PFN_pelagia_construct_swapchain_tear construct_swapchain_tear;
};

/** Boolean flags describing the state of the rendering context. */
enum pelagia_flags {
    pelagia_flag_initialized             = (1u << 0),  /**< Set on a valid rendering backend flags value. */
    pelagia_flag_vsync_enabled           = (1u << 1),  /**< Whether vertical synchronization is being used. */
    pelagia_flag_screenshot_supported    = (1u << 2),  /**< Set if the swapchain allows reading and saving the image data. */

    pelagia_flag_reserved_bits           = 20,
};
#define pelagia_flag_mask_reserved ((1u << pelagia_flag_reserved_bits) - 1)

/** Boolean flags describing the state of an explicit rendering device.
 *  The device state is hold internally and API-dependent, but we can use these flags 
 *  to either control or read the state of the device. It's internally represented by 
 *  an atomic uint64_t value, bits from 0 to 39 are reserved, flags from 40 to 63 bits 
 *  are for internal use and as such are backend-dependent. */
enum pelagia_device_flags {
    pelagia_device_flag_is_valid = (1llu << 0), /**< Set on a valid rendering device flags value. */
    pelagia_device_flag_main     = (1llu << 1), /**< True for only one device, it's responsible for controlling the rendering and shared internal constructs, like the swapchain. */

    /** Checks if the device supports accelerated ray tracing meshes - if not, ray tracing passes may only use software implementations instead. */
    pelagia_device_flag_accelerated_ray_tracing_supported = (1llu << 10),

    pelagia_device_flag_reserved_bits = 40,
}; 
/** A bitmask for the reserved bits of a cobalt_device_flags value. */
#define pelagia_device_flag_mask_reserved ((1llu << pelagia_device_flag_reserved_bits) - 1)

/** The renderer. */
struct pelagia {
    at_s32                  flags;          /**< Any write-modify operations outside the backend must be externally synchronized. */

    void                   *backend;        /**< A rendering context shared between devices. */
    void                   *devices;        /**< An array of rendering devices, representing an execution context per GPU. The device of idx 0 is always the main GPU. */
    u32                     device_count;   /**< How many devices are available in the renderer. This value will not change during the lifetime of a rendering backend. */

    u32                     backend_api;    /**< Either enum cobalt_backend_api or a custom identifier. */
    const char             *backend_name;   /**< A readable name of the running rendering backend, for logging purposes. */
    struct pelagia_calls    calls;
};

AMWAPI s32 pelagia_init(
    PFN_pelagia_entry_point entry_point__,
    struct pelagia         *pelagia, 
    struct ipomoeaalba     *ia,
    struct hadopelagic     *hadal, 
    struct riven           *riven,
    const char             *application_name,
    u32                     application_version,
    thread_id              *threads,
    ssize                   thread_count,
    s32                     preferred_main_device_idx,
    s32                     max_device_count,
    b32                     enable_vsync);

AMWAPI void pelagia_fini(struct pelagia *pelagia);

AMW_INLINE void pelagia_construct_swapchain_tear__(
    struct pelagia_construct_swapchain_work *work,
    struct rivens_tear                      *tear) 
{
    tear->procedure = (PFN_riven_tear)work->pelagia->calls.construct_swapchain_tear;
    tear->argument  = work;
    tear->name      = "pelagia:construct_swapchain_tear";
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PELAGIA_H */
