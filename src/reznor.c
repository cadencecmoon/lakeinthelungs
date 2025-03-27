#include <amw/log.h>
#include <amw/reznor.h>

RIVEN_ENCORE(reznor, native)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    static const PFN_riven_work encores[] = {
#ifdef REZNOR_D3D12
        (PFN_riven_work)reznor_encore_d3d12,
#endif
#ifdef REZNOR_METAL
        (PFN_riven_work)reznor_encore_metal,
#endif
#ifdef REZNOR_VULKAN
        (PFN_riven_work)reznor_encore_vulkan,
#endif
#ifdef REZNOR_WEBGPU
        (PFN_riven_work)reznor_encore_webgpu,
#endif
#ifndef REZNOR_DISABLE_NULL_FALLBACK
        (PFN_riven_work)reznor_encore_null,
#endif
    };
    u32 encore_count = arraysize(encores);

    for (u32 i = 0; i < encore_count; i++) {
        encores[i](encore);

        if (*encore->header.interface == NULL) 
            continue;

        const struct reznor_interface *interface = *encore->header.interface;
        const char *fmt = "'%s' is missing interface procedure - 'PFN_reznor_%s'.";
        b32 valid = true;

        /* just assert the header */
        assert_debug(interface->header.fini);

        /* check the interface procedures */
#define VALIDATE(fn) \
        if (interface->fn == NULL) { log_debug(fmt, interface->header.name.ptr, #fn); valid = false; }
        VALIDATE(device_query)
        VALIDATE(device_assembly)
        VALIDATE(device_destroy)
        VALIDATE(device_memory_assembly)
        VALIDATE(buffer_assembly)
        VALIDATE(texture_assembly)
        VALIDATE(sampler_assembly)
        VALIDATE(descriptor_set_layout_assembly)
        VALIDATE(descriptor_set_assembly)
        VALIDATE(pipeline_layout_assembly)
        VALIDATE(graphics_pipeline_assembly)
        VALIDATE(compute_pipeline_assembly)
        VALIDATE(raytracing_pipeline_assembly)
        VALIDATE(shader_binding_table_assembly)
        VALIDATE(bottom_level_assembly)
        VALIDATE(top_level_assembly)
        VALIDATE(query_pool_assembly)
        VALIDATE(swapchain_assembly)
        VALIDATE(swapchain_try_recreate)
        VALIDATE(disassembly)
        VALIDATE(frame_begin)
        VALIDATE(frame_next_images)
        VALIDATE(frame_submit)
        VALIDATE(command_draw)
        VALIDATE(command_draw_indexed)
        VALIDATE(command_draw_indexed_indirect)
        VALIDATE(command_draw_indirect)
        VALIDATE(command_dispatch)
        VALIDATE(command_dispatch_indirect)
        VALIDATE(command_copy_buffer)
        VALIDATE(command_copy_texture)
        VALIDATE(command_begin_render_pass)
        VALIDATE(command_end_render_pass)
#undef VALIDATE
        if (valid) return;

        /* continue with the next encore */
        if (interface->header.fini)
            interface->header.fini((void *)interface);
        *encore->header.interface = NULL;
    }
}

RIVEN_ENCORE(reznor, null)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    struct reznor_interface *interface = (struct reznor_interface *) 
        riven_alloc(encore->header.riven, encore->header.tag, sizeof(struct reznor_interface), _Alignof(struct reznor_interface));

    /* write the interface */
    interface->header.name = str_init("reznor_null");
    interface->header.riven = encore->header.riven;
    interface->header.tag = encore->header.tag;
    interface->header.fini = (PFN_riven_work)riven_work_nop;
    interface->thread_count = encore->thread_count;
    interface->debug_utils = encore->debug_utils;

    *encore->header.interface = (void *)(struct reznor *)interface;
    log_verbose("'%s' interface write.", interface->header.name.ptr);
}

#ifdef REZNOR_D3D12
RIVEN_ENCORE_STUB(reznor, d3d12)
#endif
#ifdef REZNOR_METAL
RIVEN_ENCORE_STUB(reznor, metal)
#endif
#ifdef REZNOR_WEBGPU
RIVEN_ENCORE_STUB(reznor, webgpu)
#endif

AMWAPI s32 AMWCALL reznor_mgpu_assembly(
    struct reznor          *reznor,
    struct hadal           *hadal,
    riven_tag_t             tag,
    u32                     frames_in_flight,
    u32                     max_device_count,
    u32                     virtual_device_count,
    b32                     only_discrete,
    s32                     force_primary_index,
    u32                    *out_device_count,
    struct reznor_device  **out_devices)
{
    struct reznor_interface *interface = (struct reznor_interface *)reznor;
    struct riven *riven = interface->header.riven;

    u32 physical_device_count = 0;
    u32 device_count = *out_device_count;

    struct reznor_physical_device_info *physical_devices = NULL;

    s32 res = interface->device_query(reznor, hadal, &physical_device_count, NULL);
    if (physical_device_count == 0 || res != result_success)
        return result_error;

    /* get device count */
    device_count = physical_device_count + virtual_device_count;
    if (max_device_count)
        device_count = min(device_count, max_device_count);
    *out_device_count = device_count;
 
    /* get info from physical devices */
    physical_devices = (struct reznor_physical_device_info *)
        riven_alloc(riven, riven_tag_deferred, sizeof(struct reznor_physical_device_info) * device_count, _Alignof(struct reznor_physical_device_info));
    interface->device_query(reznor, hadal, &physical_device_count, physical_devices);

    /* query memory requirements */
    struct reznor_device_assembly_work query_work = {
        /* we assume that any physical device will have the same memory requirements,
         * other parameters (frames in flight, thread count) are more important */
        .reznor = reznor,
        .physical_info = &physical_devices[0],
        .frames_in_flight = frames_in_flight,
        .memory = {
            .data = NULL,
            .tag = riven_tag_invalid,
        },
        .device_assembly = NULL,
    };
    interface->device_assembly(&query_work);
    usize total_bytes = query_work.memory.size * device_count;

    if (out_devices == NULL)
        total_bytes += A8(sizeof(struct reznor_device *) * device_count);

    /* allocate memory for the devices */
    void *devices_memory = riven_alloc(riven, tag, total_bytes, query_work.memory.alignment);
    memset(devices_memory, 0, total_bytes);

    usize indices_bytes = A8(sizeof(u32) * device_count);
    usize devices_bytes = A8(sizeof(struct reznor_device_assembly_work) * device_count);
    usize riven_work_bytes = A8(sizeof(struct riven_work) * device_count);

    usize o = 0;
    u8 *raw = (u8 *)riven_alloc(riven, riven_tag_deferred, indices_bytes + devices_bytes + riven_work_bytes, 8);

    u32 *indices = (u32 *)&raw[o];
    o += indices_bytes;
    struct reznor_device_assembly_work *devices_work = (struct reznor_device_assembly_work *)&raw[o];
    o += devices_bytes;
    struct riven_work *riven_work = (struct riven_work *)&raw[o];

    o = 0;
    /* collect physical device indices */
    if (physical_device_count > 1) {
        u32 best_score = 0;
        u32 best_index = 0;

        for (u32 i = 0; i < device_count; i++) {
            indices[o] = i % physical_device_count;
            struct reznor_physical_device_info *info = &physical_devices[indices[o]];
                
            if (only_discrete && !(info->features & reznor_device_feature_is_discrete))
                continue;

            if ((s32)o == force_primary_index) {
                /* only positive or zero indices will work here */
                best_score = UINT32_MAX;
                best_index = (u32)force_primary_index;

            } else if (info->score > best_score) {
                best_score = info->score;
                best_index = o;
            }
            o++; /* accept this device */
        }
        /* enforce primary device at device index 0 */
        xorswap(&indices[0], &indices[best_index]);
    } /* else (physical_device_count == 1) indices still point to the only available physical device at index 0 */

    o = 0;
    /* raw can be repurposed into our devices memory buffer */
    raw = (u8 *)devices_memory;
    if (out_devices == NULL) {
        out_devices = (struct reznor_device **)&raw[0];
        o = A8(sizeof(struct reznor_device *) * device_count);
    }

    /* prepare the work for every device to be created */
    for (u32 i = 0; i < device_count; i++) {
        devices_work[i].reznor = reznor;
        devices_work[i].result = result_error;
        devices_work[i].physical_info = &physical_devices[indices[i]];
        devices_work[i].frames_in_flight = frames_in_flight;
        devices_work[i].device_assembly = &out_devices[i];
        devices_work[i].memory.size = query_work.memory.size;
        devices_work[i].memory.alignment = query_work.memory.alignment;
        devices_work[i].memory.tag = riven_tag_invalid;
        devices_work[i].memory.data = &raw[o];
        o += query_work.memory.size;

        riven_work[i].procedure = (PFN_riven_work)interface->device_assembly;
        riven_work[i].argument = &devices_work[i];
        riven_work[i].name = str_init("reznor:mgpu:device_assembly");
    }
    riven_split_work_and_unchain(riven, riven_work, device_count);

    res = result_success;
    for (u32 i = 0; i < device_count; i++) {
        if (devices_work[i].result == result_error) {
            log_error("Creating a rendering device of index %u failed: %s.", i, devices_work[i].physical_info->name.ptr);
            res = devices_work[i].result;
        }
    }
    return res;
}
