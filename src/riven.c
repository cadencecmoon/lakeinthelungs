#include <amw/bedrock/hash_table.h>
#include <amw/bedrock/log.h>
#include <amw/bedrock/process.h>
#include <amw/aria/bits.h>
#include <amw/riven.h>

/** This fiber-based job system is implemented based on ideas presented by Naughty Dog 
 *  in the GDC2015 talk "Parallelizing the Naughty Dog Engine using Fibers". 
 *
 *  [Naughty Dog Video]
 *  http://gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
 *
 *  [Naughty Dog Slides]
 *  http://twvideo01.ubm-us.net/o1/vault/gdc2015/presentations/Gyrling_Christian_Parallelizing_The_Naughty.pdf
 *
 *  Because Riven is a core system to this game engine, and because i needed an optimal 
 *  solution for managing dynamic resources in a fiber-aware manner, Riven is responsible 
 *  for the game's host memory management. A lock-less block allocator reserves memory 
 *  under arena allocators owned by a uint32_t tag, called tagged heaps. A tag can be of 
 *  any value, but predefined tags exist that have expected lifetime frequencies in the 
 *  gameloop, even including thread-local tagged heaps for transient per-frame resources.
 *  The point is, locks are unnecessary for 90% of allocations done, and whenever these 
 *  would be needed, context switches or offset synchronization of memory access is used.
 *
 *  Since the job system is multithreaded, the work is not guaranteed to be executed 
 *  in the order that it was submitted. Work runs in parallel, we'll, that's the deal. 
 *  'riven_split_work_and_unchain' doesn't "block", instead the current execution context 
 *  is swapped with a fiber that contains new work to process. Only when all work of this 
 *  split has been finished will the context be swapped back and this function returns.
 *  Each thread is always doing work, and not "blocking" on a job that is stuck waiting 
 *  in 'riven_split_work_and_unchain' or 'riven_unchain' calls. When there is no work at 
 *  the moment, the threads may yield their processing time to the sheduler when busy-waiting
 *  for new work to enqueue the job ring buffer.
 *
 *  Context switching is very CPU specific. This functionality is implemented in assembly,
 *  for every architecture and platform ABI that should be supported. A great deal of help 
 *  in implementing this is the Boost C++ fiber context library. It has most of this figured 
 *  out, with all the registers for different ABI's in place and A LOT of supported archs.
 *  I used this to write some simpler code, so it fits the design I'm going for here in Riven. 
 *  If i wish to ever support another architecture, that boost library is my goto place to 
 *  start out with working on a port.
 *
 *  [Boost::Context]
 *  https://github.com/boostorg/context
 *
 *  Actually compiling the right assembly files is handled in Meson at /src/<arch>. It sets 
 *  up the assembly configuration (arch directory, ABI and the assembler's file extension) 
 *  then writes the source files within AMWEngine's sources. Jeff Preshing has written some 
 *  excellent articles about atomics operations, his guides were instrumental in the creation 
 *  of this system, and for me to understand lockless programming.
 *
 *  [Atomics: Introduction to Lock-Free Programming]
 *  http://preshing.com/20120612/an-introduction-to-lock-free-programming/
 *
 *  The job queue was implemented from the multiple producer, multiple consumer (MPMC) queue 
 *  described by Dmitry Vyuko on 1024cores. I used the same structure for a ring buffer to 
 *  rotate reused allocation structures within tagged heaps.
 *
 *  [High Speed Atomic MPMC Queue]
 *  http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 *
 *  As the fiber stacks are created as a large flat array, there is NO protection on a stack 
 *  overflow. It will just walk into the neighbouring fiber's stack and corrupt it. This is 
 *  an issue now FIXME, but i could implement some measures to prevent these, or atleast, 
 *  to signal us when an overflow happened. We could save a canary value at the beginning 
 *  of the stack (a stack grows from the top downwards) and periodically check this value, 
 *  asserting if this value ever changes. Guard pages sadly cannot be used, atleast not when 
 *  we're using huge pages (by default 2MB) for our page entries.
 *
 *  [Fibers, Oh My!]
 *  https://graphitemaster.github.io/fibers/
 *
 *  Fundamental to the fiber code is thread local storage (TLS). It's used as a way for the 
 *  system to communicate between jobs. Instead of using an OS or compiler provided TLS,
 *  I'm rolling out my own using a lookup on the current thread id. That is because:
 *
 *  - '__thread' on GCC is a GCC specific extension.
 *  - '__declspec(thread)' on windows only supports statically linked libraries.
 *  - 'thread_local' in C++11 is.. C++.
 *  - 'TlsAlloc' is uh, just too much work. Lazy motherfucker..
 *  - '_Thread_local' in C11 is nice, but, as with the case of atomics,
 *    i already choose to use C99 and implement the atomics and related stuff myself.
 *
 *  The thread id lookup was before implemented by iterating over a thread array and comparing 
 *  handles, but that was not really an optimal way to do stuff, and the time to calculate an 
 *  index of the current thread was not a constant value. Then I added a hash table to write 
 *  thread IDs as keys to retrieve an index. I expose this procedure with 'riven_thread_index'. 
 *
 *  For as how fast Riven is, we'll, I don't know. When i was testing on my old project, using 
 *  an empty jobs benchmark it got me between 100ns to 150ns per empty job called. In the great 
 *  scheme of things, I think it's slow. I made some changes from this time, including actually 
 *  implementing the memory allocator within the job system, adding the mentioned hash table, 
 *  reusing fibers by executing some jobs inline... Maybe I should implement some tests here 
 *  to actually check and benchmark stuff as a lot has changed from my old project.
 *
 *  Some notes:
 *
 *  - A deadlock happens if the mpmc ring buffer is full and running from only one thread. 
 *    This will also happen when dequeuing from an empty buffer, when running a single thread.
 *    The reason being, that the waiting for enqueue/dequeue thread may never leave the loop,
 *    when there is no other thread to try to empty/fill it. This should not really be an issue 
 *    unless stress testing the job system on single core environments.
 *    
 *  - The free and wait list implementations are pretty naive. There should be a better lockless 
 *    way of accessing them as opposed to looping over them. I suspect there is a lot of cache 
 *    churn and false sharing around index 0 between lots of threads.
 *
 *  - May get into improving context switching branch prediction, if this can improve speed: 
 *    http://www.crystalclearsoftware.com/soc/coroutine/coroutine/linuxasm.html
 *
 *  - The allocator and threading needs platform-specific APIs to work. For now POSIX-compliant
 *    systems are for sure supported, where Windows needs some testing but I expect it to run with 
 *    some fine work. I could implement a WebAssembly backend, when I'll decide that I'd like to 
 *    make an emscripten web build of my game, but that will not be my priority at any point - more 
 *    like an exercise whenever I'll decide I'm bored from working on other stuff.
 *
 *  - Virtual memory is mapped upfront, and commited only when needed. The block allocator 
 *    represents all memory in this budget with a bitmap, where a bit is equal to a block 
 *    of 2MB of memory. This is stupidly efficient for single-block allocations (as I expect 
 *    only a fraction of all allocations to be more than 2MB of size), and makes fitting 
 *    larger allocations within the available space a breeze. There is no real defragmentation
 *    implemented, so the strategy used for the allocator may still need some work when it 
 *    happens to become a chokepoint for heavy resource management.
 */

#ifndef RIVEN_BLOCK_SIZE
#define RIVEN_BLOCK_SIZE (2lu*1024lu*1024lu)
#endif
#ifndef RIVEN_DEFERRED_HEAPS
#define RIVEN_DEFERRED_HEAPS 3
#endif
#ifndef RIVEN_USE_HUGEPAGES
#define RIVEN_USE_HUGEPAGES 1
#endif

#define RIVEN_FIBER_INVALID (SIZE_MAX)

static_assert(sizeof(sptr) == sizeof(struct riven *), "intptr_t can't hold pointers");
static_assert(sizeof(usize) >= sizeof(void *), "size_t can't hold pointers");
static_assert(((RIVEN_BLOCK_SIZE != 0) && ((RIVEN_BLOCK_SIZE & (RIVEN_BLOCK_SIZE-1)) == 0)), "block size must be a power of 2");

#define A2MB(x)  (((x) + RIVEN_BLOCK_SIZE-1lu) & ~(RIVEN_BLOCK_SIZE-1lu))
#define A16MB(x) (((x) + 8*RIVEN_BLOCK_SIZE-1lu) & ~(8lu*RIVEN_BLOCK_SIZE-1lu))

#if defined(PLATFORM_UNIX)
    #include <unistd.h>
    #include <sys/mman.h>
    #include <errno.h>
#ifndef RIVEN_DISABLE_YIELD
    #include <sched.h>
#endif
#elif defined(PLATFORM_WINDOWS)
    #include "windows/windows.h"
    #include <process.h>
#endif

#ifdef RIVEN_DISABLE_YIELD
#define yield() ((void)0)
#else
attr_inline void yield(void) {
#if defined(PLATFORM_UNIX)
    sched_yield();
#elif defined(PLATFORM_WINDOWS)
    Sleep(0);
#endif
}
#endif

static void *map_virtual_memory(usize budget, usize huge_page_size)
{
    void *mapped = NULL;
#if defined(PLATFORM_UNIX)
    s32 flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
    if (!RIVEN_USE_HUGEPAGES) {
        /* ignore */
#if defined(MAP_HUGE_16MB)
    } else if (huge_page_size == 16*1024*1024) {
        flags |= MAP_HUGE_16MB;
#endif /* 16MB */
#if defined(MAP_HUGE_4MB)
    } else if (huge_page_size == 4*1024*1024) {
        flags |= MAP_HUGE_4MB;
#endif /* 4MB */
#if defined(MAP_HUGE_2MB)
    } else if (huge_page_size == 2*1024*1024) {
        flags |= MAP_HUGE_2MB;
#endif /* 2MB */
#if defined(MAP_HUGE_1MB)
    } else if (huge_page_size == 1024*1024) {
        flags |= MAP_HUGE_1MB;
#endif /* 1MB */
#if defined(MAP_HUGE_256KB)
    } else if (huge_page_size == 256*1024) {
        flags |= MAP_HUGE_256KB;
#endif /* 256KB */
#if defined(MAP_HUGE_64KB)
    } else if (huge_page_size == 64*1024) {
        flags |= MAP_HUGE_64KB;
#endif /* 64KB */
#if defined(MAP_HUGE_16KB)
    } else if (huge_page_size == 16*1024) {
        flags |= MAP_HUGE_16KB;
#endif /* 16KB */
    }
    mapped = mmap(NULL, budget, PROT_NONE, flags, -1, 0);
    if (mapped == MAP_FAILED) {
        log_fatal("Failed to reserve virtual memory (mmap) of %lu bytes (%lumb): %s.", budget, budget>>20, strerror(errno));
        return NULL;
    }
#elif defined(PLATFORM_WINDOWS)
    s32 flags = MEM_RESERVE;
    SIZE_T large_page_min = GetLargePageMinimum();
    if (large_page_min > 0 && huge_page_size > 4096)
        flags |= MEM_LARGE_PAGES;
    mapped = VirtualAllocEx(GetCurrentProcess(), NULL, budget, flags, PAGE_NOACCESS);
    if (!mapped) {
        log_fatal("Failed to reserve virtual memory (VirtualAlloc) of %lu bytes (%lumb), error: %lu.", budget, budget>>20, GetLastError());
        return NULL;
    }
#elif defined(PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
    return mapped;
}

static void unmap_virtual_memory(void *mapped, usize size)
{
    if (!mapped) return;
#if defined(PLATFORM_UNIX)
    s32 res = munmap(mapped, size);
    assert_debug(!res);
#elif defined(PLATFORM_WINDOWS)
    BOOL res = VirtualFreeEx(GetCurrentProcess(), (LPVOID)mapped, 0, MEM_RELEASE);
    assert_debug(res != FALSE)
#elif defined(PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
}

static b32 commit_physical_memory(void *mapped, usize page_offset, usize size)
{
    if (!mapped) return false;

    void *raw_map = (void *)(sptr)((u8 *)mapped + page_offset);
#if defined(PLATFORM_UNIX)
    s32 res = mprotect(raw_map, size, PROT_READ | PROT_WRITE);
    if (res != 0) {
        log_error("Commiting physical memory (mprotect) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s.", 
                  size, size >> 20, page_offset, page_offset >> 20, strerror(errno));
        return false;
    }
    res = madvise(raw_map, size, MADV_WILLNEED);
    if (res != 0) {
        log_error("Advising commitment of physical memory (madvise) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s.",
                  size, size >> 20, page_offset, page_offset >> 20, strerror(errno));
        return false;
    }
#elif defined(PLATFORM_WINDOWS)
    void *p = VirtualAllocEx(GetCurrentProcess(), raw_map, size, MEM_COMMIT, PAGE_READWRITE);
    if (!p) {
        log_error("Commiting physical memory (VirtualAlloc) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s", 
                size, size >> 20, page_offset, page_offset >> 20, GetLastError());
        return false;
    }
#elif defined(PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
#ifdef DEBUG
    log_debug("Commited %lu bytes (%lu MB) of physical memory at %lu offset (%lu MB).", 
              size, size >> 20, page_offset, page_offset >> 20);
#endif
    return true;
}

static b32 decommit_physical_memory(void *mapped, usize page_offset, usize size)
{
    if (!mapped) return false;

    void *raw_map = (void *)(sptr)((u8 *)mapped + page_offset);
#if defined(PLATFORM_UNIX)
    s32 res = madvise(raw_map, size, MADV_DONTNEED);
    if (res != 0) {
        log_error("Advising release of physical memory (madvise) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s.",
                  size, size >> 20, page_offset, page_offset >> 20, strerror(errno));
        return false;
    }
    res = mprotect(raw_map, size, PROT_NONE);
    if (res != 0) {
        log_error("Releasing physical memory (mprotect) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s.", 
                  size, size >> 20, page_offset, page_offset >> 20, strerror(errno));
    }
#elif defined(PLATFORM_WINDOWS)
    BOOL res = VirtualFreeEx(GetCurrentProcess(), raw_map, size, MEM_DECOMMIT);
    if (res == FALSE) {
        log_error("Releasing physical memory (VirtualFree) of %lu bytes (%lu MB) at %lu offset (%lu MB) failed: %s", 
                  size, size >> 20, page_offset, page_offset >> 20, GetLastError());
        return false;
    }
#elif defined(PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
#ifdef DEBUG
    log_debug("Released %lu bytes (%lu MB) of physical memory at %lu offset (%lu MB).", 
              size, size >> 20, page_offset, page_offset >> 20);
#endif
    return true;
}

/* thread local storage */
struct tls;

/** Fiber context. It holds saved CPU registers and pointers required for a context switch.
 *  All this data is CPU specific and managed on the stack by push/pop instructions. */
typedef void *fcontext_t;

/** Implemented in assembly, ofc/nfc - original/new fiber context, preserve_fpu - procedure. */
extern attr_hot sptr jump_fcontext(fcontext_t *ofc, fcontext_t nfc, sptr vp, s32 preserve_fpu);

/** Implemented in assembly, sp - top of stack pointer. */
extern attr_hot fcontext_t make_fcontext(void *sp, usize size, void (*fn)(sptr));

attr_inline sptr jump_fiber_context(
    struct tls *tls, 
    fcontext_t *from, 
    fcontext_t *to)
{
    assert_debug(from && to);
    assert_debug(*from != *to);
    return jump_fcontext(from, *to, (sptr)tls, 1);
}

attr_inline void make_fiber_context(
    fcontext_t *context, 
    void (*procedure)(sptr), 
    void *stack, 
    usize stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

/* an empty job for invalidating work */
void riven_work_nop(void *argument)
{
    (void)argument;
}

struct job {
    struct riven_work   work;
    at_usize           *work_left;
};

struct allocation {
    struct allocation  *prev;
    usize               offset;
    usize               size;
    usize               count;
};

union mpmc_data {
    struct job          job;         
    struct allocation  *allocation;
    u8                  padding[40];
};

static_assert(sizeof(union mpmc_data) == sizeof(((union mpmc_data *)NULL)->padding), "MPMC data padding not large enough");

struct mpmc_cell {
    union mpmc_data     data;
    at_usize            sequence;
};

struct mpmc_entry {
    struct mpmc_cell   *cell;
    usize               position;
};

/** Most CPUs have a 64 byte cacheline. PowerPC may have 128 byte cachelines, but it doesn't matter now. */
#define RIVEN_CACHELINE_SIZE (64)

/** Read and write operations on an atomic object are free from data races. However, if one thread 
 *  writes to it, all cache lines occupied by the object are invalidated. If another thread is reading
 *  from an unrelated object that shares the same cache line, it incures unnecesary overhead. This is 
 *  called false sharing, and we pad our mpmc ring buffer to avoid that. */
struct attr_aligned(RIVEN_CACHELINE_SIZE) 
mpmc_ring {
    attr_aligned(RIVEN_CACHELINE_SIZE)
    struct mpmc_cell   *buffer;
    usize               buffer_mask;
    u8                  pad0[RIVEN_CACHELINE_SIZE - sizeof(struct mpmc_cell *) - sizeof(usize)];

    attr_aligned(RIVEN_CACHELINE_SIZE)
    at_usize            enqueue_pos;
    u8                  pad1[RIVEN_CACHELINE_SIZE - sizeof(at_usize)];

    attr_aligned(RIVEN_CACHELINE_SIZE)
    at_usize            dequeue_pos;
    u8                  pad2[RIVEN_CACHELINE_SIZE - sizeof(at_usize)];
};

static void mpmc_ring_init(
    struct mpmc_ring *ring,
    struct mpmc_cell *cells,
    usize             cell_count)
{
    /* the cell count must be a power of 2 */
    assume((cell_count && !(cell_count & (cell_count - 1))));
    ring->buffer = cells;
    ring->buffer_mask = cell_count - 1; /* ((1 << log2_count) - 1) */
    for (usize i = 0; i < (ring->buffer_mask + 1); i++)
        atomic_store_explicit(&ring->buffer[i].sequence, i, memory_order_relaxed);
    atomic_store_explicit(&ring->enqueue_pos, 0lu, memory_order_relaxed);
    atomic_store_explicit(&ring->dequeue_pos, 0lu, memory_order_relaxed);
}

static struct mpmc_entry mpmc_ring_rotate(
    struct mpmc_ring *ring,
    at_usize         *in_or_out,
    const u32         pos_delta)
{
    struct mpmc_cell *cell;
    usize position = atomic_load_explicit(in_or_out, memory_order_relaxed);
    s32 spin = 0;

    for (;;) {
        cell = &ring->buffer[position & ring->buffer_mask];
        usize sequence = atomic_load_explicit(&cell->sequence, memory_order_acquire);
        sptr difference = (sptr)sequence - (sptr)(position + pos_delta);

        if (!difference) {
            if (atomic_compare_exchange_weak_explicit(in_or_out, &position, position + 1,
                memory_order_relaxed, memory_order_relaxed)) 
            {
                struct mpmc_entry result = { .cell = cell, .position = position };
                return result;
            }
        } else if (difference < 0) {
            /* it's empty */
            struct mpmc_entry result = { .cell = NULL, .position = 0 };
            return result;
        } else {
            spin++;
            if (spin > 128) {
                yield();
                spin = 0;
            }
            position = atomic_load_explicit(in_or_out, memory_order_relaxed);
        }
    }
}

static attr_hot 
b32 mpmc_ring_enqueue(
    struct mpmc_ring *ring,
    union mpmc_data  *data)
{
    struct mpmc_entry entry = mpmc_ring_rotate(ring, &ring->enqueue_pos, 0);
    if (entry.cell) {
        entry.cell->data = *data;
        atomic_store_explicit(&entry.cell->sequence, entry.position + 1, memory_order_release);
        return true;
    }
    return false;
}

static attr_hot 
b32 mpmc_ring_dequeue(
    struct mpmc_ring *ring,
    union mpmc_data  *data)
{
    struct mpmc_entry entry = mpmc_ring_rotate(ring, &ring->dequeue_pos, 1);
    if (entry.cell) { 
        *data = entry.cell->data;
        atomic_store_explicit(&entry.cell->sequence, entry.position + ring->buffer_mask + 1, memory_order_release);
        return true;
    }
    return false;
}

struct tagged_heap_arena {
    struct allocation  *head;
    struct allocation  *tail;
};

enum tls_flags {
    tls_in_use  = 0,
    tls_to_free = 0x40000000,
    tls_to_wait = 0x80000000,
    tls_mask    = ~(tls_to_free | tls_to_wait),
};

struct tls {
    struct riven               *riven;
    fcontext_t                  home_context;
    u32                         fiber_in_use;
    u32                         fiber_old;
    struct tagged_heap_arena    deferred[RIVEN_DEFERRED_HEAPS];
};

struct fiber {
    struct job                  job;
    fcontext_t                  context;
    at_usize                   *wait_counter;
};

struct tagged_heap {
    _Atomic riven_tag_t         tag;
    riven_chain_t               chain;
    struct tagged_heap_arena    arena;
};

struct heart_data {
    struct riven               *riven;
    PFN_riven_heart             procedure;
    void                       *argument;
    s32                         result;
};

struct riven {
    struct mpmc_ring            job_ring;
    struct mpmc_ring            allocation_ring;

    struct tls                 *tls;
    at_usize                    tls_sync;

    struct hash_table           table;
    thread_t                   *threads;
    struct riven_work          *ends;

    struct fiber               *fibers;
    at_usize                   *waiting;
    at_usize                   *free;
    at_usize                   *locks;

    struct tagged_heap          reserved_heaps[riven_tag_reserved_count];
    struct tagged_heap        **tagged_heaps;
    at_u32                      tagged_heap_tail;
    u32                         deferred_index;

    at_u8                      *bitmap;
    at_usize                    growth_sync;
    u8                         *stack;
    usize                       stack_size;

    usize                       memory_budget;
    usize                       page_size;
    at_usize                    commited_size;

    u32                         tagged_heap_count;
    u32                         thread_count;
    u32                         fiber_count;
};

static usize get_free_fiber(struct riven *riven)
{
    for (usize i = 0; i < riven->fiber_count; i++) {
        usize fiber_index = atomic_load_explicit(&riven->free[i], memory_order_relaxed);

        if (fiber_index == RIVEN_FIBER_INVALID) 
            continue;

        fiber_index = atomic_load_explicit(&riven->free[i], memory_order_acquire);
        if (fiber_index == RIVEN_FIBER_INVALID) 
            continue;

        usize expected = fiber_index;
        if (atomic_compare_exchange_weak_explicit(&riven->free[i], &expected, 
            RIVEN_FIBER_INVALID, memory_order_release, memory_order_relaxed))
        {
            return fiber_index;
        }
    }
    return RIVEN_FIBER_INVALID;
}

static void update_free_and_waiting(struct tls *tls)
{
    struct riven *riven = tls->riven;
    if (tls->fiber_old == (u32)RIVEN_FIBER_INVALID)
        return;

    const usize fiber_index = tls->fiber_old & tls_mask;

    /* a thread that added the fiber to the free list is the same as the one freeing it */
    if (tls->fiber_old & tls_to_free)
        atomic_store_explicit(&riven->free[fiber_index], fiber_index, memory_order_relaxed);

    /* wait threshold needs to be thread synced, hence the need for release */
    if (tls->fiber_old & tls_to_wait)
        atomic_store_explicit(&riven->waiting[fiber_index], fiber_index, memory_order_release);

    tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
}

static attr_noreturn 
void the_work(sptr raw_tls);

static usize seek_next_fiber(struct riven *riven)
{
    usize fiber_index = RIVEN_FIBER_INVALID;

    for (usize i = 0; i < riven->fiber_count; i++) {
        /* Double lock helps CPUs that have a weak memory model.
         * Arm should go 2x to 3x faster using double lock, if its the bottleneck. */
        usize fiber_waiting = atomic_load_explicit(&riven->waiting[i], memory_order_relaxed);
        if (fiber_waiting == RIVEN_FIBER_INVALID) 
            continue;

        fiber_waiting = atomic_load_explicit(&riven->waiting[i], memory_order_acquire);
        if (fiber_waiting == RIVEN_FIBER_INVALID) 
            continue;

        struct fiber *fiber = &riven->fibers[fiber_waiting];
        usize      finished = 1;
        at_usize   *counter = fiber->wait_counter;

        if (counter) {
            usize left = atomic_load_explicit(counter, memory_order_relaxed);
            finished = (!left);
        }
        if (!finished) continue;

        usize expected = fiber_waiting;
        if (atomic_compare_exchange_weak_explicit(&riven->waiting[i], &expected, 
            RIVEN_FIBER_INVALID, memory_order_release, memory_order_relaxed))
        {
            fiber_index = i;
            break;
        }
    }

    if (fiber_index == RIVEN_FIBER_INVALID) {
        union mpmc_data data;
        if (mpmc_ring_dequeue(&riven->job_ring, &data)) {

            while (fiber_index == RIVEN_FIBER_INVALID)
                fiber_index = get_free_fiber(riven);

            struct fiber *fiber = &riven->fibers[fiber_index];
            fiber->job = data.job;

            /* make_fcontext requires the top of the stack, because it grows down */
            u8 *stack = &riven->stack[(fiber_index * riven->stack_size) + riven->stack_size];
            make_fiber_context(&fiber->context, the_work, stack, riven->stack_size);
        }
    }
    return fiber_index;
}

struct tls *next_fiber(
    struct riven *riven,
    struct tls   *tls,
    fcontext_t   *context)
{
    at_usize *wait_counter = NULL;

    if ((tls->fiber_old != (u32)RIVEN_FIBER_INVALID) && (tls->fiber_old & tls_to_wait)) {
        const usize fiber_index = tls->fiber_old & tls_mask;
        struct fiber *fiber = &riven->fibers[fiber_index];
        wait_counter = fiber->wait_counter;
    }

    for (;;) {
        usize fiber_index = seek_next_fiber(riven);

        if (fiber_index != RIVEN_FIBER_INVALID) {
            struct fiber *fiber = &riven->fibers[fiber_index];
            tls->fiber_in_use = (u32)fiber_index;
            return (struct tls *)jump_fiber_context(tls, context, &fiber->context);
        }

        /* Race condition fix.
         *
         * Context needs to wait until a set of jobs are done. The jobs finish *before* 
         * a new context to swap to is found - there's no new jobs. The context swap code 
         * deadlocks looking for a new job to swap to, when no jobs may arrive. Meanwhile
         * the 'to be swapped' context is waiting to be run, but cannot as it hasn't been 
         * swapped out yet (in order to be picked up by the wait list). */
        if (wait_counter) {
            usize count = atomic_load_explicit(wait_counter, memory_order_relaxed);

            if (!count) {
                /* variable 'tls->fiber_in_use' still points to the 'to waitlist' fiber */
                tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
                return tls;
            }
        }
    }
}

attr_inline struct tls *get_thread_local_storage(struct riven *riven) {
    return &riven->tls[riven_thread_index(riven)];
}

static attr_noreturn 
void the_work(sptr raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    update_free_and_waiting(tls);
    struct fiber *fiber = &riven->fibers[tls->fiber_in_use];
    for (;;) {
        /* do the work */
        fiber->job.work.procedure(fiber->job.work.argument);
        if (fiber->job.work_left) {
            usize last = atomic_fetch_sub_explicit(fiber->job.work_left, 1lu, memory_order_relaxed);
            assert_debug(last > 0);

            /* reuse the fiber whenever viable */
            union mpmc_data next;
            if (last != 1 && mpmc_ring_dequeue(&riven->job_ring, &next)) {
                fiber->job = next.job;
                continue;
            }
        }

        /* yield the fiber */
        tls = get_thread_local_storage(riven);
        struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];
        tls->fiber_old = tls->fiber_in_use | tls_to_free;
        next_fiber(riven, tls, &old_fiber->context);
        UNREACHABLE;
    }
}

static at_usize *get_lock(
    struct riven *riven,
    usize         initial_value)
{
    for (;;) {
        for (usize i = 0; i < riven->fiber_count; i++) {
            at_usize *lock = &riven->locks[i];

            if (atomic_load_explicit(lock, memory_order_relaxed) == RIVEN_FIBER_INVALID) {
                usize expected = RIVEN_FIBER_INVALID;
                if (atomic_compare_exchange_weak_explicit(lock, &expected,
                    initial_value, memory_order_relaxed, memory_order_relaxed))
                {
                    return lock;
                }
            }
        }
    }
}

static void *dirty_deeds_done_dirt_cheap(void *raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    while (!atomic_load_explicit(&riven->tls_sync, memory_order_acquire)) { /* spin :3 */ }

    tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
    tls = next_fiber(riven, tls, &tls->home_context);

    update_free_and_waiting(tls);
    return NULL;
}

static attr_noreturn
void d4c_love_train(void *raw_riven)
{
    struct riven *riven = (struct riven *)raw_riven;
    struct tls *tls = get_thread_local_storage(riven);
    struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

    if (tls == &riven->tls[0]) {
        /* i == 1, don't join the main thread */
        for (u32 i = 1; i < riven->thread_count; i++) {
            thread_join(riven->threads[i]);
        }
    }
    tls->fiber_old = tls->fiber_in_use | tls_to_free;

    jump_fiber_context(tls, &old_fiber->context, &tls->home_context);
    UNREACHABLE;
}

static attr_noreturn
void heart(void *raw_heart_data)
{
    struct heart_data *data = (struct heart_data *)raw_heart_data;
    struct riven *riven = data->riven;
    const u32 thread_count = riven->thread_count;

    data->result = data->procedure(riven, riven->threads, riven->thread_count, data->argument);

    for (u32 i = 0; i < thread_count; i++) {
        riven->ends[i].procedure = d4c_love_train;
        riven->ends[i].argument = (void *)riven;
        riven->ends[i].name = str_init("riven:d4c_love_train");
    }
    riven_split_work_and_unchain(riven, riven->ends, thread_count);
    UNREACHABLE;
}

void riven_split_work(
    struct riven      *riven,
    struct riven_work *work,
    u32                work_count,
    riven_chain_t     *chain)
{
    at_usize **counters = chain;
    at_usize *to_use = NULL;

    if (!work_count) return;

    if (counters) {
        *counters = get_lock(riven, work_count);
        to_use = *counters;
    }

    for (u32 i = 0; i < work_count; i++) {
        union mpmc_data data = { .job = { .work = work[i], .work_left = to_use }};
        while (!mpmc_ring_enqueue(&riven->job_ring, &data)) {
            assert_release(!"Failed to enqueue work for fibers in the job queue.");
        }
    }
}

void riven_unchain(
    struct riven *riven,
    riven_chain_t chain)
{
    at_usize *counter = chain;
    usize wait_value = 0;

    if (counter) {
        wait_value = atomic_load_explicit(counter, memory_order_relaxed);
        assert_debug(wait_value != RIVEN_FIBER_INVALID);
    }

    if (wait_value) {
        struct tls *tls = get_thread_local_storage(riven);
        struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

        old_fiber->wait_counter = counter;
        tls->fiber_old = tls->fiber_in_use | tls_to_wait;
        tls = next_fiber(riven, tls, &old_fiber->context);
        update_free_and_waiting(tls);
    }
    if (counter) atomic_store_explicit(counter, RIVEN_FIBER_INVALID, memory_order_release);
}

void riven_acquire_chained(
    struct riven  *riven,
    riven_chain_t *chain)
{
    at_usize **counters = chain;
    if (counters) *counters = get_lock(riven, 1);
}

u32 riven_thread_index(struct riven *riven)
{
    s32 *index = NULL;
    thread_t current = thread_current();

    hash_table_find(&riven->table, &current, sizeof(thread_t), &index);
    return (u32)*index;
}

/** In the free bitmap, we represent 8 blocks with a single byte. In this way we 
 *  can either set the offset to the bitmap data, or calculate the relative range */
attr_inline attr_const 
usize index_from_position(const usize position) {
    return position >> 3lu; /* 8 bytes -> 1 byte */
}

/** Translate a bitmap index into a block position. */
attr_inline attr_const 
usize position_from_index(const usize index) {
    return index << 3lu;    /* 1 byte -> 8 bytes */
}

/** Get the block memory offset from the block position. */
attr_inline attr_const 
usize offset2mb_from_position(const usize pos) {
    return pos << 21lu;     /* 1 byte -> 2 MB */
}

/** Translate a per-block memory offset (aligned to 2MB) into a position index (aligned to 2mb). */
attr_inline attr_const
usize position_from_offset2mb(const ssize offset) {
    return offset >> 21lu;  /* 2 MB -> 1 byte */
}

/** Get the bitmap index from 8 (8x2mb) blocks (aligned to 16mb). */
attr_inline attr_const
usize index_from_offset16mb(const usize offset) {
    return position_from_offset2mb(offset); /* 16 MB -> 8 bytes */
}

/** We use this bitmap to represent our blocks of free memory. One bit in the bitmap 
 *  is one block of RIVEN_BLOCK_SIZE, so with one byte we represent 8 blocks. */
#define orchestrate_bitmap(func, operation)                                         \
    static attr_hot attr_nonnull_all attr_##func##_shared_capability(1)             \
    void func##_bitmap(                                                             \
        at_u8      *bitmap,                                                         \
        const usize position,                                                       \
        const usize size)                                                           \
{                                                                                   \
    if (size == 0) return;                                                          \
    const usize head_index = index_from_position(position);                         \
    const usize count      = index_from_offset16mb(size);                           \
    const usize tail_index = head_index + count - 1;                                \
                                                                                    \
    u8 bitmask = ~((1 << (position & 0x07)) - 1);                                   \
    for (usize i = 0; i < count; i++) {                                             \
        usize index = head_index + i;                                               \
                                                                                    \
        /* XOR'ing the bitmask will leave blocks beyond the allocation untouched */ \
        if (index == tail_index)                                                    \
            bitmask ^= ~((1 << ((position + count) & 0x07)) - 1);                   \
                                                                                    \
        /* set the blocks as in use (bits set to 0) or as free (bits set to 1) */   \
        operation;                                                                  \
        bitmask = 0xFF;                                                             \
    }                                                                               \
}

orchestrate_bitmap(acquire, atomic_fetch_and_explicit(&bitmap[index], ~bitmask, memory_order_acquire));
orchestrate_bitmap(release, atomic_fetch_or_explicit(&bitmap[index], bitmask, memory_order_release));

static attr_nonnull(1)
usize find_first_set(
    at_u8      *bitmap,
    const usize position,
    const usize ceiling)
{
    if (position >= ceiling) return 0lu;

    const usize index = index_from_position(position);
    const usize count = 1 + index_from_offset16mb(ceiling) - index;
    const u8 *raw = (const u8 *)bitmap;

    for (;;) {
        usize bits = aria_bits_ffs(&raw[index], count);

        /* bits_ffs returns a 1-based value, or 0 if no bits are set
         * this is also why we must decrement the bits value by one */
        if (!bits) return 0lu;
        /* the position_from_index will align the position from argument to a byte boundary */
        bits += position_from_index(index) - 1;

        /* we may have overshot on edging (!!) bytes */
        if (UNLIKELY(bits >= position_from_offset2mb(ceiling)))
            return 0lu;

        /* acquire the bit representing our block */
        const u8 bitmask = (1 << (bits & 0x07));
        const u8 prev = atomic_fetch_and_explicit(&bitmap[index_from_position(bits)], ~bitmask, memory_order_acquire);

        /* double check to avoid a possibility of data races */
        if ((prev & bitmask) == bitmask) 
            return offset2mb_from_position(bits);
    }
    UNREACHABLE;
}

static attr_nonnull(1)
usize best_fit_growth(
    at_u8      *bitmap,
    const usize position,
    const usize size,
    const usize ceiling,
    at_usize   *sync)
{
    const usize pages = position_from_offset2mb(size);
    const usize head_index = index_from_position(position);
    const usize tail_index = index_from_offset16mb(ceiling);

    if (head_index > tail_index || !pages || pages < position_from_offset2mb(ceiling) - position) 
        return 0lu;

    usize candidate = 0lu;
    usize current = 0lu;

    /* scan through the bitmap */
    for (usize i = head_index; i <= tail_index; i++) {
        const u8 byte = atomic_load_explicit(&bitmap[i], memory_order_relaxed);

        /* a full free byte is convenient */
        if (byte == 0xFF) {
            if (current == 0lu) {
                candidate = position_from_index(i);
                atomic_store_explicit(sync, offset2mb_from_position(candidate), memory_order_relaxed);
            }
            current += 8;
        } else if (byte != 0) {
            u8 bitmask = 0xFF;
            usize bit = 0;

            /* check individual bits */
            while (bitmask && bit < 8) {
                if (byte & bitmask) {
                    if (current == 0lu) {
                        candidate = position_from_index(i) + bit;
                        atomic_store_explicit(sync, offset2mb_from_position(candidate), memory_order_relaxed);
                    }
                    current++;
                    bitmask >>= 1;
                    bit++;
                } else {
                    atomic_store_explicit(sync, offset2mb_from_position(position_from_index(i)), memory_order_relaxed);
                    current = 0lu;
                    bitmask = 0lu;
                }
            } 
        } else {
            atomic_store_explicit(sync, offset2mb_from_position(position_from_index(i)), memory_order_relaxed);
            current = 0lu;
        }
        /* git gud */
        if (current >= pages) 
            return offset2mb_from_position(candidate);
    }
    return 0lu;
}

usize riven_advise(
    struct riven *riven,
    usize         size,
    b32           release)
{
    if (!size) return 0lu;

    const usize page_aligned = (size + riven->page_size-1) & ~(riven->page_size-1);
    const usize roots = position_from_offset2mb(riven->reserved_heaps[riven_tag_roots].arena.head->size);

    for (;;) {
        usize expected = 0lu;
        if (!atomic_compare_exchange_weak_explicit(&riven->growth_sync, &expected, 
            roots, memory_order_release, memory_order_relaxed)) 
        {
            continue;
        };
        const usize commited = atomic_load_explicit(&riven->commited_size, memory_order_relaxed);

        if (release) {
            /* if possible, release unused resources */
            const usize offset = commited - page_aligned;
            const usize index = index_from_offset16mb(offset);
            const usize count = index_from_offset16mb(page_aligned);

            const usize bits = (count+1) * 8    /* full bytes of free blocks */
                - (index & 0x07)                /* trim head index bits out of range */
                - ((index + count) & 0x07);     /* trim tail index bits out of range */
            const usize popcnt = aria_bits_popcnt((const u8 *)riven->bitmap + index, count);

            b32 res = false;
            if (popcnt >= bits)
                res = decommit_physical_memory((void *)riven, offset, page_aligned);
            atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
            if (res) {
                atomic_store_explicit(&riven->commited_size, offset, memory_order_release);
                return offset;
            }
            return 0lu;
        }

        usize offset = best_fit_growth(riven->bitmap, roots, page_aligned, commited, &riven->growth_sync);
        if (offset) {
            atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
            return offset;
        }
        atomic_store_explicit(&riven->growth_sync, commited, memory_order_release);

        /* we may want to commit memory for new resources */
        b32 res = commit_physical_memory((void *)riven, commited, page_aligned);
        atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
        if (res) {
            atomic_store_explicit(&riven->commited_size, commited + page_aligned, memory_order_release);
            return commited;
        }
        return 0lu;
    }
    UNREACHABLE;
}

static attr_nonnull(1)
struct allocation *reserve_allocation(
    struct riven *riven,
    usize         size)
{
    struct allocation *write;
    union mpmc_data    data;

    mpmc_ring_dequeue(&riven->allocation_ring, &data);
    write = data.allocation;

    write->prev = NULL;
    write->count = 0;

    const usize page_aligned = A2MB(size);
    const usize roots = riven->reserved_heaps[riven_tag_roots].arena.head->size;
    usize commited = atomic_load_explicit(&riven->commited_size, memory_order_relaxed);

    for (;;) {
        usize sync = atomic_load_explicit(&riven->growth_sync, memory_order_acquire);

        /* We can simplify the allocation process if we only need to acquire a single block of resources.
         * Then it's enough to lookup just one single bit that is free, flip it immediately and avoid any
         * locks in this process. Acquiring larger resources will complicate things tho. */
        if (size <= RIVEN_BLOCK_SIZE) {
            write->offset = find_first_set(riven->bitmap, position_from_offset2mb(roots), sync ? min(sync, commited) : commited);
            if (write->offset) {
                write->size = RIVEN_BLOCK_SIZE;
                return write;
            }
            if (sync) continue;
        }

        usize expected = 0lu;
        if (!atomic_compare_exchange_weak_explicit(&riven->growth_sync, &expected, 
            position_from_offset2mb(roots), memory_order_relaxed, memory_order_relaxed)) 
        {
            continue;
        };
        /* update the commited value, in case it changed */
        commited = atomic_load_explicit(&riven->commited_size, memory_order_acquire);

        usize offset = 0;
        /* If we're here for a larger allocation, now we can safely work on that. This step 
         * will be skipped in case we came here from the implication where there is not enough 
         * free commited memory right now to satisfy a minimal allocation (RIVEN_BLOCK_SIZE). */
        if (size > RIVEN_BLOCK_SIZE) {
            /* when it's non-zero value, we can satisfy the allocation from existing resources */
            offset = best_fit_growth(riven->bitmap, position_from_offset2mb(roots), page_aligned, commited, &riven->growth_sync);
            if (offset && offset + page_aligned <= commited) {
                acquire_bitmap(riven->bitmap, position_from_offset2mb(offset), page_aligned);
                atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
                write->offset = offset;
                write->size = page_aligned;
                return write;
            }
        }
        if (!offset) offset = commited;
        atomic_store_explicit(&riven->growth_sync, offset, memory_order_release);
        usize new_ceiling = page_aligned - (commited-offset);

        /* we need to commit memory for new resources */
        if (new_ceiling + commited > riven->memory_budget || !commit_physical_memory((void *)riven, commited, new_ceiling)) {
            /* we couldn't reserve resources, so yield the allocation */
            mpmc_ring_dequeue(&riven->allocation_ring, &data);
            atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
            return NULL;
        }
        acquire_bitmap(riven->bitmap, position_from_offset2mb(offset), page_aligned);
        atomic_store_explicit(&riven->commited_size, new_ceiling + commited, memory_order_release);
        atomic_store_explicit(&riven->growth_sync, 0lu, memory_order_release);
        write->offset = offset;
        write->size = page_aligned;
        return write;
    }
    UNREACHABLE;
}

static attr_nonnull(1,2)
void *acquire_tagged_resources(
    struct riven               *riven,
    struct tagged_heap_arena   *arena,
    usize                       size,
    usize                       alignment)
{
    u8 *raw = (u8 *)riven;

    if (size == 0) {
        return NULL;
    } else if (!arena->head) {
        arena->tail = arena->head = reserve_allocation(riven, size);
        if (!arena->head) {
            log_error("Host memory failure, not enough resources to reserve %lu bytes (%lu MB) of memory.", A2MB(size), A2MB(size)>>20);
            return NULL;
        }
        arena->head->count = size;
        return (void *)(sptr)(raw + arena->head->offset);
    }
    assert_debug(arena->tail);

    /* we can assume that there is no more free space than a single block within an existing allocation */
    if (size <= RIVEN_BLOCK_SIZE) {
        for (struct allocation *write = arena->tail; write; write = write->prev) {
            usize aligned = (write->count + (alignment - 1)) & ~(alignment - 1);
            if (aligned + size <= write->size) {
                write->count = aligned + size;
                return (void *)(sptr)(raw + write->offset + aligned);
            }
        }
    }

    /* we'll end up here if we still couldn't satisfy the allocation */
    struct allocation *next = reserve_allocation(riven, size);
    if (!next) {
        log_error("Host memory failure, not enough resources to reserve %lu bytes (%lu MB) of memory.", A2MB(size), A2MB(size)>>20);
        return NULL;
    }
    next->prev = arena->tail;
    next->count = size;
    arena->tail = next;
    return (void *)(sptr)(raw + next->offset);
}

attr_inline attr_nonnull(1,2)
void *chained_allocation(
    struct riven       *riven,
    struct tagged_heap *heap,
    usize               size,
    usize               alignment)
{
    riven_unchain(riven, heap->chain);
    riven_acquire_chained(riven, &heap->chain);
    void *result = acquire_tagged_resources(riven, &heap->arena, size, alignment);
    riven_release_chained(heap->chain);
    return result;
}

void *riven_alloc(
    struct riven *riven,
    riven_tag_t   tag,
    usize         size,
    usize         alignment)
{
    alignment = min(alignment, RIVEN_BLOCK_SIZE);

    assert_debug(!(alignment & (alignment - 1)));

    if (tag < riven_tag_reserved_count) {
        struct tagged_heap *heap = &riven->reserved_heaps[tag];
        return chained_allocation(riven, heap, size, alignment);
    } else if (tag == riven_tag_deferred) {
        struct tls *tls = get_thread_local_storage(riven);
        struct tagged_heap_arena *arena = &tls->deferred[riven->deferred_index];
        return acquire_tagged_resources(riven, arena, size, alignment);
    }

    usize tail = atomic_load_explicit(&riven->tagged_heap_tail, memory_order_acquire);
    for (u32 i = 0; i < tail; i++) {
        struct tagged_heap *heap = riven->tagged_heaps[i];
        riven_tag_t owner = atomic_load_explicit(&heap->tag, memory_order_relaxed);
        if (owner == tag)
            return chained_allocation(riven, heap, size, alignment);
    }

    for (;;) {
        if (tail >= riven->tagged_heap_count) {
            log_error("Reached the maximum count of unique tagged heaps (%lu),"
                      "can't perform allocation for tag: %X.", tail, tag);
            return NULL;
        }

        riven_tag_t expected = riven_tag_deferred;
        struct tagged_heap *heap = riven->tagged_heaps[tail];
        if (atomic_compare_exchange_weak_explicit(&heap->tag, &expected, tag,
            memory_order_acquire, memory_order_relaxed))
        {
            atomic_fetch_add_explicit(&riven->tagged_heap_tail, 1lu, memory_order_release);
            return chained_allocation(riven, heap, size, alignment);
        }
        tail = atomic_load_explicit(&riven->tagged_heap_tail, memory_order_acquire);
    }
    UNREACHABLE;
}

static attr_nonnull(1,2)
void release_tagged_resources(
    struct riven             *riven,
    struct tagged_heap_arena *arena)
{
    struct allocation *prev = NULL;
    for (struct allocation *release = arena->tail; release != NULL; release = prev) {
        prev = release->prev;

        release_bitmap(riven->bitmap, position_from_offset2mb(release->offset), release->size);
        *release = (struct allocation){0};

        union mpmc_data data = { .allocation = release };
        mpmc_ring_enqueue(&riven->allocation_ring, &data);
    }
    arena->tail = arena->head = NULL;
}

void riven_free(
    struct riven *riven,
    riven_tag_t   tag)
{
    assert_debug(tag != riven_tag_roots);

    if (tag < riven_tag_reserved_count) {
        struct tagged_heap *heap = &riven->reserved_heaps[tag];
        if (heap->arena.head) 
            release_tagged_resources(riven, &heap->arena);
        heap->chain = NULL;
        return;
    } else if (tag == riven_tag_deferred) {
        /* For free, we want to clean the current deferred_index of tagged heaps, 
         * without incrementing and cycling the index. */
        for (u32 i = 0; i < riven->thread_count; i++) {
            struct tls *tls = get_thread_local_storage(riven);
            struct tagged_heap_arena *arena = &tls->deferred[riven->deferred_index];
            if (arena->head) release_tagged_resources(riven, arena);
        }
        return;
    }

    for (usize i = 0; i < atomic_load_explicit(&riven->tagged_heap_tail, memory_order_relaxed); i++) {
        riven_tag_t expected = tag;
        struct tagged_heap *heap = riven->tagged_heaps[i];

        if (atomic_compare_exchange_weak_explicit(&heap->tag, &expected, 
            riven_tag_deferred, memory_order_release, memory_order_relaxed))
        {
            usize tail = atomic_fetch_sub_explicit(&riven->tagged_heap_tail, 1lu, memory_order_acquire);
            riven->tagged_heaps[i] = riven->tagged_heaps[tail];
            riven->tagged_heaps[tail] = heap;
            release_tagged_resources(riven, &heap->arena);
            heap->chain = NULL;
            return;
        }
    }
}

void riven_rotate_deferred(struct riven *riven)
{
    riven->deferred_index = (riven->deferred_index + 1) % RIVEN_DEFERRED_HEAPS;
    for (u32 i = 0; i < riven->thread_count; i++) {
        /* we release resources of the current deferred index */
        struct tagged_heap_arena *arena = &riven->tls[i].deferred[riven->deferred_index];
        if (arena->head) release_tagged_resources(riven, arena);
    }
}

void riven_format_string(
    struct riven      *riven,
    riven_tag_t        tag,
    struct str * const dest,
    const char        *fmt, ...)
{
    /* calculate formated length */
    va_list args;
    va_start(args, fmt);
    s32 n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    assert_debug(n >= 0);

    /* format the string */
    char *p = (char *)riven_alloc(riven, tag, n + 1, 1);
    va_start(args, fmt);
    vsnprintf(p, n + 1, fmt, args);
    va_end(args);

    p[n] = '\0';
    *dest = (struct str){ p, n };
}

void riven_concatenate_strings(
    struct riven      *riven,
    riven_tag_t        tag,
    struct str * const dest,
    const struct str  *src,
    usize              count)
{
    str_clear(dest);

    if (!src) return;

    /* calculate total length */
    usize sum = 0;
    for (usize o = 0; o < count; o++)
        sum += src[o].length;
    if (sum == 0) return;

    char * const buf = (char *)riven_alloc(riven, tag, sum + 1, 1);

    char *p = buf;
    for (usize o = 0; o < count; o++)
        p = (char *)(memcpy(p, src[o].ptr, src[o].length) + src[o].length);

    *p = '\0';
    dest->ptr = buf;
    dest->length = sum;
}

s32 riven_moonlit_walk(
    usize           memory_budget_size,
    usize           fiber_stack_size,
    u32             fiber_count,
    u32             thread_count,
    u32             tagged_heap_count,
    u32             log2_work_count,
    u32             log2_memory_count,
    PFN_riven_heart main_procedure,
    void           *main_argument)
{
    usize ram_size, page_size, huge_page_size = 0;
    process_meminfo(&ram_size, &page_size);
    process_hugetlbinfo(&huge_page_size, A16MB(RIVEN_BLOCK_SIZE));

    u32 cpu_count = 0;
    process_cpuinfo((s32 *)&cpu_count, NULL, NULL);

    /* provide defaults */
#if RIVEN_USE_HUGEPAGES
    if (huge_page_size < 4096)
        huge_page_size = page_size;
#else
    huge_page_size = page_size;
#endif
    if (thread_count == 0 || thread_count > cpu_count)
        thread_count = cpu_count;
    if (fiber_count == 0)
        fiber_count = 96 + 4*thread_count;
    if (tagged_heap_count == 0)
        tagged_heap_count = 16;
    if (memory_budget_size == 0)
        memory_budget_size = ((ram_size) & ~(RIVEN_BLOCK_SIZE-1));
    memory_budget_size = A2MB(memory_budget_size);

    struct riven *riven = NULL;

    struct mpmc_cell  *work_cells = NULL;
    struct mpmc_cell  *allocation_cells = NULL;
    struct allocation *allocations = NULL;

    usize riven_bytes               = A16(sizeof(struct riven));
    usize work_count                = 1ull << log2_work_count;
    usize work_cells_bytes          = A16(sizeof(struct mpmc_cell) * work_count);
    usize allocation_count          = 1ull << log2_memory_count;
    usize allocation_cells_bytes    = A16(sizeof(struct mpmc_cell) * allocation_count);
    usize allocations_bytes         = A16(sizeof(struct allocation) * allocation_count);

    usize tls_bytes                 = A16(sizeof(struct tls)) * thread_count;
    usize thread_bytes              = A16(sizeof(thread_t) * thread_count);
    usize ends_bytes                = A16(sizeof(struct riven_work) * thread_count);
    usize fibers_bytes              = A16(sizeof(struct fiber) * fiber_count);
    usize waiting_bytes             = A16(sizeof(at_usize) * fiber_count);
    usize free_bytes                = A16(sizeof(at_usize) * fiber_count);
    usize locks_bytes               = A16(sizeof(at_usize) * fiber_count);

    usize tagged_array_bytes        = A16(sizeof(struct tagged_heap *) * tagged_heap_count);
    usize tagged_heap_bytes         = A16(sizeof(struct tagged_heap) * tagged_heap_count);
    usize pages_count               = position_from_offset2mb(memory_budget_size);
    usize bitmap_bytes              = A16(index_from_position(pages_count));
    usize stack_bytes               = A16(fiber_stack_size);
    usize stack_heap_bytes          = stack_bytes * fiber_count;

    usize total_bytes = 
        riven_bytes +
        work_cells_bytes +
        allocation_cells_bytes +
        allocations_bytes +
        tls_bytes +
        thread_bytes +
        ends_bytes +
        fibers_bytes +
        waiting_bytes +
        free_bytes +
        locks_bytes +
        tagged_array_bytes +
        tagged_heap_bytes +
        bitmap_bytes +
        stack_heap_bytes;

    usize roots_allocation_size = A2MB(total_bytes);
    usize init_commit_size = min(A16MB(roots_allocation_size<<1), memory_budget_size);

    /* address to riven is offset == 0 */
    riven = (struct riven *)map_virtual_memory(memory_budget_size, huge_page_size);
    if (!riven || !commit_physical_memory(riven, 0, init_commit_size)) return result_error;
    memset(riven, 0u, init_commit_size);

    usize o = riven_bytes;
    u8 *raw = (u8 *)riven;

    work_cells          = (struct mpmc_cell *)          &raw[o]; o += work_cells_bytes;
    allocation_cells    = (struct mpmc_cell *)          &raw[o]; o += allocation_cells_bytes;
    allocations         = (struct allocation *)         &raw[o]; o += allocations_bytes;
    riven->tls          = (struct tls *)                &raw[o]; o += tls_bytes;
    riven->threads      = (thread_t *)                  &raw[o]; o += thread_bytes;
    riven->ends         = (struct riven_work *)         &raw[o]; o += ends_bytes;
    riven->fibers       = (struct fiber *)              &raw[o]; o += fibers_bytes;
    riven->waiting      = (at_usize *)                  &raw[o]; o += waiting_bytes;
    riven->free         = (at_usize *)                  &raw[o]; o += free_bytes;
    riven->locks        = (at_usize *)                  &raw[o]; o += locks_bytes;
    for (u32 i = 0; i < riven_tag_reserved_count; i++)
        atomic_store_explicit(&riven->reserved_heaps[i].tag, i, memory_order_relaxed);
    riven->tagged_heaps = (struct tagged_heap **)       &raw[o]; o += tagged_array_bytes;
    for (u32 i = 0; i < tagged_heap_count; i++) {
        riven->tagged_heaps[i] = (struct tagged_heap *) &raw[o]; o += sizeof(struct tagged_heap);
        atomic_store_explicit(&riven->tagged_heaps[i]->tag, riven_tag_deferred, memory_order_relaxed);
    }
    riven->bitmap       = (at_u8 *)                     &raw[o]; o += bitmap_bytes;
    riven->stack        = (u8 *)                        &raw[o];

    /* bits set to 1 indicate free memory blocks */
    release_bitmap(riven->bitmap, position_from_offset2mb(roots_allocation_size), memory_budget_size);

    riven->stack_size = stack_bytes;
    riven->memory_budget = memory_budget_size;
    riven->page_size = huge_page_size;
    atomic_store_explicit(&riven->commited_size, init_commit_size, memory_order_relaxed);
    riven->tagged_heap_count = tagged_heap_count;
    riven->thread_count = thread_count;
    riven->fiber_count = fiber_count;

    assert_debug(!(((sptr)work_cells)           & 15))
    assert_debug(!(((sptr)allocation_cells)     & 15))
    assert_debug(!(((sptr)allocations)          & 15))
    assert_debug(!(((sptr)riven->tls)           & 15))
    assert_debug(!(((sptr)riven->threads)       & 15))
    assert_debug(!(((sptr)riven->ends)          & 15))
    assert_debug(!(((sptr)riven->fibers)        & 15))
    assert_debug(!(((sptr)riven->waiting)       & 15))
    assert_debug(!(((sptr)riven->free)          & 15))
    assert_debug(!(((sptr)riven->locks)         & 15))
    assert_debug(!(((sptr)riven->tagged_heaps)  & 15))
    assert_debug(!(((sptr)riven->bitmap)        & 15))
    assert_debug(!(((sptr)riven->stack)         & 15))

    /* setup the ring buffers */
    mpmc_ring_init(&riven->job_ring, work_cells, 1lu << log2_work_count);
    mpmc_ring_init(&riven->allocation_ring, allocation_cells, 1lu << log2_memory_count);
    for (usize i = 0; i < (1lu << log2_memory_count); i++) {
        union mpmc_data data = { .allocation = &allocations[i] };
        b32 res = mpmc_ring_enqueue(&riven->allocation_ring, &data);
        assert_debug(res && "Failed to enqueue allocation structures to the ring buffer.");
    }

    /* setup fibers */
    for (u32 i = 0; i < fiber_count; i++) {
        atomic_store_explicit(&riven->free[i], i, memory_order_relaxed);
        atomic_store_explicit(&riven->waiting[i], RIVEN_FIBER_INVALID, memory_order_relaxed);
        atomic_store_explicit(&riven->locks[i], RIVEN_FIBER_INVALID, memory_order_relaxed);
    }

    /* setup the roots allocation */
    union mpmc_data roots = {0};
    mpmc_ring_dequeue(&riven->allocation_ring, &roots);
    riven->reserved_heaps[riven_tag_roots].arena.tail = riven->reserved_heaps[riven_tag_roots].arena.head = roots.allocation;
    riven->reserved_heaps[riven_tag_roots].arena.head->size = roots_allocation_size;
    riven->reserved_heaps[riven_tag_roots].arena.head->count = total_bytes;
    /* now we can safely call riven_alloc() with riven_tag_roots */
    hash_table_init(&riven->table, riven, riven_tag_roots, aria_log2_next_pow2(thread_count));

    riven->tls[0].riven = riven;
    riven->tls[0].fiber_in_use = (u32)get_free_fiber(riven);
    riven->threads[0] = thread_current();
    hash_table_insert(&riven->table, &riven->threads[0], sizeof(thread_t), 0);

    /* setup threads and thread-local storage */
    atomic_store_explicit(&riven->tls_sync, (usize)0, memory_order_release);
    for (u32 i = 1; i < thread_count; i++) {
        struct tls *tls = &riven->tls[i];
        tls->riven = riven;
        tls->fiber_in_use = (u32)RIVEN_FIBER_INVALID;
        thread_create(&riven->threads[i], dirty_deeds_done_dirt_cheap, (void *)tls);
        /* hash the thread handle */
        hash_table_insert(&riven->table, (void *)&riven->threads[i], sizeof(thread_t), i);
    }
    thread_affinity(riven->stack, riven->threads, thread_count, cpu_count, 0);
    atomic_store_explicit(&riven->tls_sync, (usize)1, memory_order_release);

    /* enter main */
    struct heart_data main_data = {
        .riven = riven,
        .procedure = main_procedure,
        .argument = main_argument,
        .result = result_success,
    };
    struct riven_work main_work = {
        .procedure = heart,
        .argument = (void *)&main_data,
        .name = str_init("riven:heart"),
    };

    riven_split_work(riven, &main_work, 1, NULL);
    dirty_deeds_done_dirt_cheap((void *)&riven->tls[0]);
    unmap_virtual_memory(riven, riven->memory_budget);

    return main_data.result;
}
