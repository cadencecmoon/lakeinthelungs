/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

/** 
 * @file amw/common/assert.h
 * @brief Assertions for validating state.
 */

#ifndef _AMW_COMMON_ASSERT_H
#define _AMW_COMMON_ASSERT_H

#include <amw/common/defines.h>
#include <amw/common/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AMW_NDEBUG)
    #define debugtrap() /* nothing */
#elif AMW_HAS_BUILTIN(__builtin_debugtrap)
    #define debugtrap() __builtin_debugtrap()
#elif AMW_HAS_BUILTIN(__debugbreak)
    #define debugtrap() __debugbreak()
#elif defined(AMW_CC_MSVC_VERSION)
    #define debugtrap() __debugbreak()
#elif defined(AMW_CC_ARM_VERSION)
    #define debugtrap() __breakpoint(42)
#elif defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
    AMW_INLINE void debugtrap(void) { __asm__ __volatile__("int $03"); }
#elif defined(__thumb__) /* arm32 thumb */
    AMW_INLINE void debugtrap(void) { __asm__ __volatile__(".inst 0xde01"); }
#elif defined(AMW_ARCH_AARCH64)
    AMW_INLINE void debugtrap(void) { __asm__ __volatile__(".inst 0xd4200000"); }
#elif defined(AMW_ARCH_ARM)
    AMW_INLINE void debugtrap(void) { __asm__ __volatile__(".inst 0xe7f001f0"); }
#elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(AMW_CC_GNUC_VERSION)
    #define debugtrap() __builtin_trap()
#else
    #include <signal.h>
    #if defined(SIGTRAP)
        #define debugtrap() raise(SIGTRAP)
    #else
        #define debugtrap() raise(SIGABRT)
    #endif
#endif

/* static_assert */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define static_assertion(x,desc) static_assert(x,desc)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define static_assertion(x,desc) _Static_assert(x,desc)
/* GCC 4.6 or later */
#elif AMW_CC_GNUC_VERSION_CHECK(4,6,0)
    /* It will work but it may throw a pedantic warning:
     * warning: ISO C99 does not support '_Static_assert' [-Wpedantic] */
    #define static_assertion(x,desc) _Static_assert(x,desc)
#endif
#ifndef static_assertion
    #define static_assertion(x,desc)
#endif

#ifndef AMW_ASSERT_LEVEL
    #if (defined(AMW_DEBUG) || defined(_DEBUG) || defined(DEBUG)) && !defined(AMW_NDEBUG)
        #define AMW_ASSERT_LEVEL 2
    #else
        #define AMW_ASSERT_LEVEL 1
    #endif
#endif

#define _AMW_DISABLED_ASSERT(cond) ((void)0)
#define _AMW_ENABLED_ASSERT(cond)                       \
    do {                                                \
        if (!(cond)) {                                  \
            log_fatal("Assertion '%s' failed.", #cond); \
            debugtrap();                                \
        }                                               \
    } while (false)

/** This assert is never disabled at any level. */
#define assertion(cond) _AMW_ENABLED_ASSERT(cond)

#if AMW_ASSERT_LEVEL == 1  /* release settings. */
    #define assert_debug(cond)    _AMW_DISABLED_ASSERT(cond)
    #define assert_release(cond)  _AMW_ENABLED_ASSERT(cond)
    #define assert_paranoid(cond) _AMW_DISABLED_ASSERT(cond)
#elif AMW_ASSERT_LEVEL == 2  /* debug settings. */
    #define assert_debug(cond)    _AMW_ENABLED_ASSERT(cond)
    #define assert_release(cond)  _AMW_ENABLED_ASSERT(cond)
    #define assert_paranoid(cond) _AMW_DISABLED_ASSERT(cond)
#elif AMW_ASSERT_LEVEL == 3  /* paranoid settings. */
    #define assert_debug(cond)    _AMW_ENABLED_ASSERT(cond)
    #define assert_release(cond)  _AMW_ENABLED_ASSERT(cond)
    #define assert_paranoid(cond) _AMW_ENABLED_ASSERT(cond)
#else /* assertions disabled */
    #define assert_debug(cond)    _AMW_DISABLED_ASSERT(cond)
    #define assert_release(cond)  _AMW_DISABLED_ASSERT(cond)
    #define assert_paranoid(cond) _AMW_DISABLED_ASSERT(cond)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AMW_COMMON_ASSERT_H */
