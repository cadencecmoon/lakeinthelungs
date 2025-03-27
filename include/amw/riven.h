#pragma once

#include <amw/bedrock.h>
#include <amw/string.h>
#include <amw/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

struct riven;

#define A8(x)    (((x) + 7lu) & ~7lu)
#define A16(x)   (((x) + 15lu) & ~15lu)
#define A4KB(x)  (((x) + 4095lu) & ~4095lu)

/** A chain represents an atomic counter that is bound to a job queue. If used within a 
 *  call to Riven's job system, it will block until all jobs associated with the chain finish. 
 *  In the meantime, the caller does not 'wait', instead an implicit context switch is made so
 *  that the caller's thread joins to complete the work on the job queue. */
typedef at_usize *riven_chain_t;

/** Defines a job function, that can be run in parallel. */
typedef void (AMWCALL *PFN_riven_work)(void *argument);

/** No operation, as a stub for NULL'ed out callback procedures. */
AMWAPI void riven_work_nop(void *argument);

/** Defines a job that will be running within a fiber's context. Name is used for profilling, can be NULL. */
struct riven_work {
    PFN_riven_work  procedure;
    void           *argument;
    struct str      name;
};

/** Applications entry point to the entire system. */
typedef s32 (AMWCALL *PFN_riven_heart)(
    struct riven       *riven, 
    thread_t           *threads,
    u32                 thread_count,
    void               *argument);

/** Runs 'work_count' amount of jobs, passed in the flat array 'work'. This will return immediately, and the 
 *  provided jobs will be resolved in the background in parallel. If 'chain' is not NULL, it will be set to 
 *  a value that can be used to wait for the work to finish in the meantime. */
AMWAPI attr_hot attr_nonnull(1,2) attr_acquire_shared_capability(4)
void AMWCALL riven_split_work(
    struct riven       *riven,
    struct riven_work  *work,
    u32                 work_count,
    riven_chain_t      *chain);

/** If a chain is not NULL, then an implicit context switch is made for the calling thread, to the work that 
 *  was associated with the chain to be finished. If the chain is NULL, then this call may or may not be yield
 *  to the job system before returning. Chains can be acquired outside of a job queue, to be used as 'locks' 
 *  that continue work with a context switch instead of busy-waiting for the work to finish. */
AMWAPI attr_hot attr_nonnull(1) attr_release_shared_capability(2) 
void AMWCALL riven_unchain(
    struct riven       *riven,
    riven_chain_t       chain);

/** Combines the effects of 'rivens_split_work' and 'rivens_unchain', by providing a chain. 
 *  This function does not return until all submitted work has completed. */
attr_inline attr_nonnull(1,2) 
void AMWCALL riven_split_work_and_unchain(
    struct riven       *riven,
    struct riven_work  *work,
    u32                 work_count)
{
    riven_chain_t chain = NULL;
    riven_split_work(riven, work, work_count, &chain);
    riven_unchain(riven, chain);
}

/** Acquire a valid chain outside of the work queue, not bound to any existing work. The chain must 
 *  be released by external means by another thread by calling 'rivens_release_chained'. Any calls 
 *  to 'rivens_unchain' will result in a context switch, until the chain has been released. */
AMWAPI attr_hot attr_nonnull(1) attr_acquire_shared_capability(2)
void AMWCALL riven_acquire_chained(
    struct riven       *riven,
    riven_chain_t      *chain);

/** Releases a chain previously acquired by 'rivens_acquire_chained'. */
attr_inline attr_release_shared_capability(1)
void AMWCALL riven_release_chained(riven_chain_t chain)
{
    at_usize *counter = chain;
    if (counter) atomic_store_explicit(counter, 0lu, memory_order_release);
}

/** Returns the thread index of the current thread. The index is acquired by a hash lookup of the thread id. */
AMWAPI attr_hot attr_nonnull(1)
u32 AMWCALL riven_thread_index(struct riven *riven);

/** An unique tag groups resources of a shared lifetime, to be freed all at once. */
typedef u32 riven_tag_t;

#define riven_tag_game_forward_cycle         (2u)
#define riven_tag_game_to_rendering_cycle    (3u)
#define riven_tag_game_to_gpuexec_cycle      (4u)
#define riven_tag_rendering_to_gpuexec_cycle (3u)

/** Predefined tags for expected lifetime frequencies of game resources. Tags of other values can be 
 *  used for other uses, like allocating memory for assets: scenes, textures, meshes, audio, etc. */
enum riven_tag {
    riven_tag_invalid = 0u,
    /** Resources under this tag cannot be freed, they will share the lifetime of Riven. */
    riven_tag_roots,
    /** Scratch memory for the GPU execution stage. */
    riven_tag_gpuexec,
    /** Scratch memory for the rendering stage. */
    riven_tag_rendering,
    /** Scratch memory for the game stage. */
    riven_tag_game,
    /** Simulation to rendering stage, e.g. object instance arrays (cycled by frame index modulo 3). */
    riven_tag_game_to_rendering,
    /** Simulation to GPU execution stage, e.g. skinning matrices (cycled by frame index modulo 3). */
    riven_tag_game_to_gpuexec = riven_tag_game_to_rendering + riven_tag_game_to_rendering_cycle,
    /** Rendering to GPU execution stage, e.g command buffers (cycled by frame index modulo 3). */
    riven_tag_rendering_to_gpuexec = riven_tag_game_to_gpuexec + riven_tag_game_to_gpuexec_cycle,
    /** A minimum count of tagged heaps that will be in use. */
    riven_tag_reserved_count = riven_tag_rendering_to_gpuexec + riven_tag_rendering_to_gpuexec_cycle,
};
/** For scratch allocations outside of the parallel gameloop. */
#define riven_tag_deferred UINT32_MAX

/** Holds allocation rules, either defined by the caller side or from a query in the backend system.
 *  I assert that interfaces are not responsible for allocations of a different lifetime frequency 
 *  than the tag it was given at creation - and so the application will hold responsibility for 
 *  performing the host allocation of different resources that may be created.
 *
 *  A rule of thumb is, functions that accept this structure in a *create_info type of argument, or directly
 *  in the function argument list, then they expect a host allocation for resources that don't have an lifetime 
 *  that is expected by the system. The application can either pass a tag, so the allocation can be done 
 *  internally on a tagged heap, or the backend may query it's memory requirements, write this information 
 *  here, and expect a second call with the allocation done by the caller. */
struct riven_memory {
    void       *data;       /**< The allocated memory, written here after receiving results of a query. */
    usize       size;       /**< Size requirements for the allocation, written to from a query. */
    u32         alignment;  /**< Alignment requirements for the allocation, written to from a query. */
    riven_tag_t tag;        /**< Optionally, a valid tag can be given for the backend to allocate his memory from riven. */
};

/** Advises on commitment of host memory resources. Returns non-zero value if changes were made.
 *  On growth request, the heap will only commit if there is not enough contiguous space. On freeing
 *  resources, changes will be made only if the heap can be safely trimmed. */
AMWAPI attr_nonnull_all
usize AMWCALL riven_advise(
    struct riven *riven,
    usize         size,
    b32           release);

/** Allocates memory of any size under a given heap tag. Different allocation strategies are used 
 *  depending on internal parameters of Riven, on the tag, and on the requested size. */
AMWAPI attr_hot attr_nonnull(1) attr_malloc
void *AMWCALL riven_alloc(
    struct riven *riven,
    riven_tag_t   tag,
    usize         size,
    usize         alignment);

/** Frees resources that were allocated under a given tagged heap. */
AMWAPI attr_hot attr_nonnull_all
void AMWCALL riven_free(
    struct riven *riven,
    riven_tag_t   tag);

/** Rotate the deffered thread-local heaps and free just the last resources. */
AMWAPI attr_hot attr_nonnull_all
void AMWCALL riven_rotate_deferred(struct riven *riven);

/** Duplicates some data under a given heap tag. The newly allocated data will comply with 
 *  the alignment rule provided, set alignment as 1 to ignore it. */
attr_inline attr_nonnull_all
void *AMWCALL riven_memdup(
    struct riven        *riven,
    riven_tag_t          tag,
    const void *restrict data,
    usize                size,
    usize                alignment)
{
    return memcpy(riven_alloc(riven, tag, size, alignment), data, size);
}

/** Duplicates a string under a given heap tag, the length of the string must be known. */
attr_inline attr_nonnull_all
struct str riven_strdup(
    struct riven     *riven,
    riven_tag_t       tag,
    const struct str  s)
{
    return (struct str){ (const char *)riven_memdup(riven, tag, s.ptr, s.length+1, 1), s.length };
}

/** Duplicates a C string literal under a given heap tag. */
attr_inline attr_nonnull_all
struct str riven_cstrdup(
    struct riven *riven,
    riven_tag_t   tag,
    const char   *cstr)
{
    return riven_strdup(riven, tag, (struct str){ cstr, strlen(cstr) });
}

/** Format a string and save it under dest. The string will have a lifetime of tag. */
AMWAPI attr_hot attr_nonnull(1,3) attr_printf(4,5)
void AMWCALL riven_format_string(
    struct riven      *riven,
    riven_tag_t        tag,
    struct str * const dest,
    const char        *fmt, ...);

/** Concatenate a list of strings and save it under dest. The string will have a lifetime of tag. */
AMWAPI attr_hot attr_nonnull(1,3)
void AMWCALL riven_concatenate_strings(
    struct riven      *riven,
    riven_tag_t        tag,
    struct str * const dest,
    const struct str  *src,
    usize              count);

/** Implements an interface. */
#define RIVEN_ENCORE(interface, variant) \
    void AMWCALL interface##_encore_##variant(struct interface##_encore *encore)

/** Definitions of reserved encores, when there is no valid implementation yet. */
#define RIVEN_ENCORE_STUB(interface, variant) \
    RIVEN_ENCORE(interface, variant) { log_error("'%s' is not yet implemented.", __func__); (void)encore; }

/** Allows abstracting away the create_info structures present within different engine systems,
 *  that expose an interface. It shares riven's structures and a destination pointer to the interface. */
struct riven_encore_header {
    const struct amw_metadata *metadata;       /**< The metadata given by the application. */
    struct riven              *riven;          /**< The context of our framework. */
    riven_tag_t                tag;            /**< The lifetime of this interface. */
    void                     **interface;      /**< Write destination. */
};

/** Allows abstracting away the interface structures present within different engine systems.
 *  It shares all common state and callbacks an interface may either use or want to implement. */
struct riven_interface_header {
    struct str                      name;           /**< The name of the display backend. */
    struct riven                   *riven;          /**< The context of our framework. */
    riven_tag_t                     tag;            /**< The lifetime of this interface. */

    /** Used to destroy the interface. */
    PFN_riven_work                  fini;
};

/** Obtain the 'fini' procedure to close the interface implementation. */
attr_inline attr_pure attr_nonnull_all
PFN_riven_work AMWCALL riven_interface_fini(const void *interface) {
    if (interface) {
        const struct riven_interface_header *header = (const struct riven_interface_header *)interface;
        return header->fini;
    }
    return riven_work_nop;
}

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
AMWAPI attr_nonnull(8,9)
s32 AMWCALL riven_moonlit_walk(
    usize           memory_budget_size,
    usize           fiber_stack_size,
    u32             fiber_count,
    u32             thread_count,
    u32             tagged_heap_count,
    u32             log2_work_count,
    u32             log2_memory_count,
    PFN_riven_heart main_procedure,
    void           *main_argument);

#ifdef __cplusplus
}
#endif
