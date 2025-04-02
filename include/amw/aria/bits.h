#pragma once

#include <amw/aria/core.h>

/** @file amw/aria/bits.h
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

attr_inline attr_const 
u16 aria_bswap16(u16 x) {
#if HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ushort(x);
#else
#error Implement bswap for this platform
#endif
}

attr_inline attr_const 
u32 aria_bswap32(u32 x) {
#if HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ulong(x);
#endif
}

attr_inline attr_const 
u64 aria_bswap64(u64 x) {
#if HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_uint64(x);
#endif
}

#if _ARCH_BYTEORDER == _ARCH_BYTEORDER_LE
    #define ARCH_LIL_ENDIAN 1
    #define aria_bswap16le(x) (x)
    #define aria_bswap32le(x) (x)
    #define aria_bswap64le(x) (x)
    #define aria_bswap16be(x) bswap16(x) 
    #define aria_bswap32be(x) bswap32(x)
    #define aria_bswap64be(x) bswap64(x)
#else
    #define ARCH_BIG_ENDIAN 1
    #define aria_bswap16le(x) bswap16(x) 
    #define aria_bswap32le(x) bswap32(x)
    #define aria_bswap64le(x) bswap64(x)
    #define aria_bswap16be(x) (x)
    #define aria_bswap32be(x) (x)
    #define aria_bswap64be(x) (x)
#endif

#ifdef ARCH_X86_AVX2
AMWAPI usize AMWCALL aria_bits_ffs_avx2(const u8 *data, usize n);
AMWAPI u64   AMWCALL aria_bits_popcnt_avx2(const u8 *data, usize n);
#endif

#ifdef ARCH_X86_SSE2
AMWAPI usize AMWCALL aria_bits_ffs_sse2(const u8 *data, usize n);
AMWAPI u64   AMWCALL aria_bits_popcnt_sse2(const u8 *data, usize n);
#endif

#ifdef ARCH_ARM_NEON
AMWAPI usize AMWCALL aria_bits_ffs_neon(const u8 *data, usize n);
AMWAPI u64   AMWCALL aria_bits_popcnt_neon(const u8 *data, usize n);
#endif

#ifdef ARCH_RISCV_V
AMWAPI usize AMWCALL aria_bits_ffs_rvv(const u8 *data, usize n);
AMWAPI u64   AMWCALL aria_bits_popcnt_rvv(const u8 *data, usize n);
#endif

attr_inline attr_const
s32 aria_bits_ctz(u32 x)
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
AMWAPI usize AMWCALL aria_bits_ffs_lookup(const u8 *data, usize n); 

attr_inline attr_nonnull(1) attr_pure
usize aria_bits_ffs(const u8 *data, usize n)
{
#if defined(ARCH_X86_AVX2)
    return n >= 32 ? aria_bits_ffs_avx2(data, n) : aria_bits_ffs_sse2(data, n);
#elif defined(ARCH_X86_SSE2)
    return aria_bits_ffs_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return aria_bits_ffs_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return aria_bits_ffs_rvv(data, n);
#else
    return aria_bits_ffs_lookup(data, n);
#endif
}

/** Population count. */
AMWAPI u64 AMWCALL aria_bits_popcnt_lookup(const u8 *data, usize n);

attr_inline attr_nonnull(1) attr_pure
u64 aria_bits_popcnt(const u8 *data, usize n)
{
#if defined(ARCH_X86_AVX2)
    return n >= 32 ? aria_bits_popcnt_avx2(data, n) : aria_bits_ffs_sse2(data, n);
#elif defined(ARCH_X86_SSE2)
    return aria_bits_popcnt_sse2(data, n);
#elif defined(ARCH_ARM_NEON)
    return aria_bits_popcnt_neon(data, n);
#elif defined(ARCH_RISCV_V)
    return aria_bits_popcnt_rvv(data, n);
#else
    return aria_bits_popcnt_lookup(data, n);
#endif
}

attr_inline attr_const
u32 aria_log2_next_pow2(u32 n)
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
    return aria_bits_ffs_lookup((const u8 *)&n, sizeof(u32)) - 1;
}
