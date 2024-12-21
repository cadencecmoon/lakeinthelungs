#ifndef _AMW_X86_BSWAP_H
#define _AMW_X86_BSWAP_H

#include <lake/defines.h>

/* Byte swap 16-bit integer. */
AMW_INLINE uint16_t bswap16(uint16_t x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_ushort(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
    __asm lea  edx, [x]
    __asm mov  ax, [edx]
    __asm xchg ah, al
    __asm mov  [edx], ax
    return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__("xchgb 0, %h0" : "=Q" (x) : "0" (x));
    return x;
#else
    __asm__("rorw $8, %0" : "+r" (x));
    return x;
#endif
}

/* Byte swap 32-bit integer. */
AMW_INLINE uint32_t bswap32(uint32_t x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_ulong(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#else
    #if AMW_ARCH_X86_CHECK(4)
        __asm__("bswap %0" : "+r" (x));
        return x;
    #else /* i386 has broken bswap */
        __asm__("rorw $8, %w0 ; rorl $16, %0 ; rorw $8, %w0" : "+r" (x));
        return x;
    #endif
#endif
}

AMW_INLINE uint64_t bswap64(uint64_t x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_uint64(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
	__asm lea   edx, [x]
	__asm mov   eax, [edx]
	__asm bswap eax
	__asm xchg  eax, [edx+4]
	__asm bswap eax
	__asm mov   [edx], eax
	return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#else
    #if AMW_ARCH_X86_CHECK(4)
        union {
            struct {
                uint32_t a, b;
            } s;
            uint64_t u;
        } v;
        v.u = x;
        __asm__("bswapl %0 ; bswapl %1 ; xchgl %0, %1"
                : "=r"(v.s.a), "=r" (v.s.b)
                : "0"(v.s.a), "1" (v.s.b));
    #else /* i386 has broken bswap */
        uint32_t hi, lo;
        lo = (uint32_t)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (uint32_t)(x & 0xFFFFFFFF);
        x = bswap32(lo);
        x <<= 32;
        x |= bswap32(hi);
        return x;
    #endif
#endif
}

#endif /* _AMW_X86_BSWAP_H */
