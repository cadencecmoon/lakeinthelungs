#pragma once

#include <amw/bedrock.h>
#include <amw/threads.h>

struct rivens;

/** An unique tag groups resources of a shared lifetime, to be freed all at once. */
typedef u32 rivens_tag_t;

#define rivens_tag_simulate_forward_cycle  (2u)
#define rivens_tag_instance_arrays_cycle   (3u)
#define rivens_tag_skinning_matrices_cycle (3u)
#define rivens_tag_command_buffers_cycle   (3u)

/** Predefined tags for expected lifetime frequencies of game resources. Tags of other values can be 
 *  used for other uses, like allocating memory for assets: scenes, textures, meshes, audio, etc. */
enum rivens_tag {
    /** Not a valid lifetime, for internal use. */
    rivens_tag_invalid = UINT32_MAX,
    /** Resources under this tag cannot be freed, they will share the lifetime of Riven. */
    rivens_tag_roots = 0u,
    /** For resources to live through to the next frame (cycled by frame index modulo 2). */
    rivens_tag_simulate_forward,
    /** Simulation to rendering stage, e.g. object instance arrays (cycled by frame index modulo 3). */
    rivens_tag_instance_arrays = rivens_tag_simulate_forward + rivens_tag_simulate_forward_cycle,
    /** Simulation to GPU execution stage, e.g. skinning matrices (cycled by frame index modulo 3). */
    rivens_tag_skinning_matrices = rivens_tag_instance_arrays + rivens_tag_instance_arrays_cycle,
    /** Rendering to GPU execution stage, e.g command buffers (cycled by frame index modulo 3). */
    rivens_tag_command_buffers = rivens_tag_skinning_matrices + rivens_tag_skinning_matrices_cycle,
    /** A minimum count of tagged heaps that will be in use. */
    rivens_tag_reserved_count = rivens_tag_command_buffers + rivens_tag_command_buffers_cycle,
    /** Scratch memory for the simulation stage. */
    rivens_tag_simulation = rivens_tag_reserved_count,
    /** Scratch memory for the rendering stage. */
    rivens_tag_rendering,
    /** Scratch memory for the GPU execution stage. */
    rivens_tag_gpuexec,
    /** The last reserved tag, doesn't correspond to the internal tagged heaps. */
    rivens_tag_true_count,
};

/** A chain represents an atomic counter that is bound to a job queue. If used within a 
 *  call to Riven's job system, it will block until all jobs associated with the chain finish. 
 *  In the meantime, the caller does not 'wait', instead an implicit context switch is made so
 *  that the caller's thread joins to complete the work on the job queue. */
typedef at_usize *rivens_chain_t;

/** An argument type, can be used to cast any pointer into a type valid for the job procedure. */
typedef void *rivens_arg_t;

/** Defines a job function, that can be run in parallel. */
typedef void (*PFN_rivens_job)(rivens_arg_t argument);

/** Applications entry point to the entire system. */
typedef s32 (*PFN_rivens_heart)(
    struct rivens  *riven, 
    rivens_arg_t    argument);

/** Defines a job that will be running within a fiber's context. Name is used for profilling, can be NULL. */
struct rivens_work {
    PFN_rivens_job  procedure;
    rivens_arg_t    argument;
    const char     *name;
};

/** Runs 'work_count' amount of jobs, passed in the flat array 'work'. This will return immediately, and the 
 *  provided jobs will be resolved in the background in parallel. If 'chain' is not NULL, it will be set to 
 *  a value that can be used to wait for the work to finish in the meantime. */
AMWAPI attr_hot attr_nonnull(1,2) attr_acquire_shared_capability(4)
void riven_split_work(
    struct rivens      *riven,
    struct rivens_work *work,
    u32                 work_count,
    rivens_chain_t     *chain);

/** If a chain is not NULL, then an implicit context switch is made for the calling thread, to the work that 
 *  was associated with the chain to be finished. If the chain is NULL, then this call may or may not be yield
 *  to the job system before returning. Chains can be acquired outside of a job queue, to be used as 'locks' 
 *  that continue work with a context switch instead of busy-waiting for the work to finish. */
AMWAPI attr_hot attr_nonnull(1) attr_release_shared_capability(2)
void riven_unchain(
    struct rivens *riven,
    rivens_chain_t chain);

/** Combines the effects of 'riven_split_work' and 'riven_unchain', combined by a chain. This function does not 
 *  return until all submitted work has completed. */
attr_inline attr_nonnull(1,2)
void riven_split_work_and_unchain(
    struct rivens      *riven,
    struct rivens_work *work,
    u32                 work_count)
{
    rivens_chain_t chain;
    riven_split_work(riven, work, work_count, &chain);
    riven_unchain(riven, chain);
}

/** Acquire a valid chain outside of the work queue, not bound to any existing work. The chain must be released 
 *  by external means by another thread by calling 'riven_release_exile'. Until then, any calls to 'riven_unchain'
 *  will result in a context switch, until the exiled chain has been released. */
AMWAPI attr_hot attr_nonnull(1) attr_acquire_shared_capability(2)
void riven_acquire_exile(
    struct rivens  *riven,
    rivens_chain_t *chain);

/** Releases a chain previously acquired by 'riven_acquire_exile'. */
attr_inline attr_release_shared_capability(1)
void riven_release_exile(rivens_chain_t chain)
{
    at_usize *counter = chain;
    if (counter) atomic_store_explicit(counter, 0lu, memory_order_release);
}

/** Returns the thread index of the current thread. The index is acquired by a hash lookup of the thread id.
 *  The out_thread_count is optional, if one wishes to retrieve the total number of threads in the system. */
AMWAPI attr_hot attr_nonnull(1)
u32 riven_thread_index(struct rivens *riven, u32 *out_thread_count);

/** Allocates memory of any size under a given heap tag. Different allocation strategies are used 
 *  depending on internal parameters of Riven, on the tag, and on the requested size. */
AMWAPI attr_hot attr_nonnull(1) attr_malloc
void *riven_alloc(
    struct rivens *riven,
    rivens_tag_t   tag,
    usize          size,
    usize          alignment);

/** Frees resources that were allocated under a given tagged heap. */
AMWAPI attr_hot attr_nonnull(1)
void riven_free(
    struct rivens *riven,
    rivens_tag_t   tag);

/** Advises on commitment of host memory resources. Returns non-zero value if changes were made.
 *  On growth request, the heap will only commit if there is not enough contiguous space. On freeing
 *  resources, changes will be made only if the heap can be safely trimmed. */
AMWAPI attr_nonnull(1)
usize riven_advise(
    struct rivens *riven,
    usize          size,
    b32            release);

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
AMWAPI s32 riven_moonlit_walk(
    usize               memory_budget_size,
    usize               fiber_stack_size,
    u32                 fiber_count,
    u32                 thread_count,
    u32                 tagged_heap_count,
    u32                 log2_work_count,
    u32                 log2_memory_count,
    PFN_rivens_heart    main_procedure,
    rivens_arg_t        main_argument);
