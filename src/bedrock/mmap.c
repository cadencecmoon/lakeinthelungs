#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
    #include <string.h> /* strerror */
    #include <unistd.h>
    #include <sys/mman.h>
    #include <errno.h>
#elif defined(LAKE_PLATFORM_WINDOWS)
    #include <amwe/arch/windows.h>
    #include <process.h>
#endif

void *bedrock_mmap(usize size_page_aligned, usize hugepage_size)
{
    void *mapped = NULL;
#if defined(LAKE_PLATFORM_UNIX)
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
#elif defined(LAKE_PLATFORM_WINDOWS)
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
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
    return mapped;
}

void bedrock_munmap(void *mapped, usize size_page_aligned)
{
    if (!mapped) return;
#if defined(LAKE_PLATFORM_UNIX)
    s32 res = munmap(mapped, size_page_aligned);
    bedrock_assert_debug(!res);
#elif defined(LAKE_PLATFORM_WINDOWS)
    BOOL res = VirtualFreeEx(GetCurrentProcess(), (LPVOID)mapped, 0, MEM_RELEASE);
    bedrock_assert_debug(res != FALSE)
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
#error implement wasm heapbase
#endif
}

bool bedrock_madvise(void *mapped, usize offset_page_aligned, usize size_page_aligned, enum bedrock_madvise_mode mode)
{
    if (!mapped) return false;

    void *raw_map = (void *)(sptr)(mapped + offset_page_aligned);
    const char *errtype = mode == bedrock_madvise_mode_commit ? "commitment" : "release";
    bool success = true;

    /* commit physical memory */
    if (mode == bedrock_madvise_mode_commit) {
#if defined(LAKE_PLATFORM_UNIX)
        s32 res = mprotect(raw_map, size_page_aligned, PROT_READ | PROT_WRITE);
        if (res == 0)
            res = madvise(raw_map, size_page_aligned, MADV_WILLNEED);
        success = (res == 0);
#elif defined(LAKE_PLATFORM_WINDOWS)
        void *p = VirtualAllocEx(GetCurrentProcess(), raw_map, size_page_aligned, MEM_COMMIT, PAGE_READWRITE);
        success = (p != NULL);
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
    #error implement WASM heap base
#endif /* LAKE_PLATFORM_UNIX */

    /* release physical memory */
    } else if (mode == bedrock_madvise_mode_release) {
#if defined(LAKE_PLATFORM_UNIX)
        s32 res = madvise(raw_map, size_page_aligned, MADV_DONTNEED);
        if (res == 0)
            res = mprotect(raw_map, size_page_aligned, PROT_NONE);
        success = (res == 0);
#elif defined(LAKE_PLATFORM_WINDOWS)
        BOOL res = VirtualFreeEx(GetCurrentProcess(), raw_map, size, MEM_DECOMMIT);
        success = (res != FALSE);
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
    #error implement WASM heap base
#endif /* LAKE_PLATFORM_UNIX */
    }

    /* check for errors */
    if (!success) {
        const char *errmsg = NULL;
#if defined(LAKE_PLATFORM_UNIX)
        errmsg = strerror(errno);
#elif defined(LAKE_PLATFORM_WINDOWS)
        errmsg = "TODO";
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
        errmsg = "TODO";
#endif /* LAKE_PLATFORM_UNIX */
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
