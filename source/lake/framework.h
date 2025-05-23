#pragma once

/** @file lake/framework.h
 *  @brief TODO docs
 */
#include "types.h"
#include "atomic.h"
#include "job_system.h"
#include "tagged_heap.h"
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Information about the application, given at main. After the framework initializes, it is immutable. */
typedef struct {
    char const *engine_name;        /**< Name of the game engine. */
    char const *game_name;          /**< Name of the game application. */
    char const *build_postfix;      /**< A custom name label for the build version. */
    u32         build_engine_ver;   /**< Build version of the engine. */
    u32         build_game_ver;     /**< Build version of the application. */
    u64         timer_start;        /**< Time when the system was initialized. */
    /** Hints how to initialize the framework, will be kept uptodate for defaults and limits at initialization. */
    struct {
        /** Sets a hard limit on the physical resources the framework is allowed to use. If 0, default 
         *  will be the system's total RAM memory. This budget is used to reserve virtual address space. */
        usize       memory_budget;
        /** Path relative to the executable where the file log will be written to. 
         *  If left as nullptr, file logging will be disabled. */
        char const *filelog_relative_path; 
        /** Target size for hugetlb entries. If 0, default will be the default huge page size (usually 2MB).
         *  If set at a non-zero value, this will serve as the limit - the lowest valid page size will be picked.
         *  Whenever the huge page size could not be resolved, normal page size is set (usually 4096KB). */
        u32         huge_page_size;
        /** Every fiber will have a stack of this size. If 0, default will be 64KB. */
        u32         fiber_stack_size;
        /** Number of fibers to create. If 0, default will be 96 + 4 * worker_thread_count. */
        u32         fiber_count;
        /** Number of threads to create. If 0, default will be the system's CPU count. 
         *  Worker threads have CPU affinity, thus we don't allow to create more threads than CPUs available. */
        u32         worker_thread_count;
        /** Number of tagged heaps to create. If 0, default will be 16. */
        u32         tagged_heap_count;
        /** The job queue will be of this size: (1u << log2_job_count). If 0, default will be 10 (1024). */
        u32         log2_job_count;
    } hints;
    /** The worker threads created by the framework. */ 
    lake_thread_id const *worker_threads;
} lake_app_desc;

/** Overwrites the return exitcode of the game process, whenever `lake_abort_()` will be called. */
LAKEAPI void LAKECALL 
lake_exit_status(
    s32 error_code);

/** Aborts the game and returns the given code. In case where exitcode is 0 (lake_result_success),
 *  the exitcode may be overwritten if ever `lake_exit_status()` was called. This function may 
 *  perform a logging operation before it exits. */
LAKE_NORETURN 
LAKEAPI void LAKECALL 
lake_abort_(
    s32         error_code, 
    char const *fn, 
    char const *file, 
    s32         line);
#define lake_abort(error_code) lake_abort_(error_code, LAKE_FUNCTION, LAKE_FILE, LAKE_LINE)

/** Systems can use an atomic counter for references to the system. A reference count of 0
 *  means that the system can be safely destroyed, as it is no longer in use. */
typedef atomic_u32 lake_refcnt;

/** Declare a handle to an opaque structure. */
#define LAKE_DECL_HANDLE(T) \
    typedef struct T##_impl *T

/** A common interface for other systems to inherit. Used to abstract over an interface implementation. */
typedef struct {
    /** A pointer into the metadata of the framework and the application. */
    lake_app_desc const    *app_desc;
    /** An unique name for the interface implementation. */
    lake_small_string       name;
    /** Tracks references to this interface. */
    lake_refcnt             refcnt;
    /** Indicates the maximum lifetime of this interface. */
    lake_heap_tag           tag;
    /** Will be called to destroy the interface, the work argument is self. */
    PFN_lake_work           zero_ref_callback;
} lake_interface_header;

/** Increment the reference count of an interface.
 *  @return Previous reference count. */
LAKE_FORCE_INLINE u32 lake_inc_refcnt(lake_interface_header *header)
{ return lake_atomic_add_explicit(&header->refcnt, 1u, lake_memory_model_release); }

/** Decrement the reference count of an interface, call `zero_ref_callback` if it hits 0.
 *  @return Previous reference count. */
LAKE_FORCE_INLINE u32 lake_dec_refcnt(lake_interface_header *header)
{
    u32 prev = lake_atomic_sub_explicit(&header->refcnt, 1u, lake_memory_model_release);
    if (prev == 1) header->zero_ref_callback(lake_reinterpret_cast(void *, header));
    return prev;
}

/** Implements an interface. Must be defined by an unique backend. */
typedef LAKE_NODISCARD void *(LAKECALL *PFN_lake_interface_assembly)(void *assembly, lake_heap_tag tag, s32 log_level);
/** Defines a function of signature `PFN_lake_interface_assembly`. */
#define FN_LAKE_INTERFACE_ASSEMBLY(fn, T, Tout) \
    LAKE_NODISCARD Tout *LAKECALL fn(T *assembly, lake_heap_tag tag, s32 log_level)

/** Validates an interface implementation, accepts expected data output of `PFN_lake_interface_assembly`. */
typedef LAKE_NODISCARD bool (LAKECALL *PFN_lake_interface_validation)(void *interface, s32 log_level);
/** Defines a function of signature `PFN_lake_interface_validation`. */
#define FN_LAKE_INTERFACE_VALIDATION(fn, T) \
    LAKE_NODISCARD bool LAKECALL fn(T, s32 log_level)

/** Work argument for `lake_assembly` to run them in parallel. A list of implementations 
 *  is accepted and will be looped over until a implementation is valid, or until all possible 
 *  implementations have been discarded. The assemblies are run in order, with the exception of 
 *  a first target defined via `preferred_idx` into the assembly array. `out_interface` must be 
 *  set to the address of a valid interface backend handle. The first valid assembly writes to 
 *  the address pointer by `output`. If no assembly succeeds, nullptr is written instead. */
typedef struct {
    /** A name of the requested interface, for tracing. */
    char const                     *name;
    /** Optional argument for the assembly functions. */
    void                           *argument;
    /** The expected lifetime of the interface. */
    lake_heap_tag                   tag;
    /** Will be used for tracing log messages. */
    s32                             log_level;
    /** If a non-negative value, it will be used as the first target in `assembly`. */
    s32                             preferred_idx;
    /** How many implementations are available for this interface. */
    u32                             assembly_count;
    /** An array of acceptable implementations. */
    PFN_lake_interface_assembly    *assembly;
    /** Optional procedure to possibly discard an assembly that returned successfull. */
    PFN_lake_interface_validation   validation;
    /** Where the interface (or implementation handle) should be written. */
    void                          **output;
} lake_assembly_work;

/** Assembles an interface from possible implementations. */
FN_LAKE_WORK(lake_assembly, lake_assembly_work const *work);

/** Application defined entrypoint for the framework. */
typedef void (LAKECALL *PFN_lake_framework)(lake_app_desc const *app_desc, void *userdata);
/** Defines a function of signature `PFN_lake_framework`. */
#define FN_LAKE_FRAMEWORK(fn, T, in) \
    void LAKECALL fn(lake_app_desc const *app_desc, T *in)

/** Setups the job system, tagged heaps, an advanced logger, optional systems like the profiler, 
 *  and a collection of memory allocators. The initialization process depends on hints given in 
 *  the `lake_app_desc` structure, and on limits for different internal systems. If no hints are 
 *  given (e.g. zeroed values), sane defaults will be provided, but it's still better to fine-tune 
 *  the framework for specific needs of an application. After initialization is done, a fiber calls 
 *  into the app-defined entrypoint and passes in the inmutable app_desc and self-defined userdata.
 *
 *  A fiber-aware job queue with a capacity of (1 << log2_job_count) will be created for the job system. 
 *  The queue has NO concept of priority. `worker_thread_count` of threads are created, each one is locked
 *  to a CPU core. Each fiber of `fiber_count` has it's own stack region of a `fiber_stack_size`. Virtual 
 *  memory is mapped for tagged heaps with a hard limit of `memory_budget` aligned to a huge page size.
 *  For page entries, the closest available page size to `huge_page_size` is used, with 0 defaulting to 
 *  the smallest hugetlb page entry (usually 2MB), and to the default page size if huge pages are not 
 *  available (usually 4096KB). The logger's capability to log into lock-less buffers is enabled, that 
 *  means all logging will be MUCH faster, it's safe for parallel use, and logs must be flushed for them 
 *  to appear on screen or before they are logged into a file output, if one was defined.
 *
 *  The framework and all internal structures are only valid until `PFN_lake_framework` returns,
 *  so inside the scope of the app-defined entrypoint procedure. */
LAKE_NODISCARD LAKE_NONNULL_ALL
LAKEAPI lake_result LAKECALL
a_moonlit_walk(
    lake_app_desc     *app_desc,
    PFN_lake_framework entrypoint,
    void              *userdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */
