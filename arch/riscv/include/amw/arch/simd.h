/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_ARCH_RISCV_SIMD_H
#define _AMW_ARCH_RISCV_SIMD_H

#include <amw/common/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RISC-V SIMD ISA extensions */
#if defined(__riscv_zve32x)
    #define AMW_ARCH_RISCV_ZVE32X 1
#endif
#if defined(__riscv_zve32f)
    #define AMW_ARCH_RISCV_ZVE32F 1
#endif
#if defined(__riscv_zve64x)
    #define AMW_ARCH_RISCV_ZVE64X 1
#endif
#if defined(__riscv_zve64f)
    #define AMW_ARCH_RISCV_ZVE64F 1
#endif
#if defined(__riscv_zve64d)
    #define AMW_ARCH_RISCV_ZVE64D 1
#endif
#if defined(__riscv_v)
    #define AMW_ARCH_RISCV_V 1
#endif
#if defined(__riscv_zvfh)
    #define AMW_ARCH_RISCV_ZVFH 1
#endif
#if defined(__riscv_zvfhmin)
    #define AMW_ARCH_RISCV_ZVFHMIN 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ARCH_RISCV_SIMD_H */
