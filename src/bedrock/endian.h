#pragma once

#include "defines.h"

#include "intrinsics.h"
#if defined(ARCH_X86) || defined(ARCH_AMD64)
    #include "x86/bswap.h"
#elif defined(ARCH_ARM) || defined(ARCH_AARCH64)
    #include "arm/bswap.h"
#elif defined(ARCH_RISCV)
    #include "riscv/bswap.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _ARCH_BYTEORDER_LE 1234 /* little endian */
#define _ARCH_BYTEORDER_BE 4321 /* big endian */

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

#ifdef __cplusplus
}
#endif
