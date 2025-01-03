#ifndef _AMW_ARENA_ALLOCATOR_H
#define _AMW_ARENA_ALLOCATOR_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/parser.h>
#include <lake/ipomoeaalba.h> /* TODO */

#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy, strdup */

#ifdef __cplusplus
extern "C" {
#endif

/* TODO when i'll be using ipomoeaalba.h as the memory backend, the functions can't be inline anymore,
 * or else with AMW_NO_PROTOTYPES and dynamically loading the engine library, the allocator will fail.
 * For now it's fine, it will take me some time to figure out stuff. */

/* FIXME use the tagged heap as a memory backend instead. */
typedef struct region region;

struct region {
    AMW_ATOMIC(region *) next; /* next region */
    at_usize count;      /* 1 = 8 bytes (64 bits) */
    at_usize capacity;   /* 1 = 1 byte  (8 bits) */
    uptr data[];
};

typedef struct arena_allocator {
    region *begin;
    region *end;
} arena_allocator;

/* TODO use tagged heap as a backend */
AMW_INLINE region *region_new(usize capacity) 
{
    usize bytes = sizeof(region) + sizeof(uptr) * capacity;
    region *r = (region *)malloc(bytes);
    assert_debug(r != NULL);

#ifdef AMW_DEBUG
    log_debug("Allocated a memory region of size %lu + %lu bytes", sizeof(region), capacity);
#endif

    at_store_relaxed(&r->next, NULL);
    at_store_relaxed(&r->count, 0);
    at_store_relaxed(&r->capacity, capacity);
    return r;
}

/* TODO use tagged heap as a backend */
AMW_INLINE void region_free(region *r) 
{
#ifdef AMW_DEBUG
    log_debug("Freeing a memory region of size %lu + %lu bytes", sizeof(region), at_read_relaxed(&r->capacity));
#endif
    free(r);
}

/** The arena allocator is implied to be empty, probably allocated on the stack and not zeroed out yet.
 *  Preallocates a memory region with of a size atleast given bytes. */
AMW_INLINE void arena_init(arena_allocator *a, usize bytes) 
{
    assert_debug(a);

    if (bytes < 4096) 
        bytes = 4096;

    region *r = region_new(bytes);
    a->begin = a->end = r;
}

/** If the arena has NULL begin/end regions, a new region of a minimum 4096 bytes size will be created. */
AMW_INLINE void *arena_alloc(arena_allocator *a, size_t bytes) 
{
    usize size = (bytes + sizeof(uptr) - 1) / sizeof(uptr);
    if (a->end == NULL) {
        assert_debug(a->begin == NULL);
        usize capacity = 4096;
        if (capacity < size) capacity = size;
        a->begin = a->end = region_new(capacity);
    }
    while (at_read_explicit(&a->end->count, memory_model_acquire) + size > at_read_relaxed(&a->end->capacity) 
            && at_read_relaxed(&a->end->next) != NULL)
        a->end = at_read_explicit(&a->end->next, memory_model_seq_cst);

    if (at_read_explicit(&a->end->count, memory_model_acquire) + size > at_read_relaxed(&a->end->capacity)) {
        assert_debug(at_read_relaxed(&a->end->next) != NULL);
        usize capacity = 4096;
        if (capacity < size) capacity = size;
        at_store_explicit(&a->end->next, region_new(capacity), memory_model_seq_cst);
    }
    void *result = &a->end->data[at_read_explicit(&a->end->count, memory_model_acquire)];
    at_fetch_add_explicit(&a->end->count, size, memory_model_acq_rel);

    return result;
}

/** Copies over data to newly allocated memory in the given arena. */
AMW_INLINE void *arena_memdup(arena_allocator *a, void *data, usize bytes) 
{
    return memcpy(arena_alloc(a, bytes), data, bytes);
}

AMW_INLINE char *arena_strndup(arena_allocator *a, const char *str, usize n) 
{
    char *dup;
    usize len;

    if (str == NULL) return NULL;

    len = parser_strnlen(str, n);
    dup = (char *)arena_alloc(a, len + 1);
    memcpy(dup, str, (usize)(len));
    dup[len] = '\0';
    return dup;
}

/** Resets the memory regions of an arena. */
AMW_INLINE void arena_reset(arena_allocator *a) 
{
    for (region *r = a->begin; r != NULL; r = at_read_explicit(&r->next, memory_model_acq_rel))
        at_store_explicit(&r->count, 0, memory_model_acq_rel);
    a->end = a->begin;
}

AMW_INLINE void arena_fini(arena_allocator *a) 
{
    region *r0 = a->begin;
    while (r0) {
        region *r1 = r0;
        r0 = at_read_relaxed(&r0->next);
        region_free(r1);
    }
    a->begin = a->end = NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ARENA_ALLOCATOR_H */
