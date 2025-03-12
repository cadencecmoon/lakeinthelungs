#include "lake.h"

void lake_in_the_lungs_simulation(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct riven *riven = lake->riven;

    if (!work) return;

    /* release resources of the currently processed frame */
    riven_free(riven, riven_tag_simulation_forward + ((work->frame_index+1) % riven_tag_simulation_forward_cycle));
    riven_free(riven, riven_tag_simulation);
}
