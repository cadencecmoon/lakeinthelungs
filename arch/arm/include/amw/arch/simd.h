/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_ARM_SIMD_H
#define _AMW_ARM_SIMD_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

/* Windows ARM intrinsics */
#if defined(AMW_CC_MSVC_VERSION)
    #if defined(AMW_ARCH_ARM)
        #include <armintr.h>
    #elif defined(AMW_ARCH_AARCH64)
        #include <arm64intr.h>
    #endif
#endif

/* ARM SIMD ISA extensions */
#if defined(AMW_ARCH_ARM)
    #define AMW_ARCH_ARM_NEON AMW_ARCH_ARM
    #include <arm_neon.h>
#elif defined(AMW_ARCH_AARCH64)
    #define AMW_ARCH_ARM_NEON AMW_ARCH_AARCH64
    #include <arm64_neon.h>
#endif
#if defined(__ARM_FEATURE_AES) && __ARM_FEATURE_AES
    #define AMW_ARCH_ARM_AES 1
#endif
#if defined(__ARM_FEATURE_COMPLEX) && __ARM_FEATURE_COMPLEX
    #define AMW_ARCH_ARM_COMPLEX 1
#endif
#if defined(__ARM_FEATURE_CRYPTO) && __ARM_FEATURE_CRYPTO
    #define AMW_ARCH_ARM_CRYPTO 1
#endif
#if defined(__ARM_FEATURE_CRC32) && __ARM_FEATURE_CRC32
    #define AMW_ARCH_ARM_CRC32 1
#endif
#if defined(__ARM_FEATURE_DOTPROD) && __ARM_FEATURE_DOTPROD
    #define AMW_ARCH_ARM_DOTPROD 1
#endif
#if defined(__ARM_FEATURE_FMA) && __ARM_FEATURE_FMA
    #define AMW_ARCH_ARM_FMA 1
#endif
#if defined(__ARM_FEATURE_FP16_FML) && __ARM_FEATURE_FP16_FML
    #define AMW_ARCH_ARM_FP16_FML 1
#endif
#if defined(__ARM_FEATURE_FRINT) && __ARM_FEATURE_FRINT
    #define AMW_ARCH_ARM_FRINT 1
#endif
#if defined(__ARM_FEATURE_MATMUL_INT8) && __ARM_FEATURE_MATMUL_INT8
    #define AMW_ARCH_ARM_MATMUL_INT8 1
#endif
#if defined(__ARM_FEATURE_SHA2) && __ARM_FEATURE_SHA2 && !defined(__APPLE_CC__)
    #define AMW_ARCH_ARM_SHA2 1
#endif
#if defined(__ARM_FEATURE_SHA3) && __ARM_FEATURE_SHA3
    #define AMW_ARCH_ARM_SHA3 1
#endif
#if defined(__ARM_FEATURE_SHA512) && __ARM_FEATURE_SHA512
    #define AMW_ARCH_ARM_SHA512 1
#endif
#if defined(__ARM_FEATURE_SM3) && __ARM_FEATURE_SM3
    #define AMW_ARCH_ARM_SM3 1
#endif
#if defined(__ARM_FEATURE_SM4) && __ARM_FEATURE_SM4
    #define AMW_ARCH_ARM_SM4 1
#endif
#if defined(__ARM_FEATURE_SVE) && __ARM_FEATURE_SVE
    #define AMW_ARCH_ARM_SVE 1
#endif
#if defined(__ARM_FEATURE_QRDMX) && __ARM_FEATURE_QRDMX
    #define AMW_ARCH_ARM_QRDMX 1
#endif

AMW_C_DECL_END
#endif /* _AMW_ARM_SIMD_H */
