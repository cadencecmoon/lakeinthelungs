#pragma once

#include "../bedrock.h"

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A page of memory used in the stack allocator. */
struct stack_page {
    struct stack_page      *next;           /**< Next page available to the stack allocator. */
    rivens_chain_t          chain;          /**< This chain will block with a fiber context switch, ignored if riven is NULL. */
    s16                     sp;             /**< A saved stack pointer, corresponds to the cursor index. */
    u32                     id;             /**< An unique page identifier. */
    void                   *data;           /**< The beginning of memory. */
};

/** Points to an suballocation. If cursor is not the last on the stack, no memory should be freed.
 *  We can iterate through freed cursors to know how much memory we can free. */
struct stack_cursor {
    struct stack_cursor    *prev;           /**< Previous stack cursor. */
    struct stack_page      *page;           /**< Page it's relevant to. */
    s16                     sp;             /**< A stack pointer. */
    at_b32                  is_free;        /**< True if the cursor is unused. */
#ifdef DEBUG
    struct stack_allocator *owner;          /**< The allocator that created this cursor. */
#endif
};

/** The allocator enables pushing and popping values to a stack, and has a lower overhead
 *  when compared to a block allocator. It's a good fit for small temporary allocations. */
struct stack_allocator {
    struct stack_page       head;           /**< The first page that belongs to this stack. */
    struct stack_page      *tail_page;      /**< The last stack page in the linked list. */
    struct stack_cursor    *tail_cursor;    /**< The last cursor in the linked list. */
#ifdef DEBUG
    at_s32                  cursor_count;   /**< How many cursors are inside the *prev linked list above. */
#endif
    struct riven           *riven;          /**< Mostly used for synchronization, can be NULL to reduce overhead but then must be externally synchronized. */
};

#ifdef __cplusplus
}
#endif
