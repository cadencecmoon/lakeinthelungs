#include "lake.h"

void lake_in_the_lungs_gpuexec(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct rivens *riven = lake->riven;

    if (!work) return;

    /* release resources of the currently processed frame */
    riven_free(riven, rivens_tag_simulation_to_gpuexec + (work->index % rivens_tag_simulation_to_gpuexec_cycle));
    riven_free(riven, rivens_tag_rendering_to_gpuexec + (work->index % rivens_tag_rendering_to_gpuexec_cycle));
    riven_free(riven, rivens_tag_gpuexec);
}
