/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <amw/a_moonlit_walk.h>

#include <stdio.h>

AMW_RIVENS_TEARS(a_moonlit_walk_gpuexec_tear__)
{
    struct framedata *frame = (struct framedata *)argument;

    if (!frame) /* If there is no work, return early. */
        return;

    fprintf(stderr, "gpuexec %d\n", frame->frame_idx);
}
