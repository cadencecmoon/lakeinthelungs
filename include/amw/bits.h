#pragma once

#include <amw/bedrock.h>

/** @file amw/math/bits.h
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
AMWAPI usize AMWCALL bits_ffs_avx2(const u8 *data, usize n);
AMWAPI u64   AMWCALL bits_popcnt_avx2(const u8 *data, usize n);
#endif

#ifdef ARCH_X86_SSE2
AMWAPI usize AMWCALL bits_ffs_sse2(const u8 *data, usize n);
AMWAPI u64   AMWCALL bits_popcnt_sse2(const u8 *data, usize n);
#endif

#ifdef ARCH_ARM_NEON
AMWAPI usize AMWCALL bits_ffs_neon(const u8 *data, usize n);
AMWAPI u64   AMWCALL bits_popcnt_neon(const u8 *data, usize n);
#endif

#ifdef ARCH_RISCV_V
AMWAPI usize AMWCALL bits_ffs_rvv(const u8 *data, usize n);
AMWAPI u64   AMWCALL bits_popcnt_rvv(const u8 *data, usize n);
#endif

attr_inline attr_const
s32 bits_ctz(u32 x)
{
#if HAS_BUILTIN(__builtin_ctz)
    return __builtin_ctz(x);
#elif defined(CC_MSVC_VERSION)
    u32 index;
    return _BitScanForward(&index, x) ? index : 32;
#else
    if (x == 0) 
        return 32;
    u32 count = 0;
    while ((x & 1) == 0) {
        count++;
        x >>= 1;
    }
    return count;
#endif
}

/** Find first set. */
AMWAPI usize AMWCALL bits_ffs_lookup(const u8 *data, usize n); 

attr_inline attr_nonnull(1) attr_pure
usize bits_ffs(const u8 *data, usize n)
{
#if defined(ARCH_X86_AVX2)
    return n >= 32 ? bits_ffs_avx2(data, n) : bits_ffs_sse2(data, n);
#elif defined(ARCH_X86_SSE2)
    return bits_ffs_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return bits_ffs_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return bits_ffs_rvv(data, n);
#else
    return bits_ffs_lookup(data, n);
#endif
}

/** Population count. */
AMWAPI u64 AMWCALL bits_popcnt_lookup(const u8 *data, usize n);

attr_inline attr_nonnull(1) attr_pure
u64 bits_popcnt(const u8 *data, usize n)
{
#if defined(ARCH_X86_AVX2)
    return n >= 32 ? bits_popcnt_avx2(data, n) : bits_ffs_sse2(data, n);
#elif defined(ARCH_X86_SSE2)
    return bits_popcnt_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return bits_popcnt_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return bits_popcnt_rvv(data, n);
#else
    return bits_popcnt_lookup(data, n);
#endif
}

attr_inline attr_const
u32 bits_log2_next_pow2(u32 n)
{
    /* get next power of 2 */
    if (n && !(n & (n - 1))) {
        n <<= 1;
    } else {
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
