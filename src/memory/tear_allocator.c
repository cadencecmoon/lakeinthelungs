#include "tear_allocator.h"

void *tear_alloc_aligned(
    struct tear_allocator *tear, 
    usize                  size, 
    usize                  alignment)
{
    ssize idx;
    usize aligned_offset;
    struct tear_page *page;

    if (alignment & (alignment - 1)) {
        assume(!(alignment & (alignment - 1)));
        alignment = 8; /* just pick a common alignment then */
    }

    idx = bedrock_thread_index(tear->threads, tear->thread_count);
    page = &tear->pages[idx];

    /* calculate the next memory address */
    aligned_offset = (page->offset + (alignment - 1)) & ~(alignment - 1);

    assert_debug(size < page->size);

    /* if there is not enough space, wrap the page */
    if (aligned_offset + size > page->size) {
        aligned_offset = 0;
    }

    /* :) */
    void *data = (void *)((uptr)page->data + aligned_offset);
    page->offset = aligned_offset + size;

    return data;
}

void tear_free(struct tear_allocator *tear)
{
    for (ssize i = 0; i < tear->thread_count; i++)
        tear->pages[i].offset = 0lu;
}
