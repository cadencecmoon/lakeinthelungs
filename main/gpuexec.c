#include "lake.h"

void lake_in_the_lungs_gpuexec(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct riven *riven = lake->riven;

    if (!work) return;

    /* release resources of the currently processed frame */
    riven_free(riven, riven_tag_simulation_to_gpuexec + (work->index % riven_tag_simulation_to_gpuexec_cycle));
    riven_free(riven, riven_tag_rendering_to_gpuexec + (work->index % riven_tag_rendering_to_gpuexec_cycle));
    riven_free(riven, riven_tag_gpuexec);
}
