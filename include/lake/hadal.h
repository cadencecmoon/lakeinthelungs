#ifndef _AMW_HADAL_H
#define _AMW_HADAL_H

#include <lake/defines.h>

/** @file hadal.h
 *
 *  Hadal, or hadopelagic, is the deepest region of the ocean, lying within oceanic trenches. 
 *  In this engine, it serves as a direct abstraction to the operating system resources
 *  and the display backend, like windowing or handling input devices.
 *
 *  The hadal struct holds the opaque context state of a display backend. Functions that don't accept
 *  a pointer to this context are considered stateless, or have their internal state hidden. */
struct hadal;

/** Load the dynamic library into memory. */
AMWAPI void *hadal_dll_load(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void hadal_dll_close(void *module);

/** Get a function address from a dynamic library. */
AMWAPI void *hadal_dll_proc_address(void *module, const char *procedure);

/** Returns the system timer counter. */
AMWAPI uint64_t hadal_timer_counter(void);

/** Returns the frequency of the system timer. */
AMWAPI uint64_t hadal_timer_frequency(void);

AMWAPI void hadal_cpu_count(size_t *threads, size_t *cores, size_t *packages);

/** Specifies an ID for a display backend. Not all display backends are 
 *  available, they are different on every platform. */
enum hadal_api {
    hadal_api_auto = 0u,
    hadal_api_win32,
    hadal_api_cocoa,
    hadal_api_ios,
    hadal_api_android,
    hadal_api_wayland,
    hadal_api_xcb,
    hadal_api_kms,
    hadal_api_headless,
    hadal_api_count,
};

#endif /* _AMW_HADAL_H */
