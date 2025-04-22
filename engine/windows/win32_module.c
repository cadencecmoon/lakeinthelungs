#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>

void *bedrock_module_open(const char *libname)
{
    bedrock_assert_debug(libname);
    HMODULE *module = LoadLibraryA(libname);
    if (!module)
        bedrock_log_error("LoadLibraryA '%s' failed (error %lu).", libname, GetLastError());
    return (void *)module;
}

void bedrock_module_close(void *module)
{
    bedrock_assert_debug(module);
    FreeLibrary((HMODULE)module);
}

void *bedrock_get_proc_address(void *module, const char *procedure)
{
    bedrock_assert_debug(module);
    GetProcAddress((HMODULE)module, procedure);
}
#endif /* LAKE_PLATFORM_WINDOWS */
