/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_BEDROCK_OS_H
#define _AMW_BEDROCK_OS_H

#include <lake/bedrock/defines.h>

/** Load the dynamic library into memory. */
AMWAPI void *bedrock_load_dll(const char *libname);

/** Close the dynamic library and unload it's memory. */
AMWAPI void bedrock_close_dll(void *module);

/** Get a function address from a dynamic library. */
AMWAPI void *bedrock_get_proc_address(void *module, const char *procedure);

/** Returns the counter of the system real-time clock. */
AMWAPI u64 bedrock_rtc_counter(void);

/** Returns the frequency of the system real-time clock. */
AMWAPI u64 bedrock_rtc_frequency(void);

/** Read some basic CPU info. */
AMWAPI void bedrock_cpu_count(u32 *threads, u32 *cores, u32 *packages);

/** A handle for representing a worker thread. */
typedef u64 thread_id;

/** Creates and runs a worker thread at a given handle. */
AMWAPI void bedrock_thread_create(thread_id *thread, void *(*procedure)(void *), void *argument);

/** Destroys and joins a given thread. Shouldn't really be called 
 *  unless a dynamic thread was ever spawned outside of riven. */
AMWAPI void bedrock_thread_destroy(thread_id thread);

/** Just joins a thread, without destroying it. It will wait for the 
 *  thread to finish its work before continuing. */
AMWAPI void bedrock_thread_join(thread_id thread);

/** Gets the index of the current worker thread, in a given array of threads. */
AMWAPI ssize bedrock_thread_index(thread_id *threads, ssize thread_count);

/** Retrieve the handle of the current worker thread. */
AMWAPI thread_id bedrock_thread_current(void);

/** Set thread affinity for an array of threads. Thread count per core should 
 *  be equal to on how many hardware threads has one physical CPU core. The core 
 *  start index should be 0 or less than core_count-1, the thread_count_per_core
 *  should accomodate for how many CPU cores is the affinity created for. */
AMWAPI void bedrock_thread_affinity(u8 *stack_memory, thread_id *threads, ssize thread_count, ssize begin_cpu_index);

#endif /* _AMW_BEDROCK_OS_H */
