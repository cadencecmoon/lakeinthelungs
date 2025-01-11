/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/a_moonlit_walk.h>

static s32 prepare_the_rendering_state(struct amw_workload *work)
{
    struct pelagia     *pelagia = &work->AMW->pelagia;
    struct hadopelagic *hadal   = &work->AMW->hadal;
    struct ipomoeaalba *ia      = &work->AMW->ia;
    struct riven       *riven   = work->AMW->riven;
    //thread_id          *threads = work->AMW->threads;
    //ssize          thread_count = work->AMW->thread_count;

    /* TODO here i can do stuff like building the pipelines, hot reloading shaders, 
     * allocating the command buffers, prepare audio streams for the mixer, etc. */

    struct pelagia_construct_swapchain_work swapchain_work = {
        .pelagia = pelagia,
        .hadal = hadal,
        .ia = ia,
        .use_vsync = (at_read_relaxed(&pelagia->flags) & pelagia_flag_vsync_enabled) ? true : false,
        .out_result = result_success,
    };

    struct rivens_tear tears[1];
    ssize idx = 0;

    if (at_read_explicit(&hadal->flags, memory_model_acquire) & hadal_flag_recreate_swapchain)
        pelagia_construct_swapchain_tear__(&swapchain_work, &tears[idx++]);

    /* execute the work */
    riven_split_and_unchain(riven, tears, idx);

    s32 result = result_success;
    if (swapchain_work.out_result != result_success) {
        log_error("The cobalt_construct_swapchain_tear failed with result code '%d'.", swapchain_work.out_result);
        result = swapchain_work.out_result;
    } /* else if... */

    return result;
}

RIVENS_TEAR(a_moonlit_walk_rendering_tear__, struct amw_workload *work)
{
    /* If there is no work, return early. */
    if (!work) return;

    /* Setup and update the shared auditory and visual rendering state. */
    if (prepare_the_rendering_state(work) != result_success) return;

    /* TODO:
     * - render graph construction (specify inputs and outputs, non parallel by design)
     * - evaluate the render graph (highly parallel, record high level commands)
     * - compile the rendering commands (backend record command buffers per swapchain image)
     * - complete the work, pass command buffers to GPU execution */
}
