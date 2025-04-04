#include <amw/a_moonlit_walk.h>

#include "vulkan/vk_reznor.h"

void a_moonlit_walk_gpuexec(struct amw_framedata *restrict work)
{
    if (!work) return;

    struct a_moonlit_walk_engine *amw = work->amw;
    struct reznor_interface *reznor = (struct reznor_interface *)amw->reznor;

    struct riven *riven = amw->riven;
    struct riven_work *render_device_work = (struct riven_work *)
        riven_alloc(riven, riven_tag_gpuexec, sizeof(struct riven_work) * amw->render_device_count, _Alignof(struct riven_work));
    riven_chain_t render_device_chain = NULL;

    /* submits recorded commands buffers for work on the GPU queues */
    for (u32 i = 0; i < amw->render_device_count; i++) {
        render_device_work[i].procedure = (PFN_riven_work)reznor->frame_submit;
        render_device_work[i].argument = work->render_device_frames[i];
        render_device_work[i].name = str_init("amw:gpuexec:frame_submit");
        assert_debug(render_device_work[i].argument);
    }
    riven_split_work(riven, render_device_work, amw->render_device_count, &render_device_chain);

    /* frame_submit */
    riven_unchain(riven, render_device_chain);
    render_device_chain = NULL;

    /* release resources of the currently processed frame */
    riven_free(amw->riven, work->game_to_gpuexec_tag);
    riven_free(amw->riven, work->rendering_to_gpuexec_tag);
    riven_free(amw->riven, riven_tag_gpuexec);
}
