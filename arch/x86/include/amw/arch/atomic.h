/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_X86_ATOMIC_H
#define _AMW_X86_ATOMIC_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

#define compiler_barrier() __asm__ __volatile__(" " : : : "memory")
#ifdef AMW_ARCH_AMD64
    #define cpu_barrier()  __asm__ __volatile__("mfence;" : : : "memory")
#else
    #define cpu_barrier()  __asm__ __volatile__("lock ; addl $0, (%%esp)" : : : "memory", "cc")
#endif

/* The 'volatile' keyword prevents the compiler rom keeping the atomic
 * value in a register, and generates a new memory access for each atomic 
 * operation. This allows the implementations of memory_order_relaxed and 
 * memory_order_consume to avoid issuing a compiler memory barrier, allowing 
 * full optimization of all surrounding non-atomic variables.
 *
 * For 32-bit x86, attribute aligned is used to tell the compiler to align 
 * 64-bit data on a 8-byte boundary. This allows more efficient atomic access,
 * as the CPU guarantees such memory accessess to be atomic. */
#if defined(AMW_ARCH_AMD64)
    #define AMW_ATOMIC(type) type volatile
#else
    #include <amw/common/align.h>
    #define AMW_ATOMIC(type) type volatile AMW_ALIGN_TO(sizeof(type))
#endif

enum memory_model {
    memory_model_relaxed,
    memory_model_consume,
    memory_model_acquire,
    memory_model_release,
    memory_model_acq_rel,
    memory_model_seq_cst,
};

#define AMW_ATOMIC_BOOL_LOCK_FREE 2
#define AMW_ATOMIC_CHAR_LOCK_FREE 2
#define AMW_ATOMIC_SHORT_LOCK_FREE 2
#define AMW_ATOMIC_INT_LOCK_FREE 2
#define AMW_ATOMIC_LONG_LOCK_FREE 2
#define AMW_ATOMIC_LLONG_LOCK_FREE 2
#define AMW_ATOMIC_POINTER_LOCK_FREE 2

#define _AMW_IS_LOCKLESS_ATOMIC(obj) \
    (sizeof(obj) <= 8 && is_pow2(sizeof(obj)))

#define at_init(obj, val) (*(obj) = (val), (void)0)

/** The memory_model_relaxed does not need a compiler barrier, if the 
 *  atomic operation can otherwise be guaranteed to not be moved with 
 *  respect to other atomic operations on the same memory location. Using 
 *  the 'volatile' keyword in the definition of the atomic types accomplishes 
 *  this, as memory access to volatile data may not be optimized away, or be 
 *  reordered with other volatile accesses. 
 *
 *  On x86 also memory_model_consume is automatic, and data dependency 
 *  on a volatile atomic variable means that the compiler optimizations should 
 *  not cause problems. That is, the compiler should not speculate the value of 
 *  the amw_atomic_read, as it is going to read it from the memory anyway. 
 *  This allows omiting the compiler memory barrier on atomic reads with 
 *  memory_model_consume. This matches the definition of 
 *  smp_read_barrier_depends() in Linux kernel as a nop for x86, 
 *  and its usage in rcu_dereference(). */
AMW_INLINE void at_compiler_barrier(enum memory_model model) {
    if (model > memory_model_consume) {
        compiler_barrier();
    }
}

AMW_INLINE void at_thread_fence(enum memory_model model) {
    if (model == memory_model_seq_cst) {
        cpu_barrier();
    } else {
        at_compiler_barrier(model);
    }
}

AMW_INLINE void at_signal_fence(enum memory_model model) {
    at_compiler_barrier(model);
}

#define at_is_lock_free(obj) \
    ((void)*(obj), _AMW_IS_LOCKLESS_ATOMIC(*(obj)) ? 2 : 0)

#if defined(AMW_ARCH_AMD64) 
    #define atomic_exchange__(dst, src, model)                  \
        ({                                                      \
            AMW_TYPEOF(dst) dst___ = (dst);                     \
            AMW_TYPEOF(*(dst)) src___ = (src);                  \
                                                                \
            if ((model) > memory_model_consume) {               \
                __asm__ __volatile__("xchg %1, %0 ; "           \
                                     "# atomic_exchange__"      \
                                     : "+r" (src___), /* 0 */   \
                                       "+m" (*dst___) /* 1 */   \
                                     : : "memory");             \
            } else {                                            \
                __asm__ __volatile__("xchg %1, %0 ; "           \
                                     "# atomic_exchange__"      \
                                     : "+r" (src___),   /* 0 */ \
                                       "+m" (*dst___)); /* 1 */ \
            }                                                   \
            src___;                                             \
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
        #define atomic_exchange_8__(dst, src, clob)             \
            u32 temp___;                                        \
            __asm__ __volatile__("      movl %%ebx, %2 ;    "   \
                                 "      movl %%eax, %%ebx ; "   \
                                 "      movl %%edx, %%ecx ; "   \
                                 "1:                        "   \
                                 "lock; cmpchg8b (%0) ;     "   \
                                 "      jne 1b ;            "   \
                                 "      movl %2, %%ebx ;    "   \
                                 "# atomic_exchange_8__     "   \
                                 : "+r" (dst),     /* 0 */      \
                                   "+A" (src),     /* 1 */      \
                                   "=mr" (temp___) /* 2 */      \
                                 : : "ecx", clob, "cc")
    #else
        #define atomic_exchange_8__(dst, src, clob)             \
            __asm__ __volatile__("      movl %%eax, %%ebx ; "   \
                                 "      movl %%edx, %%ecx ; "   \
                                 "1:                        "   \
                                 "lock; cmpchg8b %0 ;       "   \
                                 "      jne 1b ;            "   \
                                 "# atomic_exchange_8__     "   \
                                 : "+m" (*dst),    /* 0 */      \
                                   "+A" (src),     /* 1 */      \
                                 : : "ebx", "ecx", clob, "cc")
    #endif

    #define atomic_exchange__(dst, src, model)                      \
        ({                                                          \
            AMW_TYPEOF(dst) dst___ = (dst);                         \
            AMW_TYPEOF(*(dst)) src___ = (src);                      \
                                                                    \
            if ((model) > memory_model_consume) {                   \
                if (sizeof(*(dst)) == 8) {                          \
                                                                    \
                } else {                                            \
                    __asm__ __volatile__("xchg %1, %0 ;      "      \
                                         "# atomic_exchange__"      \
                                         : "+r" (src___), /* 0 */   \
                                           "+m" (*dst___) /* 1 */   \
                                         : : "memory");             \
                }                                                   \
            } else {                                                \
                if (sizeof(*(dst)) == 8) {                          \
                    atomic_exchange_8__(dst___, src___, "cc");      \
                } else {                                            \
                    __asm__ __volatile__("xchg %1, %0 ;      "      \
                                         "# atomic_exchange__"      \
                                         : "+r" (src___),   /* 0 */ \
                                           "+m" (*dst___)); /* 1 */ \
                }
            }
            src___;
        })
#endif

/** Atomic store. Valid memory models are:
 *
 *  memory_model_relaxed,
 *  memory_model_release, and
 *  memory_model_seq_cst. */
#if defined(AMW_ARCH_AMD64)
    #define at_store_explicit(dst, src, model)              \
        ({                                                  \
            AMW_TYPEOF(dst) dst__ = (dst);                  \
            AMW_TYPEOF(*(dst)) src__ = (src);               \
                                                            \
            if ((model) != memory_model_seq_cst) {          \
                at_compiler_barrier(model);                 \
                *dst__ = src__;                             \
            } else {                                        \
                atomic_exchange__(dst__, src__, model);     \
            }                                               \
            (void)0;                                        \
        })
#else /* 32-bit */
    #if defined(AMW_ARCH_X86_SSE)
        /** SSE registers are 128-bit wide, and moving the lowest 64-bits of an SSE 
         *  register to properly aligned memory is atomic. See AMW_ATOMIC(type) docs. */
        #define atomic_store_8__(dst, src)                  \
            __asm__ __volatile__("movq %1, %0 ;"            \
                                 "# atomic_store_8__"       \
                                 : "=m" (*dst) /* 0 */      \
                                 : "x" (src)) /* 1, SSE */
    #else
        /** Locked 64-bit exchange is available on i586 CPUs, not sure of older ones. */
        #define atomic_store_8__(dst, src) \
            atomic_exchange_8__(dst, src, "cc")
    #endif

    #define at_store_explicit(dst, src, model)              \
        ({                                                  \
            AMW_TYPEOF(dst) dst__ = (dst);                  \
            AMW_TYPEOF(*(dst)) src__ = (src);               \
                                                            \
            if ((model) != memory_model_seq_cst) {          \
                at_compiler_barrier(model);                 \
                if (sizeof(*(dst)) == 8) {                  \
                    atomic_store_8__(dst__, src__);         \
                } else {                                    \
                    *dst__ = src__;                         \
                }                                           \
                *dst__ = src__;                             \
            } else {                                        \
                atomic_exchange__(dst__, src__, model);     \
            }                                               \
            (void)0;                                        \
        })
#endif

#define at_store(dst, src) \
    at_store_explicit(dst, src, memory_model_seq_cst)

/** Atomic read. Valid memory models are:
 *
 *  memory_model_relaxed,
 *  memory_model_consume,
 *  memory_model_acquire, and
 *  memory_model_seq_cst. */
#if defined(AMW_ARCH_AMD64)
    #define at_read_explicit(src, model)       \
        ({                                     \
            AMW_TYPEOF(src) src__ = (src);     \
                                               \
            at_compiler_barrier(model);        \
            *src__;                            \
        })
#else /* 32-bit */
    #if defined(AMW_ARCH_X86_SSE)
        /** SSE registers are 128-bit wide, and moving the lowest 64-bits of an SSE 
         *  register to properly aligned memory is atomic. See AMW_ATOMIC(type) docs. */
        #define atomic_read_8__(src, dst)               \
            ({                                          \
                AMW_TYPEOF(*(dst)) res___;              \
                                                        \
                __asm__("movq %1, %0 ;    "             \
                        "# atomic_read_8__"             \
                        : "=x" (res___) /* 0, SSE */    \
                        : "m" (*src)); /* 1 */          \
                *(dst) = res__;                         \
            })
    #else
        /** Must use locked cmpxchg8b if compiled w/o SSE support. Compare *dst to a 
         *  random value in bx:cx and return the actual value in ax:dx. The registers 
         *  bx and cx are only read, so they are not clobbered. */
        #define atomic_read_8__(src, dst)               \
            ({                                          \
                AMW_TYPEOF(*(dst)) res___;              \
                                                        \
                __asm__("      movl %%ebx, %%eax ; "    \
                        "      movl %%ecx, %%edx ; "    \
                        "lock; cmpxchg8b %1 ;      "    \
                        "# atomic_read_8__         "    \
                        : "=&A" (res___), /* 0 */       \
                          "+m" (*src) /* 1 */           \
                        : : "cc");                      \
                *(dst) = res___;                        \
            })
    #endif

    #define at_read_explicit(src, model)        \
        ({                                      \
            AMW_TYPEOF(src) src__ = (src);      \
            AMW_TYPEOF(*(src)) res__;           \
                                                \
            if (sizeof(*(dst)) <= 4) {          \
                res__ = *src__;                 \
            } else {                            \
                atomic_read_8__(src, res__);    \
            }                                   \
            at_compiler_barrier(model);         \
            res__;                              \
        })
#endif

#define at_read(src) \
    at_read_explicit(src, memory_model_seq_cst)

/** All memory models are valid for read-modify-write operations.
 *  Valid memory models for the read operation of the current value in 
 *  the failure case are the same as for atomic read, but can not be 
 *  stronger than the success memory model. 'model_fail' is ignores,
 *  as atomic_compare_exchange__ already implements at least as 
 *  strong a barrier as allowed for 'model_fail' in all cases. 
 *  It's left for compatibility with other platforms. */
#if defined(AMW_ARCH_AMD64)
    #define atomic_compare_exchange__(dst, exp, src, res, clob) \
        __asm__ __volatile__("lock; cmpxchg %3, %1 ;      " \
                             "      sete    %0            " \
                             "# atomic_compare_exchange__ " \
                             : "=q" (res),  /* 0 */         \
                               "+m" (*dst), /* 1 */         \
                               "+a" (exp)   /* 2 */         \
                             : "r" (src)    /* 3 */         \
                             : clob, "cc")
    
    #define at_compare_exchange_strong_explicit(dst, exp, src, model, model_fail)   \
        ({                                                                          \
            AMW_TYPEOF(dst) dst__ = (dst);                                          \
            AMW_TYPEOF(dst) expp__ = (exp);                                         \
            AMW_TYPEOF(*(dst)) src__ = (src);                                       \
            AMW_TYPEOF(*(dst)) exp__ = *expp__;                                     \
            u8 res__;                                                               \
            (void)model_fail;                                                       \
                                                                                    \
            if ((model) > memory_model_consume) {                                   \
                atomic_compare_exchange__(dst__, exp__, src__, res__, "memory");    \
            } else {                                                                \
                atomic_compare_exchange__(dst__, exp__, src__, res__, "cc");        \
            }                                                                       \
            if (!res__) {                                                           \
                *expp__ = exp__;                                                    \
            }                                                                       \
            (bool)res__;                                                            \
        })
#else /* 32-bit */
    #if defined(__PIC__)
        /** ebx may not be used as an input when compiled with -fPIC, must save
         *  and restore it. Furthermore, 'DST' may be addressed via ebx, so
         *  the address must be passed via a register so that it remains valid
         *  also after changing ebx. */
        #define atomic_compare_exchange_8__(dst, exp, src, res, clob)   \
            __asm__ __volatile__("      xchgl %%ebx,%3 ;       "        \
                                 "lock; cmpxchg8b (%1) ;       "        \
                                 "      xchgl %3,%%ebx ;       "        \
                                 "      sete %0                "        \
                                 "# atomic_compare_exchange_8__"        \
                                 : "=q" (res),                 /* 0 */  \
                                   "+r" (dst),                 /* 1 */  \
                                   "+A" (exp)                  /* 2 */  \
                                 : "r" ((u32)src),             /* 3 */  \
                                   "c" ((u32)((u64)src >> 32)) /* 4 */  \
                                 : clob, "cc")
    #else
        #define atomic_compare_exchange_8__(dst, exp, src, res, clob)   \
            __asm__ __volatile__("lock; cmpxchg8b %1 ;         "        \
                                 "      sete %0                "        \
                                 "# atomic_compare_exchange_8__"        \
                                 : "=q" (res),                 /* 0 */  \
                                   "+m" (*dst),                /* 1 */  \
                                   "+A" (exp)                  /* 2 */  \
                                 : "b" ((u32)src),             /* 3 */  \
                                   "c" ((u32)((u64)src >> 32)) /* 4 */  \
                                 : clob, "cc")
    #endif
    #define atomic_compare_exchange__(dst, exp, src, res, clob) \
        __asm__ __volatile__("lock; cmpxchg %3,%1 ;      "      \
                             "      sete    %0           "      \
                             "# atomic_compare_exchange__"      \
                             : "=q" (res),  /* 0 */             \
                               "+m" (*dst), /* 1 */             \
                               "+a" (exp)   /* 2 */             \
                             : "r" (src)    /* 3 */             \
                             : clob, "cc")

    #define at_compare_exchange_strong_explicit(dst, exp, src, model, model_fail)       \
        ({                                                                              \
            AMW_TYPEOF(dst) dst__ = (dst);                                              \
            AMW_TYPEOF(dst) expp__ = (exp);                                             \
            AMW_TYPEOF(*(dst)) src__ = (src);                                           \
            AMW_TYPEOF(*(dst)) exp__ = *expp__;                                         \
            u8 res__;                                                                   \
            (void)model_fail;                                                           \
                                                                                        \
            if ((model) > memory_model_consume) {                                       \
                if (sizeof(*(dst)) <= 4) {                                              \
                    atomic_compare_exchange__(dst__, exp__, src__, res__, "memory");    \
                } else {                                                                \
                    atomic_compare_exchange_8__(dst__, exp__, src__, res__, "memory");  \
                }                                                                       \
            } else {                                                                    \
                if (sizeof(*(dst)) <= 4) {                                              \
                    atomic_compare_exchange__(dst__, exp__, src__, res__, "cc");        \
                } else {                                                                \
                    atomic_compare_exchange_8__(dst__, exp__, src__, res__, "cc");      \
                }                                                                       \
            }                                                                           \
            if (!res__) {                                                               \
                *expp__ = exp__;                                                        \
            }                                                                           \
            (bool)res__;                                                                \
        })
#endif

#define at_compare_exchange_strong(dst, exp, src)               \
    at_compare_exchange_strong_explicit(dst, exp, src,          \
                                        memory_model_seq_cst,   \
                                        memory_model_seq_cst)

#define at_compare_exchange_weak \
    at_compare_exchange_strong
#define at_compare_exchange_weak_explicit \
    at_compare_exchange_strong_explicit

#define at_exchange_explicit(rmw, arg, model) \
    atomic_exchange__(rmw, arg, model)
#define at_exchange(rmw, arg) \
    at_exchange_explicit(rmw, arg, memory_model_seq_cst)

#define atomic_op__(rmw, op, arg, model)                                    \
    ({                                                                      \
        AMW_TYPEOF(rmw) rmw__ = (rmw);                                      \
        AMW_TYPEOF(arg) arg__ = (arg);                                      \
                                                                            \
        AMW_TYPEOF(*(rmw)) val__;                                           \
                                                                            \
        val__ = at_read_explicit(rmw__, memory_model_relaxed);              \
        do {                                                                \
        } while (!at_compare_exchange_weak_explicit(rmw__, &val__,          \
                                                    val__ op arg__,         \
                                                    model,                  \
                                                    memory_model_relaxed)); \
        val__;                                                              \
    })

#define atomic_add_64__(rmw, arg, clob) \
    __asm__ __volatile__("lock; xadd %0, %1 ; " \
                         "# atomic_add_64__   " \
                         : "+r" (arg), /* 0 */  \
                           "+m" (*rmw) /* 1 */  \
                         : : clob, "cc")

#define atomic_add__(rmw, arg, model)                   \
    ({                                                  \
        AMW_TYPEOF(rmw) rmw__ = (rmw);                  \
        AMW_TYPEOF(*(rmw)) arg__ = (arg);               \
                                                        \
        if ((model) > memory_model_consume) {           \
            atomic_add_64__(rmw__, arg__, "memory");    \
        } else {                                        \
            atomic_add_64__(rmw__, arg__, "cc");        \
        }                                               \
        arg__;                                          \
    })

#if defined(AMW_ARCH_AMD64)
    #define at_fetch_add_explicit(rmw, arg, model)  \
            atomic_add__(rmw, arg, model)           \

#else /* 32-bit */
    #define at_fetch_add_explicit(rmw, arg, model)  \
        ({                                          \
            (sizeof(*(rmw)) <= 4                    \
             ? atomic_add_64__(rmw, arg, model)     \
             : atomic_op__(rmw, +, arg, model))     \
            arg;                                    \
        })
#endif

#define at_fetch_add(rmw, arg) \
    at_fetch_add_explicit(rmw, arg, memory_model_seq_cst)

#define at_fetch_sub_explicit(rmw, arg, model) \
    at_fetch_add_explicit(rmw, -(arg), model)
#define at_fetch_sub(rmw, arg) \
    at_fetch_sub_explicit(rmw, arg, memory_model_seq_cst)

#define at_fetch_or_explicit(rmw, arg, model) \
    atomic_op__(rmw, |, arg, model)
#define at_fetch_or(rmw, arg) \
    at_fetch_or_explicit(rmw, arg, memory_model_seq_cst)

#define at_fetch_xor_explicit(rmw, arg, model) \
    atomic_op__(rmw, ^, arg, model)
#define at_fetch_xor(rmw, arg) \
    at_fetch_xor_explicit(rmw, arg, memory_model_seq_cst)

#define at_fetch_and_explicit(rmw, arg, model) \
    atomic_op__(rmw, &, arg, model)
#define at_fetch_and(rmw, arg) \
    at_fetch_and_explicit(rmw, arg, memory_model_seq_cst)

typedef AMW_ATOMIC(int) at_flag_t;
#define AMW_ATOMIC_FLAG_INIT { false }

#define at_flag_test_and_set_explicit(flag, model) \
    ((bool)atomic_exchange__(flag, 1, model))
#define at_flag_test_and_set(flag) \
    at_flag_test_and_set_explicit(flag, memory_model_seq_cst)

#define at_flag_clear_explicit(flag, model) \
    at_store_explicit(flag, 0, model)
#define at_flag_clear(flag) \
    at_flag_clear_explicit(flag, memory_model_seq_cst)

AMW_C_DECL_END
#endif /* _AMW_X86_ATOMIC_H */
