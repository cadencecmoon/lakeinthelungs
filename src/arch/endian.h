#ifndef _AMW_ENDIAN_H
#define _AMW_ENDIAN_H

#include "base/defines.h"

#define AMW_BYTEORDER_LE 1234 /* little endian */
#define AMW_BYTEORDER_BE 4321 /* big endian */

#ifndef AMW_BYTEORDER
    #if defined(AMW_PLATFORM_LINUX)
        #include <endian.h>
        #define AMW_BYTEORDER __BYTE_ORDER
    #elif defined(__sun) && defined(__SVR4) /* solaris */
        #include <sys/byteorder.h>
        #if defined(_LITTLE_ENDIAN)
            #define AMW_BYTEORDER AMW_BYTEORDER_LE
        #elif defined(_BIG_ENDIAN)
            #define AMW_BYTEORDER AMW_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__OpenBSD__) || defined(__DragonFly__)
        #include <endian.h>
        #define AMW_BYTEORDER BYTE_ORDER
    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
        #include <sys/endian.h>
        #define AMW_BYTEORDER BYTE_ORDER
    /* predefs from newer GCC and CLANG versions */
    #elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define AMW_BYTEORDER AMW_BYTEORDER_LE
        #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define AMW_BYTEORDER AMW_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #else  
        /* not supported for now, but a possible case */
        #if defined(AMW_ARCH_MIPS) || defined(AMW_ARCH_POWER) || defined(AMW_ARCH_SPARC)
            #define AMW_BYTEORDER AMW_BYTEORDER_BE
        #else
            #define AMW_BYTEORDER AMW_BYTEORDER_LE
        #endif
    #endif
#endif

#ifndef AMW_FLOAT_WORD_ORDER
    /* predefs from newer GCC and Clang versions */
    #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__FLOAT_WORD_ORDER__)
        #if (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__)
            #define AMW_FLOAT_WORD_ORDER = AMW_BYTEORDER_LE
        #elif (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__)
            #define AMW_FLOAT_WORD_ORDER = AMW_BYTEORDER_BE
        #else
            #error Unsupported endianness.
        #endif
    #elif defined(__MAVERICK__)
        #define AMW_FLOAT_WORD_ORDER AMW_BYTEORDER_LE
    #elif (defined(AMW_ARCH_ARM) || defined(__thumb__)) && !defined(__VFP_FP__) && !defined(__ARM_EABI__)
        #define AMW_FLOAT_WORD_ORDER AMW_BYTEORDER_BE
    #else
        #define AMW_FLOAT_WORD_ORDER AMW_BYTEORDER
    #endif
#endif

#if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
    #include <arch/x86/bswap.h>
#elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
    #include <arch/arm/bswap.h>
#elif defined(AMW_ARCH_RISCV)
    #include <arch/riscv/bswap.h>
#endif

#if AMW_BYTEORDER == AMW_BYTEORDER_LE
    #define AMW_LIL_ENDIAN (1234)
    #define bswap16le(x) (x)
    #define bswap32le(x) (x)
    #define bswap64le(x) (x)
    #define bswap16be(x) bswap16(x) 
    #define bswap32be(x) bswap32(x)
    #define bswap64be(x) bswap64(x)
#else
    #define AMW_BIG_ENDIAN (4321)
    #define bswap16le(x) bswap16(x) 
    #define bswap32le(x) bswap32(x)
    #define bswap64le(x) bswap64(x)
    #define bswap16be(x) (x)
    #define bswap32be(x) (x)
    #define bswap64be(x) (x)
#endif

#endif /* _AMW_ENDIAN_H */
