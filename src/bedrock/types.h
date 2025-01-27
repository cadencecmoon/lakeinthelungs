#pragma once 

#include "defines.h"
#include "align.h"

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <complex.h>
#include <limits.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t                 b32;

typedef int8_t                  s8;
typedef int16_t                 s16;
typedef int32_t                 s32;
typedef int64_t                 s64;

typedef uint8_t                 u8;
typedef uint16_t                u16;
typedef uint32_t                u32;
typedef uint64_t                u64;

typedef float                   f32;
typedef double                  f64;

typedef intptr_t                sptr;
typedef uintptr_t               uptr;
typedef ptrdiff_t               ssize;
typedef size_t                  usize;

typedef _Complex float          cmplxf;
typedef _Complex double         cmplx;

typedef s32                     svec2[2];
typedef s32                     svec3[3];
typedef s32                     svec4[4];

typedef f32                     vec2[2];
typedef f32                     vec3[3];
typedef ALIGN_TO(16) f32        vec4[4];

typedef vec4                    quat;       /* quaternion */

typedef ALIGN_TO(16) vec2       mat2[2];    /* 2x2 matrix */
typedef vec3                    mat2x3[2];
typedef vec4                    mat2x4[2];

typedef vec3                    mat3[3];    /* 3x3 matrix */
typedef vec2                    mat3x2[3];
typedef vec4                    mat3x4[3];

typedef ALIGN_TO_VECTOR vec4    mat4[4];    /* 4x4 matrix */
typedef vec2                    mat4x2[4];
typedef vec3                    mat4x3[4];

#ifndef HAS_BUILTIN_COMPLEX
    #if HAS_BUILTIN(__builtin_complex)
        #define HAS_BUILTIN_COMPLEX 1
    #elif CC_GNUC_VERSION_CHECK(4,7,0)
        #define HAS_BUILTIN_COMPLEX 1
    #endif
#endif

#ifdef CC_CLANG_VERSION
    /** On these platforms CMPLX is defined without __extension__, causing false warnings. */
    #if defined(PLATFORM_EMSCRIPTEN) || defined(PLATFORM_APPLE)
        #undef CMPLX
        #undef CMPLXF
    #endif
#endif

#if !defined(CMPLX)
    #if defined(HAS_BUILTIN_COMPLEX)
        #define CMPLX(re, im) __builtin_complex((double)(re), (double)(im))
    #elif defined(CC_CLANG_VERSION)
        #define CMPLX(re, im) (__extension__ (_Complex double){ (double)(re), (double)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Imaginary_I * (double)(im))
    #else
        #define CMPLX(re, im) (_Complex double)((double)(re) + _Complex_I * (double)(im))
    #endif
#endif

#if !defined(CMPLXF)
    #if defined(HAS_BUILTIN_COMPLEX)
        #define CMPLXF(re, im) __builtin_complex((float)(re), (float)(im))
    #elif defined(CC_CLANG_VERSION)
        #define CMPLXF(re, im) (__extension__ (_Complex float){ (float)(re), (float)(im) })
    #elif defined(_Imaginary_I)
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Imaginary_I * (float)(im))
    #else
        #define CMPLXF(re, im) (_Complex float)((float)(re) + _Complex_I * (float)(im))
    #endif
#endif

#undef EPSILON
#undef EPSILONf
#define EPSILON     DBL_EPSILON
#define EPSILONf    FLT_EPSILON

#undef M_E
#undef M_LOG2E
#undef M_LOG10E
#undef M_E
#undef M_LOG2E
#undef M_LOG10E
#undef M_LN2
#undef M_LN10
#undef M_PI
#undef M_PI_2
#undef M_PI_4
#undef M_1_PI
#undef M_2_PI
#undef M_2_SQRTPI
#undef M_SQRT2
#undef M_SQRT1_2

#define M_E         2.71828182845904523536028747135266250   /* e           */
#define M_LOG2E     1.44269504088896340735992468100189214   /* log2(e)     */
#define M_LOG10E    0.434294481903251827651128918916605082  /* log10(e)    */
#define M_LN2       0.693147180559945309417232121458176568  /* loge(2)     */
#define M_LN10      2.30258509299404568401799145468436421   /* loge(10)    */
#define M_PI        3.14159265358979323846264338327950288   /* pi          */
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2        */
#define M_PI_4      0.785398163397448309615660845819875721  /* pi/4        */
#define M_1_PI      0.318309886183790671537767526745028724  /* 1/pi        */
#define M_2_PI      0.636619772367581343075535053490057448  /* 2/pi        */
#define M_2_SQRTPI  1.12837916709551257389615890312154517   /* 2/sqrt(pi)  */
#define M_SQRT2     1.41421356237309504880168872420969808   /* sqrt(2)     */
#define M_SQRT1_2   0.707106781186547524400844362104849039  /* 1/sqrt(2)   */

#undef M_Ef
#undef M_LOG2Ef
#undef M_LOG10Ef
#undef M_LN2f
#undef M_LN10f
#undef M_PIf
#undef M_PI_2f
#undef M_PI_4f
#undef M_1_PIf
#undef M_2_PIf
#undef M_2_SQRTPIf
#undef M_SQRT2f
#undef M_SQRT1_2f

#define M_Ef        ((f32)M_E)
#define M_LOG2Ef    ((f32)M_LOG2E)
#define M_LOG10Ef   ((f32)M_LOG10E)
#define M_LN2f      ((f32)M_LN2)
#define M_LN10f     ((f32)M_LN10)
#define M_PIf       ((f32)M_PI)
#define M_PI_2f     ((f32)M_PI_2)
#define M_PI_4f     ((f32)M_PI_4)
#define M_1_PIf     ((f32)M_1_PI)
#define M_2_PIf     ((f32)M_2_PI)
#define M_2_SQRTPIf ((f32)M_2_SQRTPI)
#define M_SQRT2f    ((f32)M_SQRT2)
#define M_SQRT1_2f  ((f32)M_SQRT1_2)

/** Codes to interpret the result of some work. */
enum result {
    result_success = 0,             /**< No errors, work was done as intended. */
    result_failure,                 /**< A commonly used error, to indicate errors on a success/failure basis. */
    result_work_ignored,            /**< The work execution was ignored. */
    result_feature_unsupported,     /**< This feature or other backend implementation is unsupported for this build or host environment. */

    result_missing_library,         /**< Could not load a shared library module. */
    result_missing_procedures,      /**< Could not get all procedure addresses. */

    result_out_of_memory,           /**< Failed to satisfy an allocation, ran out of reserved memory space. */
    result_host_memory_failure,     /**< Ran out of RAM, either we hit the SWAP space or the system is close to a fatal failure if any new allocations are done. */
    result_device_memory_failure,   /**< Ran out of VRAM on the GPU. New allocations are not possible until memory is freed. */
    result_memory_fragmented,       /**< An allocation failed due to fragmented free memory blocks/pages, but would succeed otherwise. */

    result_max_enum = INT32_MAX,
};

/** The execution context of a work argument. Procedures may interpret the different contexts as they wish,
 *  or completely ignore it's value. For example, a procedure may not modify any state, and the assemble-like
 *  context may be ignored. */
enum execution {
    execution_continue = 0,     /**< The procedure is responsible for interpreting the given work argument. The default context. */
    execution_assemble,         /**< Create new state at destination. If it exists, the work is ignored. */
    execution_reassemble,       /**< Cleanup and recreate existing state. If it doesn't yet exist, it will be created. */
    execution_disassemble,      /**< All related state is destroyed, after this point the state is considered invalid. */
    execution_max_enum = INT32_MAX,
};

/** A base structure that defines members shared between all work argument structures. */
struct work {
    s32 result;     /**< enum result */
    s32 execution;  /**< enum execution */
};

/** Quickly define common work members in a specialized work argument structure. It's so that 
 *  we can cast any work used in parallel, to interpret the results of such work for example. */
#define WORK_STRUCTURE_HEADER               \
    s32 result;     /**< enum result */     \
    s32 execution;  /**< enum execution */

#ifdef __cplusplus
}
#endif
