#pragma once

#include <amwe/xaku/encore.h>
#include <amwe/xaku/commands.h>
#include <amwe/xaku/gpu_resources.h>
#include <amwe/xaku/pipelines.h>
#include <amwe/xaku/swapchain.h>
#include <amwe/xaku/device.h>
#include <amwe/xaku/render_graph.h>

#ifndef XAKU_MAX_ENCORE_COUNT
    #define XAKU_MAX_ENCORE_COUNT 1
#endif /* XAKU_MAX_ENCORE_COUNT */

#ifndef XAKU_ENABLE_GPU_PROFILER
    #ifndef LAKE_NDEBUG
        #define XAKU_ENABLE_GPU_PROFILER 1
    #else
        #define XAKU_ENABLE_GPU_PROFILER 0
    #endif /* LAKE_NDEBUG */
#endif /* XAKU_ENABLE_GPU_PROFILER */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the rendering engine, implemented by a backend 'xaku_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct xaku_interface {
    struct riven_interface_header           header;
    struct xaku_interface_cmd               cmd;
};

/** A view into the backend. */
union xaku_encore_view {
    struct riven_interface_header          *header;
    struct xaku_interface                  *interface;
    struct xaku_encore                     *encore;
};

/** A view into a rendering device. */
union xaku_device_view {
    struct xaku_device_header              *header;
    struct xaku_device                     *data;
};

/** Implies different strategies for the rendering engine. */
enum xaku_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    xaku_strategy_auto = 0,
    /** Rendering is done on a single device, with only one rendering backend at a time. */
    xaku_strategy_optimal,
    /** Multi-GPU setting with one encore, using a primary device 
     *  and multiple secondary devices. Implies that the system is or will be 
     *  running on more than one rendering device (from one or more backends). */
    xaku_strategy_optimal_mgpu,
};

/** An engine structure for GPU renderering. */
struct xaku {
    enum xaku_strategy                      strategy;
    atomic_u32                              flags;
    union xaku_encore_view                  backends[XAKU_MAX_ENCORE_COUNT];

    union xaku_device_view                 *devices;
    u32                                     device_count;
    u32                                     device_capacity;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
