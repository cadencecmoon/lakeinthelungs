#ifndef _LAKE_ATOMICS_H
#error The 'X86' atomics header must not be included outside of 'bedrock.h'.
#endif

#define _x86_compiler_barrier() __asm__ volatile(" " : : : "memory")
#ifdef LAKE_ARCH_AMD64
    #define _x86_cpu_barrier()  __asm__ volatile("mfence;" : : : "memory")
#else
    #define _x86_cpu_barrier()  __asm__ volatile("lock ; addl $0, (%%esp)" : : : "memory", "cc")
#endif

/** The 'volatile' keyword prevents the compiler rom keeping the atomic
 *  value in a register, and generates a new memory access for each atomic 
 *  operation. This allows the implementations of memory_order_relaxed and 
 *  memory_order_consume to avoid issuing a compiler memory barrier, allowing 
 *  full optimization of all surrounding non-atomic variables.
 *
 *  For 32-bit x86, attribute aligned is used to tell the compiler to align 
 *  64-bit data on a 8-byte boundary. This allows more efficient atomic access,
 *  as the CPU guarantees such memory accessess to be atomic. */
#if defined(LAKE_ARCH_AMD64)
    #define LAKE_ATOMIC(type) type volatile
#else
    #define LAKE_ATOMIC(type) type volatile lake_alignment(lake_sizeof(type))
#endif

/** Values of this enum are correct with the C++11, C11 or GNU atomics memory models. */
enum lake_memory_model {
    lake_memory_model_relaxed,
    lake_memory_model_consume,
    lake_memory_model_acquire,
    lake_memory_model_release,
    lake_memory_model_acq_rel,
    lake_memory_model_seq_cst,
};

#define LAKE_ATOMIC_BOOL_LOCK_FREE 2
#define LAKE_ATOMIC_CHAR_LOCK_FREE 2
#define LAKE_ATOMIC_SHORT_LOCK_FREE 2
#define LAKE_ATOMIC_INT_LOCK_FREE 2
#define LAKE_ATOMIC_LONG_LOCK_FREE 2
#define LAKE_ATOMIC_LLONG_LOCK_FREE 2
#define LAKE_ATOMIC_POINTER_LOCK_FREE 2

#define lake_atomic_is_lockless(obj) \
    ((void)*(obj), (lake_sizeof((*(obj)) ? 2 : 0) <= 8 && lake_is_pow2(lake_sizeof((*(obj)) ? 2 : 0))

#define lake_atomic_init(obj, value) \
    (*(obj) = (value), (void)0)

/** The 'lake_memory_model_relaxed' does not need a compiler barrier, if the 
 *  atomic operation can otherwise be guaranteed to not be moved with 
 *  respect to other atomic operations on the same memory location. Using 
 *  the 'volatile' keyword in the definition of the atomic types accomplishes 
 *  this, as memory access to volatile data may not be optimized away, or be 
 *  reordered with other volatile accesses. 
 *
 *  On x86 also 'lake_memory_model_consume' is automatic, and data dependency 
 *  on a volatile atomic variable means that the compiler optimizations should 
 *  not cause problems. That is, the compiler should not speculate the value of 
 *  the amw_atomic_read, as it is going to read it from the memory anyway. 
 *  This allows omiting the compiler memory barrier on atomic reads with 
 *  'lake_memory_model_consume'. This matches the definition of 
 *  smp_read_barrier_depends() in Linux kernel as a nop for x86, 
 *  and its usage in rcu_dereference(). */
lake_force_inline void 
_atomic_compiler_barrier(enum lake_memory_model model) 
{
    if (model > lake_memory_model_consume) 
        _x86_compiler_barrier();
}

lake_force_inline void
lake_atomic_thread_fence(enum lake_memory_model model)
{
    if (model == lake_memory_model_seq_cst)
        _x86_cpu_barrier();
    else 
        _atomic_compiler_barrier(model);
}

lake_force_inline void
lake_atomic_signal_fence(enum lake_memory_model model) 
{
    _atomic_compiler_barrier(model);
}

#if defined(LAKE_ARCH_AMD64) 
    #define _atomic_exchange(dst, src, model)               \
        ({                                                  \
            lake_typeof(dst) dst___ = (dst);                \
            lake_typeof(*(dst)) src___ = (src);             \
                                                            \
            if ((model) > lake_memory_model_consume) {      \
                __asm__ volatile("xchg %1, %0 ; "           \
                                 "# _atomic_exchange"       \
                               : "+r" (src___), /* 0 */     \
                                 "+m" (*dst___) /* 1 */     \
                               : : "memory");               \
            } else {                                        \
                __asm__ volatile("xchg %1, %0 ; "           \
                                 "# _atomic_exchange"       \
                               : "+r" (src___),   /* 0 */   \
                                 "+m" (*dst___)); /* 1 */   \
            }                                               \
            src___;                                         \
        })
#else /* 32-bit */
    /** The 8-byte atomic exchange uses cmpxchg8b with the SRC (ax:dx) as
     *  the expected value (bx:cx), which will get replaced by the current 
     *  value in the likely case it did not match, after which we keep 
     *  trying until the swap succeeds. */
    #if defined(__PIC__)
        /** ebx may not be clobbered when compiled with -fPIC, must save 
         *  and restore it. Furthermore, 'dst' may be addressed via ebx,
         *  so the address must be passed via a register so that it remains 
         *  valid also after changing ebx. */
        #define _atomic_exchange_8(dst, src, clob)          \
            u32 temp___;                                    \
            __asm__ volatile("      movl %%ebx, %2 ;    "   \
                             "      movl %%eax, %%ebx ; "   \
                             "      movl %%edx, %%ecx ; "   \
                             "1:                        "   \
                             "lock; cmpchg8b (%0) ;     "   \
                             "      jne 1b ;            "   \
                             "      movl %2, %%ebx ;    "   \
                             "# _atomic_exchange_8      "   \
                           : "+r" (dst),     /* 0 */        \
                             "+A" (src),     /* 1 */        \
                             "=mr" (temp___) /* 2 */        \
                           : : "ecx", clob, "cc")
    #else
        #define _atomic_exchange_8(dst, src, clob)          \
            __asm__ volatile("      movl %%eax, %%ebx ; "   \
                             "      movl %%edx, %%ecx ; "   \
                             "1:                        "   \
                             "lock; cmpchg8b %0 ;       "   \
                             "      jne 1b ;            "   \
                             "# _atomic_exchange_8      "   \
                           : "+m" (*dst),    /* 0 */        \
                             "+A" (src),     /* 1 */        \
                           : : "ebx", "ecx", clob, "cc")
    #endif

    #define _atomic_exchange(dst, src, model)                   \
        ({                                                      \
            lake_typeof(dst) dst___ = (dst);                    \
            lake_typeof(*(dst)) src___ = (src);                 \
                                                                \
            if ((model) > lake_memory_model_consume) {          \
                if (lake_sizeof(*(dst)) == 8) {                 \
                                                                \
                } else {                                        \
                    __asm__ volatile("xchg %1, %0 ;     "       \
                                     "# _atomic_exchange"       \
                                   : "+r" (src___), /* 0 */     \
                                     "+m" (*dst___) /* 1 */     \
                                   : : "memory");               \
                }                                               \
            } else {                                            \
                if (lake_sizeof(*(dst)) == 8) {                 \
                    _atomic_exchange_8(dst___, src___, "cc");   \
                } else {                                        \
                    __asm__ volatile("xchg %1, %0 ;     "       \
                                     "# _atomic_exchange"       \
                                   : "+r" (src___),   /* 0 */   \
                                     "+m" (*dst___)); /* 1 */   \
                }                                               \
            }                                                   \
            src___;                                             \
        })
#endif

/** Atomic store. Valid memory models are:
 *
 *  lake_memory_model_relaxed,
 *  lake_memory_model_release, and
 *  lake_memory_model_seq_cst. */
#if defined(LAKE_ARCH_AMD64)
    #define lake_atomic_store_explicit(dst, src, model) \
        ({                                              \
            lake_typeof(dst) dst__ = (dst);             \
            lake_typeof(*(dst)) src__ = (src);          \
                                                        \
            if ((model) != lake_memory_model_seq_cst) { \
                _atomic_compiler_barrier(model);        \
                *dst__ = src__;                         \
            } else {                                    \
                _atomic_exchange(dst__, src__, model);  \
            }                                           \
            (void)0;                                    \
        })
#else /* 32-bit */
    #if defined(LAKE_ARCH_X86_SSE)
        /** SSE registers are 128-bit wide, and moving the lowest 64-bits of an SSE 
         *  register to properly aligned memory is atomic. */
        #define _atomic_store_8(dst, src)               \
            __asm__ volatile("movq %1, %0 ;"            \
                             "# _atomic_store_8"        \
                           : "=m" (*dst) /* 0 */        \
                           : "x" (src))  /* 1, SSE */
    #else
        /** Locked 64-bit exchange is available on i586 CPUs, not sure of older ones. */
        #define _atomic_store_8(dst, src) \
            _atomic_exchange_8(dst, src, "cc")
    #endif

    #define lake_atomic_store_explicit(dst, src, model) \
        ({                                              \
            lake_typeof(dst) dst__ = (dst);             \
            lake_typeof(*(dst)) src__ = (src);          \
                                                        \
            if ((model) != lake_memory_model_seq_cst) { \
                _atomic_compiler_barrier(model);        \
                if (lake_sizeof(*(dst)) == 8) {         \
                    _atomic_store_8(dst__, src__);      \
                } else {                                \
                    *dst__ = src__;                     \
                }                                       \
                *dst__ = src__;                         \
            } else {                                    \
                _atomic_exchange(dst__, src__, model); \
            }                                           \
            (void)0;                                    \
        })
#endif

/** Atomic read. Valid memory models are:
 *
 *  lake_memory_model_relaxed,
 *  lake_memory_model_consume,
 *  lake_memory_model_acquire, and
 *  lake_memory_model_seq_cst. */
#if defined(LAKE_ARCH_AMD64)
    #define lake_atomic_read_explicit(src, model)   \
        ({                                          \
            lake_typeof(src) src__ = (src);         \
            _atomic_compiler_barrier(model);        \
            *src__;                                 \
        })
#else /* 32-bit */
    #if defined(LAKE_ARCH_X86_SSE)
        /** SSE registers are 128-bit wide, and moving the lowest 64-bits of an SSE 
         *  register to properly aligned memory is atomic. */
        #define _atomic_read_8(src, dst)            \
            ({                                      \
                lake_typeof(*(dst)) res___;         \
                __asm__ ("movq %1, %0 ;    "        \
                         "# _atomic_read_8"         \
                       : "=x" (res___) /* 0, SSE */ \
                       : "m" (*src)); /* 1 */       \
                *(dst) = res__;                     \
            })
    #else
        /** Must use locked cmpxchg8b if compiled w/o SSE support. Compare *dst to a 
         *  random value in bx:cx and return the actual value in ax:dx. The registers 
         *  bx and cx are only read, so they are not clobbered. */
        #define _atomic_read_8(src, dst)                \
            ({                                          \
                lake_typeof(*(dst)) res___;             \
                __asm__ ("      movl %%ebx, %%eax ; "   \
                         "      movl %%ecx, %%edx ; "   \
                         "lock; cmpxchg8b %1 ;      "   \
                         "# _atomic_read_8          "   \
                       : "=&A" (res___), /* 0 */        \
                         "+m" (*src) /* 1 */            \
                       : : "cc");                       \
                *(dst) = res___;                        \
            })
    #endif

    #define lake_atomic_read_explicit(src, model)   \
        ({                                          \
            lake_typeof(src) src__ = (src);         \
            lake_typeof(*(src)) res__;              \
                                                    \
            if (lake_sizeof(*(dst)) <= 4) {         \
                res__ = *src__;                     \
            } else {                                \
                _atomic_read_8(src, res__);         \
            }                                       \
            _atomic_compiler_barrier(model);        \
            res__;                                  \
        })
#endif

/** All memory models are valid for read-modify-write operations.
 *  Valid memory models for the read operation of the current value in 
 *  the failure case are the same as for atomic read, but can not be 
 *  stronger than the success memory model. 'model_fail' is ignores,
 *  as _atomic_compare_exchange already implements at least as 
 *  strong a barrier as allowed for 'model_fail' in all cases. 
 *  It's left for compatibility with other platforms. */
#if defined(LAKE_ARCH_AMD64)
    #define _atomic_compare_exchange(dst, exp, src, res, clob)  \
        __asm__ volatile("lock; cmpxchg %3, %1 ;     "          \
                         "      sete    %0           "          \
                         "# _atomic_compare_exchange "          \
                       : "=q" (res),  /* 0 */                   \
                         "+m" (*dst), /* 1 */                   \
                         "+a" (exp)   /* 2 */                   \
                       : "r" (src)    /* 3 */                   \
                       : clob, "cc")
    
    #define lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model, fail)\
        ({                                                                          \
            lake_typeof(dst) dst__ = (dst);                                         \
            lake_typeof(dst) expp__ = (exp);                                        \
            lake_typeof(*(dst)) src__ = (src);                                      \
            lake_typeof(*(dst)) exp__ = *expp__;                                    \
            u8 res__;                                                               \
            (void)fail;                                                             \
                                                                                    \
            if ((model) > lake_memory_model_consume) {                              \
                _atomic_compare_exchange(dst__, exp__, src__, res__, "memory");     \
            } else {                                                                \
                _atomic_compare_exchange(dst__, exp__, src__, res__, "cc");         \
            }                                                                       \
            if (!res__) {                                                           \
                *expp__ = exp__;                                                    \
            }                                                                       \
            (b32)res__;                                                             \
        })
#else /* 32-bit */
    #if defined(__PIC__)
        /** ebx may not be used as an input when compiled with -fPIC, must save
         *  and restore it. Furthermore, 'DST' may be addressed via ebx, so
         *  the address must be passed via a register so that it remains valid
         *  also after changing ebx. */
        #define _atomic_compare_exchange_8(dst, exp, src, res, clob)    \
            __asm__ volatile("      xchgl %%ebx,%3 ;      "             \
                            "lock; cmpxchg8b (%1) ;       "             \
                            "      xchgl %3,%%ebx ;       "             \
                            "      sete %0                "             \
                            "# _atomic_compare_exchange_8 "             \
                          : "=q" (res),                 /* 0 */         \
                            "+r" (dst),                 /* 1 */         \
                            "+A" (exp)                  /* 2 */         \
                          : "r" ((uint32_t)src),        /* 3 */         \
                            "c" ((uint32_t)                             \
                                ((uint64_t)src >> 32))  /* 4 */         \
                          : clob, "cc")
    #else
        #define _atomic_compare_exchange_8(dst, exp, src, res, clob)    \
            __asm__ volatile("lock; cmpxchg8b %1 ;         "            \
                             "      sete %0                "            \
                             "# _atomic_compare_exchange_8"             \
                           : "=q" (res),                 /* 0 */        \
                             "+m" (*dst),                /* 1 */        \
                             "+A" (exp)                  /* 2 */        \
                           : "b" ((uint32_t)src),        /* 3 */        \
                             "c" ((uint32_t)                            \
                                 ((uint64_t)src >> 32))  /* 4 */        \
                           : clob, "cc")
    #endif
    #define _atomic_compare_exchange(dst, exp, src, res, clob)  \
        __asm__ volatile("lock; cmpxchg %3,%1 ;      "          \
                         "      sete    %0           "          \
                         "# _atomic_compare_exchange"           \
                       : "=q" (res),  /* 0 */                   \
                         "+m" (*dst), /* 1 */                   \
                         "+a" (exp)   /* 2 */                   \
                       : "r" (src)    /* 3 */                   \
                       : clob, "cc")

    #define lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model, fail)    \
        ({                                                                              \
            lake_typeof(dst) dst__ = (dst);                                             \
            lake_typeof(dst) expp__ = (exp);                                            \
            lake_typeof(*(dst)) src__ = (src);                                          \
            lake_typeof(*(dst)) exp__ = *expp__;                                        \
            u8 res__;                                                                   \
            (void)fail;                                                                 \
                                                                                        \
            if ((model) > lake_memory_model_consume) {                                  \
                if (lake_sizeof(*(dst)) <= 4) {                                         \
                    _atomic_compare_exchange(dst__, exp__, src__, res__, "memory");     \
                } else {                                                                \
                    _atomic_compare_exchange_8(dst__, exp__, src__, res__, "memory");   \
                }                                                                       \
            } else {                                                                    \
                if (lake_sizeof(*(dst)) <= 4) {                                         \
                    _atomic_compare_exchange(dst__, exp__, src__, res__, "cc");         \
                } else {                                                                \
                    _atomic_compare_exchange_8(dst__, exp__, src__, res__, "cc");       \
                }                                                                       \
            }                                                                           \
            if (!res__) {                                                               \
                *expp__ = exp__;                                                        \
            }                                                                           \
            (b32)res__;                                                                 \
        })
#endif

#define lake_atomic_compare_exchange_weak_explicit(dst, exp, src, model, fail) \
    lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model, fail)

/* The x86 memory model is strong anyways. */
#define lake_atomic_compare_exchange_explicit(dst, exp, src, weak, model, fail) \
    lake_atomic_compare_exchange_strong_explicit(dst, exp, src, model, fail)

#define lake_atomic_exchange_explicit(rmw, arg, model) \
    _atomic_exchange(rmw, arg, model)

#define _atomic_op(rmw, op, arg, model)                                             \
    ({                                                                              \
        lake_typeof(rmw) rmw__ = (rmw);                                             \
        lake_typeof(arg) arg__ = (arg);                                             \
        lake_typeof(*(rmw)) val__;                                                  \
                                                                                    \
        val__ = lake_atomic_read_explicit(rmw__, lake_memory_model_relaxed);        \
        do {} while (!lake_atomic_compare_exchange_weak_explicit(rmw__, &val__,     \
                        val__ op arg__, model, lake_memory_model_relaxed));         \
        val__;                                                                      \
    })

#define _atomic_add_64(rmw, arg, clob)      \
    __asm__ volatile("lock; xadd %0, %1 ; " \
                     "# _atomic_add_64    " \
                   : "+r" (arg), /* 0 */    \
                     "+m" (*rmw) /* 1 */    \
                   : : clob, "cc")

#define _atomic_add(rmw, arg, model)                    \
    ({                                                  \
        lake_typeof(rmw) rmw__ = (rmw);                 \
        lake_typeof(*(rmw)) arg__ = (arg);              \
                                                        \
        if ((model) > lake_memory_model_consume) {      \
            _atomic_add_64(rmw__, arg__, "memory");     \
        } else {                                        \
            _atomic_add_64(rmw__, arg__, "cc");         \
        }                                               \
        arg__;                                          \
    })

#if defined(LAKE_ARCH_AMD64)
    #define lake_atomic_add_explicit(rmw, arg, model)   \
            _atomic_add(rmw, arg, model)
#else /* 32-bit */
    #define lake_atomic_add_explicit(rmw, arg, model)   \
        ({                                              \
            (lake_sizeof(*(rmw)) <= 4                   \
             ? _atomic_add_64(rmw, arg, model)          \
             : _atomic_op(rmw, +, arg, model))          \
            arg;                                        \
        })
#endif

#define lake_atomic_sub_explicit(rmw, arg, model) \
    lake_atomic_add_explicit(rmw, -(arg), model)
#define lake_atomic_and_explicit(rmw, arg, model) \
    _atomic_op(rmw, &, arg, model)
#define lake_atomic_xor_explicit(rmw, arg, model) \
    _atomic_op(rmw, ^, arg, model)
#define lake_atomic_or_explicit(rmw, arg, model) \
    _atomic_op(rmw, |, arg, model)

#define lake_atomic_nand_explicit(rmw, arg, model) \
    ({ \
        lake_typeof(rmw) rmw__ = (rmw);                                             \
        lake_typeof(arg) arg__ = (arg);                                             \
        lake_typeof(*(rmw)) val__;                                                  \
                                                                                    \
        val__ = lake_atomic_read_explicit(rmw__, lake_memory_model_relaxed);        \
        do {} while (!lake_atomic_compare_exchange_weak_explicit(rmw__, &val__,     \
                        ~(val__ & arg__, model, lake_memory_model_relaxed));        \
        val__;                                                                      \
    })

typedef LAKE_ATOMIC(s32) atomic_spinlock;
#define LAKE_ATOMIC_SPINLOCK_INIT { 0 }

#define lake_atomic_spinlock_try_acquire_explicit(spinlock, model) \
    ((_Bool)_atomic_exchange(spinlock, 1, model))

#define lake_atomic_spinlock_acquire_explicit(spinlock, model) \
    do { /* spin */ } while(lake_atomic_spinlock_try_acquire_explicit(spinlock, model))

#define lake_atomic_spinlock_release(spinlock, model) \
    lake_atomic_store_explicit(spinlock, 0, model)
