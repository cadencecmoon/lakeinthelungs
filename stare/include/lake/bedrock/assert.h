/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_ASSERT_H
#define _AMW_ASSERT_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/log.h>

#ifdef __cplusplus
extern "C" {
#endif

/* debugtrap */
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
    AMW_INLINE void debugtrap(void) { __asm__ volatile("int $03"); }
#elif defined(__thumb__) /* arm32 thumb */
    AMW_INLINE void debugtrap(void) { __asm__ volatile(".inst 0xde01"); }
#elif defined(AMW_ARCH_AARCH64)
    AMW_INLINE void debugtrap(void) { __asm__ volatile(".inst 0xd4200000"); }
#elif defined(AMW_ARCH_ARM)
    AMW_INLINE void debugtrap(void) { __asm__ volatile(".inst 0xe7f001f0"); }
#elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(AMW_CC_GNUC_VERSION)
    #define debugtrap() __builtin_trap()
#endif

/* static assertion */
#ifndef static_assertion
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
        #define static_assertion(x,desc) static_assert(x,desc)
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
        #define static_assertion(x,desc) _Static_assert(x,desc)
    /* GCC 4.6 or later */
    #elif AMW_CC_GNUC_VERSION_CHECK(4,6,0)
        /* It will work but it may throw a pedantic warning:
         * warning: ISO C99 does not support '_Static_assert' [-Wpedantic] */
        #define static_assertion(x,desc) _Static_assert(x,desc)
    #else
        #define static_assertion(x,desc)
    #endif
#endif

/* this assertion is always enabled */
#define assertion(cond) \
    do { \
        if (!(cond)) { \
            log_fatal("Assertion failed %s", #cond); \
            debugtrap(); \
        } \
    } while (false)

/* debug assertions */
#if defined(AMW_DEBUG) && !defined(AMW_NDEBUG)
    #define assert_release(cond) assertion(cond) 
    #define assert_debug(cond)   assertion(cond)
#elif !defined(AMW_ASSERTION_DISABLED)
    #define assert_release(cond) assertion(cond) 
    #define assert_debug(cond)   ((void)0)
#else /* the compiler will optimize them out */
    #define assert_release(cond) ((void)0)
    #define assert_debug(cond)   ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ASSERT_H */
