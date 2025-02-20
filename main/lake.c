#include "lake.h"

#define DEBUG_CLOSE_COUNTER_MS 100

static const char * const g_stage_names[3] = {
    "lake_in_the_lungs:simulation",
    "lake_in_the_lungs:rendering",
    "lake_in_the_lungs:gpuexec",
};
    
struct hints {
    const char *engine_name;
    const char *game_name;
    u32         build_version;
    u32         frames_in_flight;

    usize       riven_memory_budget_size;
    usize       riven_fiber_stack_size;
    u32         riven_fiber_count;
    u32         riven_thread_count;
    u32         riven_tagged_heap_count;
    u32         riven_log2_work_count;
    u32         riven_log2_memory_count;
};

static void lake_in_the_lungs()

s32 main(s32 argc, char **argv)
{
    return 0;
}
