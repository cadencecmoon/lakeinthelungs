#pragma once

#include "defines.h"
#include "types.h"
#include "atomics.h"

/** This fiber-based job system is implemented based on ideas presented by 
 *  Naughty Dog in the GDC2015 talk "Parallelizing the Naughty Dog Engine using Fibers".
 *
 *  [Naughty Dog Video]
 *  http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
 *
 *  [Naughty Dog Slides]
 *  http://twvideo01.ubm-us.net/o1/vault/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf
 *
 *  Since the job system is multithreaded, the jobs (here called tears)
 *  are not guaranteed to be executed in the order that jobs were submitted. 
 *  They run in parallel, that's the deal. 'riven_split_and_unchain' doesn't 
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
 *  library is my goto place to start out with a port.
 *
 *  [Boost::Context]
 *  https://github.com/boostorg/context
 *
 *  Actually compiling the right assembly files is handled in Meson at /src/arch.
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
 *  will be a great exercise later that will throw everything upside down here.
 *
 *  Some ideas to make it better:
 *
 *  - A deadlock happens if the mpmc queue is full and running from only one thread.
 *    I'm a shitty programmer, so till now i'm still wrapping my head around it.
 *    That should not be a real problem if running the engine on a multicore CPU,
 *    with atleast two threads used in the application.
 *
 *  - The fibers may not be swapped to find a new job. If there are no old fibers 
 *    to resume, then just run another job instead of putting the fiber on the 
 *    free list and making a new fiber. Implementing this tho will prevent using 
 *    a dual-stack size job system as described in the 'Naughty Dog Engine' talk. 
 *    (it is not implemented yet, maybe i'll get there...)
 *
 *  - The free and wait list implementations are pretty naive. There should be 
 *    a better lockless way of accessing them as opposed to looping over them.
 *    I suspect there is a lot of cache churn and false sharing around index 0 
 *    between lots of threads, so it may be an optimization point later. 
 *
 *  - May get into improving context switching branch prediction, if this can improve speed: 
 *    http://www.crystalclearsoftware.com/soc/coroutine/coroutine/linuxasm.html
 *
 *  [Fibers, Oh My!]
 *  https://graphitemaster.github.io/fibers/
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque handle to use the fiber-based job system. */
struct riven;

/** A handle for representing a worker thread. */
typedef u64 thread_t;

typedef void (*PFN_riven_work)(void *argument);
typedef void (*PFN_riven_main)(struct riven *riven, thread_t *threads, ssize thread_count, void *argument);

/** A chain points to an atomic counter that is binded to a job queue. 
 *  If used within a Riven call, it will block and wait until all jobs 
 *  bound to the chain finish, in the meantime the caller doesnt 'wait'
 *  but does a context switch to work on the jobs too. In short, it's 
 *  used as a synchronization primitive for the job system. */
typedef at_ssize *rivens_chain_t;

/** A job for the fiber context */
struct rivens_tear {
    PFN_riven_work procedure;
    void          *argument;
    const char    *name;
};

/** Run 'splits' amount of jobs, passed in the flat array 'tears'.
 *  This will return immediately, and the jobs will be handled in 
 *  the background in parallel. If 'chain' is not null, it will 
 *  be set to a value that can be used to wait for a bound split 
 *  to finish the tears. */
LAKEAPI void riven_split(
    struct riven       *riven, 
    struct rivens_tear *tears, 
    ssize               splits, 
    rivens_chain_t     *chain)
    attr_nonnull(1,2);

/** If chain is not null, then wait for all of the jobs bound to 
 *  a given chain to finish. If the chain is null, then the call 
 *  may or may not be yield to the job system before returning.
 *  Exiled chains may be used to synchronize between work outside 
 *  of a rivens_tear - if chained, the thread will take the next 
 *  free fiber and do a context switch instead of blocking. */
LAKEAPI void riven_unchain(struct riven *riven, rivens_chain_t chain) attr_nonnull(1);

/** Runs 'splits' amount of jobs, passed in the flat array 'tears'.
 *  Waits for them to finish, before returning. So, this will block 
 *  until all jobs have been handled. */
INLINE void riven_split_and_unchain(
    struct riven       *riven, 
    struct rivens_tear *tears, 
    ssize               splits)
{
    rivens_chain_t chain;
    riven_split(riven, tears, splits, &chain);
    riven_unchain(riven, chain);
}

/** Used to get an exiled chain that's not bound to any working 
 *  tears. The chain will become a bound state and block any call 
 *  to riven_unchain() until riven_release_chain() is called. */
LAKEAPI void riven_acquire_chain(struct riven *riven, rivens_chain_t *chain) attr_nonnull(1);

/** Used to mark an exiled chain as unbound. Once called, the 
 *  chain is now reset and will not block calls to riven_unchain(). */
INLINE void riven_release_chain(rivens_chain_t chain)
{
    at_ssize *counter = chain;
    if (counter) at_store_explicit(counter, 0lu, memory_model_release);
}

/** This function serves as the entry point to the fiber job system.
 *  Riven's memory will be initialized using a standard malloc.
 *  'fiber_count' of fibers will be created, each with a private 
 *  stack each of a size 'stack_bytes'. A threadsafe job queue with 
 *  a capacity of (1 << log_2_tear_count) jobs is created, the queue 
 *  has no concept of a "job priority". 'thread_count' -1 (main) 
 *  system threads will be created before calling 'main_procedure'
 *  with the arguments of 'riven', 'threads', 'thread_count' and 
 *  'argument' to serve as an entry point. */
LAKEAPI ssize riven_unveil_rift(
    void          *riven_memory,
    ssize          fiber_stack_bytes,
    ssize          fiber_count,
    ssize          log_2_tear_count,
    ssize          thread_count,
    PFN_riven_main main_procedure,
    void          *main_argument)
    attr_nonnull(6);

#ifdef __cplusplus
}
#endif
