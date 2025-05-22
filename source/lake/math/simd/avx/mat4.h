#pragma once

#include "../../../simd.h"
#if defined(LAKE_ARCH_X86_AVX)

LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_scale_avx(mat4 m, f32 s) 
{
    f256 y0;
    y0 = _mm256_set1_ps(s);
    lake_simd256_write(m[0], _mm256_mul_ps(y0, lake_simd256_read(m[0])));
    lake_simd256_write(m[2], _mm256_mul_ps(y0, lake_simd256_read(m[2])));
}

LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_mat4_mul_avx(mat4 m1, mat4 m2, mat4 dest) 
{
    /* D = R * L (column-major) */
    f256 y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;

    y0 = lake_simd256_read(m2[0]); /* h g f e d c b a */
    y1 = lake_simd256_read(m2[2]); /* p o n m l k j i */

    y2 = lake_simd256_read(m1[0]); /* h g f e d c b a */
    y3 = lake_simd256_read(m1[2]); /* p o n m l k j i */

    /* 0x03: 0b00000011 */
    y4 = _mm256_permute2f128_ps(y2, y2, 0x03); /* d c b a h g f e */
    y5 = _mm256_permute2f128_ps(y3, y3, 0x03); /* l k j i p o n m */

    /* f f f f a a a a */
    /* h h h h c c c c */
    /* e e e e b b b b */
    /* g g g g d d d d */
    y6 = _mm256_permutevar_ps(y0, _mm256_set_epi32(1, 1, 1, 1, 0, 0, 0, 0));
    y7 = _mm256_permutevar_ps(y0, _mm256_set_epi32(3, 3, 3, 3, 2, 2, 2, 2));
    y8 = _mm256_permutevar_ps(y0, _mm256_set_epi32(0, 0, 0, 0, 1, 1, 1, 1));
    y9 = _mm256_permutevar_ps(y0, _mm256_set_epi32(2, 2, 2, 2, 3, 3, 3, 3));

    lake_simd256_write(dest[0], 
        _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(y2, y6),
                                    _mm256_mul_ps(y3, y7)),
                      _mm256_add_ps(_mm256_mul_ps(y4, y8),
                                    _mm256_mul_ps(y5, y9))));

    /* n n n n i i i i */
    /* p p p p k k k k */
    /* m m m m j j j j */
    /* o o o o l l l l */
    y6 = _mm256_permutevar_ps(y1, _mm256_set_epi32(1, 1, 1, 1, 0, 0, 0, 0));
    y7 = _mm256_permutevar_ps(y1, _mm256_set_epi32(3, 3, 3, 3, 2, 2, 2, 2));
    y8 = _mm256_permutevar_ps(y1, _mm256_set_epi32(0, 0, 0, 0, 1, 1, 1, 1));
    y9 = _mm256_permutevar_ps(y1, _mm256_set_epi32(2, 2, 2, 2, 3, 3, 3, 3));

    lake_simd256_write(dest[2], 
        _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(y2, y6),
                                    _mm256_mul_ps(y3, y7)),
                      _mm256_add_ps(_mm256_mul_ps(y4, y8),
                                    _mm256_mul_ps(y5, y9))));
}
#endif /* LAKE_ARCH_X86_AVX */
