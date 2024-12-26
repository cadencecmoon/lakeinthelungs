#ifndef _AMW_SILVER_H
#define _AMW_SILVER_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/* forward declarations */
struct hadal;
struct riven;

/** An opaque context of the rendering backend. */
struct silver;

/** A backend implements over a native GPU API or in another way interacts with the internal API 
 *  in Silver. For now Vulkan is my primary target for early development. */
enum silver_backend {
    silver_backend_auto = 0u,
    silver_backend_vulkan,  /**< Target core 1.4, minimal target is 1.2 + extensions. */
    silver_backend_dx12,    /**< TODO Target DirectX 12 Ultimate (ver. 12.2) */
    silver_backend_metal,   /**< TODO Target Metal 3 for Apple silicon. */
    silver_backend_webgpu,  /**< TODO Modern browsers, target most recent WebGPU implementation. */
    silver_backend_proxy,   /**< TODO A possible render proxy for routing and streaming render commands (to a mGPU cluster, for example), using something like a gRPC framework. */
    silver_backend_mock,    /**< TODO No GPU API, but resource tracking and validation for testing/debugging. */
    silver_backend_count,
};

/** Boolean flags describing the state of a rendering backend. */
enum silver_flags {
    silver_flag_initialized = (1u << 0),
};

AMWAPI struct silver *silver_init(
        enum silver_backend id,
        struct hadal *hadal,
        const char *application_name);

AMWAPI void silver_fini(struct silver *silver);

/** An opaque context of a logical GPU device. */
struct silvdevice;

/* TODO */
AMWAPI struct silvdevice *silvdevice_create(struct silver *silver);
AMWAPI void silvdevice_destroy(struct silvdevice *device);

/** TODO The rendering graph would be inspired by Frostbite's Frame Graph [O'Donnell 2017].
 *  It should have no concept of a 'frame', with automatic transitions and barriers and support 
 *  for mGPU (mostly implicit and automatic, with an explicit sheduling policy). There should 
 *  be one implementation, regardless of backends, translating from a high level render command 
 *  stream. API differences would be hidden from the render graph. A composition of multiple 
 *  graphs at varying frequencies should be possible, for mixing rendering techniques, assigning
 *  a graph per GPU or remote streaming to a server cluster with the proxy backend. The graph 
 *  would have a construction and evaluation phases. Graph evaluation records high level render 
 *  commands and inserts barriers/transitions, this phase will be highly parallelized with Riven. */
struct silvgraph;

/** Some render graph passes that can be implemented first:
 *  Geometry Buffer, Present, Screenshot, Ray Tracing, Skinning, Visibility, Translucency, Game UI, Editor UI */

/** Device resources associated by handle, with a constant-time lookup, shared and mapped per device using it. */
typedef uint64_t silvhandle_t;

/* TODO */
enum silvhandle_datatype {          /* handle : encoded in 8 most significant bits */
    silvhandle_datatype_buffer      = (0x01llu << 56),
    silvhandle_datatype_texture     = (0x02llu << 56),
};

/* TODO 
 * mapping render handles to device resources
 * translating render commands/passes
 *
 * Virtual GPUs for testing explicit mGPU (create multiple logical devices of one physical device)
 *
 * Render proxy for remote rendering (this will need networking, so only if i'll implement this first).
 * a render proxy should allow any API calls to route remotely over a local network
 * this should make using GPU clusters or platforms without an API support to still render 
 * the graphics from another computer, configured to accept these command calls
 * only rendering is routed, scene and game state is local; this also means that for 
 * example Vulkan or DX12 raytracing can be used on an old android via render proxy. */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_SILVER_H */
