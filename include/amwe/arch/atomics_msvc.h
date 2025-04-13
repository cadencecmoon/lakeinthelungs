#ifndef _LAKE_ATOMICS_H
#error The 'MSVC' atomics header must not be included outside of 'bedrock.h'.
#endif

/** Though there is no clear documentation that states that anything greater than VS 2005, where 
 *  msdn documentation states that volatile to volatile references are ordered; has the same behaviour 
 *  as described. Looking through MSVCs C++ atomics library in newe versions of Visual Studio shows 
 *  that the compiler still takes acquire/release semantics on volatile variables. */
#define LAKE_ATOMIC(type) type volatile

enum lake_memory_model {
    lake_memory_model_relaxed, 
    lake_memory_model_consume, 
    lake_memory_model_acquire, 
    lake_memory_model_release, 
    lake_memory_model_acq_rel, 
    lake_memory_model_seq_cst, 
};

#if _MSC_VER > 1800 && defined(_M_IX86)
#define _InterlockedExchangeAdd64 _InlineInterlockedExchangeAdd64
#define _InterlockedExchange64 _InlineInterlockedExchange64
#endif

#define LAKE_ATOMIC_BOOL_LOCK_FREE 2
#define LAKE_ATOMIC_CHAR_LOCK_FREE 2
#define LAKE_ATOMIC_SHORT_LOCK_FREE 2
#define LAKE_ATOMIC_INT_LOCK_FREE 2
#define LAKE_ATOMIC_LONG_LOCK_FREE 2
#define LAKE_ATOMIC_LLONG_LOCK_FREE 2
#define LAKE_ATOMIC_POINTER_LOCK_FREE 2

#define lake_atomic_is_lock_free(obj) \
    ((void)*(obj), (lake_sizeof((*(obj)) ? 2 : 0) <= 8 && lake_is_pow2(lake_sizeof((*(obj)) ? 2 : 0))

#define lake_atomic_init(obj, value) \
    (*(obj) = (value), (void)0)

lake_force_inline void 
_atomic_compiler_barrier(enum lake_memory_model model)
{
    /** In case of 'lake_memory_model_consume', it is implicitly assumed that 
     *  the compiler will not move instructions that have data-dependency 
     *  on the variable in question before the barrier. */
    if (model > lake_memory_model_consume) 
        _ReadWriteBarrier();
}

lake_force_inline void
lake_atomic_thread_fence(enum lake_memory_model model)
{
    /* x86 is strongly ordered and acquire/release semantics come automatically */
    _atomic_compiler_barrier(model);
    if (model == lake_memory_model_seq_cst) {
        MemoryBarrier();
        _atomic_compiler_barrier(model);
    }
}

lake_force_inline void 
lake_atomic_signal_fence(enum lake_memory_model model)
{
    _atomic_compiler_barrier(model);
}

/** 1, 2 and 4 bytes loads and stores are atomic on aligned memory. In addition, since the 
 *  compiler automatically takes acquire and release semantics on volatile variables, for any 
 *  order lesser than 'lake_memory_model_seq_cst' we can directly assign or read values. */
#define _atomic_store32(dst, src, model)            \
    if (model == lake_memory_model_seq_cst) {       \
        InterlockedExchange((long volatile *)(dst), \
                                (long)(src);        \
    } else {                                        \
        *(dst) = (src);                             \
    }

/** MSVC converts 64-bit writes into two instructions. So there is a possibility that an 
 *  interrupt can make a 64-bit write non-atomic even when 8-byte aligned. 
 *  So use _InterlockedExchange64(). For atomic stores, 'consume' and 'acquire' semantics 
 *  are not valid. But we are using 'Exchange' to get atomic stores here and we only have 
 *  InterlockedExchange64(), InterlockedExchangeNoFence64() and InterlockedExchange64Acquire()
 *  available. We are forced to use InterlockedExchange64() which uses full memory barrier 
 *  for everything greater than 'lake_memory_model_relaxed'. */
#ifdef _M_IX86
    #define _atomic_store64(dst, src, model)                                    \
        if (model == lake_memory_model_relaxed) {                               \
            InterlockedExchangeNoFence64((int64_t volatile *)(dst),             \
                                         (int64_t)(src));                       \
        } else {                                                                \
            InterlockedExchange64((int64_t volatile *)(dst), (int64_t)(src));   \
        }
#elif _M_64
    /** 64-bit writes are atomic on amd64 if 64-bit aligned */
    #define _atomic_store64(dst, src, model)                                    \
        if (model == lake_memory_model_seq_cst) {                               \
            InterlockedExchange64((int64_t volatile *)(dst),                    \
                                  (int64_t)(src));                              \
        } else {                                                                \
            *(dst) = (src);                                                     \
        }
#else
    #error Implement MSVC atomics for Windows ARM
#endif

#define _atomic_store8(dst, src, model)                                         \
    if (model == lake_memory_model_seq_cst) {                                   \
        InterlockedExchange8((char volatile *)(dst), (char)(src));              \
    } else {                                                                    \
        *(dst) = (src);                                                         \
    }

#define _atomic_store16(dst, src, model)                                        \
    if (model == lake_memory_model_seq_cst) {                                   \
        InterlockedExchange16((short volatile *)(dst), (short)(src));           \
    } else {                                                                    \
        *(dst) = (src);                                                         \
    }

#define lake_atomic_store_explicit(dst, src, model) \
    if (lake_sizeof(*(dst)) == 1) {         \
        _atomic_store8(dst, src, model)     \
    } else if (lake_sizeof(*(dst)) == 2) {  \
        _atomic_store16(dst, src, model)    \
    } else if (lake_sizeof(*(dst)) == 4) {  \
        _atomic_store32(dst, src, model)    \
    } else if (lake_sizeof(*(dst)) == 8) {  \
        _atomic_store64(dst, src, model)    \
    } else {                                \
        lake_unreachable;                   \
    }

/* On x86, for 'lake_memory_model_seq_cst', if stores are locked, the corresponding
 * reads don't need to be locked (based on the following in Intel Developers manual:
 *
 * "Locked operations are atomic with respect to all other memory operations
 * and all externally visible events. Only instruction fetch and page table
 * accesses can pass locked instructions. Locked instructions can be used to
 * synchronize data written by one processor and read by another processor.
 * For the P6 family processors, locked operations serialize all outstanding
 * load and store operations (that is, wait for them to complete). This rule
 * is also true for the Pentium 4 and Intel Xeon processors, with one
 * exception. Load operations that reference weakly ordered memory types
 * (such as the WC memory type) may not be serialized." */

/** For 8, 16 and 32-bit variations. */
#define _atomic_readX(src, dst, model) \
    *(dst) = *(src);

#ifdef _M_IX86
    #define _atomic_read64(src, dst, model)                     \
        __pragma (warning(push))                                \
        __pragma (warning(disable:4047))                        \
        *(dst) = InterlockedOr64((int64_t volatile *)(src), 0); \
        _pragma (warning(pop))
#elif _M_64
    /** 64-bit writes are atomic on amd64 if 64-bit aligned */
    #define _atomic_read64(src, dst, model) \
        *(dst) = *(src);
#else
    #error Implement MSVC atomics for Windows ARM
#endif

#define lake_atomic_read_explicit(src, model)                                   \
    ({                                                                          \
        lake_typeof(*src) _temp;                                                \
        if (lake_sizeof(*(src)) == 1 ||                                         \
            lake_sizeof(*(src)) == 2 ||                                         \
            lake_sizeof(*(src)) == 4)                                           \
        {                                                                       \
            _atomic_readX(src, _temp, model)                                    \
        } else if (lake_sizeof(*(src) == 8)) {                                  \
            _atomic_read64(src, _temp, model)                                   \
        } else {                                                                \
            lake_unreachable;                                                   \
        }                                                                       \
        _temp;                                                                  \
    })

/** For 8, 16 and 64-bit variations. */
#define _atomic_op(op, x, rmw, arg, model) \
    _atomic_##OP##_generic(x, rmw, arg, model)
#define _atomic_add8(rmw, arg, model) \
    _InterlockedExchangeAdd8((char volatile *)(rmw), (char)(arg));
#define _atomic_add16(rmw, arg, model) \
    _InterlockedExchangeAdd16((short volatile *)(rmw), (short)(arg));
#define _atomic_add32(rmw, arg, model) \
    _InterlockedExchangeAdd((long volatile *)(rmw), (long)(arg));
#define _atomic_add64(rmw, arg, model) \
    _InterlockedExchangeAdd64((int64_t volatile *)(rmw), (int64_t)(arg));

#define lake_atomic_add_explicit(rmw, arg, model)                               \
    ({                                                                          \
        lake_typeof(*src) _temp;                                                \
        if (lake_sizeof(*(src) == 1)) {                                         \
            _temp = _atomic_add8(src, arg, model)                               \
        } else if (lake_sizeof(*(src) == 2)) {                                  \
            _temp = _atomic_add16(src, arg, model)                              \
        } else if (lake_sizeof(*(src) == 4)) {                                  \
            _temp = _atomic_add32(src, arg, model)                              \
        } else if (lake_sizeof(*(src) == 8)) {                                  \
            _temp = _atomic_add64(src, arg, model)                              \
        } else {                                                                \
            lake_unreachable;                                                   \
        }                                                                       \
        _temp;                                                                  \
    })

#define lake_atomic_sub_explicit(rmw, arg, model) \
    lake_atomic_add_explicit(rmw, (0 - (arg)), model)

/* For 32-bit variations. */
#define _atomic_and32(rmw, arg, model) \
    InterlockedAnd((int32_t volatile *)(rmw), (int32_t)(arg));
#define _atomic_or32(rmw, arg, model) \
    InterlockedOr((int32_t volatile *)(rmw), (int32_t)(arg));
#define _atomic_xor32(rmw, arg, model) \
    InterlockedXor((int32_t volatile *)(rmw), (int32_t)(arg));

/* For 8, 16 and 64-bit variations. */
#define _atomic_and_generic(x, rmw, arg, model) \
    InterlockedAnd##x((int##x##_t volatile *)(rmw), (int##x##_t)(arg));
#define _atomic_or_generic(x, rmw, arg, model) \
    InterlockedOr##x((int##x##_t volatile *)(rmw), (int##x##_t)(arg));
#define _atomic_xor_generic(x, rmw, arg, model) \
    InterlockedXor##x((int##x##_t volatile *)(rmw), (int##x##_t)(arg));

#define lake_atomic_and_explicit(rmw, arg, model)                               \
    ({                                                                          \
        lake_typeof(*src) _temp;                                                \
        if (lake_sizeof(*(src) == 1)) {                                         \
            _temp = _atomic_op(and, 8, src, arg, model)                         \
        } else if (lake_sizeof(*(src) == 2)) {                                  \
            _temp = _atomic_op(and, 16, src, arg, model)                        \
        } else if (lake_sizeof(*(src) == 4)) {                                  \
            _temp = _atomic_and32(src, arg, model)                              \
        } else if (lake_sizeof(*(src) == 8)) {                                  \
            _temp = _atomic_op(and, 64, src, arg, model)                        \
        } else {                                                                \
            lake_unreachable;                                                   \
        }                                                                       \
        _temp;                                                                  \
    })

#define lake_atomic_or_explicit(rmw, arg, model)                                \
    ({                                                                          \
        lake_typeof(*src) _temp;                                                \
        if (lake_sizeof(*(src) == 1)) {                                         \
            _temp = _atomic_op(or, 8, src, arg, model)                          \
        } else if (lake_sizeof(*(src) == 2)) {                                  \
            _temp = _atomic_op(or, 16, src, arg, model)                         \
        } else if (lake_sizeof(*(src) == 4)) {                                  \
            _temp = _atomic_or32(src, arg, model)                               \
        } else if (lake_sizeof(*(src) == 8)) {                                  \
            _temp = _atomic_op(or, 64, src, arg, model)                         \
        } else {                                                                \
            lake_unreachable;                                                   \
        }                                                                       \
        _temp;                                                                  \
    })

#define lake_atomic_xor_explicit(rmw, arg, model)                               \
    ({                                                                          \
        lake_typeof(*src) _temp;                                                \
        if (lake_sizeof(*(src) == 1)) {                                         \
            _temp = _atomic_op(xor, 8, src, arg, model)                         \
        } else if (lake_sizeof(*(src) == 2)) {                                  \
            _temp = _atomic_op(xor, 16, src, arg, model)                        \
        } else if (lake_sizeof(*(src) == 4)) {                                  \
            _temp = _atomic_xor32(src, arg, model)                              \
        } else if (lake_sizeof(*(src) == 8)) {                                  \
            _temp = _atomic_op(xor, 64, src, arg, model)                        \
        } else {                                                                \
            lake_unreachable;                                                   \
        }                                                                       \
        _temp;                                                                  \
    })

/* For 32-bit variations. */
#define _atomic_nand32(rmw, arg, model)                                                 \
    ({                                                                                  \
        int32_t old_val, new_val;                                                       \
        do {                                                                            \
            old_val = *(int32_t volatile *)(rmw);                                       \
            new_val = ~(old_val & (int32_t)(arg));                                      \
        } while (_InterlockedCompareExchange((int32_t volatile *)(rmw),                 \
                                             (int32_t)new_val,                          \
                                             (int32_t)old_val) != (int32_t)old_val);    \
        new_val;                                                                        \
    })

/* For 8, 16, and 64-bit variations. */
#define _atomic_nand_generic(x, rmw, arg, model)                                        \
    ({                                                                                  \
        int##x##_t old_val, new_val;                                                    \
        do {                                                                            \
            old_val = *(int##x##_t volatile *)(rmw);                                    \
            new_val = ~(old_val & (int##x##_t)(arg));                                   \
        } while (_InterlockedCompareExchange##x((int##x##_t volatile *)(rmw),           \
                                                (int##x##_t)new_val,                    \
                                                (int##x##_t)old_val) != old_val);       \
        new_val;                                                                        \
    })

#define lake_atomic_nand_explicit(rmw, arg, model)                  \
    ({                                                              \
        lake_typeof(*src) _temp;                                    \
        if (lake_sizeof(*(src) == 1)) {                             \
            _temp = _atomic_op(nand, 8, src, arg, model)            \
        } else if (lake_sizeof(*(src) == 2)) {                      \
            _temp = _atomic_op(nand, 16, src, arg, model)           \
        } else if (lake_sizeof(*(src) == 4)) {                      \
            _temp = _atomic_nand32(src, arg, model)                 \
        } else if (lake_sizeof(*(src) == 8)) {                      \
            _temp = _atomic_op(nand, 64, src, arg, model)           \
        } else {                                                    \
            lake_unreachable;                                       \
        }                                                           \
        _temp;                                                      \
    })

#define lake_atomic_exchange_explicit(dst, src, model)              \
    ((lake_sizeof(*(dst)) == 1) ?                                   \
        _InterlockedExchange8((char volatile *)dst, src)            \
    : (lake_sizeof(*(dst)) == 2) ?                                  \
        _InterlockedExchange16((short volatile *)dst, src)          \
    : (lake_sizeof(*(dst)) == 4) ?                                  \
        _InterlockedExchange((long int volatile *)dst, src)         \
    : (lake_sizeof(*(dst)) == 8) ?                                  \
        _InterlockedExchange64((__int64 volatile *)dst, src)        \
    : (lake_unreachable, 0)

lake_force_inline _Bool 
_atomic_compare_exchange8(int8_t volatile *dst, int8_t *expected, int8_t src)
{
    int8_t previous = _InterlockedCompareExchange8((char volatile *)dst, src, *expected);
    if (previous == *expected) {
        return true;
    } else {
        *expected = previous;
        return false;
    }
}

lake_force_inline _Bool 
_atomic_compare_exchange16(int16_t volatile *dst, int16_t *expected, int16_t src)
{
    int16_t previous = _InterlockedCompareExchange16((short volatile *)dst, src, *expected);
    if (previous == *expected) {
        return true;
    } else {
        *expected = previous;
        return false;
    }
}

lake_force_inline _Bool 
_atomic_compare_exchange32(int32_t volatile *dst, int32_t *expected, int32_t src)
{
    int32_t previous = _InterlockedCompareExchange16((long volatile *)dst, src, *expected);
    if (previous == *expected) {
        return true;
    } else {
        *expected = previous;
        return false;
    }
}

lake_force_inline _Bool 
_atomic_compare_exchange64(int64_t volatile *dst, int64_t *expected, int64_t src)
{
    int64_t previous = _InterlockedCompareExchange64(dst, src, *expected);
    if (previous == *expected) {
        return true;
    } else {
        *expected = previous;
        return false;
    }
}

#define lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model1, model2) \
    lake_atomic_compare_exchange_explicit(dst, exp, src, false, model1, model2)

#define lake_atomic_compare_exchange_weak_explicit(dst, exp, src, model1, model2) \
    lake_atomic_compare_exchange_explicit(dst, exp, src, true, model1, model2)

#define lake_atomic_compare_exchange_explicit(dst, exp, src, weak, model1, model2)              \
    (lake_sizeof(*(dst)) == 1                                                                   \
     ? _atomic_compare_exchange8((int8_t volatile *)(dst), (int8_t *)(exp), (int8_t)(src))      \
     : lake_sizeof(*(dst)) == 2                                                                 \
     ? _atomic_compare_exchange16((int16_t volatile *)(dst), (int16_t *)(exp), (int16_t)(src))  \
     : lake_sizeof(*(dst)) == 4                                                                 \
     ? _atomic_compare_exchange32((int32_t volatile *)(dst), (int32_t *)(exp), (int32_t)(src))  \
     : lake_sizeof(*(dst)) == 8                                                                 \
     ? _atomic_compare_exchange64((int64_t volatile *)(dst), (int64_t *)(exp), (int64_t)(src))  \
     : lake_unreachable)

typedef LAKE_ATOMIC(int32_t) atomic_spinlock;
#define LAKE_ATOMIC_SPINLOCK_INIT 0

#define lake_atomic_spinlock_try_acquire_explicit(spinlock, model) \
    (_Bool)InterlockedBitTestAndSet(spinlock, 0)

#define lake_atomic_spinlock_acquire_explicit(spinlock, model) \
    do { /* spin */ } while(lake_atomic_spinlock_try_acquire_explicit(spinlock, model))

#define lake_atomic_spinlock_release(spinlock, model) \
    InterlockedBitTestAndReset(spinlock, 0)
