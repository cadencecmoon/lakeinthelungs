#include <lake/riven.h>

#define FIBER_INVALID SSIZE_MAX

#define A16(x) (((x) + 15) & ~15)

#ifndef RIVEN_STACK_GUARD
    #define RIVEN_STACK_GUARD 0
#endif

/* fiber context */
typedef void *fcontext_t;

/* implemented in asm, ofc/nfc - original/new fiber context, preserve_fpu - procedure */
extern sptr jump_fcontext(fcontext_t *ofc, fcontext_t nfc, sptr vp, s32 preserve_fpu);

/* implemented in asm, sp - top of stack pointer */
extern fcontext_t make_fcontext(void *sp, usize size, void (*fn)(sptr));

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

static_assertion(sizeof(sptr) == sizeof(rivens_rift *), "intptr_t size assumption");
static_assertion(sizeof(usize) >= sizeof(void *), "size_t can't hold pointers");

static sptr internal_jump(struct tls *tls, fcontext_t *from, fcontext_t *to)
{
    assert_debug(from && to);
    assert_debug(*from != *to);
    return jump_fcontext(from, *to, (sptr)tls, 1);
}

static void internal_make(fcontext_t *context, void (*procedure)(sptr), void *stack, usize stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

#if RIVEN_STACK_GUARD
static void stack_memory_guard(void *mem, size_t bytes)
{
#if defined(AMW_PLATFORM_LINUX)
    s32 res = mprotect(mem, bytes, PROT_NONE);
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
    s32 res = mprotect(mem, bytes, PROT_READ | PROT_WRITE);
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
    rivens_tear tear;
    at_ssize   *work_left;
};

struct mpmc_cell;

#define CACHELINE_SIZE 64
struct mpmc_queue {
    char              pad0[CACHELINE_SIZE];

    struct mpmc_cell *buffer;
    ssize             buffer_mask;
    char              pad1[CACHELINE_SIZE];

    at_ssize          enqueue_pos;
    char              pad2[CACHELINE_SIZE];

    at_ssize          dequeue_pos;
    char              pad3[CACHELINE_SIZE];
};

struct mpmc_cell {
    at_ssize        sequence;
    struct ripntear rip;
};

static void mpmc_queue_init(
    struct mpmc_queue *q, 
    struct mpmc_cell  *cells, 
    ssize              cell_count)
{
    if (!(cell_count && !(cell_count & (cell_count - 1)))) {
        assert_release(!"cell_count is not a power of 2");
        q->buffer = NULL;
        return;
    }
    q->buffer = cells;
    q->buffer_mask = cell_count - 1; /* a bitmask from ((1 << log_2_tear_count) - 1) */

    for (ssize i = 0; i != (q->buffer_mask + 1); i++) {
        at_store_explicit(&q->buffer[i].sequence, i, memory_model_relaxed);
    }
    at_store_explicit(&q->enqueue_pos, (ssize)0, memory_model_relaxed);
    at_store_explicit(&q->dequeue_pos, (ssize)0, memory_model_relaxed);
}

struct mpmc_work {
    struct mpmc_cell *cell;
    size_t            position;
};

static struct mpmc_work mpmc_queue_work(
    struct mpmc_queue *q, 
    at_ssize          *in_or_out, 
    const u32          pos_delta)
{
    struct mpmc_cell *cell;
    ssize position = at_read_explicit(in_or_out, memory_model_relaxed);

    for (;;) {
        cell = &q->buffer[position & q->buffer_mask];

        ssize sequence = at_read_explicit(&cell->sequence, memory_model_acquire);
        sptr difference = (sptr)sequence - (sptr)(position + pos_delta);

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

static s32 mpmc_enqueue(
    struct mpmc_queue *q, 
    struct ripntear   *rip)
{
    struct mpmc_work work = mpmc_queue_work(q, &q->enqueue_pos, 0); 

    if (work.cell) {
        work.cell->rip = *rip;
        at_store_explicit(&work.cell->sequence, work.position + 1, memory_model_release);
        return 1;
    }
    return 0;
}

static s32 mpmc_dequeue(
    struct mpmc_queue *q,
    struct ripntear   *rip)
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
    at_ssize       *wait_counter;

    RIVEN_VALGRIND_ID;
};

enum tls_flags {
    TLS_IN_USE  = 0,
    TLS_TO_FREE = 0x40000000,
    TLS_TO_WAIT = 0x80000000,
    TLS_MASK    = ~(TLS_TO_FREE | TLS_TO_WAIT),
};

struct tls {
    rivens_rift    *riven;
    fcontext_t      home_context;
    uint32_t        fiber_in_use;
    uint32_t        fiber_old;
};

struct entrypoint_data {
    rivens_rift    *riven;
    PFN_riven_main  procedure;
    void           *argument;
};

struct rivens_rift {
    struct mpmc_queue   queue;
    struct mpmc_cell   *cells;

    struct tls         *tls;
    at_ssize            tls_sync;

    struct fiber       *fibers;
    at_ssize           *waiting;
    at_ssize           *free;
    at_ssize           *locks;

    thread_id          *threads;
    rivens_tear        *ends;

    u8                 *stack;
    ssize               stack_bytes;
    ssize               thread_count;
    ssize               fiber_count;
};

static struct tls *get_thread_local_storage(rivens_rift *riven)
{
    ssize index = bedrock_thread_index(riven->threads, riven->thread_count);
    return &riven->tls[index];
}

static ssize get_free_fiber(rivens_rift *riven)
{
    for (ssize i = 0; i < riven->fiber_count; i++) {
        ssize fiber = at_read_explicit(&riven->free[i], memory_model_relaxed);

        if (fiber == FIBER_INVALID)
            continue;

        fiber = at_read_explicit(&riven->free[i], memory_model_acquire);
        if (fiber == FIBER_INVALID)
            continue;

        ssize expected = fiber;

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
    rivens_rift *riven = tls->riven;

    if (tls->fiber_old == (u32)FIBER_INVALID)
        return;

    const ssize fiber_idx = tls->fiber_old & TLS_MASK;

    if (tls->fiber_old & TLS_TO_FREE) {
        RIVEN_VALGRIND_STACK_DEREGISTER(riven->fibers[fiber_idx]);

        /* thread that added fiber to free list is the same as the one freeing it */
        at_store_explicit(&riven->free[fiber_idx], fiber_idx, memory_model_relaxed);
    }

    if (tls->fiber_old & TLS_TO_WAIT) {
        /* wait threshold needs to be thread synced, hence the need for release */
        at_store_explicit(&riven->waiting[fiber_idx], fiber_idx, memory_model_release);
    }
    tls->fiber_old = (u32)FIBER_INVALID;
}

static void weave(sptr raw_tls);

static ssize get_next_fiber(rivens_rift *riven)
{
    ssize fiber_idx = FIBER_INVALID;

    for (ssize i = 0; i < riven->fiber_count; i++) {
        /* double lock helps CPUs that have a weak memory model 
         * arm should go 2x to 3x faster using double lock if its the bottleneck */
        ssize fiber_waiting = at_read_explicit(&riven->waiting[i], memory_model_relaxed);

        if (fiber_waiting == FIBER_INVALID)
            continue;

        fiber_waiting = at_read_explicit(&riven->waiting[i], memory_model_acquire);

        if (fiber_waiting == FIBER_INVALID)
            continue;

        struct fiber *fiber = &riven->fibers[fiber_waiting];
        ssize         finished = 1;
        at_ssize     *counter = fiber->wait_counter;

        if (counter) {
            ssize left = at_read_explicit(counter, memory_model_relaxed);
            finished = (!left);
        }
        if (!finished)
            continue;

        ssize expected = fiber_waiting;

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

            const ssize bytes = riven->stack_bytes + RIVEN_STACK_GUARD;
            u8         *stack = &riven->stack[(fiber_idx * bytes) + bytes];

            /* make_fcontext requires top of the stack, because it grows down */
            internal_make(&fiber->context, weave, stack, riven->stack_bytes);

            RIVEN_VALGRIND_STACK_REGISTER(fiber, stack, stack - bytes);
        }
    }
    return fiber_idx;
}

static struct tls *next_fiber(rivens_rift *riven, struct tls  *tls, fcontext_t  *context)
{
    at_ssize *wait_counter = NULL;

    if ((tls->fiber_old != (u32)FIBER_INVALID) && (tls->fiber_old & TLS_TO_WAIT)) {
        const ssize  fiber_idx = tls->fiber_old & TLS_MASK;
        struct fiber *fiber = &riven->fibers[fiber_idx];
        wait_counter = fiber->wait_counter;
    }

    for (;;) {
        ssize fiber_idx = get_next_fiber(riven);

        if (fiber_idx != FIBER_INVALID) {
            struct fiber *fiber = &riven->fibers[fiber_idx];
            tls->fiber_in_use = (u32)fiber_idx;
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
            ssize count = at_read_explicit(wait_counter, memory_model_relaxed);

            if (!count) {
                /* tls->fiber_in_use still points to the 'to waitlist' fiber */
                tls->fiber_old = (u32)FIBER_INVALID;
                return tls;
            }
        }
    }
}

static void weave(intptr_t raw_tls)
{
    struct tls *tls = (struct tls *)raw_tls;
    rivens_rift *riven = tls->riven;

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

static at_ssize *get_lock(rivens_rift *riven, ssize initial_value)
{
    for (;;) {
        for (ssize i = 0; i < riven->fiber_count; i++) {
            at_ssize *lock = &riven->locks[i];

            if (at_read_explicit(lock, memory_model_relaxed) == FIBER_INVALID) {
                ssize expected = FIBER_INVALID;

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
    rivens_rift *riven = tls->riven;

    while (!at_read_explicit(&riven->tls_sync, memory_model_acquire)) { /* spin :3 */ }

    tls->fiber_old = (u32)FIBER_INVALID;
    tls = next_fiber(riven, tls, &tls->home_context);

    update_free_and_waiting(tls);

    return NULL;
}

/* quit the work */
static void payday(void *raw_riven)
{
    rivens_rift *riven = (rivens_rift *)raw_riven;
    struct tls *tls = get_thread_local_storage(riven);
    struct fiber *old_fiber = &riven->fibers[tls->fiber_in_use];

    if (tls == &riven->tls[0]) {
        /* i = 1, don't join the main thread... */
        for (ssize i = 1; i < riven->thread_count; i++) {
            bedrock_thread_join(riven->threads[i]); 
        }
    }
    tls->fiber_old = tls->fiber_in_use | TLS_TO_FREE;

    internal_jump(tls, &old_fiber->context, &tls->home_context);
    AMW_UNREACHABLE;
}

static void entrypoint(void *raw_entrypoint_data)
{
    struct entrypoint_data *data = (struct entrypoint_data *)raw_entrypoint_data;
    rivens_rift *riven = data->riven;
    const ssize thread_count = riven->thread_count;

    data->procedure(riven, riven->threads, thread_count, data->argument);

    /* returned from main procedure, tell every thread to <redacted> */
    for (ssize i = 0; i < thread_count; i++) {
        riven->ends[i].procedure = payday;
        riven->ends[i].argument  = (void *)riven;
        riven->ends[i].name      = "riven:payday";
    }

    riven_split_and_unchain(riven, riven->ends, thread_count);
    AMW_UNREACHABLE;
}

AMWAPI void riven_split_and_unchain(rivens_rift *riven, rivens_tear *tears, ssize splits)
{
    rivens_chain chain;
    riven_split(riven, tears, splits, &chain);
    riven_unchain(riven, chain);
}

AMWAPI void riven_split(rivens_rift *riven, rivens_tear *tears, ssize splits, rivens_chain *chain)
{
    at_ssize **counters = chain;
    at_ssize  *to_use = NULL;

    if (counters) {
        *counters = get_lock(riven, splits);
        to_use = *counters;
    }

    for (ssize i = 0; i < splits; i++) {
        struct ripntear rip = { .tear = tears[i], .work_left = to_use };
        while (!mpmc_enqueue(&riven->queue, &rip)) { 
            assertion(!"Failed to queue work for fibers in the multiple-producer multiple-consumer queue."); 
        };
    }
}

AMWAPI void riven_unchain(rivens_rift *riven, rivens_chain chain)
{
    at_ssize *counter = chain;
    ssize wait_value = 0;

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

AMWAPI void riven_chain_exile(rivens_rift *riven, rivens_chain *chain)
{
    at_ssize **counters = chain;
    *counters = get_lock(riven, 1);
}

AMWAPI void riven_unchain_exile(rivens_chain chain)
{
    at_ssize *counter = chain;
    at_store_explicit(counter, 0ul, memory_model_release);
}

AMWAPI ssize riven_unveil_rift(
    void           *riven_memory,
    ssize           fiber_stack_bytes,
    ssize           fiber_count,
    ssize           log_2_tear_count,
    ssize           thread_count,
    PFN_riven_main  main_procedure,
    void           *main_argument)
{
    assert_debug(thread_count > 0);
    assert_debug(fiber_stack_bytes > 1);
    assert_debug(fiber_count >= (thread_count << 1));

    rivens_rift *riven = NULL;
    {
        ssize riven_bytes          = A16(sizeof(rivens_rift));
        ssize cells_count          = 1ull << log_2_tear_count;
        ssize cells_bytes          = A16(sizeof(struct mpmc_cell) * cells_count);
        ssize tls_bytes            = A16(sizeof(struct tls)) * thread_count;
        ssize fibers_bytes         = A16(sizeof(struct fiber) * fiber_count);
        ssize waiting_bytes        = A16(sizeof(ssize) * fiber_count);
        ssize free_bytes           = A16(sizeof(ssize) * fiber_count);
        ssize lock_bytes           = A16(sizeof(ssize) * fiber_count);
        ssize thread_bytes         = A16(sizeof(thread_id) * thread_count);
        ssize end_bytes            = A16(sizeof(rivens_tear) * thread_count);
        ssize stack_bytes          = A16(fiber_stack_bytes);
        ssize stack_heap_bytes     = stack_bytes * fiber_count;
        ssize bytes_alignment      = 15 + (RIVEN_STACK_GUARD);

        stack_heap_bytes += RIVEN_STACK_GUARD * (fiber_count + 1);

        ssize total_bytes = 
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

        riven = (rivens_rift *)A16((sptr)riven_memory);
        { /* zero */
            u8 *z = (u8 *)riven;
            for (ssize i = 0; i < total_bytes; i++) {
                z[i] = 0;
            }
        }

        ssize o = riven_bytes;
        u8 *raw = (u8 *)riven;

        riven->cells   = (struct mpmc_cell *)   &raw[o]; o += cells_bytes;
        riven->tls     = (struct tls *)         &raw[o]; o += tls_bytes;
        riven->fibers  = (struct fiber *)       &raw[o]; o += fibers_bytes;
        riven->waiting = (ssize *)              &raw[o]; o += waiting_bytes;
        riven->free    = (ssize *)              &raw[o]; o += free_bytes;
        riven->locks   = (ssize *)              &raw[o]; o += lock_bytes;
        riven->threads = (thread_id *)          &raw[o]; o += thread_bytes;
        riven->ends    = (rivens_tear *)        &raw[o]; o += end_bytes;
#if RIVEN_STACK_GUARD > 0
        riven->stack = (u8 *)((((sptr)&raw[o]) + (RIVEN_STACK_GUARD - 1)) & ~(RIVEN_STACK_GUARD - 1));
#else
        riven->stack = (u8 *)&raw[o];
#endif /* RIVEN_STACK_GUARD */
        riven->stack_bytes = stack_bytes;
        riven->thread_count = thread_count;
        riven->fiber_count = fiber_count;

        assert_debug(!(((sptr)riven->cells  ) & 15));
        assert_debug(!(((sptr)riven->tls    ) & 15));
        assert_debug(!(((sptr)riven->fibers ) & 15));
        assert_debug(!(((sptr)riven->waiting) & 15));
        assert_debug(!(((sptr)riven->free   ) & 15));
        assert_debug(!(((sptr)riven->locks  ) & 15));
        assert_debug(!(((sptr)riven->threads) & 15));
        assert_debug(!(((sptr)riven->ends   ) & 15));
        assert_debug(!(((sptr)riven->stack  ) & 15));
    }
    mpmc_queue_init(&riven->queue, riven->cells, 1ull << log_2_tear_count);

    for (ssize i = 0; i <= riven->fiber_count; i++) {
        stack_memory_guard(&riven->stack[i * (riven->stack_bytes + RIVEN_STACK_GUARD)], RIVEN_STACK_GUARD);
    }

    for (ssize i = 0; i < riven->fiber_count; i++) {
        at_store_explicit(&riven->free[i], i, memory_model_release);
        at_store_explicit(&riven->waiting[i], FIBER_INVALID, memory_model_release);
        at_store_explicit(&riven->locks[i], FIBER_INVALID, memory_model_release);
    }

    { /* setup main thread */
        riven->tls[0].riven        = riven;
        riven->tls[0].fiber_in_use = (u32)get_free_fiber(riven);
        riven->threads[0]          = bedrock_thread_current();
    }
    at_store_explicit(&riven->tls_sync, (ssize)0, memory_model_release);

    for (ssize i = 1; i < thread_count; i++) {
        struct tls *tls = &riven->tls[i];
        tls->riven = riven;
        tls->fiber_in_use = (u32)FIBER_INVALID;
        bedrock_thread_create(&riven->threads[i], do_the_work, (void *)tls);
    }
    at_store_explicit(&riven->tls_sync, (ssize)1, memory_model_release);

    bedrock_thread_affinity(&riven->stack[RIVEN_STACK_GUARD], riven->threads, thread_count, 0);

    struct entrypoint_data main_data = {
        .riven = riven,
        .procedure = main_procedure,
        .argument = main_argument,
    };

    rivens_tear main_tear = {
        .procedure = entrypoint,
        .argument = (void *)&main_data,
        .name = "riven:entrypoint",
    };

    riven_split(riven, &main_tear, 1, NULL);
    do_the_work((void *)&riven->tls[0]);

    for (ssize i = 0; i <= riven->fiber_count; i++) {
        stack_memory_stop(&riven->stack[i * (riven->stack_bytes + RIVEN_STACK_GUARD)], RIVEN_STACK_GUARD);
    }

    return 0;
}
