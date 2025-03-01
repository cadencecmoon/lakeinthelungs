#include "lake.h"

void lake_in_the_lungs_gpuexec(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct rivens *riven = lake->riven;

    if (!work) return;

    /* release resources of the currently processed frame */
    riven_free(riven, rivens_tag_skinning_matrices + (work->index % rivens_tag_skinning_matrices_cycle));
    riven_free(riven, rivens_tag_command_buffers + (work->index % rivens_tag_command_buffers_cycle));
    riven_free(riven, rivens_tag_gpuexec);
    work->type = work_type_continue;
}
