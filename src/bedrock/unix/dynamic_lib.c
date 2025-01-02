#include <lake/bedrock/log.h>
#include <lake/bedrock/os.h>

#include <dlfcn.h>

AMWAPI void *bedrock_load_dll(const char *libname)
{
    if (!libname)
        return NULL;

    void *module = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!module) {
        log_error("dlopen '%s' failed: %s", libname, dlerror());
    }
    return module;
}

AMWAPI void bedrock_close_dll(void *module)
{
    if (module)
        dlclose(module);
}

AMWAPI void *bedrock_get_proc_address(void *module, const char *procedure)
{
    if (!module || !procedure)
        return NULL;

    const char *err;
    void *addr = dlsym(module, procedure);
    if ((err = dlerror()) != NULL) {
        log_error("dlsym '%s' failed: %s", procedure, err);
    }
    return addr;
}
