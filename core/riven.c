/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/riven.h>

void riven_split_and_unchain(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits)
{
    (void)riven;
    (void)tears;
    (void)splits;
}

void riven_split(
        struct riven *riven,
        struct riven_tear *tears,
        size_t splits,
        riven_chain_t *chain)
{
    (void)riven;
    (void)tears;
    (void)splits;
    (void)chain;
}

void riven_unchain(
        struct riven *riven, 
        riven_chain_t chain)
{
    (void)riven;
    (void)chain;
}

void riven_chain_exile(
        struct riven *riven,
        riven_chain_t *chain)
{
    (void)riven;
    (void)chain;
}

void riven_unchain_exile(riven_chain_t chain)
{
    (void)chain;
}

size_t riven_unveil_rift(
        void *riven_memory,
        size_t fiber_stack_bytes,
        size_t fiber_count,
        size_t log_2_tear_count,
        size_t thread_count,
        PFN_riven_main main_procedure,
        riven_argument_t main_argument)
{
    (void)riven_memory;
    (void)fiber_stack_bytes;
    (void)fiber_count;
    (void)log_2_tear_count;
    (void)thread_count;
    (void)main_procedure;
    (void)main_argument;

    return 0;
}
