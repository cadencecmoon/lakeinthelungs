#include "lake.h"

void lake_in_the_lungs_rendering(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct rivens *riven = lake->riven;

    if (!work) return;

    /* assembly state */

    if (work->type == work_type_disassembly)
        return;

    /* release resources of the currently processed frame */
    riven_free(riven, rivens_tag_instance_arrays + (work->index % rivens_tag_instance_arrays_cycle));
    riven_free(riven, rivens_tag_rendering);
}
