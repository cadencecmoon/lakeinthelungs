#include "lake.h"

/** Limits the jobs array for the assembly phase of the rendering state. */
#define ASSEMBLY_JOBS_MAX 1
static void resolve_assembly(struct framedata *work)
{
    struct harridan *harridan = &work->lake->harridan;
    struct hadopelagic *hadal = &work->lake->hadal;
    struct rivens *riven = work->lake->riven;

    u32 harridan_flags = atomic_load_explicit(&harridan->flags, memory_order_acquire);
    u32 hadal_flags = atomic_load_explicit(&hadal->flags, memory_order_acquire);

    b32 do_assembly_work = ((work->type != work_type_continue));
    b32 do_swapchain_work = (do_assembly_work || (hadal_flags & hadal_flag_framebuffer_resized) || (harridan_flags & harridan_flag_surface_lost));

    struct harridan_orchestrate_swapchain_work swapchain_work = {
        .type = work->type,
        .harridan = harridan,
        .hadal = hadal,
        .swapchain = &harridan->swapchain,
        .use_vsync = (harridan_flags & harridan_flag_vsync_enabled) ? true : false,
    };

    s32 o = 0;
    struct rivens_work jobs[ASSEMBLY_JOBS_MAX];

    /* only commit to work that must be done */
    if (do_swapchain_work) harridan_orchestrate_swapchain_job__(&swapchain_work, &jobs[o++]);
    assert_debug(o <= ASSEMBLY_JOBS_MAX);

    /* execute the work */
    if (o > 0) riven_split_work_and_unchain(riven, jobs, o);

    /* check results */
    for (--o; o >= 0; o--) {
        struct work *w = (struct work *)jobs[o].argument;
        if (w->result != result_success) {
            log_error("Rendering assembly work for '%s' failed.", jobs[o].name);
            work->result = w->result;
        }
    }
}

void lake_in_the_lungs_rendering(struct framedata *work)
{
    struct lake *lake = work->lake;
    struct rivens *riven = lake->riven;

    if (!work) return;

    /* handle the rendering context before recording any commands */
    resolve_assembly(work); 
    if (work->result != result_success || work->type == work_type_disassembly)
        goto done;

done:
    /* release resources of the currently processed frame */
    riven_free(riven, rivens_tag_instance_arrays + (work->index % rivens_tag_instance_arrays_cycle));
    riven_free(riven, rivens_tag_rendering);
}
