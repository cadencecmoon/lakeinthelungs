#include "tagged_heap_allocator.h"

#include "../math/popcnt.h"

/** Counts the total set bits in the free bitmap. */
static u32 attr_hot attr_nonnull_all 
count_free_blocks_in_range(const at_u64 *bitmap, const u32 block_count)
{
    const u8 *bitmap_data = (const u8 *)bitmap;
    const usize bitmap_size = sizeof(u64) * (block_count >> 6); /* block_count/64 */
    return (u32)popcnt_lookup(bitmap_data, bitmap_size);
}

s32 thinit(
    struct tagged_heap_allocator *th,
    usize                         expected_root_usage)
{
    return 0;
}

void thinitlate(
    struct tagged_heap_allocator *th,
    struct riven                 *riven,
    thread_t                     *threads,
    ssize                         thread_count)
{
    assert_debug(!th->riven);

    th->riven = riven;
    th->tears.threads = threads;
    th->tears.thread_count = thread_count;

    th->tears.offsets = (usize *)throots(th, sizeof(usize) * thread_count, ALIGNOF(usize));
    
    /* reserve one 2mb block per thread in the system */
    struct allocation alloc = {0};
    thmalloc(th, heap_tag_tears__, thread_count, &alloc);

    th->tears.data = alloc.data;
    for (s32 i = 0; i < thread_count; i++) {
        th->tears.offsets[i] = 0lu;
    }
}

s32 thgrow(struct tagged_heap_allocator *th)
{
    return 0;
}

s32 thtrim(struct tagged_heap_allocator *th)
{
    return 0;
}

void thfini(struct tagged_heap_allocator *th)
{

}

s32 thmalloc(
    struct tagged_heap_allocator *th, 
    u16                           tag, 
    u32                           reserve_2mb_count, 
    struct allocation            *dst) 
{
    return 0;
}

void *throots(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
{
    assume(!(alignment & (alignment - 1)));

    void *data = allocate_aligned_synchronized(&th->roots.tail->alloc, size, alignment, th->riven);
    if (data) return data;

    /* the current root block may not satisfy this allocation */
    /* TODO malloc new root block */

    return data;
}

void *thtears(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
{
    assert_debug(at_read_relaxed(&th->flags) & (tagged_heap_flag_is_valid | tagged_heap_flag_parallel_workflow));

    assume(!(alignment & (alignment - 1)));
    assume(size < TAGGED_HEAP_BLOCK_SIZE);

    usize thread_idx;
    usize aligned_offset;
    usize memory_offset;
    void *memory;

    /* find the correct memory block */
    thread_idx = bedrock_thread_index(th->tears.threads, th->tears.thread_count);
    memory_offset = TAGGED_HEAP_BLOCK_SIZE * thread_idx;
    memory = &th->tears.data + memory_offset;

    /* calculate the next memory address */
    aligned_offset = (th->tears.offsets[thread_idx] + (alignment - 1)) & ~(alignment - 1);

    /* wrap the tears block if not enough space is left */
    if (aligned_offset + size >= TAGGED_HEAP_BLOCK_SIZE + memory_offset) {
#ifdef NDEBUG
        /* silently wrap the page */
        aligned_offset = memory_offset;
#else
        if (at_read_relaxed(&th->flags) & tagged_heap_wrap_tear_pages) {
            aligned_offset = memory_offset;
            log_warn("Wrapped the tear page for thread idx %lu to the beginning.", thread_idx);
        } else {
            log_error("Tried to wrap the tear page for thread idx %lu, but this is asserted.", thread_idx);
            debugtrap();
        }
#endif
    }

    th->tears.offsets[thread_idx] = aligned_offset + size;
    return (void *)((uptr)memory + aligned_offset);
}

void thfree(
    struct tagged_heap_allocator *th, 
    u16                           tag) 
{
    switch (tag) { 
        /* noop for now */
        case heap_tag_free__:
        case heap_tag_roots__: 
            return;

        /* a simple reset to the per-thread memory pools is sufficient */
        case heap_tag_tears__:
            for (ssize i = 0; i < th->tears.thread_count; i++)
                th->tears.offsets[i] = 0lu;
            return;

        /* any other tag must be handled accordingly */
        default: break;
    }

    /* TODO */
}
