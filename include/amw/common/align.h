/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_ALIGN_H
#define _AMW_ALIGN_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

#ifndef AMW_ALIGN_MAXIMUM
    #if defined(AMW_CC_MSVC_VERSION) && AMW_CC_MSVC_VERSION_CHECK(19,16,0)
        /* Visual Studio 2017 or newer don't need an alignment cap */
        #if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64) || defined(AMW_ARCH_E2K)
            #if AMW_CC_MSVC_VERSION_CHECK(19,14,0)
                #define AMW_ALIGN_MAXIMUM (64)
            #elif AMW_CC_MSVC_VERSION_CHECK(16,0,0)
                #define AMW_ALIGN_MAXIMUM (32)
            #else
                #define AMW_ALIGN_MAXIMUM (16)
            #endif
        #elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
            #define AMW_ALIGN_MAXIMUM (8)
        #endif
    #else
        #define AMW_ALIGN_MAXIMUM (0) /* no cap ! */
    #endif
#endif

#if AMW_ALIGN_MAXIMUM
    #define _AMW_ALIGN_CAP(alignment) (((alignment) < (AMW_ALIGN_MAXIMUM)) ? (alignment) : (AMW_ALIGN_MAXIMUM))
#else
    #define _AMW_ALIGN_CAP(alignment) (alignment)
#endif

#if defined(AMW_CC_GNUC_VERSION) && __has_attribute(__aligned__)
    #define AMW_ALIGN_TO(alignment) __attribute__((__aligned__(_AMW_ALIGN_CAP(alignment))))
#elif defined(AMW_CC_MSVC_VERSION)
    #define AMW_ALIGN_TO(alignment) __declspec(align(_AMW_ALIGN_CAP(alignment)))
#else
    #define AMW_ALIGN_TO(alignment) /* no alignment */
#endif

/* to be used by AVX 256-bit registers */
#ifndef AMW_ALIGN_MATRIX
    #ifdef AMW_ARCH_X86_AVX
        #define AMW_ALIGN_MATRIX AMW_ALIGN_TO(32)
    #else
        #define AMW_ALIGN_MATRIX AMW_ALIGN_TO(16)
    #endif
#endif


#if AMW_HAS_BUILTIN(__builtin_assume_aligned) || AMW_CC_GNUC_VERSION_CHECK(4,7,0)
    #define AMW_ASSUME_ALIGNED(expr, alignment) \
        __builtin_assume_aligned((expr), (alignment))
#else
    #define AMW_ASSUME_ALIGNED(expr, alignment) (expr)
#endif

#ifdef AMW_CC_MSVC_VERSION
    #define AMW_CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type *)(AMW_ASSUME_ALIGNED((expr), __alignof(type))))
#else
    #define AMW_CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type *)(AMW_ASSUME_ALIGNED((expr), __alignof__(type))))
#endif

AMW_C_DECL_END
#endif /* _AMW_ALIGN_H */
