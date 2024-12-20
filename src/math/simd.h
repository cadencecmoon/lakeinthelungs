#ifndef _AMW_SIMD_H
#define _AMW_SIMD_H

#include <base/defines.h>

#if !defined(AMW_SIMD_DISABLED)
    #if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
        #include <arch/x86/intrin.h>
    #elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
        #include <arch/arm/intrin.h>
    #elif defined(AMW_ARCH_RISCV)
        #include <arch/riscv/intrin.h>
    #endif
    #define AMW_SIMD (1)
#else
    #define AMW_SIMD (0)
#endif /* AMW_SIMD */

#endif /* _AMW_SIMD_H */
