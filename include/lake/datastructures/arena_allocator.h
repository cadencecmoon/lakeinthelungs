#ifndef _AMW_ARENA_ALLOCATOR_H
#define _AMW_ARENA_ALLOCATOR_H

#include <lake/bedrock/defines.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/parser.h>
#include <lake/ipomoea.h>

#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy, strdup */

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME use the tagged heap as a memory backend instead. */
struct region {
    AMW_ATOMIC(struct region *) next; /* next region */
    at_size_t count;      /* 1 = 8 bytes (64 bits) */
    at_size_t capacity;   /* 1 = 1 byte  (8 bits) */
    uintptr_t data[];
};

struct arena_allocator {
    struct region *begin;
    struct region *end;
};

AMW_INLINE struct region *region_new(size_t capacity) 
{
    /* TODO use tagged heap as a backend */
    size_t bytes = sizeof(struct region) + sizeof(uintptr_t) * capacity;
    struct region *r = (struct region *)malloc(bytes);
    assert_debug(r != NULL);

    at_store_relaxed(&r->next, NULL);
    at_store_relaxed(&r->count, 0);
    at_store_relaxed(&r->capacity, capacity);
    return r;
}

AMW_INLINE void region_free(struct region *r) 
{
    /* TODO use tagged heap as a backend */
    free(r);
}

/** The arena allocator is implied to be empty, probably allocated on the stack and not zeroed out yet.
 *  Preallocates a memory region with of a size atleast given bytes. */
AMW_INLINE void arena_init(struct arena_allocator *a, size_t bytes) 
{
    if (bytes < 4096) 
        bytes = 4096;
    struct region *r = region_new(bytes);
    a->begin = a->end = r;
}

/** If the arena has NULL begin/end regions, a new region of a minimum 4096 bytes size will be created. */
AMW_INLINE void *arena_alloc(struct arena_allocator *a, size_t bytes) 
{
    size_t size = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
    if (a->end == NULL) {
        assert_debug(a->begin == NULL);
        size_t capacity = 4096;
        if (capacity < size) capacity = size;
        a->begin = a->end = region_new(capacity);
    }
    while (at_read_explicit(&a->end->count, memory_model_acquire) + size > at_read_relaxed(&a->end->capacity) 
            && at_read_relaxed(&a->end->next) != NULL)
        a->end = at_read_explicit(&a->end->next, memory_model_seq_cst);

    if (at_read_explicit(&a->end->count, memory_model_acquire) + size > at_read_relaxed(&a->end->capacity)) {
        assert_debug(at_read_relaxed(&a->end->next) != NULL);
        size_t capacity = 4096;
        if (capacity < size) capacity = size;
        at_store_explicit(&a->end->next, region_new(capacity), memory_model_seq_cst);
    }
    void *result = &a->end->data[at_read_explicit(&a->end->count, memory_model_acquire)];
    at_fetch_add_explicit(&a->end->count, size, memory_model_acq_rel);

    return result;
}

/** Copies over data to newly allocated memory in the given arena. */
AMW_INLINE void *arena_memdup(struct arena_allocator *a, void *data, size_t bytes) 
{
    return memcpy(arena_alloc(a, bytes), data, bytes);
}

AMW_INLINE char *arena_strndup(struct arena_allocator *a, const char *str, size_t n) 
{
    char *dup;
    size_t len;

    if (str == NULL) return NULL;

    len = parse_strnlen(str, n);
    dup = (char *)arena_alloc(a, len + 1);
    memcpy(dup, str, (size_t)(len));
    dup[len] = '\0';
    return dup;
}

/** Resets the memory regions of an arena. */
AMW_INLINE void arena_reset(struct arena_allocator *a) 
{
    for (struct region *r = a->begin; r != NULL; r = at_read_explicit(&r->next, memory_model_acq_rel))
        at_store_explicit(&r->count, 0, memory_model_acq_rel);
    a->end = a->begin;
}

AMW_INLINE void arena_fini(struct arena_allocator *a) 
{
    struct region *r0 = a->begin;
    while (r0) {
        struct region *r1 = r0;
        r0 = at_read_relaxed(&r0->next);
        region_free(r1);
    }
    a->begin = NULL;
    a->end = NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMW_ARENA_ALLOCATOR_H */
