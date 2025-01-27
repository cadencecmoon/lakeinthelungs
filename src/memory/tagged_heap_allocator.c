#include "tagged_heap_allocator.h"

#ifndef TAGGED_HEAP_DISABLE_INTRINSICS
#include "../bedrock/intrinsics.h"

    /* The article on fast population-count by Wojciech Muła, and his github repository:
     * https://github.com/WojciechMula/sse-popcount/tree/master,
     * http://0x80.pl/notesen/2008-05-24-sse-popcount.html
     * were very helpful in implementing this. */
    #if !defined(ARCH_X86_AVX2) && !defined(ARCH_X86_SSSE3) 
     && !defined(ARCH_X86_SSE2) && !defined(ARCH_ARM_NEON) 
     && !defined(ARCH_RISCV_ZVE32X) && !defined(ARCH_RISCV_ZVE64X)
    static const u8 free_lookup_table[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };
    #endif
#endif

/** Counts the total set bits in the free bitmap. */
static u32 attr_hot attr_nonnull_all 
batch_count_free_blocks(at_u64 *free_bitmap, usize block_count)
{
    u32 count = 0;
    usize bitmap_count = 1 + (block_count - 1) / 64;

    assert_debug(bitmap_count > 0 && bitmap_count <= 8);

#ifndef TAGGED_HEAP_DISABLE_INTRINSICS
    #if defined(ARCH_X86_AVX2)
    for (usize idx = 0; idx < bitmap_count; idx += 4) {
    }

    #elif defined(ARCH_X86_SSSE3)

    #elif defined(ARCH_X86_SSE)
    for (usize idx = 0; idx < bitmap_count; idx += 2) {
    }

    #elif defined(ARCH_ARM_NEON)
    for (usize idx = 0; idx < bitmap_count; idx += 2) {
        /* TODO Neon */
    }

    #elif defined(ARCH_RISCV_ZVE32X) || defined(ARCH_RISCV_ZVE64X)
    for (usize idx = 0; idx < bitmap_count; idx += 2) {
        /* TODO RISC-V */
    }

    #else /*  no intrinsics available, fallback to a free lookup table */
    for (usize idx = 0; idx < bitmap_count; idx++) {
        u64 bitmap = at_read_relaxed(&free_bitmap[0]);

        count += free_lookup_table[bitmap & 0xFF] +
                 free_lookup_table[(bitmap >> 8) & 0xFF] +
                 free_lookup_table[(bitmap >> 16) & 0xFF] +
                 free_lookup_table[(bitmap >> 24) & 0xFF] +
                 free_lookup_table[(bitmap >> 32) & 0xFF] +
                 free_lookup_table[(bitmap >> 40) & 0xFF] +
                 free_lookup_table[(bitmap >> 48) & 0xFF] +
                 free_lookup_table[(bitmap >> 56) & 0xFF];
    }
    #endif
#else
    for (usize idx = 0; idx < 1 + (block_count - 1) / 64; idx++) {
        u64 bitmap = at_read_relaxed(&free_bitmap[idx]);

        if (bitmap == 0llu) continue;

    #if HAS_BUILTIN(__builtin_popcountll)
        count += __builtin_popcountll(bitmap);
    #elif defined(CC_MSVC_VERSION)
        count += __popcnt64(bitmap);
    #else 
        while (bitmap) {
            bitmap &= (bitmap - 1);
            count++;
        }
    #endif
    }
#endif
    return count;
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
}

s32 thmalloc(
    struct tagged_heap_allocator *th, 
    u32                           heap_tag, 
    u32                           reserve_2mb_count, 
    struct allocation            *dst) 
{

}

void *throots(
    struct tagged_heap_allocator *th, 
    usize                         size, 
    usize                         alignment) 
{

}

void thfree(
    struct tagged_heap_allocator *th, 
    u32                           tag) 
{

}
