#include <amw/a_moonlit_walk.h>

void a_moonlit_walk_game(struct amw_framedata *restrict work)
{
    if (!work) return;

    struct a_moonlit_walk_engine *amw = work->amw;

    /* release resources of the currently processed frame */
    riven_free(amw->riven, riven_tag_game);
}
