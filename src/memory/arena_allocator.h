#pragma once

#include "../bedrock.h"

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A memory region available to the arena allocator, with information needed 
 *  to perform suballocations. */
struct arena_page {
    struct arena_page *next;    /**< Next page available to the allocator from a linked list. */
    rivens_chain_t     chain;   /**< This chain will block with a fiber context switch, ignored if riven is NULL. */
    usize              offset;  /**< Currently suballocated space, offset from the address of *data. */
    usize              size;    /**< Space available to the arena in this allocation. */
    void              *data;    /**< The beginning of the memory. */
};

/** Saves the offset of an arena page, so we can rewind to it later instead of resetting the arena. */
struct arena_scratch {
    struct arena_page *page;    /**< The arena page being scratched. */
    usize              offset;  /**< The saved offset we can rewind to later. */
};

/** An arena enables allocating memory in a pool, that can be freed all at once
 *  and reused with new resources, a good fit for temporary allocations of any 
 *  size in range with the size of a given arena_page. */
struct arena_allocator {
    struct arena_page  head;    /**< The first page that belongs to this arena. */
    struct arena_page *tail;    /**< The last page in the linked list of head->next, or points to head. */
    struct riven      *riven;   /**< Mostly used for synchronization, can be NULL to reduce overhead but then must be externally synchronized. */
};

#ifdef __cplusplus
}
#endif
