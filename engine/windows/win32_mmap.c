#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>
#include <process.h>

void *bedrock_mmap(usize size_page_aligned, usize hugepage_size)
{
    void *mapped = NULL;
    s32 flags = MEM_RESERVE;

    SIZE_T large_page_min = GetLargePageMinimum();
    if (large_page_min > 0 && hugepage_size > 4096)
        flags |= MEM_LARGE_PAGES;
    mapped = VirtualAllocEx(GetCurrentProcess(), NULL, size_page_aligned, flags, PAGE_NOACCESS);
    if (!mapped) {
        bedrock_log_fatal("VirtualAlloc failed to reserve virtual memory of %lu bytes (%lumb): %lu.", 
                size_page_aligned, size_page_aligned >> 20, GetLastError());
        return NULL;
    }
    return mapped;
}

void bedrock_munmap(void *mapped, usize size_page_aligned)
{
    if (!mapped) return;
    BOOL res = VirtualFreeEx(GetCurrentProcess(), (LPVOID)mapped, 0, MEM_RELEASE);
    bedrock_assert_debug(res != FALSE)
}

bool bedrock_madvise(void *mapped, usize offset_page_aligned, usize size_page_aligned, enum bedrock_madvise_mode mode)
{
    if (!mapped) return false;

    void *raw_map = (void *)(sptr)(mapped + offset_page_aligned);
    const char *errtype = mode == bedrock_madvise_mode_commit ? "commitment" : "release";
    bool success = true;

    /* commit physical memory */
    if (mode == bedrock_madvise_mode_commit) {
        void *p = VirtualAllocEx(GetCurrentProcess(), raw_map, size_page_aligned, MEM_COMMIT, PAGE_READWRITE);
        success = (p != NULL);

    /* release physical memory */
    } else if (mode == bedrock_madvise_mode_release) {
        BOOL res = VirtualFreeEx(GetCurrentProcess(), raw_map, size, MEM_DECOMMIT);
        success = (res != FALSE);
    }

    /* check for errors */
    if (!success) {
        const char *errmsg = NULL;
        errmsg = "TODO";
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
#endif /* LAKE_PLATFORM_WINDOWS */
