#include <amwe/bedrock.h>

#if LAKE_BYTEORDER == LAKE_BYTEORDER_LE
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

void *bedrock_memset(void *restrict dest, s32 c, usize size)
{
    u8 *s = dest;
    usize k;

    /* Fill head and tail with minimal branching. Each conditional 
     * ensures that all the subsequently used offsets are well-defined 
     * and in the dest region. */

    if (!size) return dest;
    s[0] = c;
    s[size-1] = c;
    if (size <= 2) return dest;
    s[1] = c;
    s[2] = c;
    s[size-2] = c;
    s[size-3] = c;
    if (size <= 6) return dest;
    s[3] = c;
    s[size-4] = c;
    if (size <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary, and truncate n to 
     * a multiple of 4. The previous code already took care of any head/tail 
     * that get cut off by the alignment. */

    k = -(uptr)s & 3;
    s += k;
    size -= k;
    size &= -4;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 c32 = ((alias_u32) - 1) / 255 * (u8)c;

    /* In preparation to copy 32 bits at a time, aligned on an 8-byte boundary,
     * fill head/tail up to 28 bytes each. As in the initial byte-based head/tail 
     * fill, each conditional below ensures that the subsequent offsets are valid. */

    *(alias_u32 *)(s + 0) = c32;
    *(alias_u32 *)(s + size - 4) = c32;
    if (size <= 8) return dest;
    *(alias_u32 *)(s + 4) = c32;
    *(alias_u32 *)(s + 8) = c32;
    *(alias_u32 *)(s + size - 12) = c32;
    *(alias_u32 *)(s + size - 8) = c32;
    if (size <= 24) return dest;
    *(alias_u32 *)(s + 12) = c32;
    *(alias_u32 *)(s + 16) = c32;
    *(alias_u32 *)(s + 20) = c32;
    *(alias_u32 *)(s + 24) = c32;
    *(alias_u32 *)(s + size - 28) = c32;
    *(alias_u32 *)(s + size - 24) = c32;
    *(alias_u32 *)(s + size - 20) = c32;
    *(alias_u32 *)(s + size - 16) = c32;

    /* Align to a multiple of 8, so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as mush as is practical 
     * without introducing additional branching. */

    k = 24 + ((uptr)s & 4);
    s += k;
    size -= k;

    /* If this loop is reached, 28 tail bytes have already been filled,
     * so any remainder when n drops below 32 can be safely ignored. */

    alias_u64 c64 = c32 | ((alias_u64)c32 << 32);
    for (; size >= 32; size -= 32, s += 32) {
        *(alias_u64 *)(s + 0) = c64;
        *(alias_u64 *)(s + 8) = c64;
        *(alias_u64 *)(s + 16) = c64;
        *(alias_u64 *)(s + 24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; size; size--, s++) *s = c;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dest;
}

void *bedrock_memcpy(void *restrict dest, const void *restrict src, usize size)
{
    u8       *d = dest;
    const u8 *s = src;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 w, x;

	for (; (uptr)s % 4 && size; size--) *d++ = *s++;

	if ((uptr)d % 4 == 0) {
		for (; size >= 16; s += 16, d += 16, size -= 16) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			*(alias_u32 *)(d+8) = *(alias_u32 *)(s+8);
			*(alias_u32 *)(d+12) = *(alias_u32 *)(s+12);
		}
		if (size & 8) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			d += 8; s += 8;
		}
		if (size & 4) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			d += 4; s += 4;
		}
		if (size & 2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (size & 1) {
			*d = *s;
		}
		return dest;
	}

	if (size >= 32) switch ((uptr)d % 4) {
	case 1:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		size -= 3;
		for (; size >= 17; s += 16, d += 16, size -= 16) {
			x = *(alias_u32 *)(s+1);
			*(alias_u32 *)(d+0) = (w LS 24) | (x RS 8);
			w = *(alias_u32 *)(s+5);
			*(alias_u32 *)(d+4) = (x LS 24) | (w RS 8);
			x = *(alias_u32 *)(s+9);
			*(alias_u32 *)(d+8) = (w LS 24) | (x RS 8);
			w = *(alias_u32 *)(s+13);
			*(alias_u32 *)(d+12) = (x LS 24) | (w RS 8);
		}
		break;
	case 2:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		size -= 2;
		for (; size >= 18; s += 16, d += 16, size -=16) {
			x = *(alias_u32 *)(s+2);
			*(alias_u32 *)(d+0) = (w LS 16) | (x RS 16);
			w = *(alias_u32 *)(s+6);
			*(alias_u32 *)(d+4) = (x LS 16) | (w RS 16);
			x = *(alias_u32 *)(s+10);
			*(alias_u32 *)(d+8) = (w LS 16) | (x RS 16);
			w = *(alias_u32 *)(s+14);
			*(alias_u32 *)(d+12) = (x LS 16) | (w RS 16);
		}
		break;
	case 3:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		size -= 1;
		for (; size >= 19; s += 16, d += 16, size -= 16) {
			x = *(alias_u32 *)(s+3);
			*(alias_u32 *)(d+0) = (w LS 8) | (x RS 24);
			w = *(alias_u32 *)(s+7);
			*(alias_u32 *)(d+4) = (x LS 8) | (w RS 24);
			x = *(alias_u32 *)(s+11);
			*(alias_u32 *)(d+8) = (w LS 8) | (x RS 24);
			w = *(alias_u32 *)(s+15);
			*(alias_u32 *)(d+12) = (x LS 8) | (w RS 24);
		}
		break;
	}
	if (size & 16) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (size & 8) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (size & 4) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (size & 2) {
		*d++ = *s++; *d++ = *s++;
	}
	if (size & 1) {
		*d = *s;
	}
	return dest;
#else
    /* Pure C fallback with no aliasing violations. */
    for (; size; size--) *d++ = *s++;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dest;
}

void *bedrock_memrchr(const void *restrict mem, s32 c, usize size)
{
    const u8 *raw = mem;
    c = (u8)c;
    while (size--) if (raw[size] == c) return (void *)(raw + size);
    return 0;
}

usize bedrock_strlen(const char *str)
{
    const char *a = str;
#if LAKE_HAS_MAY_ALIAS
    const alias_word *w;
    for (; (uptr)str % sizeof(usize); str++) 
        if (!*str) return str - a;
    for (w = (const void *)str; !((*w) - ((usize) - 1/UCHAR_MAX) * (UCHAR_MAX/2 + 1)); w++);
    str = (const void *)w;
#endif /* LAKE_HAS_MAY_ALIAS */
    for (; *str; str++);
    return str - a;
}
