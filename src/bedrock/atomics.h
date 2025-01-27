#pragma once

#include "defines.h"
#include "intrinsics.h"

/** @file arch/atomics.h
 *
 *  Functions for atomic sequences, access and writes of data that will be shared between CPU threads.
 *  They are necessary to guarantee safe operations in a multithreaded environment, and if used 
 *  correctly (I don't do that I guess) will prevent data races, deadlocks and other pesky issues.
 *  Most modern architectures have a pretty straightforward lockless implementation of atomic operations,
 *  on platforms that don't we'll just simulate the behaviour with simple locks instead. Defines like 
 *  ARCH_ATOMIC_BOOL_LOCK_FREE if non-zero, guarantee that the data type is lockless. An inline function 
 *  at_is_lock_free(obj) can be used to determine if a custom data struct declared as an atomic
 *  type is lock free. A macro ARCH_ATOMIC(type) is defined for enforcing some core rules of how the variable
 *  of this atomic type should be handled, that are both used by the compiler & CPU, as is an information
 *  to the user that this variable should be accessed atomically. Common atomic types are defined in riven.h.
 *
 *  @enum memory_model  An atomic operation can both constrain code motion and be mapped to hardware instructions
 *      for synchronization between threads. To which extent this happens is controlled by the memory models,
 *      which are listed in this enum in approximately ascending order of strength. The descriptions of each 
 *      memory model is only meant to roughly illustrate the effects and is not a specification. C++11 memory 
 *      models can be seen for precise semantics.
 *
 *      memory_model_relaxed  Implies no inter-thread ordering constraints.
 *      memory_model_consume  Is implemented either as relaxed, or as the stronger ACQUIRE model, depending on the platform.
 *      memory_model_acquire  Creates an inter-thread happens-before constraint from the release (or stronger) semantic store 
 *                            to this acquire load. Can prevent hoisting of code to before the operation.
 *      memory_model_release  Creates an inter-thread happens-before constraint to acquire (or stronger) semantic loads that 
 *                            read from this release store. Can prevent sinking of code to after the operation.
 *      memory_model_acq_rel  Combines the effects of both ACQUIRE and RELEASE models.
 *      memory_model_seq_cst  Enforces total ordering with all other SEQ_CST operations.
 *
 *  The memory model is enforced using different synchronization methods. They are implemented in the following:
 *      @fn at_compiler_barrier
 *      @fn at_thread_fence
 *      @fn at_signal_fence
 *
 *  All atomic operations have an explicit and implicit version. The explicit versions have one more argument, that allows
 *  to specify a memory model constraint for this operation. The implicit versions assume memory_model_seq_cst. Additional
 *  constraints come from a weak and strong memory model for the read-modify-write compare exchange operations. This is 
 *  only desctinct for platforms with a weak memory ordering, thus on platforms like x86 the weak compare exchange gets 
 *  redefined in the strong memory model strong instead.
 *
 *  The following atomic operations are defined:
 *      @fn at_init                 May help with correctly initializing new atomic variables.
 *      @fn at_store                Atomically write value of src to dst.
 *      @fn at_read                 Atomically read the value of src, no modify operations.
 *      @fn at_compare_exchange     Compares the contents of expected, and if equal writes desired to ptr.
 *      @fn at_exchange             Stores the contents of val into ptr. The original value of ptr is returned.
 *      @fn at_fetch_add            Atomic add operation, returns old value.
 *      @fn at_fetch_sub            Atomic sub operation, returns old value.
 *      @fn at_fetch_or             Atomic OR |= operation, returns old value.
 *      @fn at_fetch_xor            Atomic XOR ^= operation, returns old value.
 *      @fn at_fetch_and            Atomic AND &= operation, returns old value.
 *      @fn at_test_and_set         An atomic byte check, can be used to implement flags or spinlocks.
 *      @fn at_clear                Zeroes out *ptr that is either bool or char, after the operation *ptr contains 0.
 */
#if defined(ARCH_X86) || defined(ARCH_AMD64)
    #include "x86/atomic_asm.h"
#elif defined(ARCH_ARM) || defined(ARCH_AARCH64)
    #include "arm/atomic_asm.h"
#elif defined(ARCH_RISCV)
    #include "riscv/atomic_asm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef ARCH_ATOMIC(b32)    at_b32;  

typedef ARCH_ATOMIC(s8)     at_s8;
typedef ARCH_ATOMIC(s16)    at_s16;
typedef ARCH_ATOMIC(s32)    at_s32;
typedef ARCH_ATOMIC(s64)    at_s64;

typedef ARCH_ATOMIC(u8)     at_u8;
typedef ARCH_ATOMIC(u16)    at_u16;
typedef ARCH_ATOMIC(u32)    at_u32;
typedef ARCH_ATOMIC(u64)    at_u64;

typedef ARCH_ATOMIC(sptr)   at_sptr;
typedef ARCH_ATOMIC(uptr)   at_uptr;
typedef ARCH_ATOMIC(ssize)  at_ssize;
typedef ARCH_ATOMIC(usize)  at_usize;

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
