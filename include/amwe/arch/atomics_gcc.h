#ifndef _LAKE_ATOMICS_H
#error The 'GCC' atomics header must not be included outside of 'bedrock.h'.
#endif

#define LAKE_ATOMIC(T) _Atomic T

enum lake_memory_model {
    lake_memory_model_relaxed = __ATOMIC_RELAXED,
    lake_memory_model_consume = __ATOMIC_CONSUME,
    lake_memory_model_acquire = __ATOMIC_ACQUIRE,
    lake_memory_model_release = __ATOMIC_RELEASE,
    lake_memory_model_acq_rel = __ATOMIC_ACQ_REL,
    lake_memory_model_seq_cst = __ATOMIC_SEQ_CST,
};

#define LAKE_ATOMIC_BOOL_LOCK_FREE		__GCC_ATOMIC_BOOL_LOCK_FREE
#define LAKE_ATOMIC_CHAR_LOCK_FREE		__GCC_ATOMIC_CHAR_LOCK_FREE
#define LAKE_ATOMIC_SHORT_LOCK_FREE		__GCC_ATOMIC_SHORT_LOCK_FREE
#define LAKE_ATOMIC_INT_LOCK_FREE		__GCC_ATOMIC_INT_LOCK_FREE
#define LAKE_ATOMIC_LONG_LOCK_FREE		__GCC_ATOMIC_LONG_LOCK_FREE
#define LAKE_ATOMIC_LLONG_LOCK_FREE		__GCC_ATOMIC_LLONG_LOCK_FREE
#define LAKE_ATOMIC_POINTER_LOCK_FREE	__GCC_ATOMIC_POINTER_LOCK_FREE

#define lake_atomic_init(object, value) \
    (*(object) = (value), (void)0)

#define lake_atomic_thread_fence    __atomic_thread_fence
#define lake_atomic_signal_fence    __atomic_signal_fence
#define lake_atomic_is_lock_free    __atomic_is_lock_free

#define lake_atomic_store_explicit  __atomic_store_n
#define lake_atomic_read_explicit   __atomic_load_n

#define lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model1, model2) \
    lake_atomic_compare_exchange_explicit(dst, exp, src, false, model1, model2)
#define lake_atomic_compare_exchange_weak_explicit(dst, exp, src, model1, model2) \
    lake_atomic_compare_exchange_explicit(dst, exp, src, true, model1, model2)
#define lake_atomic_compare_exchange_explicit       __atomic_compare_exchange_n
#define lake_atomic_exchange_explicit               __atomic_exchange_n

#define lake_atomic_add_explicit    __atomic_fetch_add
#define lake_atomic_sub_explicit    __atomic_fetch_sub
#define lake_atomic_and_explicit    __atomic_fetch_and
#define lake_atomic_xor_explicit    __atomic_fetch_xor
#define lake_atomic_or_explicit     __atomic_fetch_or
#define lake_atomic_nand_explicit   __atomic_fetch_nand

typedef struct {
    LAKE_ATOMIC(bool) flag;
} atomic_spinlock;

#define LAKE_ATOMIC_SPINLOCK_INIT { 0 }

lake_force_inline bool 
lake_atomic_spinlock_try_acquire_explicit(
    volatile atomic_spinlock *spinlock,
    enum lake_memory_model    model)
{
    return __atomic_test_and_set(spinlock, model);
}

lake_force_inline void 
lake_atomic_spinlock_acquire_explicit(
    volatile atomic_spinlock *spinlock,
    enum lake_memory_model    model)
{
    do { /* spin */ } while(lake_atomic_spinlock_try_acquire_explicit(spinlock, model));
}

lake_force_inline void 
lake_atomic_spinlock_release_explicit(
    volatile atomic_spinlock *spinlock,
    enum lake_memory_model    model)
{
    __atomic_clear(spinlock, model);
}
