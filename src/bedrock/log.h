#pragma once

#include "defines.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LAKE_SRC_FILENAME
    #ifdef PLATFORM_WINDOWS
        #define _LAKE_SRC_FILENAME __FILE__ /* don't include string.h */
    #else
        #define _LAKE_SRC_FILENAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
    #endif
#endif

enum log_level {
    log_level_debug,
    log_level_info,
    log_level_warn,
    log_level_error,
    log_level_fatal,
};

/** Logs a message to the default sink (XXX for now, the standard console output). */
LAKEAPI void bedrock_log(
    s32         log_level, 
    const char *func, 
    const char *filename, 
    s32         line, 
    const char *fmt, ...) 
    attr_printf(5,6);

/** Sets a minimal log severity level. */
LAKEAPI void log_set_level(s32 log_level);

/** If set to false, only the fmt will be logged (without time, file or line info). */
LAKEAPI void log_set_verbose(b32 verbose);

/** Used to disable all log output at runtime. */
LAKEAPI void log_set_quiet(b32 quiet);

#define log_debug(fmt, ...) bedrock_log(log_level_debug, __func__, _LAKE_SRC_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  bedrock_log(log_level_info,  __func__, _LAKE_SRC_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  bedrock_log(log_level_warn,  __func__, _LAKE_SRC_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) bedrock_log(log_level_error, __func__, _LAKE_SRC_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) bedrock_log(log_level_fatal, __func__, _LAKE_SRC_FILENAME, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
