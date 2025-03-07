#include "lake.h"

void lake_in_the_lungs_rendering(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct rivens *riven = lake->riven;

    if (!work) return;

    /* release resources of the currently processed frame */
    riven_free(riven, rivens_tag_simulation_to_rendering + (work->index % rivens_tag_simulation_to_rendering_cycle));
    riven_free(riven, rivens_tag_rendering);
}
