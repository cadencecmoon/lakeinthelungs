#pragma once

/** @file lake/sys.h 
 *  @brief TODO docs system
 */
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Open a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void *LAKECALL 
lake_sys_open_library(char const *libname);

/** Close a shared library. */
LAKE_NONNULL_ALL 
LAKEAPI void LAKECALL 
lake_sys_close_library(void *module);

/** Get a procedure address from a module. */
LAKE_NONNULL_ALL 
LAKEAPI void *LAKECALL 
lake_sys_proc_addr(
    void       *module, 
    char const *procname);

/** Map virtual memory space without physical memory commitment.
 *  Before memory is used, must be commited with `lake_system_madvise()`. */
LAKEAPI void *LAKECALL 
lake_sys_mmap(
    usize size_page_aligned, 
    usize hugepage_size);

/** Unmap virtual memory space and release physical resources. */
LAKE_NONNULL_ALL
LAKEAPI void LAKECALL 
lake_sys_munmap(
    void *mapped, 
    usize size_page_aligned);

typedef enum : s32 {
    lake_madvise_mode_commit = 0,
    lake_madvise_mode_release,
} lake_madvise_mode;

/** Control state and commitment of physical resources. */
LAKE_NONNULL_ALL
LAKEAPI bool LAKECALL 
lake_sys_madvise(
    void             *mapped, 
    usize             offset_page_aligned, 
    usize             size_page_aligned, 
    lake_madvise_mode mode);

/** Read system info about the CPU. */
LAKEAPI void LAKECALL 
lake_sys_cpuinfo(
    s32 *out_threads, 
    s32 *out_cores, 
    s32 *out_packages);

/** Read system info about RAM. */
LAKEAPI void LAKECALL 
lake_sys_meminfo(
    usize *out_total_ram, 
    usize *out_pagesize);

/** Read system info about support of huge pages. */
LAKEAPI void LAKECALL 
lake_sys_hugetlbinfo(
    usize *out_huge_pagesize, 
    usize  max_accept_size);

/** The thread identifier/handle. */
typedef u64 lake_thread_id;

/** Creates and runs a thread, saves it's identifier to the given thread handle. */
LAKE_NONNULL(1,2) 
LAKEAPI void LAKECALL 
lake_sys_thread_create(
    lake_thread_id *out_thread, 
    void           *(*procedure)(void *), 
    void           *argument);

/** Destroys and jois a thread. */
LAKEAPI void LAKECALL 
lake_sys_thread_destroy(lake_thread_id thread);

/** Joins a thread. It will wait for the thread to finish it's work before continuing. */
LAKEAPI void LAKECALL 
lake_sys_thread_join(lake_thread_id thread);

/** Retrieve the identifier of the currently running thread. */
LAKEAPI lake_thread_id LAKECALL 
lake_sys_thread_current(void);

/** Set thread affinity for an array of threads. */
LAKE_NONNULL(2) 
LAKEAPI void LAKECALL 
lake_sys_thread_affinity(
    u32                   thread_count, 
    lake_thread_id const *threads, 
    u32                   cpu_count, 
    u32                   begin_cpu_index);

#ifdef __cplusplus
}
#endif /* __cplusplus */
