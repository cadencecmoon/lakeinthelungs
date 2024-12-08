/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_DEFINES_H
#define _AMW_DEFINES_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {

#define AMW_C_DECL_BEGIN extern "C" {
#define AMW_C_DECL_END }
#else
#define AMW_C_DECL_BEGIN
#define AMW_C_DECL_END
#endif

#define VERSION_NUM_MAJOR(version) ((version) / 1000000)
#define VERSION_NUM_MINOR(version) (((version) / 1000) % 1000)
#define VERSION_NUM_REVISION(version) ((version) % 1000)

#define VERSION_NUM(major, minor, revision) \
    ((major) * 1000000 + (minor) * 1000 + (revision))

#define CONCAT_VERSION_STR__(major, minor, revision) #major "." #minor "." #revision
#define VERSION_MAKE_STR(major, minor, revision) CONCAT_VERSION_STR__(major, minor, revision)

/* detect platform */
#if !defined(AMW_PLATFORM_WINDOWS)
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
        #define AMW_PLATFORM_WINDOWS 1
    #endif
#elif !defined(AMW_PLATFORM_UNIX)
    #if defined(__unix__) || defined(__unix) || defined(unix)
        #define AMW_PLATFORM_UNIX 1
    #endif
#endif
#if !defined(AMW_PLATFORM_APPLE)
    #if defined(__APPLE__)
        #define AMW_PLATFORM_APPLE 1
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            #define AMW_PLATFORM_IOS 1
        #else
            #define AMW_PLATFORM_MACOS 1
        #endif
    #endif
#elif !defined(AMW_PLATFORM_LINUX)
    #if defined(__linux__) || defined(__gnu_linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
        #define AMW_PLATFORM_LINUX 1
    #endif
#endif
#if !defined(AMW_PLATFORM_ANDROID)
    #if defined(__ANDROID__)
        #if !defined(AMW_PLATFORM_ANDROID)
            #define AMW_PLATFORM_ANDROID 1
        #endif
    #endif
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define AMW_CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC__MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define AMW_CC_GNUC_VERSION VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(AMW_CC_GNUC_VERSION)
    #define AMW_CC_GNUC_VERSION_CHECK(major, minor, revision) (AMW_CC_GNUC_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define AMW_CC_GNUC_VERSION_CHECK(major, minor, revision) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define AMW_CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define AMW_CC_MSVC_VERSION VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define AMW_CC_MSVC_VERSION VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(AMW_CC_MSVC_VERSION)
    #define AMW_CC_MSVC_VERSION_CHECK(major, minor, revision) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define AMW_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (revision)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define AMW_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (revision)))
#else
    #define AMW_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_VER >= ((major * 100) + (minor)))
#endif

/* MINGW */
#if defined(__MINGW64__)
    #define CC_MINGW_VERSION VERSION_NUM(__MINGW64_VERSION_MAJOR,__MINGW64_VERSION_MINOR,__MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32__)
    #define CC_MINGW_VERSION VERSION_NUM(__MINGW32_VERSION_MAJOR,__MINGW32_VERSION_MINOR,0)
#endif
#if defined(AMW_CC_MINGW_VERSION)
    #define AMW_CC_MINGW_VERSION_CHECK(major, minor, revision) (AMW_CC_MINGW_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define AMW_CC_MINGW_VERSION_CHECK(major, minor, revision) (0)
#endif

/* ARM */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, \
            (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define CC_ARM_VERSION VERSION_NUM(__ARMCC_VERSION / 1000000, \
            (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(AMW_CC_ARM_VERSION)
    #define AMW_CC_ARM_VERSION_CHECK(major, minor, revision) (AMW_CC_ARM_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define AMW_CC_ARM_VERSION_CHECK(major, minor, revision) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(AMW_CC_CLANG_VERSION)
    #if __has_warning("-Wmissing-designated-field-initializers")
        #define AMW_CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define AMW_CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define AMW_CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define AMW_CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define AMW_CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define AMW_CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define AMW_CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define AMW_CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define AMW_CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define AMW_CC_CLANG_VERSION 100000
    #elif defined(__FILE_NAME__)
        #define AMW_CC_CLANG_VERSION 90000
    #elif __has_warning("-Wextra-semi-stmt") || __has_builtin(__builtin_rotateleft32)
        #define AMW_CC_CLANG_VERSION 80000
    /* For reasons unknown, Xcode 10.3 (Apple LLVM version 10.0.1) is apparently
     * based on Clang 7, but does not support the warning we test.
     * See https://en.wikipedia.org/wiki/Xcode#Toolchain_versions and
     * https://trac.macports.org/wiki/XcodeVersionInfo. */
    #elif __has_warning("-Wc++98-compat-extra-semi") || \
        (defined(__apple_build_version__) && __apple_build_version__ >= 10010000)
        #define AMW_CC_CLANG_VERSION 70000
    #elif __has_warning("-Wpragma-pack")
        #define AMW_CC_CLANG_VERSION 60000
    #elif __has_warning("-Wbitfield-enum-conversion")
        #define AMW_CC_CLANG_VERSION 50000
    #elif __has_attribute(diagnose_if)
        #define AMW_CC_CLANG_VERSION 40000
    #elif __has_warning("-Wcomma")
        #define AMW_CC_CLANG_VERSION 39000
    #elif __has_warning("-Wdouble-promotion")
        #define AMW_CC_CLANG_VERSION 38000
    #elif __has_warning("-Wshift-negative-value")
        #define AMW_CC_CLANG_VERSION 37000
    #elif __has_warning("-Wambiguous-ellipsis")
        #define AMW_CC_CLANG_VERSION 36000
    #else
        #define AMW_CC_CLANG_VERSION 1
    #endif
#endif
#if defined(AMW_CC_CLANG_VERSION)
    #define AMW_CC_CLANG_VERSION_CHECK(major, minor, revision) (AMW_CC_CLANG_VERSION >= ((major * 10000) + (minor * 1000) + (revision)))
#else
    #define AMW_CC_CLANG_VERSION_CHECK(major, minor, revision) (0)
#endif

/* GCC */
#if (defined(AMW_CC_GNUC_VERSION) || defined(AMW_CC_MINGW_VERSION)) && \
    !defined(AMW_CC_CLANG_VERSION) && \
    !defined(AMW_CC_ARM_VERSION) && \
    !defined(__EMSCRIPTEN__) && \
    !defined(__ibmxl__) && !(defined(__xlC__) && defined(__xlC_ver__)) && \
    !defined(__SUNPRO_C) && !defined(__SUNPRO_CC) && \
    !defined(__PGI) && defined(__PGIC__) && \
    !defined(__INTEL_COMPILER) && \
    !defined(__COMPCERT__)
    #define AMW_CC_GCC_VERSION AMW_CC_GNUC_VERSION
#endif
#if defined(AMW_CC_GCC_VERSION)
    #define AMW_CC_GCC_VERSION_CHECK(major, minor, revision) (AMW_CC_GCC_VERSION >= VERSION_NUM(major, minor, revision))
#else
    #define AMW_CC_GCC_VERSION_CHECK(major, minor, revision) (0)
#endif

/* ARM
 * https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__ARM_ARCH)
    #if __ARM_ARCH > 100
        #define AMW_ARCH_ARM (__ARM_ARCH)
    #else
        #define AMW_ARCH_ARM (__ARM_ARCH * 100)
    #endif
#elif defined(_M_ARM)
    #if _M_ARM > 100
        #define AMW_ARCH_ARM (_M_ARM)
    #else
        #define AMW_ARCH_ARM (_M_ARM * 100)
    #endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
    #define AMW_ARCH_ARM 800
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARM)
    #define AMW_ARCH_ARM 1
#endif
#if defined(AMW_ARCH_ARM)
    #define AMW_ARCH_ARM_CHECK(major, minor) (((major * 100) + (minor)) <= AMW_ARCH_ARM)
#else
    #define AMW_ARCH_ARM_CHECK(major, minor) (0)
#endif

/* AArch64
 * https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define AMW_ARCH_AARCH64 1000
#endif
#if defined(AMW_ARCH_AARCH64)
    #define AMW_ARCH_AARCH64_CHECK(ver) ((ver) <= AMW_ARCH_AARCH64)
#else
    #define AMW_ARCH_AARCH64_CHECK(ver) (0)
#endif

/* AMD64 / x86_64
 * https://en.wikipedia.org/wiki/X86-64 */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #if !defined(_M_ARM64EC)
        #define AMW_ARCH_AMD64 1000
    #endif
#endif

/* x86 32-bit
 * https://en.wikipedia.org/wiki/X86 */
#if defined(_M_IX86)
    #define AMW_ARCH_X86 (_M_IX86 / 100)
#elif defined(__I86__)
    #define AMW_ARCH_X86 __I86__
#elif defined(i686) || defined(__i686) || defined(__i686__)
    #define AMW_ARCH_X86 6
#elif defined(i586) || defined(__i586) || defined(__i586__)
    #define AMW_ARCH_X86 5
#elif defined(i486) || defined(__i486) || defined(__i486__)
    #define AMW_ARCH_X86 4
#elif defined(i386) || defined(__i386) || defined(__i386__)
    #define AMW_ARCH_X86 3
#elif defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__)
    #define AMW_ARCH_X86 3
#endif
#if defined(AMW_ARCH_X86)
    #define AMW_ARCH_X86_CHECK(version) ((version) <= AMW_ARCH_X86)
#else
    #define AMW_ARCH_X86_CHECK(version) (0)
#endif

/* RISC-V
 * https://en.wikipedia.org/wiki/RISC-V */
#if defined(__riscv) || defined(__riscv__)
    #if __riscv_xlen == 64
        #define AMW_ARCH_RISCV64
    #elif __riscv_xlen == 32
        #define AMW_ARCH_RISCV32
    #endif
#endif

/* inline */
#if !defined(AMW_INLINE) && !defined(AMW_NOINLINE)
    #if defined(AMW_CC_CLANG_VERSION) || defined(AMW_CC_GNUC_VERSION)
        #define AMW_INLINE __attribute__((always_inline)) inline
        #define AMW_NOINLINE __attribute__((noinline))
    #elif defined(AMW_CC_MSVC_VERSION)
        #define AMW_INLINE __forceinline
        #define AMW_NOINLINE __declspec(noinline)
    #else
        #define AMW_INLINE static inline
        #define AMW_NOINLINE
    #endif
#endif

/* noreturn */
#if !defined(AMW_NORETURN)
    #if defined(AMW_CC_CLANG_VERSION) || defined(AMW_CC_GNUC_VERSION)
        #define AMW_NORETURN __attribute__((noreturn))
    #elif defined(AMW_CC_MSVC_VERSION)
        #define AMW_NORETURN __declspec(noreturn)
    #else
        #define AMW_NORETURN
    #endif
#endif

/* unreachable */
#if !defined(AMW_UNREACHABLE)
    #if defined(AMW_CC_CLANG_VERSION) || defined(AMW_CC_GNUC_VERSION)
        #define AMW_UNREACHABLE __builtin_unreachable()
    #elif defined(AMW_CC_MSVC_VERSION)
        #define AMW_UNREACHABLE __assume(false)
    #else
        #define AMW_UNREACHABLE assertion(!"Unreachable code branch!");
    #endif
#endif

/* unused */
#if !defined(AMW_UNUSED)
    #if defined(AMW_CC_CLANG_VERSION) || defined(AMW_CC_GNUC_VERSION)
        #define AMW_UNUSED __attribute__((unused))
    #else
        /* As far as I know, MSVC doesn't have an 'unused' attribute equivalent. */
        #define AMW_UNUSED 
    #endif
#endif

/* typeof */
#if !defined(AMW_TYPEOF)
    #if defined(AMW_CC_MSVC_VERSION)
        #define AMW_TYPEOF(x) decltype((x))
    #elif defined(AMW_CC_CLANG_VERSION) || defined(AMW_CC_GNUC_VERSION)
        #define AMW_TYPEOF(x) __typeof__((x)) 
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
        #define AMW_TYPEOF(x) typeof((x))
    #elif defined(__cplusplus)
        #define AMW_TYPEOF(x) auto
    #endif
#endif

/* to check compiler extensions */
#ifdef __has_builtin
    #define AMW_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define AMW_HAS_BUILTIN(x) (0)
#endif
#ifdef __has_feature
    #define AMW_HAS_FEATURE(x) __has_feature(x)
#else
    #define AMW_HAS_FEATURE(x) (0)
#endif
#ifdef __has_warning
    #define AMW_HAS_WARNING(x) __has_warning(x)
#else
    #define AMW_HAS_WARNING(x) (0)
#endif

/* DLL export for public API */
#ifndef AMWAPI
    #ifdef AMW_DLL_EXPORT
        #if defined(AMW_PLATFORM_WINDOWS) || defined(__CYGWIN__)
            #define AMWAPI extern __declspec(dllexport)
        #elif AMW_CC_GNUC_VERSION_CHECK(4,0,0)
            #define AMWAPI extern __attribute__((visibility("default")))
        #endif
    #else
        #define AMWAPI extern
    #endif
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef bool
typedef _Bool       bool;
#endif

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;

#undef min
#undef max
#undef swap
#undef clamp
#undef clamp_zo
#undef bitmask
#undef arraysize

#define min(x, y)       (((x) < (y)) ? (x) : (y))
#define max(x, y)       (((x) > (y)) ? (x) : (y))
#define swap(a,b)       { AMW_TYPEOF(a) temp_ = a; a = b; b = temp_; }
#define clamp(x, a, b)  (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define clamp_zo(x)     (clamp(x, 0, 1))
#define bitmask(n)      ((1u << (n)) - 1u)
#define arraysize(a)    (sizeof(a) / sizeof(a[0]))

AMW_INLINE bool is_pow2(size_t x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

AMW_INLINE i32 most_significant_bit_idx(u32 x) {
#if AMW_CC_GNUC_VERSION_CHECK(3,4,0)
    if (x == 0)
        return -1;
    return 31 - __builtin_clz(x);
#elif defined(AMW_CC_MSVC_VERSION)
    u32 idx;
    if (_BitScanReverse(&idx, x)) {
        return (i32)idx;
    }
    return -1;
#else
    /* Based off of Bit Twiddling Hacks by Sean Eron Anderson
     * <seander@cs.stanford.edu>, released in the public domain.
     * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog */
    const u32 b[] = { 0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000 };
    const i32 S[] = { 1, 2, 4, 8, 16 };

    i32 msb_idx = 0;
    i32 i;

    if (x == 0)
        return -1;
    for (i = 4; i >= 0; i--)  {
        if (x & b[i]) {
            x >>= S[i];
            msb_idx |= S[i];
        }
    }
    return msb_idx;
#endif
}

AMW_INLINE bool has_exactly_one_bit_set(u32 x) {
    if (x && !(x & (x - 1)))
        return true;
    return false;
}

enum result {
    result_success = 0,
    result_error_unknown,
    result_error_invalid_context,
    result_error_no_fallback,
};

AMW_C_DECL_END
#endif /* _AMW_DEFINES_H */
