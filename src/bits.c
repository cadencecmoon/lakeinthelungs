#include <amw/bits.h>

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

usize bits_ffs_lookup(const u8 *data, const usize n)
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

usize bits_popcnt_lookup(const u8 *data, const usize n)
{
    usize bits = 0;
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
usize bits_ffs_lookup_avx2(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}

usize bits_popcnt_lookup_avx2(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}
#endif

#ifdef ARCH_X86_SSE2
usize bits_ffs_lookup_sse2(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}

usize bits_popcnt_lookup_sse2(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}
#endif

#ifdef ARCH_ARM_NEON
usize bits_ffs_lookup_neon(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}

usize bits_popcnt_lookup_neon(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}
#endif

#ifdef ARCH_RISCV_V
usize bits_ffs_lookup_rvv(const u8 *data, const usize n)
{
    (void)data; (void)n; return 0;
}

usize bits_popcnt_lookup_rvv(const u8 *data, const usize n) 
{
    (void)data; (void)n; return 0;
}
#endif
