/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/ipomoea.h>

i32 iainit(struct ipomoea *ia, thread_t *threads, size_t thread_count)
{
    (void)ia;
    (void)threads;
    (void)thread_count;
    return 0;
}

void iafini(struct ipomoea *ia)
{
    (void)ia;
}

void *ialloc(struct ipomoea *ia, size_t n, size_t alignment, u64 tag)
{
    (void)ia;
    (void)n;
    (void)alignment;
    (void)tag;
    return NULL;
}

void iafree(struct ipomoea *ia, u64 tag)
{
    (void)ia;
    (void)tag;
}
