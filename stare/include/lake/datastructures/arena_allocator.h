/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_ARENA_ALLOCATOR_H
#define _AMW_ARENA_ALLOCATOR_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/ipomoea.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME use the tagged heap as a memory backend instead. */
struct region {
    AMW_ATOMIC(struct region *) next; /* next region */
    at_usize count;      /* 1 = 8 bytes (64 bits) */
    at_usize capacity;   /* 1 = 1 byte  (8 bits) */
    uptr     data[];
};

struct arena_allocator {
    struct region *begin;
    struct region *end;
};

/** Allocates a new memory region (TODO use ipomoeaalba later) */
AMWAPI struct region *region_new(usize capacity);

/** Frees a memory region (TODO use ipomoeaalba later) */
AMWAPI void region_free(struct region *r);

/** The arena allocator is implied to be empty, probably allocated on the stack and not zeroed out yet.
 *  Preallocates a memory region with of a size atleast given bytes. */
AMWAPI void arena_init(struct arena_allocator *a, usize bytes);

/** If the arena has NULL begin/end regions, a new region of a minimum 4096 bytes size will be created. */
AMWAPI void *arena_alloc(struct arena_allocator *a, usize bytes);

/** Copies over data to newly allocated memory in the given arena. */
AMW_INLINE void *arena_memdup(struct arena_allocator *a, void *data, usize bytes) {
    return iamemcpy(arena_alloc(a, bytes), data, bytes);
}

/** Resets the memory regions of an arena. */
AMWAPI void arena_reset(struct arena_allocator *a);

/** Duplicates a string of a maximum N character length. */
AMWAPI char *arena_strndup(struct arena_allocator *a, const char *str, usize n);

/** Frees memory regions bound to the arena. */
AMWAPI void arena_fini(struct arena_allocator *a);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ARENA_ALLOCATOR_H */
