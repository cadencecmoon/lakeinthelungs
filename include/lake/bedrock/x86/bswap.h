#ifndef _AMW_X86_BSWAP_H
#define _AMW_X86_BSWAP_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Byte swap 16-bit integer. */
AMW_INLINE u16 bswap16(u16 x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_ushort(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
    __asm__ volatile ("lea  edx, [x] ;"
                      "mov  ax, [edx];"
                      "xchg ah, al   ;"
                      "mov  [edx], ax "
                    : "+r" (x));
    return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__ volatile ("xchgb 0, %h0" : "=Q" (x) : "0" (x));
    return x;
#else
    __asm__ volatile ("rorw $8, %0" : "+r" (x));
    return x;
#endif
}

/* Byte swap 32-bit integer. */
AMW_INLINE u32 bswap32(u32 x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_ulong(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
    __asm__ volatile ("lea   edx, [x]  ;"
                  "mov   eax, [edx];"
                  "bswap eax       ;"
                  "mov   [edx], eax "
                : "+r" (x));
	return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__ volatile ("bswapl %0" : "=r" (x) : "0" (x));
    return x;
#else
    #if AMW_ARCH_X86_CHECK(4)
        __asm__ volatile ("bswap %0" : "+r" (x));
        return x;
    #else /* i386 has broken bswap */
        __asm__ volatile ("rorw $8, %w0 ;" 
                          "rorl $16, %0 ;"
                          "rorw $8, %w0  " 
                        : "+r" (x));
        return x;
    #endif
#endif
}

AMW_INLINE u64 bswap64(u64 x)
{
#if AMW_HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_AMD64)
    return _byteswap_uint64(x);
#elif AMW_CC_MSVC_VERSION_CHECK(14,0,0) && defined(AMW_ARCH_X86)
    __asm__ volatile ("lea   edx, [x]    ;"
                      "mov   eax, [edx]  ;"
                      "bswap eax         ;"
                      "xchg  eax, [edx+4];"
                      "bswap eax         ;"
                      "mov   [edx], eax   "
                    : "+r" (x));
	return x;
#elif defined(AMW_ARCH_AMD64)
    __asm__ volatile ("bswapq %0" : "=r" (x) : "0" (x));
    return x;
#else
    #if AMW_ARCH_X86_CHECK(4)
        union {
            struct {
                u32 a, b;
            } s;
            u64 u;
        } v;
        v.u = x;
        __asm__ volatile ("bswapl %0   ;" 
                          "bswapl %1   ;" 
                          "xchgl  %0, %1"
                        : "=r"(v.s.a), 
                          "=r" (v.s.b)
                        : "0"(v.s.a), 
                          "1" (v.s.b));
    #else /* i386 has broken bswap */
        u32 hi, lo;
        lo = (u32)(x & 0xFFFFFFFF);
        x >>= 32;
        hi = (u32)(x & 0xFFFFFFFF);
        x = bswap32(lo);
        x <<= 32;
        x |= bswap32(hi);
        return x;
    #endif
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _AMW_X86_BSWAP_H */
