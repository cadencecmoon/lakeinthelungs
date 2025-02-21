#pragma once

#include <amw/bedrock.h>

/** @file amw/bits.h
 *
 *  The article on fast population-count by Wojciech Muła, and his github repository
 *  were very helpful in implementing these operations:
 *
 *  https://github.com/WojciechMula/sse-popcount/tree/master,
 *  http://0x80.pl/notesen/2008-05-24-sse-popcount.html 
 *
 *  The paper listed below, by Wojciech Muła, Nathan Kurz and Daniel Lemire was a great 
 *  deal in understanding the different algorithms that could be implemented here:
 *
 *  https://www.google.com/url?q=https://arxiv.org/pdf/1611.07612&sa=U&ved=2ahUKEwiz2pCwxZaLAxVpwjgGHSvfNpoQFnoECAoQAg&usg=AOvVaw3WMSV89LThBSsU2_fZApCR
 */

#ifdef ARCH_X86_AVX2
AMWAPI usize bits_ffs_lookup_avx2(const u8 *data, const usize n);
AMWAPI usize bits_popcnt_lookup_avx2(const u8 *data, const usize n);
#endif

#ifdef ARCH_X86_SSE2
AMWAPI usize bits_ffs_lookup_sse2(const u8 *data, const usize n);
AMWAPI usize bits_popcnt_lookup_sse2(const u8 *data, const usize n);
#endif

#ifdef ARCH_ARM_NEON
AMWAPI usize bits_ffs_lookup_neon(const u8 *data, const usize n);
AMWAPI usize bits_popcnt_lookup_neon(const u8 *data, const usize n);
#endif

#ifdef ARCH_RISCV_V
AMWAPI usize bits_ffs_lookup_rvv(const u8 *data, const usize n);
AMWAPI usize bits_popcnt_lookup_rvv(const u8 *data, const usize n);
#endif

/** Find first set. */
AMWAPI usize bits_ffs_lookup(const u8 *data, const usize n); 

attr_inline attr_nonnull(1) attr_pure
usize bits_ffs(const u8 *data, const usize n)
{
    return bits_ffs_lookup(data, n);
#if 0
    if (n <= 8) return bits_ffs_lookup(data, n);
#if defined(ARCH_X86_AVX2)
    if (n < 32) return bits_ffs_lookup_sse2(data, n);
    return bits_ffs_lookup_avx2(data, n);
#elif defined(ARCH_X86_SSE2)
    return bits_ffs_lookup_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return bits_ffs_lookup_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return bits_ffs_lookup_rvv(data, n);
#else
    return bits_ffs_lookup(data, n);
#endif
#endif /* TODO */
}

/** Population count. */
AMWAPI usize bits_popcnt_lookup(const u8 *data, const usize n);

attr_inline attr_nonnull(1) attr_pure
usize bits_popcnt(const u8 *data, const usize n)
{
    return bits_popcnt_lookup(data, n);
#if 0
    if (n <= 8) return bits_popcnt_lookup(data, n);
#if defined(ARCH_X86_AVX2)
    if (n < 32) return bits_popcnt_lookup_sse2(data, n);
    return bits_popcnt_lookup_avx2(data, n);
#elif defined(ARCH_X86_SSE2)
    return bits_popcnt_lookup_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return bits_popcnt_lookup_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return bits_popcnt_lookup_rvv(data, n);
#else
    return bits_popcnt_lookup(data, n);
#endif
#endif /* TODO */
}

attr_inline attr_const
u32 bits_log2_next_pow2(u32 n)
{
    if (!n) return 0;

    if (n && ((n & (n - 1)) == 0)) {
        n <<= 1;
    } else {
        /* get next power of 2 */
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n += 1;
    }

    /* compute log2 */
    return bits_ffs_lookup((const u8 *)&n, sizeof(u32)) - 1;
}
