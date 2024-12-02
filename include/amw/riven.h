/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_RIVEN_H
#define _AMW_RIVEN_H

#include <amw/common/defines.h>

/** Atomic operations are provided by a CPU-specific implementation. */
#include <amw/arch/atomic.h>

#ifdef __cplusplus
extern "C" {
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

typedef AMW_ATOMIC(u8)                  at_u8_t;
typedef AMW_ATOMIC(u16)                 at_u16_t;
typedef AMW_ATOMIC(u32)                 at_u32_t;
typedef AMW_ATOMIC(u64)                 at_u64_t;

typedef AMW_ATOMIC(i8)                  at_i8_t;
typedef AMW_ATOMIC(i16)                 at_i16_t;
typedef AMW_ATOMIC(i32)                 at_i32_t;
typedef AMW_ATOMIC(i64)                 at_i64_t;

/** Relaxed atomic operations.
 *
 *  When an operation on an atomic variable is not expected to synchronize 
 *  with operations on other (atomic or non-atomic) variables, no memory 
 *  barriers are needed and the relaxed memory ordering can be used. These 
 *  macros make such uses less daunting, but not invisible. */

#define at_store_relaxed(var, value) \
    at_store_explicit(var, value, memory_model_relaxed)
#define at_read_relaxed(var, dst) \
    at_read_explicit(var, dst, memory_model_relaxed)

#define at_compare_exchange_strong_relaxed(dst, exp, src)    \
    at_compare_exchange_strong_relaxed(dst, exp, src,        \
                                       memory_model_relaxed, \
                                       memory_model_relaxed)
#define at_compare_exchange_weak_relaxed(dst, exp, src)      \
    at_compare_exchange_weak_relaxed(dst, exp, src,          \
                                     memory_model_relaxed,   \
                                     memory_model_relaxed)

#define at_add_relaxed(rmw, arg, orig) \
    at_add_explicit(rmw, arg, orig, memory_model_relaxed) 
#define at_sub_relaxed(rmw, arg, orig) \
    at_sub_explicit(rmw, arg, orig, memory_model_relaxed)
#define at_or_relaxed(rmw, arg, orig) \
    at_or_explicit(rmw, arg, orig, memory_model_relaxed)
#define at_xor_relaxed(rmw, arg, orig) \
    at_xor_explicit(rmw, arg, orig, memory_model_relaxed)
#define at_and_relaxed(rmw, arg, orig) \
    at_and_explicit(rmw, arg, orig, memory_model_relaxed)

#define at_flag_test_and_set_relaxed(flag) \
    at_flag_test_and_set_explicit(flag, memory_model_relaxed)
#define at_flag_clear_relaxed(flag) \
    at_flag_clear_explicit(flag, memory_model_relaxed)

/* To enable analysis, set these environment variables before running meson:
 *     export CC=clang
 *     export CFLAGS="-DAMW_THREAD_SAFETY_ANALYSIS -Wthread-safety" */
#if defined(AMW_THREAD_SAFETY_ANALYSIS) && defined(AMW_CC_CLANG_VERSION) && !defined(SWIG) && !defined(AMW_NDEBUG)
    #define _AMW_THREAD_ATTRIBUTE(x) __attribute__((x))
#else
    #define _AMW_THREAD_ATTRIBUTE(x)
#endif

#define AMW_THREAD_CAPABILITY(x)               _AMW_THREAD_ATTRIBUTE(capability(x))
#define AMW_THREAD_SCOPED_CAPABILITY           _AMW_THREAD_ATTRIBUTE(scoped_lockable)
#define AMW_THREAD_GUARDED_BY(x)               _AMW_THREAD_ATTRIBUTE(guarded_by(x))
#define AMW_THREAD_PT_GUARDED_BY(x)            _AMW_THREAD_ATTRIBUTE(pt_guarded_by(x))
#define AMW_THREAD_ACQUIRED_BEFORE(x)          _AMW_THREAD_ATTRIBUTE(acquired_before(x))
#define AMW_THREAD_ACQUIRED_AFTER(x)           _AMW_THREAD_ATTRIBUTE(acquired_after(x))
#define AMW_THREAD_REQUIRES(x)                 _AMW_THREAD_ATTRIBUTE(requires_capability(x))
#define AMW_THREAD_REQUIRES_SHARED(x)          _AMW_THREAD_ATTRIBUTE(requires_shared_capability(x))
#define AMW_THREAD_ACQUIRE(x)                  _AMW_THREAD_ATTRIBUTE(acquire_capability(x))
#define AMW_THREAD_ACQUIRE_SHARED(x)           _AMW_THREAD_ATTRIBUTE(acquire_shared_capability(x))
#define AMW_THREAD_RELEASE(x)                  _AMW_THREAD_ATTRIBUTE(release_capability(x))
#define AMW_THREAD_RELEASE_SHARED(x)           _AMW_THREAD_ATTRIBUTE(release_shared_capability(x))
#define AMW_THREAD_RELEASE_GENERIC(x)          _AMW_THREAD_ATTRIBUTE(release_generic_capability(x))
#define AMW_THREAD_TRY_ACQUIRE(x)              _AMW_THREAD_ATTRIBUTE(try_acquire_capability(x))
#define AMW_THREAD_TRY_ACQUIRE_SHARED(x)       _AMW_THREAD_ATTRIBUTE(try_acquire_shared_capability(x))
#define AMW_THREAD_EXCLUDES(x)                 _AMW_THREAD_ATTRIBUTE(locks_excluded(x))
#define AMW_THREAD_ASSERT_CAPABILITY(x)        _AMW_THREAD_ATTRIBUTE(assert_capability(x))
#define AMW_THREAD_ASSERT_SHARED_CAPABILITY(x) _AMW_THREAD_ATTRIBUTE(assert_shared_capability(x))
#define AMW_THREAD_RETURN_CAPABILITY(x)        _AMW_THREAD_ATTRIBUTE(lock_returned(x))
#define AMW_THREAD_NO_THREAD_SAFETY_ANALYSIS   _AMW_THREAD_ATTRIBUTE(no_thread_safety_analysis)

/** A handle for representing a worker thread. */
typedef u64 thread_t;

/** Creates and runs a worker thread at a given handle. */
AMWAPI void AMWCALL thread_create(thread_t *thread, void *(*procedure)(void *), void *argument);

/** Destroys and joins a given thread. Shouldn't really be called 
 *  unless a dynamic thread was ever spawned outside of riven. */
AMWAPI void AMWCALL thread_destroy(thread_t thread);

/** Just joins a thread, without destroying it. It will wait for the 
 *  thread to finish its work before continuing. */
AMWAPI void AMWCALL thread_join(thread_t thread);

/** Gets the index of the current worker thread, in a given array of threads. */
AMWAPI size_t AMWCALL thread_index(thread_t *threads, size_t thread_count);

/** Retrieve the handle of the current worker thread. */
AMWAPI thread_t AMWCALL thread_current(void);

/** Set thread affinity for an array of threads. Thread count per core should 
 *  be equal to on how many hardware threads has one physical CPU core. The core 
 *  start index should be 0 or less than core_count-1, the thread_count_per_core
 *  should accomodate for how many CPU cores is the affinity created for. */
AMWAPI void AMWCALL thread_affinity(thread_t *threads, size_t thread_count, size_t start_index);

/** I had this fiber job system implemented in my old project.
 *  It is based on ideas presented by Naughty Dog in the talk 
 *  'Parallelizing the Naughty Dog Engine':
 *
 *  [Naughty Dog Video]
 *  http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
 *
 *  [Naughty Dog Slides]
 *  http://twvideo01.ubm-us.net/o1/vault/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf
 *
 *  Since the job system is multithreaded, the jobs (here called tears)
 *  is not guaranteed to be the order that jobs were submitted. They run 
 *  in parallel, that's the deal. 'riven_split_and_unchain' doesn't 
 *  "block", but the current context is swapped with a fiber that contains 
 *  a new tear to process. Only when all tears have been finished will 
 *  the context be swapped back and 'riven_split_and_unchain' returns.
 *  Each thread is always doing work, and not "blocking" on a job that 
 *  is stuck waiting in 'riven_split_and_unchain' or 'riven_unchain' calls.
 *
 *  One problem, context switching is very CPU specific. So this functionality 
 *  is implemented in assembly, for every architecture and platform that 
 *  should be supported. A great deal of help in implementing this is the 
 *  Boost C++ fiber context library. It got most of this figured out, with 
 *  all the registers for different ABI's in place and A LOT of supported archs.
 *  I used this to write some simpler code, so it fits the design i'm going for
 *  here in Riven. If i wish to ever support another architecture, that boost 
 *  library is my goto place to start out.
 *
 *  [Boost::Context]
 *  https://github.com/boostorg/context
 *
 *  Actually compiling the right assembly files is handled in Meson at /src/asm.
 *  It sets up the assembly configuration (OS, ABI and the Assembler) and gives
 *  the corresponding source files to the compiler.
 *  Jeff Preshing has written some excellent articles about atomics operations,
 *  his guides were instrumental in the creation of this system.
 *
 *  [Atomics: Introduction to Lock-Free Programming]
 *  http://preshing.com/20120612/an-introduction-to-lock-free-programming/
 *
 *  The job queue was implemented from the multiple producer, multiple consumer
 *  (MPMC) queue described by Dmitry Vyuko on 1024cores.
 *
 *  [High Speed Atomic MPMC Queue]
 *  http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 *
 *  As the fiber stacks are created as a large flat array, there is NO protection
 *  on stack overflow. An overflow will just walk into the neighbouring fiber's 
 *  stack and corrupt it. In Linux and Windows you can enable a stack guard by 
 *  setting the define 'RIVEN_STACK_GUARD' to a multiple of the target system's 
 *  page size value (4096 bytes on my PC). If the stack gets touched out of bounds
 *  in the guard area, then it should crash (or raise a signal) there and then,
 *  resulting in a useful stacktrace.
 *
 *  Fundamental to the fiber code is thread local storage (TLS). It's used as a 
 *  way for the system to communicate between tears. Instead of using an OS or 
 *  compiler provided TLS, i'm rolling out my own using a lookup on the current 
 *  thread id. That is because:
 *  - '__thread' on GCC is a GCC specific extension.
 *  - '__declspec(thread)' on windows only supports statically linked libraries.
 *  - 'thread_local' in C++11 is.. C++.
 *  - 'TlsAlloc' is uh, just too much work. Lazy motherfucker..
 *  - '_Thread_local' in C11 is nice, but, as with the case of atomics,
 *    i already choose to use C99 and implement the atomics and related stuff myself.
 *
 *  For as how fast Riven is, i don't know. When i was testing on my old project,
 *  using an empty jobs benchmark it gets between 100ns to 150ns per empty job 
 *  called. In the great scheme of things, i think it's slow. So making it faster
 *  will be a great exercise later to throw everything upside down here.
 *
 *  Some ideas to make it better:
 *
 *  - A deadlock happens if the mpmc queue is full and running from only one thread.
 *    I'm a shitty programmer, so till now i'm still wrapping my head around it.
 *    That should not be a real problem if running the engine in full parallel.
 *
 *  - The fibers may not be swapped to find a new job. If there are no old fibers 
 *    to resume, then just run another job instead of putting the fiber on the 
 *    free list and making a new fiber. Implementing this tho will prevent using 
 *    a dual-stack job system as described in the 'Naughty Dog Engine' talk. 
 *    (it is not implemented yet, maybe i'll get there...)
 *
 *  - The free and wait list implementations are pretty naive. There should be 
 *    a better lockless way of accessing them as opposed to looping over them.
 *    I suspect there is a lot of cache churn and false sharing around index 0 
 *    between lots of threads. I should actually try to fix this issue now, as 
 *    i'm moving the source code between my old projects to here.
 *
 *  - May get into improving context switching branch prediction, if this can 
 *    improve speed: http://www.crystalclearsoftware.com/soc/coroutine/coroutine/linuxasm.html
 *
 *  [Fibers, Oh My!]
 *  https://graphitemaster.github.io/fibers/
 */

/** Opaque handle for the fiber job system's context. */
struct riven;

typedef void *riven_argument_t;
typedef void (AMWCALL *PFN_riven_job)(void *argument);
typedef void (AMWCALL *PFN_riven_main)(struct riven *riven,
                                       thread_t *threads,
                                       size_t thread_count,
                                       riven_argument_t argument);

#define AMW_RIVENS_TEARS(JOB_DECL) \
     extern void AMWCALL JOB_DECL(void *argument)

/** A chain points to an atomic counter that is binded to 
 *  a job queue. It will block any wait calls to Riven, until
 *  all jobs bound to the chain are finished. In short, it's 
 *  used as a synchronization primitive for the job system. */
typedef at_size_t riven_chain_t;

/** A job for the fiber context */
struct riven_tear {
    PFN_riven_job    procedure;
    riven_argument_t argument;
    const char      *name;
};

/** Runs 'splits' amount of jobs, passed in the flat array 'tears'.
 *  Waits for them to finish, before returning. So, this will block 
 *  until all jobs have been handled. */
AMWAPI void AMWCALL riven_split_and_unchain(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits);

/** Run 'splits' amount of jobs, passed in the flat array 'tears'.
 *  This will return immediately, and the jobs will be handled in 
 *  the background in parallel. If 'chain' is not null, it will 
 *  be set to a value that can be used to wait for a bound split 
 *  to finish the tears. */
AMWAPI void AMWCALL riven_split(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits,
        riven_chain_t *chain);

/** If chain is not null, then wait for all of the jobs bound to 
 *  a given chain to finish. If the chain is null, then the call 
 *  may or may not be yield to the job system before returning. */
AMWAPI void AMWCALL riven_unchain(
        struct riven *riven, 
        riven_chain_t chain);

/** Used to get an exiled chain that's not bound to any working 
 *  tears. The chain will become a bound state and block any call 
 *  to 'riven_unchain' until 'riven_unchain_exile' is called. */
AMWAPI void AMWCALL riven_chain_exile(
        struct riven *riven,
        riven_chain_t *chain);

/** Used to mark an exiled chain as unbound. Once called, the 
 *  chain is now invalid and should not be used in any future
 *  chain/unchain nor split calls - implies risk of fiber leaks. */
AMWAPI void AMWCALL riven_unchain_exile(riven_chain_t chain);

/** This call serves two purposes:
 *  1) Get the amount of memory (in bytes) needed to allocate. 
 *  2) Entry point for the fiber job system. 
 *
 *  When passed with a non-null value of 'riven_memory', then it 
 *  will initialize it's internal state using the provided memory. 
 *  'fiber_count' of fibers will be created, each with a private 
 *  stack of 'stack_bytes' in size. A threadsafe job queue with 
 *  a size of (1 << log_2_tear_count) jobs. It will then create 
 *  'thread_count' - 1 (main) system threads before calling
 *  'main_procedure' with the arguments of 'riven', 'threads',
 *  'thread_count' and 'argument' to serve as an entry point.
 *  In our case, this will be the mainloop of the game engine.
 *
 *  A 'riven' is an opaque handle required to any calls to the 
 *  fiber job system - it's not the 'riven_memory' itself.
 *  When 'unveil_rift' returns, the memory can be deallocated.
 *
 *  If 'riven_memory' is null, then 'riven_unveil_rift' will return
 *  the amount of bytes it needs for its internal state. This is 
 *  calculated from 'thread_count' threads, 'fiber_count' fibers
 *  each using a stack size of 'stack_bytes', and a job queue of
 *  (1 << log_2_tear_count) tears. Allocate a buffer of this size 
 *  and pass that into Riven to multithread the fuck out of it. */
AMWAPI size_t AMWCALL riven_unveil_rift(
        void *riven_memory,
        size_t fiber_stack_bytes,
        size_t fiber_count,
        size_t log_2_tear_count,
        size_t thread_count,
        PFN_riven_main main_procedure,
        riven_argument_t main_argument);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_RIVEN_H */
