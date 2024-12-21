#include <lake/a_moonlit_walk.h>

RIVENS_TEAR(a_moonlit_walk_gpuexec_tear__)
{
    struct amw_framedata *frame = (struct amw_framedata *)argument;

    if (!frame) /* If there is no work, return early. */
        return;
}
