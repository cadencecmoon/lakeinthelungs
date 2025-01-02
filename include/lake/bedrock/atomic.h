#ifndef _AMW_ATOMIC_H
#define _AMW_ATOMIC_H

#include <lake/bedrock/defines.h>

#if defined(AMW_ARCH_X86) || defined(AMW_ARCH_AMD64)
    #include <lake/bedrock/x86/atomic_asm.h>
#elif defined(AMW_ARCH_ARM) || defined(AMW_ARCH_AARCH64)
    #include <lake/bedrock/arm/atomic_asm.h>
#elif defined(AMW_ARCH_RISCV)
    #include <lake/bedrock/riscv/atomic_asm.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef AMW_ATOMIC(b32)     at_b32;  

typedef AMW_ATOMIC(s8)      at_s8;
typedef AMW_ATOMIC(s16)     at_s16;
typedef AMW_ATOMIC(s32)     at_s32;
typedef AMW_ATOMIC(s64)     at_s64;

typedef AMW_ATOMIC(u8)      at_u8;
typedef AMW_ATOMIC(u16)     at_u16;
typedef AMW_ATOMIC(u32)     at_u32;
typedef AMW_ATOMIC(u64)     at_u64;

typedef AMW_ATOMIC(sptr)    at_sptr;
typedef AMW_ATOMIC(uptr)    at_uptr;
typedef AMW_ATOMIC(ssize)   at_ssize;
typedef AMW_ATOMIC(usize)   at_usize;

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

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ATOMIC_H */
