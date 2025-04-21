#pragma once

/* C99 freestanding headers */
#ifdef __cplusplus
#include <cstddef>
#include <cstdarg>
#include <cstdint>
#include <ccomplex>
#include <climits>
#include <cfloat>
extern "C" {
#else
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <complex.h>
#include <limits.h>
#include <float.h>
#endif /* __cplusplus */

#define LAKE_VERSION_NUM_MAJOR(version) ((version) / 1000000)
#define LAKE_VERSION_NUM_MINOR(version) (((version) / 1000) % 1000)
#define LAKE_VERSION_NUM_REVISION(version) ((version) % 1000)

#define LAKE_VERSION_NUM(major, minor, revision) \
    ((major) * 1000000 + (minor) * 1000 + (revision))

#define _LAKE_CONCAT_VERSION_STR__(major, minor, revision) #major "." #minor "." #revision
#define LAKE_VERSION_MAKE_STR(major, minor, revision) _LAKE_CONCAT_VERSION_STR__(major, minor, revision)

/* detect platform */
#if !defined(LAKE_PLATFORM_WINDOWS)
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
        #define LAKE_PLATFORM_WINDOWS 1
    #endif
#endif
#if !defined(LAKE_PLATFORM_UNIX)
    #if defined(__unix__) || defined(__unix) || defined(unix)
        #define LAKE_PLATFORM_UNIX 1
    #endif
#endif
#if !defined(LAKE_PLATFORM_APPLE)
    #if defined(__APPLE__)
        #define LAKE_PLATFORM_APPLE 1
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            #define LAKE_PLATFORM_APPLE_IOS 1
        #else
            #define LAKE_PLATFORM_APPLE_MACOS 1
        #endif
    #endif
#endif
#if !defined(LAKE_PLATFORM_LINUX)
    #if defined(__linux__) || defined(__gnu_linux__)
        #define LAKE_PLATFORM_LINUX 1
    #endif
#endif
#if !defined(LAKE_PLATFORM_ANDROID)
    #if defined(__ANDROID__)
        #define LAKE_PLATFORM_ANDROID 1
    #endif
#endif
#if !defined(LAKE_PLATFORM_EMSCRIPTEN)
    #if defined(__EMSCRIPTEN__)
        #define LAKE_PLATFORM_EMSCRIPTEN 1
    #endif
#endif

/* GNUC */
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define LAKE_CC_GNUC_VERSION LAKE_VERSION_NUM(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define LAKE_CC_GNUC_VERSION LAKE_VERSION_NUM(__GNUC__, __GNUC_MINOR__, 0)
#endif
#if defined(LAKE_CC_GNUC_VERSION)
    #define LAKE_CC_GNUC_VERSION_CHECK(major, minor, revision) (LAKE_CC_GNUC_VERSION >= LAKE_VERSION_NUM(major, minor, revision))
#else
    #define LAKE_CC_GNUC_VERSION_CHECK(major, minor, revision) (0)
#endif

/* MSVC */
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 100)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define LAKE_CC_MSVC_VERSION LAKE_VERSION_NUM(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif
#if !defined(LAKE_CC_MSVC_VERSION)
    #define LAKE_CC_MSVC_VERSION_CHECK(major, minor, revision) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define LAKE_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (revision)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define LAKE_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (revision)))
#else
    #define LAKE_CC_MSVC_VERSION_CHECK(major, minor, revision) (_MSC_VER >= ((major * 100) + (minor)))
#endif

/* MINGW */
#if defined(__MINGW64__)
    #define LAKE_CC_MINGW_VERSION LAKE_VERSION_NUM(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, __MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32__)
    #define LAKE_CC_MINGW_VERSION LAKE_VERSION_NUM(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#endif
#if defined(LAKE_CC_MINGW_VERSION)
    #define LAKE_CC_MINGW_VERSION_CHECK(major, minor, revision) (LAKE_CC_MINGW_VERSION >= LAKE_VERSION_NUM(major, minor, revision))
#else
    #define LAKE_CC_MINGW_VERSION_CHECK(major, minor, revision) (0)
#endif

/* EMSCRIPTEN */
#if defined(LAKE_PLATFORM_EMSCRIPTEN) || defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #define LAKE_CC_EMSCRIPTEN_VERSION LAKE_VERSION_NUM(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif
#if defined(LAKE_CC_EMSCRIPTEN_VERSION)
    #define LAKE_CC_EMSCRIPTEN_VERSION_CHECK(ma,mi,rev) (LAKE_CC_EMSCRIPTEN_VERSION >= LAKE_VERSION_NUM(ma,mi,rev))
    #define LAKE_CC_EMSCRIPTEN_VERSION_NOT(ma,mi,rev) (LAKE_CC_EMSCRIPTEN_VERSION < LAKE_VERSION_NUM(ma,mi,rev))
#else
    #define LAKE_CC_EMSCRIPTEN_VERSION_CHECK(ma,mi,rev) (0)
    #define LAKE_CC_EMSCRIPTEN_VERSION_NOT(ma,mi,rev) (0)
#endif

/* ARMC */
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define LAKE_CC_ARM_VERSION LAKE_VERSION_NUM(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define LAKE_CC_ARM_VERSION LAKE_VERSION_NUM(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif
#if defined(LAKE_CC_ARM_VERSION)
    #define LAKE_CC_ARM_VERSION_CHECK(major, minor, revision) (LAKE_CC_ARM_VERSION >= LAKE_VERSION_NUM(major, minor, revision))
#else
    #define LAKE_CC_ARM_VERSION_CHECK(major, minor, revision) (0)
#endif

/* IBM */
#if defined(__ibmxl__)
    #define LAKE_CC_IBM_VERSION LAKE_VERSION_NUM(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined(__xlC__) && defined(__xlC_ver__)
    #define LAKE_CC_IBM_VERSION LAKE_VERSION_NUM(__xlC__ >> 8, __xlC__ & 0xff, (__xlC_ver__ >> 8) & 0xff)
#elif defined(__xlC__)
    #define LAKE_CC_IBM_VERSION LAKE_VERSION_NUM(__xlC__ >> 8, __xlC__ & 0xff, 0)
#endif
#if defined(LAKE_CC_IBM_VERSION)
    #define LAKE_CC_IBM_VERSION_CHECK(ma,mi,rev) (LAKE_CC_IBM_VERSION >= LAKE_VERSION_NUM(ma,mi,rev))
    #define LAKE_CC_IBM_VERSION_NOT(ma,mi,rev) (LAKE_CC_IBM_VERSION < LAKE_VERSION_NUM(ma,mi,rev))
#else
    #define LAKE_CC_IBM_VERSION_CHECK(ma,mi,rev) (0)
    #define LAKE_CC_IBM_VERSION_NOT(ma,mi,rev) (0)
#endif

/* INTEL */
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && !defined(__ICL)
    #define LAKE_CC_INTEL_VERSION LAKE_VERSION_NUM(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#elif defined(__INTEL_COMPILER) && !defined(__ICL)
    #define LAKE_CC_INTEL_VERSION LAKE_VERSION_NUM(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#endif
#if defined(LAKE_CC_INTEL_VERSION)
    #define LAKE_CC_INTEL_VERSION_CHECK(ma,mi,rev) (LAKE_CC_INTEL_VERSION >= LAKE_VERSION_NUM(ma,mi,rev))
    #define LAKE_CC_INTEL_VERSION_NOT(ma,mi,rev) (LAKE_CC_INTEL_VERSION < LAKE_VERSION_NUM(ma,mi,rev))
#else
    #define LAKE_CC_INTEL_VERSION_CHECK(ma,mi,rev) (0)
    #define LAKE_CC_INTEL_VERSION_NOT(ma,mi,rev) (0)
#endif

/* PGI */
#if defined(__PGI) && defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__)
    #define LAKE_CC_PGI_VERSION LAKE_VERSION_NUM(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#endif
#if defined(LAKE_CC_PGI_VERSION)
    #define LAKE_CC_PGI_VERSION_CHECK(ma,mi,rev) (LAKE_CC_PGI_VERSION >= LAKE_VERSION_NUM(ma,mi,rev))
    #define LAKE_CC_PGI_VERSION_NOT(ma,mi,rev) (LAKE_CC_PGI_VERSION < LAKE_VERSION_NUM(ma,mi,rev))
#else
    #define LAKE_CC_PGI_VERSION_CHECK(ma,mi,rev) (0)
    #define LAKE_CC_PGI_VERSION_NOT(ma,mi,rev) (0)
#endif

/* SUNPRO */
#if defined(__SUNPRO_C) && (__SUNPRO_C > 0x1000)
    #define LAKE_CC_SUNPRO_VERSION LAKE_VERSION_NUM((((__SUNPRO_C >> 16) & 0xf) * 10) + ((__SUNPRO_C >> 12) & 0xf), \
            (((__SUNPRO_C >> 8) & 0xf) * 10) + ((__SUNPRO_C >> 4) & 0xf), (__SUNPRO_C & 0xf) * 10)
#elif defined(__SUNPRO_C)
    #define LAKE_CC_SUNPRO_VERSION LAKE_VERSION_NUM((__SUNPRO_C >> 8) & 0xf, (__SUNPRO_C >> 4) & 0xf, (__SUNPRO_C) & 0xf)
#elif defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x1000)
    #define LAKE_CC_SUNPRO_VERSION LAKE_VERSION_NUM((((__SUNPRO_CC >> 16) & 0xf) * 10) + ((__SUNPRO_CC >> 12) & 0xf), \
            (((__SUNPRO_CC >> 8) & 0xf) * 10) + ((__SUNPRO_CC >> 4) & 0xf), (__SUNPRO_CC & 0xf) * 10)
#elif defined(__SUNPRO_CC)
    #define LAKE_CC_SUNPRO_VERSION LAKE_VERSION_NUM((__SUNPRO_CC >> 8) & 0xf, (__SUNPRO_CC >> 4) & 0xf, (__SUNPRO_CC) & 0xf)
#endif
#if defined(LAKE_CC_SUNPRO_VERSION)
    #define LAKE_CC_SUNPRO_VERSION_CHECK(ma,mi,rev) (LAKE_CC_SUNPRO_VERSION >= LAKE_VERSION_NUM(ma,mi,rev))
    #define LAKE_CC_SUNPRO_VERSION_NOT(ma,mi,rev) (LAKE_CC_SUNPRO_VERSION < LAKE_VERSION_NUM(ma,mi,rev))
#else
    #define LAKE_CC_SUNPRO_VERSION_CHECK(ma,mi,rev) (0)
    #define LAKE_CC_SUNPRO_VERSION_NOT(ma,mi,rev) (0)
#endif

/* CLANG */
#if defined(__clang__) && !defined(LAKE_CC_CLANG_VERSION)
    #if __has_warning("-Wmissing-designated-field-initializers")
        #define LAKE_CC_CLANG_VERSION 190000
    #elif __has_warning("-Woverriding-option")
        #define LAKE_CC_CLANG_VERSION 180000
    #elif __has_attribute(unsafe_buffer_usage)  /* no new warnings in 17.0 */
        #define LAKE_CC_CLANG_VERSION 170000
    #elif __has_attribute(nouwtable)  /* no new warnings in 16.0 */
        #define LAKE_CC_CLANG_VERSION 160000
    #elif __has_warning("-Warray-parameter")
        #define LAKE_CC_CLANG_VERSION 150000
    #elif __has_warning("-Wbitwise-instead-of-logical")
        #define LAKE_CC_CLANG_VERSION 140000
    #elif __has_warning("-Waix-compat")
        #define LAKE_CC_CLANG_VERSION 130000
    #elif __has_warning("-Wformat-insufficient-args")
        #define LAKE_CC_CLANG_VERSION 120000
    #elif __has_warning("-Wimplicit-const-int-float-conversion")
        #define LAKE_CC_CLANG_VERSION 110000
    #elif __has_warning("-Wmisleading-indentation")
        #define LAKE_CC_CLANG_VERSION 100000
    #elif defined(__FILE_NAME__)
        #define LAKE_CC_CLANG_VERSION 90000
    #elif __has_warning("-Wextra-semi-stmt") || __has_builtin(__builtin_rotateleft32)
        #define LAKE_CC_CLANG_VERSION 80000
    /* For reasons unknown, Xcode 10.3 (Apple LLVM version 10.0.1) is apparently
     * based on Clang 7, but does not support the warning we test.
     * See https://en.wikipedia.org/wiki/Xcode#Toolchain_versions and
     * https://trac.macports.org/wiki/XcodeVersionInfo. */
    #elif __has_warning("-Wc++98-compat-extra-semi") || \
        (defined(__apple_build_version__) && __apple_build_version__ >= 10010000)
        #define LAKE_CC_CLANG_VERSION 70000
    #elif __has_warning("-Wpragma-pack")
        #define LAKE_CC_CLANG_VERSION 60000
    #elif __has_warning("-Wbitfield-enum-conversion")
        #define LAKE_CC_CLANG_VERSION 50000
    #elif __has_attribute(diagnose_if)
        #define LAKE_CC_CLANG_VERSION 40000
    #elif __has_warning("-Wcomma")
        #define LAKE_CC_CLANG_VERSION 39000
    #elif __has_warning("-Wdouble-promotion")
        #define LAKE_CC_CLANG_VERSION 38000
    #elif __has_warning("-Wshift-negative-value")
        #define LAKE_CC_CLANG_VERSION 37000
    #elif __has_warning("-Wambiguous-ellipsis")
        #define LAKE_CC_CLANG_VERSION 36000
    #else
        #define LAKE_CC_CLANG_VERSION 1
    #endif
#endif
#if defined(LAKE_CC_CLANG_VERSION)
    #define LAKE_CC_CLANG_VERSION_CHECK(major, minor, revision) (LAKE_CC_CLANG_VERSION >= ((major * 10000) + (minor * 1000) + (revision)))
#else
    #define LAKE_CC_CLANG_VERSION_CHECK(major, minor, revision) (0)
#endif

/* GCC */
#if (defined(LAKE_CC_GNUC_VERSION) || defined(LAKE_CC_MINGW_VERSION)) && \
    !defined(LAKE_CC_CLANG_VERSION) && \
    !defined(LAKE_CC_ARM_VERSION) && \
    !defined(LAKE_CC_PGI_VERSION) && \
    !defined(LAKE_CC_ARM_VERSION) && \
    !defined(LAKE_CC_IBM_VERSION) && \
    !defined(LAKE_CC_INTEL_VERSION) && \
    !defined(__COMPCERT__)
    #define LAKE_CC_GCC_VERSION LAKE_CC_GNUC_VERSION
#endif
#if defined(LAKE_CC_GCC_VERSION)
    #define LAKE_CC_GCC_VERSION_CHECK(major, minor, revision) (LAKE_CC_GCC_VERSION >= LAKE_VERSION_NUM(major, minor, revision))
#else
    #define LAKE_CC_GCC_VERSION_CHECK(major, minor, revision) (0)
#endif

/* check compiler extensions */
#ifdef __has_attribute
    #define LAKE_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
    #define LAKE_HAS_ATTRIBUTE(x) (0)
#endif
#ifdef __has_builtin
    #define LAKE_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define LAKE_HAS_BUILTIN(x) (0)
#endif
#ifdef __has_feature
    #define LAKE_HAS_FEATURE(x) __has_feature(x)
#else
    #define LAKE_HAS_FEATURE(x) (0)
#endif
#ifdef __has_warning
    #define LAKE_HAS_WARNING(x) __has_warning(x)
#else
    #define LAKE_HAS_WARNING(x) (0)
#endif

/** Declares API function visibility for DLL builds. */
#if !defined(LAKEAPI)
    #ifdef LAKE_BUILD_DLL_EXPORT
        #if defined(LAKE_PLATFORM_WINDOWS) || defined(__CYGWIN__)
            #define LAKEAPI extern __declspec(dllexport)
        #elif defined(LAKE_CC_GNUC_VERSION)
            #define LAKEAPI extern __attribute__((visibility("default")))
        #else
            #define LAKEAPI extern
        #endif
    #else
        #define LAKEAPI extern
    #endif
#endif

/* declares the calling convention */
#if !defined(LAKECALL)
    #ifdef LAKE_PLATFORM_WINDOWS
        /* on windows use the stdcall convention */
        #define LAKECALL __stdcall
    #else
        /* on other platforms use the default calling convention */
        #define LAKECALL
    #endif
#endif

/* https://en.wikipedia.org/wiki/X86-64 */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #if !defined(_M_ARM64EC)
        #define LAKE_ARCH_AMD64 1000
    #endif
#endif

/* https://en.wikipedia.org/wiki/X86 */
#if defined(_M_IX86)
    #define LAKE_ARCH_X86 (_M_IX86 / 100)
#elif defined(__I86__)
    #define LAKE_ARCH_X86 __I86__
#elif defined(i686) || defined(__i686) || defined(__i686__)
    #define LAKE_ARCH_X86 6
#elif defined(i586) || defined(__i586) || defined(__i586__)
    #define LAKE_ARCH_X86 5
#elif defined(i486) || defined(__i486) || defined(__i486__)
    #define LAKE_ARCH_X86 4
#elif defined(i386) || defined(__i386) || defined(__i386__)
    #define LAKE_ARCH_X86 3
#elif defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__)
    #define LAKE_ARCH_X86 3
#endif
#if defined(LAKE_ARCH_X86)
    #define LAKE_ARCH_X86_CHECK(version) ((version) <= LAKE_ARCH_X86)
#else
    #define LAKE_ARCH_X86_CHECK(version) (0)
#endif

/* SIMD ISA extensions for intel x86 and amd64 */
#if defined(_M_IX86_FP)
    #define LAKE_ARCH_X86_MMX 1
    #include <mmintrin.h>
    #if (_M_IX86_FP >= 1)
        #define LAKE_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if (_M_IX86_FP >= 2)
        #define LAKE_ARCH_X86_SSE 1
        #define LAKE_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#elif defined(_M_X64)
    #define LAKE_ARCH_X86_SSE 1
    #define LAKE_ARCH_X86_SSE2 1
    #include <emmintrin.h>
#else
    #if defined(__MMX__)
        #define LAKE_ARCH_X86_MMX 1
        #include <mmintrin.h>
    #endif
    #if defined(__SSE__)
        #define LAKE_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if defined(__SSE2__)
        #define LAKE_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#endif
#if defined(__SSE3__)
    #define LAKE_ARCH_X86_SSE3 1
    #include <pmmintrin.h>
#endif
#if defined(__SSSE3__)
    #define LAKE_ARCH_X86_SSSE3 1
    #include <tmmintrin.h>
#endif
#if defined(__SSE4_1__)
    #define LAKE_ARCH_X86_SSE4_1 1
    #include <smmintrin.h>
#endif
#if defined(__SSE4_2__)
    #define LAKE_ARCH_X86_SSE4_2 1
    #include <nmmintrin.h>
#endif
#if defined(__SSE4A__)
    #define LAKE_ARCH_X86_SSE4A 1
    #include <ammintrin.h>
#endif
#if defined(__XOP__)
    #define LAKE_ARCH_X86_XOP 1
    #include <xopintrin.h>
#endif
#if defined(__AVX__)
    #define LAKE_ARCH_X86_AVX 1
    #if !defined(LAKE_ARCH_X86_SSE3)
        #define LAKE_ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if !defined(LAKE_ARCH_X86_SSE4_1)
        #define LAKE_ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if !defined(LAKE_ARCH_X86_SSE4_2)
        #define LAKE_ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX2__)
    #define LAKE_ARCH_X86_AVX2 1
    #if defined(LAKE_CC_MSVC_VERSION)
        #ifndef __FMA__
            #define __FMA__ 1
        #endif
    #endif
    #include <immintrin.h>
#endif
#if defined(__FMA__)
    #define LAKE_ARCH_X86_FMA 1
    #if !defined(LAKE_ARCH_X86_AVX)
        #define LAKE_ARCH_X86_AVX 1
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX512VP2INTERSECT__)
    #define LAKE_ARCH_X86_AVX512VP2INTERSECT 1
#endif
#if defined(__AVX512BITALG__)
    #define LAKE_ARCH_X86_AVX512BITALG 1
#endif
#if defined(__AVX512VPOPCNTDQ__)
    #define LAKE_ARCH_X86_AVX512VPOPCNTDQ 1
#endif
#if defined(__AVX512VBMI__)
    #define LAKE_ARCH_X86_AVX512VBMI 1
#endif
#if defined(__AVX512VBMI2__)
    #define LAKE_ARCH_X86_AVX512VBMI2 1
#endif
#if defined(__AVX512VNNI__)
    #define LAKE_ARCH_X86_AVX512VNNI 1
#endif
#if defined(__AVX5124VNNIW__)
    #define LAKE_ARCH_X86_AVX5124VNNIW 1
#endif
#if defined(__AVX512BW__)
    #define LAKE_ARCH_X86_AVX512BW 1
#endif
#if defined(__AVX512BF16__)
    #define LAKE_ARCH_X86_AVX512BF16 1
#endif
#if defined(__AVX512CD__)
    #define LAKE_ARCH_X86_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
    #define LAKE_ARCH_X86_AVX512DQ 1
#endif
#if defined(__AVX512F__)
    #define LAKE_ARCH_X86_AVX512F 1
#endif
#if defined(__AVX512VL__)
    #define LAKE_ARCH_X86_AVX512VL 1
#endif
#if defined(__AVX512FP16__)
    #define LAKE_ARCH_X86_AVX512FP16 1
#endif
#if defined(__GFNI__)
    #define LAKE_ARCH_X86_GFNI 1
#endif
#if defined(__PCLMUL__)
    #define LAKE_ARCH_X86_PCLMUL 1
#endif
#if defined(__VPCLMULQDQ__)
    #define LAKE_ARCH_X86_VPCLMULQDQ 1
#endif
#if (defined(__F16C__) || LAKE_CC_MSVC_VERSION_CHECK(19,30,0)) && defined(LAKE_ARCH_X86_AVX2)
    #define LAKE_ARCH_X86_F16C 1
#endif
#if defined(__AES__)
    #define LAKE_ARCH_X86_AES 1
#endif

#if LAKE_CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef LAKE_CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            _m_prefetch(void *__P) {
            static void __attribute__((__always_inline__, __nodebug__)) inline
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif LAKE_CC_MINGW_VERSION_CHECK(4,0,0)
    #include <intrin.h>
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define LAKE_ARCH_AARCH64 1000
#endif
#if defined(LAKE_ARCH_AARCH64)
    #define LAKE_ARCH_AARCH64_CHECK(ver) ((ver) <= LAKE_ARCH_AARCH64)
#else
    #define LAKE_ARCH_AARCH64_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/ARM_architecture */
#if defined(__ARM_ARCH)
    #if __ARM_ARCH > 100
        #define LAKE_ARCH_ARM (__ARM_ARCH)
    #else
        #define LAKE_ARCH_ARM (__ARM_ARCH * 100)
    #endif
#elif defined(_M_ARM)
    #if _M_ARM > 100
        #define LAKE_ARCH_ARM (_M_ARM)
    #else
        #define LAKE_ARCH_ARM (_M_ARM * 100)
    #endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
    #define LAKE_ARCH_ARM 800
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARM)
    #define LAKE_ARCH_ARM 1
#endif
#if defined(LAKE_ARCH_ARM)
    #define LAKE_ARCH_ARM_CHECK(major, minor) (((major * 100) + (minor)) <= LAKE_ARCH_ARM)
#else
    #define LAKE_ARCH_ARM_CHECK(major, minor) (0)
#endif

/* ARM SIMD ISA extensions */
#if defined(__ARM_NEON) || defined(LAKE_ARCH_AARCH64)
    #if defined(LAKE_ARCH_AARCH64)
        #define LAKE_ARCH_ARM_NEON LAKE_ARCH_AARCH64
        #include <arm64_neon.h>
    #elif defined(LAKE_ARCH_ARM)
        #define LAKE_ARCH_ARM_NEON LAKE_ARCH_ARM
        #include <arm_neon.h>
    #endif
#endif
#if defined(__ARM_FEATURE_AES) && __ARM_FEATURE_AES
    #define LAKE_ARCH_ARM_AES 1
#endif
#if defined(__ARM_FEATURE_COMPLEX) && __ARM_FEATURE_COMPLEX
    #define LAKE_ARCH_ARM_COMPLEX 1
#endif
#if defined(__ARM_FEATURE_CRYPTO) && __ARM_FEATURE_CRYPTO
    #define LAKE_ARCH_ARM_CRYPTO 1
#endif
#if defined(__ARM_FEATURE_CRC32) && __ARM_FEATURE_CRC32
    #define LAKE_ARCH_ARM_CRC32 1
#endif
#if defined(__ARM_FEATURE_DOTPROD) && __ARM_FEATURE_DOTPROD
    #define LAKE_ARCH_ARM_DOTPROD 1
#endif
#if defined(__ARM_FEATURE_FMA) && __ARM_FEATURE_FMA
    #define LAKE_ARCH_ARM_FMA 1
#endif
#if defined(__ARM_FEATURE_FP16_FML) && __ARM_FEATURE_FP16_FML
    #define LAKE_ARCH_ARM_FP16_FML 1
#endif
#if defined(__ARM_FEATURE_FRINT) && __ARM_FEATURE_FRINT
    #define LAKE_ARCH_ARM_FRINT 1
#endif
#if defined(__ARM_FEATURE_MATMUL_INT8) && __ARM_FEATURE_MATMUL_INT8
    #define LAKE_ARCH_ARM_MATMUL_INT8 1
#endif
#if defined(__ARM_FEATURE_SHA2) && __ARM_FEATURE_SHA2 && !defined(__APPLE_CC__)
    #define LAKE_ARCH_ARM_SHA2 1
#endif
#if defined(__ARM_FEATURE_SHA3) && __ARM_FEATURE_SHA3
    #define LAKE_ARCH_ARM_SHA3 1
#endif
#if defined(__ARM_FEATURE_SHA512) && __ARM_FEATURE_SHA512
    #define LAKE_ARCH_ARM_SHA512 1
#endif
#if defined(__ARM_FEATURE_SM3) && __ARM_FEATURE_SM3
    #define LAKE_ARCH_ARM_SM3 1
#endif
#if defined(__ARM_FEATURE_SM4) && __ARM_FEATURE_SM4
    #define LAKE_ARCH_ARM_SM4 1
#endif
#if defined(__ARM_FEATURE_SVE) && __ARM_FEATURE_SVE
    #define LAKE_ARCH_ARM_SVE 1
#endif
#if defined(__ARM_FEATURE_QRDMX) && __ARM_FEATURE_QRDMX
    #define LAKE_ARCH_ARM_QRDMX 1
#endif
/* Availability of 16-bit floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
    #define LAKE_ARCH_ARM_NEON_FP16
#endif
/* Availability of 16-bit brain floating-point arithmetic intrinsics */
#if defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)
    #define LAKE_ARCH_ARM_NEON_BF16
#endif

/* https://en.wikipedia.org/wiki/RISC-V */
#if defined(__riscv) || defined(__riscv__)
    #define LAKE_ARCH_RISCV 1
    #if __riscv_xlen == 64
        #define LAKE_ARCH_RISCV64 1
    #elif __riscv_xlen == 32
        #define LAKE_ARCH_RISCV32 1
    #endif
#endif
/* RISC-V SIMD ISA extensions */
#if defined(__riscv_zve32x)
    #define LAKE_ARCH_RISCV_ZVE32X 1
#endif
#if defined(__riscv_zve32f)
    #define LAKE_ARCH_RISCV_ZVE32F 1
#endif
#if defined(__riscv_zve64x)
    #define LAKE_ARCH_RISCV_ZVE64X 1
#endif
#if defined(__riscv_zve64f)
    #define LAKE_ARCH_RISCV_ZVE64F 1
#endif
#if defined(__riscv_zve64d)
    #define LAKE_ARCH_RISCV_ZVE64D 1
#endif
#if defined(__riscv_v)
    #define LAKE_ARCH_RISCV_V 1
#endif
#if defined(__riscv_zvfh)
    #define LAKE_ARCH_RISCV_ZVFH 1
#endif
#if defined(__riscv_zvfhmin)
    #define LAKE_ARCH_RISCV_ZVFHMIN 1
#endif

/* https://en.wikipedia.org/wiki/IBM_POWER_Instruction_Set_Architecture */
#if defined(_M_PPC)
    #define LAKE_ARCH_POWER _M_PPC
#elif defined(_ARCH_PWR9)
    #define LAKE_ARCH_POWER 900
#elif defined(_ARCH_PWR8)
    #define LAKE_ARCH_POWER 800
#elif defined(_ARCH_PWR7)
    #define LAKE_ARCH_POWER 700
#elif defined(_ARCH_PWR6)
    #define LAKE_ARCH_POWER 600
#elif defined(_ARCH_PWR5)
    #define LAKE_ARCH_POWER 500
#elif defined(_ARCH_PWR4)
    #define LAKE_ARCH_POWER 400
#elif defined(_ARCH_440) || defined(__ppc440__)
    #define LAKE_ARCH_POWER 440
#elif defined(_ARCH_450) || defined(__ppc450__)
    #define LAKE_ARCH_POWER 450
#elif defined(_ARCH_601) || defined(__ppc601__)
    #define LAKE_ARCH_POWER 601
#elif defined(_ARCH_603) || defined(__ppc603__)
    #define LAKE_ARCH_POWER 603
#elif defined(_ARCH_604) || defined(__ppc604__)
    #define LAKE_ARCH_POWER 604
#elif defined(_ARCH_605) || defined(__ppc605__)
    #define LAKE_ARCH_POWER 605
#elif defined(_ARCH_620) || defined(__ppc620__)
    #define LAKE_ARCH_POWER 620
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC) || defined(__ppc)
    #define LAKE_ARCH_POWER 1
#endif
#if defined(LAKE_ARCH_POWER)
    #define LAKE_ARCH_POWER_CHECK(ver) ((ver) <= LAKE_ARCH_POWER)
#else
    #define LAKE_ARCH_POWER_CHECK(ver) (0)
#endif
#if defined(__ALTIVEC__)
    #define LAKE_ARCH_POWER_ALTIVEC            LAKE_ARCH_POWER
    #define LAKE_ARCH_POWER_ALTIVEC_CHECK(ver) ((ver) <= LAKE_ARCH_POWER)
#else
    #define LAKE_ARCH_POWER_ALTIVEC_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/MIPS_architecture */
#if defined(_MIPS_ISA_MIPS64R2)
    #define LAKE_ARCH_MIPS 642
#elif defined(_MIPS_ISA_MIPS64)
    #define LAKE_ARCH_MIPS 640
#elif defined(_MIPS_ISA_MIPS32R2)
    #define LAKE_ARCH_MIPS 322
#elif defined(_MIPS_ISA_MIPS32)
    #define LAKE_ARCH_MIPS 320
#elif defined(_MIPS_ISA_MIPS4)
    #define LAKE_ARCH_MIPS 4
#elif defined(_MIPS_ISA_MIPS3)
    #define LAKE_ARCH_MIPS 3
#elif defined(_MIPS_ISA_MIPS2)
    #define LAKE_ARCH_MIPS 2
#elif defined(_MIPS_ISA_MIPS1)
    #define LAKE_ARCH_MIPS 1
#elif defined(_MIPS_ISA_MIPS) || defined(__mips) || defined(__MIPS__)
    #define LAKE_ARCH_MIPS 1
#endif
#if defined(LAKE_ARCH_MIPS)
    #define LAKE_ARCH_MIPS_CHECK(ver) ((ver) <= LAKE_ARCH_MIPS)
#else
    #define LAKE_ARCH_MIPS_CHECK(ver) (0)
#endif
#if defined(__mips_loongson_mmi)
    #define LAKE_ARCH_MIPS_LOONGSON_MMI 1
#endif
#if defined(__mips_msa)
    #define LAKE_ARCH_MIPS_MSA 1
#endif

/* https://en.wikipedia.org/wiki/SPARC */
#if defined(__sparc_v9__) || defined(__sparcv9)
    #define LAKE_ARCH_SPARC 9
#elif defined(__sparc_v8__) || defined(__sparcv8)
    #define LAKE_ARCH_SPARC 8
#elif defined(__sparc_v7__) || defined(__sparcv7)
    #define LAKE_ARCH_SPARC 7
#elif defined(__sparc_v6__) || defined(__sparcv6)
    #define LAKE_ARCH_SPARC 6
#elif defined(__sparc_v5__) || defined(__sparcv5)
    #define LAKE_ARCH_SPARC 5
#elif defined(__sparc_v4__) || defined(__sparcv4)
    #define LAKE_ARCH_SPARC 4
#elif defined(__sparc_v3__) || defined(__sparcv3)
    #define LAKE_ARCH_SPARC 3
#elif defined(__sparc_v2__) || defined(__sparcv2)
    #define LAKE_ARCH_SPARC 2
#elif defined(__sparc_v1__) || defined(__sparcv1)
    #define LAKE_ARCH_SPARC 1
#elif defined(__sparc__) || defined(__sparc)
    #define LAKE_ARCH_SPARC 1
#endif
#if defined(LAKE_ARCH_SPARC)
    #define LAKE_ARCH_SPARC_CHECK(ver) ((ver) <= LAKE_ARCH_SPARC)
#else
    #define LAKE_ARCH_SPARC_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/Loongson#LoongArch */
#if defined(__loongarch32)
    #define LAKE_ARCH_LOONGARCH 1
#elif defined(__loongarch64)
    #define LAKE_ARCH_LOONGARCH 2
#endif

/* LSX: LoongArch 128-bits SIMD extension */
#if defined(__loongarch_sx)
    #define LAKE_ARCH_LOONGARCH_LSX 1
    #include <lsxintrin.h>
#endif

/* LASX: LoongArch 256-bits SIMD extension */
#if defined(__loongarch_asx)
    #define LAKE_ARCH_LOONGARCH_LASX 2
    #include <lasxintrin.h>
#endif

/* https://en.wikipedia.org/wiki/WebAssembly */
#if defined(__wasm__)
    #define LAKE_ARCH_WASM 1
#endif
#if defined(LAKE_ARCH_WASM) && defined(__wasm_simd128__)
    #define LAKE_ARCH_WASM_SIMD128
    #include <wasm_simd128.h>
#endif
#if defined(LAKE_ARCH_WASM) && defined(__wasm_relaxed_simd__)
    #define LAKE_ARCH_WASM_RELAXED_SIMD
#endif

/** Likely and unlikely branches. */
#if !defined(lake_likely) && !defined(lake_unlikely)
    #if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define lake_likely(x)   __builtin_expect(!!(x), 1)
        #define lake_unlikely(x) __builtin_expect(!!(x), 0)
    #else
        #define lake_likely(x)   (x)
        #define lake_unlikely(x) (x)
    #endif
#endif

/** The compiler declares a type for you. */
#if !defined(lake_typeof)
    #if defined(LAKE_CC_MSVC_VERSION)
        #define lake_typeof(x) decltype((x))
    #elif defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define lake_typeof(x) __typeof__((x)) 
    #else /* may generate errors depending on the compiler */
        #define lake_typeof(x) typeof((x))
    #endif
#endif

/** inline & noinline */
#if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
    #define lake_force_inline static __attribute__((always_inline)) inline
    #define lake_force_noinline __attribute__((noinline))
#elif defined(LAKE_CC_MSVC_VERSION)
    #define lake_force_inline static __forceinline
    #define lake_force_noinline __declspec(noinline)
#else
    #define lake_force_inline static inline
    #define lake_force_noinline
#endif

/** The function never returns. */
#if LAKE_HAS_ATTRIBUTE(noreturn)
    #define lake_noreturn __attribute__((noreturn))
#elif defined(LAKE_CC_MSVC_VERSION)
    #define lake_noreturn __declspec(noreturn)
#else
    #define lake_noreturn
#endif

/** Function is a cold spot and will be optimized for size. */
#if LAKE_HAS_ATTRIBUTE(cold)
#define lake_cold __attribute__((cold))
#else
#define lake_cold
#endif

/** Function is a hot spot and will be optimized for speed. */
#if LAKE_HAS_ATTRIBUTE(hot)
#define lake_hot __attribute__((hot))
#else
#define lake_hot
#endif

/** Function has no side-effects. */
#if LAKE_HAS_ATTRIBUTE(pure)
#define lake_pure __attribute__((pure))
#else
#define lake_pure
#endif

/** Function has no side-effects, return value depends on arguments only.
 *  Must not take pointer parameters, must not return NULL. */
#if LAKE_HAS_ATTRIBUTE(const)
#define lake_const __attribute__((const))
#else
#define lake_const
#endif

/** Function never returns NULL. */
#if LAKE_HAS_ATTRIBUTE(returns_nonnull)
#define lake_returns_nonnull __attribute__((returns_nonnull))
#else
#define lake_returns_nonnull
#endif

/** Function must be called with NULL as the last argument (for varargs functions). */
#if LAKE_HAS_ATTRIBUTE(sentinel)
#define lake_sentinel __attribute__((sentinel))
#else
#define lake_sentinel
#endif

/** Symbol is meant to be possibly unused. */
#if LAKE_HAS_ATTRIBUTE(unused)
#define lake_unused __attribute__((unused))
#else
#define lake_unused
#endif

/** Symbol should be emitted even if it appears to be unused. */
#if LAKE_HAS_ATTRIBUTE(used)
#define lake_used __attribute__((used))
#else
#define lake_used
#endif

/** Function or type is deprecated and should not be used. */
#if LAKE_HAS_ATTRIBUTE(deprecated)
#define lake_deprecated(msg) __attribute__((deprecated(msg)))
#else
#define lake_deprecated(msg) #warning msg
#endif

/** Function parameters at specified positions must not be NULL. */
#if LAKE_HAS_ATTRIBUTE(nonnull)
#define lake_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define lake_nonnull(...)
#endif

/** All pointer parameters must not be NULL. */
#if LAKE_HAS_ATTRIBUTE(nonnull)
#define lake_nonnull_all __attribute__((nonnull))
#else
#define lake_nonnull_all
#endif

/** The return value of this function must not be ignored. */
#if LAKE_HAS_ATTRIBUTE(warn_unused_result)
#define lake_nodiscard __attribute__((warn_unused_result))
#else
#define lake_nodiscard
#endif


/** Function takes a printf-style format string and variadic arguments. */
#if LAKE_HAS_ATTRIBUTE(format)
#define lake_printf(fmt, va) __attribute__((format(__printf__, fmt, va)))
#else
#define lake_printf(fmt, va)
#endif

/** The pointer returned by this function cannot alias any other pointer valid when the function returns. */
#if LAKE_HAS_ATTRIBUTE(malloc)
#define lake_malloc __attribute__((malloc))
#else
#define lake_malloc
#endif

/** Hints to the compiler that a statement that falls through to another case label is intentional. */
#if LAKE_HAS_ATTRIBUTE(fallthrough)
#define lake_fallthrough __attribute__((fallthrough))
#else
#define lake_fallthrough
#endif

/** Enables detection of invalid or unsafe accesses by functions or their callers.
 *  Possible modes are: read_only, read_write, write_only, none. */
#if LAKE_HAS_ATTRIBUTE(access)
#define lake_access(mode, ref)              __attribute__((access(mode, ref)))
#define lake_access_sized(mode, ref, size)  __attribute__((access(mode, ref, size)))
#else
#define lake_access(mode, ref)
#define lake_access_sized(mode, ref, size)
#endif

#ifndef LAKE_CACHELINE_SIZE
    #if defined(LAKE_ARCH_POWER) || defined(LAKE_ARCH_SPARC) || (defined(LAKE_PLATFORM_APPLE) && defined(LAKE_ARCH_AARCH64))
        #define LAKE_CACHELINE_SIZE 128
    #else /* should be valid for most systems */
        #define LAKE_CACHELINE_SIZE 64
    #endif
#endif /* LAKE_CACHELINE_SIZE */

#ifndef LAKE_ALIGN_MAXIMUM
    #ifdef LAKE_CC_MSVC_VERSION
        #if LAKE_CC_MSVC_VERSION_CHECK(19,16,0)
            /* Visual Studio 2017 or newer don't need an alignment cap */
        #else 
            #if defined(LAKE_ARCH_X86) || defined(LAKE_ARCH_AMD64)
                #if LAKE_CC_MSVC_VERSION_CHECK(19,14,0)
                    #define LAKE_ALIGN_MAXIMUM 64
                #elif LAKE_CC_MSVC_VERSION_CHECK(16,0,0)
                    #define LAKE_ALIGN_MAXIMUM 32
                #else
                    #define LAKE_ALIGN_MAXIMUM 16
                #endif
            #elif defined(LAKE_ARCH_ARM) || defined(LAKE_ARCH_AARCH64)
                #define LAKE_ALIGN_MAXIMUM 8
            #endif
        #endif
    #elif defined(LAKE_IBM_VERSION)
        #define LAKE_ALIGN_MAXIMUM 16
    #else
        #define LAKE_ALIGN_MAXIMUM 0
    #endif
#endif /* LAKE_ALIGN_MAXIMUM */

#if LAKE_ALIGN_MAXIMUM
    #define _LAKE_ALIGN_CAP(alignment) (((alignment) < (LAKE_ALIGN_MAXIMUM)) ? (alignment) : (LAKE_ALIGN_MAXIMUM))
#else
    #define _LAKE_ALIGN_CAP(alignment) (alignment)
#endif 

/** Set alignment rules for a type. */
#if LAKE_HAS_ATTRIBUTE(__aligned__)
    #define lake_alignment(alignment) __attribute__((__aligned__(_LAKE_ALIGN_CAP(alignment))))
#elif defined(LAKE_CC_MSVC_VERSION)
    #define lake_alignment(alignment) __declspec(align(_LAKE_ALIGN_CAP(alignment)))
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
    #define lake_alignment(alignment) _Alignas(_LAKE_ALIGN_CAP(alignment))
#else
    #define lake_alignment(alignment) /* no alignment */
#endif

/** Set alignment rules to match the vector extension registers. */
#ifndef lake_simd_alignment
    #ifdef LAKE_ARCH_X86_AVX
        /* matrix to be used by AVX 256-bit registers */
        #define lake_simd_alignment lake_alignment(32)
    #else
        /* by default use 16-byte alignment rules for SIMD data */ 
        #define lake_simd_alignment lake_alignment(16)
    #endif
#endif

/** Set alignment rules to match the size of a cacheline (64 or 128 bytes). */
#ifndef lake_cacheline_alignment
    #define lake_cacheline_alignment lake_alignment(LAKE_CACHELINE_SIZE)
#endif

#ifndef lake_assume_aligned
    #if LAKE_HAS_BUILTIN(__builtin_assume_aligned) || LAKE_CC_GNUC_VERSION_CHECK(4,7,0)
        #define lake_assume_aligned(expr, alignment) __builtin_assume_aligned((expr), (alignment))
    #else
        #define lake_assume_aligned(expr, alignment) (expr)
    #endif
#endif

#define lake_castptr_assume_aligned(expr, type) \
    ((type *)lake_assume_aligned((expr), lake_alignof(type)))

#ifndef LAKE_LOG_USE_COLOR
    #ifndef LAKE_LOG_DISABLE_COLOR
        #define LAKE_LOG_USE_COLOR 1
    #else
        #define LAKE_LOG_USE_COLOR 0
    #endif
#endif

#ifdef __cplusplus
#if defined(LAKE_CC_GNUC_VERSION)
    #define restrict __restrict__
#elif defined(LAKE_CC_MSVC_VERSION)
    #define restrict __restrict
#else
    #define restrict
#endif
#define LAKE_ZERO_INIT {}
#else
#define true 1
#define false 0
typedef _Bool bool;
#define LAKE_ZERO_INIT {0}
#endif /* __cplusplus */

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef intptr_t    sptr;
typedef uintptr_t   uptr;
typedef ptrdiff_t   ssize;
typedef size_t      usize;

typedef float       f32;
typedef double      f64;

#if !defined(lake_cast) && !defined(lake_const_cast) && !defined(lake_ptr_cast)
    #ifndef __cplusplus
        #define lake_cast(T, V)         ((T)(V))
        #define lake_const_cast(T, V)   ((T)(V))
        #define lake_ptr_cast(T, V)     ((T)(uptr)(V))
    #else
        #define lake_cast(T, V)         (static_cast<T>(V))
        #define lake_const_cast(T, V)   (const_cast<T>(V))
        #define lake_ptr_cast(T, V)     (reinterpret_cast<T>(V))
    #endif
#endif

#ifndef lake_sizeof
    #define lake_sizeof(T) (lake_cast(usize, sizeof(T)))
#endif

#ifndef lake_alignof
    #ifdef __cplusplus
        #define lake_alignof(T) (static_cast<usize>(alignof(T)))
    #elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
        #define lake_alignof(T) ((usize)alignof(T))
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define lake_alignof(T) ((usize)__alignof(T))
    #elif defined(LAKE_CC_GNUC_VERSION) || defined(LAKE_CC_CLANG_VERSION)
        #define lake_alignof(T) ((usize)__alignof__(T))
    #else
        #define lake_alignof(T) ((usize)&((struct { char c; T d; } *)0)->d)
    #endif
#endif

#define lake_min(x,y)               (((x) < (y)) ? (x) : (y))
#define lake_max(x,y)               (((x) > (y)) ? (x) : (y))
#define lake_clamp(x,a,b)           (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define lake_clamp_zo(x)            (lake_clamp(x,0,1))
#define lake_arraysize(a)           (lake_sizeof(a) / lake_sizeof(a[0]))
#define lake_lengthof(s)            (lake_arraysize(s) - 1)
#define lake_xorswap(a,b)           { if (a != b) { *a ^= *b; *b ^= *a; *a ^= *b; } }
#define lake_is_pow2(x)             ((x != 0) && ((x & (x - 1)) == 0))
#define lake_align_any(size,align)  (usize)((((((usize)size) - 1) / ((usize)align)) + 1) * ((usize)align))

/* To enable analysis, set these environment variables before running meson:
 *     export CC=clang
 *     export CFLAGS="-DLAKE_THREAD_SAFETY_ANALYSIS -Wthread-safety" */
#if defined(LAKE_THREAD_SAFETY_ANALYSIS) && defined(LAKE_CC_CLANG_VERSION) && (!defined(SWIG))
    #define _LAKE_THREAD_ATTRIBUTE(x) __attribute__((x))
#else
    #define _LAKE_THREAD_ATTRIBUTE(x)
#endif

#define lake_thread_capability(x)               _LAKE_THREAD_ATTRIBUTE(capability(x))
#define lake_thread_scoped_capability           _LAKE_THREAD_ATTRIBUTE(scoped_lockable)
#define lake_thread_guarded_by(x)               _LAKE_THREAD_ATTRIBUTE(guarded_by(x))
#define lake_thread_pt_guarded_by(x)            _LAKE_THREAD_ATTRIBUTE(pt_guarded_by(x))
#define lake_thread_acquired_before(x)          _LAKE_THREAD_ATTRIBUTE(acquired_before(x))
#define lake_thread_acquired_after(x)           _LAKE_THREAD_ATTRIBUTE(acquired_after(x))
#define lake_thread_requires(x)                 _LAKE_THREAD_ATTRIBUTE(requires_capability(x))
#define lake_thread_requires_shared(x)          _LAKE_THREAD_ATTRIBUTE(requires_shared_capability(x))
#define lake_thread_acquire(x)                  _LAKE_THREAD_ATTRIBUTE(acquire_capability(x))
#define lake_thread_acquire_shared(x)           _LAKE_THREAD_ATTRIBUTE(acquire_shared_capability(x))
#define lake_thread_release(x)                  _LAKE_THREAD_ATTRIBUTE(release_capability(x))
#define lake_thread_release_shared(x)           _LAKE_THREAD_ATTRIBUTE(release_shared_capability(x))
#define lake_thread_release_generic(x)          _LAKE_THREAD_ATTRIBUTE(release_generic_capability(x))
#define lake_thread_try_acquire(x)              _LAKE_THREAD_ATTRIBUTE(try_acquire_capability(x))
#define lake_thread_try_acquire_shared(x)       _LAKE_THREAD_ATTRIBUTE(try_acquire_shared_capability(x))
#define lake_thread_excludes(x)                 _LAKE_THREAD_ATTRIBUTE(locks_excluded(x))
#define lake_thread_assert_capability(x)        _LAKE_THREAD_ATTRIBUTE(assert_capability(x))
#define lake_thread_assert_shared_capability(x) _LAKE_THREAD_ATTRIBUTE(assert_shared_capability(x))
#define lake_thread_return_capability(x)        _LAKE_THREAD_ATTRIBUTE(lock_returned(x))
#define lake_thread_no_safety_analysis          _LAKE_THREAD_ATTRIBUTE(no_thread_safety_analysis)

#if LAKE_HAS_FEATURE(address_sanitizer)
    #define __SANITIZE_ADDRESS__ 1
#endif
#if LAKE_HAS_FEATURE(memory_sanitizer)
    #include <sanitizer/msan_interface.h>
    #ifndef LAKE_HAS_VALGRIND
        #define LAKE_HAS_VALGRIND 1
    #endif
    #define lake_memory_undefined(a,len)           __msan_allocated_memory(a,len)
    #define lake_memory_make_addressable(a,len)    MEM_UNDEFINED(a,len)
    #define lake_memory_make_defined(a,len)        __msan_unpoison(a,len)
    #define lake_memory_noaccess(a,len)            ((void)0)
    #define lake_memory_check_addressable(a,len)   ((void)0)
    #define lake_memory_check_defined(a,len)       __msan_check_mem_is_initialized(a,len)
    #define lake_memory_get_vbits(a,b,len)         __msan_copy_shadow(b,a,len)
    #define lake_memory_set_vbits(a,b,len)         __msan_copy_shadow(a,b,len)
    #define LAKE_REDZONE_SIZE 8
#elif defined(LAKE_HAS_VALGRIND)
    #include <valgrind/memcheck.h>
    #define lake_memory_undefined(a,len)           VALGRIND_MAKE_MEM_UNDEFINED(a,len)
    #define lake_memory_make_addressable(a,len)    MEM_UNDEFINED(a,len)
    #define lake_memory_make_defined(a,len)        VALGRIND_MAKE_MEM_DEFINED(a,len)
    #define lake_memory_noaccess(a,len)            VALGRIND_MAKE_MEM_NOACCESS(a,len)
    #define lake_memory_check_addressable(a,len)   VALGRIND_CHECK_MEM_IS_ADDRESSABLE(a,len)
    #define lake_memory_check_defined(a,len)       VALGRIND_CHECK_MEM_IS_DEFINED(a,len)
    #define lake_memory_get_vbits(a,b,len)         VALGRIND_GET_VBITS(a,b,len)
    #define lake_memory_set_vbits(a,b,len)         VALGRIND_SET_VBITS(a,b,len)
    #define LAKE_REDZONE_SIZE 8
#elif defined(__SANITIZE_ADDRESS__)
    #include <sanitizer/asan_interface.h>
    /* How to do manual poisoning:
     * https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning */
    #define lake_memory_undefined(a,len)           ((void)0)
    #define lake_memory_make_addressable(a,len)    ASAN_UNPOISON_MEMORY_REGION(a,len)
    #define lake_memory_make_defined(a,len)        ((void)0)
    #define lake_memory_noaccess(a,len)            ASAN_POISON_MEMORY_REGION(a,len)
    #define lake_memory_check_addressable(a,len)   lake_assertion(!__asan_region_is_poisoned((void*) a,len))
    #define lake_memory_check_defined(a,len)       ((void)0)
    #define lake_memory_get_vbits(a,b,len)         ((void)0)
    #define lake_memory_set_vbits(a,b,len)         ((void)0)
    #define LAKE_REDZONE_SIZE 8
#else
    #define lake_memory_undefined(a,len)           ((void)0)
    #define lake_memory_make_addressable(a,len)    ((void)0)
    #define lake_memory_make_defined(a,len)        ((void)0)
    #define lake_memory_noaccess(a,len)            ((void)0)
    #define lake_memory_check_addressable(a,len)   ((void)0)
    #define lake_memory_check_defined(a,len)       ((void)0)
    #define lake_memory_get_vbits(a,b,len)         ((void)0)
    #define lake_memory_set_vbits(a,b,len)         ((void)0)
    #define LAKE_REDZONE_SIZE 0
#endif /* valgrind */

#ifdef LAKE_HAS_VALGRIND
    #define LAKE_IF_VALGRIND(A,B) A
#else
    #define LAKE_IF_VALGRIND(A,B) B
#endif

#ifdef LAKE_TRASH_FREED_MEMORY
   /* _LAKE_TRASH_FILL() has to call lake_memory_make_addressable() to cancel any effect of
    * LAKE_TRASH_FREE().
    * This can happen in the case one does
    * LAKE_TRASH_ALLOC(A,B) ; LAKE_TRASH_FREE(A,B) ; LAKE_TRASH_ALLOC(A,B)
    * to reuse the same memory in an internal memory allocator like MEM_ROOT.
    * _LAKE_TRASH_FILL() is an internal function and should not be used externally. */
    #define _LAKE_TRASH_FILL(A,B,C) \
        do { const usize trash_tmp= (B); lake_memory_make_addressable(a, trash_tmp); lake_memset(A, C, trash_tmp); } while (0)
#else
    #define _LAKE_TRASH_FILL(A,B,C) \
        do { lake_memory_undefined((A), (B)); } while (0)
#endif
/** Note that some memory became allocated and/or uninitialized. */
#define LAKE_TRASH_ALLOC(A,B) do { _LAKE_TRASH_FILL(A,B,0xA5); lake_memory_make_addressable(A,B); } while(0)
/** Note that some memory became freed. (Prohibit further access to it.) */
#define LAKE_TRASH_FREE(A,B) do { _LAKE_TRASH_FILL(A,B,0x8F); lake_memory_noaccess(a,b); } while(0)

#define LAKE_BYTEORDER_LE 1234
#define LAKE_BYTEORDER_BE 4321

#ifndef LAKE_BYTEORDER
    #if defined(LAKE_PLATFORM_LINUX)
        #include <endian.h>
        #define LAKE_BYTEORDER __BYTE_ORDER
    #elif defined(__sun) && defined(__SVR4) /* solaris */
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__OpenBSD__) || defined(__DragonFly__)
        #include <endian.h>
        #define LAKE_BYTEORDER BYTE_ORDER
    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
        #include <sys/endian.h>
        #define LAKE_BYTEORDER BYTE_ORDER
    /* predefs from newer GCC and CLANG versions */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        #if defined(LAKE_ARCH_MIPS) || defined(LAKE_ARCH_POWER) || defined(LAKE_ARCH_SPARC)
            #define LAKE_BYTEORDER LAKE_BYTEORDER_BE
        #else
            #define LAKE_BYTEORDER LAKE_BYTEORDER_LE
        #endif
    #endif
#endif

#ifndef LAKE_FLOAT_WORD_ORDER
    /* predefs from newer GCC and Clang versions */
    #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__FLOAT_WORD_ORDER__)
        #if (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define LAKE_FLOAT_WORD_ORDER LAKE_BYTEORDER_LE
        #elif (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define LAKE_FLOAT_WORD_ORDER LAKE_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__MAVERICK__)
        #define LAKE_FLOAT_WORD_ORDER LAKE_BYTEORDER_LE
    #elif defined(LAKE_ARCH_ARM) && !defined(__VFP_FP__) && !defined(__ARM_EABI__)
        #define LAKE_FLOAT_WORD_ORDER LAKE_BYTEORDER_BE
    #else
        #define LAKE_FLOAT_WORD_ORDER LAKE_BYTEORDER
    #endif
#endif

#if LAKE_FLOAT_WORD_ORDER == LAKE_BYTEORDER_LE
    #define LAKE_FLOAT_LIL_ENDIAN 1
#else
    #define LAKE_FLOAT_BIG_ENDIAN 1
#endif

lake_force_inline lake_const u16 
lake_bswap16(u16 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_ushort(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
    __asm lea  edx, [x]
    __asm mov  ax, [edx]
    __asm xchg ah, al
    __asm mov  [edx], ax
    return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("xchgb 0, %h0" : "=Q" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    __asm__("rorw $8, %0" : "+r" (x));
    return x;
#else
#error Implement bswap for this platform
#endif
}

lake_force_inline lake_const u32 
lake_bswap32(u32 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_ulong(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    #if LAKE_ARCH_X86_CHECK(4)
        __asm__("bswap %0" : "+r" (x));
        return x;
    #else /* i386 has broken bswap */
        __asm__("rorw $8, %w0 ; rorl $16, %0 ; rorw $8, %w0" : "+r" (x));
        return x;
    #endif
#endif
}

lake_force_inline lake_const u64 
lake_bswap64(u64 x) 
{
#if LAKE_HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_AMD64)
    return _byteswap_uint64(x);
#elif LAKE_CC_MSVC_VERSION_CHECK(14,0,0) && defined(LAKE_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm xchg  eax, [edx+4]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(LAKE_ARCH_AMD64)
    __asm__("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#elif defined(LAKE_ARCH_X86)
    #if LAKE_ARCH_X86_CHECK(4)
        union {
            struct {
                u32 a, b;
            } s;
            u64 u;
        } v;
        v.u = x;
        __asm__("bswapl %0 ; bswapl %1 ; xchgl %0, %1"
                : "=r"(v.s.a), "=r" (v.s.b)
                : "0"(v.s.a), "1" (v.s.b));
    #else /* i386 has broken bswap */
        u32 hi, lo;
        lo = (u32)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (u32)(x & 0xFFFFFFFF);
        x = bswap32(lo);
        x <<= 32;
        x |= bswap32(hi);
        return x;
    #endif
#endif
}

lake_force_inline lake_const f32 
lake_bswapf32(f32 x) 
{
    union {
        f32 f;
        u32 u;
    } bswap;
    bswap.f = x;
    bswap.u = lake_bswap32(bswap.u);
    return bswap.f;
}

#if LAKE_BYTEORDER == LAKE_BYTEORDER_LE
    #define LAKE_LIL_ENDIAN 1
    #define lake_bswap16le(x)   (x)
    #define lake_bswap32le(x)   (x)
    #define lake_bswap64le(x)   (x)
    #define lake_bswapfle(x)    (x)
    #define lake_bswap16be(x)   lake_bswap16(x) 
    #define lake_bswap32be(x)   lake_bswap32(x)
    #define lake_bswap64be(x)   lake_bswap64(x)
    #define lake_bswapfbe(x)    lake_bswapf(x)
#else
    #define LAKE_BIG_ENDIAN 1
    #define lake_bswap16le(x)   lake_bswap16(x) 
    #define lake_bswap32le(x)   lake_bswap32(x)
    #define lake_bswap64le(x)   lake_bswap64(x)
    #define lake_bswapfle(x)    lake_bswapf32(x)
    #define lake_bswap16be(x)   (x)
    #define lake_bswap32be(x)   (x)
    #define lake_bswap64be(x)   (x)
    #define lake_bswapfbe(x)    (x)
#endif 

#if LAKE_HAS_BUILTIN(__builtin_debugtrap)
    #define lake_debugtrap() __builtin_debugtrap()
#elif LAKE_HAS_BUILTIN(__debugbreak)
    #define lake_debugtrap() __debugbreak()
#endif
#if !defined(lake_debugtrap)
    #if defined(LAKE_CC_MSVC_VERSION) || defined(LAKE_CC_INTEL_VERSION)
        #define lake_debugtrap() __debugbreak()
    #elif defined(LAKE_CC_ARM_VERSION)
        #define lake_debugtrap() __breakpoint(42)
    #elif defined(LAKE_CC_IBM_VERSION)
        #include <builtins.h>
        #define lake_debugtrap() __trap(42)
    #elif LAKE_ARCH_X86_CHECK(7) /* _M_IX86 */
        lake_force_inline void lake_debugtrap(void) { __asm int 3h; }
    #elif defined(LAKE_ARCH_X86) || defined(LAKE_ARCH_AMD64)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__("int $03"); }
    #elif defined(__thumb__) /* arm32 */
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__(".inst 0xde01"); }
    #elif defined(LAKE_ARCH_AARCH64)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__(".inst 0xd4200000"); }
    #elif defined(LAKE_ARCH_ARM)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__(".inst 0xe7f001f0"); }
    #elif defined(LAKE_ARCH_ALPHA) && !defined(__osf__)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__("bpt"); }
    #elif defined(_54_)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__("ESTOP"); }
    #elif defined(_55_)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
    #elif defined(_64P_)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__("SWBP 0"); }
    #elif defined(_6x_)
        lake_force_inline void lake_debugtrap(void) { __asm__ __volatile__("NOP\n .word 0x10000000"); }
    #elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(LAKE_CC_GNUC_VERSION)
        #define lake_debugtrap() __builtin_trap()
    #else
        #include <signal.h>
        #if defined(SIGTRAP)
            #define lake_debugtrap() raise(SIGTRAP)
        #else
            #define lake_debugtrap() raise(SIGABRT)
        #endif
    #endif
#endif

/* unreachable */
#if !defined(lake_unreachable)
    #if defined(LAKE_CC_CLANG_VERSION) || defined(LAKE_CC_GNUC_VERSION)
        #define lake_unreachable __builtin_unreachable()
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define lake_unreachable __assume(false)
    #else
        #define lake_unreachable lake_assertion(!"Unreachable code branch, shouldn't get here!");
    #endif
#endif

/* assumption */
#if !defined(lake_assume)
    #if LAKE_HAS_BUILTIN(__builtin_assume)
        #define lake_assume(x) __builtin_assume(x)
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define lake_assume(x) __assume(x)
    #else
        #define lake_assume(x) do { if(!(x)) { lake_unreachable; } } while(0)
    #endif
#endif

#define lake_pair(T0, T1)               \
    struct {                            \
        T0 first;                       \
        T1 second;                      \
    }

#define lake_dynamic_array(T)           \
    struct {                            \
        T  *data;                       \
        u32 count;                      \
        u32 capacity;                   \
    }
/* TODO implement dynamic array procedures as macros */

#define lake_span_to_const(T)           \
    struct {                            \
        const T *data;                  \
        usize size;                     \
    }

#define lake_fixed_list(T, CAPACITY)    \
    struct {                            \
        T  data[CAPACITY];              \
        u8 size;                        \
    }

typedef s16     wchar16;
typedef s32     wchar32;
#ifdef LAKE_USE_WCHAR32_UNICODE
typedef wchar32 wchar;
#else
typedef wchar16 wchar;
#endif

#define LAKE_SMALL_STRING_CAPACITY (63u)
typedef lake_fixed_list(char, LAKE_SMALL_STRING_CAPACITY) lake_small_string;

#define LAKE_EPSILON    DBL_EPSILON
#define LAKE_EPSILONf   FLT_EPSILON

#define LAKE_E          2.71828182845904523536 /* e          */
#define LAKE_LOG2E      1.44269504088896340735 /* log2(e)    */
#define LAKE_LOG10E     0.43429448190325182765 /* log10(e)   */
#define LAKE_LN2        0.69314718055994530941 /* loge(2)    */
#define LAKE_LN10       2.30258509299404568401 /* loge(10)   */
#define LAKE_TAU        6.28318530717958647692 /* 2pi        */
#define LAKE_INV_TAU    0.15915494309189533577 /* 1/2pi      */
#define LAKE_PI         3.14159265358979323846 /* pi         */
#define LAKE_PI_2       1.57079632679489661923 /* pi/2       */
#define LAKE_PI_4       0.78539816339744830961 /* pi/4       */
#define LAKE_1_PI       0.31830988618379067153 /* 1/pi       */
#define LAKE_2_PI       0.63661977236758134307 /* 2/pi       */
#define LAKE_2_SQRTPI   1.12837916709551257389 /* 2/sqrt(pi) */
#define LAKE_SQRT2      1.41421356237309504880 /* sqrt(2)    */
#define LAKE_SQRT1_2    0.70710678118654752440 /* 1/sqrt(2)  */

#define LAKE_Ef         ((f32)LAKE_E)
#define LAKE_LOG2Ef     ((f32)LAKE_LOG2E)
#define LAKE_LOG10Ef    ((f32)LAKE_LOG10E)
#define LAKE_LN2f       ((f32)LAKE_LN2)
#define LAKE_LN10f      ((f32)LAKE_LN10)
#define LAKE_TAUf       ((f32)LAKE_TAU)
#define LAKE_INV_TAUf   ((f32)LAKE_INV_TAU)
#define LAKE_PIf        ((f32)LAKE_PI)
#define LAKE_PI_2f      ((f32)LAKE_PI_2)
#define LAKE_PI_4f      ((f32)LAKE_PI_4)
#define LAKE_1_PIf      ((f32)LAKE_1_PI)
#define LAKE_2_PIf      ((f32)LAKE_2_PI)
#define LAKE_2_SQRTPIf  ((f32)LAKE_2_SQRTPI)
#define LAKE_SQRT2f     ((f32)LAKE_SQRT2)
#define LAKE_SQRT1_2f   ((f32)LAKE_SQRT1_2)

typedef _Complex float      cmplxf;
typedef _Complex double     cmplx;

#ifndef LAKE_HAS_BUILTIN_COMPLEX
    #if LAKE_HAS_BUILTIN(__builtin_complex)
        #define LAKE_HAS_BUILTIN_COMPLEX 1
    #elif LAKE_CC_GNUC_VERSION_CHECK(4,7,0)
        #define LAKE_HAS_BUILTIN_COMPLEX 1
    #endif
#endif

#ifdef LAKE_CC_CLANG_VERSION
    /** On these platforms CMPLX is defined without __extension__, causing false warnings. */
    #if defined(LAKE_PLATFORM_EMSCRIPTEN) || defined(LAKE_PLATFORM_APPLE)
        #undef CMPLX
        #undef CMPLXF
    #endif
#endif

#if !defined(CMPLX)
    #if defined(LAKE_HAS_BUILTIN_COMPLEX)
        #define CMPLX(re, im) __builtin_complex((double)(re), (double)(im))
    #elif defined(LAKE_CC_CLANG_VERSION)
        #define CMPLX(re, im) (__extension__ (_Complex double){ (double)(re), (double)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Imaginary_I * (double)(im))
    #else
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Complex_I * (double)(im))
    #endif
#endif

#if !defined(CMPLXF)
    #if defined(LAKE_HAS_BUILTIN_COMPLEX)
        #define CMPLXF(re, im) __builtin_complex((float)(re), (float)(im))
    #elif defined(LAKE_CC_CLANG_VERSION)
        #define CMPLXF(re, im) (__extension__ (_Complex float){ (float)(re), (float)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Imaginary_I * (float)(im))
    #else
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Complex_I * (float)(im))
    #endif
#endif

typedef s32                         svec2[2];
typedef s32                         svec3[3];
typedef s32                         svec4[4];

typedef f32                         vec2[2];
typedef f32                         vec3[3];
typedef lake_alignment(16)  f32     vec4[4];

typedef vec4                        quat;       /* quaternion */

typedef lake_alignment(16)  vec2    mat2[2];    /* 2x2 matrix */
typedef vec3                        mat2x3[2];
typedef vec4                        mat2x4[2];

typedef vec3                        mat3[3];    /* 3x3 matrix */
typedef vec2                        mat3x2[3];
typedef vec4                        mat3x4[3];

typedef lake_simd_alignment vec4    mat4[4];    /* 4x4 matrix */
typedef vec2                        mat4x2[4];
typedef vec3                        mat4x3[4];

struct lake_viewport {
    f32 x, y, width, height, min_depth, max_depth;
};

struct lake_offset2d {
    s32 x, y;
};

struct lake_offset3d {
    s32 x, y, z;
};

struct lake_extent2d {
    u32 width, height;
};

struct lake_extent3d {
    u32 width, height, depth;
};

struct lake_rect2d {
    struct lake_offset2d offset;
    struct lake_extent2d extent;
};

#define LAKE_MS_PER_SECOND       1000
#define LAKE_US_PER_SECOND       1000000
#define LAKE_NS_PER_SECOND       1000000000LL
#define LAKE_NS_PER_MS           1000000
#define LAKE_NS_PER_US           1000
#define LAKE_SECONDS_TO_NS(S)    (((u64)(S)) * LAKE_NS_PER_SECOND)
#define LAKE_NS_TO_SECONDS(NS)   ((NS) / LAKE_NS_PER_SECOND)
#define LAKE_MS_TO_NS(MS)        (((u64)(MS)) * LAKE_NS_PER_MS)
#define LAKE_NS_TO_MS(NS)        ((NS) / LAKE_NS_PER_MS)
#define LAKE_US_TO_NS(US)        (((u64)(US)) * LAKE_NS_PER_US)
#define LAKE_NS_TO_US(NS)        ((NS) / LAKE_NS_PER_US)

#ifndef lake_may_alias
    #if defined(LAKE_CC_GNUC_VERSION)
        #define lake_may_alias __attribute__((__may_alias__))
        #define LAKE_HAS_MAY_ALIAS 1
    #elif defined(LAKE_CC_MSVC_VERSION)
        /* MSVC does not enforce strict aliasing rules, as is the case with GCC or Clang. */
        #define lake_may_alias
        #define LAKE_HAS_MAY_ALIAS 1
    #else
        #define lake_may_alias
        #define LAKE_HAS_MAY_ALIAS 0
    #endif
#endif

#if LAKE_HAS_MAY_ALIAS
typedef u32   lake_may_alias    alias_u32;
typedef u64   lake_may_alias    alias_u64;
typedef usize lake_may_alias    alias_word;
#define lake_union_alias        union lake_may_alias
#define lake_struct_alias       struct lake_may_alias
#else
#define lake_union_alias        /* empty */
#define lake_struct_alias       /* empty */
#endif /* LAKE_HAS_MAY_ALIAS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define _LAKE_ATOMIC_IMPLEMENTATION_CPP_ATOMICS         1
#define _LAKE_ATOMIC_IMPLEMENTATION_GCC_ATOMICS         2
#define _LAKE_ATOMIC_IMPLEMENTATION_CLANG_C11_ATOMICS   3
#define _LAKE_ATOMIC_IMPLEMENTATION_MSVC_INTERLOCKED    4
#define _LAKE_ATOMIC_IMPLEMENTATION_X86_ASM             5   /* 64-bit and 32-bit */
#define _LAKE_ATOMIC_IMPLEMENTATION_ARM_ASM             6   /* 64-bit and 32-bit */
#define _LAKE_ATOMIC_IMPLEMENTATION_RISCV_ASM           7   /* 64-bit and 32-bit */

#ifndef LAKE_ATOMIC_IMPLEMENTATION
    #if __cplusplus
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_CPP_ATOMICS
    #elif LAKE_CC_GCC_VERSION_CHECK(4,7,0)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_GCC_ATOMICS
    #elif defined(LAKE_CC_CLANG_VERSION) && __has_extension(c_atomic)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_CLANG_C11_ATOMICS
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_MSVC_INTERLOCKED
    #elif defined(LAKE_CC_GNUC_VERSION) && (defined(LAKE_ARCH_AMD64) || defined(LAKE_ARCH_X86))
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_X86_ASM
    #elif defined(LAKE_CC_GNUC_VERSION) && (defined(LAKE_ARCH_AARCH64) || defined(LAKE_ARCH_ARM))
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_ARM_ASM
    #elif defined(LAKE_CC_GNUC_VERSION) && (defined(LAKE_ARCH_RISCV64) || defined(LAKE_ARCH_RISCV32))
        #define LAKE_ATOMIC_IMPLEMENTATION _LAKE_ATOMIC_IMPLEMENTATION_RISCV_ASM
    #else
        #error Not a valid atomics implementation
    #endif
#endif

#define _LAKE_ATOMICS_H
#if LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_CPP_ATOMICS
    #include <amwe/arch/atomics_c++.hpp>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_GCC_ATOMICS
    #include <amwe/arch/atomics_gcc.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_CLANG_C11_ATOMICS
    #include <amwe/arch/atomics_clang.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_MSVC_INTERLOCKED
    #include <amwe/arch/atomics_msvc.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_X86_ASM
    #include <amwe/arch/atomics_x86.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_ARM_ASM
    #include <amwe/arch/atomics_arm.h>
#elif LAKE_ATOMIC_IMPLEMENTATION == _LAKE_ATOMIC_IMPLEMENTATION_RISV_ASM
    #include <amwe/arch/atomics_riscv.h>
#endif
#undef _LAKE_ATOMICS_H

typedef LAKE_ATOMIC(s8)         atomic_s8;
typedef LAKE_ATOMIC(s16)        atomic_s16;
typedef LAKE_ATOMIC(s32)        atomic_s32;
typedef LAKE_ATOMIC(s64)        atomic_s64;

typedef LAKE_ATOMIC(u8)         atomic_u8;
typedef LAKE_ATOMIC(u16)        atomic_u16;
typedef LAKE_ATOMIC(u32)        atomic_u32;
typedef LAKE_ATOMIC(u64)        atomic_u64;

typedef LAKE_ATOMIC(wchar16)    atomic_wchar16;
typedef LAKE_ATOMIC(wchar32)    atomic_wchar32;
typedef LAKE_ATOMIC(wchar)      atomic_wchar;

typedef LAKE_ATOMIC(sptr)       atomic_sptr;
typedef LAKE_ATOMIC(uptr)       atomic_uptr;
typedef LAKE_ATOMIC(ssize)      atomic_ssize;
typedef LAKE_ATOMIC(usize)      atomic_usize;

/** Relaxed atomic operations.
 *
 *  When an operation on an atomic variable is not expected to synchronize 
 *  with operations on other (atomic or non-atomic) variables, no memory 
 *  barriers are needed and the relaxed memory ordering can be used. These 
 *  macros make such uses less daunting, but not invisible. */
#define lake_atomic_store(var, value) \
    lake_atomic_store_explicit(var, value, lake_memory_model_relaxed)
#define lake_atomic_read(var) \
    lake_atomic_read_explicit(var, lake_memory_model_relaxed)

#define lake_atomic_compare_exchange(ptr, expected, desired, weak) \
    lake_atomic_compare_exchange_explicit(ptr, expected, desired, weak, \
                                          lake_memory_model_relaxed, \
                                          lake_memory_model_relaxed)

#define lake_atomic_exchange(ptr, value, original) \
    lake_atomic_exchange_explicit(ptr, value, original, lake_memory_model_relaxed)

/** These operands always return the value that had previously been in rmw:
 *     { tmp = *ptr; *ptr op= val; return tmp; }
 *     { tmp = *ptr; *ptr = ~(*ptr & val); return tmp; } // nand */
#define lake_atomic_add(rmw, arg) \
    lake_atomic_add_explicit(rmw, arg, lake_memory_model_relaxed)
#define lake_atomic_sub(rmw, arg) \
    lake_atomic_sub_explicit(rmw, arg, lake_memory_model_relaxed)
#define lake_atomic_and(rmw, arg) \
    lake_atomic_and_explicit(rmw, arg, lake_memory_model_relaxed)
#define lake_atomic_xor(rmw, arg) \
    lake_atomic_xor_explicit(rmw, arg, lake_memory_model_relaxed)
#define lake_atomic_or(rmw, arg) \
    lake_atomic_or_explicit(rmw, arg, lake_memory_model_relaxed)
#define lake_atomic_nand(rmw, arg) \
    lake_atomic_nand_explicit(rmw, arg, lake_memory_model_relaxed)

/** Type 'atomic_spinlock' is defined from within the implementation headers. */
#define lake_atomic_spinlock_try_acquire(spinlock) \
    lake_atomic_spinlock_try_acquire_explicit(spinlock, lake_memory_model_relaxed)
#define lake_atomic_spinlock_acquire(spinlock) \
    lake_atomic_spinlock_acquire_explicit(spinlock, lake_memory_model_acquire)
#define lake_atomic_spinlock_release(spinlock) \
    lake_atomic_spinlock_release_explicit(spinlock, lake_memory_model_release)
