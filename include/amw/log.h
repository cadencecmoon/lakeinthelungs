#pragma once

#include <amw/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __STDC_VERSION__ >= 202000
    /* static_assert is already defined */
#else
    #define static_assert _Static_assert
#endif

/* debugtrap */
#if defined(NDEBUG)
    #define debugtrap() /* nothing for now */
#elif HAS_BUILTIN(__builtin_debugtrap)
    #define debugtrap() __builtin_debugtrap()
#elif HAS_BUILTIN(__debugbreak) || defined(CC_MSVC_VERSION)
    #define debugtrap() __debugbreak()
#elif defined(CC_ARM_VERSION)
    #define debugtrap() __breakpoint(42)
#else
    #define debugtrap() __builtin_trap()
#endif

/* unreachable */
#if !defined(UNREACHABLE)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define UNREACHABLE __builtin_unreachable()
    #elif defined(CC_MSVC_VERSION)
        #define UNREACHABLE __assume(false)
    #else
        #define UNREACHABLE assertion(!"Unreachable code branch!");
    #endif
#endif

/* assumption */
#if !defined(ASSUME)
    #if HAS_BUILTIN(__builtin_assume)
        #define ASSUME(x) __builtin_assume(x)
    #elif defined(CC_MSVC_VERSION)
        #define ASSUME(x) __assume(x)
    #else
        #define ASSUME(X) do { if(!(x)) { UNREACHABLE; } } while(0)
    #endif
#endif

#define assertion(cond) \
    if (UNLIKELY(!(cond))) { \
        log_fatal("Assertion '%s' failed.", #cond); \
        debugtrap(); \
    }

#if defined(DEBUG) && !defined(NDEBUG)
    #define assert_debug(cond)   assertion(cond)
    #define assert_release(cond) assertion(cond)
    #define assume(cond)         assertion(cond)
#elif !defined(DISABLE_ASSERT)
    #define assert_debug(cond)   ((void)0)
    #define assert_release(cond) assertion(cond)
    #define assume(cond)         ASSUME(cond)
#else
    #define assert_debug(cond)   ((void)0)
    #define assert_release(cond) ((void)0)
    #define assume(cond)         ((void)0)
#endif

/* file name */
#ifndef SOURCE_FILENAME
    #ifdef PLATFORM_WINDOWS
        #define SOURCE_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
    #else
        #define SOURCE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
    #endif
#endif

enum log_level {
    log_level_verbose = 0,
    log_level_debug,
    log_level_info,
    log_level_warn,
    log_level_error,
    log_level_fatal,
};

/** Logs a message to console. For now no other output sink is available. */
AMWAPI attr_printf(5,6)
void log_message(
    s32         log_level,
    const char *func,
    const char *filename,
    s32         line,
    const char *fmt, ...);

/** Overwrite log severity level. */
AMWAPI void log_set_level(s32 set_log_level);

/** Overwrite details in the log message. */
AMWAPI void log_set_verbose(b32 set_verbose);

/** Disable or enable log output. */
AMWAPI void log_set_quiet(b32 set_quiet);

#define log_verbose(fmt, ...)   log_message(log_level_verbose, __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)     log_message(log_level_debug,   __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)      log_message(log_level_info,    __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)      log_message(log_level_warn,    __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)     log_message(log_level_error,   __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)     log_message(log_level_fatal,   __func__, SOURCE_FILENAME, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
