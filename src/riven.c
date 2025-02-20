#include <amw/riven.h>

void riven_split_work(
    struct rivens      *riven,
    struct rivens_work *work,
    u32                 work_count,
    rivens_chain_t     *chain)
{

}

void riven_unchain(
    struct rivens *riven,
    rivens_chain_t chain)
{

}

void riven_acquire_exile(
    struct rivens  *riven,
    rivens_chain_t *chain)
{

}

u32 riven_thread_index(struct rivens *riven)
{
    return 0;
}

void *riven_alloc(
    struct rivens *riven,
    rivens_tag_t   tag,
    usize          size,
    usize          alignment)
{
    return NULL;
}

void riven_free(
    struct rivens *riven,
    rivens_tag_t   tag)
{

}

s32 riven_go_for_a_walk(
    usize               memory_budget_size,
    usize               fiber_stack_size,
    u32                 fiber_count,
    u32                 thread_count,
    u32                 tagged_heap_count,
    u32                 log2_work_count,
    u32                 log2_memory_count,
    PFN_rivens_heart    main_procedure,
    rivens_arg_t        main_argument)
{
    return 0;
}
