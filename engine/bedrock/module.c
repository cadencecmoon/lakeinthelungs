#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
    #include <dlfcn.h>
#elif defined(LAKE_PLATFORM_WINDOWS)
    #include <amwe/arch/windows.h>
#endif /* LAKE_PLATFORM_UNIX */

void *bedrock_module_open(const char *libname)
{
    bedrock_assert_debug(libname);

#if defined(LAKE_PLATFORM_UNIX)
    void *module = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!module) 
        bedrock_log_error("dlopen '%s' failed: %s.", libname, dlerror());
    return module;
#elif defined(LAKE_PLATFORM_WINDOWS)
    HMODULE *module = LoadLibraryA(libname);
    if (!module)
        bedrock_log_error("LoadLibraryA '%s' failed (error %lu).", libname, GetLastError());
    return (void *)module;
#endif /* LAKE_PLATFORM_UNIX */
}

void bedrock_module_close(void *module)
{
    bedrock_assert_debug(module);

#if defined(LAKE_PLATFORM_UNIX)
    dlclose(module);
#elif defined(LAKE_PLATFORM_WINDOWS)
    FreeLibrary((HMODULE)module);
#endif /* LAKE_PLATFORM_UNIX */
}

void *bedrock_get_proc_address(void *module, const char *procedure)
{
    bedrock_assert_debug(module);

#if defined(LAKE_PLATFORM_UNIX)
    #ifndef LAKE_NDEBUG
    const char *err;
    void *addr = dlsym(module, procedure);
    if ((err = dlerror()) != NULL)
        bedrock_log_error("dlsym '%s' failed: %s.", procedure, err);
    return addr;
    #else
    return dlsym(module, procedure);
    #endif
#elif defined(LAKE_PLATFORM_WINDOWS)
    GetProcAddress((HMODULE)module, procedure);
#endif /* LAKE_PLATFORM_UNIX */
}
