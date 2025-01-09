#include <lake/a_moonlit_walk.h>

#define VALIDATE_RENDERING_STATE_SPLITS_COUNT 1
static s32 construct_the_rendering_state(struct amw_workload *work)
{
    struct cobalt      *cobalt  = &work->AMW->cobalt;
    struct hadopelagic *hadal   = &work->AMW->hadal;
    struct ipomoeaalba *ia      = &work->AMW->ia;
    struct riven       *riven   = work->AMW->riven;
    thread_id          *threads = work->AMW->threads;
    ssize          thread_count = work->AMW->thread_count;

    /* TODO here i can do stuff like building the pipelines, hot reloading shaders, allocating the command buffers */

    struct cobalt_construct_swapchain_work swapchain_work = {
        .cobalt = cobalt,
        .hadal = hadal,
        .ia = ia,
        .threads = threads,
        .thread_count = thread_count,
        .use_vsync = (at_read_relaxed(&cobalt->flags) & cobalt_flag_vsync_enabled) ? true : false,
        .result = result_success,
    };

    /* prepare the work */
    struct rivens_tear tears[VALIDATE_RENDERING_STATE_SPLITS_COUNT];
    ssize idx = 0;

    if (at_read_explicit(&hadal->flags, memory_model_acquire) & hadal_flag_recreate_swapchain)
        cobalt_construct_swapchain_tear__(&swapchain_work, &tears[idx++]);

    /* execute the work */
    riven_split_and_unchain(riven, tears, idx);

    s32 result = result_success;
    if (swapchain_work.result != result_success) {
        log_error("The cobalt_construct_swapchain_tear failed with result code '%d'.", swapchain_work.result);
        result = swapchain_work.result;
    } /* else if... */

    return result;
}

RIVENS_TEAR(a_moonlit_walk_rendering_tear__, struct amw_workload *work)
{
    /* If there is no work, return early. */
    if (!work) return;

    /* Setup and update the shared and frame-to-frame rendering state. */
    if (construct_the_rendering_state(work) != result_success) return;
}
