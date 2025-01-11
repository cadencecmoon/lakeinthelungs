/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_MATH_TYPES_H
#define _AMW_MATH_TYPES_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/align.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s32                         svec2[2];
typedef s32                         svec3[3];
typedef s32                         svec4[4];

typedef f32                         vec2[2];
typedef f32                         vec3[3];
typedef AMW_ALIGN_TO(16) f32        vec4[4];

typedef vec4                        quat;       /* quaternion */

typedef AMW_ALIGN_TO(16) vec2       mat2[2];    /* 2x2 matrix */
typedef vec3                        mat2x3[2];
typedef vec4                        mat2x4[2];

typedef vec3                        mat3[3];    /* 3x3 matrix */
typedef vec2                        mat3x2[3];
typedef vec4                        mat3x4[3];

typedef AMW_ALIGN_TO_VECTOR vec4    mat4[4];    /* 4x4 matrix */
typedef vec2                        mat4x2[4];
typedef vec3                        mat4x3[4];

#define AMW_EPSILON   1e-5f

#define AMW_E         2.71828182845904523536028747135266250   /* e           */
#define AMW_LOG2E     1.44269504088896340735992468100189214   /* log2(e)     */
#define AMW_LOG10E    0.434294481903251827651128918916605082  /* log10(e)    */
#define AMW_LN2       0.693147180559945309417232121458176568  /* loge(2)     */
#define AMW_LN10      2.30258509299404568401799145468436421   /* loge(10)    */
#define AMW_PI        3.14159265358979323846264338327950288   /* pi          */
#define AMW_PI_2      1.57079632679489661923132169163975144   /* pi/2        */
#define AMW_PI_4      0.785398163397448309615660845819875721  /* pi/4        */
#define AMW_1_PI      0.318309886183790671537767526745028724  /* 1/pi        */
#define AMW_2_PI      0.636619772367581343075535053490057448  /* 2/pi        */
#define AMW_2_SQRTPI  1.12837916709551257389615890312154517   /* 2/sqrt(pi)  */
#define AMW_SQRT2     1.41421356237309504880168872420969808   /* sqrt(2)     */
#define AMW_SQRT1_2   0.707106781186547524400844362104849039  /* 1/sqrt(2)   */

#define AMW_Ef        ((f32)AMW_E)
#define AMW_LOG2Ef    ((f32)AMW_LOG2E)
#define AMW_LOG10Ef   ((f32)AMW_LOG10E)
#define AMW_LN2f      ((f32)AMW_LN2)
#define AMW_LN10f     ((f32)AMW_LN10)
#define AMW_PIf       ((f32)AMW_PI)
#define AMW_PI_2f     ((f32)AMW_PI_2)
#define AMW_PI_4f     ((f32)AMW_PI_4)
#define AMW_1_PIf     ((f32)AMW_1_PI)
#define AMW_2_PIf     ((f32)AMW_2_PI)
#define AMW_2_SQRTPIf ((f32)AMW_2_SQRTPI)
#define AMW_SQRT2f    ((f32)AMW_SQRT2)
#define AMW_SQRT1_2f  ((f32)AMW_SQRT1_2)

#ifdef __cplusplus
}
#endif

#endif /* _AMW_MATH_TYPES_H */
