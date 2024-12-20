#ifndef _AMW_ATOMIC_H
#define _AMW_ATOMIC_H

#include <base/defines.h>

#if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
    #include <arch/x86/atomic_asm.h>
#elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
    #include <arch/arm/atomic_asm.h>
#elif defined(AMW_ARCH_RISCV)
    #include <arch/riscv/atomic_asm.h>
#endif

typedef AMW_ATOMIC(bool)                at_bool_t;

typedef AMW_ATOMIC(char)                at_char_t;
typedef AMW_ATOMIC(signed char)         at_schar_t;
typedef AMW_ATOMIC(unsigned char)       at_uchar_t;

typedef AMW_ATOMIC(short)               at_short_t;
typedef AMW_ATOMIC(unsigned short)      at_ushort_t;

typedef AMW_ATOMIC(int)                 at_int_t;
typedef AMW_ATOMIC(unsigned int)        at_uint_t;

typedef AMW_ATOMIC(long)                at_long_t;
typedef AMW_ATOMIC(unsigned long)       at_ulong_t;

typedef AMW_ATOMIC(long long)           at_llong_t;
typedef AMW_ATOMIC(unsigned long long)  at_ullong_t;

typedef AMW_ATOMIC(size_t)              at_size_t;
typedef AMW_ATOMIC(ptrdiff_t)           at_ptrdiff_t;

typedef AMW_ATOMIC(intptr_t)            at_intptr_t;
typedef AMW_ATOMIC(uintptr_t)           at_uintptr_t;

typedef AMW_ATOMIC(uint8_t)             at_uint8_t;
typedef AMW_ATOMIC(uint16_t)            at_uint16_t;
typedef AMW_ATOMIC(uint32_t)            at_uint32_t;
typedef AMW_ATOMIC(uint64_t)            at_uint64_t;

typedef AMW_ATOMIC(int8_t)              at_int8_t;
typedef AMW_ATOMIC(int16_t)             at_int16_t;
typedef AMW_ATOMIC(int32_t)             at_int32_t;
typedef AMW_ATOMIC(int64_t)             at_int64_t;

/** Relaxed atomic operations.
 *
 *  When an operation on an atomic variable is not expected to synchronize 
 *  with operations on other (atomic or non-atomic) variables, no memory 
 *  barriers are needed and the relaxed memory ordering can be used. These 
 *  macros make such uses less daunting, but not invisible. */

#define at_store_relaxed(var, value) \
    at_store_explicit(var, value, memory_model_relaxed)
#define at_read_relaxed(var) \
    at_read_explicit(var, memory_model_relaxed)

#define at_compare_exchange_strong_relaxed(dst, exp, src)    \
    at_compare_exchange_strong_relaxed(dst, exp, src,        \
                                       memory_model_relaxed, \
                                       memory_model_relaxed)
#define at_compare_exchange_weak_relaxed(dst, exp, src)      \
    at_compare_exchange_weak_relaxed(dst, exp, src,          \
                                     memory_model_relaxed,   \
                                     memory_model_relaxed)

#define at_fetch_add_relaxed(rmw, arg) \
    at_fetch_add_explicit(rmw, arg, memory_model_relaxed) 
#define at_fetch_sub_relaxed(rmw, arg) \
    at_fetch_sub_explicit(rmw, arg, memory_model_relaxed)
#define at_fetch_or_relaxed(rmw, arg) \
    at_fetch_or_explicit(rmw, arg, memory_model_relaxed)
#define at_fetch_xor_relaxed(rmw, arg) \
    at_fetch_xor_explicit(rmw, arg, memory_model_relaxed)
#define at_fetch_and_relaxed(rmw, arg) \
    at_fetch_and_explicit(rmw, arg, memory_model_relaxed)

#define at_test_and_set_relaxed(flag) \
    at_test_and_set_explicit(flag, memory_model_relaxed)
#define at_clear_relaxed(flag) \
    at_clear_explicit(flag, memory_model_relaxed)

#endif /* _AMW_ATOMIC_H */
