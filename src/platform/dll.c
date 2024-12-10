/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/common/assert.h>
#include <amw/hadal.h>

#if defined(AMW_PLATFORM_WINDOWS)
#elif defined(AMW_PLATFORM_UNIX)
#include <dlfcn.h>
#else
    #error Define your platform.
#endif

void *hadal_load_dll(const char *libname)
{
#if defined(AMW_PLATFORM_WINDOWS)
#else
    void *handle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        log_error("dlopen '%s' failed: %s", 
                libname ? libname : "null", 
                libname ? dlerror() : "a NULL libname was passed to hadal_load_dll()");
    }
    return handle;
#endif
}

void hadal_close_dll(void *handle)
{
#if defined(AMW_PLATFORM_WINDOWS)
#else
    if (handle)
        dlclose(handle);
#endif
}

void *hadal_get_proc_address(void *handle, const char *procname)
{
    assert_paranoid(handle && procname);

#if defined(AMW_PLATFORM_WINDOWS)
#else
    const char *err;
    void *addr = dlsym(handle, procname);
    if ((err = dlerror()) != NULL) {
        log_error("hadal_get_proc_address: dlsym '%s' failed: %s", 
                procname ? procname : "null", 
                procname ? err : "a NULL procname was passed to hadal_get_proc_address()");
    }
    return addr;
#endif
}
