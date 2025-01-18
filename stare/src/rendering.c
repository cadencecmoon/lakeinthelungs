/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/a_moonlit_walk.h>

static s32 assemble_core_rendering_state(
    struct a_moonlit_walk *AMW,
    b32                    startup_or_exit,
    b32                    dissasemble)
{
    struct pelagia     *pelagia = &AMW->pelagia;
    /* TODO silver, assemble the auditory rendering state */
    struct hadopelagic *hadal   = &AMW->hadal;
    struct riven       *riven   = AMW->riven;

    u32 pelagia_flags = at_read_explicit(&pelagia->flags, memory_model_acquire);
    u32 hadal_flags = at_read_explicit(&hadal->flags, memory_model_acquire);

    b32 do_swapchain_work = startup_or_exit || hadal_flags & hadal_flag_recreate_swapchain || pelagia_flags & pelagia_flag_swapchain_surface_lost;
    b32 do_render_targets_work = startup_or_exit;
    b32 do_uniform_buffers_work = startup_or_exit;
    b32 do_shader_pipelines_work = startup_or_exit;

    struct pelagia_assemble_swapchain_work assemble_swapchain_work = {
        .pelagia = pelagia,
        .hadal = hadal,
        .use_vsync = (at_read_relaxed(&pelagia->flags) & pelagia_flag_vsync_enabled) ? true : false,
        .dissasemble = dissasemble,
    };
    struct pelagia_assemble_render_targets_work assemble_render_targets = {
        .pelagia = pelagia,
        .render_targets = &AMW->render_targets,
        .dissasemble = dissasemble,
    };
    struct pelagia_assemble_uniform_buffers_work assemble_uniform_buffers = {
        .pelagia = pelagia,
        .uniform_buffers = &AMW->uniform_buffers,
        .dissasemble = dissasemble,
    };
    struct pelagia_assemble_shader_pipelines_work assemble_shader_pipelines = {
        .pelagia = pelagia,
        .riven = riven,
        .dissasemble = dissasemble,
    };

    struct rivens_tear tears[4];
    s32 idx = 0;

    if (do_swapchain_work)          pelagia_assemble_swapchain_tear__(&assemble_swapchain_work, &tears[idx++]);
    if (do_render_targets_work)     pelagia_assemble_render_targets_tear__(&assemble_render_targets, &tears[idx++]);
    if (do_uniform_buffers_work)    pelagia_assemble_uniform_buffers_tear__(&assemble_uniform_buffers, &tears[idx++]);
    if (do_shader_pipelines_work)   pelagia_assemble_shader_pipelines_tear__(&assemble_shader_pipelines, &tears[idx++]);

    if (idx > 0) {
        /* FIXME find a better way to synchronize the work between rendering and GPU execution */
        pelagia->device.api.vkDeviceWaitIdle(pelagia->device.logical);
        riven_split_and_unchain(riven, tears, idx);
    }

    s32 result = result_success;
    for (--idx; idx >= 0; idx--) {
        struct pelagia_work_result *work = (struct pelagia_work_result *)tears[idx].argument;
        if (work->out_result != result_success) {
            log_error("Rendering: tear '%s' failed and returned %d.", tears[idx].name, work->out_result);
            result = work->out_result;
        }
    }
    return result;
}

void a_moonlit_walk_rendering_tear__(struct amw_workload *work)
{
    /* If there is no work, return early. */
    if (!work) return;

    struct a_moonlit_walk *AMW = work->AMW;
    b32 startup_or_exit = (work->context == amw_workload_context_startup || work->context == amw_workload_context_exit);
    b32 dissasemble = (work->context == amw_workload_context_exit);
    
    s32 result = assemble_core_rendering_state(AMW, startup_or_exit, dissasemble);

    if (dissasemble) { 
        return;
    } else if (result != result_success) {
        at_fetch_or_explicit(&AMW->flags, amw_flag_forced_exit, memory_model_release);
        return;
    }

    /* TODO:
     * - render graph construction (specify inputs and outputs, non parallel by design)
     * - evaluate the render graph (highly parallel, record high level commands)
     * - compile the rendering commands (backend record command buffers per swapchain image)
     * - complete the work, pass command buffers to GPU execution */
}
