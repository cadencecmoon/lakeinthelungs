#pragma once

#include "defines.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
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
        #define UNREACHABLE _lake_assertion(!"Unreachable code branch!");
    #endif
#endif

#if !defined(ASSUME)
    #if HAS_BUILTIN(__builtin_assume)
        #define ASSUME(x) __builtin_assume(x)
    #elif defined(CC_MSVC_VERSION)
        #define ASSUME(x) __assume(x)
    #else
        #define ASSUME(X) do { if(!(x)) { UNREACHABLE; } } while(0)
    #endif
#endif

/* static assertion */
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || CC_GNUC_VERSION_CHECK(4,6,0)
    #define static_assertion(cond)          _Static_assert(cond, #cond)
    #define static_assertion_msg(cond, msg) _Static_assert(cond, msg)
#else
    #define static_assertion(cond)
    #define static_assertion_msg(cond, msg)
#endif

/* the implementation of an assertion */
#define _bedrock_assertion(cond, msg) \
    if (UNLIKELY(!(cond))) { \
        log_fatal("Assertion '%s' failed%s", #cond, (msg) ? msg : "."); \
        debugtrap(); \
    }
#ifdef DEBUG
    #define _bedrock_assume(cond, msg) _bedrock_assertion(cond, msg)
#else
    #define _bedrock_assume(cond, msg) ASSUME(cond)
#endif

#define _bedrock_assert_1(cond)         _bedrock_assertion(cond, NULL) 
#define _bedrock_assert_2(cond, msg)    _bedrock_assertion(cond, msg) 
#define _bedrock_assume_1(cond)         _bedrock_assume(cond, NULL)
#define _bedrock_assume_2(cond, msg)    _bedrock_assume(cond, msg)

#if defined(DEBUG) && !defined(NDEBUG)
    #define assert_release(cond, ...) \
        MACROMAGIC_OVERLOAD_NARGS(_bedrock_assert_, __VA_ARGS__)(cond, ##__VA_ARGS__)
    #define assert_debug(cond, ...) \
        MACROMAGIC_OVERLOAD_NARGS(_bedrock_assert_, __VA_ARGS__)(cond, ##__VA_ARGS__)
    #define assume(cond, ...) \
        MACROMAGIC_OVERLOAD_NARGS(_bedrock_assume_, __VA_ARGS__)(cond, ##__VA_ARGS__)
#elif !defined(ASSERTIONS_DISABLED)
    #define assert_release(cond, ...) \
        MACROMAGIC_OVERLOAD_NARGS(_bedrock_assert_, __VA_ARGS__)(cond, ##__VA_ARGS__)
    #define assert_debug(cond, ...) ((void)0)
    #define assume(cond, ...) \
        MACROMAGIC_OVERLOAD_NARGS(_bedrock_assume_, __VA_ARGS__)(cond, ##__VA_ARGS__)
#else /* the compiler will optimize them out */
    #define assert_release(cond, ...) ((void)0)
    #define assert_debug(cond, ...) ((void)0)
    #define assume(cond, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif
