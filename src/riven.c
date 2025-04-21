#include <amwe/riven.h>
#include <amwe/hash_table.h>
#include <amwe/math/bits.h>

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

#define RIVEN_FIBER_INVALID (SIZE_MAX)

bedrock_static_assert(lake_sizeof(sptr) == lake_sizeof(struct riven *), "intptr can't hold pointers");
bedrock_static_assert(lake_sizeof(usize) >= lake_sizeof(void *), "size word can't hold pointers");
bedrock_static_assert(((RIVEN_BLOCK_SIZE != 0) && ((RIVEN_BLOCK_SIZE & (RIVEN_BLOCK_SIZE-1)) == 0)), "block size must be a power of 2");

/** Declares a job, holding work info */
struct job {
    struct riven_work       work;       /**< Defines the job context. */
    atomic_usize           *work_left;  /**< An atomic counter assigned to a bound chain. */
};

struct job_cell {
    struct job              job;        /**< Holds the job structure directly, instead of a pointer. */
    atomic_usize            sequence;
};

/** Declares an arena with an allocation range. */
struct arena {
    struct arena           *prev;       /**< Previous arena in a linked-list. */
    usize                   offset;     /**< The offset in mapped virtual memory. */
    usize                   capacity;   /**< The size of this allocation, */
    usize                   size;       /**< Bytes in use within this allocation, linear growth. */
};

struct arena_cell {
    struct arena           *arena;      /**< Holds pointers so we can process arenas and cells seperately. */
    atomic_usize            sequence;
};

/** Data used within the MPMC ring buffer. */
struct mpmc_entry {
    usize                   position;
    union {
        struct job_cell    *job;
        struct arena_cell  *arena;
    } cell;
};

#define MPMC_RING_TEMPLATE_REFERENCED(type)     MPMC_RING_TEMPLATE(type, *, )
#define MPMC_RING_TEMPLATE_DEREFERENCED(type)   MPMC_RING_TEMPLATE(type, , *)
/** Read and write operations on an atomic object are free from data races. However, if one thread 
 *  writes to it, all cache lines occupied by the object are invalidated. If another thread is reading
 *  from an unrelated object that shares the same cache line, it incures unnecesary overhead. This is 
 *  called false sharing, and we pad our mpmc ring buffer to avoid that. */
#define MPMC_RING_TEMPLATE(type, in_access, out_access)                                             \
    struct lake_alignment(LAKE_CACHELINE_SIZE)                                                      \
    type##_ring {                                                                                   \
        struct type##_cell *buffer;                                                                 \
        usize               buffer_mask;                                                            \
        u8 pad0[LAKE_CACHELINE_SIZE - sizeof(struct type##_cell *) - sizeof(usize)];                \
                                                                                                    \
        lake_alignment(LAKE_CACHELINE_SIZE)                                                         \
        atomic_usize        enqueue_pos;                                                            \
        u8                  pad1[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];                       \
                                                                                                    \
        lake_alignment(LAKE_CACHELINE_SIZE)                                                         \
        atomic_usize        dequeue_pos;                                                            \
        u8                  pad2[LAKE_CACHELINE_SIZE - sizeof(atomic_usize)];                       \
    };                                                                                              \
                                                                                                    \
    static lake_hot void                                                                            \
    type##_ring_rotate(                                                                             \
        struct type##_ring *ring,                                                                   \
        atomic_usize       *in_or_out,                                                              \
        const u32           pos_delta,                                                              \
        struct mpmc_entry  *output)                                                                 \
    {                                                                                               \
        struct type##_cell *cell;                                                                   \
        usize position = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);           \
                                                                                                    \
        for (;;) {                                                                                  \
            cell = &ring->buffer[position & ring->buffer_mask];                                     \
            usize sequence = lake_atomic_read_explicit(&cell->sequence, lake_memory_model_acquire); \
            sptr difference = (sptr)sequence - (sptr)(position + pos_delta);                        \
                                                                                                    \
            if (!difference) {                                                                      \
                if (lake_atomic_compare_exchange_weak_explicit(in_or_out, &position, position + 1,  \
                    lake_memory_model_relaxed, lake_memory_model_relaxed))                          \
                {                                                                                   \
                    *output = (struct mpmc_entry){ .cell.type = cell, .position = position };       \
                    return;                                                                         \
                }                                                                                   \
            } else if (difference < 0) {                                                            \
                /* it's empty */                                                                    \
                *output = (struct mpmc_entry){ .cell.type = NULL, .position = 0 };                  \
                return;                                                                             \
            } else {                                                                                \
                position = lake_atomic_read_explicit(in_or_out, lake_memory_model_relaxed);         \
            }                                                                                       \
        }                                                                                           \
        lake_unreachable;                                                                           \
    }                                                                                               \
                                                                                                    \
    static lake_hot bool                                                                            \
    type##_ring_enqueue(                                                                            \
        struct type##_ring *ring,                                                                   \
        struct type        *data)                                                                   \
    {                                                                                               \
        struct mpmc_entry entry;                                                                    \
        type##_ring_rotate(ring, &ring->enqueue_pos, 0, &entry);                                    \
        if (entry.cell.type) {                                                                      \
            entry.cell.type->type = in_access data;                                                 \
            lake_atomic_store_explicit(&entry.cell.type->sequence,                                  \
                                        entry.position + 1,                                         \
                                        lake_memory_model_release);                                 \
            return true;                                                                            \
        }                                                                                           \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    static lake_hot bool                                                                            \
    type##_ring_dequeue(                                                                            \
        struct type##_ring *ring,                                                                   \
        struct type out_access *out_data)                                                           \
    {                                                                                               \
        struct mpmc_entry entry;                                                                    \
        type##_ring_rotate(ring, &ring->dequeue_pos, 1, &entry);                                    \
        if (entry.cell.type) {                                                                      \
            *out_data = entry.cell.type->type;                                                      \
            lake_atomic_store_explicit(&entry.cell.type->sequence,                                  \
                                        entry.position + ring->buffer_mask + 1,                     \
                                        lake_memory_model_release);                                 \
            return true;                                                                            \
        }                                                                                           \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    static void                                                                                     \
    type##_ring_init(                                                                               \
        struct type##_ring *ring,                                                                   \
        struct type##_cell *cells,                                                                  \
        usize               cell_count)                                                             \
    {                                                                                               \
        /* the cell count must be a power of 2 */                                                   \
        lake_assume((cell_count && !(cell_count & (cell_count - 1))));                              \
                                                                                                    \
        ring->buffer = cells;                                                                       \
        ring->buffer_mask = cell_count - 1; /* ((1 << log2_count) - 1) */                           \
                                                                                                    \
        for (usize i = 0; i < (ring->buffer_mask + 1); i++)                                         \
            lake_atomic_store_explicit(&ring->buffer[i].sequence, i, lake_memory_model_relaxed);    \
        lake_atomic_store_explicit(&ring->enqueue_pos, 0lu, lake_memory_model_relaxed);             \
        lake_atomic_store_explicit(&ring->dequeue_pos, 0lu, lake_memory_model_relaxed);             \
    }
MPMC_RING_TEMPLATE_REFERENCED(job)
MPMC_RING_TEMPLATE_DEREFERENCED(arena)

/* thread local storage */
struct tls;

/** Fiber context. It holds saved CPU registers and pointers required for a context switch.
 *  All this data is CPU specific and managed on the stack by push/pop instructions. */
typedef void *fcontext_t;

/** Implemented in assembly, ofc/nfc - original/new fiber context, preserve_fpu - procedure. */
extern lake_hot sptr jump_fcontext(fcontext_t *ofc, fcontext_t nfc, sptr vp, s32 preserve_fpu);

/** Implemented in assembly, sp - top of stack pointer. */
extern lake_hot fcontext_t make_fcontext(void *sp, usize size, void (*fn)(sptr));

lake_force_inline lake_nonnull_all
sptr jump_fiber_context(
    struct tls *tls, 
    fcontext_t *from, 
    fcontext_t *to)
{
    bedrock_assert_debug(*from != *to);
    return jump_fcontext(from, *to, (sptr)tls, 1);
}

lake_force_inline lake_nonnull_all
void make_fiber_context(
    fcontext_t *context, 
    void        (*procedure)(sptr), 
    void       *stack, 
    usize       stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

struct tagged_heap_arena {
    struct arena                       *head;
    struct arena                       *tail;
};

enum tls_flags {
    tls_in_use  = 0,
    tls_to_free = 0x40000000,
    tls_to_wait = 0x80000000,
    tls_mask    = ~(tls_to_free | tls_to_wait),
};

struct tls {
    struct riven                       *riven;
    fcontext_t                          home_context;
    u32                                 fiber_in_use;
    u32                                 fiber_old;
    struct tagged_heap_arena            deferred[RIVEN_DEFERRED_CYCLE];
};

struct fiber {
    struct job                          job;
    fcontext_t                          context;
    atomic_usize                       *wait_counter;
};

struct tagged_heap {
    LAKE_ATOMIC(riven_tag_t)            tag;
    lake_alignment(4) atomic_spinlock   spinlock;
    struct tagged_heap_arena            arena;
};

struct framework {
    struct riven                       *riven;
    struct riven_metadata              *metadata;
    PFN_riven_framework                 procedure;
    void                               *argument;
    s32                                 result;
};

struct riven {
    struct job_ring                     jobs;
    struct arena_ring                   arenas;

    struct tls                         *tls;
    atomic_usize                        tls_sync;

    struct hash_table                   thread_table;
    bedrock_thread_t                   *threads;
    struct riven_work                  *ends;

    struct fiber                       *fibers;
    atomic_usize                       *waiting;
    atomic_usize                       *free;
    atomic_usize                       *locks;

    struct tagged_heap                  reserved_heaps[riven_tag_reserved_count];
    struct tagged_heap                **tagged_heaps;
    atomic_usize                        tagged_heap_tail;
    /* cycled by RIVEN_DEFERRED_CYCLE, access externally synchronized */
    u32                                 deferred_index;

    u32                                 tagged_heap_count;
    u32                                 thread_count;
    u32                                 fiber_count;

    atomic_u8                          *bitmap;
    atomic_usize                        growth_sync;
    u8                                 *stack;
    usize                               stack_size;

    usize                               budget;
    usize                               page_size;
    atomic_usize                        commited_size;
    atomic_usize                        malloc_allocated;

    const struct riven_hints           *hints;
};

u32 riven_thread_index(struct riven *riven)
{
    s32 *index = NULL;
    bedrock_thread_t current = bedrock_thread_current();
    hash_table_find(&riven->thread_table, &current, sizeof(bedrock_thread_t), &index);
    return (u32)*index;
}

lake_force_inline 
struct tls *get_thread_local_storage(struct riven *riven) 
{
    return &riven->tls[riven_thread_index(riven)];
}

/* position - a bit position, within the memory bitmap represents a single 256 KiB 
 *            (RIVEN_BLOCK_SIZE) block of memory using a single bit.
 *    index - an index value to access a byte within the bitmap (uint8_t per index).
 *    block - a memory offset in bytes, represents the range of a single block (1 block is 256 KiB).
 *     page - a memory offset in bytes, represents the range of a single "page" (1 page is 2 MiB, 
 *            this translates into: 1 hugepage, 1 byte worth of blocks in the bitmap). */
#define __index_from_position(val) (val >> 3lu)
#define __position_from_index(val) (val << 3lu)
#define __block_from_position(val) (val << 18lu)
#define __position_from_block(val) (val >> 18lu)
#define __index_from_page(val) (__position_from_block(val))

/** We use this bitmap to represent our blocks of free memory. One bit in the bitmap 
 *  is one block of RIVEN_BLOCK_SIZE, so with one byte we represent 8 blocks. */
#define orchestrate_bitmap(FUNC, OPERATION)                                         \
    static lake_hot lake_nonnull_all lake_thread_##FUNC##_shared(1)                 \
    void FUNC##_bitmap(                                                             \
        atomic_u8  *bitmap,                                                         \
        const usize position,                                                       \
        const usize size)                                                           \
{                                                                                   \
    if (size == 0) return;                                                          \
    const usize head_index = __index_from_position(position);                       \
    const usize count      = __index_from_page(size);                               \
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
        OPERATION;                                                                  \
        bitmask = 0xFF;                                                             \
    }                                                                               \
}

orchestrate_bitmap(acquire, lake_atomic_and_explicit(&bitmap[index], ~bitmask, lake_memory_model_acquire));
orchestrate_bitmap(release, lake_atomic_or_explicit(&bitmap[index], bitmask, lake_memory_model_release));

#include <stdlib.h>
/** Until i figure out how to implement a malloc for myself using Riven,
 *  i will be calling into the standard malloc/realloc/free implementations 
 *  and just introduce a header to comply with any alignment rules. */
struct malloc_allocation_header {
    void *outer;    /**< Unaligned pointer returned by malloc(). */
    usize size;     /**< Original size from the requested allocation. */
};

void *riven_malloc_aligned(
    struct riven   *riven,
    usize           size,
    usize           alignment)
{
    bedrock_assert_debug(alignment != 0 && !(alignment & (alignment - 1)));

    if (size == 0)
        return NULL;

    void *outer = malloc(alignment + lake_sizeof(struct malloc_allocation_header) + size);
    if (!outer)
        return NULL;

    /* add enough space for the header, then round up to alignment */
    uptr inner = ((uptr)outer + lake_sizeof(struct malloc_allocation_header) + alignment - 1) & ~(alignment - 1);

    struct malloc_allocation_header header;
    header.outer = outer;
    header.size = size;

    /* store the header just before inner */
    bedrock_memcpy((void *)(inner - lake_sizeof(struct malloc_allocation_header)), &header, lake_sizeof(struct malloc_allocation_header));
    lake_atomic_add_explicit(&riven->malloc_allocated, size, lake_memory_model_release);
    return (void *)inner;
}

void *riven_realloc_aligned(
    struct riven   *riven,
    void           *memory,
    usize           size,
    usize           alignment)
{
    bedrock_assert_debug(alignment != 0 && !(alignment & (alignment - 1)));

    if (memory == NULL)
        return NULL;

    if (size == 0) {
        riven_free(riven, memory);
        return NULL;
    }

    uptr inner = (uptr)memory;

    /* header of the original allocation */
    struct malloc_allocation_header header;
    bedrock_memcpy(&header, (void *)(inner - lake_sizeof(struct malloc_allocation_header)), lake_sizeof(struct malloc_allocation_header));

    /* If we can be certain that realloc will return a correctly-aligned pointer (which typically 
     * means alignment <= alignof(double)) then it's most efficiently to simply use that.
     *
     * Otherwise, we have no choice but to allocate a fresh buffer and copy the data across.
     * We can't speculatively try a realloc and hope that it just shrinks the buffer and preserves 
     * alignment - the problem is that if realloc breaks the alignment, and we need to fall back 
     * to the fresh-buffer-and-copy method, but the fresh allocation fails, we will have already 
     * freed the original buffer (in realloc). We can only legally return NULL if we guarantee
     * the original buffer is still valid. */
    static const usize min_realloc_alignment = lake_alignof(f64);

    if (alignment <= min_realloc_alignment) {
        void *new_outer = realloc(header.outer, alignment + lake_sizeof(struct malloc_allocation_header) + size);
        if (!new_outer)
            return NULL;

        /* verify realloc returned the alignment we expected */
        bedrock_assert_debug(((uptr)new_outer & (alignment - 1)) == 0);

        /* realloc already copied the inner contents, we just need to update the header */
        uptr new_inner = ((uptr)new_outer + lake_sizeof(struct malloc_allocation_header) + alignment - 1) & ~(alignment - 1);

        /* double check */
        bedrock_assert_debug(new_inner - inner == (uptr)new_outer - (uptr)header.outer);

        /* update the header */
        lake_atomic_add_explicit(&riven->malloc_allocated, header.size - size, lake_memory_model_release);
        header.outer = new_outer;
        header.size = size;

        /* store the updated header */
        bedrock_memcpy((void *)(new_inner - lake_sizeof(struct malloc_allocation_header)), &header, lake_sizeof(struct malloc_allocation_header));

        return (void *)new_inner;
    } else {
        /* get a totally new aligned buffer */
        void *new_inner = riven_malloc_aligned(riven, size, alignment);
        if (!new_inner)
            return NULL;
        
        /* copy the inner buffer */
        bedrock_memcpy(new_inner, (void *)inner, lake_min(size, header.size));

        /* release the original buffer */
        riven_free(riven, header.outer);
        return new_inner;
    }
}

void riven_free(
    struct riven   *riven,
    void           *memory)
{
    uptr inner = (uptr)memory;
    struct malloc_allocation_header header;

    bedrock_memcpy(&header, (void *)(inner - lake_sizeof(struct malloc_allocation_header)), lake_sizeof(struct malloc_allocation_header));
    lake_atomic_sub_explicit(&riven->malloc_allocated, header.size, lake_memory_model_release);
    free(header.outer);
}

/** Function for allocations of the tagged heap, that are equal in size to one block (RIVEN_BLOCK_SIZE).
 *  What's different from 'best_fit_growth', we don't own the sync value but it is passed as the ceiling 
 *  for our search, AND we commit the found memory block immediately by writing into the bitmap. */
static lake_nonnull(1)
usize find_first_set(
    atomic_u8  *bitmap,
    const usize position,
    const usize ceiling)
{
    if (position >= ceiling) return 0lu;

    const usize index = __index_from_position(position);
    const usize bytes = 1 + __index_from_page(ceiling) - index;

    for (;;) {
        usize bits = bits_ffs((const u8 *)&bitmap[index], bytes);

        /* 'bits_ffs()' returns a 1-based value, or 0 if no bits are set.
         * This is also why we must decrement the bits value by one. */
        if (!bits) return 0lu;

        /* The '__position_from_index' will align the value of 'position' 
         * to a byte boundary (index is: position >> 3lu). */
        bits += __position_from_index(index) - 1;

        /* We may have overshot on edging (!! holy hell) bytes. */
        if (lake_unlikely(bits >= __position_from_block(ceiling)))
            return 0lu;

        /* try to acquire the block represented in our bitmap */
        const u8 bitmask = (1 << (bits & 0x07));
        const u8 prev = lake_atomic_and_explicit(&bitmap[__index_from_position(bits)], ~bitmask, lake_memory_model_acquire);

        /* double check to avoid a possibility of data races */
        if ((prev & bitmask) == bitmask)
            return __block_from_position(bits);
    }
    lake_unreachable;
}

/** Function for allocations of the tagged heap, that are multiple contiguous blocks (RIVEN_BLOCK_SIZE * blocks). */
static lake_nonnull(1)
usize best_fit_growth(
    const atomic_u8    *bitmap,
    const usize         position,
    const usize         request,
    const usize         ceiling,
    atomic_usize       *sync)
{
    const usize blocks = __position_from_block(request);
    const usize head_index = __index_from_position(position);
    const usize tail_index = __index_from_page(ceiling);

    if (head_index > tail_index || !blocks || blocks < __position_from_block(ceiling) - position)
        return 0lu;
    /* if this function was called, we own the 'sync' counter */

    usize candidate = 0lu;
    usize current = 0lu;

    /* scan through the bitmap */
    for (usize i = head_index; i <= tail_index; i++) {
        const u8 page = lake_atomic_read_explicit(&bitmap[i], lake_memory_model_relaxed);

        /* a full free byte is convenient */
        if (page == 0xFF) {
            if (current == 0lu)
                candidate = __position_from_index(i);
            current += 8;
        } else if (page != 0) {
            u8 bitmask = 0xff;
            usize bit = 0;

            /* check individual bits */
            while (bitmask && bit < 8) {
                if (page & bitmask) {
                    if (current == 0lu)
                        candidate = __position_from_index(i) + bit;
                    current++;
                    bitmask >>= 1;
                    bit++;
                } else {
                    lake_atomic_store_explicit(sync, __block_from_position(__position_from_index(i)), lake_memory_model_relaxed);
                    current = 0lu;
                    bitmask = 0lu;
                }
            }
        } else {
            /* this memory range does not satisfy our request */
            lake_atomic_store_explicit(sync, __block_from_position(__position_from_index(i)), lake_memory_model_relaxed);
            current = 0lu;
        }

        /* git gut */
        if (current >= blocks)
            return __block_from_position(candidate);
    }
    return 0lu;
}

static lake_nonnull(1)
struct arena *acquire_new_arena(
    struct riven   *riven,
    usize           request)
{
    struct arena *arena;
    arena_ring_dequeue(&riven->arenas, &arena);
    bedrock_assert_debug(arena);

    arena->prev = NULL;
    arena->size = 0;

    const usize block_aligned = A256KB(request);
    const usize roots = riven->reserved_heaps[riven_tag_roots].arena.head->size;
    usize commited = lake_atomic_read_explicit(&riven->commited_size, lake_memory_model_relaxed);

    for (;;) {
        usize sync = lake_atomic_read_explicit(&riven->growth_sync, lake_memory_model_acquire);

        /** We can simplify the allocation process if we only need to acquire a single block of resources 
         *  from the system (256 KiB). Then it is enough to lookup just one single bit that is free, 
         *  flip it's value immediately and avoid any locks in this process. Acquiring larger amount 
         *  of resources will complicate things, tho. */
        if (block_aligned == RIVEN_BLOCK_SIZE) {
            usize ceiling = sync ? lake_min(sync, commited) : commited;
            usize offset = find_first_set(riven->bitmap, __position_from_block(roots), ceiling);

            if (offset) {
                arena->offset = offset;
                arena->capacity = RIVEN_BLOCK_SIZE;
                return arena;
            }
            if (sync) continue;
        }

        usize expected = 0lu;
        if (!lake_atomic_compare_exchange_weak_explicit(&riven->growth_sync, &expected,
            __position_from_block(roots), lake_memory_model_relaxed, lake_memory_model_relaxed))
        {
            /* sync is currently owned by another thread */
            continue;
        }
        /* we now own the sync counter */
        usize offset = 0;

        /* update the commited value, in case it has just changed */
        commited = lake_atomic_read_explicit(&riven->commited_size, lake_memory_model_acquire);

        /** This step may be skipped in case we came here from the implication, that there is not 
         *  enough free commited memory right now to satisfy even a minimal allocation (RIVEN_BLOCK_SIZE). */
        if (block_aligned > RIVEN_BLOCK_SIZE) {
            /* when it's a non-zero value, we can satisfy the allocation from existing resources */
            offset = best_fit_growth(riven->bitmap, __position_from_block(roots), block_aligned, commited, &riven->growth_sync);
            if (offset && offset + block_aligned <= commited) {
                /* write changes and drop the sync */
                acquire_bitmap(riven->bitmap, __position_from_block(offset), block_aligned);
                lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);

                arena->offset = offset;
                arena->capacity = block_aligned;
                return arena;
            }
        }
        if (!offset)
            offset = commited;
        lake_atomic_store_explicit(&riven->growth_sync, offset, lake_memory_model_release);
        usize new_ceiling = block_aligned - (commited - offset);

        /* we need to commit memory for new resources */
        if (new_ceiling + commited > riven->budget || 
            !bedrock_madvise((void *)riven, commited, new_ceiling, bedrock_madvise_mode_commit)) 
        {
            /* this step failed, so yield the allocation request */
            arena_ring_enqueue(&riven->arenas, arena);
            lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);
            return NULL;
        }
        acquire_bitmap(riven->bitmap, __position_from_block(offset), block_aligned);
        lake_atomic_store_explicit(&riven->commited_size, new_ceiling + commited, lake_memory_model_release);
        lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);
        arena->offset = offset;
        arena->capacity = block_aligned;
        return arena;
    }
    lake_unreachable;
}

static lake_nonnull(1,2)
void *tagged_heap_alloc(
    struct riven               *riven,
    struct tagged_heap_arena   *arena,
    usize                       size,
    usize                       alignment)
{
    /* riven's address is offset 0 */
    u8 *raw = (u8 *)riven;

    static const char *fatal = "Host memory failure (trace %u), not enough resources to reserve %lu bytes of memory (aligned to %lu bytes, %lu MB).";

    if (size == 0)
        return NULL;
    if (!arena->head) {
        arena->tail = arena->head = acquire_new_arena(riven, size);
        if (!arena->head) {
            bedrock_log_fatal(fatal, 1, size, A256KB(size), A256KB(size) >> 20);
            return NULL;
        }
        arena->head->size = size;
        return (void *)(uptr)(raw + arena->head->offset);
    }
    bedrock_assert_debug(arena->tail);

    /* we can assume that there is no more free space than a single block within an existing arena */
    if (size <= RIVEN_BLOCK_SIZE) {
        for (struct arena *tail = arena->tail; tail; tail = tail->prev) {
            usize aligned = (tail->size + (alignment - 1)) & ~(alignment - 1);
            if (aligned + size <= tail->capacity) {
                tail->size = aligned + size;
                return (void *)(uptr)(raw + tail->offset + aligned);
            }
        }
    }

    /* we could not yet satisfy the requested allocation */
    struct arena *next = acquire_new_arena(riven, size);
    if (!next) {
        bedrock_log_fatal(fatal, 2, size, A256KB(size), A256KB(size) >> 20);
        return NULL;
    }
    next->prev = arena->tail;
    next->size = size;
    arena->tail = next;
    return (void *)(uptr)(raw + next->offset);
}

lake_force_inline lake_nonnull(1,2)
void *protected_tagged_heap_alloc(
    struct riven       *riven,
    struct tagged_heap *heap,
    usize               size,
    usize               alignment)
{
    lake_atomic_spinlock_acquire(&heap->spinlock);
    void *memory = tagged_heap_alloc(riven, &heap->arena, size, alignment);
    lake_atomic_spinlock_release(&heap->spinlock);
    return memory;
}

void *riven_thalloc(
    struct riven   *riven,
    riven_tag_t     tag,
    usize           size,
    usize           alignment)
{
    /* must be a power of 2 */
    bedrock_assert_debug(!(alignment & (alignment - 1)));

    /* process known tags */
    if (tag < riven_tag_reserved_count) {
        struct tagged_heap *heap = &riven->reserved_heaps[tag];
        return protected_tagged_heap_alloc(riven, heap, size, alignment);
    } else if (tag == riven_tag_deferred) {
        struct tls *tls = get_thread_local_storage(riven);
        struct tagged_heap_arena *arena = &tls->deferred[riven->deferred_index];
        return tagged_heap_alloc(riven, arena, size, alignment);
    }

    /* find the heap, if the tag is in use */
    usize tail = lake_atomic_read_explicit(&riven->tagged_heap_tail, lake_memory_model_acquire);
    for (u32 i = 0; i < tail; i++) {
        struct tagged_heap *heap = riven->tagged_heaps[i];
        if (tag == lake_atomic_read_explicit(&heap->tag, lake_memory_model_relaxed))
            return protected_tagged_heap_alloc(riven, heap, size, alignment);
    }

    /* process custom tags */
    for (;;) {
        if (tail >= riven->tagged_heap_count) {
            bedrock_log_error("Reached the maximum count of unique tagged heaps (%lu), "
                "can't perform an allocation for tag: %X.", tail, tag);
            return NULL;
        }
        /* we'll use the roots tag to identify free heaps */
        riven_tag_t expected = riven_tag_roots;
        struct tagged_heap *heap = riven->tagged_heaps[tail];
        if (lake_atomic_compare_exchange_weak_explicit(&heap->tag, &expected, tag,
                lake_memory_model_acquire, lake_memory_model_relaxed))
        {
            lake_atomic_add_explicit(&riven->tagged_heap_tail, 1lu, lake_memory_model_release);
            return protected_tagged_heap_alloc(riven, heap, size, alignment);
        }
        tail = lake_atomic_read_explicit(&riven->tagged_heap_tail, lake_memory_model_acquire);
    }
    lake_unreachable;
}

static lake_hot lake_nonnull(1,2)
void tagged_heap_free(
    struct riven             *riven,
    struct tagged_heap_arena *arena)
{
    struct arena *prev = NULL;
    for (struct arena *tail = arena->tail; tail; tail = prev) {
        prev = tail->prev;

        release_bitmap(riven->bitmap, __position_from_block(tail->offset), tail->capacity);
        *tail = (struct arena){0};
        arena_ring_enqueue(&riven->arenas, tail);
    }
    arena->tail = arena->head = NULL;
}

void riven_thfree(
    struct riven   *riven,
    riven_tag_t     tag)
{
    bedrock_assert_debug(tag != riven_tag_roots);

    /* process known tags */
    if (tag < riven_tag_reserved_count) {
        struct tagged_heap *heap = &riven->reserved_heaps[tag];
        if (heap->arena.head)
            tagged_heap_free(riven, &heap->arena);
        lake_atomic_spinlock_release(&heap->spinlock);
        return;
    } else if (tag == riven_tag_deferred) {
        /* for free, we wan't to clean the current deferred_index without cycling the index value */
        u32 deferred_index = riven->deferred_index;
        for (u32 i = 0; i < riven->thread_count; i++) {
            struct tls *tls = &riven->tls[i];
            struct tagged_heap_arena *arena = &tls->deferred[deferred_index];
            if (arena->head)
                tagged_heap_free(riven, arena);
        }
        return;
    }

    /* process custom tags */
    const usize tail = lake_atomic_read_explicit(&riven->tagged_heap_tail, lake_memory_model_relaxed);
    for (usize i = 0; i < tail; i++) {
        riven_tag_t expected = tag;
        struct tagged_heap *heap = riven->tagged_heaps[i];

        if (lake_atomic_compare_exchange_weak_explicit(&heap->tag, &expected, riven_tag_roots,
                lake_memory_model_release, lake_memory_model_relaxed))
        {
            usize index = lake_atomic_sub_explicit(&riven->tagged_heap_tail, 1lu, lake_memory_model_acquire);
            /* swap the heaps */
            riven->tagged_heaps[i] = riven->tagged_heaps[index];
            riven->tagged_heaps[index] = heap;
            tagged_heap_free(riven, &heap->arena);
            lake_atomic_spinlock_release(&heap->spinlock);
            return;
        }
    }
}

void riven_rotate_deferred(struct riven *riven)
{
    u32 index = (riven->deferred_index + 1) % RIVEN_DEFERRED_CYCLE;

    /* we release resources of the current deferred index */
    for (u32 i = 0; i < riven->thread_count; i++) {
        struct tagged_heap_arena *arena = &riven->tls[i].deferred[index];
        if (arena->head)
            tagged_heap_free(riven, arena);
    }
    riven->deferred_index = index;
}

usize riven_advise_commitment(
    struct riven               *riven,
    usize                       size,
    enum bedrock_madvise_mode   mode)
{
    if (!size) return 0lu;

    const usize page_aligned = (size + riven->page_size-1) & ~(riven->page_size-1);
    const usize roots = __position_from_block(riven->reserved_heaps[riven_tag_roots].arena.head->size);

    for (;;) {
        usize expected = 0lu;
        if (!lake_atomic_compare_exchange_weak_explicit(&riven->growth_sync, &expected, 
            roots, lake_memory_model_release, lake_memory_model_relaxed)) 
        {
            continue;
        };
        const usize commited = lake_atomic_read_explicit(&riven->commited_size, lake_memory_model_relaxed);

        if (mode == bedrock_madvise_mode_release) {
            /* if possible, release unused resources */
            const usize offset = commited - page_aligned;
            const usize index = __index_from_page(offset);
            const usize count = __index_from_page(page_aligned);

            const usize bits = (count+1) * 8    /* full bytes of free blocks */
                - (index & 0x07)                /* trim head index bits out of range */
                - ((index + count) & 0x07);     /* trim tail index bits out of range */
            const usize popcnt = bits_popcnt((const u8 *)riven->bitmap + index, count);

            bool res = false;
            if (popcnt >= bits)
                res = bedrock_madvise((void *)riven, offset, page_aligned, mode);
            lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);
            if (res) {
                lake_atomic_store_explicit(&riven->commited_size, offset, lake_memory_model_release);
                return offset;
            }
            return 0lu;
        }

        usize offset = best_fit_growth(riven->bitmap, roots, page_aligned, commited, &riven->growth_sync);
        if (offset) {
            lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);
            return offset;
        }
        lake_atomic_store_explicit(&riven->growth_sync, commited, lake_memory_model_release);

        /* we may want to commit memory for new resources */
        bool res = bedrock_madvise((void *)riven, commited, page_aligned, mode);
        lake_atomic_store_explicit(&riven->growth_sync, 0lu, lake_memory_model_release);
        if (res) {
            lake_atomic_store_explicit(&riven->commited_size, commited + page_aligned, lake_memory_model_release);
            return commited;
        }
        return 0lu;
    }
    lake_unreachable;
}

static usize get_free_fiber(struct riven *riven)
{
    for (usize i = 0; i < riven->fiber_count; i++) {
        usize fiber_index = lake_atomic_read_explicit(&riven->free[i], lake_memory_model_relaxed);

        if (fiber_index == RIVEN_FIBER_INVALID) 
            continue;

        fiber_index = lake_atomic_read_explicit(&riven->free[i], lake_memory_model_acquire);
        if (fiber_index == RIVEN_FIBER_INVALID) 
            continue;

        usize expected = fiber_index;
        if (lake_atomic_compare_exchange_weak_explicit(&riven->free[i], &expected, 
            RIVEN_FIBER_INVALID, lake_memory_model_release, lake_memory_model_relaxed))
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
        lake_atomic_store_explicit(&riven->free[fiber_index], fiber_index, lake_memory_model_relaxed);

    /* wait threshold needs to be thread synced, hence the need for release */
    if (tls->fiber_old & tls_to_wait)
        lake_atomic_store_explicit(&riven->waiting[fiber_index], fiber_index, lake_memory_model_release);

    tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
}

static lake_noreturn 
void the_work(sptr raw_tls);

static usize seek_next_fiber(struct riven *riven)
{
    usize fiber_index = RIVEN_FIBER_INVALID;

    for (usize i = 0; i < riven->fiber_count; i++) {
        /** Double lock helps CPUs that have a weak memory model.
         *  ARM should go 2x to 3x faster using double lock, if its the bottleneck. */
        usize fiber_waiting = lake_atomic_read_explicit(&riven->waiting[i], lake_memory_model_relaxed);
        if (fiber_waiting == RIVEN_FIBER_INVALID) 
            continue;

        fiber_waiting = lake_atomic_read_explicit(&riven->waiting[i], lake_memory_model_acquire);
        if (fiber_waiting == RIVEN_FIBER_INVALID) 
            continue;

        struct fiber *fiber = &riven->fibers[fiber_waiting];
        usize finished = 1;
        atomic_usize *counter = fiber->wait_counter;

        if (counter) {
            usize left = lake_atomic_read_explicit(counter, lake_memory_model_relaxed);
            finished = (!left);
        }
        if (!finished) continue;

        usize expected = fiber_waiting;
        if (lake_atomic_compare_exchange_weak_explicit(&riven->waiting[i], &expected, 
            RIVEN_FIBER_INVALID, lake_memory_model_release, lake_memory_model_relaxed))
        {
            fiber_index = i;
            break;
        }
    }

    if (fiber_index == RIVEN_FIBER_INVALID) {
        struct job data;
        if (job_ring_dequeue(&riven->jobs, &data)) {

            while (fiber_index == RIVEN_FIBER_INVALID)
                fiber_index = get_free_fiber(riven);

            struct fiber *fiber = &riven->fibers[fiber_index];
            fiber->job = data;

            /* make_fcontext requires the top of the stack, because it grows down */
            u8 *stack = &riven->stack[(fiber_index * riven->stack_size) + riven->stack_size];
            make_fiber_context(&fiber->context, the_work, stack, riven->stack_size);
        }
    }
    return fiber_index;
}

struct tls *acquire_next_fiber(
    struct riven *riven,
    struct tls   *tls,
    fcontext_t   *context)
{
    atomic_usize *wait_counter = NULL;

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
            usize count = lake_atomic_read_explicit(wait_counter, lake_memory_model_relaxed);

            if (!count) {
                /* variable 'tls->fiber_in_use' still points to the 'to waitlist' fiber */
                tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
                return tls;
            }
        }
    }
    lake_unreachable;
}

static lake_noreturn 
void the_work(sptr raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    update_free_and_waiting(tls);
    struct fiber *fiber = &riven->fibers[tls->fiber_in_use];
    for (;;) {
        /* do the work */
        fiber->job.work.procedure(fiber->job.work.userdata);

        if (fiber->job.work_left) {
            usize last = lake_atomic_sub_explicit(fiber->job.work_left, 1lu, lake_memory_model_relaxed);
            bedrock_assert_debug(last > 0);

            /* reuse the fiber whenever viable */
            struct job next;
            if (last > 1 && job_ring_dequeue(&riven->jobs, &next)) {
                fiber->job = next;
                continue;
            }
        }

        /* yield the fiber */
        tls = get_thread_local_storage(riven);
        struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];
        tls->fiber_old = tls->fiber_in_use | tls_to_free;
        acquire_next_fiber(riven, tls, &old_fiber->context);

        /* context jump */
        lake_unreachable;
    }
    lake_unreachable;
}

static atomic_usize *get_lock(
    struct riven *riven,
    usize         initial_value)
{
    for (;;) {
        /* this method of traversing through the fibers is primitive
         * it may be a point of optimization to revisit this later */
        for (usize i = 0; i < riven->fiber_count; i++) {
            atomic_usize *lock = &riven->locks[i];

            if (lake_atomic_read_explicit(lock, lake_memory_model_relaxed) == RIVEN_FIBER_INVALID) {
                usize expected = RIVEN_FIBER_INVALID;

                if (lake_atomic_compare_exchange_weak_explicit(lock, &expected,
                    initial_value, lake_memory_model_relaxed, lake_memory_model_relaxed))
                {
                    return lock;
                }
            }
        }
    }
    lake_unreachable;
}

static void *dirty_deeds_done_dirt_cheap(void *raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    /* we need to wait for the main thread to be ready */
    while (!lake_atomic_read_explicit(&riven->tls_sync, lake_memory_model_acquire)) { /* spin :3 */ }

    tls->fiber_old = (u32)RIVEN_FIBER_INVALID;
    tls = acquire_next_fiber(riven, tls, &tls->home_context);

    update_free_and_waiting(tls);
    return NULL;
}

/* ends the Riven system */
static lake_noreturn
void d4c_love_train(void *raw_riven)
{
    struct riven *riven = (struct riven *)raw_riven;
    struct tls *tls = get_thread_local_storage(riven);
    struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

    if (tls == &riven->tls[0]) {
        /* i == 1, don't join the main thread */
        for (u32 i = 1; i < riven->thread_count; i++)
            bedrock_thread_join(riven->threads[i]);
    }
    tls->fiber_old = tls->fiber_in_use | tls_to_free;

    jump_fiber_context(tls, &old_fiber->context, &tls->home_context);
    lake_unreachable;
}

static lake_noreturn
void execute_framework(void *raw)
{
    struct framework *data = (struct framework *)raw;
    struct riven *riven = data->riven;
    const u32 thread_count = riven->thread_count;

    /* execute the main function */
    data->result = data->procedure(riven, riven->hints, riven->threads, data->argument);

    for (u32 i = 0; i < thread_count; i++) {
        riven->ends[i].procedure = d4c_love_train;
        riven->ends[i].userdata = (void *)riven;
        riven->ends[i].name = "riven:d4c_love_train";
    }
    riven_split_work_and_unchain(riven, riven->ends, thread_count);
    lake_unreachable;
}

void riven_split_work(
    struct riven       *riven,
    struct riven_work  *work,
    u32                 work_count,
    riven_chain_t      *chain)
{
    atomic_usize *to_use = NULL;
    bedrock_assert_debug(work_count);

    if (chain) {
        *chain = get_lock(riven, work_count);
        to_use = *chain;
    }

    for (u32 i = 0; i < work_count; i++) {
        struct job submit = { .work = work[i], .work_left = to_use };
#ifndef LAKE_NDEBUG
        while (!job_ring_enqueue(&riven->jobs, &submit)) {
            bedrock_assert_release(!"Failed to enqueue work for fibers in the job queue.");
        }
#else
        job_ring_enqueue(&riven->jobs, &submit);
#endif
    }
}

void riven_unchain(
    struct riven   *riven,
    riven_chain_t   chain)
{
    usize wait_value = 0;

    if (chain) {
        wait_value = lake_atomic_read_explicit(chain, lake_memory_model_relaxed);
        bedrock_assert_debug(wait_value != RIVEN_FIBER_INVALID);
    }

    if (wait_value) {
        struct tls *tls = get_thread_local_storage(riven);
        struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

        old_fiber->wait_counter = chain;
        tls->fiber_old = tls->fiber_in_use | tls_to_wait;
        tls = acquire_next_fiber(riven, tls, &old_fiber->context);
        update_free_and_waiting(tls);
    }
    if (chain) lake_atomic_store_explicit(chain, RIVEN_FIBER_INVALID, lake_memory_model_release);
}

void *riven_encore(struct riven *riven, struct riven_encore_work *restrict work, void *encore_userdata)
{
#ifdef LAKE_DEBUG
    if (!work->encores || !work->encore_count) {
        bedrock_log_debug("'%s' encore is empty, no work to do.", work->name);
        return NULL;
    }
#endif /* LAKE_DEBUG */

    union { void *data; struct riven_interface_header *header; } interface = { .data = NULL };
    
    for (u32 i = 0; i < work->encore_count; i++) {
        interface.data = work->encores[i](riven, riven->hints, work->metadata, encore_userdata, work->tag);

        /* the encore was discarded by internal means */
        if (interface.data == NULL)
            continue;

#ifdef LAKE_DEBUG
        bedrock_assert_debug(interface.header->zero_ref_callback);

        if (work->interface_validation && !work->interface_validation(interface.data)) {
            /* destroy the interface and continue */
            bedrock_log_debug("'%s' encore (%u out of %u) invalidated interface '%s', it will be destroyed.",
                work->name, i, work->encore_count, interface.header->name);
            interface.header->zero_ref_callback(interface.data);
            continue;
        }
#endif /* LAKE_DEBUG */

        /* accept this implementation */
        riven_inc_refcnt(&interface.header->refcnt);
        bedrock_log_verbose("'%s: %s' interface write.", work->name, interface.header->backend);
        return interface.data;
    }
    return NULL;
}

s32 riven_moonlit_walk(
    struct riven_hints     *hints,
    PFN_riven_framework     main,
    void                   *userdata)
{
    usize ram_size, page_size, hugepage_size = 0;
    bedrock_meminfo(&ram_size, &page_size);
    bedrock_hugetlbinfo(&hugepage_size, 2lu*A2MB(RIVEN_BLOCK_SIZE));

    s32 cpu_count = 0;
    bedrock_cpuinfo(&cpu_count, NULL, NULL);

    /* provide defaults */
    if (hints->thread_count == 0 || hints->thread_count > (u32)cpu_count)
        hints->thread_count = cpu_count;
    if (hints->fiber_count == 0)
        hints->fiber_count = 96 + 4 * hints->thread_count;
    if (hints->tagged_heap_count == 0)
        hints->tagged_heap_count = 16;
    if (hints->memory_budget == 0)
        hints->memory_budget = ((ram_size) & ~(RIVEN_BLOCK_SIZE-1));
    hints->memory_budget = A2MB(hints->memory_budget);
    if (hints->log2_job_count == 0)
        hints->log2_job_count = 11; /* 2048 */
    if (hints->log2_arena_count == 0)
        hints->log2_arena_count = 10; /* 1024 */

    bedrock_log_verbose("Riven setup properties: \n"
        "       Memory budget : %lu bytes (%lu MB)\n"
        "    Fiber stack size : %lu bytes (%lu KB, total: %lu bytes, %lu MB)\n"
        "      Context counts : %u fibers, %u threads, %u tagged heaps\n"
        "        Ring buffers : %u job queue, %u arenas",
        hints->memory_budget, hints->memory_budget >> 20lu,
        hints->fiber_stack_size, hints->fiber_stack_size >> 10lu, 
        hints->fiber_stack_size * hints->fiber_count, 
        (hints->fiber_stack_size * hints->fiber_count) >> 20lu,
        hints->fiber_count, hints->thread_count, hints->tagged_heap_count,
        1u << hints->log2_job_count, 1u << hints->log2_arena_count);

    struct riven *riven = NULL;
    {
        struct job_cell   *job_cells = NULL;
        struct arena_cell *arena_cells = NULL;
        struct arena      *arenas = NULL;

        usize riven_bytes               = A16(lake_sizeof(struct riven));
        usize job_count                 = 1ull << hints->log2_job_count;
        usize job_cells_bytes           = A16(lake_sizeof(struct job_cell) * job_count);
        usize arena_count               = 1ull << hints->log2_arena_count;
        usize arena_cells_bytes         = A16(lake_sizeof(struct arena_cell) * arena_count);
        usize arenas_bytes              = A16(lake_sizeof(struct arena) * arena_count);
        usize tls_bytes                 = A16(lake_sizeof(struct tls) * hints->thread_count);
        usize threads_bytes             = A16(lake_sizeof(bedrock_thread_t) * hints->thread_count);
        usize ends_bytes                = A16(lake_sizeof(struct riven_work) * hints->thread_count);
        usize fibers_bytes              = A16(lake_sizeof(struct fiber) * hints->fiber_count);
        usize waiting_bytes             = A16(lake_sizeof(atomic_usize) * hints->fiber_count);
        usize free_bytes                = A16(lake_sizeof(atomic_usize) * hints->fiber_count);
        usize locks_bytes               = A16(lake_sizeof(atomic_usize) * hints->fiber_count);
        usize tagged_heaps_array_bytes  = A16(lake_sizeof(struct tagged_heap *) * hints->tagged_heap_count);
        usize tagged_heaps_bytes        = A16(lake_sizeof(struct tagged_heap) * hints->tagged_heap_count);
        usize block_count               = __position_from_block(hints->memory_budget);
        usize bitmap_bytes              = A16(__index_from_position(block_count));
        usize stack_bytes               = A16(hints->fiber_stack_size);
        usize stack_heap_bytes          = stack_bytes * hints->fiber_count;

        usize total_bytes =
            riven_bytes +
            job_cells_bytes +
            arena_cells_bytes +
            arenas_bytes +
            tls_bytes +
            threads_bytes +
            ends_bytes +
            fibers_bytes +
            waiting_bytes +
            free_bytes +
            locks_bytes +
            tagged_heaps_array_bytes +
            tagged_heaps_bytes +
            bitmap_bytes +
            stack_heap_bytes;

        usize roots_arena_size = A256KB(total_bytes);
        usize commit_size = lake_min(A2MB(roots_arena_size << 1), hints->memory_budget);

        /* address to riven is offset 0 */
        riven = (struct riven *)bedrock_mmap(hints->memory_budget, hugepage_size);
        if (!riven || !bedrock_madvise(riven, 0, commit_size, bedrock_madvise_mode_commit)) 
            return -1;
        bedrock_memset(riven, 0u, commit_size);

        usize o = riven_bytes;
        u8 *raw = (u8 *)riven;

        job_cells       = (struct job_cell *)   &raw[o]; o += job_cells_bytes;
        arena_cells     = (struct arena_cell *) &raw[o]; o += arena_cells_bytes;
        arenas          = (struct arena *)      &raw[o]; o += arenas_bytes;
        riven->tls      = (struct tls *)        &raw[o]; o += tls_bytes;
        riven->threads  = (bedrock_thread_t *)  &raw[o]; o += threads_bytes;
        riven->ends     = (struct riven_work *) &raw[o]; o += ends_bytes;
        riven->fibers   = (struct fiber *)      &raw[o]; o += fibers_bytes;
        riven->waiting  = (atomic_usize *)      &raw[o]; o += waiting_bytes;
        riven->free     = (atomic_usize *)      &raw[o]; o += free_bytes;
        riven->locks    = (atomic_usize *)      &raw[o]; o += locks_bytes;

        for (u32 i = 0; i < riven_tag_reserved_count; i++)
            lake_atomic_store_explicit(&riven->reserved_heaps[i].tag, i, lake_memory_model_release);

        riven->tagged_heaps = (struct tagged_heap **)&raw[o]; o += tagged_heaps_array_bytes;
        for (u32 i = 0; i < hints->tagged_heap_count; i++) {
            riven->tagged_heaps[i] = (struct tagged_heap *)&raw[o]; o += tagged_heaps_bytes;
        }
        riven->bitmap   = (atomic_u8 *)&raw[o]; 
        o += bitmap_bytes;
        riven->stack    = (u8 *)&raw[o];

        /* bits set to 1 indicate free memory blocks */
        release_bitmap(riven->bitmap, __position_from_block(roots_arena_size), hints->memory_budget);

        riven->stack_size = stack_bytes;
        riven->budget = hints->memory_budget;
        riven->page_size = hugepage_size;
        lake_atomic_store_explicit(&riven->commited_size, commit_size, lake_memory_model_release);
        riven->tagged_heap_count = hints->tagged_heap_count;
        riven->thread_count = hints->thread_count;
        riven->fiber_count = hints->fiber_count;
        riven->hints = hints;

        bedrock_assert_debug(!(((sptr)job_cells)           & 15));
        bedrock_assert_debug(!(((sptr)arena_cells)         & 15));
        bedrock_assert_debug(!(((sptr)arenas)              & 15));
        bedrock_assert_debug(!(((sptr)riven->tls)          & 15));
        bedrock_assert_debug(!(((sptr)riven->threads)      & 15));
        bedrock_assert_debug(!(((sptr)riven->ends)         & 15));
        bedrock_assert_debug(!(((sptr)riven->fibers)       & 15));
        bedrock_assert_debug(!(((sptr)riven->waiting)      & 15));
        bedrock_assert_debug(!(((sptr)riven->free)         & 15));
        bedrock_assert_debug(!(((sptr)riven->locks)        & 15));
        bedrock_assert_debug(!(((sptr)riven->tagged_heaps) & 15));
        bedrock_assert_debug(!(((sptr)riven->bitmap)       & 15));
        bedrock_assert_debug(!(((sptr)riven->stack)        & 15));

        /* setup the ring buffers */
        job_ring_init(&riven->jobs, job_cells, job_count);
        arena_ring_init(&riven->arenas, arena_cells, arena_count);
        for (u32 i = 0; i < arena_count; i++)
            arena_ring_enqueue(&riven->arenas, &arenas[i]);

        /* setup fibers */
        for (u32 i = 0; i < hints->fiber_count; i++) {
            lake_atomic_store_explicit(&riven->free[i], i, lake_memory_model_relaxed);
            lake_atomic_store_explicit(&riven->waiting[i], RIVEN_FIBER_INVALID, lake_memory_model_relaxed);
            lake_atomic_store_explicit(&riven->locks[i], RIVEN_FIBER_INVALID, lake_memory_model_relaxed);
        }

        /* setup the roots arena */
        struct arena *roots = NULL;
        arena_ring_dequeue(&riven->arenas, &roots);
        bedrock_assert_debug(roots);

        riven->reserved_heaps[riven_tag_roots].arena.tail = riven->reserved_heaps[riven_tag_roots].arena.head = roots;
        riven->reserved_heaps[riven_tag_roots].arena.head->capacity = roots_arena_size;
        riven->reserved_heaps[riven_tag_roots].arena.head->size = total_bytes;
        /* now we can safely call riven_thalloc() with riven_tag_roots */
        hash_table_init(&riven->thread_table, riven, riven_tag_roots, bits_log2_next_pow2(hints->thread_count));
    }
    /* prepare the main thread */
    riven->tls[0].riven = riven;
    riven->tls[0].fiber_in_use = (u32)get_free_fiber(riven);
    riven->threads[0] = bedrock_thread_current();
    /* hash the main thread handle */
    hash_table_insert(&riven->thread_table, (void *)&riven->threads[0], lake_sizeof(bedrock_thread_t), 0);

    /* setup threads and thread-local storage */
    lake_atomic_store_explicit(&riven->tls_sync, 0lu, lake_memory_model_release);
    for (u32 i = 1; i < hints->thread_count; i++) {
        struct tls *tls = &riven->tls[i];
        tls->riven = riven;
        tls->fiber_in_use = (u32)RIVEN_FIBER_INVALID;
        bedrock_thread_create(&riven->threads[i], dirty_deeds_done_dirt_cheap, (void *)tls);
        /* hash the thread handle */
        hash_table_insert(&riven->thread_table, (void *)&riven->threads[i], lake_sizeof(bedrock_thread_t), i);
    }
    bedrock_thread_affinity(riven->stack, riven->threads, hints->thread_count, cpu_count, 0);
    lake_atomic_store_explicit(&riven->tls_sync, 1lu, lake_memory_model_release);

    /* setup the framework */
    struct framework framework = {
        .riven = riven,
        .procedure = main,
        .argument = userdata,
        .result = 0,
    };
    struct riven_work work = {
        .procedure = execute_framework,
        .userdata = (void *)&framework,
        .name = "riven:framework",
    };

    riven_split_work(riven, &work, 1, NULL);
    dirty_deeds_done_dirt_cheap((void *)&riven->tls[0]);
    bedrock_munmap(riven, riven->budget);
    return framework.result;
}
