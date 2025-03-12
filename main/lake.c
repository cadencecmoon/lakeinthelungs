#define A_MOONLIT_WALK_MAIN
#include "lake.h"

static_assert(MAX_FRAMES_IN_FLIGHT >= 3, "AMW_MAX_FRAMES_IN_FLIGHT must be 3 or more.");

/* XXX delete later */
#define DEBUG_CLOSE_COUNTER_MS 100

#define FRAME_TIME_PRINT_INTERVAL_MS 1000

struct engine_hints {
    u32                 frames_in_flight;

    u32                 window_width, window_height;
    const struct str    window_title;

    s32                 pelagia_force_primary_device_index;
    u32                 pelagia_device_max_count;
    u32                 pelagia_virtual_device_count;

    usize               riven_memory_budget_size;
    usize               riven_fiber_stack_size;
    u32                 riven_fiber_count;
    u32                 riven_thread_count;
    u32                 riven_tagged_heap_count;
    u32                 riven_log2_work_count;
    u32                 riven_log2_memory_count;

    struct {
        PFN_riven_work  octavia;
        PFN_riven_work  pelagia;
        PFN_riven_work  hadal;
    } encores;

    b32                 verbose;
    b32                 debug_utilities;
};

static void lake_fini(struct lake *lake)
{
    if (lake->devices) {
        struct pelagia_interface *pelagia = (struct pelagia_interface *)lake->pelagia;
        struct riven_work *device_work = (struct riven_work *)
            riven_alloc(lake->riven, riven_tag_deferred, sizeof(struct riven_work) * lake->device_count, _Alignof(struct riven_work));
        for (u32 i = 0; i < lake->device_count; i++) {
            device_work[i].name = str_init("lake_fini:destroy_device");
            device_work[i].argument = lake->devices[i];
            device_work[i].procedure = (PFN_riven_work)pelagia->destroy_device;
        }
        riven_split_work_and_unchain(lake->riven, device_work, lake->device_count);
    }

    riven_argument_t interfaces[] = {
        (riven_argument_t)lake->hadal,
        (riven_argument_t)lake->pelagia,
        (riven_argument_t)lake->octavia,
    };

    PFN_riven_work finales[] = {
        riven_interface_fini(lake->hadal),
        riven_interface_fini(lake->pelagia),
        riven_interface_fini(lake->octavia),
    };

    struct riven_work fini_work[arraysize(interfaces)];
    for (u32 i = 0; i < arraysize(interfaces); i++) {
        fini_work[i].name = str_init("lake_fini");
        fini_work[i].argument = interfaces[i];
        fini_work[i].procedure = finales[i];
    }
    riven_split_work_and_unchain(lake->riven, fini_work, arraysize(interfaces));
}

static s32 lake_init(
    struct lake         *lake,
    struct engine_hints *hints)
{
    s32 res = result_success;
    const riven_tag_t tag = riven_tag_roots;
    const b32 debug = hints->debug_utilities;
    {
        /* the display backend */
        struct hadal_create_info hadal_info = {
            .header = {
                .interface = (riven_argument_t *)&lake->hadal,
                .metadata = lake->metadata,
                .riven = lake->riven,
                .tag = tag,
            },
            .width = hints->window_width,
            .height = hints->window_height,
            .title = hints->window_title,
        };
        
        /* the rendering backend */
        struct pelagia_create_info pelagia_info = {
            .header = {
                .interface = (riven_argument_t *)&lake->pelagia,
                .metadata = lake->metadata,
                .riven = lake->riven,
                .tag = tag,
            },
            .debug_utilities = debug,
        };

        /* the audio backend */
        struct octavia_create_info octavia_info = {
            .header = {
                .interface = (riven_argument_t *)&lake->octavia,
                .metadata = lake->metadata,
                .riven = lake->riven,
                .tag = tag,
            },
        };

        riven_argument_t create_infos[] = {
            (riven_argument_t)&hadal_info,
            (riven_argument_t)&pelagia_info,
            (riven_argument_t)&octavia_info,
        };

        PFN_riven_work encores[] = {
            hints->encores.hadal,
            hints->encores.pelagia,
            hints->encores.octavia,
        };

        struct riven_work init[arraysize(create_infos)];
        for (u32 i = 0; i < arraysize(create_infos); i++) {
            init[i].name = str_init("lake_init");
            init[i].argument = create_infos[i];
            init[i].procedure = encores[i];
        }
        riven_split_work_and_unchain(lake->riven, init, arraysize(create_infos));

        for (u32 i = 0; i < arraysize(create_infos); i++) {
            struct riven_create_info_header *header = create_infos[i];

            /* we'll exit on errors */
            if (*header->interface == NULL)
                lake->exit_game = true;
        }
        if (lake->exit_game) return result_error;
    }
    u32 physical_device_count = 0;
    res = pelagia_query_physical_devices(lake->pelagia, lake->hadal, &physical_device_count, NULL);
    if (res != result_success) {
        log_error("Failed to query physical rendering devices.");
        return res;
    }

    struct pelagia_physical_device_info *physical_devices = (struct pelagia_physical_device_info *)
        riven_alloc(lake->riven, riven_tag_deferred, sizeof(struct pelagia_physical_device_info) * physical_device_count, _Alignof(struct pelagia_physical_device_info));
    res = pelagia_query_physical_devices(lake->pelagia, lake->hadal, &physical_device_count, physical_devices);
    assert_debug(res == result_success);

    /* select devices from quered information */
    u32 *indices = NULL;
    {
        lake->device_count = physical_device_count;
        lake->device_count += hints->pelagia_virtual_device_count;
        if (hints->pelagia_device_max_count)
            lake->device_count = min(lake->device_count, hints->pelagia_device_max_count);
        assert_debug(lake->device_count);

        indices = (u32 *)riven_alloc(lake->riven, riven_tag_deferred, sizeof(u32) * lake->device_count, _Alignof(u32));

        u32 best_score = 0;
        u32 best_index;
        /* save the index of the best device, this will be our 'primary' */
        for (u32 i = 0; i < lake->device_count; i++) {
            indices[i] = i % physical_device_count;

            if (physical_devices[indices[i]].presentation) {
                if ((s32)i == hints->pelagia_force_primary_device_index) {
                    best_score = UINT32_MAX;
                    best_index = (u32)hints->pelagia_force_primary_device_index;
                } else if (physical_devices[indices[i]].score > best_score) {
                    best_score = physical_devices[indices[i]].score;
                    best_index = i;
                }
            }
        }
        /* ensure our selected primary device is first in the array */
        xorswap(&indices[0], &indices[best_index]);

        /* log for presentation */
        if (!physical_devices[indices[0]].presentation) {
            log_warn("Selected primary device '%s' of index %u has no presentation. "
                     "Creating a swapchain and drawing to a surface is not supported.", 
                     physical_devices[indices[0]].name.ptr, indices[0]);
        }
    }

    /* create rendering devices */
    lake->devices = (struct pelagia_device **)
        riven_alloc(lake->riven, tag, sizeof(struct pelagia_device *) * lake->device_count, _Alignof(struct pelagia_device *));

    struct pelagia_device_create_info *device_infos = (struct pelagia_device_create_info *) 
        riven_alloc(lake->riven, riven_tag_deferred, sizeof(struct pelagia_device_create_info) * lake->device_count, _Alignof(struct pelagia_device_create_info));

    struct pelagia_interface *pelagia = (struct pelagia_interface *)lake->pelagia;
    struct riven_work *device_work = (struct riven_work *)
        riven_alloc(lake->riven, riven_tag_deferred, sizeof(struct riven_work) * lake->device_count, _Alignof(struct riven_work));

    for (u32 i = 0; i < lake->device_count; i++) {
        lake->devices[i] = NULL;
        device_infos[i].work_header = (struct work_header){ .index = i, };
        device_infos[i].pelagia = lake->pelagia;
        device_infos[i].write_device = lake->devices;
        device_infos[i].physical_device = &physical_devices[indices[i]];
        device_infos[i].allocation = (struct riven_allocation){ .tag = tag, };
        riven_format_string(lake->riven, riven_tag_deferred, &device_work[i].name, "%s:create_device:%u", pelagia->header.name.ptr, i);
        device_work[i].argument = &device_infos[i];
        device_work[i].procedure = (PFN_riven_work)pelagia->create_device;
    }
    riven_split_work_and_unchain(lake->riven, device_work, lake->device_count);

#if DEBUG
    for (u32 i = 0; i < lake->device_count; i++)
        assert_debug(device_infos[i].write_device);
#endif

    return result_success;
}

static s32 lake_in_the_lungs(
    struct riven           *riven, 
    struct riven_metadata  *metadata,
    thread_t               *threads,
    u32                     thread_count,
    struct engine_hints    *hints)
{
    s32 res = result_success;
    u64 frame_index = 0;
    u64 time_last = 0, time_now = rtc_counter();
    f64 dt = 0, dt_frequency = 1000/(f64)rtc_frequency();

    struct lake lake = {0};
    lake.riven = riven;
    lake.metadata = metadata;
    lake.threads = threads;
    lake.thread_count = thread_count;

    res = lake_init(&lake, hints);
    if (res != result_success) {
        lake_fini(&lake);
        return res;
    }
    riven_free(riven, riven_tag_deferred);

    struct framedata frames[MAX_FRAMES_IN_FLIGHT];
    zeroa(frames);

    for (s32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        frames[i].work_header = (struct work_header){
            .result = result_success,
        };
        frames[i].lake = &lake;
        frames[i].last_frame = &frames[(i - 1 + MAX_FRAMES_IN_FLIGHT) % MAX_FRAMES_IN_FLIGHT];
        frames[i].next_frame = &frames[(i + 1 + MAX_FRAMES_IN_FLIGHT) % MAX_FRAMES_IN_FLIGHT];
    }
    struct riven_work work[3];
    work[LAKE_SIMULATION_WORK_IDX].procedure = (PFN_riven_work)lake_in_the_lungs_simulation;
    work[LAKE_SIMULATION_WORK_IDX].name = str_init("lake_in_the_lungs:simulation");
    work[LAKE_RENDERING_WORK_IDX].procedure = (PFN_riven_work)lake_in_the_lungs_rendering;
    work[LAKE_RENDERING_WORK_IDX].name = str_init("lake_in_the_lungs:rendering");
    work[LAKE_GPUEXEC_WORK_IDX].procedure = (PFN_riven_work)lake_in_the_lungs_gpuexec;
    work[LAKE_GPUEXEC_WORK_IDX].name = str_init("lake_in_the_lungs:gpuexec");

    struct framedata *simulation = frames;
    struct framedata *rendering = NULL;
    struct framedata *gpuexec = NULL;

    /* XXX delete later */
    f64 close_counter = DEBUG_CLOSE_COUNTER_MS;

    while (simulation || rendering || gpuexec) {
        time_last = time_now;
        time_now = rtc_counter();
        dt = (f64)((time_now - time_last) * dt_frequency);
        record_frame_time(time_now, dt_frequency);

        /* GPU EXECUTION for frame N-2 */
        if (gpuexec) {
            /* gpuexec points to the frame that may currently be in the rendering stage */
            riven_unchain(riven, gpuexec->chain);
            gpuexec->chain = NULL;

            /* check results of last stage (rendering) */
            if (gpuexec->work_header.result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_RENDERING_WORK_IDX].name.ptr);
                lake.exit_game = true;
            }

            /* feed forward to GPU execution */
            work[LAKE_GPUEXEC_WORK_IDX].argument = gpuexec;
            riven_split_work(riven, &work[LAKE_GPUEXEC_WORK_IDX], 1, &gpuexec->chain);
        }

        /* RENDERING for frame N-1 */
        if (rendering) {
            /* rendering points to the frame that may currently be in the simulation stage */
            riven_unchain(riven, rendering->chain);
            rendering->chain = NULL;

            /* check results of last stage (simulation) */
            if (rendering->work_header.result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_SIMULATION_WORK_IDX].name.ptr);
                lake.exit_game = true;
            }

            /* feed forward to rendering */
            work[LAKE_RENDERING_WORK_IDX].argument = rendering;
            riven_split_work(riven, &work[LAKE_RENDERING_WORK_IDX], 1, &rendering->chain);
        }

        /* SIMULATION for current frame */
        if (simulation) {
            /* simulation points to the frame that may not yet completed the pipeline */
            riven_unchain(riven, simulation->chain);
            simulation->chain = NULL;

            /* check results of last stage (gpuexec) */
            if (simulation->work_header.result == result_error) {
                log_error("Frame '%lu' for '%s' returned an error, the game will exit now.", 
                          frame_index, work[LAKE_GPUEXEC_WORK_IDX].name.ptr);
                lake.exit_game = true;
            }

            /* prepare the next framedata */
            simulation->frame_index = frame_index;
            simulation->dt = dt;

            /* begin the next frame */
            work[LAKE_SIMULATION_WORK_IDX].argument = simulation;
            riven_split_work(riven, &work[LAKE_SIMULATION_WORK_IDX], 1, &simulation->chain);
        }

        close_counter -= dt;
        if ((lake.exit_game || close_counter <= 0))
            lake.finalize_gameloop = true;

        /* rotate the framedata */
        gpuexec = rendering;
        rendering = simulation;
        simulation = lake.finalize_gameloop ? NULL : &frames[(frame_index++) % MAX_FRAMES_IN_FLIGHT];

        print_frame_time(FRAME_TIME_PRINT_INTERVAL_MS);
        riven_rotate_deferred(riven);
    }

    /* wait for any existing work to finish */
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        riven_unchain(riven, frames[i].chain);

    dt = median_frame_time();
    log_info("Last recorded frame time: %.3f ms (%.0f FPS)", dt, 1000/dt);

    if (lake.restart_engine && res == result_success)
        res = result_continue;
    lake_fini(&lake);
    return res;
}

s32 amw_main(s32 argc, char **argv)
{
    s32 res = result_success;
    struct engine_hints hints = {
        .frames_in_flight = MAX_FRAMES_IN_FLIGHT,
        .window_width = 1200,
        .window_height = 900,
        .window_title = str_init("UwU miau mlem"),
        .pelagia_force_primary_device_index = -1,
        .pelagia_device_max_count = 0,
        .riven_memory_budget_size = 0,
        .riven_fiber_stack_size = 96*1024,
        .riven_fiber_count = 0,
        .riven_thread_count = 0,
        .riven_tagged_heap_count = 0,
        .riven_log2_work_count = 11,
        .riven_log2_memory_count = 9,

        .encores = {
            .octavia = (PFN_riven_work)octavia_encore_native,
            .pelagia = (PFN_riven_work)pelagia_encore_native,
            .hadal = (PFN_riven_work)hadal_encore_native,
        },
#ifdef DEBUG
        .debug_utilities = true,
        .pelagia_virtual_device_count = 0, // XXX
#endif
    };
    log_set_verbose(argc-1);

    /* TODO parse the command line */
    (void)argc;
    (void)argv;

    assert_debug(hints.frames_in_flight <= MAX_FRAMES_IN_FLIGHT && hints.frames_in_flight > 0);

    struct riven_metadata metadata = {
        .engine_name = str_init("A Moonlit Walk Engine"),
        .engine_build_version = LAKE_VERSION, /* XXX */
        .game_name = str_init("Lake in the Lungs"),
        .game_build_version = LAKE_VERSION,
    };

    /* go for a walk */
    do {res = riven_moonlit_walk(
            hints.riven_memory_budget_size,
            hints.riven_fiber_stack_size,
            hints.riven_fiber_count,
            hints.riven_thread_count,
            hints.riven_tagged_heap_count,
            hints.riven_log2_work_count,
            hints.riven_log2_memory_count,
            &metadata,
            (PFN_riven_heart)lake_in_the_lungs,
            (riven_argument_t)&hints);
    } while (res == result_continue);
    return res;
}
