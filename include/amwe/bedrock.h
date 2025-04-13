#pragma once

#include <amwe/lake_defines.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Invoke this function exactly once per frame to record the current frame time.
 *  Only when the other functions defined in this header will be available. */
LAKEAPI void LAKECALL 
bedrock_frame_time_record(u64 time_now, f64 dt_frequency);

/** Retrieves the current estimate of the frame time in seconds. It is the median 
 *  of a certain number of previously recorded frame times. */
LAKEAPI lake_const f32 LAKECALL 
bedrock_frame_time_median(void);

/** Prints the current estimate of the total frame time periodically, namely once 
 *  per given time interval, assuming this function is called once per frame. */
LAKEAPI void LAKECALL 
bedrock_frame_time_print(f32 interval_ms);

/* file name */
#ifndef BEDROCK_FILENAME
    #ifdef LAKE_PLATFORM_WINDOWS
        #define BEDROCK_FILENAME (bedrock_strrchr(__FILE__, '\\') ? bedrock_strrchr(__FILE__, '\\') + 1 : __FILE__)
    #else
        #define BEDROCK_FILENAME (bedrock_strrchr(__FILE__, '/') ? bedrock_strrchr(__FILE__, '/') + 1 : __FILE__)
    #endif
#endif

#define BEDROCK_LOG_VERBOSE 0
#define BEDROCK_LOG_DEBUG   1
#define BEDROCK_LOG_INFO    2
#define BEDROCK_LOG_WARN    3
#define BEDROCK_LOG_ERROR   4
#define BEDROCK_LOG_FATAL   5

LAKEAPI void LAKECALL 
bedrock_log_message(
    s32         level, 
    const char *fn, 
    const char *file, 
    s32         line, 
    const char *fmt, 
    ...) lake_printf(5,6);

LAKEAPI void LAKECALL bedrock_log_set_level(s32 level);
LAKEAPI void LAKECALL bedrock_log_set_verbose(bool set);
LAKEAPI void LAKECALL bedrock_log_set_quiet(bool set);

#ifndef BEDROCK_LOG_DISABLE_CONSOLE
#define bedrock_log_verbose(fmt, ...)  bedrock_log_message(BEDROCK_LOG_VERBOSE, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define bedrock_log_debug(fmt, ...)    bedrock_log_message(BEDROCK_LOG_DEBUG, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define bedrock_log_info(fmt, ...)     bedrock_log_message(BEDROCK_LOG_INFO, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define bedrock_log_warn(fmt, ...)     bedrock_log_message(BEDROCK_LOG_WARN, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define bedrock_log_error(fmt, ...)    bedrock_log_message(BEDROCK_LOG_ERROR, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#define bedrock_log_fatal(fmt, ...)    bedrock_log_message(BEDROCK_LOG_FATAL, __func__, BEDROCK_FILENAME, __LINE__, fmt, ##__VA_ARGS__)
#else
#define bedrock_log_verbose(fmt, ...)  ((void)0)
#define bedrock_log_debug(fmt, ...)    ((void)0)
#define bedrock_log_info(fmt, ...)     ((void)0)
#define bedrock_log_warn(fmt, ...)     ((void)0)
#define bedrock_log_error(fmt, ...)    ((void)0)
#define bedrock_log_fatal(fmt, ...)    ((void)0)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
    #define bedrock_static_assert(x, desc) static_assert(x, desc)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define bedrock_static_assert(x, desc) static_assert(x, desc)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define bedrock_static_assert(x, desc) _Static_assert(x, desc)
/* GCC 4.6 or later */
#elif LAKE_CC_GNUC_VERSION_CHECK(4,6,0)
    /* It will work but it may throw a warning:
     * warning: ISO C99 does not support '_Static_assert' [-Wpedantic] */
    #define bedrock_static_assert(x, desc) _Static_assert(x, desc)
#endif
#ifndef bedrock_static_assert
    #define bedrock_static_assert(x, desc) /* blank */
#endif
#ifndef BEDROCK_ASSERT_LEVEL
    #if defined(LAKE_DEBUG) || defined(_DEBUG) || defined(DEBUG) || (defined(LAKE_CC_GNUC_VERSION) && !defined(__OPTIMIZE__))
        #define BEDROCK_ASSERT_LEVEL 2
    #else
        #define BEDROCK_ASSERT_LEVEL 1
    #endif
#endif

#define bedrock_assertion(cond)                                 \
    do {                                                        \
        if (!lake_unlikely(cond)) {                             \
            bedrock_log_fatal("Assertion '%s' failed.", #cond); \
            lake_debugtrap();                                   \
        }                                                       \
    } while (0) 

#if BEDROCK_ASSERT_LEVEL == 3 /* paranoid settings */
    #define bedrock_assert_debug(cond)     bedrock_assertion(cond)
    #define bedrock_assert_release(cond)   bedrock_assertion(cond)
    #define bedrock_assert_paranoid(cond)  bedrock_assertion(cond)
#elif BEDROCK_ASSERT_LEVEL == 2 /* normal/debug settings */
    #define bedrock_assert_debug(cond)     bedrock_assertion(cond)
    #define bedrock_assert_release(cond)   bedrock_assertion(cond)
    #define bedrock_assert_paranoid(cond)  ((void)0)
#elif BEDROCK_ASSERT_LEVEL == 1 /* release settings */
    #define bedrock_assert_debug(cond)     ((void)0)
    #define bedrock_assert_release(cond)   bedrock_assertion(cond)
    #define bedrock_assert_paranoid(cond)  ((void)0)
#else /* assertions disabled */
    #define bedrock_assert_debug(cond)     ((void)0)
    #define bedrock_assert_release(cond)   ((void)0)
    #define bedrock_assert_paranoid(cond)  ((void)0)
#endif

LAKEAPI lake_hot lake_nonnull(1) void *LAKECALL 
bedrock_memset(void *restrict dest, s32 c, usize size);

LAKEAPI lake_hot lake_nonnull(1,2) void *LAKECALL
bedrock_memcpy(void *restrict dest, const void *restrict src, usize size);

#define bedrock_zero(mem)       bedrock_memset(&(mem), 0, lake_sizeof((mem)))
#define bedrock_zerop(mem)      bedrock_memset((mem), 0, lake_sizeof(*(mem)))
#define bedrock_zeroa(mem)      bedrock_memset((mem), 0, lake_sizeof((mem)))

LAKEAPI void *LAKECALL
bedrock_memrchr(const void *restrict mem, s32 c, usize size);

LAKEAPI usize LAKECALL 
bedrock_strlen(const char *str);

lake_force_inline char *
bedrock_strrchr(const char *str, s32 c) 
{
    return lake_ptr_cast(char *, bedrock_memrchr(lake_ptr_cast(const void *, str), c, bedrock_strlen(str) + 1));
}

/** A handle for representing a worker thread. */
typedef u64 bedrock_thread_t;

/** Creates and runs a worker thread at a given handle. */
LAKEAPI lake_nonnull(1,2) void LAKECALL 
bedrock_thread_create(
    bedrock_thread_t *thread, 
    void             *(*procedure)(void *), 
    void             *argument); 

/** Destroys and joins a given thread. Shouldn't really be called 
 *  unless a dynamic thread was ever spawned outside of riven. */
LAKEAPI void LAKECALL
bedrock_thread_destroy(bedrock_thread_t thread);

/** Joins a thread, without destroying it. It will wait for the 
 *  thread to finish its work before continuing. */
LAKEAPI void LAKECALL
bedrock_thread_join(bedrock_thread_t thread);

/** A primitive way of acquiring a thread index of the current thread. */
LAKEAPI lake_nonnull(1) u32 LAKECALL
bedrock_thread_array_index(
    const bedrock_thread_t *threads, 
    u32                     thread_count);

/** Retrieve the handle of the current worker thread. */
LAKEAPI lake_const lake_hot bedrock_thread_t LAKECALL 
bedrock_thread_current(void);

/** Set thread affinity for an array of threads. Thread count per core should 
 *  be equal to on how many hardware threads has one physical CPU core. The core 
 *  start index should be 0 or less than core_count-1, the thread_count_per_core
 *  should accomodate for how many CPU cores is the affinity created for. */
LAKEAPI lake_nonnull(1,2) void LAKECALL 
bedrock_thread_affinity(
    u8                 *stack_memory, 
    bedrock_thread_t   *threads, 
    u32                 thread_count, 
    u32                 cpu_count,
    u32                 begin_cpu_index);

/** Open a shared library. */
LAKEAPI void *LAKECALL 
bedrock_module_open(const char *libname);

/** Close a shared library. */
LAKEAPI void LAKECALL 
bedrock_module_close(void *module);

/** Get a procedure address from the loaded shared library module. */
LAKEAPI void *LAKECALL 
bedrock_get_proc_address(void *module, const char *procedure);

/** Returns the counter of the real-time clock. */
LAKEAPI lake_const u64 LAKECALL
bedrock_rtc_counter(void);

/** Returns the frequency of the real-time clock. */
LAKEAPI lake_const u64 LAKECALL 
bedrock_rtc_frequency(void);

/** Read system info about the CPU. */
LAKEAPI void LAKECALL 
bedrock_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages);

/** Read system info about RAM. */
LAKEAPI void LAKECALL 
bedrock_meminfo(usize *out_total_ram, usize *out_pagesize);

/** Read system info about support of huge pages. */
LAKEAPI void LAKECALL 
bedrock_hugetlbinfo(usize *out_huge_pagesize, usize max_accept_size);

enum bedrock_madvise_mode {
    bedrock_madvise_mode_commit = 0, 
    bedrock_madvise_mode_release,
};

/** Map virtual memory space without physical memory commitment.
 *  Before use, must be commited with bedrock_madvise(). */
LAKEAPI lake_malloc void *LAKECALL
bedrock_mmap(usize size_page_aligned, usize hugepage_size);

/** Unmap virtual memory space and release physical resources. */
LAKEAPI void LAKECALL
bedrock_munmap(void *mapped, usize size_page_aligned);

LAKEAPI bool LAKECALL
bedrock_madvise(void *mapped, usize offset_page_aligned, usize size_page_aligned, enum bedrock_madvise_mode mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
