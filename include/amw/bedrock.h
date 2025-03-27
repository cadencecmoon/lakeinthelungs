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

/* C99 freestanding headers */
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <complex.h>
#include <limits.h>
#include <float.h>

/* C11 freestanding headers */
#include <stdalign.h>
#include <stdatomic.h>

/* standard library headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            #define PLATFORM_APPLE_IOS 1
        #else
            #define PLATFORM_APPLE_MACOS 1
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
#if !defined(PLATFORM_EMSCRIPTEN)
    #if defined(__EMSCRIPTEN__)
        #define PLATFORM_EMSCRIPTEN 1
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

/** Likely and unlikely branches. */
#if !defined(LIKELY) && !defined(UNLIKELY)
    #if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define LIKELY(x)   __builtin_expect(!!(x), 1)
        #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else
        #define LIKELY(x)   (x)
        #define UNLIKELY(x) (x)
    #endif
#endif

/** The compiler declares a type for you. */
#if !defined(TYPEOF)
    #if defined(CC_MSVC_VERSION)
        #define TYPEOF(x) decltype((x))
    #elif defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
        #define TYPEOF(x) __typeof__((x)) 
    #else /* may generate errors depending on the compiler */
        #define TYPEOF(x) typeof((x))
    #endif
#endif

/** Declares API function visibility for DLL builds. */
#if !defined(AMWAPI)
    #ifdef AMW_BUILD_DLL_EXPORT
        #if defined(PLATFORM_WINDOWS) || defined(__CYGWIN__)
            #define AMWAPI extern __declspec(dllexport)
        #elif defined(CC_GNUC_VERSION)
            #define AMWAPI extern __attribute__((visibility("default")))
        #else
            #define AMWAPI extern
        #endif
    #else
        #define AMWAPI extern
    #endif
#endif

/** Declares the calling convention. */
#if !defined(AMWCALL)
    #ifdef PLATFORM_WINDOWS
        /* on windows use the stdcall convention */
        #define AMWCALL __stdcall
    #else
        /* on other platforms use the default calling convention */
        #define AMWCALL
    #endif
#endif

/** inline & noinline */
#if defined(CC_CLANG_VERSION) || defined(CC_GNUC_VERSION)
    #define attr_inline static __attribute__((always_inline)) inline
    #define attr_noinline __attribute__((noinline))
#elif defined(CC_MSVC_VERSION)
    #define attr_inline static __forceinline
    #define attr_noinline __declspec(noinline)
#else
    #define attr_inline static inline
    #define attr_noinline
#endif

/** The function never returns. */
#if HAS_ATTRIBUTE(noreturn)
    #define attr_noreturn __attribute__((noreturn))
#elif defined(CC_MSVC_VERSION)
    #define attr_noreturn __declspec(noreturn)
#else
    #define attr_noreturn
#endif

/** Function is a cold spot and will be optimized for size. */
#if HAS_ATTRIBUTE(cold)
#define attr_cold __attribute__((cold))
#else
#define attr_cold
#endif

/** Function is a hot spot and will be optimized for speed. */
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

/** The pointer returned by this function cannot alias any other pointer valid when the function returns. */
#if HAS_ATTRIBUTE(malloc)
#define attr_malloc __attribute__((malloc))
#else
#define attr_malloc
#endif

/** Hints to the compiler that a statement that falls through to another case label is intentional. */
#if HAS_ATTRIBUTE(fallthrough)
#define attr_fallthrough __attribute__((fallthrough))
#else
#define attr_fallthrough
#endif

/** Enables detection of invalid or unsafe accesses by functions or their callers.
 *  Possible modes are: read_only, read_write, write_only, none. */
#if HAS_ATTRIBUTE(access)
#define attr_access(mode, ref)              __attribute__((access(mode, ref)))
#define attr_access_sized(mode, ref, size)  __attribute__((access(mode, ref, size)))
#else
#define attr_access(mode, ref)
#define attr_access_sized(mode, ref, size)
#endif

/** Set alignment rules for a type. */
#if HAS_ATTRIBUTE(__aligned__)
    #define attr_aligned(alignment) __attribute__((__aligned__(alignment)))
#elif defined(CC_MSVC_VERSION)
    #define attr_aligned(alignment) __declspec(align(alignment))
#else
    #define attr_aligned(alignment) /* no alignment */
#endif

/* https://en.wikipedia.org/wiki/X86-64 */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #if !defined(_M_ARM64EC)
        #define ARCH_AMD64 1000
        #define ARCH_X86_64 ARCH_AMD64
    #endif
#endif

/* https://en.wikipedia.org/wiki/X86 */
#if defined(_M_IX86)
    #define ARCH_X86 (_M_IX86 / 100)
#elif defined(__I86__)
    #define ARCH_X86 __I86__
#elif defined(i686) || defined(__i686) || defined(__i686__)
    #define ARCH_X86 6
#elif defined(i586) || defined(__i586) || defined(__i586__)
    #define ARCH_X86 5
#elif defined(i486) || defined(__i486) || defined(__i486__)
    #define ARCH_X86 4
#elif defined(i386) || defined(__i386) || defined(__i386__)
    #define ARCH_X86 3
#elif defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__)
    #define ARCH_X86 3
#endif
#if defined(ARCH_X86)
    #define ARCH_X86_CHECK(version) ((version) <= ARCH_X86)
#else
    #define ARCH_X86_CHECK(version) (0)
#endif

/* SIMD ISA extensions for intel x86 and amd64 */
#if defined(_M_IX86_FP)
    #define ARCH_X86_MMX 1
    #include <mmintrin.h>
    #if (_M_IX86_FP >= 1)
        #define ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if (_M_IX86_FP >= 2)
        #define ARCH_X86_SSE 1
        #define ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#elif defined(_M_X64)
    #define ARCH_X86_SSE 1
    #define ARCH_X86_SSE2 1
    #include <emmintrin.h>
#else
    #if defined(__MMX__)
        #define ARCH_X86_MMX 1
        #include <mmintrin.h>
    #endif
    #if defined(__SSE__)
        #define ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if defined(__SSE2__)
        #define ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#endif
#if defined(__SSE3__)
    #define ARCH_X86_SSE3 1
    #include <pmmintrin.h>
#endif
#if defined(__SSSE3__)
    #define ARCH_X86_SSSE3 1
    #include <tmmintrin.h>
#endif
#if defined(__SSE4_1__)
    #define ARCH_X86_SSE4_1 1
    #include <smmintrin.h>
#endif
#if defined(__SSE4_2__)
    #define ARCH_X86_SSE4_2 1
    #include <nmmintrin.h>
#endif
#if defined(__SSE4A__)
    #define ARCH_X86_SSE4A 1
    #include <ammintrin.h>
#endif
#if defined(__XOP__)
    #define ARCH_X86_XOP 1
    #include <xopintrin.h>
#endif
#if defined(__AVX__)
    #define ARCH_X86_AVX 1
    #if !defined(ARCH_X86_SSE3)
        #define ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if !defined(ARCH_X86_SSE4_1)
        #define ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if !defined(ARCH_X86_SSE4_2)
        #define ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX2__)
    #define ARCH_X86_AVX2 1
    #if defined(CC_MSVC_VERSION)
        #ifndef __FMA__
            #define __FMA__ 1
        #endif
    #endif
    #include <immintrin.h>
#endif
#if defined(__FMA__)
    #define ARCH_X86_FMA 1
    #if !defined(ARCH_X86_AVX)
        #define ARCH_X86_AVX 1
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX512VP2INTERSECT__)
    #define ARCH_X86_AVX512VP2INTERSECT 1
#endif
#if defined(__AVX512BITALG__)
    #define ARCH_X86_AVX512BITALG 1
#endif
#if defined(__AVX512VPOPCNTDQ__)
    #define ARCH_X86_AVX512VPOPCNTDQ 1
#endif
#if defined(__AVX512VBMI__)
    #define ARCH_X86_AVX512VBMI 1
#endif
#if defined(__AVX512VBMI2__)
    #define ARCH_X86_AVX512VBMI2 1
#endif
#if defined(__AVX512VNNI__)
    #define ARCH_X86_AVX512VNNI 1
#endif
#if defined(__AVX5124VNNIW__)
    #define ARCH_X86_AVX5124VNNIW 1
#endif
#if defined(__AVX512BW__)
    #define ARCH_X86_AVX512BW 1
#endif
#if defined(__AVX512BF16__)
    #define ARCH_X86_AVX512BF16 1
#endif
#if defined(__AVX512CD__)
    #define ARCH_X86_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
    #define ARCH_X86_AVX512DQ 1
#endif
#if defined(__AVX512F__)
    #define ARCH_X86_AVX512F 1
#endif
#if defined(__AVX512VL__)
    #define ARCH_X86_AVX512VL 1
#endif
#if defined(__AVX512FP16__)
    #define ARCH_X86_AVX512FP16 1
#endif
#if defined(__GFNI__)
    #define ARCH_X86_GFNI 1
#endif
#if defined(__PCLMUL__)
    #define ARCH_X86_PCLMUL 1
#endif
#if defined(__VPCLMULQDQ__)
    #define ARCH_X86_VPCLMULQDQ 1
#endif
#if (defined(__F16C__) || CC_MSVC_VERSION_CHECK(19,30,0)) && defined(ARCH_X86_AVX2)
    #define ARCH_X86_F16C 1
#endif
#if defined(__AES__)
    #define ARCH_X86_AES 1
#endif

#if CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            INLINE void __attribute__((__always_inline__, __nodebug__))
            _m_prefetch(void *__P) {
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif CC_MINGW_VERSION_CHECK(4,0,0)
    #include <intrin.h>
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define ARCH_AARCH64 1000
#endif
#if defined(ARCH_AARCH64)
    #define ARCH_AARCH64_CHECK(ver) ((ver) <= ARCH_AARCH64)
#else
    #define ARCH_AARCH64_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__ARM_ARCH)
    #if __ARM_ARCH > 100
        #define ARCH_ARM (__ARM_ARCH)
    #else
        #define ARCH_ARM (__ARM_ARCH * 100)
    #endif
#elif defined(_M_ARM)
    #if _M_ARM > 100
        #define ARCH_ARM (_M_ARM)
    #else
        #define ARCH_ARM (_M_ARM * 100)
    #endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
    #define ARCH_ARM 800
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARM)
    #define ARCH_ARM 1
#endif
#if defined(ARCH_ARM)
    #define ARCH_ARM_CHECK(major, minor) (((major * 100) + (minor)) <= ARCH_ARM)
#else
    #define ARCH_ARM_CHECK(major, minor) (0)
#endif

/* ARM SIMD ISA extensions */
#if defined(__ARM_NEON) || defined(AMW_ARCH_AARCH64)
    #if defined(ARCH_AARCH64)
        #define ARCH_ARM_NEON ARCH_AARCH64
        #include <arm64_neon.h>
    #elif defined(ARCH_ARM)
        #define ARCH_ARM_NEON ARCH_ARM
        #include <arm_neon.h>
    #endif
#endif
#if defined(__ARM_FEATURE_AES) && __ARM_FEATURE_AES
    #define ARCH_ARM_AES 1
#endif
#if defined(__ARM_FEATURE_COMPLEX) && __ARM_FEATURE_COMPLEX
    #define ARCH_ARM_COMPLEX 1
#endif
#if defined(__ARM_FEATURE_CRYPTO) && __ARM_FEATURE_CRYPTO
    #define ARCH_ARM_CRYPTO 1
#endif
#if defined(__ARM_FEATURE_CRC32) && __ARM_FEATURE_CRC32
    #define ARCH_ARM_CRC32 1
#endif
#if defined(__ARM_FEATURE_DOTPROD) && __ARM_FEATURE_DOTPROD
    #define ARCH_ARM_DOTPROD 1
#endif
#if defined(__ARM_FEATURE_FMA) && __ARM_FEATURE_FMA
    #define ARCH_ARM_FMA 1
#endif
#if defined(__ARM_FEATURE_FP16_FML) && __ARM_FEATURE_FP16_FML
    #define ARCH_ARM_FP16_FML 1
#endif
#if defined(__ARM_FEATURE_FRINT) && __ARM_FEATURE_FRINT
    #define ARCH_ARM_FRINT 1
#endif
#if defined(__ARM_FEATURE_MATMUL_INT8) && __ARM_FEATURE_MATMUL_INT8
    #define ARCH_ARM_MATMUL_INT8 1
#endif
#if defined(__ARM_FEATURE_SHA2) && __ARM_FEATURE_SHA2 && !defined(__APPLE_CC__)
    #define ARCH_ARM_SHA2 1
#endif
#if defined(__ARM_FEATURE_SHA3) && __ARM_FEATURE_SHA3
    #define ARCH_ARM_SHA3 1
#endif
#if defined(__ARM_FEATURE_SHA512) && __ARM_FEATURE_SHA512
    #define ARCH_ARM_SHA512 1
#endif
#if defined(__ARM_FEATURE_SM3) && __ARM_FEATURE_SM3
    #define ARCH_ARM_SM3 1
#endif
#if defined(__ARM_FEATURE_SM4) && __ARM_FEATURE_SM4
    #define ARCH_ARM_SM4 1
#endif
#if defined(__ARM_FEATURE_SVE) && __ARM_FEATURE_SVE
    #define ARCH_ARM_SVE 1
#endif
#if defined(__ARM_FEATURE_QRDMX) && __ARM_FEATURE_QRDMX
    #define ARCH_ARM_QRDMX 1
#endif
/* Availability of 16-bit floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
    #define ARCH_ARM_NEON_FP16
#endif
/* Availability of 16-bit brain floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)
    #define ARCH_ARM_NEON_BF16
#endif

/* https://en.wikipedia.org/wiki/RISC-V */
#if defined(__riscv) || defined(__riscv__)
    #define ARCH_RISCV 1
    #if __riscv_xlen == 64
        #define ARCH_RISCV64 1
    #elif __riscv_xlen == 32
        #define ARCH_RISCV32 1
    #endif
#endif
/* RISC-V SIMD ISA extensions */
#if defined(__riscv_zve32x)
    #define ARCH_RISCV_ZVE32X 1
#endif
#if defined(__riscv_zve32f)
    #define ARCH_RISCV_ZVE32F 1
#endif
#if defined(__riscv_zve64x)
    #define ARCH_RISCV_ZVE64X 1
#endif
#if defined(__riscv_zve64f)
    #define ARCH_RISCV_ZVE64F 1
#endif
#if defined(__riscv_zve64d)
    #define ARCH_RISCV_ZVE64D 1
#endif
#if defined(__riscv_v)
    #define ARCH_RISCV_V 1
#endif
#if defined(__riscv_zvfh)
    #define ARCH_RISCV_ZVFH 1
#endif
#if defined(__riscv_zvfhmin)
    #define ARCH_RISCV_ZVFHMIN 1
#endif

/* https://en.wikipedia.org/wiki/WebAssembly */
#if defined(__wasm__)
    #define ARCH_WASM 1
#endif
#if defined(ARCH_WASM) && defined(__wasm_simd128__)
    #define ARCH_WASM_SIMD128
    #include <wasm_simd128.h>
#endif
#if defined(ARCH_WASM) && defined(__wasm_relaxed_simd__)
    #define ARCH_WASM_RELAXED_SIMD
#endif

#define _ARCH_BYTEORDER_LE 1234
#define _ARCH_BYTEORDER_BE 4321

#ifndef _ARCH_BYTEORDER
    #if defined(PLATFORM_LINUX)
        #include <endian.h>
        #define _ARCH_BYTEORDER __BYTE_ORDER
    #elif defined(__sun) && defined(__SVR4) /* solaris */
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__OpenBSD__) || defined(__DragonFly__)
        #include <endian.h>
        #define _ARCH_BYTEORDER BYTE_ORDER
    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
        #include <sys/endian.h>
        #define _ARCH_BYTEORDER BYTE_ORDER
    /* predefs from newer GCC and CLANG versions */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        #if defined(ARCH_MIPS) || defined(ARCH_POWER) || defined(ARCH_SPARC)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #endif
    #endif
#endif

#ifndef _ARCH_FLOAT_WORD_ORDER
    /* predefs from newer GCC and Clang versions */
    #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__FLOAT_WORD_ORDER__)
        #if (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_LE
        #elif (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__MAVERICK__)
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_LE
    #elif (defined(ARCH_ARM) || defined(__thumb__)) && !defined(__VFP_FP__) && !defined(__ARM_EABI__)
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_BE
    #else
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER
    #endif
#endif

#if _ARCH_FLOAT_WORD_ORDER == _ARCH_BYTEORDER_LE
    #define ARCH_FLOAT_LIL_ENDIAN 1
#else
    #define ARCH_FLOAT_BIG_ENDIAN 1
#endif

#ifndef __cplusplus
#define true 1
#define false 0
#endif

typedef int8_t                  b8;
typedef int32_t                 b32;

typedef int8_t                  s8;
typedef int16_t                 s16;
typedef int32_t                 s32;
typedef int64_t                 s64;

typedef uint8_t                 u8;
typedef uint16_t                u16;
typedef uint32_t                u32;
typedef uint64_t                u64;

typedef intptr_t                sptr;
typedef uintptr_t               uptr;
typedef ptrdiff_t               ssize;
typedef size_t                  usize;

typedef float                   f32;
typedef double                  f64;

typedef _Complex float          cmplxf;
typedef _Complex double         cmplx;

#ifndef HAS_BUILTIN_COMPLEX
    #if HAS_BUILTIN(__builtin_complex)
        #define HAS_BUILTIN_COMPLEX 1
    #elif CC_GNUC_VERSION_CHECK(4,7,0)
        #define HAS_BUILTIN_COMPLEX 1
    #endif
#endif

#ifdef CC_CLANG_VERSION
    /** On these platforms CMPLX is defined without __extension__, causing false warnings. */
    #if defined(PLATFORM_EMSCRIPTEN) || defined(PLATFORM_APPLE)
        #undef CMPLX
        #undef CMPLXF
    #endif
#endif

#if !defined(CMPLX)
    #if defined(HAS_BUILTIN_COMPLEX)
        #define CMPLX(re, im) __builtin_complex((double)(re), (double)(im))
    #elif defined(CC_CLANG_VERSION)
        #define CMPLX(re, im) (__extension__ (_Complex double){ (double)(re), (double)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Imaginary_I * (double)(im))
    #else
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Complex_I * (double)(im))
    #endif
#endif

#if !defined(CMPLXF)
    #if defined(HAS_BUILTIN_COMPLEX)
        #define CMPLXF(re, im) __builtin_complex((float)(re), (float)(im))
    #elif defined(CC_CLANG_VERSION)
        #define CMPLXF(re, im) (__extension__ (_Complex float){ (float)(re), (float)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Imaginary_I * (float)(im))
    #else
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Complex_I * (float)(im))
    #endif
#endif

typedef _Atomic b32             at_b32;

typedef _Atomic s8              at_s8;
typedef _Atomic s16             at_s16;
typedef _Atomic s32             at_s32;
typedef _Atomic s64             at_s64;

typedef _Atomic u8              at_u8;
typedef _Atomic u16             at_u16;
typedef _Atomic u32             at_u32;
typedef _Atomic u64             at_u64;

typedef _Atomic sptr            at_sptr;
typedef _Atomic uptr            at_uptr;
typedef _Atomic ssize           at_ssize;
typedef _Atomic usize           at_usize;

#undef min
#define min(x,y)        (((x) < (y)) ? (x) : (y))
#undef max
#define max(x,y)        (((x) > (y)) ? (x) : (y))
#undef clamp
#define clamp(x,a,b)    (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#undef clamp_zo
#define clamp_zo(x)     (clamp(x,0,1))
#undef arraysize
#define arraysize(a)    (sizeof(a) / sizeof(a[0]))
#undef lengthof 
#define lengthof(s)     (arraysize(s) - 1)
#undef xorswap
#define xorswap(a,b)    { if (a != b) { *a ^= *b; *b ^= *a; *a ^= *b; } }
#undef zero
#define zero(mem)       memset(&(mem), 0, sizeof((mem)))
#undef zerop
#define zerop(mem)      memset((mem), 0, sizeof(*(mem)))
#undef zeroa
#define zeroa(mem)      memset((mem), 0, sizeof((mem)))

attr_inline attr_const 
u16 bswap16(u16 x) {
#if HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ushort(x);
#else
#error Implement bswap for this platform
#endif
}

attr_inline attr_const 
u32 bswap32(u32 x) {
#if HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ulong(x);
#endif
}

attr_inline attr_const 
u64 bswap64(u64 x) {
#if HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_uint64(x);
#endif
}

#if _ARCH_BYTEORDER == _ARCH_BYTEORDER_LE
    #define ARCH_LIL_ENDIAN 1
    #define bswap16le(x) (x)
    #define bswap32le(x) (x)
    #define bswap64le(x) (x)
    #define bswap16be(x) bswap16(x) 
    #define bswap32be(x) bswap32(x)
    #define bswap64be(x) bswap64(x)
#else
    #define ARCH_BIG_ENDIAN 1
    #define bswap16le(x) bswap16(x) 
    #define bswap32le(x) bswap32(x)
    #define bswap64le(x) bswap64(x)
    #define bswap16be(x) (x)
    #define bswap32be(x) (x)
    #define bswap64be(x) (x)
#endif

#define const_epsilon   DBL_EPSILON
#define const_epsilonf  FLT_EPSILON

#define const_e         2.71828182845904523536028747135266250   /* e           */
#define const_log2e     1.44269504088896340735992468100189214   /* log2(e)     */
#define const_log10e    0.434294481903251827651128918916605082  /* log10(e)    */
#define const_ln2       0.693147180559945309417232121458176568  /* loge(2)     */
#define const_ln10      2.30258509299404568401799145468436421   /* loge(10)    */
#define const_pi        3.14159265358979323846264338327950288   /* pi          */
#define const_pi_2      1.57079632679489661923132169163975144   /* pi/2        */
#define const_pi_4      0.785398163397448309615660845819875721  /* pi/4        */
#define const_1_pi      0.318309886183790671537767526745028724  /* 1/pi        */
#define const_2_pi      0.636619772367581343075535053490057448  /* 2/pi        */
#define const_2_sqrtpi  1.12837916709551257389615890312154517   /* 2/sqrt(pi)  */
#define const_sqrt2     1.41421356237309504880168872420969808   /* sqrt(2)     */
#define const_sqrt1_2   0.707106781186547524400844362104849039  /* 1/sqrt(2)   */

#define const_ef        ((f32)const_e)
#define const_log2ef    ((f32)const_log2e)
#define const_log10ef   ((f32)const_log10e)
#define const_ln2f      ((f32)const_ln2)
#define const_ln10f     ((f32)const_ln10)
#define const_pif       ((f32)const_pi)
#define const_pi_2f     ((f32)const_pi_2)
#define const_pi_4f     ((f32)const_pi_4)
#define const_1_pif     ((f32)const_1_pi)
#define const_2_pif     ((f32)const_2_pi)
#define const_2_sqrtpif ((f32)const_2_sqrtpi)
#define const_sqrt2f    ((f32)const_sqrt2)
#define const_sqrt1_2f  ((f32)const_sqrt1_2)

typedef s32                     svec2[2];
typedef s32                     svec3[3];
typedef s32                     svec4[4];

typedef f32                     vec2[2];
typedef f32                     vec3[3];
typedef attr_aligned(16) f32    vec4[4];

typedef vec4                    quat; /* quaternion */

typedef attr_aligned(16) vec2   mat2[2];    /* 2x2 matrix */
typedef vec3                    mat2x3[2];
typedef vec4                    mat2x4[2];

typedef vec3                    mat3[3];    /* 3x3 matrix */
typedef vec2                    mat3x2[3];
typedef vec4                    mat3x4[3];

typedef attr_aligned(32) vec4   mat4[4];    /* 4x4 matrix */
typedef vec2                    mat4x2[4];
typedef vec3                    mat4x3[4];

enum result {
    /** Work was successful. */
    result_success = 0,
    /** A hint for the caller side to run a job again, or iterate through a loop. */
    result_continue,
    /** A hint that an allocation query was performed, and the procedure expects to receive an allocated 
     *  buffer in the next call. A query will write this code, and assert it whenever there is no 
     *  allocation it can work with on the subsequent calls. */
    result_allocation_query,
    result_warn,    /* TODO expand */
    result_error,   /* TODO expand */
};

#ifdef __cplusplus
}
#endif
