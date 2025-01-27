#pragma once

#include "../bedrock.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A memory page used within the block allocator. */
struct block_page {
    struct block_page *next;    /**< Next page available to the block allocator. */
    rivens_chain_t     chain;   /**< A synchronization primitive owned by the page. */
    void              *data;    /**< The beginning of memory */
};

/** Metadata used to divide block pages into smaller chunks. */
struct block_chunk_header {
    struct block_chunk_header *next;    /**< Next chunk. */
};

/** The allocator returns memory of a fixed size. 
 *  This size can be set when creating the block allocator. */
struct block_allocator {
    struct block_chunk_header  *head;               /**< The beginning of the chunk headers. */
    struct block_page          *block_head;         /**< The beginning of the memory blocks. */
    struct block_page          *block_tail;         /**< The last memory block. */
    s32                         chunk_size;         /**< Blocks are divided into chunks of this fixed size. */
    s32                         data_size;          /**< The total memory available in the allocator. */
    s32                         chunks_per_block;   /**< How many chunks fit into a single block page. */
    s32                         block_size;         /**< The fixed size of every block page. */
#ifdef DEBUG
    at_s32                      alloc_count;        /**< How many allocations were done. */
#endif
};

#ifdef __cplusplus
}
#endif
