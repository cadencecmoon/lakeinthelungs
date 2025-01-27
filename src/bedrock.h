#pragma once

/* bedrock.h is a common header, included by source files and any other header not listed below.
 * It also abstracts away commonly used functionality and system calls between platforms. */

#include "bedrock/defines.h"
#include "bedrock/types.h"

/* keep them sorted */
#include "bedrock/align.h"
#include "bedrock/assert.h"
#include "bedrock/atomics.h"
#include "bedrock/endian.h"
#include "bedrock/intrinsics.h"
#include "bedrock/log.h"
#include "bedrock/riven.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Load the dynamic library into memory. */
LAKEAPI void *bedrock_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
LAKEAPI void bedrock_close_dll(void *module);

/** Get a function address from a dynamic library. */
LAKEAPI void *bedrock_get_proc_address(void *module, const char *procedure);

/** Returns the counter of the real-time clock. */
LAKEAPI u64 bedrock_rtc_counter(void);

/** Returns the frequency of the real-time clock. */
LAKEAPI u64 bedrock_rtc_frequency(void);

/** Read CPU info. */
LAKEAPI void bedrock_cpuinfo(ssize *threads, ssize *cores, ssize *packages);

/** Read RAM info. */
LAKEAPI void bedrock_meminfo(ssize *systemram, ssize *pagesize);

/** Read HugeTLB Pages info. */
LAKEAPI void bedrock_hugetlbinfo(ssize *hugepagesize, ssize max_target_hugepagesize);

/** Copies a memory region of size bytes from dest to src. */
LAKEAPI void *bedrock_memcpy(void *restrict dest, const void *restrict src, usize size);

/** Sets bits of a memory region of size bytes as c. */
LAKEAPI void *bedrock_memset(void *restrict dest, s32 c, usize size);

#undef zero
#define zero(mem)  bedrock_memset(&(mem), 0, sizeof((mem)))
#undef zerop
#define zerop(mem) bedrock_memset((mem), 0, sizeof(*(mem)))
#undef zeroa
#define zeroa(mem) bedrock_memset((mem), 0, sizeof((mem)))

/** Creates and runs a worker thread at a given handle. */
LAKEAPI void bedrock_thread_create(thread_t *thread, void *(*procedure)(void *), void *argument);

/** Destroys and joins a given thread. Shouldn't really be called 
 *  unless a dynamic thread was ever spawned outside of riven. */
LAKEAPI void bedrock_thread_destroy(thread_t thread);

/** Just joins a thread, without destroying it. It will wait for the 
 *  thread to finish its work before continuing. */
LAKEAPI void bedrock_thread_join(thread_t thread);

/** Gets the index of the current worker thread, in a given array of threads. */
LAKEAPI ssize bedrock_thread_index(thread_t *threads, ssize thread_count);

/** Retrieve the handle of the current worker thread. */
LAKEAPI thread_t bedrock_thread_current(void);

/** Set thread affinity for an array of threads. Thread count per core should 
 *  be equal to on how many hardware threads has one physical CPU core. The core 
 *  start index should be 0 or less than core_count-1, the thread_count_per_core
 *  should accomodate for how many CPU cores is the affinity created for. */
LAKEAPI void bedrock_thread_affinity(u8 *stack_memory, thread_t *threads, ssize thread_count, ssize begin_cpu_index);

/** Initialize the game clock. Counts time from the moment this functions was called. */
LAKEAPI void bedrock_apptimer_init(void);

/** Get the time passed since the game started, in milliseconds. */
LAKEAPI u64 bedrock_apptimer_ms(void);

/** Get the time passed since the game started, in nanoseconds. */
LAKEAPI u64 bedrock_apptimer_ns(void);

#ifdef __cplusplus
}
#endif
