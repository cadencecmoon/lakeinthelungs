#pragma once

#include <amw/bedrock.h>

#define _ARCH_BYTEORDER_LE 1234
#define _ARCH_BYTEORDER_BE 4321

#ifndef _ARCH_BYTEORDER
    #if defined(PLATFORM_LINUX)
        #include <endian.h>
        #define _ARCH_BYTEORDER __BYTE_ORDER
    #elif defined(__sun) && defined(__SVR4) /* solaris */
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__OpenBSD__) || defined(__DragonFly__)
        #include <endian.h>
        #define _ARCH_BYTEORDER BYTE_ORDER
    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
        #include <sys/endian.h>
        #define _ARCH_BYTEORDER BYTE_ORDER
    /* predefs from newer GCC and CLANG versions */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        #if defined(ARCH_MIPS) || defined(ARCH_POWER) || defined(ARCH_SPARC)
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_BE
        #else
            #define _ARCH_BYTEORDER _ARCH_BYTEORDER_LE
        #endif
    #endif
#endif

#ifndef _ARCH_FLOAT_WORD_ORDER
    /* predefs from newer GCC and Clang versions */
    #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__FLOAT_WORD_ORDER__)
        #if (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_LE
        #elif (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__MAVERICK__)
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_LE
    #elif (defined(ARCH_ARM) || defined(__thumb__)) && !defined(__VFP_FP__) && !defined(__ARM_EABI__)
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER_BE
    #else
        #define _ARCH_FLOAT_WORD_ORDER _ARCH_BYTEORDER
    #endif
#endif

#if _ARCH_FLOAT_WORD_ORDER == _ARCH_BYTEORDER_LE
    #define ARCH_FLOAT_LIL_ENDIAN 1
#else
    #define ARCH_FLOAT_BIG_ENDIAN 1
#endif

attr_inline attr_const 
u16 bswap16(u16 x) 
{
#if HAS_BUILTIN(__builtin_bswap16)
    return __builtin_bswap16(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ushort(x);
#endif
}

attr_inline attr_const 
u32 bswap32(u32 x) 
{
#if HAS_BUILTIN(__builtin_bswap32)
    return __builtin_bswap32(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_ulong(x);
#endif
}

attr_inline attr_const 
u64 bswap64(u64 x) 
{
#if HAS_BUILTIN(__builtin_bswap64)
    return __builtin_bswap64(x);
#elif CC_MSVC_VERSION_CHECK(14,0,0)
    return _byteswap_uint64(x);
#endif
}

#if _ARCH_BYTEORDER == _ARCH_BYTEORDER_LE
    #define ARCH_LIL_ENDIAN 1
    #define bswap16le(x) (x)
    #define bswap32le(x) (x)
    #define bswap64le(x) (x)
    #define bswap16be(x) bswap16(x) 
    #define bswap32be(x) bswap32(x)
    #define bswap64be(x) bswap64(x)
#else
    #define ARCH_BIG_ENDIAN 1
    #define bswap16le(x) bswap16(x) 
    #define bswap32le(x) bswap32(x)
    #define bswap64le(x) bswap64(x)
    #define bswap16be(x) (x)
    #define bswap32be(x) (x)
    #define bswap64be(x) (x)
#endif

