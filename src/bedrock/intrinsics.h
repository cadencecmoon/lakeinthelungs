#pragma once

#include "defines.h"

#ifdef __cplusplus
extern "C" {
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

/* https://en.wikipedia.org/wiki/MIPS_architecture */
#if defined(_MIPS_ISA_MIPS64R2)
    #define ARCH_MIPS 642
#elif defined(_MIPS_ISA_MIPS64)
    #define ARCH_MIPS 640
#elif defined(_MIPS_ISA_MIPS32R2)
    #define ARCH_MIPS 322
#elif defined(_MIPS_ISA_MIPS32)
    #define ARCH_MIPS 320
#elif defined(_MIPS_ISA_MIPS4)
    #define ARCH_MIPS 4
#elif defined(_MIPS_ISA_MIPS3)
    #define ARCH_MIPS 3
#elif defined(_MIPS_ISA_MIPS2)
    #define ARCH_MIPS 2
#elif defined(_MIPS_ISA_MIPS1)
    #define ARCH_MIPS 1
#elif defined(_MIPS_ISA_MIPS) || defined(__mips) || defined(__MIPS__)
    #define ARCH_MIPS 1
#endif
#if defined(ARCH_MIPS)
    #define ARCH_MIPS_CHECK(ver) ((ver) <= ARCH_MIPS)
#else
    #define ARCH_MIPS_CHECK(ver) (0)
#endif
#if defined(__mips_loongson_mmi)
    #define ARCH_MIPS_LOONGSON_MMI 1
#endif
#if defined(__mips_msa)
    #define ARCH_MIPS_MSA 1
#endif

/* https://en.wikipedia.org/wiki/Loongson#LoongArch */
#if defined(__loongarch32)
    #define ARCH_LOONGARCH 1
#elif defined(__loongarch64)
    #define ARCH_LOONGARCH 2
#endif
/* LSX: LoongArch 128-bits SIMD extension */
#if defined(__loongarch_sx)
    #define ARCH_LOONGARCH_LSX 1
    #include <lsxintrin.h>
#endif
/* LASX: LoongArch 256-bits SIMD extension */
#if defined(__loongarch_asx)
    #define ARCH_LOONGARCH_LASX 2
    #include <lasxintrin.h>
#endif

/* https://en.wikipedia.org/wiki/SPARC */
#if defined(__sparc_v9__) || defined(__sparcv9)
    #define ARCH_SPARC 9
#elif defined(__sparc_v8__) || defined(__sparcv8)
    #define ARCH_SPARC 8
#elif defined(__sparc_v7__) || defined(__sparcv7)
    #define ARCH_SPARC 7
#elif defined(__sparc_v6__) || defined(__sparcv6)
    #define ARCH_SPARC 6
#elif defined(__sparc_v5__) || defined(__sparcv5)
    #define ARCH_SPARC 5
#elif defined(__sparc_v4__) || defined(__sparcv4)
    #define ARCH_SPARC 4
#elif defined(__sparc_v3__) || defined(__sparcv3)
    #define ARCH_SPARC 3
#elif defined(__sparc_v2__) || defined(__sparcv2)
    #define ARCH_SPARC 2
#elif defined(__sparc_v1__) || defined(__sparcv1)
    #define ARCH_SPARC 1
#elif defined(__sparc__) || defined(__sparc)
    #define ARCH_SPARC 1
#endif
#if defined(ARCH_SPARC)
    #define ARCH_SPARC_CHECK(ver) ((ver) <= ARCH_SPARC)
#else
    #define ARCH_SPARC_CHECK(ver) (0)
#endif

/* https://en.wikipedia.org/wiki/IBM_POWER_Instruction_Set_Architecture */
#if defined(_M_PPC)
    #define ARCH_POWER _M_PPC
#elif defined(_ARCH_PWR9)
    #define ARCH_POWER 900
#elif defined(_ARCH_PWR8)
    #define ARCH_POWER 800
#elif defined(_ARCH_PWR7)
    #define ARCH_POWER 700
#elif defined(_ARCH_PWR6)
    #define ARCH_POWER 600
#elif defined(_ARCH_PWR5)
    #define ARCH_POWER 500
#elif defined(_ARCH_PWR4)
    #define ARCH_POWER 400
#elif defined(_ARCH_440) || defined(__ppc440__)
    #define ARCH_POWER 440
#elif defined(_ARCH_450) || defined(__ppc450__)
    #define ARCH_POWER 450
#elif defined(_ARCH_601) || defined(__ppc601__)
    #define ARCH_POWER 601
#elif defined(_ARCH_603) || defined(__ppc603__)
    #define ARCH_POWER 603
#elif defined(_ARCH_604) || defined(__ppc604__)
    #define ARCH_POWER 604
#elif defined(_ARCH_605) || defined(__ppc605__)
    #define ARCH_POWER 605
#elif defined(_ARCH_620) || defined(__ppc620__)
    #define ARCH_POWER 620
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC) || defined(__ppc)
    #define ARCH_POWER 1
#endif
#if defined(ARCH_POWER)
    #define ARCH_POWER_CHECK(ver) ((ver) <= ARCH_POWER)
#else
    #define ARCH_POWER_CHECK(ver) (0)
#endif
#if defined(__ALTIVEC__)
    #define ARCH_POWER_ALTIVEC ARCH_POWER
    #define ARCH_POWER_ALTIVEC_CHECK(ver) ((ver) <= ARCH_POWER)
#else
    #define ARCH_POWER_ALTIVEC_CHECK(ver) (0)
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

#ifdef __cplusplus
}
#endif
