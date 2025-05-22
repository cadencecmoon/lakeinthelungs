#include "log.h"
#include "job_system.h"
#include "tagged_heap.h"

/** Thread-local storage. */
struct tls;

/** A fiber context, it holds saved CPU registers and pointers required to perform a CPU context switch. */
typedef void *fcontext;

enum tls_flags : u32 {
    tls_in_use = 0,
    tls_to_free = 0x40000000,
    tls_to_wait = 0x80000000,
    tls_mask    = ~(tls_to_free | tls_to_wait),
};

/** Implemented in assembly, ofc/nfc - original/new fiber context, preserve_fpu - procedure. */
LAKE_HOT_FN
extern sptr jump_fcontext(fcontext *ofc, fcontext nfc, sptr vp, s32 preserve_fpu);

/** Implemented in assembly, sp - top of stack pointer. */
LAKE_HOT_FN
extern fcontext make_fcontext(void *sp, usize size, void (*fn)(sptr));

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
sptr jump_fiber_context(struct tls *tls, fcontext *from, fcontext *to)
{
    lake_san_assert(*from != *to, LAKE_INVALID_PARAMETERS, "Can't switch fiber context to itself.");
    return jump_fcontext(from, *to, (sptr)tls, 1);
}

LAKE_FORCE_INLINE LAKE_NONNULL_ALL
void make_fiber_context(fcontext *context, void (*procedure)(sptr), void *stack, usize stack_bytes)
{
    *context = make_fcontext(stack, stack_bytes, procedure);
}

struct jezioro {

};
static struct jezioro *g_lake;

void lake_submit_work(u32 work_count, lake_work_details const *work, lake_work_chain *out_chain)
{

}

void lake_yield(lake_work_chain chain)
{

}

u32 lake_worker_thread_index(void)
{

}

lake_result lake_thalloc_heap(lake_heap_tag tag, lake_heap_attr attr, usize request, lake_heap_memory *out_memory)
{

}

lake_result lake_thfree_heap(lake_heap_tag tag)
{

}

lake_result lake_thadvise_heap(usize request, lake_madvise_mode mode)
{

}

void lake_log_flush(bool yield)
{

}

void lake_log_(
    s32         level, 
    char const *fn, 
    char const *file, 
    s32         line, 
    char const *fmt, ...)
{

}

s32 lake_log_set_level(s32 level)
{

}

lake_assert_status lake_assert_log_(
    s32         error_code,
    char const *condition,
    char const *fn,
    char const *file,
    s32         line,
    char const *fmt, ...)
{

}

s32 lake_log_get_level(void)
{

}

bool lake_log_enable_colors(bool enabled)
{

}

bool lake_log_enable_timestamp(bool enabled)
{

}

bool lake_log_enable_threading(bool enabled)
{

}

void lake_exit_status(s32 error_code)
{

}

void lake_abort_(
    s32         error_code, 
    char const *fn, 
    char const *file, 
    s32         line)
{

}

FN_LAKE_WORK(lake_assembly, lake_assembly_work const *work)
{

}

lake_result a_moonlit_walk(
    lake_app_desc     *app_desc,
    PFN_lake_framework entrypoint,
    void              *userdata)
{

}
