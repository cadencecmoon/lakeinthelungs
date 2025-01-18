/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_X86_INTRIN_H
#define _AMW_X86_INTRIN_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SIMD ISA extensions for intel x86 and amd64 */
#if defined(_M_IX86_FP)
    #define AMW_ARCH_X86_MMX 1
    #include <mmintrin.h>
    #if (_M_IX86_FP >= 1)
        #define AMW_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if (_M_IX86_FP >= 2)
        #define AMW_ARCH_X86_SSE 1
        #define AMW_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#elif defined(_M_X64)
    #define AMW_ARCH_X86_SSE 1
    #define AMW_ARCH_X86_SSE2 1
    #include <emmintrin.h>
#else
    #if defined(__MMX__)
        #define AMW_ARCH_X86_MMX 1
        #include <mmintrin.h>
    #endif
    #if defined(__SSE__)
        #define AMW_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if defined(__SSE2__)
        #define AMW_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#endif
#if defined(__SSE3__)
    #define AMW_ARCH_X86_SSE3 1
    #include <pmmintrin.h>
#endif
#if defined(__SSSE3__)
    #define AMW_ARCH_X86_SSSE3 1
    #include <tmmintrin.h>
#endif
#if defined(__SSE4_1__)
    #define AMW_ARCH_X86_SSE4_1 1
    #include <smmintrin.h>
#endif
#if defined(__SSE4_2__)
    #define AMW_ARCH_X86_SSE4_2 1
    #include <nmmintrin.h>
#endif
#if defined(__SSE4A__)
    #define AMW_ARCH_X86_SSE4A 1
    #include <ammintrin.h>
#endif
#if defined(__XOP__)
    #define AMW_ARCH_X86_XOP 1
    #include <xopintrin.h>
#endif
#if defined(__AVX__)
    #define AMW_ARCH_X86_AVX 1
    #if !defined(AMW_ARCH_X86_SSE3)
        #define AMW_ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if !defined(AMW_ARCH_X86_SSE4_1)
        #define AMW_ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if !defined(AMW_ARCH_X86_SSE4_2)
        #define AMW_ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX2__)
    #define AMW_ARCH_X86_AVX2 1
    #if defined(AMW_CC_MSVC_VERSION)
        #ifndef __FMA__
            #define __FMA__ 1
        #endif
    #endif
    #include <immintrin.h>
#endif
#if defined(__FMA__)
    #define AMW_ARCH_X86_FMA 1
    #if !defined(AMW_ARCH_X86_AVX)
        #define AMW_ARCH_X86_AVX 1
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX512VP2INTERSECT__)
    #define AMW_ARCH_X86_AVX512VP2INTERSECT 1
#endif
#if defined(__AVX512BITALG__)
    #define AMW_ARCH_X86_AVX512BITALG 1
#endif
#if defined(__AVX512VPOPCNTDQ__)
    #define AMW_ARCH_X86_AVX512VPOPCNTDQ 1
#endif
#if defined(__AVX512VBMI__)
    #define AMW_ARCH_X86_AVX512VBMI 1
#endif
#if defined(__AVX512VBMI2__)
    #define AMW_ARCH_X86_AVX512VBMI2 1
#endif
#if defined(__AVX512VNNI__)
    #define AMW_ARCH_X86_AVX512VNNI 1
#endif
#if defined(__AVX5124VNNIW__)
    #define AMW_ARCH_X86_AVX5124VNNIW 1
#endif
#if defined(__AVX512BW__)
    #define AMW_ARCH_X86_AVX512BW 1
#endif
#if defined(__AVX512BF16__)
    #define AMW_ARCH_X86_AVX512BF16 1
#endif
#if defined(__AVX512CD__)
    #define AMW_ARCH_X86_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
    #define AMW_ARCH_X86_AVX512DQ 1
#endif
#if defined(__AVX512F__)
    #define AMW_ARCH_X86_AVX512F 1
#endif
#if defined(__AVX512VL__)
    #define AMW_ARCH_X86_AVX512VL 1
#endif
#if defined(__AVX512FP16__)
    #define AMW_ARCH_X86_AVX512FP16 1
#endif
#if defined(__GFNI__)
    #define AMW_ARCH_X86_GFNI 1
#endif
#if defined(__PCLMUL__)
    #define AMW_ARCH_X86_PCLMUL 1
#endif
#if defined(__VPCLMULQDQ__)
    #define AMW_ARCH_X86_VPCLMULQDQ 1
#endif
#if (defined(__F16C__) || AMW_CC_MSVC_VERSION_CHECK(19,30,0)) && defined(AMW_ARCH_X86_AVX2)
    #define AMW_ARCH_X86_F16C 1
#endif
#if defined(__AES__)
    #define AMW_ARCH_X86_AES 1
#endif

#if AMW_CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef AMW_CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            AMW_INLINE void __attribute__((__always_inline__, __nodebug__))
            _m_prefetch(void *__P) {
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif AMW_CC_MINGW_VERSION_CHECK(4,0,0)
    #include <intrin.h>
#endif

/* do not use aligned SIMD instructions for older visual studio versions */
#if defined(AMW_CC_MSVC_VERSION) && !AMW_CC_MSVC_VERSION_CHECK(19,13,0)
    /* Visual Studio 2017 version 15.6 */
    #if !defined(AMW_SIMD_UNALIGNED) && !defined(AMW_SIMD_DISABLED)
        #define AMW_SIMD_UNALIGNED 1
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AMW_X86_INTRIN_H */
