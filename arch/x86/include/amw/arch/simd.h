/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_X86_SIMD_H
#define _AMW_X86_SIMD_H

#include <amw/common/defines.h>
#include <amw/common/align.h>

AMW_C_DECL_BEGIN

/* SIMD ISA extensions for x86/x86_64 and Elbrus */
#if defined(_M_IX86_FP)
    #define AMW_ARCH_X86_MMX 1
    #include <mmintrin.h>
    #if (_M_IX86_FP >= 1)
        #define AMW_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if (_M_IX86_FP >= 2)
        #define AMW_ARCH_X86_SSE 1
        #define AMW_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#elif defined(_M_X64)
    #define AMW_ARCH_X86_SSE 1
    #define AMW_ARCH_X86_SSE2 1
    #include <emmintrin.h>
#else
    #if defined(__MMX__)
        #define AMW_ARCH_X86_MMX 1
        #include <mmintrin.h>
    #endif
    #if defined(__SSE__)
        #define AMW_ARCH_X86_SSE 1
        #include <xmmintrin.h>
    #endif
    #if defined(__SSE2__)
        #define AMW_ARCH_X86_SSE2 1
        #include <emmintrin.h>
    #endif
#endif
#if defined(__SSE3__)
    #define AMW_ARCH_X86_SSE3 1
    #include <pmmintrin.h>
#endif
#if defined(__SSSE3__)
    #define AMW_ARCH_X86_SSSE3 1
    #include <tmmintrin.h>
#endif
#if defined(__SSE4_1__)
    #define AMW_ARCH_X86_SSE4_1 1
    #include <smmintrin.h>
#endif
#if defined(__SSE4_2__)
    #define AMW_ARCH_X86_SSE4_2 1
    #include <nmmintrin.h>
#endif
#if defined(__SSE4A__)
    #define AMW_ARCH_X86_SSE4A 1
    #include <ammintrin.h>
#endif
#if defined(__XOP__)
    #define AMW_ARCH_X86_XOP 1
    #include <xopintrin.h>
#endif
#if defined(__AVX__)
    #define AMW_ARCH_X86_AVX 1
    #if !defined(AMW_ARCH_X86_SSE3)
        #define AMW_ARCH_X86_SSE3 1
        #include <pmmintrin.h>
    #endif
    #if !defined(AMW_ARCH_X86_SSE4_1)
        #define AMW_ARCH_X86_SSE4_1 1
        #include <smmintrin.h>
    #endif
    #if !defined(AMW_ARCH_X86_SSE4_2)
        #define AMW_ARCH_X86_SSE4_2 1
        #include <nmmintrin.h>
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX2__)
    #define AMW_ARCH_X86_AVX2 1
    #if defined(AMW_CC_MSVC_VERSION)
        #ifndef __FMA__
            #define __FMA__ 1
        #endif
    #endif
    #include <immintrin.h>
#endif
#if defined(__FMA__)
    #define AMW_ARCH_X86_FMA 1
    #if !defined(AMW_ARCH_X86_AVX)
        #define AMW_ARCH_X86_AVX 1
    #endif
    #include <immintrin.h>
#endif
#if defined(__AVX512VP2INTERSECT__)
    #define AMW_ARCH_X86_AVX512VP2INTERSECT 1
#endif
#if defined(__AVX512BITALG__)
    #define AMW_ARCH_X86_AVX512BITALG 1
#endif
#if defined(__AVX512VPOPCNTDQ__)
    #define AMW_ARCH_X86_AVX512VPOPCNTDQ 1
#endif
#if defined(__AVX512VBMI__)
    #define AMW_ARCH_X86_AVX512VBMI 1
#endif
#if defined(__AVX512VBMI2__)
    #define AMW_ARCH_X86_AVX512VBMI2 1
#endif
#if defined(__AVX512VNNI__)
    #define AMW_ARCH_X86_AVX512VNNI 1
#endif
#if defined(__AVX5124VNNIW__)
    #define AMW_ARCH_X86_AVX5124VNNIW 1
#endif
#if defined(__AVX512BW__)
    #define AMW_ARCH_X86_AVX512BW 1
#endif
#if defined(__AVX512BF16__)
    #define AMW_ARCH_X86_AVX512BF16 1
#endif
#if defined(__AVX512CD__)
    #define AMW_ARCH_X86_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
    #define AMW_ARCH_X86_AVX512DQ 1
#endif
#if defined(__AVX512F__)
    #define AMW_ARCH_X86_AVX512F 1
#endif
#if defined(__AVX512VL__)
    #define AMW_ARCH_X86_AVX512VL 1
#endif
#if defined(__AVX512FP16__)
    #define AMW_ARCH_X86_AVX512FP16 1
#endif
#if defined(__GFNI__)
    #define AMW_ARCH_X86_GFNI 1
#endif
#if defined(__PCLMUL__)
    #define AMW_ARCH_X86_PCLMUL 1
#endif
#if defined(__VPCLMULQDQ__)
    #define AMW_ARCH_X86_VPCLMULQDQ 1
#endif
#if (defined(__F16C__) || AMW_CC_MSVC_VERSION_CHECK(19,30,0)) && defined(AMW_ARCH_X86_AVX2)
    #define AMW_ARCH_X86_F16C 1
#endif
#if defined(__AES__)
    #define AMW_ARCH_X86_AES 1
#endif

#if AMW_CC_MSVC_VERSION_CHECK(14,0,0)
    #ifdef AMW_CC_CLANG_VERSION
        #ifndef __PRFCHWINTRIN_H
            #define __PRFCHWINTRIN_H
            static __inline__ void __attribute__((__always_inline__, __nodebug__))
            _m_prefetch(void *__P) {
                __builtin_prefetch(__P, 0, 3);
            }
        #endif
    #endif
    #include <intrin.h>
#elif AMW_CC_MINGW_VERSION_CHECK(4,0,0)
    #include <intrin.h>
#endif

/* do not use aligned SIMD instructions for older visual studio versions */
#if AMW_CC_MSVC_VERSION && !AMW_CC_MSVC_VERSION_CHECK(19,13,0)
    /* Visual Studio 2017 version 15.6 */
    #ifndef AMW_SIMD_UNALIGNED
        #define AMW_SIMD_UNALIGNED 1
    #endif
#endif

#if defined(AMW_ARCH_X86_MMX) && !defined(AMW_SIMD_DISABLED)
#define AMW_HAS_f128 1
typedef __m128 f128;
#else
#define AMW_HAS_f128 0
#endif /* AMW_ARCH_X86_MMX */

#if defined(AMW_ARCH_X86_AVX) && !defined(AMW_SIMD_DISABLED)
#define AMW_HAS_f256 1
typedef __m256 f256;
#else
#define AMW_HAS_f256 0
#endif /* AMW_ARCH_X86_AVX */

#if AMW_HAS_f128 && !defined(AMW_SIMD_DISABLED)
/* MMX / SSE / AVX intrinsics */

#ifdef AMW_SIMD_UNALIGNED
    #define simd_load(p)     _mm_loadu_ps(p)
    #define simd_store(p, a) _mm_storeu_ps(p, a)
#else
    #define simd_load(p)     _mm_load_ps(p)
    #define simd_store(p, a) _mm_store_ps(p, a)
#endif
#define simd_set1(x) _mm_set1_ps(x)

#if defined(AMW_SIMD_INT_DOMAIN) && defined(AMW_ARCH_X86_SSE)
    #define simd_shuff1(xmm, z, y, x, w) \
        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(xmm), _MM_SHUFFLE(z, y, x, w)))
#else
    #define simd_shuff1(xmm, z, y, x, w) \
        _mm_shuffle_ps(xmm, xmm, _MM_SHUFFLE(z, y, x, w))
#endif

#define simd_splat(x, lane) simd_shuff1(x, lane, lane, lane, lane)
#define simd_splatx(x) simd_splat(x,0)
#define simd_splaty(y) simd_splat(y,0)
#define simd_splatz(z) simd_splat(z,0)
#define simd_splatw(w) simd_splat(w,0)

#define amw_simd_shuff2(a, b, z0, y0, x0, w0, z1, y1, x1, w1) \
    amw_simd_shuff2(_mm_shuffle_ps(a, b, _MM_SHUFFLE(z0, y0, x0, w0)), x1, y1, x1, w1)

#ifdef AMW_ARCH_X86_AVX
    #ifdef AMW_SIMD_UNALIGNED
        #define simd256_load(p)     _mm256_loadu_ps(p)
        #define simd256_store(p, a) _mm256_storeu_ps(p, a)
    #else
        #define simd256_load(p)     _mm256_load_ps(p)
        #define simd256_store(p, a) _mm256_store_ps(p, a)
    #endif
#endif

/* note that 0x80000000 corresponds to INT_MIN for a 32-bit int */
#ifdef AMW_ARCH_X86_SSE2
    #define AMW_SIMD_NEGZEROf ((i32)0x80000000) /* -> -0.0f */
    #define AMW_SIMD_POSZEROf ((i32)0x00000000) /* -> +0.0f */
#else
    #ifdef AMW_SIMD_FAST_MATH
        union { i32 i; f32 f; } static AMW_SIMD_NEGZEROf_TU = { .i = (i32)0x80000000 };
        #define AMW_SIMD_NEGZEROf AMW_SIMD_NEGZEROf_TU.f;
        #define AMW_SIMD_POSZEROf 0.0f
    #else
        #define AMW_SIMD_NEGZEROf -0.0f
        #define AMW_SIMD_POSZEROf 0.0f
    #endif
#endif

#ifdef AMW_ARCH_X86_SSE2
    #define AMW_SIMD_SIGNMASKf(x, y, z, w) _mm_castsi128_ps(_mm_set_epi32(x, y, z, w))
#else
    #define AMW_SIMD_SIGNMASKf(x, y, z, w) __mm_set_ps(x, y, z, w)
#endif

#define simd_float32x4_SIGNMASK_PNPN \
        AMW_SIMD_SIGNMASKf(AMW_SIMD_POSZEROf, AMW_SIMD_NEGZEROf, AMW_SIMD_POSZEROf, AMW_SIMD_NEGZEROf)
#define simd_float32x4_SIGNMASK_NPNP \
        AMW_SIMD_SIGNMASKf(AMW_SIMD_NEGZEROf, AMW_SIMD_POSZEROf, AMW_SIMD_NEGZEROf, AMW_SIMD_POSZEROf)
#define simd_float32x4_SIGNMASK_NPPN \
        AMW_SIMD_SIGNMASKf(AMW_SIMD_NEGZEROf, AMW_SIMD_POSZEROf, AMW_SIMD_POSZEROf, AMW_SIMD_NEGZEROf)

/* fast math prevents -0.0f to work */
#ifdef AMW_ARCH_X86_SSE2
    #define simd_float32x4_SIGNMASK_NEG _mm_castsi128_ps(_mm_set1_epi32(AMW_SIMD_NEGZEROf))
#else
    #define simd_float32x4_SIGNMASK_NEG _mm_set1_ps(AMW_SIMD_NEGZEROf)
#endif
#ifdef AMW_ARCH_X86_AVX
#define simd_float32x8_SIGNMASK_NEG     _mm256_castsi256_ps(_mm256_set1_epi32(AMW_SIMD_NEGZEROf))
#endif

AMW_INLINE f128 simd_abs(f128 x) {
    return _mm_andnot_ps(simd_float32x4_SIGNMASK_NEG, x);
}

AMW_INLINE f128 simd_min(f128 a, f128 b) {
    return _mm_min_ps(a, b);
}

AMW_INLINE f128 simd_max(f128 a, f128 b) {
    return _mm_max_ps(a, b);
}

AMW_INLINE f128 simd_vhadd(f128 v) {
    f128 x;
    x = _mm_add_ps(v, simd_shuff1(v, 0, 1, 2, 3));
    x = _mm_add_ps(x, simd_shuff1(x, 1, 0, 0, 1));
    return x;
}

AMW_INLINE f128 simd_vhadds(f128 v) {
  f128 shuf, sums;
#ifdef AMW_ARCH_X86_SSE3
  shuf = _mm_movehdup_ps(v);
#else
  shuf = simd_shuff1(v, 2, 3, 0, 1);
#endif
  sums = _mm_add_ps(v, shuf);
  shuf = _mm_movehl_ps(shuf, sums);
  sums = _mm_add_ss(sums, shuf);
  return sums;
}

AMW_INLINE f32 simd_hadd(f128 v) {
    return _mm_cvtss_f32(simd_vhadds(v));
}

AMW_INLINE f128 simd_vhmin(f128 v) {
    f128 x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);   /* [ 2,  3,  2,  3 ] */
    x1 = _mm_min_ps(x0, v);     /* [0|2 1|3 2|3 3|3] */
    x2 = simd_splat(x1, 1); /* [1|3 1|3 1|3 1|3] */
    return _mm_min_ss(x1, x2);
}

AMW_INLINE f128 simd_vhmax(f128 v) {
    f128 x0, x1, x2;
    x0 = _mm_movehl_ps(v, v);   /* [ 2,  3,  2,  3 ] */
    x1 = _mm_max_ps(x0, v);     /* [0|2 1|3 2|3 3|3] */
    x2 = simd_splat(x1, 1); /* [1|3 1|3 1|3 1|3] */
    return _mm_max_ss(x1, x2);
}

AMW_INLINE f32 simd_hmax(f128 v) {
    return _mm_cvtss_f32(simd_vhmax(v));
}

AMW_INLINE f128 simd_vdots(f128 a, f128 b) {
#if (defined(AMW_ARCH_X86_SSE4_1) || defined(AMW_ARCH_X86_SSE4_2)) && !defined(AMW_SIMD_X86_USE_SSE3_DOT)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(AMW_ARCH_X86_SSE3) && defined(AMW_SIMD_X86_USE_SSE3_DOT)
    f128 x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    return simd_vhadds(_mm_mul_ps(a, b));
#endif
}

AMW_INLINE f128 simd_vdot(f128 a, f128 b) {
#if (defined(AMW_ARCH_X86_SSE4_1) || defined(AMW_ARCH_X86_SSE4_2)) && !defined(AMW_SIMD_X86_USE_SSE3_DOT)
    return _mm_dp_ps(a, b, 0xFF);
#elif defined(AMW_ARCH_X86_SSE3) && defined(AMW_SIMD_X86_USE_SSE3_DOT)
    f128 x0, x1;
    x0 = _mm_mul_ps(a, b);
    x1 = _mm_hadd_ps(x0, x0);
    return _mm_hadd_ps(x1, x1);
#else
    f128 x0;
    x0 = _mm_mul_ps(a, b);
    x0 = _mm_add_ps(x0, amw_simd_shuff1(x0, 1, 0, 3, 2));
    return _mm_add_ps(x0, amw_simd_shuff1(x0, 0, 1, 0, 1));
#endif
}

AMW_INLINE f32 simd_dot(f128 a, f128 b) {
    return _mm_cvtss_f32(simd_vdots(a, b));
}

AMW_INLINE f32 simd_norm(f128 a) {
    return _mm_cvtss_f32(_mm_sqrt_ss(simd_vhadds(_mm_mul_ps(a, a))));
}

AMW_INLINE f32 simd_norm2(f128 a) {
    return _mm_cvtss_f32(simd_vhadds(_mm_mul_ps(a, a)));
}

AMW_INLINE f32 simd_norm_one(f128 a) {
    return _mm_cvtss_f32(simd_vhadds(simd_abs(a)));
}

AMW_INLINE f32 simd_norm_inf(f128 a) {
    return _mm_cvtss_f32(simd_vhmax(simd_abs(a)));
}

#ifdef AMW_ARCH_X86_SSE2
AMW_INLINE f128 simd_load3(f32 v[3]) {
    __m128i xy;
    __m128  z;
    xy = _mm_loadl_epi64(AMW_CASTPTR_ASSUME_ALIGNED(v, const __m128i));
    z  = _mm_load_ss(&v[2]);
    return _mm_movelh_ps(_mm_castsi128_ps(xy), z);
}

AMW_INLINE void simd_store3(f32 v[3], f128 vx) {
    _mm_storel_pi(AMW_CASTPTR_ASSUME_ALIGNED(v, __m64), vx);
    _mm_store_ss(&v[2], simd_shuff1(vx, 2, 2, 2, 2));
}
#endif /* ARCH_X86_SSE2 */

AMW_INLINE f128 simd_div(f128 a, f128 b) {
    return _mm_div_ps(a, b);
}

AMW_INLINE f128 simd_fmadd(f128 a, f128 b, f128 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm_fmadd_ps(a, b, c);
#else
    return _mm_add_ps(c, _mm_mul_ps(a, b));
#endif
}

AMW_INLINE f128 simd_fnmadd(f128 a, f128 b, f128 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm_fnmadd_ps(a, b, c);
#else
    return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif 
}

AMW_INLINE f128 simd_fmsub(f128 a, f128 b, f128 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm_fmsub_ps(a, b, c);
#else
    return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif
}

AMW_INLINE f128 simd_fnmsub(f128 a, f128 b, f128 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm_fmnsub_ps(a, b, c);
#else
    return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(a, b), c), simd_float32x4_SIGNMASK_NEG);
#endif 
}

#ifdef AMW_ARCH_X86_AVX
AMW_INLINE f256 simd256_fmadd(f256 a, f256 b, f256 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm256_fmadd_ps(a, b, c);
#else
    return _mm256_add_ps(c, _mm256_mul_ps(a, b));
#endif
}

AMW_INLINE f256 simd256_fnmadd(f256 a, f256 b, f256 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm256_fnmadd_ps(a, b, c);
#else
    return _mm256_sub_ps(c, _mm256_mul_ps(a, b));
#endif
}

AMW_INLINE f256 simd256_fmsub(f256 a, f256 b, f256 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm256_fmsub_ps(a, b, c);
#else
    return _mm256_sub_ps(_mm256_mul_ps(a, b), c);
#endif
}

AMW_INLINE f256 simd256_fnmsub(f256 a, f256 b, f256 c) {
#ifdef AMW_ARCH_X86_FMA
    return _mm256_fmnsub_ps(a, b, c);
#else
    return _mm256_xor_ps(_mm256_add_ps(_mm256_mul_ps(a, b), c), simd_float32x8_SIGNMASK_NEG);
#endif
}
#endif /* AMW_ARCH_X86_AVX */

#define AMW_SIMD (1)
#else
#define AMW_SIMD (0)
#endif /* AMW_SIMD_DISABLED */

AMW_C_DECL_END
#endif /* _AMW_X86_SIMD_H */
