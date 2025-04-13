#pragma once

#include <amwe/bedrock.h>   

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @file math/bits.h
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

lake_force_inline lake_const s32 
bits_ctz(u32 x)
{
#if LAKE_HAS_BUILTIN(__builtin_ctz)
    return __builtin_ctz(x);
#elif defined(LAKE_CC_MSVC_VERSION)
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

/** Find first set bit. */
LAKEAPI lake_nonnull(1) lake_pure usize LAKECALL 
bits_ffs_table_lookup(const u8 *data, usize n);

/** Population count. */
LAKEAPI lake_nonnull(1) lake_pure u64 LAKECALL
bits_popcnt_table_lookup(const u8 *data, usize n);

#if defined(LAKE_ARCH_X86_AVX2)
    LAKEAPI lake_nonnull(1) lake_pure usize LAKECALL
    bits_ffs_avx2(const u8 *data, usize n);

    LAKEAPI lake_nonnull(1) lake_pure u64 LAKECALL 
    bits_popcnt_avx2(const u8 *data, usize n);
#endif /* LAKE_ARCH_X86_AVX2 */

#if defined(LAKE_ARCH_X86_SSE2)
    LAKEAPI lake_nonnull(1) lake_pure usize LAKECALL 
    bits_ffs_sse2(const u8 *data, usize n);

    LAKEAPI lake_nonnull(1) lake_pure u64 LAKECALL 
    bits_popcnt_sse2(const u8 *data, usize n);
#endif /* LAKE_ARCH_X86_SSE2 */

/* TODO neon, rvv, wasm_simd */

lake_force_inline lake_nonnull(1) lake_pure usize 
bits_ffs(const u8 *data, usize n)
{
#if defined(LAKE_ARCH_X86_AVX2)
    return n >= 32 ? bits_ffs_avx2(data, n) : bits_ffs_sse2(data, n);
#elif defined(LAKE_ARCH_X86_SSE2)
    return bits_ffs_sse2(data, n);
#else
    return bits_ffs_table_lookup(data, n);
#endif /* bits_ffs */
}

lake_force_inline lake_nonnull(1) lake_pure u64 
bits_popcnt(const u8 *data, usize n)
{
#if defined(LAKE_ARCH_X86_AVX2)
    return n >= 32 ? bits_popcnt_avx2(data, n) : bits_popcnt_sse2(data, n);
#elif defined(LAKE_ARCH_X86_SSE2)
    return bits_popcnt_sse2(data, n);
#else
    return bits_popcnt_table_lookup(data, n);
#endif /* bits_popcnt */
}

/** Computes the bit of the next power of 2. */
LAKEAPI lake_const u32 LAKECALL 
bits_log2_next_pow2(u32 n);

#ifdef __cplusplus
}
#endif /* __cplusplus */
