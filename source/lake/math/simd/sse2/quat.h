#pragma once

#include "../../../simd.h"
#if defined(LAKE_ARCH_X86_SSE2)

LAKE_NONNULL_ALL
LAKE_FORCE_INLINE void lake_quat_mul_sse2(quat p, quat q, quat dest) 
{
    /* (a1 b2 + b1 a2 + c1 d2 - d1 c2)i
     * (a1 c2 - b1 d2 + c1 a2 + d1 b2)j
     * (a1 d2 + b1 c2 - c1 b2 + d1 a2)k
     *  a1 a2 - b1 b2 - c1 c2 - d1 d2 */
    f128 xp, xq, x1, x2, x3, r, x, y, z;

    xp = lake_simd_read(p); /* 3 2 1 0 */
    xq = lake_simd_read(q);
    x1 = lake_simd_float32x4_SIGNMASK_NPNP; /* XXX _mm_set1_ss() + shuff ? */
    r  = _mm_mul_ps(lake_simd_splat_w(xp), xq);

    x2 = _mm_unpackhi_ps(x1, x1);
    x3 = lake_simd_shuffle1(x1, 3, 2, 0, 1);
    x  = lake_simd_splat_x(xp);
    y  = lake_simd_splat_y(xp);
    z  = lake_simd_splat_z(xp);

    x  = _mm_xor_ps(x, x1);
    y  = _mm_xor_ps(y, x2);
    z  = _mm_xor_ps(z, x3);

    x1 = lake_simd_shuffle1(xq, 0, 1, 2, 3);
    x2 = lake_simd_shuffle1(xq, 1, 0, 3, 2);
    x3 = lake_simd_shuffle1(xq, 2, 3, 0, 1);

    r  = lake_simd_fmadd(x, x1, r);
    r  = lake_simd_fmadd(y, x2, r);
    r  = lake_simd_fmadd(z, x3, r);

    lake_simd_write(dest, r);
}
#endif /* LAKE_ARCH_X86_SSE2 */
