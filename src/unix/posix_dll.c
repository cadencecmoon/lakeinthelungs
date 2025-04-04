#ifdef PLATFORM_UNIX
#include <amw/bedrock/log.h>
#include <amw/bedrock/process.h>

#include <dlfcn.h>

void *process_load_dll(const char *libname)
{
    if (!libname) return NULL;

    void *module = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!module)
        log_error("dlopen '%s' failed: %s.", libname, dlerror());
    return module;
}

void process_close_dll(void *module)
{
    if (module) dlclose(module);
}

void *process_get_address(void *module, const char *procedure)
{
    const char *err;
    void *addr = dlsym(module, procedure);
    if ((err = dlerror()) != NULL)
        log_error("dlsym '%s' failed: %s.", procedure, err);
    return addr;
}

#endif /* PLATFORM_UNIX */
