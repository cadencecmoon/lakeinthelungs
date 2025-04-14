#pragma once

#include <amwe/xaku/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** We represent GPU resources (buffers, textures, views, samplers and acceleration structures) as IDs.
 *  They are all accessed via a bindless API, and can represent the same resource on multiple devices. */
typedef u64 xaku_render_handle;
#define XAKU_RENDER_HANDLE_INDEX_BITS       (20u)
#define XAKU_RENDER_HANDLE_INDEX_MASK       ((1ull << XAKU_RENDER_HANDLE_INDEX_BITS) - 1ull)
#define XAKU_RENDER_HANDLE_INDEX_OFFSET     (0u)
#define XAKU_RENDER_HANDLE_VERSION_BITS     (40u)
#define XAKU_RENDER_HANDLE_VERSION_MASK     ((1ull << XAKU_RENDER_HANDLE_VERSION_BITS) - 1ull)
#define XAKU_RENDER_HANDLE_VERSION_OFFSET   (XAKU_RENDER_HANDLE_INDEX_BITS)
#define XAKU_RENDER_HANDLE_TYPE_BITS        (4u)
#define XAKU_RENDER_HANDLE_TYPE_MASK        ((1ull << XAKU_RENDER_HANDLE_TYPE_BITS) - 1ull)
#define XAKU_RENDER_HANDLE_TYPE_OFFSET      (XAKU_RENDER_HANDLE_INDEX_BITS + XAKU_RENDER_HANDLE_VERSION_BITS)

#define xaku_index_of_handle(HANDLE) \
    ((u32)((HANDLE >> XAKU_RENDER_HANDLE_INDEX_OFFSET) & XAKU_RENDER_HANDLE_INDEX_MASK))
#define xaku_version_of_handle(HANDLE) \
    ((u64)((HANDLE >> XAKU_RENDER_HANDLE_VERSION_OFFSET) & XAKU_RENDER_HANDLE_VERSION_MASK))
#define xaku_type_of_handle(HANDLE) \
    ((enum xaku_render_handle_type)((HANDLE >> XAKU_RENDER_HANDLE_TYPE_OFFSET) & XAKU_RENDER_HANDLE_TYPE_MASK))

enum xaku_render_handle_type {
    xaku_render_handle_type_invalid         = 0,
    xaku_render_handle_type_buffer          = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 0),
    xaku_render_handle_type_texture         = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 1),
    xaku_render_handle_type_texture_view    = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 2),
    xaku_render_handle_type_sampler         = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 3),
    xaku_render_handle_type_tlas            = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 4),
    xaku_render_handle_type_blas            = ((1ull << XAKU_RENDER_HANDLE_TYPE_OFFSET) + 5),
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
