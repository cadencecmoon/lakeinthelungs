#include "endian.h"
#include "utility.h"

#if defined(LAKE_LITTLE_ENDIAN)
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

#define FIBER_INVALID (SIZE_MAX)
/* how many past frame times are used to compute the median */
#define FRAME_TIME_COUNT (128)

#define ONES ((usize) - (1/UCHAR_MAX))
#define HIGHS (ONES * (UCHAR_MAX/2 + 1))
#define HASZERO(x) (((x) - ONES) & ~(x) & HIGHS)

void *lake_memset(void *restrict dst, s32 c, usize n)
{
    u8 *s = dst;
    usize k;

    /* Fill head and tail with minimal branching. Each conditional 
     * ensures that all the subsequently used offsets are well-defined 
     * and in the dest region. */

    if (!n) return dst;
    s[0] = c;
    s[n-1] = c;
    if (n <= 2) return dst;
    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;
    if (n <= 6) return dst;
    s[3] = c;
    s[n-4] = c;
    if (n <= 8) return dst;

    /* Advance pointer to align it at a 4-byte boundary, and truncate n to 
     * a multiple of 4. The previous code already took care of any head/tail 
     * that get cut off by the alignment. */

    k = -(uptr)s & 3;
    s += k;
    n -= k;
    n &= -4;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 c32 = ((alias_u32) - 1) / 255 * (u8)c;

    /* In preparation to copy 32 bits at a time, aligned on an 8-byte boundary,
     * fill head/tail up to 28 bytes each. As in the initial byte-based head/tail 
     * fill, each conditional below ensures that the subsequent offsets are valid. */

    *(alias_u32 *)(s + 0) = c32;
    *(alias_u32 *)(s + n - 4) = c32;
    if (n <= 8) return dst;
    *(alias_u32 *)(s + 4) = c32;
    *(alias_u32 *)(s + 8) = c32;
    *(alias_u32 *)(s + n - 12) = c32;
    *(alias_u32 *)(s + n - 8) = c32;
    if (n <= 24) return dst;
    *(alias_u32 *)(s + 12) = c32;
    *(alias_u32 *)(s + 16) = c32;
    *(alias_u32 *)(s + 20) = c32;
    *(alias_u32 *)(s + 24) = c32;
    *(alias_u32 *)(s + n - 28) = c32;
    *(alias_u32 *)(s + n - 24) = c32;
    *(alias_u32 *)(s + n - 20) = c32;
    *(alias_u32 *)(s + n - 16) = c32;

    /* Align to a multiple of 8, so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as mush as is practical 
     * without introducing additional branching. */

    k = 24 + ((uptr)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been filled,
     * so any remainder when n drops below 32 can be safely ignored. */

    alias_u64 c64 = c32 | ((alias_u64)c32 << 32);
    for (; n >= 32; n -= 32, s += 32) {
        *(alias_u64 *)(s + 0) = c64;
        *(alias_u64 *)(s + 8) = c64;
        *(alias_u64 *)(s + 16) = c64;
        *(alias_u64 *)(s + 24) = c64;
    }
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--, s++) *s = c;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dst;
}

void *lake_memcpy(void *restrict dst, void const *restrict src, usize n)
{
    u8       *d = dst;
    u8 const *s = src;

#if LAKE_HAS_MAY_ALIAS
    alias_u32 w, x;

	for (; (uptr)s % 4 && n; n--) *d++ = *s++;

	if ((uptr)d % 4 == 0) {
		for (; n >= 16; s += 16, d += 16, n -= 16) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			*(alias_u32 *)(d+8) = *(alias_u32 *)(s+8);
			*(alias_u32 *)(d+12) = *(alias_u32 *)(s+12);
		}
		if (n & 8) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			*(alias_u32 *)(d+4) = *(alias_u32 *)(s+4);
			d += 8; s += 8;
		}
		if (n & 4) {
			*(alias_u32 *)(d+0) = *(alias_u32 *)(s+0);
			d += 4; s += 4;
		}
		if (n & 2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n & 1) {
			*d = *s;
		}
		return dst;
	}

	if (n >= 32) switch ((uptr)d % 4) {
	case 1:
		w = *(alias_u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		n -= 3;
		for (; n >= 17; s += 16, d += 16, n -= 16) {
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
		n -= 2;
		for (; n >= 18; s += 16, d += 16, n -=16) {
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
		n -= 1;
		for (; n >= 19; s += 16, d += 16, n -= 16) {
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
	if (n & 16) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 8) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 4) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 2) {
		*d++ = *s++; *d++ = *s++;
	}
	if (n & 1) {
		*d = *s;
	}
	return dst;
#else
    /* Pure C fallback with no aliasing violations. */
    for (; n; n--) *d++ = *s++;
#endif /* LAKE_HAS_MAY_ALIAS */
    return dst;
}

s32 lake_memcmp(void const *vl, void const *vr, usize n)
{
    u8 const *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

void *lake_memrchr(void const *restrict mem, s32 c, usize n)
{
    u8 const *raw = mem;
    c = (u8)c;
    while (n--) if (raw[n] == c) return (void *)(raw + n);
    return 0;
}

usize lake_strlen(char const *str)
{
    char const *a = str;
#if LAKE_HAS_MAY_ALIAS
    alias_word const *w;
    for (; (uptr)str % sizeof(usize); str++) 
        if (!*str) return str - a;
    for (w = (void const *)str; !((*w) - ((usize) - 1/UCHAR_MAX) * (UCHAR_MAX/2 + 1)); w++);
    str = (void const *)w;
#endif /* LAKE_HAS_MAY_ALIAS */
    for (; *str; str++);
    return str - a;
}

s32 lake_strncmp(char const *vl, char const *vr, usize n)
{
    u8 const *l = (void const *)vl, *r = (void const *)vr;
    if (!n--) return 0;
    for (; *l && *r && n && *l == *r; n--, l++, r++);
    return *l - *r;
}

char *lake_strncpy(char *restrict dst, char const *restrict str, usize n)
{
    usize       *wd;
    usize const *ws;

    if (((uptr)str & (sizeof(usize)-1)) == ((uptr)dst & (sizeof(usize)-1))) {
        for (; ((uptr)str & (sizeof(usize)-1)) && n && (*dst = *str); n--, str++, dst++);
        if (!n || !*str) goto tail;

        wd = (void *)dst;
        ws = (void const *)str;
        for (; n >= sizeof(usize) && !HASZERO(*ws); n -= sizeof(usize), ws++, wd++) *wd = *ws;
        dst = (void *)wd;
        str = (void const *)ws;
    }
    for (; n && (*dst = *str); n--, str++, dst++);
tail:
    lake_memset(dst, 0, n);
    return dst;
}

char *lake_strchrnul(char const *str, s32 c)
{
    usize *w, k;
    c = (u8)c;
    if (!c) return (char *)str + lake_strlen(str);

    for (; (uptr)str % sizeof(usize); str++)
        if (!*str || *(u8 *)str == c) return (char *)str;
    k = ONES * c;

    for (w = (void *)str; !HASZERO(*w) && !HASZERO(*w ^ k); w++);
    for (str = (void *)w; *str && *(u8 *)str != c; str++);
    return (char *)str;
}

/* a ring buffer of delta times, invalid entries are zero */
static f64 g_recorded_frame_times[FRAME_TIME_COUNT] = {0.0};
/* the most recently written entry in the ring buffer */
static u32 g_recorded_frame_time_idx = FRAME_TIME_COUNT - 1;
static f64 g_last_print_time = 0.0;

void lake_frame_time_record(u64 time_app_start, u64 time_now, f64 dt_frequency_reciprocal)
{
    g_recorded_frame_time_idx++;
    if (g_recorded_frame_time_idx >= FRAME_TIME_COUNT)
        g_recorded_frame_time_idx -= FRAME_TIME_COUNT;
    g_recorded_frame_times[g_recorded_frame_time_idx] = ((f64)(time_now - time_app_start) * dt_frequency_reciprocal);
}

static s32 compare_floats(void const *lhs_raw, void const *rhs_raw)
{
    f32 const lhs = *((f32 const *)lhs_raw);
    f32 const rhs = *((f32 const *)rhs_raw);
    return (lhs < rhs) ? -1 : ((lhs == rhs) ? 0 : 1);
}

f32 lake_frame_time_median(void)
{
    f32 frame_times[FRAME_TIME_COUNT];
    u32 recorded_count = 0;
    for (s32 i = 0; i < FRAME_TIME_COUNT-1; i++) {
        s32 lhs = (g_recorded_frame_time_idx + FRAME_TIME_COUNT - i) % FRAME_TIME_COUNT;
        s32 rhs = (g_recorded_frame_time_idx + FRAME_TIME_COUNT - i - 1) % FRAME_TIME_COUNT;
        if (g_recorded_frame_times[lhs] != 0.0 && g_recorded_frame_times[rhs] != 0.0) {
            frame_times[recorded_count] = (f32)(g_recorded_frame_times[lhs] - g_recorded_frame_times[rhs]);
            recorded_count++;
        }
    }
    if (recorded_count == 0) return 0.0f;

    /* sort our frame times */
    qsort(frame_times, recorded_count, sizeof(frame_times[0]), compare_floats);
    /* calculate the median */
    return frame_times[recorded_count / 2];
}

void lake_frame_time_print(f32 interval_ms)
{
    f64 current_time = g_recorded_frame_times[g_recorded_frame_time_idx];
    if (g_last_print_time == 0.0f || g_last_print_time + (f64)interval_ms < current_time) {
        f32 frame_time = lake_frame_time_median();
        if (frame_time > 0.0f) {
            lake_trace("Frame time: %.3f ms (%.0f FPS)", frame_time, 1000/frame_time);
        }
        g_last_print_time = current_time;
    }
}
