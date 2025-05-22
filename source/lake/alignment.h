#pragma once

/** @file lake/alignment.h 
 *  @brief TODO docs 
 */
#include "compiler.h"

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
            #define LAKE_ALIGN_MAXIMUM 0
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
#ifndef lake_alignment
    #if LAKE_HAS_ATTRIBUTE(__aligned__)
        #define lake_alignment(alignment) __attribute__((__aligned__(_LAKE_ALIGN_CAP(alignment))))
    #elif defined(LAKE_CC_MSVC_VERSION)
        #define lake_alignment(alignment) __declspec(align(_LAKE_ALIGN_CAP(alignment)))
    #elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
        #define lake_alignment(alignment) _Alignas(_LAKE_ALIGN_CAP(alignment))
    #else
        #define lake_alignment(alignment) /* no alignment */
    #endif
#endif /* lake_alignment */

/** Set alignment rules to match the vector extension registers. */
#ifndef lake_simd_alignment
    #ifdef LAKE_ARCH_X86_AVX
        /* matrix to be used by AVX 256-bit registers */
        #define lake_simd_alignment lake_alignment(32)
    #else
        /* by default use 16-byte alignment rules for SIMD data */ 
        #define lake_simd_alignment lake_alignment(16)
    #endif
#endif /* lake_simd_alignment */

/** Set alignment rules to match the size of a cacheline (64 or 128 bytes). */
#ifndef lake_cacheline_alignment
    #define lake_cacheline_alignment lake_alignment(LAKE_CACHELINE_SIZE)
#endif

#ifndef lake_assume_aligned
    #if LAKE_HAS_BUILTIN(__builtin_assume_aligned)
        #define lake_assume_aligned(EXPR, ALIGNMENT) __builtin_assume_aligned((EXPR), (ALIGNMENT))
    #else
        #define lake_assume_aligned(EXPR, ALIGNMENT) (EXPR)
    #endif
#endif

#define lake_assume_aligned_cast(T, V) \
    ((T *)(lake_assume_aligned((V), lake_static_cast(int32_t,alignof(T)))))

/** Align to a power of 2. */
#define lake_align(x, a)        (((x) + (a)-1) & ~((a)-1))
#define lake_align_any(x, a)    (usize)((((((usize)x) - 1) / ((u32)a)) + 1) * ((u32)a))
