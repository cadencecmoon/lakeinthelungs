#pragma once

#include <amw/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif

/* To enable analysis, set these environment variables before running meson:
 *     export CC=clang
 *     export CFLAGS="-DENABLE_THREAD_SAFETY_ANALYSIS -Wthread-safety" */
#if (defined(ENABLE_THREAD_SAFETY_ANALYSIS)) && defined(CC_CLANG_VERSION) && !defined(SWIG)
    #define THREAD_SAFETY_ATTRIBUTE(x) __attribute__((x))
#else
    #define THREAD_SAFETY_ATTRIBUTE(x)
#endif

#define attr_capability(x)                      THREAD_SAFETY_ATTRIBUTE(capability(x))
#define attr_scoped_lockable                    THREAD_SAFETY_ATTRIBUTE(scoped_lockable)
#define attr_guarded_by(x)                      THREAD_SAFETY_ATTRIBUTE(guarded_by(x))
#define attr_pt_guarded_by(x)                   THREAD_SAFETY_ATTRIBUTE(pt_guarded_by(x))
#define attr_acquired_before(x)                 THREAD_SAFETY_ATTRIBUTE(acquired_before(x))
#define attr_acquired_after(x)                  THREAD_SAFETY_ATTRIBUTE(acquired_after(x))
#define attr_requires_capability(x)             THREAD_SAFETY_ATTRIBUTE(requires_capability(x))
#define attr_requires_shared_capability(x)      THREAD_SAFETY_ATTRIBUTE(requires_shared_capability(x))
#define attr_acquire_capability(x)              THREAD_SAFETY_ATTRIBUTE(acquire_capability(x))
#define attr_acquire_shared_capability(x)       THREAD_SAFETY_ATTRIBUTE(acquire_shared_capability(x))
#define attr_release_capability(x)              THREAD_SAFETY_ATTRIBUTE(release_capability(x))
#define attr_release_shared_capability(x)       THREAD_SAFETY_ATTRIBUTE(release_shared_capability(x))
#define attr_release_generic_capability(x)      THREAD_SAFETY_ATTRIBUTE(release_generic_capability(x))
#define attr_try_acquire_capability(x)          THREAD_SAFETY_ATTRIBUTE(try_acquire_capability(x))
#define attr_try_acquire_shared_capability(x)   THREAD_SAFETY_ATTRIBUTE(try_acquire_shared_capability(x))
#define attr_locks_excluded(x)                  THREAD_SAFETY_ATTRIBUTE(locks_excluded(x))
#define attr_assert_capability(x)               THREAD_SAFETY_ATTRIBUTE(assert_capability(x))
#define attr_assert_shared_capability(x)        THREAD_SAFETY_ATTRIBUTE(assert_shared_capability(x))
#define attr_lock_returned(x)                   THREAD_SAFETY_ATTRIBUTE(lock_returned(x))
#define attr_no_thread_safety_analysis(x)       THREAD_SAFETY_ATTRIBUTE(no_thread_safety_analysis(x))

/** A handle for representing a worker thread. */
typedef u64 thread_t;

/** Creates and runs a worker thread at a given handle. */
AMWAPI attr_nonnull(1,2)
void AMWCALL thread_create(
    thread_t *thread, 
    void *(*procedure)(void *), 
    void *argument); 

/** Destroys and joins a given thread. Shouldn't really be called 
 *  unless a dynamic thread was ever spawned outside of riven. */
AMWAPI void AMWCALL thread_destroy(thread_t thread);

/** Just joins a thread, without destroying it. It will wait for the 
 *  thread to finish its work before continuing. */
AMWAPI void AMWCALL thread_join(thread_t thread);

/** A primitive way of acquiring a thread index of the current thread. */
AMWAPI attr_nonnull(1)
u32 AMWCALL thread_array_index(const thread_t *threads, u32 thread_count);

/** Retrieve the handle of the current worker thread. */
AMWAPI attr_const attr_hot
thread_t AMWCALL thread_current(void);

/** Set thread affinity for an array of threads. Thread count per core should 
 *  be equal to on how many hardware threads has one physical CPU core. The core 
 *  start index should be 0 or less than core_count-1, the thread_count_per_core
 *  should accomodate for how many CPU cores is the affinity created for. */
AMWAPI attr_nonnull(1,2)
void AMWCALL thread_affinity(
    u8       *stack_memory, 
    thread_t *threads, 
    u32       thread_count, 
    u32       cpu_count,
    u32       begin_cpu_index);

#ifdef __cplusplus
}
#endif
