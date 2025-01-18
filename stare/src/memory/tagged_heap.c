#include <lake/ipomoea.h>

#if defined(AMW_PLATFORM_LINUX)
#include <unistd.h>
#include <sys/mman.h>

/* returns count of allocated pages (blocks) */
static u32 map_memory(void *ptr, usize bytes) 
{
    u32 page_count;
    (void)page_count;
    (void)ptr;
    (void)bytes;
    return 0;
}

static void unmap_memory(struct tagged_heap *heap) 
{
    (void)heap;
}

#elif defined(AMW_PLATFORM_WINDOW)
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#endif

static at_flag spinlock = false;

AMWAPI struct ipomoea *iainit(usize max_memory_budget, usize root_memory_estimate)
{
    (void)max_memory_budget;
    (void)root_memory_estimate;
    (void)spinlock;
    (void)unmap_memory;
    (void)map_memory;
    return NULL;
}

AMWAPI void iafini(struct ipomoea *ia)
{
    (void)ia;
}

AMWAPI s32 iassemble_scratch(struct ipomoea *ia, thread_t *threads, ssize thread_count)
{
    (void)ia;
    (void)threads;
    (void)thread_count;
    return 1;
}

AMWAPI void *ialloc(struct ipomoea *ia, usize size, usize alignment, u64 tag)
{
    (void)ia;
    (void)size;
    (void)alignment;
    (void)tag;
    return NULL;
}

AMWAPI void *iascratch(struct ipomoea *ia, usize size, usize alignment)
{
    (void)ia;
    (void)size;
    (void)alignment;
    return NULL;
}

AMWAPI void iafree(struct ipomoea *ia, u64 tag)
{
    (void)ia;
    (void)tag;
}
