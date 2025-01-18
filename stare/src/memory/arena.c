#include <lake/datastructures/arena_allocator.h>

#include <lake/bedrock/assert.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/parser.h>

#include <stdlib.h> /* malloc, free */
#include <string.h> /* strdup */

/* TODO use tagged heap as a backend */
AMWAPI struct region *region_new(usize capacity)
{
    usize bytes = sizeof(struct region) + sizeof(uptr) * capacity;
    struct region *r = (struct region *)malloc(bytes);
    assert_debug(r != NULL);

#ifdef AMW_DEBUG
    log_debug("Allocated a memory region of size %lu + %lu bytes", sizeof(struct region), capacity);
#endif

    at_store_relaxed(&r->next, NULL);
    at_store_relaxed(&r->count, 0);
    at_store_relaxed(&r->capacity, capacity);
    return r;
}

/* TODO use tagged heap as a backend */
AMWAPI void region_free(struct region *r) 
{
#ifdef AMW_DEBUG
    log_debug("Freeing a memory region of size %lu + %lu bytes", sizeof(struct region), at_read_relaxed(&r->capacity));
#endif
    free(r);
}

AMWAPI void arena_init(struct arena_allocator *a, usize bytes) 
{
    assert_debug(a);
    if (a->end)
        return;

    struct region *r = region_new(bytes);
    a->begin = a->end = r;
}

AMWAPI void *arena_alloc(struct arena_allocator *a, usize bytes) 
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

AMWAPI void arena_reset(struct arena_allocator *a) 
{
    for (struct region *r = a->begin; r != NULL; r = at_read_explicit(&r->next, memory_model_acq_rel))
        at_store_explicit(&r->count, 0, memory_model_acq_rel);
    a->end = a->begin;
}

AMWAPI char *arena_strndup(struct arena_allocator *a, const char *str, usize n)
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

AMWAPI void arena_fini(struct arena_allocator *a) 
{
    struct region *r0 = a->begin;
    while (r0) {
        struct region *r1 = r0;
        r0 = at_read_relaxed(&r0->next);
        region_free(r1);
    }
    a->begin = a->end = NULL;
}
