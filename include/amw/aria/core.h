#pragma once

#include <amw/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s32                     svec2[2];
typedef s32                     svec3[3];
typedef s32                     svec4[4];

typedef f32                     vec2[2];
typedef f32                     vec3[3];
typedef attr_aligned(16) f32    vec4[4];

typedef vec4                    quat;       /* quaternion */

typedef attr_aligned(16) vec2   mat2[2];    /* 2x2 matrix */
typedef vec3                    mat2x3[2];
typedef vec4                    mat2x4[2];

typedef vec3                    mat3[3];    /* 3x3 matrix */
typedef vec2                    mat3x2[3];
typedef vec4                    mat3x4[3];

typedef attr_aligned(32) vec4   mat4[4];    /* 4x4 matrix */
typedef vec2                    mat4x2[4];
typedef vec3                    mat4x3[4];

#define ARIA_EPSILON    DBL_EPSILON
#define ARIA_EPSILONf   FLT_EPSILON

#define ARIA_E          2.71828182845904523536 /* e          */
#define ARIA_LOG2E      1.44269504088896340735 /* log2(e)    */
#define ARIA_LOG10E     0.43429448190325182765 /* log10(e)   */
#define ARIA_LN2        0.69314718055994530941 /* loge(2)    */
#define ARIA_LN10       2.30258509299404568401 /* loge(10)   */
#define ARIA_TAU        6.28318530717958647692 /* 2pi        */
#define ARIA_INV_TAU    0.15915494309189533577 /* 1/2pi      */
#define ARIA_PI         3.14159265358979323846 /* pi         */
#define ARIA_PI_2       1.57079632679489661923 /* pi/2       */
#define ARIA_PI_4       0.78539816339744830961 /* pi/4       */
#define ARIA_1_PI       0.31830988618379067153 /* 1/pi       */
#define ARIA_2_PI       0.63661977236758134307 /* 2/pi       */
#define ARIA_2_SQRTPI   1.12837916709551257389 /* 2/sqrt(pi) */
#define ARIA_SQRT2      1.41421356237309504880 /* sqrt(2)    */
#define ARIA_SQRT1_2    0.70710678118654752440 /* 1/sqrt(2)  */

#define ARIA_Ef         ((f32)ARIA_E)
#define ARIA_LOG2Ef     ((f32)ARIA_LOG2E)
#define ARIA_LOG10Ef    ((f32)ARIA_LOG10E)
#define ARIA_LN2f       ((f32)ARIA_LN2)
#define ARIA_LN10f      ((f32)ARIA_LN10)
#define ARIA_TAUf       ((f32)ARIA_TAU)
#define ARIA_INV_TAUf   ((f32)ARIA_INV_TAU)
#define ARIA_PIf        ((f32)ARIA_PI)
#define ARIA_PI_2f      ((f32)ARIA_PI_2)
#define ARIA_PI_4f      ((f32)ARIA_PI_4)
#define ARIA_1_PIf      ((f32)ARIA_1_PI)
#define ARIA_2_PIf      ((f32)ARIA_2_PI)
#define ARIA_2_SQRTPIf  ((f32)ARIA_2_SQRTPI)
#define ARIA_SQRT2f     ((f32)ARIA_SQRT2)
#define ARIA_SQRT1_2f   ((f32)ARIA_SQRT1_2)

typedef _Complex float      cmplxf;
typedef _Complex double     cmplx;

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

attr_inline void aria_force_evalf(f32 x) {
    volatile f32 y; y = x; (void)y;
}
attr_inline void aria_force_eval(f64 x) {
    volatile f64 y; y = x; (void)y;
}
attr_inline void aria_force_evall(long double x) {
    volatile long double y; y = x; (void)y;
}
#define ARIA_FORCE_EVAL(x) do {                 \
        if (sizeof(x) == sizeof(f32)) {         \
            aria_force_evalf(x);                \
        } else if (sizeof(x) == sizeof(f64)) {  \
            aria_force_eval(x);                 \
        } else {                                \
            aria_force_evall(x);                \
        }                                       \
    } while(0)

AMWAPI f32 attr_const AMWCALL aria_floorf(f32 x);

/** Fast approximation of Sin function. */
AMWAPI f32 attr_hot attr_const AMWCALL aria_sinf(f32 x);
AMWAPI f32 attr_hot attr_const AMWCALL aria_cosf(f32 x);

#ifdef __cplusplus
}
#endif
