#define A_MOONLIT_WALK_FRAMEWORK
#include "lake.h"

/* variables for a sphere of 100 faces */
#define SLICES 10
#define STACKS 7
#define VERTICES_COUNT (2 + (STACKS-2) * SLICES)
#define INDICES_COUNT (100 * 3)

PFN_riven_framework pelagial_main(
    struct pelagial_metadata   *metadata,
    struct riven_hints         *hints, 
    s32                         argc, 
    char                      **argv)
{
    (void)argv;
    bedrock_log_set_verbose(argc-1);

    metadata->engine_name = "A Moonlit Walk Engine";
    metadata->app_name = "Lake in the Lungs";
    metadata->engine_build_ver = AMWE_VERSION;
    metadata->app_build_ver = AMWE_VERSION;

    /* keep em at default for now */
    (void)hints;

    return (PFN_riven_framework)a_moonlit_walk;
}

static void lake_encore_fini(struct pelagial_encore *lake)
{
    (void)lake;
}

FN_RIVEN_ENCORE(pelagial, lake)
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
    lake->interface.header.encore_fini = (PFN_riven_work)lake_encore_fini;
    return lake; 
}
