#ifndef _LAKE_ATOMICS_H
#error The 'C++' atomics header must not be included outside of 'bedrock.h'.
#endif

#include <atomic>

#define LAKE_ATOMIC(T) std::atomic<T>

enum lake_memory_model {
    lake_memory_model_relaxed = std::memory_order_relaxed,
    lake_memory_model_consume = std::memory_order_consume,
    lake_memory_model_acquire = std::memory_order_acquire,
    lake_memory_model_release = std::memory_order_release,
    lake_memory_model_acq_rel = std::memory_order_acq_rel,
    lake_memory_model_seq_cst = std::memory_order_seq_cst,
};

#define LAKE_ATOMIC_BOOL_LOCK_FREE      2
#define LAKE_ATOMIC_CHAR_LOCK_FREE      2
#define LAKE_ATOMIC_SHORT_LOCK_FREE     2
#define LAKE_ATOMIC_INT_LOCK_FREE       2
#define LAKE_ATOMIC_LONG_LOCK_FREE      2
#define LAKE_ATOMIC_LLONG_LOCK_FREE     2
#define LAKE_ATOMIC_POINTER_LOCK_FREE   2

#define lake_atomic_init            std::atomic_init
#define lake_atomic_thread_fence    std::atomic_thread_fence
#define lake_atomic_signal_fence    std::atomic_signal_fence
#define lake_atomic_is_lock_free    std::atomic_is_lock_free

#define lake_atomic_store_explicit  std::atomic_store_explicit
#define lake_atomic_read_explicit   std::atomic_load_explicit

#define lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model1, model2) \
    std::atomic_compare_exchange_strong_explicit(dst, exp, src, model1, model2)
#define lake_atomic_compare_exchange_weak_explicit(dst, exp, src, model1, model2) \
    std::atomic_compare_exchange_weak_explicit(dst, exp, src, model1, model2)
#define lake_atomic_compare_exchange_explicit(dst, exp, src, weak, model1, model2) \
    (weak ? lake_atomic_compare_exchange_weak_explicit(dst, exp, src, model1, model2) \
         : lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model1, model2))

#define lake_atomic_exchange_explicit(rmw, arg, model) \
    std::atomic_exchange_explicit(rmw, arg, model)

#define lake_atomic_add_explicit(rmw, arg, model) \
    (*(rmw)).fetch_add(arg, model)
#define lake_atomic_sub_explicit(rmw, arg, model) \
    (*(rmw)).fetch_sub(arg, model)
#define lake_atomic_and_explicit(rmw, arg, model) \
    (*(rmw)).fetch_and(arg, model)
#define lake_atomic_xor_explicit(rmw, arg, model) \
    (*(rmw)).fetch_xor(arg, model)
#define lake_atomic_or_explicit(rmw, arg, model) \
    (*(rmw)).fetch_or(arg, model)

template <typename T>
T lake_atomic_fetch_nand(std::atomic<T>& atomic_obj, T value, std::memory_order order = std::memory_order_seq_cst) {
    T current_value = atomic_obj.load(order);
    T new_value = ~(current_value & value);
    while (!atomic_obj.compare_exchange_weak(current_value, new_value, order)) {
        new_value = ~(current_value & value);
    }
    return current_value;
}

typedef struct {
    std::atomic_flag flag;
} atomic_spinlock;

#define LAKE_ATOMIC_SPINLOCK_INIT { ATOMIC_FLAG_INIT }

#define lake_atomic_spinlock_try_acquire_explicit(spinlock, model) \
    std::atomic_flag_test_and_set_explicit(spinlock->flag, model)

#define lake_atomic_spinlock_acquire_explicit(spinlock, model) \
    do { /* spin */ } while(lake_atomic_spinlock_try_acquire_explicit(spinlock, model))

#define lake_atomic_spinlock_release_explicit(spinlock, model) \
    std::atomic_flag_clear(spinlock->flag, model)
