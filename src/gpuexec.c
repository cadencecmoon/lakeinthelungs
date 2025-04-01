#include <amw/a_moonlit_walk.h>

void a_moonlit_walk_gpuexec(struct amw_framedata *restrict work)
{
    if (!work) return;

    struct a_moonlit_walk_engine *amw = work->amw;
    struct reznor_interface *reznor = (struct reznor_interface *)amw->reznor;

    struct riven *riven = amw->riven;
    struct riven_work render_device_work[AMW_MAX_RENDERING_DEVICE_COUNT];
    riven_chain_t render_device_chain = NULL;

    for (u32 i = 0; i < amw->render_device_count; i++) {
        render_device_work[i].procedure = (PFN_riven_work)reznor->frame_submit;
        render_device_work[i].argument = work->render_device_frames[i];
        render_device_work[i].name = str_init("amw:gpuexec:frame_submit");
        assert_debug(render_device_work[i].argument);
    }
    riven_split_work(riven, render_device_work, amw->render_device_count, &render_device_chain);

    riven_unchain(riven, render_device_chain);
    /* release resources of the currently processed frame */
    riven_free(amw->riven, work->game_to_gpuexec_tag);
    riven_free(amw->riven, work->rendering_to_gpuexec_tag);
    riven_free(amw->riven, riven_tag_gpuexec);
}
