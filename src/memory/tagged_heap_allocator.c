#include "tagged_heap_allocator.h"

#include "../math/popcnt.h"

/** Counts the total set bits in the free bitmap. */
static u32 attr_hot attr_nonnull_all 
batch_count_free_blocks(const at_u64 *free_bitmap, const usize block_count)
{
    assert_debug(block_count <= 512 && "A maximum of 512 blocks per tagged heap batch is supported.");

    const u8 *bitmap_data = (const u8 *)free_bitmap;
    const usize bitmap_size = sizeof(u64) * (block_count > 256 ? 4 : 8);
    return popcnt_lookup(bitmap_data, bitmap_size);
}

/** Seeks for N free blocks of contiguous memory, that will satisfy our allocation.
 *  Gives the bit position that can be acquired by using it in a bit shift.
 *  If -1 is returned, no such reservation is possible from this batch. */
static ssize attr_hot attr_nonnull_all 
batch_seek_free_head(struct tagged_heap_batch *batch, usize request_block_count)
{
    usize pos;

    for (usize idx = 0; idx < 1 + (batch->block_count - 1) / 64; idx++) {

    }
    return 0;
}

s32 thmalloc(
    struct tagged_heap_allocator *th, 
    u32                           heap_tag, 
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
    return NULL;
}

void thfree(
    struct tagged_heap_allocator *th, 
    u32                           tag) 
{

}
