#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
#include <string.h> /* strerror */
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

void *bedrock_mmap(usize size_page_aligned, usize hugepage_size)
{
    void *mapped = NULL;
    s32 flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

    if (!hugepage_size) {
        /* ignore */
#if defined(MAP_HUGE_4MB)
    } else if (hugepage_size == 4*1024*1024) {
        flags |= MAP_HUGE_4MB;
#endif /* 4MB */
#if defined(MAP_HUGE_2MB)
    } else if (hugepage_size == 2*1024*1024) {
        flags |= MAP_HUGE_2MB;
#endif /* 2MB */
#if defined(MAP_HUGE_1MB)
    } else if (hugepage_size == 1024*1024) {
        flags |= MAP_HUGE_1MB;
#endif /* 1MB */
    }
    mapped = mmap(NULL, size_page_aligned, PROT_NONE, flags, -1, 0);
    if (mapped == MAP_FAILED) {
        bedrock_log_fatal("mmap failed to reserve virtual memory of %lu bytes (%lumb): %s.", 
                size_page_aligned, size_page_aligned >> 20, strerror(errno));
        return NULL;
    }
    return mapped;
}

void bedrock_munmap(void *mapped, usize size_page_aligned)
{
    if (!mapped) return;
    s32 res = munmap(mapped, size_page_aligned);
    bedrock_assert_debug(!res);
}

bool bedrock_madvise(void *mapped, usize offset_page_aligned, usize size_page_aligned, enum bedrock_madvise_mode mode)
{
    if (!mapped) return false;

    void *raw_map = (void *)(sptr)(mapped + offset_page_aligned);
    const char *errtype = mode == bedrock_madvise_mode_commit ? "commitment" : "release";
    bool success = true;

    /* commit physical memory */
    if (mode == bedrock_madvise_mode_commit) {
        s32 res = mprotect(raw_map, size_page_aligned, PROT_READ | PROT_WRITE);
        if (res == 0)
            res = madvise(raw_map, size_page_aligned, MADV_WILLNEED);
        success = (res == 0);

    /* release physical memory */
    } else if (mode == bedrock_madvise_mode_release) {
        s32 res = madvise(raw_map, size_page_aligned, MADV_DONTNEED);
        if (res == 0)
            res = mprotect(raw_map, size_page_aligned, PROT_NONE);
        success = (res == 0);
    }

    /* check for errors */
    if (!success) {
        const char *errmsg = NULL;
        errmsg = strerror(errno);
        bedrock_log_error("Failed %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB): %s.", errtype,
                size_page_aligned, size_page_aligned >> 20, offset_page_aligned, offset_page_aligned >> 20, errmsg);
#ifdef LAKE_DEBUG
    } else {
        bedrock_log_debug("Done %s of physical memory: %lu bytes (%lu MB) at %lu mapped offset (%lu MB).", errtype,
                size_page_aligned, size_page_aligned >> 20, offset_page_aligned, offset_page_aligned >> 20);
#endif
    }
    return success;
}
#endif /* LAKE_PLATFORM_UNIX */
