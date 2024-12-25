#ifndef _AMW_SIMD_H
#define _AMW_SIMD_H

#include <lake/bedrock/defines.h>

#if !defined(AMW_SIMD_DISABLED)
    #if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
        #include <lake/bedrock/x86/intrin.h>
    #elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
        #include <lake/bedrock/arm/intrin.h>
    #elif defined(AMW_ARCH_RISCV)
        #include <lake/bedrock/riscv/intrin.h>
    #endif
    #define AMW_SIMD (1)
#else
    #define AMW_SIMD (0)
#endif /* AMW_SIMD */

#ifdef __cplusplus
extern "C" {
#endif

// TODO

#ifdef __cplusplus
}
#endif

#endif /* _AMW_SIMD_H */
