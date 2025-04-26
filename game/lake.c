#define A_MOONLIT_WALK_FRAMEWORK
#include "lake.h"

/* variables for a sphere of 100 faces */
#define SLICES 10
#define STACKS 7
#define VERTICES_COUNT (2 + (STACKS-2) * SLICES)
#define INDICES_COUNT (100 * 3)

static void lake_encore_zero_ref(struct lake_encore *lake)
{
    bedrock_zerop(lake);
}

static FN_RIVEN_ENCORE(lake, in_the_lungs)
{
    struct a_moonlit_walk_engine *amwe = (struct a_moonlit_walk_engine *)context->app->engine;

    struct lake_encore *lake = (struct lake_encore *)
        riven_thalloc(context->self, tag, lake_sizeof(struct lake_encore), lake_alignof(struct lake_encore));
    bedrock_zerop(lake);

    /* write interface header */
    lake->interface.riven.context = *context;
    lake->interface.riven.tag = tag;
    lake->interface.riven.name = "lake_in_the_lungs";
    lake->interface.riven.zero_ref_callback = (PFN_riven_work)lake_encore_zero_ref;

    lake->interface.amwe = amwe;
    return lake; 
}

PFN_riven_framework lake_main(
    struct riven_hints *hints,
    struct riven_app   *app)
{
    app->app_name = "Lake in the Lungs";
    app->app_build_ver = AMWE_VERSION;
    hints->thread_count = 0; /* debug */

    bedrock_log_set_verbose(app->argc-1);
    amwe_hint_framework(AMWE_HINT_PIPELINE_SETTING, amwe_hint_pipeline_setting_auto);

    amwe_hint_encore(AMWE_HINT_ENCORE_LAKE, (PFN_riven_encore)lake_encore_in_the_lungs);
    return (PFN_riven_framework)a_moonlit_walk;
}
