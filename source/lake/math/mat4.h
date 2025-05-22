#pragma once

/** @file lake/math/mat4.h
 *  @brief TODO docs
 */
#include "../simd.h"
#include "../log.h"

#ifdef LAKE_ARCH_X86_SSE2
#include "simd/sse2/mat4.h"
#endif
#ifdef LAKE_ARCH_X86_AVX
#include "simd/avx/mat4.h"
#endif

#include "vec4.h"

/** Initializes a mat4 as an identity matrix. */
#define LAKE_MAT4_IDENTITY_INIT {{1.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 1.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 1.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 1.0f}}

/** Initializes a mat4 with zeroes. */
#define LAKE_MAT4_ZERO_INIT     {{0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}, \
                                {0.0f, 0.0f, 0.0f, 0.0f}}

#define LAKE_MAT4_IDENTITY      ((mat4)LAKE_MAT4_IDENTITY_INIT)
#define LAKE_MAT4_ZERO          ((mat4)LAKE_MAT4_ZERO_INIT)

/** Copy all members of mat to dest. The matrix may not be aligned,
 *  u stands for unaligned. This may be useful when copying a matrix 
 *  from an external source (from importing assets, for example). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_ucopy(mat4 m, mat4 dest) 
{
    dest[0][0] = m[0][0];  dest[1][0] = m[1][0];
    dest[0][1] = m[0][1];  dest[1][1] = m[1][1];
    dest[0][2] = m[0][2];  dest[1][2] = m[1][2];
    dest[0][3] = m[0][3];  dest[1][3] = m[1][3];

    dest[2][0] = m[2][0];  dest[3][0] = m[3][0];
    dest[2][1] = m[2][1];  dest[3][1] = m[3][1];
    dest[2][2] = m[2][2];  dest[3][2] = m[3][2];
    dest[2][3] = m[2][3];  dest[3][3] = m[3][3];
}

/** Copy all members of mat to dest. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_copy(mat4 m, mat4 dest) 
{
#if defined(LAKE_ARCH_X86_AVX)
  lake_simd256_write(dest[0], lake_simd256_read(m[0]));
  lake_simd256_write(dest[2], lake_simd256_read(m[2]));
#elif defined(LAKE_ARCH_X86_SSE2)
  lake_simd_write(dest[0], lake_simd_read(m[0]));
  lake_simd_write(dest[1], lake_simd_read(m[1]));
  lake_simd_write(dest[2], lake_simd_read(m[2]));
  lake_simd_write(dest[3], lake_simd_read(m[3]));
#else
  lake_mat4_ucopy(m, dest);
#endif
}

/** Make given matrix an identity. It is easier to use it for member matrices. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_identity(mat4 m) 
{
    lake_simd_alignment mat4 temp = LAKE_MAT4_IDENTITY_INIT;
    lake_mat4_copy(temp, m);
}

/** Make given matrix array's each element identity matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_identity_array(mat4 *restrict m, u32 count)
{
    lake_simd_alignment mat4 temp = LAKE_MAT4_IDENTITY_INIT;
    for (usize i = 0; i < count; lake_mat4_copy(temp, m[i++]));
}

/** Make a given matrix zero. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_zero(mat4 m) 
{
#if defined(LAKE_ARCH_X86_AVX)
    f256 x0;
    x0 = _mm256_setzero_ps();
    lake_simd256_write(m[0], x0);
    lake_simd256_write(m[2], x0);
#elif defined(LAKE_ARCH_X86_SSE2)
    f128 x0;
    x0 = _mm_setzero_ps();
    lake_simd_write(m[0], x0);
    lake_simd_write(m[1], x0);
    lake_simd_write(m[2], x0);
    lake_simd_write(m[3], x0);
#else
    lake_simd_alignment mat4 temp = LAKE_MAT4_ZERO_INIT;
    lake_mat4_copy(temp, m);
#endif
}

/** Scale (multiply with scalar) a matrix without simd optimization. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_scale_p(mat4 m, f32 s) 
{
    m[0][0] *= s; m[0][1] *= s; m[0][2] *= s; m[0][3] *= s;
    m[1][0] *= s; m[1][1] *= s; m[1][2] *= s; m[1][3] *= s;
    m[2][0] *= s; m[2][1] *= s; m[2][2] *= s; m[2][3] *= s;
    m[3][0] *= s; m[3][1] *= s; m[3][2] *= s; m[3][3] *= s;
}

/** Scale (multiply with scalar) a matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_scale(mat4 m, f32 s) 
{
#if defined(LAKE_ARCH_X86_AVX)
    lake_mat4_scale_avx(m, s);
#elif defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_scale_sse2(m, s);
#else
    lake_mat4_scale_p(m, s);
#endif
}

/** Copy upper-left of mat4 to mat3. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_pick3(mat4 m, mat3 dest)
{
    dest[0][0] = m[0][0];
    dest[0][1] = m[0][1];
    dest[0][2] = m[0][2];

    dest[1][0] = m[1][0];
    dest[1][1] = m[1][1];
    dest[1][2] = m[1][2];

    dest[2][0] = m[2][0];
    dest[2][1] = m[2][1];
    dest[2][2] = m[2][2];
}

/** Copy upper-left of mat4 to mat3 (transposed). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_pick3t(mat4 m, mat3 dest)
{
    dest[0][0] = m[0][0];
    dest[0][1] = m[1][0];
    dest[0][2] = m[2][0];

    dest[1][0] = m[0][1];
    dest[1][1] = m[1][1];
    dest[1][2] = m[2][1];

    dest[2][0] = m[0][2];
    dest[2][1] = m[1][2];
    dest[2][2] = m[2][2];
}

/** Copy mat3 to mat4's upper-left. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat3_ins3(mat3 m, mat4 dest)
{
    dest[0][0] = m[0][0];
    dest[0][1] = m[0][1];
    dest[0][2] = m[0][2];

    dest[1][0] = m[1][0];
    dest[1][1] = m[1][1];
    dest[1][2] = m[1][2];

    dest[2][0] = m[2][0];
    dest[2][1] = m[2][1];
    dest[2][2] = m[2][2];
}

/** Multiply m1 and m2 to dest -- m1, m2 and dest can be the same matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_mul(mat4 m1, mat4 m2, mat4 dest) 
{
#if defined(LAKE_ARCH_X86_AVX)
    lake_mat4_mul_avx(m1, m2, dest);
#elif defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_mul_sse2(m1, m2, dest);
#else
    f32 a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
        a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
        a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
        a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

        b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
        b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
        b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
        b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

    dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
#endif
}

/** Multiply N mat4 matrices and store result in dest. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_muln(mat4 *restrict matrices[], u32 n, mat4 dest) 
{
    LAKE_ASSUME(n > 1 && "there must be atleast 2 matrices");
    lake_mat4_mul(*matrices[0], *matrices[1], dest);
    for (u32 i = 2; i < n; lake_mat4_mul(dest, *matrices[i++], dest));
}

/** Multiply mat4 with vec4 (column vector) and store in dest vector. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_mulv(mat4 m, vec4 v, vec4 dest)
{
#if defined(LAKE_ARCH_X86_SSE2)
    lake_mat4_mulv_sse2(m, v, dest);
#else
    vec4 res;
    res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
    lake_vec4_copy(res, dest);
#endif
}

/** Trace of matrix. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE f32 lake_mat4_trace(mat4 m)
{ return m[0][0] + m[1][1] + m[2][2] + m[3][3]; }

/** Trace of matrix (rotation part). */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE f32 lake_mat4_trace3(mat4 m)
{ return m[0][0] + m[1][1] + m[2][2]; }

/** Convert mat4's rotation part to quaternion. */
LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_quat(mat4 m, quat dest)
{
    f32 trace, r, rinv;

    trace = m[0][0] + m[1][1] + m[2][2];
    if (trace >= 0.0f) {
        r = sqrtf(1.0f + trace);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[1][2] - m[2][1]);
        dest[1] = rinv * (m[2][0] - m[0][2]);
        dest[2] = rinv * (m[0][1] - m[1][0]);
        dest[3] =    r * 0.5f;
    } else if (m[0][0] >= m[1][1] && m[0][0] >= m[2][2]) {
        r = sqrtf(1.0f - m[1][1] - m[2][2] + m[0][0]);
        rinv = 0.5f / r;
        dest[0] =    r * 0.5f;
        dest[1] = rinv * (m[0][1] - m[1][0]);
        dest[2] = rinv * (m[0][2] - m[2][0]);
        dest[3] = rinv * (m[1][2] - m[2][1]);
    } else if (m[1][1] >= m[2][2]) {
        r = sqrtf(1.0f - m[0][0] - m[2][2] + m[1][1]);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[0][1] - m[1][0]);
        dest[1] =    r * 0.5f;
        dest[2] = rinv * (m[1][2] - m[2][1]);
        dest[3] = rinv * (m[2][0] - m[0][2]);
    } else {
        r = sqrtf(1.0f - m[0][0] - m[1][1] + m[2][2]);
        rinv = 0.5f / r;
        dest[0] = rinv * (m[0][2] - m[2][0]);
        dest[1] = rinv * (m[1][2] - m[2][1]);
        dest[2] =    r * 0.5f;
        dest[1] = rinv * (m[0][1] - m[1][0]);
    }
}
