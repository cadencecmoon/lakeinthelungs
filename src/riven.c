#include "riven.h"

#define FIBER_INVALID SIZE_MAX

#define A16(x) (((x) + 15) & ~15)

#ifndef RIVEN_STACK_GUARD
    #define RIVEN_STACK_GUARD 0
#endif

/* fiber context */
typedef void *fcontext_t;

/* implemented in asm, ofc/nfc - original/new fiber context, preserve_fpu - procedure */
extern intptr_t jump_fcontext(fcontext_t *ofc, fcontext_t nfc, intptr_t vp, int preserve_fpu);

/* implemented in asm, sp - top of stack pointer */
extern fcontext_t make_fcontext(void *sp, size_t size, void (*fn)(intptr_t));

#if RIVEN_STACK_GUARD
#if defined(AMW_PLATFORM_LINUX)
    #include <unistd.h>
    #include <sys/mman.h>
#elif defined(AMW_PLATFORM_WINDOWS)
    #include <process.h>
#else
    #error Figure out how to protect a memory stack for this platform
#endif
#endif /* RIVEN_STACK_GUARD */

/* thread local storage forward declaration */
struct tls;

static_assertion(sizeof(intptr_t) == sizeof(struct riven *), "intptr_t size assumption");
static_assertion(sizeof(size_t) >= sizeof(void *), "size_t can't hold pointers");

static intptr_t internal_jump(struct tls *tls, fcontext_t *from, fcontext_t *to)
{
    assert_debug(from && to);
    assert_debug(*from != *to);
    return jump_fcontext(from, *to, (intptr_t)tls, 1);
}

static void internal_make(fcontext_t *context, void (*procedure)(intptr_t), void *stack, size_t stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

#if RIVEN_STACK_GUARD
static void stack_memory_guard(void *mem, size_t bytes)
{
#if defined(AMW_PLATFORM_LINUX)
    int res = mprotect(mem, bytes, PROT_NONE);
    assert_debug(!res);
#elif defined(AMW_PLATFORM_WINDOWS)
    DWORD ignored;
    BOOL res = VirtualProtect(mem, bytes, PAGE_NOACCESS, &ignored);
    assert_debug(res);
#endif
}
#else
#define stack_memory_guard(mem, bytes)
#endif /* RIVEN_STACK_GUARD */

#if RIVEN_STACK_GUARD
static void stack_memory_stop(void *mem, size_t bytes)
{
#if defined(AMW_PLATFORM_LINUX)
    int res = mprotect(mem, bytes, PROT_READ | PROT_WRITE);
    assert_debug(!res);
#elif defined(AMW_PLATFORM_WINDOWS)
    DWORD ignored;
    BOOL res = VirtualProtect(mem, bytes, PAGE_READWRITE, &ignored);
    assert_debug(res);
#endif
}
#else
#define stack_memory_stop(mem, bytes)
#endif /* RIVEN_STACK_GUARD */

#if AMW_HAS_VALGRIND && defined(AMW_DEBUG)
    #include <valgrind/valgrind.h>
    #define RIVEN_VALGRIND_ID unsigned stack_id
    #define RIVEN_VALGRIND_STACK_REGISTER(x,s,e) x->stack_id = VALGRIND_STACK_REGISTER(s,e)
    #define RIVEN_VALGRIND_STACK_DEREGISTER(x) VALGRIND_STACK_DEREGISTER(x.stack_id)
#else
    #define RIVEN_VALGRIND_ID
    #define RIVEN_VALGRIND_STACK_REGISTER(x,s,e)
    #define RIVEN_VALGRIND_STACK_DEREGISTER(x)
#endif

struct ripntear {
    struct riven_tear tear;
    at_size_t        *work_left;
};

struct mpmc_cell;

#define CACHELINE_SIZE 64
struct mpmc_queue {
    char              pad0[CACHELINE_SIZE];

    struct mpmc_cell *buffer;
    size_t            buffer_mask;
    char              pad1[CACHELINE_SIZE];

    at_size_t         enqueue_pos;
    char              pad2[CACHELINE_SIZE];

    at_size_t         dequeue_pos;
    char              pad3[CACHELINE_SIZE];
};

struct mpmc_cell {
    at_size_t       sequence;
    struct ripntear rip;
};

static void mpmc_queue_init(
        struct mpmc_queue *q, 
        struct mpmc_cell *cells, 
        size_t cell_count)
{
    if (!(cell_count && !(cell_count & (cell_count - 1)))) {
        assert_release(!"cell_count is not a power of 2");
        q->buffer = NULL;
        return;
    }
    q->buffer = cells;
    q->buffer_mask = cell_count - 1; /* a bitmask from ((1 << log_2_tear_count) - 1) */

    for (size_t i = 0; i != (q->buffer_mask + 1); i++) {
        at_store_explicit(&q->buffer[i].sequence, i, memory_model_relaxed);
    }
    at_store_explicit(&q->enqueue_pos, (size_t)0, memory_model_relaxed);
    at_store_explicit(&q->dequeue_pos, (size_t)0, memory_model_relaxed);
}

struct mpmc_work {
    struct mpmc_cell *cell;
    size_t            position;
};

static struct mpmc_work mpmc_queue_work(
        struct mpmc_queue *q, 
        at_size_t *in_or_out, 
        const unsigned pos_delta)
{
    struct mpmc_cell *cell;
    size_t position = at_read_explicit(in_or_out, memory_model_relaxed);

    for (;;) {
        cell = &q->buffer[position & q->buffer_mask];

        size_t sequence = at_read_explicit(&cell->sequence, memory_model_acquire);
        intptr_t difference = (intptr_t)sequence - (intptr_t)(position + pos_delta);

        if (!difference) {
            if (at_compare_exchange_weak_explicit(
                in_or_out, &position, position + 1,
                memory_model_relaxed, memory_model_relaxed))
            {
                struct mpmc_work result = { .cell = cell, .position = position };
                return result;
            }
        } else {
            if (difference < 0) {
                struct mpmc_work result = { .cell = NULL, .position = 0 };
                return result;
            } else {
                position = at_read_explicit(in_or_out, memory_model_relaxed);
                /* XXX a deadlock happens here if the job queue is full and running only one thread */
            }
        }
    }
}

static int32_t mpmc_enqueue(
        struct mpmc_queue *q, 
        struct ripntear *rip)
{
    struct mpmc_work work = mpmc_queue_work(q, &q->enqueue_pos, 0); 

    if (work.cell) {
        work.cell->rip = *rip;
        at_store_explicit(&work.cell->sequence, work.position + 1, memory_model_release);
        return 1;
    }
    return 0;
}

static int32_t mpmc_dequeue(
        struct mpmc_queue *q,
        struct ripntear *rip)
{
    struct mpmc_work work = mpmc_queue_work(q, &q->dequeue_pos, 1);

    if (work.cell) {
        *rip = work.cell->rip;
        at_store_explicit(&work.cell->sequence, work.position + q->buffer_mask + 1, memory_model_release);
        return 1;
    }
    return 0;
}

struct fiber {
    struct ripntear rip;
    fcontext_t      context;
    at_size_t      *wait_counter;

    RIVEN_VALGRIND_ID;
};

enum tls_flags {
    TLS_IN_USE  = 0,
    TLS_TO_FREE = 0x40000000,
    TLS_TO_WAIT = 0x80000000,
    TLS_MASK    = ~(TLS_TO_FREE | TLS_TO_WAIT),
};

struct tls {
    struct riven   *riven;
    fcontext_t      home_context;
    uint32_t        fiber_in_use;
    uint32_t        fiber_old;
};

struct entrypoint_data {
    struct riven   *riven;
    PFN_riven_main  procedure;
    void           *argument;
};

struct riven {
    struct mpmc_queue   queue;
    struct mpmc_cell   *cells;

    struct tls         *tls;
    at_size_t           tls_sync;

    struct fiber       *fibers;
    at_size_t          *waiting;
    at_size_t          *free;
    at_size_t          *locks;

    thread_t           *threads;
    struct riven_tear  *ends;

    uint8_t            *stack;
    size_t              stack_bytes;
    size_t              thread_count;
    size_t              fiber_count;
};

static struct tls *get_thread_local_storage(struct riven *riven)
{
    size_t index = thread_index(riven->threads, riven->thread_count);
    return &riven->tls[index];
}

static size_t get_free_fiber(struct riven *riven)
{
    for (size_t i = 0; i < riven->fiber_count; i++) {
        size_t fiber = at_read_explicit(&riven->free[i], memory_model_relaxed);

        if (fiber == FIBER_INVALID)
            continue;

        fiber = at_read_explicit(&riven->free[i], memory_model_acquire);
        if (fiber == FIBER_INVALID)
            continue;

        size_t expected = fiber;

        if (at_compare_exchange_weak_explicit(
            &riven->free[i], &expected, FIBER_INVALID,
            memory_model_release, memory_model_relaxed))
        {
            return fiber;
        }
    }
    return FIBER_INVALID;
}

static void update_free_and_waiting(struct tls *tls)
{
    struct riven *riven = tls->riven;

    if (tls->fiber_old == (uint32_t)FIBER_INVALID)
        return;

    const size_t fiber_idx = tls->fiber_old & TLS_MASK;

    if (tls->fiber_old & TLS_TO_FREE) {
        RIVEN_VALGRIND_STACK_DEREGISTER(riven->fibers[fiber_idx]);

        /* thread that added fiber to free list is the same as the one freeing it */
        at_store_explicit(&riven->free[fiber_idx], fiber_idx, memory_model_relaxed);
    }

    if (tls->fiber_old & TLS_TO_WAIT) {
        /* wait threshold needs to be thread synced, hence the need for release */
        at_store_explicit(&riven->waiting[fiber_idx], fiber_idx, memory_model_release);
    }
    tls->fiber_old = (uint32_t)FIBER_INVALID;
}

static void weave(intptr_t raw_tls);

static size_t get_next_fiber(struct riven *riven)
{
    size_t fiber_idx = FIBER_INVALID;

    for (size_t i = 0; i < riven->fiber_count; i++) {
        /* double lock helps CPUs that have a weak memory model 
         * arm should go 2x to 3x faster using double lock if its the bottleneck */
        size_t fiber_waiting = at_read_explicit(&riven->waiting[i], memory_model_relaxed);

        if (fiber_waiting == FIBER_INVALID)
            continue;

        fiber_waiting = at_read_explicit(&riven->waiting[i], memory_model_acquire);

        if (fiber_waiting == FIBER_INVALID)
            continue;

        struct fiber *fiber = &riven->fibers[fiber_waiting];
        size_t        finished = 1;
        at_size_t    *counter = fiber->wait_counter;

        if (counter) {
            size_t left = at_read_explicit(counter, memory_model_relaxed);
            finished = (!left);
        }
        if (!finished)
            continue;

        size_t expected = fiber_waiting;

        if (at_compare_exchange_weak_explicit(
            &riven->waiting[i], &expected, FIBER_INVALID,
            memory_model_release, memory_model_relaxed))
        {
            fiber_idx = i;
            break;
        }
    }

    if (fiber_idx == FIBER_INVALID) {
        struct ripntear rip;

        if (mpmc_dequeue(&riven->queue, &rip)) {

            while (fiber_idx == FIBER_INVALID)
                fiber_idx = get_free_fiber(riven);

            struct fiber *fiber = &riven->fibers[fiber_idx];
            fiber->rip = rip;

            const size_t bytes = riven->stack_bytes + RIVEN_STACK_GUARD;
            uint8_t     *stack = &riven->stack[(fiber_idx * bytes) + bytes];

            /* make_fcontext requires top of the stack, because it grows down */
            internal_make(&fiber->context, weave, stack, riven->stack_bytes);

            RIVEN_VALGRIND_STACK_REGISTER(fiber, stack, stack - bytes);
        }
    }
    return fiber_idx;
}

static struct tls *next_fiber(
        struct riven *riven, 
        struct tls *tls, 
        fcontext_t *context)
{
    at_size_t *wait_counter = NULL;

    if ((tls->fiber_old != (uint32_t)FIBER_INVALID) && (tls->fiber_old & TLS_TO_WAIT)) {
        const size_t  fiber_idx = tls->fiber_old & TLS_MASK;
        struct fiber *fiber = &riven->fibers[fiber_idx];
        wait_counter = fiber->wait_counter;
    }

    for (;;) {
        size_t fiber_idx = get_next_fiber(riven);

        if (fiber_idx != FIBER_INVALID) {
            struct fiber *fiber = &riven->fibers[fiber_idx];
            tls->fiber_in_use = (uint32_t)fiber_idx;
            return (struct tls *)internal_jump(tls, context, &fiber->context);
        }

        /* Race condition fix.
         * 
         * Context needs to wait until a set of jobs are done, so starts
         * swapping to do a new job.
         *
         * The jobs finish *before* a new context to swap to is found.
         * There are no new jobs.
         *
         * The context swap code deadlocks looking for a new job to swap to,
         * but none will arrive. Meanwhile the 'to be swapped' context is
         * waiting to be run, but cannot as it hasn't been swapped out yet
         * (in order to be picked up by the wait list). */
        if (wait_counter) {
            size_t count = at_read_explicit(wait_counter, memory_model_relaxed);

            if (!count) {
                /* tls->fiber_in_use still points to the 'to waitlist' fiber */
                tls->fiber_old = (uint32_t)FIBER_INVALID;
                return tls;
            }
        }
    }
}

static void weave(intptr_t raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    update_free_and_waiting(tls);
    { /* do the work */
        struct fiber *fiber = &riven->fibers[tls->fiber_in_use];
        fiber->rip.tear.procedure(fiber->rip.tear.argument);

        if (fiber->rip.work_left) {
            size_t last = at_fetch_sub_explicit(fiber->rip.work_left, (size_t)1u, memory_model_relaxed);
            if (!last) {
                assert_debug(last > 0);
            }
        }
    }

    tls = get_thread_local_storage(riven);
    struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];
    tls->fiber_old = tls->fiber_in_use | TLS_TO_FREE;

    next_fiber(riven, tls, &old_fiber->context);
    AMW_UNREACHABLE;
}

static at_size_t *get_lock(
        struct riven *riven, 
        size_t initial_value)
{
    for (;;) {
        for (size_t i = 0; i < riven->fiber_count; i++) {
            at_size_t *lock = &riven->locks[i];

            if (at_read_explicit(lock, memory_model_relaxed) == FIBER_INVALID) {
                size_t expected = FIBER_INVALID;

                if (at_compare_exchange_weak_explicit(
                    lock, &expected, initial_value, 
                    memory_model_relaxed, memory_model_relaxed))
                {
                    return lock;
                }
            }
        }
    }
}

static void *do_the_work(void *raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    struct riven *riven = tls->riven;

    while (!at_read_explicit(&riven->tls_sync, memory_model_acquire)) { /* spin :3 */ }

    tls->fiber_old = (uint32_t)FIBER_INVALID;
    tls = next_fiber(riven, tls, &tls->home_context);

    update_free_and_waiting(tls);

    return NULL;
}

/* quit the work */
static void payday(void *raw_riven)
{
    struct riven *riven = (struct riven *)raw_riven;
    struct tls *tls = get_thread_local_storage(riven);
    struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

    if (tls == &riven->tls[0]) {
        /* i = 1, don't join the main thread... */
        for (size_t i = 1; i < riven->thread_count; i++) {
            thread_join(riven->threads[i]); 
        }
    }
    tls->fiber_old = tls->fiber_in_use | TLS_TO_FREE;

    internal_jump(tls, &old_fiber->context, &tls->home_context);
    AMW_UNREACHABLE;
}

static void entrypoint(void *raw_entrypoint_data)
{
    struct entrypoint_data *data = (struct entrypoint_data *)raw_entrypoint_data;
    struct riven *riven = data->riven;
    const size_t thread_count = riven->thread_count;

    data->procedure(riven, riven->threads, thread_count, data->argument);

    /* returned from main procedure, tell every thread to <redacted> */
    for (size_t i = 0; i < thread_count; i++) {
        riven->ends[i].procedure = payday;
        riven->ends[i].argument  = (void *)riven;
        riven->ends[i].name      = "riven:payday";
    }

    riven_split_and_unchain(riven, riven->ends, thread_count);
    AMW_UNREACHABLE;
}

void riven_split_and_unchain(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits)
{
    riven_chain_t chain;
    riven_split(riven, tears, splits, &chain);
    riven_unchain(riven, chain);
}

void riven_split(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits,
        riven_chain_t *chain)
{
    at_size_t **counters = chain;
    at_size_t  *to_use = NULL;

    if (counters) {
        *counters = get_lock(riven, splits);
        to_use = *counters;
    }

    for (size_t i = 0; i < splits; i++) {
        struct ripntear rip = { .tear = tears[i], .work_left = to_use };
        while (!mpmc_enqueue(&riven->queue, &rip)) { 
            assertion(!"Failed to queue work for fibers in the multiple-producer multiple-consumer queue."); 
        };
    }
}

void riven_unchain(
        struct riven *riven, 
        riven_chain_t chain)
{
    at_size_t *counter = chain;
    size_t wait_value = 0;

    if (counter) {
        wait_value = at_read_explicit(counter, memory_model_relaxed);
        assert_debug(wait_value != FIBER_INVALID);
    }

    if (wait_value) {
        struct tls *tls = get_thread_local_storage(riven);
        struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

        old_fiber->wait_counter = counter;
        tls->fiber_old = tls->fiber_in_use | TLS_TO_WAIT;
        tls = next_fiber(riven, tls, &old_fiber->context);

        update_free_and_waiting(tls);
    }
    if (counter) {
        at_store_explicit(counter, FIBER_INVALID, memory_model_release);
    }
}

void riven_chain_exile(
        struct riven *riven,
        riven_chain_t *chain)
{
    at_size_t **counters = chain;
    *counters = get_lock(riven, 1);
}

void riven_unchain_exile(riven_chain_t chain)
{
    at_size_t *counter = chain;
    at_store_explicit(counter, 0ul, memory_model_release);
}

size_t riven_unveil_rift(
        void *riven_memory,
        size_t fiber_stack_bytes,
        size_t fiber_count,
        size_t log_2_tear_count,
        size_t thread_count,
        PFN_riven_main main_procedure,
        void *main_argument)
{
    assert_debug(thread_count > 0);
    assert_debug(fiber_stack_bytes > 1);
    assert_debug(fiber_count >= (thread_count << 1));

    struct riven *riven = NULL;
    {
        size_t riven_bytes          = A16(sizeof(struct riven));
        size_t cells_count          = 1ull << log_2_tear_count;
        size_t cells_bytes          = A16(sizeof(struct mpmc_cell) * cells_count);
        size_t tls_bytes            = A16(sizeof(struct tls)) * thread_count;
        size_t fibers_bytes         = A16(sizeof(struct fiber) * fiber_count);
        size_t waiting_bytes        = A16(sizeof(size_t) * fiber_count);
        size_t free_bytes           = A16(sizeof(size_t) * fiber_count);
        size_t lock_bytes           = A16(sizeof(size_t) * fiber_count);
        size_t thread_bytes         = A16(sizeof(thread_t) * thread_count);
        size_t end_bytes            = A16(sizeof(struct riven_tear) * thread_count);
        size_t stack_bytes          = A16(fiber_stack_bytes);
        size_t stack_heap_bytes     = stack_bytes * fiber_count;
        size_t bytes_alignment      = 15 + (RIVEN_STACK_GUARD);

        stack_heap_bytes += RIVEN_STACK_GUARD * (fiber_count + 1);

        size_t total_bytes = 
            riven_bytes +
            cells_bytes +
            tls_bytes +
            fibers_bytes +
            waiting_bytes +
            free_bytes +
            lock_bytes +
            thread_bytes +
            end_bytes +
            stack_heap_bytes +
            bytes_alignment;

        if (!riven_memory)
            return total_bytes;

        riven = (struct riven *)A16((intptr_t)riven_memory);
        { /* zero */
            uint8_t *z = (uint8_t *)riven;
            for (size_t i = 0; i < total_bytes; i++) {
                z[i] = 0;
            }
        }

        size_t o = riven_bytes;
        uint8_t *raw = (uint8_t *)riven;

        riven->cells   = (struct mpmc_cell *)   &raw[o]; o += cells_bytes;
        riven->tls     = (struct tls *)         &raw[o]; o += tls_bytes;
        riven->fibers  = (struct fiber *)       &raw[o]; o += fibers_bytes;
        riven->waiting = (size_t *)             &raw[o]; o += waiting_bytes;
        riven->free    = (size_t *)             &raw[o]; o += free_bytes;
        riven->locks   = (size_t *)             &raw[o]; o += lock_bytes;
        riven->threads = (thread_t *)           &raw[o]; o += thread_bytes;
        riven->ends    = (struct riven_tear *)  &raw[o]; o += end_bytes;
#if RIVEN_STACK_GUARD > 0
        riven->stack = (uint8_t *)((((intptr_t)&raw[o]) + (RIVEN_STACK_GUARD - 1)) & ~(RIVEN_STACK_GUARD - 1));
#else
        riven->stack = (uint8_t *)&raw[o];
#endif /* RIVEN_STACK_GUARD */
        riven->stack_bytes = stack_bytes;
        riven->thread_count = thread_count;
        riven->fiber_count = fiber_count;

        assert_debug(!(((intptr_t)riven->cells  ) & 15));
        assert_debug(!(((intptr_t)riven->tls    ) & 15));
        assert_debug(!(((intptr_t)riven->fibers ) & 15));
        assert_debug(!(((intptr_t)riven->waiting) & 15));
        assert_debug(!(((intptr_t)riven->free   ) & 15));
        assert_debug(!(((intptr_t)riven->locks  ) & 15));
        assert_debug(!(((intptr_t)riven->threads) & 15));
        assert_debug(!(((intptr_t)riven->ends   ) & 15));
        assert_debug(!(((intptr_t)riven->stack  ) & 15));
    }
    mpmc_queue_init(&riven->queue, riven->cells, 1ull << log_2_tear_count);

    for (size_t i = 0; i <= riven->fiber_count; i++) {
        stack_memory_guard(&riven->stack[i * (riven->stack_bytes + RIVEN_STACK_GUARD)], RIVEN_STACK_GUARD);
    }

    for (size_t i = 0; i < riven->fiber_count; i++) {
        at_store_explicit(&riven->free[i], i, memory_model_release);
        at_store_explicit(&riven->waiting[i], FIBER_INVALID, memory_model_release);
        at_store_explicit(&riven->locks[i], FIBER_INVALID, memory_model_release);
    }

    { /* setup main thread */
        riven->tls[0].riven        = riven;
        riven->tls[0].fiber_in_use = (uint32_t)get_free_fiber(riven);
        riven->threads[0]          = thread_current();
    }
    at_store_explicit(&riven->tls_sync, (size_t)0, memory_model_release);

    for (size_t i = 1; i < thread_count; i++) {
        struct tls *tls = &riven->tls[i];
        tls->riven = riven;
        tls->fiber_in_use = (uint32_t)FIBER_INVALID;
        thread_create(&riven->threads[i], do_the_work, (void *)tls);
    }
    at_store_explicit(&riven->tls_sync, (size_t)1, memory_model_release);

    thread_affinity(&riven->stack[RIVEN_STACK_GUARD], riven->threads, thread_count, 0);

    struct entrypoint_data main_data = {
        .riven = riven,
        .procedure = main_procedure,
        .argument = main_argument,
    };

    struct riven_tear main_tear = {
        .procedure = entrypoint,
        .argument = (void *)&main_data,
        .name = "riven:entrypoint",
    };

    riven_split(riven, &main_tear, 1, NULL);
    do_the_work((void *)&riven->tls[0]);

    for (size_t i = 0; i <= riven->fiber_count; i++) {
        stack_memory_stop(&riven->stack[i * (riven->stack_bytes + AMW_RIVEN_STACK_GUARD)], AMW_RIVEN_STACK_GUARD);
    }

    return 0;
}
