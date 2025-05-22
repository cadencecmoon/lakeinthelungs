#include "../sys.h"
#include "../log.h"

#if defined(LAKE_PLATFORM_UNIX)
#include <dlfcn.h>

void *lake_sys_open_library(char const *libname)
{
    void *module = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
#ifndef LAKE_NDEBUG
    if (!module) 
        lake_error("dlopen '%s' failed: %s.", libname, dlerror());
#endif
    return module;
}

void lake_sys_close_library(void *module)
{
    dlclose(module);
}

void *lake_sys_proc_addr(void *module, char const *procname)
{
#ifndef LAKE_NDEBUG
    char const *err;
    void *addr = dlsym(module, procname);
    if ((err = dlerror()) != nullptr)
        lake_error("dlsym '%s' failed: %s.", procname, err);
    return addr;
#else
    return dlsym(module, procname);
#endif
}
#endif /* LAKE_PLATFORM_UNIX */
