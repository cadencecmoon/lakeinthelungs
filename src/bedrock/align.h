#pragma once

#include "defines.h"
#include "intrinsics.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ALIGN_MAXIMUM
    #if defined(CC_MSVC_VERSION) && CC_MSVC_VERSION_CHECK(19,16,0)
        /* Visual Studio 2017 or newer don't need an alignment cap */
        #if defined(ARCH_X86) || defined(ARCH_AMD64) 
            #if CC_MSVC_VERSION_CHECK(19,14,0)
                #define _ALIGN_MAXIMUM (64)
            #elif CC_MSVC_VERSION_CHECK(16,0,0)
                #define _ALIGN_MAXIMUM (32)
            #else
                #define _ALIGN_MAXIMUM (16)
            #endif
        #elif defined(ARCH_ARM) || defined(_ARCH_AARCH64)
            #define _ALIGN_MAXIMUM (8)
        #endif
    #else
        #define _ALIGN_MAXIMUM (0)
    #endif
#endif

#if _ALIGN_MAXIMUM
    #define _ALIGN_CAP(alignment) (((alignment) < (_ALIGN_MAXIMUM)) ? (alignment) : (_ALIGN_MAXIMUM))
#else
    #define _ALIGN_CAP(alignment) (alignment)
#endif

#if defined(CC_GNUC_VERSION) && HAS_ATTRIBUTE(__aligned__)
    #define ALIGN_TO(alignment) __attribute__((__aligned__(_ALIGN_CAP(alignment))))
#elif defined(CC_MSVC_VERSION)
    #define ALIGN_TO(alignment) __declspec(align(_ALIGN_CAP(alignment)))
#else
    #define ALIGN_TO(alignment) /* no alignment */
#endif

/* to be used by AVX 256-bit registers */
#ifndef ALIGN_TO_VECTOR
    #ifdef ARCH_X86_AVX
        #define ALIGN_TO_VECTOR ALIGN_TO(32)
    #else
        #define ALIGN_TO_VECTOR ALIGN_TO(16)
    #endif
#endif

#if HAS_BUILTIN(__builtin_assume_aligned) || CC_GNUC_VERSION_CHECK(4,7,0)
    #define ASSUME_ALIGNED(expr, alignment) __builtin_assume_aligned((expr), (alignment))
#else
    #define ASSUME_ALIGNED(expr, alignment) (expr)
#endif

#ifdef CC_MSVC_VERSION
    #define CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type *)(ASSUME_ALIGNED((expr), __alignof(type))))
#else
    #define CASTPTR_ASSUME_ALIGNED(expr, type) \
        ((type *)(ASSUME_ALIGNED((expr), __alignof__(type))))
#endif

#ifdef __cplusplus
}
#endif
