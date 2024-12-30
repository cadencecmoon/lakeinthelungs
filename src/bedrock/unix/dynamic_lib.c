#include <lake/bedrock/log.h>
#include <lake/hadopelagic.h>

#include <dlfcn.h>

AMWAPI void * AMWAPIENTRY 
hadal_load_dll(const char *libname)
{
    void *handle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        log_error("dlopen '%s' failed: %s", 
                libname ? libname : "null", 
                libname ? dlerror() : "a NULL libname was passed to hadal_load_dll()");
    }
    return handle;
}

AMWAPI void AMWAPIENTRY 
hadal_close_dll(void *handle)
{
    if (handle)
        dlclose(handle);
}

AMWAPI void * AMWAPIENTRY 
hadal_get_proc_address(void *handle, const char *procname)
{
    assert_debug(handle && procname);

    const char *err;
    void *addr = dlsym(handle, procname);
    if ((err = dlerror()) != NULL) {
        log_error("hadal_get_proc_address: dlsym '%s' failed: %s", 
                procname ? procname : "null", 
                procname ? err : "a NULL procname was passed to hadal_get_proc_address()");
    }
    return addr;
}
