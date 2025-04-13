#include "internal.h"

s32 a_moonlit_walk(
    struct riven                   *riven,
    const struct riven_hints       *riven_hints,
    bedrock_thread_t               *threads,
    struct pelagial_metadata       *metadata)
{
    (void)riven;
    (void)riven_hints;
    (void)threads;
    (void)metadata;

    bedrock_log_info("miau");

    return 0;
}

s32 a_moonlit_walk_entry_point__(
    PFN_riven_framework (LAKECALL *app_main)(
        struct pelagial_metadata *, struct riven_hints *, s32 argc, char **argv),
    struct pelagial_metadata *metadata, s32 argc, char **argv)
{
    /* provide defaults */
    struct riven_hints riven_hints = {
        .memory_budget = 1lu*1024*1024*1024, /* 1GB */
        .fiber_stack_size = 64lu*1024, /* 64KB */
        .fiber_count = 0,
        .tagged_heap_count = 0,
        .thread_count = 0,
        .log2_job_count = 0,
        .log2_arena_count = 0,
    };

    PFN_riven_framework framework = app_main(metadata, &riven_hints, argc, argv);
    if (!framework)
        return 0;

    return riven_moonlit_walk(&riven_hints, framework, metadata);
}
