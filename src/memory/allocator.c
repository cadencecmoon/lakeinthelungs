#include "allocator.h"

#include <string.h>

void *allocate_aligned_synchronized(
    struct allocation *alloc, 
    usize              size, 
    usize              alignment, 
    struct riven      *riven)
{
    assume(!(alignment & (alignment - 1)));

    usize aligned_offset = (alloc->offset + (alignment - 1)) & ~(alignment - 1);

    if (size > alloc->size || aligned_offset + size > alloc->size) {
        log_error("Allocation failed: %lu bytes requested, structure holds a total %lu/%lu bytes reserved.", size, alloc->offset, alloc->size);
        return NULL;
    }

    if (!riven) {
        /* no synchronization needed */
        void *data = (void *)((uptr)alloc->data + aligned_offset);
        alloc->offset = aligned_offset + size;
        return data;
    }

    riven_unchain(riven, alloc->chain); /* if the chain is neither NULL nor 0, this will context swap */
    riven_acquire_chain(riven, &alloc->chain);

    void *data = (void *)((uptr)alloc->data + aligned_offset);
    alloc->offset = aligned_offset + size;

    riven_release_chain(alloc->chain);
    return data;
}

void *allocate_cstr(
    struct allocation *alloc, 
    const char        *cstr, 
    struct riven      *riven) 
{
    usize n = strlen(cstr);
    char *dup = (char *)allocate_synchronized(alloc, n, riven);
    bedrock_memcpy(dup, cstr, n);
    dup[n] = '\0';
    return dup;
}
