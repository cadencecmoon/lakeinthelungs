#pragma once

#include <amw/bedrock.h>

/** Load the dynamic library into memory. */
AMWAPI void *plugin_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void plugin_close_dll(void *module);

/** Get a function address from a dynamic library. */
AMWAPI attr_hot attr_nonnull_all
void *plugin_get_proc_address(void *module, const char *procedure);
