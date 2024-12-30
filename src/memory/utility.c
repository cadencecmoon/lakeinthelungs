#include <lake/ipomoeaalba.h>

AMWAPI void * AMWAPIENTRY 
iamemset(void *dest, int32_t c, size_t n)
{
    uint8_t *s = dest;
    size_t k;

    /* Fill head and tail with minimal branching. Each conditional 
     * ensures that all the subsequently used offsets are well-defined 
     * and in the dest region. */

    if (!n) return dest;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dest;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dest;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary, and truncate n to 
     * a multiple of 4. The previous code already took care of any head/tail 
     * that get cut off by the alignment. */

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;

#ifdef AMW_CC_GNUC_VERSION
    typedef uint32_t __attribute__((__may_alias__)) ma_u32;
    typedef uint64_t __attribute__((__may_alias__)) ma_u64;

    ma_u32 c32 = ((ma_u32) - 1) / 255 * (uint8_t)c;

    /* In preparation to copy 32 bits at a time, aligned on an 8-byte boundary,
     * fill head/tail up to 28 bytes each. As in the initial byte-based head/tail 
     * fill, each conditional below ensures that the subsequent offsets are valid. */

    *(ma_u32 *)(s + 0) = c32;
    *(ma_u32 *)(s + n - 4) = c32;
    if (n <= 8) return dest;
    *(ma_u32 *)(s + 4) = c32;
    *(ma_u32 *)(s + 8) = c32;
    *(ma_u32 *)(s + n - 12) = c32;
    *(ma_u32 *)(s + n - 8) = c32;
    if (n <= 24) return dest;
    *(ma_u32 *)(s + 12) = c32;
    *(ma_u32 *)(s + 16) = c32;
    *(ma_u32 *)(s + 20) = c32;
    *(ma_u32 *)(s + 24) = c32;
    *(ma_u32 *)(s + n - 28) = c32;
    *(ma_u32 *)(s + n - 24) = c32;
    *(ma_u32 *)(s + n - 20) = c32;
    *(ma_u32 *)(s + n - 16) = c32;

    /* Align to a multiple of 8, so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as mush as is practical 
     * without introducing additional branching. */

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been filled,
     * so any remainder when n drops below 32 can be safely ignored. */

    ma_u64 c64 = c32 | ((ma_u64)c32 << 32);
    for (; n >= 32; n -= 32, s += 32) {
        *(ma_u64 *)(s + 0) = c64;
        *(ma_u64 *)(s + 8) = c64;
        *(ma_u64 *)(s + 16) = c64;
        *(ma_u64 *)(s + 24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif
    return dest;
}

AMWAPI int32_t AMWAPIENTRY 
iainit(struct ipomoeaalba *ia)
{
    (void)ia;
    return result_success;
}

AMWAPI void AMWAPIENTRY 
iafini(struct ipomoeaalba *ia)
{
    (void)ia;
}

AMWAPI void * AMWAPIENTRY 
iaalloc(struct ipomoeaalba *ia, size_t size, size_t alignment, uint64_t tag)
{
    (void)ia;
    (void)size;
    (void)alignment;
    (void)tag;
    return NULL;
}

AMWAPI void * AMWAPIENTRY 
iarealloc(struct ipomoeaalba *ia, void *allocation, size_t size, size_t alignment, uint64_t tag)
{
    (void)ia;
    (void)allocation;
    (void)size;
    (void)alignment;
    (void)tag;
    return NULL;
}

AMWAPI void AMWAPIENTRY 
iafree(struct ipomoeaalba *ia, uint64_t tag)
{
    (void)ia;
    (void)tag;
}
