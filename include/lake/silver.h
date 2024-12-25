#ifndef _AMW_SILVER_H
#define _AMW_SILVER_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

enum silver_backend {
    silver_backend_auto = 0u,
    silver_backend_vulkan,
    silver_backend_dx12,    /* TODO */
    silver_backend_metal,   /* TODO */
    silver_backend_webgpu,  /* TODO */
    silver_backend_proxy,   /* TODO */
    silver_backend_mock,    /* no API, resource tracking and validation for testing/debugging */
    silver_backend_count,
};

/** An opaque context for a rendering backend. Multiple rendering backends can exist. */
struct silver;

/** An opaque context for a logical rendering device. This can abstract over a physical GPU */
struct silvdevice;

/** TODO rendering graph */
struct silvgraph;

/* TODO 
 * handles for render resources
 * parsing render commands/passes
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
