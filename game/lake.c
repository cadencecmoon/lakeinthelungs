#define A_MOONLIT_WALK_FRAMEWORK
#include "lake.h"

/* variables for a sphere of 100 faces */
#define SLICES 10
#define STACKS 7
#define VERTICES_COUNT (2 + (STACKS-2) * SLICES)
#define INDICES_COUNT (100 * 3)

static void lake_encore_zero_ref_callback(struct pelagial_encore *lake)
{
    (void)lake;
}

static FN_RIVEN_ENCORE(pelagial, lake)
{
    (void)metadata;
    (void)riven_hints;

    struct a_moonlit_walk_engine *amwe = (struct a_moonlit_walk_engine *)userdata;

    struct pelagial_encore *lake = (struct pelagial_encore *)
        riven_thalloc(riven, tag, lake_sizeof(struct pelagial_encore), lake_alignof(struct pelagial_encore));
    bedrock_zerop(lake);

    lake->amwe = amwe;

    /* write interface header */
    lake->interface.header.riven = riven;
    lake->interface.header.tag = tag;
    lake->interface.header.name = "pelagial";
    lake->interface.header.backend = "lake";
    lake->interface.header.zero_ref_callback = (PFN_riven_work)lake_encore_zero_ref_callback;
    return lake; 
}

PFN_riven_framework pelagial_main(
    struct riven_hints         *hints,
    struct pelagial_metadata   *metadata)
{
    metadata->app_name = "Lake in the Lungs";
    metadata->app_build_ver = AMWE_VERSION;
    hints->thread_count = 0; /* debug */

    bedrock_log_set_verbose(metadata->argc-1);
    amwe_hint_framework(AMWE_HINT_PIPELINE_SETTING, amwe_hint_pipeline_setting_auto);

    amwe_hint_encore(AMWE_HINT_ENCORE_PELAGIAL, (PFN_riven_encore)pelagial_encore_lake);
    return (PFN_riven_framework)a_moonlit_walk;
}
