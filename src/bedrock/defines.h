#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION_NUM_MAJOR(version) ((version) / 1000000)
#define VERSION_NUM_MINOR(version) (((version) / 1000) % 1000)
#define VERSION_NUM_REVISION(version) ((version) % 1000)

#define VERSION_NUM(major, minor, revision) \
    ((major) * 1000000 + (minor) * 1000 + (revision))

#define _CONCAT_VERSION_STR__(major, minor, revision) #major "." #minor "." #revision
#define VERSION_MAKE_STR(major, minor, revision) _CONCAT_VERSION_STR__(major, minor, revision)

/* detect platform */
#if !defined(PLATFORM_WINDOWS)
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
        #define PLATFORM_WINDOWS 1
    #endif
#endif
#if !defined(PLATFORM_UNIX)
    #if defined(__unix__) || defined(__unix) || defined(unix)
        #define PLATFORM_UNIX 1
    #endif
#endif
#if !defined(PLATFORM_APPLE)
    #if defined(__APPLE__)
        #define PLATFORM_APPLE 1
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            #define PLATFORM_IOS 1
        #else
            #define PLATFORM_MACOS 1
        #endif
    #endif
#endif
#if !defined(PLATFORM_LINUX)
    #if defined(__linux__) || defined(__gnu_linux__)
        #define PLATFORM_LINUX 1
    #endif
#endif
#if !defined(PLATFORM_ANDROID)
    #if defined(__ANDROID__)
        #define PLATFORM_ANDROID 1
    #endif
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC__MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(CC_GNUC_VERSION)
    #define CC_GNUC_VERSION_CHECK(major, minor, revision) (CC_GNUC_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define CC_GNUC_VERSION_CHECK(major, minor, revision) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define CC_MSVC_VERSION VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(CC_MSVC_VERSION)
    #define CC_MSVC_VERSION_CHECK(major, minor, revision) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (revision)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (revision)))
#else
    #define CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_VER >= ((major * 100) + (minor)))
#endif

/* MINGW */
#if defined(__MINGW64__)
    #define CC_MINGW_VERSION VERSION_NUM(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, __MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32__)
    #define CC_MINGW_VERSION VERSION_NUM(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#endif
#if defined(CC_MINGW_VERSION)
    #define CC_MINGW_VERSION_CHECK(major, minor, revision) (CC_MINGW_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define CC_MINGW_VERSION_CHECK(major, minor, revision) (0)
#endif

/* ARMC */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(CC_ARM_VERSION)
    #define CC_ARM_VERSION_CHECK(major, minor, revision) (CC_ARM_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define CC_ARM_VERSION_CHECK(major, minor, revision) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(CC_CLANG_VERSION)
    #if __has_warning("-Wmissing-designated-field-initializers")
        #define CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define CC_CLANG_VERSION 100000
    #elif defined(__FILE_NAME__)
        #define CC_CLANG_VERSION 90000
    #elif __has_warning("-Wextra-semi-stmt") || __has_builtin(__builtin_rotateleft32)
        #define CC_CLANG_VERSION 80000
    /* For reasons unknown, Xcode 10.3 (Apple LLVM version 10.0.1) is apparently
     * based on Clang 7, but does not support the warning we test.
     * See https://en.wikipedia.org/wiki/Xcode#Toolchain_versions and
     * https://trac.macports.org/wiki/XcodeVersionInfo. */
    #elif __has_warning("-Wc++98-compat-extra-semi") || \
        (defined(__apple_build_version__) && __apple_build_version__ >= 10010000)
        #define CC_CLANG_VERSION 70000
    #elif __has_warning("-Wpragma-pack")
        #define CC_CLANG_VERSION 60000
    #elif __has_warning("-Wbitfield-enum-conversion")
        #define CC_CLANG_VERSION 50000
    #elif __has_attribute(diagnose_if)
        #define CC_CLANG_VERSION 40000
    #elif __has_warning("-Wcomma")
        #define CC_CLANG_VERSION 39000
    #elif __has_warning("-Wdouble-promotion")
        #define CC_CLANG_VERSION 38000
    #elif __has_warning("-Wshift-negative-value")
        #define CC_CLANG_VERSION 37000
    #elif __has_warning("-Wambiguous-ellipsis")
        #define CC_CLANG_VERSION 36000
    #else
        #define CC_CLANG_VERSION 1
    #endif
#endif
#if defined(CC_CLANG_VERSION)
    #define CC_CLANG_VERSION_CHECK(major, minor, revision) (CC_CLANG_VERSION >= ((major * 10000) + (minor * 1000) + (revision)))
#else
    #define CC_CLANG_VERSION_CHECK(major, minor, revision) (0)
#endif

/* check compiler extensions */
#ifdef __has_attribute
    #define HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define HAS_ATTRIBUTE(x) (0)
#endif
#ifdef __has_builtin
    #define HAS_BUILTIN(x) __has_builtin(x)
#else
    #define HAS_BUILTIN(x) (0)
#endif
#ifdef __has_feature
    #define HAS_FEATURE(x) __has_feature(x)
#else
    #define HAS_FEATURE(x) (0)
#endif
#ifdef __has_warning
    #define HAS_WARNING(x) __has_warning(x)
#else
    #define HAS_WARNING(x) (0)
#endif

/* inline */
#if !defined(INLINE) && !defined(NOINLINE)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define INLINE __attribute__((always_inline)) inline
        #define NOINLINE __attribute__((noinline))
    #elif defined(CC_MSVC_VERSION)
        #define INLINE __forceinline
        #define NOINLINE __declspec(noinline)
    #else
        #define INLINE static inline
        #define NOINLINE
    #endif
#endif

/* noreturn */
#if !defined(NORETURN)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define NORETURN __attribute__((noreturn))
    #elif defined(CC_MSVC_VERSION)
        #define NORETURN __declspec(noreturn)
    #else
        #define NORETURN
    #endif
#endif

/* likely and unlikely expect */
#if !defined(LIKELY) && !defined(UNLIKELY)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define LIKELY(x)   __builtin_expect(!!(x), 1)
        #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else
        #define LIKELY(x)   (x)
        #define UNLIKELY(x) (x)
    #endif
#endif

/* typeof */
#if !defined(TYPEOF)
    #if defined(CC_MSVC_VERSION)
        #define TYPEOF(x) decltype((x))
    #elif defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define TYPEOF(x) __typeof__((x)) 
    #else /* may generate errors depending on the compiler */
        #define TYPEOF(x) typeof((x))
    #endif
#endif

/* api */
#if !defined(LAKEAPI)
    #ifdef BUILD_DLL_EXPORT
        #if defined(PLATFORM_WINDOWS) || defined(__CYGWIN__)
            #define LAKEAPI extern __declspec(dllexport)
        #elif defined(CC_GNUC_VERSION)
            #define LAKEAPI extern __attribute__((visibility("default")))
        #else
            #define LAKEAPI extern
        #endif
    #else
        #define LAKEAPI extern
    #endif
#endif

#define MS_PER_SEC      1000
#define US_PER_SEC      1000000
#define NS_PER_SEC      1000000000LL
#define NS_PER_MS       1000000
#define NS_PER_US       1000
#define SEC_TO_NS(S)    (((u64)(S)) * NS_PER_SEC)
#define NS_TO_SEC(NS)   ((NS) / NS_PER_SEC)
#define MS_TO_NS(MS)    (((u64)(MS)) * NS_PER_MS)
#define NS_TO_MS(NS)    ((NS) / NS_PER_MS)
#define US_TO_NS(US)    (((u64)(US)) * NS_PER_US)
#define NS_TO_US(NS)    ((NS) / NS_PER_US)

#ifndef min
#define min(x,y)        (((x) < (y)) ? (x) : (y))
#endif
#ifndef max
#define max(x,y)        (((x) > (y)) ? (x) : (y))
#endif
#ifndef clamp
#define clamp(x,a,b)    (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#endif
#ifndef clamp_zo
#define clamp_zo(x)     (clamp(x,0,1))
#endif
#ifndef arraysize
#define arraysize(a)    (sizeof(a) / sizeof(a[0]))
#endif
#ifndef lengthof
#define lengthof(s)     (arraysize(s) - 1)
#endif

#define MACROMAGIC_FIRST_(f, ...) f
#define MACROMAGIC_FIRST(...) MACROMAGIC_FIRST_(__VA_ARGS__, _)
#define MACROMAGIC_EMPTY()
#define MACROMAGIC_DEFER(id) id MACROMAGIC_EMPTY()
#define MACROMAGIC_OBSTRUCT(...) __VA_ARGS__ MACROMAGIC_DEFER(MACROMAGIC_EMPTY)()
#define MACROMAGIC_EXPAND(...) __VA_ARGS__
#define MACROMAGIC_CONCAT_(a, b) a ## b
#define MACROMAGIC_CONCAT(a, b) MACROMAGIC_CONCAT_(a, b)
#define MACROMAGIC_ADDLINENUM(a) MACROMAGIC_CONCAT(a, __LINE__)
#define MACROMAGIC_STRINGIFY(x) #x

/** Expands to 0 if argument list is empty, 1 otherwise. Up to 32 arguments supported. */
#define MACROMAGIC_HASARGS(...) \
    MACROMAGIC_EXPAND(MACROMAGIC_DEFER(MACROMAGIC_ARGCOUNT_HELPER)(\
        _, ##__VA_ARGS__, MACROMAGIC_HASARGS_RSEQ_N() \
    ))

/** Expands to the number of its arguments. Up to 32 arguments supported. */
#define MACROMAGIC_NARGS(...) \
    MACROMAGIC_EXPAND(MACROMAGIC_DEFER(MACROMAGIC_ARGCOUNT_HELPER)(\
        _, ##__VA_ARGS__, MACROMAGIC_NARGS_RSEQ_N() \
    ))

#define MACROMAGIC_HASARGS_RSEQ_N() \
	1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 0,

#define MACROMAGIC_NARGS_RSEQ_N() \
    32, 31, 30, 29, 28, 27, 26, 25,  \
	24, 23, 22, 21, 20, 19, 18, 17,  \
	16, 15, 14, 13, 12, 11, 10,  9,  \
	 8,  7,  6,  5,  4,  3,  2,  1,  0,

#define MACROMAGIC_ARGCOUNT_HELPER(_0, \
	__1, __2, __3, __4, __5, __6, __7, __8, \
	__9, _10, _11, _12, _13, _14, _15, _16, \
	_17, _18, _19, _20, _21, _22, _23, _24, \
	_25, _26, _27, _28, _29, _30, _31, _32, N, ...) N

/** Expands to base N, where N is the number of arguments. */
#define MACROMAGIC_OVERLOAD_NARGS(base, ...) \
    MACROMAGIC_CONCAT(base, MACROMAGIC_NARGS(__VA_ARGS__))

/** Expands to base N, where N is 0 if the argument list is empty, 1 otherwise. */
#define MACROMAGIC_OVERLOAD_HASARGS(base, ...) \
    MACROMAGIC_CONCAT(base, MACROMAGIC_HASARGS(__VA_ARGS__))

/** Function is a hot spot. */
#if HAS_ATTRIBUTE(hot)
#define attr_hot __attribute__((hot))
#else
#define attr_hot
#endif

/** Function has no side-effects. */
#if HAS_ATTRIBUTE(pure)
#define attr_pure __attribute__((pure))
#else
#define attr_pure
#endif

/** Function has no side-effects, return value depends on arguments only.
 *  Must not take pointer parameters, must not return NULL. */
#if HAS_ATTRIBUTE(const)
#define attr_const __attribute__((const))
#else
#define attr_const
#endif

/** Function never returns NULL. */
#if HAS_ATTRIBUTE(returns_nonnull)
#define attr_returns_nonnull __attribute__((returns_nonnull))
#else
#define attr_returns_nonnull
#endif

/** Function must be called with NULL as the last argument (for varargs functions). */
#if HAS_ATTRIBUTE(sentinel)
#define attr_sentinel __attribute__((sentinel))
#else
#define attr_sentinel
#endif

/** Symbol is meant to be possibly unused. */
#if HAS_ATTRIBUTE(unused)
#define attr_unused __attribute__((unused))
#else
#define attr_unused
#endif

/** Symbol should be emitted even if it appears to be unused. */
#if HAS_ATTRIBUTE(used)
#define attr_used __attribute__((used))
#else
#define attr_used
#endif

/** Function or type is deprecated and should not be used. */
#if HAS_ATTRIBUTE(deprecated)
#define attr_deprecated(msg) __attribute__((deprecated(msg)))
#else
#define attr_deprecated(msg) #warning msg
#endif

/** Function parameters at specified positions must not be NULL. */
#if HAS_ATTRIBUTE(nonnull)
#define attr_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define attr_nonnull(...)
#endif

/** All pointer parameters must not be NULL. */
#if HAS_ATTRIBUTE(nonnull_all)
#define attr_nonnull_all __attribute__((nonnull))
#else
#define attr_nonnull_all
#endif

/** The return value of this function must not be ignored. */
#if HAS_ATTRIBUTE(warn_unused_result)
#define attr_nodiscard __attribute__((warn_unused_result))
#else
#define attr_nodiscard
#endif

/** Function takes a printf-style format string and variadic arguments. */
#if HAS_ATTRIBUTE(format)
#define attr_printf(fmt, va) __attribute__((format(__printf__, fmt, va)))
#else
#define attr_printf(fmt, va)
#endif

#ifdef __cplusplus
}
#endif
