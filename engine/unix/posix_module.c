#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
#include <dlfcn.h>

void *bedrock_module_open(const char *libname)
{
    bedrock_assert_debug(libname);
    void *module = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!module) 
        bedrock_log_error("dlopen '%s' failed: %s.", libname, dlerror());
    return module;
}

void bedrock_module_close(void *module)
{
    bedrock_assert_debug(module);
    dlclose(module);
}

void *bedrock_get_proc_address(void *module, const char *procedure)
{
    bedrock_assert_debug(module);

#ifndef LAKE_NDEBUG
    const char *err;
    void *addr = dlsym(module, procedure);
    if ((err = dlerror()) != NULL)
        bedrock_log_error("dlsym '%s' failed: %s.", procedure, err);
    return addr;
#else
    return dlsym(module, procedure);
#endif
}
#endif /* LAKE_PLATFORM_UNIX */
