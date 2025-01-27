#pragma once

#include "../../bedrock/types.h"
#ifdef ARCH_X86_SSE2

LAKEAPI u64 popcnt_sse2_lookup(const u8 *data, const usize n) attr_nonnull(1);

#ifdef LAKE_MATH_IMPLEMENTATION

u64 popcnt_sse2_lookup(const u8 *data, const usize n)
{
    usize i = 0;
    const __m128i lookup = _mm_setr_epi8(
        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4
    );

    const __m128i low_mask = _mm_set1_epi8(0x0f);
    __m128i acc = _mm_setzero_si128();

#define ITER { \
        const __m128i vec = _mm_loadu_si128((const __m128i *)(data + i)); \
        const __m128i lo  = _mm_and_si128(vec, low_mask); \
        const __m128i hi  = _mm_and_si128(_mm_srli_epi16(vec, 4), low_mask); \
        const __m128i popcnt1 = _mm_shuffle_epi8(lookup, lo); \
        const __m128i popcnt2 = _mm_shuffle_epi8(lookup, hi); \
        local = _mm_add_epi8(local, popcnt1); \
        local = _mm_add_epi8(local, popcnt2); \
        i += 16; \
    }

    while (i + 8*16 <= n) {
        __m128i local = _mm_setzero_si128();
        ITER ITER ITER ITER ITER ITER ITER ITER
        acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
    }

    __m128i local = _mm_setzero_si128();

    while (i + 16 <= n) { ITER }
    acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
#undef ITER

    usize result = _mm_cvtsi128_si64(acc) + _mm_cvtsi128_si64(_mm_srli_si128(acc, 8));

    for (; i < n; i++) {
        result += popcnt_lookup_8bit_table[data[i]];
    }

    return result;
}

#endif /* LAKE_MATH_IMPLEMENTATION */
#endif /* ARCH_X86_SSE2 */
