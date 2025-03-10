#include <amw/bits.h>

/* XXX debug */
#include <amw/log.h>

static const u8 g_ffs_table[256] = {
    0,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    8,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
    6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1
};

usize bits_ffs_lookup(const u8 *data, usize n)
{
    for (usize o = 0; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0;
}

static const u8 g_popcnt_table[256] = {
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

u64 bits_popcnt_lookup(const u8 *data, usize n)
{
    u64 bits = 0;
    usize o = 0;
    while (o + 4 <= n) {
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
        bits += g_popcnt_table[data[o]]; o++;
    }
    while (o < n) {
        bits += g_popcnt_table[data[o]]; o++;
    }
    return bits;
}

#ifdef ARCH_X86_AVX2
usize bits_ffs_avx2(const u8 *data, usize n)
{
    usize o = 0;
    while (o + 32 <= n) {
        __m256i vec  = _mm256_loadu_si256((const __m256i *)(data + o));

        /* sets the FZ flag on non-zero values, we AND vec with itself to check if any bits are set to 1 */
        if (!_mm256_testz_si256(vec, vec)) {
            __m256i cmp = _mm256_cmpeq_epi8(vec, _mm256_setzero_si256());
            __m256i nonzero = _mm256_andnot_si256(cmp, _mm256_set1_epi8((u8)0xFF));

            s32 mask = _mm256_movemask_epi8(nonzero);
            s32 idx = bits_ctz(mask);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
        o += 32;
    }
    for (; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0lu;
}

u64 bits_popcnt_avx2(const u8 *data, usize n)
{
    usize o = 0;
    const __m256i lookup = _mm256_setr_epi8(
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
    const __m256i low_mask = _mm256_set1_epi8((u8)0x0F);
    __m256i acc = _mm256_setzero_si256();

#define ITER {                                                                      \
        const __m256i vec = _mm256_loadu_si256((const __m256i *)(data + o));        \
        const __m256i lo  = _mm256_and_si256(vec, low_mask);                        \
        const __m256i hi  = _mm256_and_si256(_mm256_srli_epi16(vec, 4), low_mask);  \
        const __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);                    \
        const __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);                    \
        local = _mm256_add_epi8(local, popcnt1);                                    \
        local = _mm256_add_epi8(local, popcnt2);                                    \
        o += 32; \
    }
    while (o + 8*32 <= n) {
        __m256i local = _mm256_setzero_si256();
        ITER ITER ITER ITER ITER ITER ITER ITER
        acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
    }
    __m256i local = _mm256_setzero_si256();

    while (o + 32 <= n) { ITER }
    acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
#undef ITER

    u64 result = 0;
    result += (u64)_mm256_extract_epi64(acc, 0);
    result += (u64)_mm256_extract_epi64(acc, 1);
    result += (u64)_mm256_extract_epi64(acc, 2);
    result += (u64)_mm256_extract_epi64(acc, 3);

    for (; o < n; o++)
        result += g_popcnt_table[data[o]];
    return result;
}
#endif

#ifdef ARCH_X86_SSE2
usize bits_ffs_sse2(const u8 *data, usize n)
{
    usize o = 0;
    while (o + 16 <= n) {
        __m128i vec = _mm_loadu_si128((const __m128i *)(data + o));
#ifdef ARCH_X86_SSE4_1
        if (!_mm_testz_si128(vec, vec)) {
            __m128i cmp = _mm_cmpeq_epi8(vec, _mm_setzero_si128());
            __m128i nonzero = _mm_andnot_si128(cmp, _mm_set1_epi8((u8)0xFF));

            s32 mask = _mm_movemask_epi8(nonzero);
            s32 idx = bits_ctz(mask);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
#else
        __m128i cmp = _mm_cmpeq_epi8(vec, _mm_setzero_si128());
        s32 mask = _mm_movemask_epi8(cmp);
        if (mask != 0xFFFF) {
            s32 nonzero = (~mask) & 0xFFFF;
            s32 idx = bits_ctz(nonzero);
            return ((idx + o) << 3) + g_ffs_table[data[idx + o]];
        }
#endif
        o += 16;
    }
    for (; o < n; o++)
        if (data[o] != 0) return (o << 3) + g_ffs_table[data[o]];
    return 0lu;
}

u64 bits_popcnt_sse2(const u8 *data, usize n)
{
    usize o = 0;
    const __m128i lookup = _mm_setr_epi8(
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
    const __m128i low_mask = _mm_set1_epi8((u8)0x0F);
    __m128i acc = _mm_setzero_si128();

#define ITER {                                                                  \
        const __m128i vec = _mm_loadu_si128((const __m128i *)(data + o));       \
        const __m128i lo  = _mm_and_si128(vec, low_mask);                       \
        const __m128i hi  = _mm_and_si128(_mm_srli_epi16(vec, 4), low_mask);    \
        const __m128i popcnt1 = _mm_shuffle_epi8(lookup, lo);                   \
        const __m128i popcnt2 = _mm_shuffle_epi8(lookup, hi);                   \
        local = _mm_add_epi8(local, popcnt1);                                   \
        local = _mm_add_epi8(local, popcnt2);                                   \
        o += 16;                                                                \
    }
    while (o + 8*16 <= n) {
        __m128i local = _mm_setzero_si128();
        ITER ITER ITER ITER ITER ITER ITER ITER
        acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
    }
    __m128i local = _mm_setzero_si128();

    while (o + 16 <= n) { ITER }
    acc = _mm_add_epi64(acc, _mm_sad_epu8(local, _mm_setzero_si128()));
#undef ITER

    u64 result = _mm_cvtsi128_si64(acc) + _mm_cvtsi128_si64(_mm_srli_si128(acc, 8));

    for (; o < n; o++)
        result += g_popcnt_table[data[o]];
    return result;
}
#endif

#ifdef ARCH_ARM_NEON
usize bits_ffs_neon(const u8 *data, usize n)
{
    assert_debug(!"TODO");
    (void)data; (void)n; return 0;
}

u64 bits_popcnt_neon(const u8 *data, usize n)
{
    assert_debug(!"TODO");
    (void)data; (void)n; return 0;
}
#endif

#ifdef ARCH_RISCV_V
usize bits_ffs_rvv(const u8 *data, usize n)
{
    assert_debug(!"TODO");
    (void)data; (void)n; return 0;
}

u64 bits_popcnt_rvv(const u8 *data, usize n) 
{
    assert_debug(!"TODO");
    (void)data; (void)n; return 0;
}
#endif
