#ifndef _AMW_MATH_TYPES_H
#define _AMW_MATH_TYPES_H

#include <lake/defines.h>
#include <lake/align.h>

typedef int32_t                     ivec2_t[2];
typedef int32_t                     ivec3_t[3];
typedef int32_t                     ivec4_t[4];

typedef float                       vec2_t[2];
typedef float                       vec3_t[3];
typedef AMW_ALIGN_TO(16) float      vec4_t[4];

typedef vec4_t                      quat_t;       /* quaternion */

typedef AMW_ALIGN_TO(16) vec2_t     mat2_t[2];    /* 2x2 matrix */
typedef vec3_t                      mat2x3_t[2];
typedef vec4_t                      mat2x4_t[2];

typedef vec3_t                      mat3_t[3];    /* 3x3 matrix */
typedef vec2_t                      mat3x2_t[3];
typedef vec4_t                      mat3x4_t[3];

typedef AMW_ALIGN_TO_VECTOR vec4_t  mat4_t[4];    /* 4x4 matrix */
typedef vec2_t                      mat4x2_t[4];
typedef vec3_t                      mat4x3_t[4];

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

#define AMW_Ef        ((float)AMW_E)
#define AMW_LOG2Ef    ((float)AMW_LOG2E)
#define AMW_LOG10Ef   ((float)AMW_LOG10E)
#define AMW_LN2f      ((float)AMW_LN2)
#define AMW_LN10f     ((float)AMW_LN10)
#define AMW_PIf       ((float)AMW_PI)
#define AMW_PI_2f     ((float)AMW_PI_2)
#define AMW_PI_4f     ((float)AMW_PI_4)
#define AMW_1_PIf     ((float)AMW_1_PI)
#define AMW_2_PIf     ((float)AMW_2_PI)
#define AMW_2_SQRTPIf ((float)AMW_2_SQRTPI)
#define AMW_SQRT2f    ((float)AMW_SQRT2)
#define AMW_SQRT1_2f  ((float)AMW_SQRT1_2)

#endif /* _AMW_MATH_TYPES_H */
