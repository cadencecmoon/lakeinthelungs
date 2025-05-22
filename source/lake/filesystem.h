#pragma once

/** @file lake/filesystem.h 
 *  @brief TODO docs filesystem
 */
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Returns a saved path to the executable file of an application. */
LAKEAPI char const *LAKECALL 
lake_fs_executable_path(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
