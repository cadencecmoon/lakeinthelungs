#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque handle of the rendering backend. Can be safely cast into 
 *  (struct silvera_interface *) if obtained from a valid renderer implementation. */
struct silvera;

/** Information needed to construct a rendering backend. */
struct silvera_overture {
    struct rivens_overture_header   header;
    b32 debug_utilities;
};

#ifdef SILVERA_D3D12
/** The entry point to a Windows Direct 3D 12 Ultimate rendering backend. */
AMWAPI RIVENS_ENCORE(silvera, d3d12);
#endif
#ifdef SILVERA_METAL
/** The entry point to an Apple Metal3 rendering backend. */
AMWAPI RIVENS_ENCORE(silvera, metal);
#endif
#ifdef SILVERA_VULKAN
/** The entry point to a WebGPU rendering backend. */
AMWAPI RIVENS_ENCORE(silvera, vulkan);
#endif
#ifdef SILVERA_WEBGPU
/** The entry point to a Vulkan rendering backend. */
AMWAPI RIVENS_ENCORE(silvera, webgpu);
#endif

/** The entry point to a stub rendering backend, available on all platforms.
 *  This is a valid API that provides a zeroed-out configuration. */
AMWAPI RIVENS_ENCORE(silvera, stub);

/** Writes a list of encores native to the host system. May write a dummy encore if requested,
 *  but it will be written if there is no other rendering backend available.
 *
 *  The following encores may be written:
 *  - d3d12 - Windows platforms with DX12 drivers.
 *  - metal - Apple platforms with Metal 3 drivers (Apple Silicon).
 *  - vulkan - All platforms (except Emscripten) with Vulkan 1.2+ drivers.
 *  - webgpu - WebAssembly builds, or expose layers from native implementations I guess.
 *  - stub - if requested, or as a fallback.
 *
 *  Available encores are written in the order they are declared.
 *
 *  Other possible rendering backends that could be created:
 *      GNM (PS5), NVN, AGC, serialized proxy, older APIs(OpenGL, OpenGL ES, DX11, etc.)? */
AMWAPI attr_nonnull_all
const PFN_rivens_encore *silvera_acquire_native_encores(u32 *out_count, b32 fallback);

/** Procedures to be provided by an implementation. The backend must implement the 'struct silvera' and 
 *  put the silvera_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the rendering backend and the public interface defined below. */
struct silvera_interface {
    struct rivens_interface_header  header;
};

#ifdef __cplusplus
}
#endif
