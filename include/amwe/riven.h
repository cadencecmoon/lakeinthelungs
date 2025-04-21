#pragma once

#include <amwe/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* count of deferred lock-less heaps per thread, the last heap gets recycled */
#ifndef RIVEN_DEFERRED_CYCLE
#define RIVEN_DEFERRED_CYCLE 2
#endif

/* we assume a block size of 256KB */
#define RIVEN_BLOCK_SIZE (256lu*1024lu)

#define A8(x)       (((x) + 7lu) & ~7lu)
#define A16(x)      (((x) + 15lu) & ~15lu)
#define A4KB(x)     (((x) + 4095lu) & ~4095lu)
#define A256KB(x)   (((x) + RIVEN_BLOCK_SIZE-1lu) & ~(RIVEN_BLOCK_SIZE-1lu))
#define A2MB(x)     (((x) + 8lu*RIVEN_BLOCK_SIZE-1lu) & ~(8lu*RIVEN_BLOCK_SIZE-1lu))

struct riven;

/** A chain represents an atomic counter that is bound to a job queue. If used within a 
 *  call to Riven's job system, it will block until all jobs associated with the chain finish. 
 *  In the meantime, the caller does not 'wait', instead an implicit context switch is made so
 *  that the caller's thread joins to complete the work on the job queue. */
typedef atomic_usize *riven_chain_t;

/** Defines a job function, that can be run in parallel. */
typedef void (LAKECALL *PFN_riven_work)(void *userdata);

/** Defines a job that will be running within a fiber's context. Name is used for profilling, can be NULL. */
struct riven_work {
    PFN_riven_work  procedure;
    void           *userdata;
    const char     *name;
};

/** Parameters for Riven, after being evaluated a read-only pointer is given forward into the framework. */
struct riven_hints {
    usize               memory_budget;
    usize               fiber_stack_size;
    u32                 fiber_count;
    u32                 thread_count;
    u32                 tagged_heap_count;
    u32                 log2_job_count;
    u32                 log2_arena_count;
};
struct pelagial_metadata;

/** An entry point to the entire system, defined by a framework. */
typedef s32 (LAKECALL *PFN_riven_framework)(
    struct riven                   *riven,
    const struct riven_hints       *riven_hints,
    bedrock_thread_t               *threads,
    void                           *userdata);

/** Returns an array index of the current thread. The index is acquired by a hash lookup of the thread id. */
LAKEAPI lake_pure lake_hot lake_nonnull(1) u32 LAKECALL
riven_thread_index(struct riven *riven);

/** An unique tag groups resources of a shared lifetime, to be freed all at once. */
typedef u32 riven_tag_t;

/** Predefined tags for expected lifetime frequencies of game resources. Tags of other values can be 
 *  used for other uses, like allocating memory for assets: scenes, textures, meshes, audio, etc. */
enum riven_tag {
    /** Resources under this tag cannot be freed, they will share the lifetime of Riven. */
    riven_tag_roots = 0,
    /** Can be freely assigned by the framework or application:
     *      e.g #define my_app_riven_tag_rendering riven_tag_reserved_1
     *
     *  Reserved tags serve as an array index, eliminate overhead of looking up through 
     *  tagged heaps to find the correct arena, assuming one with such tag exists.
     *  Managing lifetimes of these tags is the responsibility of the application. */
    riven_tag_reserved_1,
    riven_tag_reserved_2,
    riven_tag_reserved_3,
    riven_tag_reserved_4,
    riven_tag_reserved_5,
    riven_tag_reserved_6,
    riven_tag_reserved_7,
    riven_tag_reserved_8,
    riven_tag_reserved_9,
    riven_tag_reserved_10,
    riven_tag_reserved_11,
    riven_tag_reserved_12,
    riven_tag_reserved_13,
    riven_tag_reserved_14,
    riven_tag_reserved_15,
    /** A minimum count of tagged heaps that will be in use. */
    riven_tag_reserved_count,
};
/** For scratch allocations on thread-local arenas. */
#define riven_tag_deferred UINT32_MAX

/** Implements a dynamic malloc, memory returned must be later freed with 'riven_free' 
 *  (not 'riven_tfree') to avoid memory leaks within the system. */
LAKEAPI lake_hot lake_nonnull(1) lake_malloc void *LAKECALL 
riven_malloc_aligned(
    struct riven   *riven,
    usize           size,
    usize           alignment);

lake_force_inline lake_nonnull(1) void *
riven_malloc(struct riven *riven, usize size) 
{
    return riven_malloc_aligned(riven, size, 1);
}

/** Implement a dynamic realloc, memory returned must be later freed with 'riven_free'. */
LAKEAPI lake_hot lake_nonnull(1) lake_malloc void *LAKECALL 
riven_realloc_aligned(
    struct riven   *riven,
    void           *memory,
    usize           size,
    usize           alignment);

lake_force_inline lake_nonnull(1) void *
riven_realloc(struct riven *riven, void *memory, usize size) {
    return riven_realloc_aligned(riven, memory, size, 1);
}

/** Frees memory that was returned from 'riven_malloc_aligned'. Must not be called
 *  with a pointer returned by 'riven_talloc'. */
LAKEAPI lake_hot lake_nonnull(1,2) void LAKECALL
riven_free(
    struct riven   *riven,
    void           *memory);

/** A linear allocation under a tagged heap arena. */
LAKEAPI lake_hot lake_nonnull(1) lake_malloc void *LAKECALL 
riven_thalloc(
    struct riven   *riven,
    riven_tag_t     tag,
    usize           size,
    usize           alignment);

/** Frees resources used by an arena under the given tagged heap. */
LAKEAPI lake_hot void LAKECALL 
riven_thfree(
    struct riven   *riven,
    riven_tag_t     tag);

/** Rotate the deffered thread-local heaps and free just the last resources. */
LAKEAPI lake_hot lake_nonnull(1) void LAKECALL 
riven_rotate_deferred(struct riven *riven);

/** Advises on commitment of host memory resources. Returns non-zero value if changes were made.
 *  On growth request, the heap will only commit if there is not enough contiguous space. On freeing
 *  resources, changes will be made only if the heap can be safely trimmed. */
LAKEAPI lake_nonnull(1) usize LAKECALL 
riven_advise_commitment(
    struct riven             *riven,
    usize                     size,
    enum bedrock_madvise_mode mode);

/** Runs 'work_count' amount of jobs, passed in the flat array 'work'. This will return immediately, and the 
 *  provided jobs will be resolved in the background in parallel. If 'chain' is not NULL, it will be set to 
 *  a value that can be used to wait for the work to finish in the meantime. */
LAKEAPI lake_hot lake_nonnull(1,2) lake_thread_acquire_shared(4) void 
riven_split_work(
    struct riven       *riven,
    struct riven_work  *work,
    u32                 work_count,
    riven_chain_t      *chain);

/** If a chain is not NULL, then an implicit context switch is made for the calling thread, to the work that 
 *  was associated with the chain to be finished. If the chain is NULL, then this call may or may not be yield
 *  to the job system before returning. Chains can be acquired outside of a job queue, to be used as 'locks' 
 *  that continue work with a context switch instead of busy-waiting for the work to finish. */
LAKEAPI lake_hot lake_nonnull(1) lake_thread_release_shared(2) void LAKECALL 
riven_unchain(
    struct riven   *riven,
    riven_chain_t   chain);

/** Combines the effects of 'rivens_split_work' and 'rivens_unchain', by providing a chain. 
 *  This function does not return until all submitted work has completed. */
lake_force_inline lake_nonnull(1,2) void
riven_split_work_and_unchain(
    struct riven       *riven,
    struct riven_work  *work,
    u32                 work_count)
{
    riven_chain_t chain = NULL;
    riven_split_work(riven, work, work_count, &chain);
    riven_unchain(riven, chain);
}

/** Arguments for implementations of PFN_riven_encore. */
#define ARGS_RIVEN_ENCORE \
    struct riven                   *riven, \
    const struct riven_hints       *riven_hints, \
    const struct pelagial_metadata *metadata, \
    void                           *userdata, \
    riven_tag_t                     tag
typedef void *(LAKECALL *PFN_riven_encore)(ARGS_RIVEN_ENCORE);

/** Defines an interface implementation to an engine subsystem, can be run as a job. */
#define FN_RIVEN_ENCORE(INTERFACE, IMPLEMENTATION) \
    struct INTERFACE##_encore *LAKECALL \
    INTERFACE##_encore_##IMPLEMENTATION(ARGS_RIVEN_ENCORE)

/** Definitions of reserved encores, when there is no valid implementation yet. */
#define FN_RIVEN_ENCORE_STUB(INTERFACE, IMPLEMENTATION) \
    FN_RIVEN_ENCORE(INTERFACE, IMPLEMENTATION) { \
        bedrock_log_debug("Interface '%s: %s' not yet implemented.", #INTERFACE, #IMPLEMENTATION); \
        (void)riven; (void)riven_hints; (void)metadata; (void)userdata; (void)tag; \
        return NULL; \
    }

/** Abstracts away an interface structure that defines this header. */
struct riven_interface_header {
    /** An interface implementation may live until it is in use by any independent system.
     *  Losing all reference points will zombify */
    atomic_u64                      refcnt;
    struct riven                   *riven;          /**< The context for Riven. */
    riven_tag_t                     tag;            /**< The lifetime of this interface. */
    const char                     *name;           /**< The name of this interface. */
    const char                     *backend;        /**< The name of this implementation. */
    /**< Callback used to destroy the encore, the work argument is 'encore' of the interface. */
    PFN_riven_work                  zero_ref_callback;
};

#define riven_inc_refcnt(REFCNT) \
    (u64)lake_atomic_add_explicit(REFCNT, 1llu, lake_memory_model_relaxed)
#define riven_dec_refcnt(REFCNT) \
    (u64)lake_atomic_sub_explicit(REFCNT, 1llu, lake_memory_model_relaxed)

typedef bool (LAKECALL *PFN_riven_interface_validation)(const void *interface);
#define FN_RIVEN_INTERFACE_VALIDATION(INTERFACE) \
    bool LAKECALL INTERFACE##_interface_validation(const struct INTERFACE##_interface *interface)

/** Work argument to execute a list of encores related to a single interface,
 *  and perform other work to validate the interface. Encores are executed
 *  one-by-one, until one is valid, and this encore is then written (returned).
 *  All fields except the 'debug' stuff must be set (not NULL). */
struct riven_encore_work {
    /** The name of the interface, for logging. */
    const char                     *name;
    /** A list of encore implementations to try, should be sorted by the user by most important. */
    const PFN_riven_encore         *encores;
    /** Maximum number of encores to try. */
    u32                             encore_count;
    /** The lifetime of the interface, handled by the application. */
    riven_tag_t                     tag;
    /** Information about the engine and application. */
    const struct pelagial_metadata *metadata;
    /** Optional, performs a validation on the public interface structure.
     *  Must return true if the interface implementation is accepted,
     *  otherwise if false is returned, the interface will be discarded. */
    PFN_riven_interface_validation  interface_validation;
};

/** Executes a list of encores for one interface, until one implementation is valid. */
LAKEAPI lake_nonnull_all void *LAKECALL
riven_encore(struct riven *riven, struct riven_encore_work *restrict work, void *encore_userdata);

/** Setups the job system and maps virtual memory to be used within the engine. The resource requirements of 
 *  internal systems depends on given argument hints and on the capabilities of the host system. Passing 0 as 
 *  arguments for size and count hints will provide reasonable defaults within the limits of the host - but it 
 *  is still better to fine-tune these for the expected needs of the application. 
 *
 *  A threadsafe job queue with a capacity of (1 << log2_work_count) will be created, the queue has no concept
 *  of a priority (for now I guess). Thread count (including main) worker threads are created, each one locked 
 *  to a CPU core. Fiber count of fibers are created, each with its own stack of a given size in bytes. Fibers 
 *  are the context of execution, while threads are the units that do the execution. Rivens memory allocator 
 *  is intended as a backend for managing host memory resources in a threadsafe and efficient way. It's a block 
 *  allocator, where a block is the size of one large page (2MB -> 1 tbl entry), that is redistributed between
 *  tagged heaps. A tagged heap is an arena with a rivens_tag_t associated, a tag implies a frequency of resource 
 *  lifetime. To free resources back to Riven it is enough to free the tag, as there is no free(ptr) interface.
 *
 *  Returns the value given by the main procedure, the application can do whatever it wants with this information. */
LAKEAPI lake_nonnull(1,2) s32 LAKECALL 
riven_moonlit_walk(
    struct riven_hints     *hints,
    PFN_riven_framework     main,
    void                   *userdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */
