#pragma once

#include <amw/bedrock.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque handle of the rendering backend. Can be safely cast into 
 *  (struct pelagia_interface *) if obtained from a valid renderer implementation. */
struct pelagia;

/** Information needed to construct a rendering backend. */
struct pelagia_overture {
    struct rivens_overture_header   header;
    b32 debug_utilities;
};

#ifdef PELAGIA_D3D12
/** The entry point to a Windows Direct 3D 12 Ultimate rendering backend. */
AMWAPI RIVENS_ENCORE(pelagia, d3d12);
#endif
#ifdef PELAGIA_METAL
/** The entry point to an Apple Metal3 rendering backend. */
AMWAPI RIVENS_ENCORE(pelagia, metal);
#endif
#ifdef PELAGIA_VULKAN
/** The entry point to a WebGPU rendering backend. */
AMWAPI RIVENS_ENCORE(pelagia, vulkan);
#endif
#ifdef PELAGIA_WEBGPU
/** The entry point to a Vulkan rendering backend. */
AMWAPI RIVENS_ENCORE(pelagia, webgpu);
#endif

/** The entry point to a stub rendering backend, available on all platforms.
 *  This is a valid API that provides a zeroed-out configuration. */
AMWAPI RIVENS_ENCORE(pelagia, stub);

/** Writes a list of encores native to the host system. May write a fallback encore if 
 *  requested, but it will be written anyways if there is no other rendering backend available.
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
const PFN_rivens_encore *pelagia_acquire_native_encores(u32 *out_count, b32 fallback);

/** Procedures to be provided by an implementation. The backend must implement the 'struct silvera' and 
 *  put the silvera_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the rendering backend and the public interface defined below. */
struct pelagia_interface {
    struct rivens_interface_header  header;
};

#ifdef __cplusplus
}
#endif
