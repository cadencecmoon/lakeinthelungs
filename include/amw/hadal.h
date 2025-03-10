#pragma once

#include <amw/bedrock.h>
#include <amw/input.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque handle of the display backend. Can be safely cast into 
 *  (struct hadal_interface *) if obtained from a valid display implementation. */
struct hadal;

/** Information needed to construct a display backend. */
struct hadal_create_info {
    struct riven_create_info_header header;

    u32         width, height;
    struct str  title;
};

#if defined(PLATFORM_WINDOWS)
/** The entry point to a Windows display backend. */
AMWAPI RIVEN_ENCORE(hadal, win32);
#elif defined(PLATFORM_APPLE_MACOS)
/** The entry point to an Apple MacOS Cocoa display backend. */
AMWAPI RIVEN_ENCORE(hadal, cocoa);
#elif defined(PLATFORM_APPLE_IOS)
/** The entry point to an Apple iOS display backend. */
AMWAPI RIVEN_ENCORE(hadal, ios);
#elif defined(PLATFORM_ANDROID)
/** The entry point to an Android display backend, implementing an activity. */
AMWAPI RIVEN_ENCORE(hadal, android);
#elif defined(PLATFORM_EMSCRIPTEN)
/** The entry point to a WebAssembly/Emscripten HTML5 display backend. */
AMWAPI RIVEN_ENCORE(hadal, html5);
#endif
#ifdef HADAL_WAYLAND
/** The entry point to a Linux Wayland display backend. */
AMWAPI RIVEN_ENCORE(hadal, wayland);
#endif
#ifdef HADAL_XCB
/** The entry point to a Linux X11 XCB display backend. */
AMWAPI RIVEN_ENCORE(hadal, xcb);
#endif
#ifdef HADAL_DRMKMS
/** The entry point to a Linux DRM/KMS display backend. */
AMWAPI RIVEN_ENCORE(hadal, drmkms);
#endif

/** The entry point to a headless display backend, available on all platforms.
 *  A swapchain may draw into a headless surface if it is supported by the GPU driver. */
AMWAPI RIVEN_ENCORE(hadal, headless);

/** Executes native encores until one writes to the interface:
 *  - win32, cocoa, ios, android, html5 - native builds.
 *  - wayland, xcb - Linux builds.
 *  - drkms - available on Linux and Android builds.
 *
 *  Available encores are tried in order:
 *      native builds -> wayland -> xcb -> drmkms -> headless.
 *
 *  If all options fail, the headless interface may be written as a fallback.
 *  This may be disabled by defining HADAL_DISABLE_HEADLESS_FALLBACK.
 *
 *  Other possible display backends that could be created:
 *      XBox, PlayStation 5, Nintendo Switch, XR/VR? */
AMWAPI RIVEN_ENCORE(hadal, native);

/** Acquires the framebuffer extent/resolution, may be different from the window size. */
typedef void (AMWCALL *PFN_acquire_framebuffer_extent)(struct hadal *hadal, u32 *width, u32 *height);

#ifdef PELAGIA_VULKAN
/* to avoid including the Vulkan header */
struct VkInstance_T;
struct VkSurface_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
typedef void (*(*PFN_vkGetInstanceProcAddr)(struct VkInstance_T *, const char *))(void);

/** Creates a Vulkan surface from a given window. The display backend must support 
 *  interfacing with Vulkan (then for example, an Emscripten/HTML5 backend can't).
 *  The caller must pass a valid Vulkan instance, a PFN_vkGetInstanceProcAddr and 
 *  a pointer to the surface handle, where the Vulkan surface will be created.
 *  An optional pointer to the allocation callbacks can be given too.
 *
 *  Returns a non-zero value on errors, or if the display does not support Vulkan. */
typedef s32 (AMWCALL *PFN_vulkan_create_surface)(
    const struct hadal                 *hadal,
    struct VkInstance_T                *instance,
    struct VkSurface_T                **out_surface,
    const struct VkAllocationCallbacks *callbacks,
    void *(*vkGetInstanceProcAddr)(struct VkInstance_T *, const char *),
    void                               *vulkan_module);

/** Checks whether the physical device supports presentation for a given display backend.
 *  If a display backend has no dedicated vkGetPhysicalDevice*PresentationSupport procedure,
 *  then a queue family's presentation support is checked via the Vulkan surface instead.
 *
 *  Returns true if presentation is supported, otherwise (or on errors) returns false. */
typedef b32 (AMWCALL *PFN_vulkan_physical_device_presentation_support)(
    const struct hadal                 *hadal,
    struct VkInstance_T                *instance,
    struct VkSurface_T                 *surface,
    u32                                 queue_family,
    void *(*vkGetInstanceProcAddr)(struct VkInstance_T *, const char *),
    void                               *vulkan_module);
#endif

/** Procedures to be provided by an implementation. The backend must implement the 'struct hadal' and 
 *  put the hadal_interface as the first member of this structure, to allow casting between the opaque 
 *  handle of the display backend and the public interface defined below. */
struct hadal_interface {
    struct riven_interface_header   header;

    PFN_acquire_framebuffer_extent  acquire_framebuffer_extent;

    /* swapchain stuff */
#ifdef PELAGIA_VULKAN
    PFN_vulkan_create_surface                       vulkan_create_surface;
    PFN_vulkan_physical_device_presentation_support vulkan_physical_device_presentation_support;
#endif
};

/** Acquires the framebuffer extent/resolution, may be different from the window size. */
attr_inline attr_nonnull(1)
void hadal_acquire_framebuffer_extent(
    struct hadal *hadal,
    u32          *out_width, 
    u32          *out_height)
{
    struct hadal_interface *interface = (struct hadal_interface *)hadal;
    u32 width = 0, height = 0;

    interface->acquire_framebuffer_extent(hadal, &width, &height);
    if (out_width) *out_width = width;
    if (out_height) *out_height = height;
}

#ifdef __cplusplus
}
#endif
