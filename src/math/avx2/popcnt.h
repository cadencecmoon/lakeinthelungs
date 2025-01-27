#pragma once

#include "../../bedrock/types.h"
#ifdef ARCH_X86_AVX2

LAKEAPI u64 popcnt_avx2_lookup(const u8 *data, const usize n) attr_nonnull(1);

#ifdef LAKE_MATH_IMPLEMENTATION

u64 popcnt_avx2_lookup(const u8 *data, const usize n) 
{
    usize i = 0;
    const __m256i lookup = _mm256_setr_epi8(
        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4,

        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4
    );

    const __m256i low_mask = _mm256_set1_epi8(0x0f);
    __m256i acc = _mm256_setzero_si256();

#define ITER { \
        const __m256i vec = _mm256_loadu_si256((const __m256i *)(data + i)); \
        const __m256i lo  = _mm256_and_si256(vec, low_mask); \
        const __m256i hi  = _mm256_and_si256(_mm256_srli_epi16(vec, 4), low_mask); \
        const __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo); \
        const __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi); \
        local = _mm256_add_epi8(local, popcnt1); \
        local = _mm256_add_epi8(local, popcnt2); \
        i += 32; \
    }

    while (i + 8*32 <= n) {
        __m256i local = _mm256_setzero_si256();
        ITER ITER ITER ITER ITER ITER ITER ITER
        acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
    }

    __m256i local = _mm256_setzero_si256();

    while (i + 32 <= n) { ITER }
    acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
#undef ITER

    u64 result = 0;

    result += (u64)_mm256_extract_epi64(acc, 0);
    result += (u64)_mm256_extract_epi64(acc, 1);
    result += (u64)_mm256_extract_epi64(acc, 2);
    result += (u64)_mm256_extract_epi64(acc, 3);

    for (; i < n; i++) {
        result += popcnt_lookup_8bit_table[data[i]];
    }

    return result;
}

#endif /* LAKE_MATH_IMPLEMENTATION */
#endif /* ARCH_X86_AVX2 */
