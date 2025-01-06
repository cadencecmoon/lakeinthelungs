#ifndef _AMW_COBALT_H
#define _AMW_COBALT_H

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
typedef struct cobalt cobalt;

/** Entry point to the rendering backend. 
 *  @return True if loading the backend module and procedures was successful. */
typedef s32 (*PFN_cobalt_entry_point)(cobalt *co, ipomoeaalba *ia);

enum cobalt_backend_api {
    cobalt_backend_api_vulkan = 0,
    cobalt_backend_api_d3dx12,      // TODO reserved id
    cobalt_backend_api_metal3,      // TODO reserved id
    cobalt_backend_api_webgpu,      // TODO reserved id
    cobalt_backend_api_proxy,       // TODO reserved id
    cobalt_backend_api_mock,
    cobalt_backend_api_custom = 0x20,
};

AMWAPI s32 cobalt_vulkan_entry_point(cobalt *co, ipomoeaalba *ia);
AMWAPI s32 cobalt_mock_entry_point(cobalt *co, ipomoeaalba *ia);
/** Select the first appropriate rendering backend supported by this build. */
AMWAPI s32 cobalt_entry_point(cobalt *co, ipomoeaalba *ia);

/** Initialize the internal rendering backend. */
typedef s32 (*PFN_cobalt_renderer_init)(
    cobalt          *co, 
    ipomoeaalba     *ia, 
    hadopelagic     *hadal, 
    const char      *application_name, 
    u32              application_version, 
    arena_allocator *temp_arena);

/** Finalize the internal rendering backend. */
typedef void (*PFN_cobalt_renderer_fini)(cobalt *co);

/** Using the display backend, create a swapchain surface we can draw to. */
typedef s32 (*PFN_cobalt_create_swapchain_surface)(cobalt *co, hadopelagic *hadal);

/** Create new rendering devices from physical GPUs for this backend. */
typedef s32 (*PFN_cobalt_construct_devices)(
    cobalt          *co,
    rivens_rift     *riven,
    thread_id       *threads,
    ssize            thread_count,
    s32              preferred_main_device_idx,
    s32              max_device_count,
    arena_allocator *temp_arena);

/** Destroy all existing rendering devices for this backend. */
typedef void (*PFN_cobalt_destroy_devices)(cobalt *co);

/** Internal calls to be implemented by a rendering backend and assigned from the entry point. */
typedef struct {
    PFN_cobalt_renderer_init            renderer_init;
    PFN_cobalt_renderer_fini            renderer_fini;
    PFN_cobalt_create_swapchain_surface create_swapchain_surface;
    PFN_cobalt_construct_devices        construct_devices;
    PFN_cobalt_destroy_devices          destroy_devices;
} cobalt_calls;

/** Boolean flags describing the state of the rendering context. */
enum cobalt_flags {
    cobalt_flag_initialized = (1u << 0),  /**< Set on a valid rendering backend flags value. */

    cobalt_flag_reserved_bits = 20,
};
#define cobalt_flag_mask_reserved ((1u << cobalt_flag_reserved_bits) - 1)

/** Boolean flags describing the state of an explicit rendering device.
 *  The device state is hold internally and API-dependent, but we can use these flags 
 *  to either control or read the state of the device. It's internally represented by 
 *  an atomic uint64_t value, bits from 0 to 39 are reserved, flags from 40 to 63 bits 
 *  are for internal use and as such are backend-dependent. */
enum cobalt_device_flags {
    cobalt_device_flag_is_valid = (1llu << 0), /**< Set on a valid rendering device flags value. */
    cobalt_device_flag_main     = (1llu << 1), /**< True for only one device, it's responsible for controlling the rendering and shared internal constructs, like the swapchain. */

    /** Checks if the device supports accelerated ray tracing meshes - if not, ray tracing passes may only use software implementations instead. */
    cobalt_device_flag_accelerated_ray_tracing_support = (1llu << 10),

    cobalt_device_flag_reserved_bits = 40,
}; 
/** A bitmask for the reserved bits of a cobalt_device_flags value. */
#define cobalt_device_flag_mask_reserved ((1llu << cobalt_device_flag_reserved_bits) - 1)

/** The renderer. */
struct cobalt {
    at_s32           flags;          /**< Any write-modify operations outside the backend must be externally synchronized. */

    void            *renderer;       /**< A rendering context shared between devices. */
    void            *devices;        /**< An array of rendering devices, representing an execution context per GPU. The device of idx 0 is always the main GPU. */
    u32              device_count;   /**< How many devices are available in the renderer. This value will not change during the lifetime of a rendering backend. */

    u32              backend_api;    /**< Either enum cobalt_backend_api or a custom identifier. */
    const char      *backend_name;   /**< A readable name of the running rendering backend, for logging purposes. */
    cobalt_calls     calls;
};

#ifndef AMW_NO_PROTOTYPES

AMWAPI s32 cobalt_init(
    PFN_cobalt_entry_point entry_point__,
    cobalt                *co, 
    ipomoeaalba           *ia,
    hadopelagic           *hadal, 
    rivens_rift           *riven,
    const char            *application_name,
    u32                    application_version,
    thread_id             *threads,
    ssize                  thread_count,
    s32                    preferred_main_device_idx,
    s32                    max_device_count);

AMWAPI void cobalt_fini(cobalt *co);

#endif /* AMW_NO_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_COBALT_H */
