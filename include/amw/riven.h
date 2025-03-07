#pragma once

#include <amw/bedrock.h>
#include <amw/string.h>
#include <amw/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rivens;

/** Information about the application. */
struct rivens_metadata {
    const struct str game_name;
    const struct str engine_name;
    u32              game_build_version;
    u32              engine_build_version;
};

/** A chain represents an atomic counter that is bound to a job queue. If used within a 
 *  call to Riven's job system, it will block until all jobs associated with the chain finish. 
 *  In the meantime, the caller does not 'wait', instead an implicit context switch is made so
 *  that the caller's thread joins to complete the work on the job queue. */
typedef at_usize *rivens_chain_t;

/** An argument type, can be used to cast any pointer into a type valid for a pointer argument.
 *  It can represent any data type that is valid for the procedure callback. */
typedef void *rivens_song_t;

/** Defines a job function, that can be run in parallel. */
typedef void (*PFN_rivens_job)(rivens_song_t argument);

/** Defines a job that will be running within a fiber's context. Name is used for profilling, can be NULL. */
struct rivens_work {
    PFN_rivens_job  procedure;
    rivens_song_t   argument;
    struct str      name;
};

/** Applications entry point to the entire system. */
typedef s32 (*PFN_rivens_heart)(
    struct rivens *riven, 
    thread_t      *threads,
    u32            thread_count,
    rivens_song_t  argument);

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

/** Combines the effects of 'rivens_split_work' and 'rivens_unchain', by providing a chain. 
 *  This function does not return until all submitted work has completed. */
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

/** Acquire a valid chain outside of the work queue, not bound to any existing work. The chain must 
 *  be released by external means by another thread by calling 'rivens_release_chained'. Any calls 
 *  to 'rivens_unchain' will result in a context switch, until the chain has been released. */
AMWAPI attr_hot attr_nonnull(1) attr_acquire_shared_capability(2)
void riven_acquire_chained(
    struct rivens  *riven,
    rivens_chain_t *chain);

/** Releases a chain previously acquired by 'rivens_acquire_chained'. */
attr_inline attr_release_shared_capability(1)
void riven_release_chained(rivens_chain_t chain)
{
    at_usize *counter = chain;
    if (counter) atomic_store_explicit(counter, 0lu, memory_order_release);
}

/** Returns the thread index of the current thread. The index is acquired by a hash lookup of the thread id. */
AMWAPI attr_hot attr_nonnull(1)
u32 riven_thread_index(struct rivens *riven);

/** An unique tag groups resources of a shared lifetime, to be freed all at once. */
typedef u64 rivens_tag_t;

#define rivens_tag_simulation_forward_cycle      (2llu)
#define rivens_tag_simulation_to_rendering_cycle (3llu)
#define rivens_tag_simulation_to_gpuexec_cycle   (4llu)
#define rivens_tag_rendering_to_gpuexec_cycle    (3llu)

/** Predefined tags for expected lifetime frequencies of game resources. Tags of other values can be 
 *  used for other uses, like allocating memory for assets: scenes, textures, meshes, audio, etc. */
enum rivens_tag {
    /** Not a valid lifetime, for internal use. */
    rivens_tag_invalid = UINT64_MAX,
    /** Resources under this tag cannot be freed, they will share the lifetime of Riven. */
    rivens_tag_roots = 0llu,
    /** For resources to live through to the next frame (cycled by frame index modulo 2). */
    rivens_tag_simulation_forward,
    /** Simulation to rendering stage, e.g. object instance arrays (cycled by frame index modulo 3). */
    rivens_tag_simulation_to_rendering = rivens_tag_simulation_forward + rivens_tag_simulation_forward_cycle,
    /** Simulation to GPU execution stage, e.g. skinning matrices (cycled by frame index modulo 3). */
    rivens_tag_simulation_to_gpuexec = rivens_tag_simulation_to_rendering + rivens_tag_simulation_to_rendering_cycle,
    /** Rendering to GPU execution stage, e.g command buffers (cycled by frame index modulo 3). */
    rivens_tag_rendering_to_gpuexec = rivens_tag_simulation_to_gpuexec + rivens_tag_simulation_to_gpuexec_cycle,
    /** A minimum count of tagged heaps that will be in use. */
    rivens_tag_reserved_count = rivens_tag_rendering_to_gpuexec + rivens_tag_rendering_to_gpuexec_cycle,
    /** Scratch memory for the simulation stage. */
    rivens_tag_simulation = rivens_tag_reserved_count,
    /** Scratch memory for the rendering stage. */
    rivens_tag_rendering,
    /** Scratch memory for the GPU execution stage. */
    rivens_tag_gpuexec,
    /** The last reserved tag, doesn't correspond to the internal tagged heaps. */
    rivens_tag_true_count,
};
/** For scratch allocations outside of the parallel gameloop. */
#define rivens_tag_drifter rivens_tag_simulation

/** Advises on commitment of host memory resources. Returns non-zero value if changes were made.
 *  On growth request, the heap will only commit if there is not enough contiguous space. On freeing
 *  resources, changes will be made only if the heap can be safely trimmed. */
AMWAPI attr_nonnull_all
usize riven_advise(
    struct rivens *riven,
    usize          size,
    b32            release);

/** Allocates memory of any size under a given heap tag. Different allocation strategies are used 
 *  depending on internal parameters of Riven, on the tag, and on the requested size. */
AMWAPI attr_hot attr_nonnull(1) attr_malloc
void *riven_alloc(
    struct rivens *riven,
    rivens_tag_t   tag,
    usize          size,
    usize          alignment);

/** Frees resources that were allocated under a given tagged heap. */
AMWAPI attr_hot attr_nonnull_all
void riven_free(
    struct rivens *riven,
    rivens_tag_t   tag);

/** Duplicates some data under a given heap tag. The newly allocated data will comply with 
 *  the alignment rule provided, set alignment as 1 to ignore it. */
attr_inline attr_nonnull_all
void *riven_memdup(
    struct rivens       *riven,
    rivens_tag_t         tag,
    const void *restrict data,
    usize                size,
    usize                alignment)
{
    return memcpy(riven_alloc(riven, tag, size, alignment), data, size);
}

/** Duplicates a string under a given heap tag, the length of the string must be known. */
attr_inline attr_nonnull_all
struct str riven_strdup(
    struct rivens    *riven,
    rivens_tag_t      tag,
    const struct str  s)
{
    return (struct str){ (const char *)riven_memdup(riven, tag, s.ptr, s.length+1, 1), s.length };
}

/** Duplicates a C string literal under a given heap tag. */
attr_inline attr_nonnull_all
struct str riven_cstrdup(
    struct rivens *riven,
    rivens_tag_t   tag,
    const char    *cstr)
{
    return riven_strdup(riven, tag, (struct str){ cstr, strlen(cstr) });
}

/** Format a string and save it under dest. The string will have a lifetime of tag. */
AMWAPI attr_hot attr_nonnull(1,3) attr_printf(4,5)
void riven_format_string(
    struct rivens     *riven,
    rivens_tag_t       tag,
    struct str * const dest,
    const char        *fmt, ...);

/** Concatenate a list of strings and save it under dest. The string will have a lifetime of tag. */
AMWAPI attr_hot attr_nonnull(1,3)
void riven_concatenate_strings(
    struct rivens     *riven,
    rivens_tag_t       tag,
    struct str * const dest,
    const struct str  *src,
    usize              count);

/** Used to cleanup a backend and destroy the interface. */
typedef void (*PFN_rivens_interface_fini)(rivens_song_t interface);

/** Check whether the interface implementation is complete. */
typedef b32 (*PFN_rivens_interface_validate)(rivens_song_t interface);

/** Information shared between all interfaces, may be used to cast from any defined interface structure.
 *  An engine system may want to define an interface in the following cases:
 *  
 *  1) The system may have multiple implementations, where one must be choosen at runtime - either 
 *     by platform-specific backends, a layer of indirection, or extensibility by external means 
 *     without touching and reprogramming the interface used by an application.
 *
 *  2) The system may be implemented as a plugin/extension and be loaded at runtime at will,
 *     or hot-reloaded without recompiling the engine itself. */
struct rivens_interface_header {
    struct str                      name;           /**< The name of the display backend. */
    struct rivens                  *riven;          /**< The context of our framework. */
    rivens_tag_t                    tag;            /**< The lifetime of this interface. */

    /* every interface must implement these callbacks */
    PFN_rivens_interface_fini       fini;
    PFN_rivens_interface_validate   validate;
};

/** Write the header within an interface initializer. */
#define riven_write_interface_header(_name, _fini, _validate)   \
    {                                                           \
        .name = _name,                                          \
        .tag = overture->header.tag,                            \
        .riven = overture->header.riven,                        \
        .fini = (PFN_rivens_interface_fini)_fini,               \
        .validate = (PFN_rivens_interface_validate)_validate,   \
    }

/** We may define a framework for abstracting away interfaces of systems with hidden implementation details,
 *  or that wish to have their functionality be extended under a common application interface. Three structures 
 *  must be declared by a system - assume the systems name is 'example':
 *
 *  - struct example_overture   :  Defines application provided configuration for the system, passed by the user.
 *  - struct example_interface  :  Defines shared state and pointers to procedures that must be implemented.
 *  - struct example            :  Opaque, implemented by the backend, it must have the interface as the first 
 *                                 struct member to allow type conversions between public and private code.
 *
 *  We can assume that an 'encore', defined as an entry point to the system, is enough to determine whether 
 *  an implementation is supported or not for a given host configuration - for example, whether it has the 
 *  required shared libraries installed, correctly setup in the PATH, or other up-to-date drivers. The system 
 *  and the backend is entrusted with implementing and validating it's specification. */
typedef rivens_song_t attr_nonnull_all (*PFN_rivens_encore)(rivens_song_t overture);

/** Declares a procedure that may be executed by the prime encore to initialize one's internal systems.
 *  This function must be declared by a specific implementation of such interface, that wishes to expose 
 *  itself under this logic, and to directly connect with Riven for it's resource management. */
#define RIVENS_ENCORE(system, variant) \
    struct system *attr_nonnull_all system##_##variant##_encore(struct system##_overture *overture)

/** A header for the rivens_encore system-specific overture argument definition. It will be automatically
 *  filled by passing it into 'riven_encore' or 'riven_improvise', before calling 'riven_prime_encore'. */
struct rivens_overture_header {
    struct rivens                *riven;
    rivens_tag_t                  tag;
    const PFN_rivens_encore      *encores;      /**< Executes the encores sequentially, until one writes a valid interface. */
    u32                           count;        /**< Maximum encores to try, executed in an order starting from index 0. */
    struct str                    name;         /**< The name of the system defining an encore. */
    rivens_song_t                *interface;    /**< The pointer to where our interface may be written. */
    const struct rivens_metadata *metadata;
};

/** Write the header within an overture initializer. */
#define riven_write_overture_header(_riven, _tag, _name, _interface, _encores, _count) \
    {                               \
        .riven = _riven,            \
        .tag = _tag,                \
        .encores = _encores,        \
        .count = _count,            \
        .name = _name,              \
        .interface = _interface,    \
    }

/** Executes an encore, saving the returned interface in the arguments header.
 *  This function expects that the given overture's header is written. Can be run as a job. */
AMWAPI attr_nonnull_all 
void riven_encore(rivens_song_t overture);

/** Closes and cleans up the interface. Can be run as a job. */
AMWAPI attr_nonnull_all
void riven_finale(rivens_song_t interface);

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
    usize                         memory_budget_size,
    usize                         fiber_stack_size,
    u32                           fiber_count,
    u32                           thread_count,
    u32                           tagged_heap_count,
    u32                           log2_work_count,
    u32                           log2_memory_count,
    const struct rivens_metadata *metadata,
    PFN_rivens_heart              main_procedure,
    rivens_song_t                 main_argument);

#ifdef __cplusplus
}
#endif
