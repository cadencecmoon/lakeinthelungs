#ifndef _AMW_LOG_H
#define _AMW_LOG_H

#include <lake/defines.h>

#ifdef AMW_CC_GNUC_VERSION
    #define AMW_PRINTF_FORMAT(fmt,va) __attribute__((format(__printf__, fmt, va)))
#else
    #define AMW_PRINTF_FORMAT(fmt,va)
#endif

enum log_level {
    log_level_debug,
    log_level_info,
    log_level_warn,
    log_level_error,
    log_level_fatal,
};

#ifndef AMW_LOG_FILE_NAME
    #ifdef AMW_PLATFORM_WINDOWS
        #include <string.h>
        #define AMW_LOG_FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
    #else
        #define AMW_LOG_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
    #endif
#endif

/** Logs a message to the standard console output. TODO Files not supported rn. */
AMWAPI void log_message(
        enum log_level level, 
        const char *filename, 
        int32_t line, 
        const char *fmt, 
        ...) AMW_PRINTF_FORMAT(4,5);

/** Sets a minimal log severity level. */
AMWAPI void log_set_level(enum log_level level);

/** If set to false, only the fmt will be logged (without time, file or line info). */
AMWAPI void log_set_verbose(bool verbose);

/** Used to disable all log output at runtime. */
AMWAPI void log_set_quiet(bool quiet);

#ifndef AMW_LOG_DISABLE_MACROS
#define log_debug(fmt, ...) log_message(log_level_debug, AMW_LOG_FILE_NAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  log_message(log_level_info,  AMW_LOG_FILE_NAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  log_message(log_level_warn,  AMW_LOG_FILE_NAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_message(log_level_error, AMW_LOG_FILE_NAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) log_message(log_level_fatal, AMW_LOG_FILE_NAME, __LINE__, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#define log_info(fmt, ...)
#define log_warn(fmt, ...)
#define log_error(fmt, ...)
#define log_fatal(fmt, ...)
#endif

#endif /* _AMW_LOG_H */
