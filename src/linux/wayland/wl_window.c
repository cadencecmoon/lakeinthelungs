#ifdef HADAL_WAYLAND

#include "wl_hadal.h"
#include <amw/bedrock/log.h>
#include <amw/reznor.h>

static void resize_framebuffer(struct hadal_window *window)
{
    /* TODO framebuffer scaling */
    atomic_store_explicit(&window->fb_width, window->window_width, memory_order_release);
    atomic_store_explicit(&window->fb_height, window->window_height, memory_order_release);

    if (window->header.swapchain)
        atomic_fetch_or_explicit(&window->header.swapchain_header->flags, reznor_swapchain_flag_framebuffer_resized, memory_order_release);
}

static b32 resize_window(struct hadal_window *window, s32 width, s32 height)
{
    width = max(width, 1);
    height = max(height, 1);

    if ((u32)width == window->window_width && (u32)height == window->window_height)
        return false;

    window->window_width = width;
    window->window_height = height;

    resize_framebuffer(window);

    return true;
}

static void handle_libdecor_frame_configure(
    struct libdecor_frame         *frame,
    struct libdecor_configuration *config,
    void                          *data)
{
    struct hadal_window *window = (struct hadal_window *)data;
    u32 flags = atomic_load_explicit(&window->header.flags, memory_order_acquire);
    s32 width, height;

    enum libdecor_window_state window_state;
    b32 fullscreen, activated, maximized;

    if (libdecor_configuration_get_window_state(config, &window_state)) {
        fullscreen = (window_state & LIBDECOR_WINDOW_STATE_FULLSCREEN) != 0;
        activated = (window_state & LIBDECOR_WINDOW_STATE_ACTIVE) != 0;
        maximized = (window_state & LIBDECOR_WINDOW_STATE_MAXIMIZED) != 0;
    } else {
        fullscreen = (flags & hadal_window_flag_fullscreen) != 0;
        activated = (flags & hadal_window_flag_shell_activated) != 0;
        maximized = (flags & hadal_window_flag_maximized) != 0;
    }

    if (!libdecor_configuration_get_content_size(config, frame, &width, &height)) {
        width = window->window_width;
        height = window->window_height;
    }

    if (!maximized && !fullscreen) {
        /* TODO set the aspect ratio */
    }

    struct libdecor_state *frame_state = libdecor_state_new(width, height);
    libdecor_frame_commit(frame, frame_state, config);
    libdecor_state_free(frame_state);

    if (((flags & hadal_window_flag_shell_activated) != 0) != activated) {
        u32 old = atomic_fetch_xor_explicit(&window->header.flags, hadal_window_flag_shell_activated, memory_order_relaxed);

        /* if the shell was deactivated */
        if ((old & hadal_window_flag_shell_activated) && (window->output_count && (old & hadal_window_flag_auto_minimize)))
            libdecor_frame_set_minimized(frame);
    }

    if (((flags & hadal_window_flag_maximized) != 0) != maximized)
        atomic_fetch_xor_explicit(&window->header.flags, hadal_window_flag_maximized, memory_order_relaxed);

    if (((flags & hadal_window_flag_fullscreen) != 0) != fullscreen)
        atomic_fetch_xor_explicit(&window->header.flags, hadal_window_flag_fullscreen, memory_order_relaxed);

    b32 damaged = false;
    if ((flags & hadal_window_flag_visible) != 0)
        damaged = true;
    if (resize_window(window, width, height))
        damaged = true;

    if (damaged && window->header.swapchain) {
        atomic_fetch_or_explicit(&window->header.swapchain_header->flags, reznor_swapchain_flag_framebuffer_resized, memory_order_relaxed);
    } else if (!damaged) {
        wl_surface_commit(window->surface);
    }
}

static void handle_libdecor_frame_close(
    struct libdecor_frame *frame,
    void                  *data)
{
    /* unused */
    (void)frame;

    struct hadal_window *window = (struct hadal_window *)data;
    atomic_fetch_or_explicit(&window->header.flags, hadal_window_flag_should_close, memory_order_release);
}

static void handle_libdecor_frame_commit(
    struct libdecor_frame *frame,
    void                  *data)
{
    /* unused */
    (void)frame;

    struct hadal_window *window = (struct hadal_window *)data;
    wl_surface_commit(window->surface);
}

void handle_libdecor_frame_dismiss_popup(
    struct libdecor_frame *frame,
    const char            *seat_name,
    void                  *data)
{
    (void)frame;
    (void)seat_name;
    (void)data;
}

static struct libdecor_frame_interface libdecor_frame_interface = {
    .close = handle_libdecor_frame_close,
    .commit = handle_libdecor_frame_commit,
    .configure = handle_libdecor_frame_configure,
    .dismiss_popup = handle_libdecor_frame_dismiss_popup,
};

static b32 create_shell_libdecor_frame(struct hadal_window *window)
{
    struct hadal *hadal = window->header.hadal;

    /* allow libdecor to finish initialization of itself and its plugin */
    /* while (!hadal->libdecor.ready) { */
    /*     _hadal_wayland_event_poll(hadal, NULL); */
    /* } */

    window->shell_surface.libdecor.frame = 
        libdecor_decorate(hadal->shell.libdecor, window->surface, &libdecor_frame_interface, window);

    if (!window->shell_surface.libdecor.frame) {
        log_error("Wayland: failed to create libdecor frame for window titled '%s'.", window->header.title.ptr);
        return false;
    }

    struct libdecor_state *frame_state = libdecor_state_new(window->window_width, window->window_height);
    libdecor_frame_commit(window->shell_surface.libdecor.frame, frame_state, NULL);
    libdecor_state_free(frame_state);

    libdecor_frame_set_title(window->shell_surface.libdecor.frame, window->header.title.ptr);

    u32 flags = atomic_load_explicit(&window->header.flags, memory_order_acquire);

    if (!(flags & hadal_window_flag_resizable))
        libdecor_frame_unset_capabilities(window->shell_surface.libdecor.frame, LIBDECOR_ACTION_RESIZE);

    libdecor_frame_map(window->shell_surface.libdecor.frame);
    wl_display_roundtrip(hadal->display);

    return true;
}

static void handle_xdg_surface_configure(
    void               *data,
    struct xdg_surface *xdg_surface,
    u32                 serial)
{
    struct hadal_window *window = (struct hadal_window *)data;
      
    xdg_surface_ack_configure(xdg_surface, serial);

    /* TODO */
    (void)window;
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void handle_xdg_toplevel_close(
    void                *data,
    struct xdg_toplevel *xdg_toplevel)
{
    /* unused */
    (void)xdg_toplevel;

    struct hadal_window *window = (struct hadal_window *)data;
    atomic_fetch_or_explicit(&window->header.flags, hadal_window_flag_should_close, memory_order_release);
}

static void handle_xdg_toplevel_configure(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height,
    struct wl_array     *states)
{
    struct hadal_window *window = (struct hadal_window *)data;
    u32 *state;

    u32 flags = 0u;

    wl_array_for_each(state, states)
    {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            flags |= hadal_window_flag_maximized;
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            flags |= hadal_window_flag_fullscreen;
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            flags |= hadal_window_flag_shell_activated;
            break;
        }
    }

    /* TODO */
    (void)xdg_toplevel;
    (void)width;
    (void)height;
    (void)flags;
    (void)window;
}

static void handle_xdg_toplevel_configure_bounds(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    s32                  width,
    s32                  height)
{
    (void)data;
    (void)xdg_toplevel;
    (void)width;
    (void)height;
}

static void handle_xdg_toplevel_wm_capabilities(
    void                *data,
    struct xdg_toplevel *xdg_toplevel,
    struct wl_array     *array)
{
    (void)data;
    (void)xdg_toplevel;
    (void)array;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .close = handle_xdg_toplevel_close,
    .configure = handle_xdg_toplevel_configure,
    .configure_bounds = handle_xdg_toplevel_configure_bounds,
    .wm_capabilities = handle_xdg_toplevel_wm_capabilities,
};

static b32 create_shell_xdg_toplevel(struct hadal_window *window)
{
    struct hadal *hadal = window->header.hadal;
    assert_debug(hadal->shell.xdg);

    window->shell_surface.xdg.surface = xdg_wm_base_get_xdg_surface(hadal->shell.xdg, window->surface);
    if (!window->shell_surface.xdg.surface) {
        log_error("Wayland: failed to create xdg-surface for window of title '%s'.", window->header.title.ptr);
        return false;
    }
    xdg_surface_add_listener(window->shell_surface.xdg.surface, &xdg_surface_listener, window);

    window->shell_surface.xdg.roleobj.toplevel = xdg_surface_get_toplevel(window->shell_surface.xdg.surface);
    if (!window->shell_surface.xdg.roleobj.toplevel) {
        log_error("Wayland: failed to create xdg-toplevel for window of title '%s'.", window->header.title.ptr);
        return false;
    }
    xdg_toplevel_add_listener(window->shell_surface.xdg.roleobj.toplevel, &xdg_toplevel_listener, window);

    xdg_toplevel_set_title(window->shell_surface.xdg.roleobj.toplevel, window->header.title.ptr);

    wl_surface_commit(window->surface);
    wl_display_roundtrip(hadal->display);
    return true;
}

static b32 create_shell_xdg_popup(struct hadal_window *window)
{
    /* TODO */
    (void)window;
    return false;
}

static b32 create_shell_objects(struct hadal_window *window)
{
    if (window->shell_surface.libdecor.frame || 
        window->shell_surface.xdg.roleobj.toplevel || 
        window->shell_surface.xdg.roleobj.popup.popup)
        return true;

    switch (window->shell_surface_type) {
    case WAYLAND_SURFACE_LIBDECOR: 
        return create_shell_libdecor_frame(window);
    case WAYLAND_SURFACE_XDG_TOPLEVEL: 
        return create_shell_xdg_toplevel(window);
    case WAYLAND_SURFACE_XDG_POPUP: 
        return create_shell_xdg_popup(window);
    case WAYLAND_SURFACE_UNKNOWN: 
        return false;
    }
    UNREACHABLE;
}

static void destroy_shell_objects(struct hadal_window *window)
{
    if (window->shell_surface_type == WAYLAND_SURFACE_LIBDECOR && window->shell_surface.libdecor.frame) {
        libdecor_frame_unref(window->shell_surface.libdecor.frame);
        window->shell_surface.libdecor.frame = NULL;

    } else if (window->shell_surface_type != WAYLAND_SURFACE_UNKNOWN) {
        if (window->shell_surface_type == WAYLAND_SURFACE_XDG_TOPLEVEL && window->shell_surface.xdg.roleobj.toplevel) {
            xdg_toplevel_destroy(window->shell_surface.xdg.roleobj.toplevel);
            window->shell_surface.xdg.roleobj.toplevel = NULL;
        } else if (window->shell_surface.xdg.roleobj.popup.popup) {
            xdg_popup_destroy(window->shell_surface.xdg.roleobj.popup.popup);
            window->shell_surface.xdg.roleobj.popup.popup = NULL;
            window->shell_surface.xdg.roleobj.popup.positioner = NULL;
            window->shell_surface.xdg.roleobj.popup.child = NULL;
            window->shell_surface.xdg.roleobj.popup.parent_id = 0u;
        }

        if (window->shell_surface.xdg.surface)
            xdg_surface_destroy(window->shell_surface.xdg.surface);
        window->shell_surface.xdg.surface = NULL;
    }
}

static void handle_surface_enter(
    void              *data, 
    struct wl_surface *surface, 
    struct wl_output  *output)
{
    /* unused */
    (void)surface;
    (void)data; // window

    if (!wayland_own_output(output))
        return;
}

static void handle_surface_leave(
    void              *data,
    struct wl_surface *surface,
    struct wl_output  *output)
{
    /* unused */
    (void)surface;
    (void)data; // window

    if (!wayland_own_output(output))
        return;
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
};

FN_HADAL_WINDOW_CREATE(wayland)
{
    struct hadal *hadal = work->hadal;

    u32 flags = hadal_window_flag_is_valid;
    usize total_bytes = sizeof(struct hadal_window);
    usize alignment = _Alignof(struct hadal_window);

    work->memory.size = total_bytes;
    work->memory.alignment = alignment;

    if (!work->memory.data) {
        if (work->memory.tag) {
            work->memory.data = riven_alloc(hadal->interface.header.riven, work->memory.tag, total_bytes, alignment);
        } else {
            /* the caller must allocate memory */
            assert_debug(work->result != result_allocation_query);
            work->result = result_allocation_query;
            return;
        }
    }
    assert_debug(work->out_window);
    work->result = result_error;

    struct hadal_window *window = work->memory.data;
    zerop(window);

    if (work->flags & hadal_window_flag_modal) {
        window->shell_surface_type = WAYLAND_SURFACE_XDG_POPUP;
    } else {
        window->shell_surface_type = hadal->shell.libdecor ? WAYLAND_SURFACE_LIBDECOR : WAYLAND_SURFACE_XDG_TOPLEVEL;
    }
    window->header.hadal = hadal;
    window->header.title = work->title;

    window->surface = wl_compositor_create_surface(hadal->compositor);
    if (!window->surface) {
        log_error("Wayland: failed to create a window surface, for work of title '%s'.", work->title.ptr);
        return;
    }
    wayland_register_surface(window->surface);
    wl_surface_add_listener(window->surface, &surface_listener, window);

    window->floating_width = window->window_width = work->width;
    window->floating_height = window->window_height = work->height;
    atomic_store_explicit(&window->fb_width, work->width, memory_order_relaxed);
    atomic_store_explicit(&window->fb_height, work->height, memory_order_relaxed);

    if (work->flags & hadal_window_flag_fullscreen) 
        flags |= hadal_window_flag_fullscreen;
    else if (work->flags & hadal_window_flag_maximized)
        flags |= hadal_window_flag_maximized;

    if (work->flags & hadal_window_flag_shell_activated)
        flags |= hadal_window_flag_shell_activated;
#ifdef REZNOR_VULKAN
    if (hadal->vulkan.instance)
        flags |= hadal_window_flag_vulkan;
#endif /* REZNOR_VULKAN */
    atomic_store_explicit(&window->header.flags, flags, memory_order_release);

    if (work->flags & hadal_window_flag_visible) {
        atomic_fetch_or_explicit(&window->header.flags, hadal_window_flag_visible, memory_order_relaxed);

        if (!create_shell_objects(window)) {
            _hadal_wayland_window_destroy(window);
            return;
        }
    }
    *work->out_window = window;
    work->result = result_success;
}

FN_HADAL_WINDOW_DESTROY(wayland)
{
    destroy_shell_objects(window);

    if (window->surface)
        wl_surface_destroy(window->surface);

    zerop(window);
}

FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(wayland)
{
    u32 fb_width = atomic_load_explicit(&window->fb_width, memory_order_consume);
    u32 fb_height = atomic_load_explicit(&window->fb_height, memory_order_consume);

    if (out_width) 
        *out_width = fb_width;
    if (out_height) 
        *out_height = fb_height;
}

FN_HADAL_WINDOW_VISIBILITY(wayland)
{
    u32 flags = atomic_load_explicit(&window->header.flags, memory_order_relaxed);

    if ((flags & hadal_window_flag_visible) != (visible ? hadal_window_flag_visible : 0)) {
        if (visible && !create_shell_objects(window)) {
            /* an error occured when creating the shell objects */
            return flags;
        } else {
            destroy_shell_objects(window);
            wl_surface_attach(window->surface, NULL, 0, 0);
            wl_surface_commit(window->surface);
        }
        return atomic_fetch_xor_explicit(&window->header.flags, hadal_window_flag_visible, memory_order_acq_rel);
    }
    /* no modify/write operation was done */
    return flags;
}

#ifdef REZNOR_VULKAN
FN_HADAL_VULKAN_SURFACE_CREATE(wayland)
{
    struct hadal *hadal = window->header.hadal;
    struct VkWaylandSurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = hadal->display,
        .surface = window->surface,
    };
    assert_debug(atomic_load_explicit(&window->header.flags, memory_order_relaxed) & hadal_window_flag_vulkan);

    /* VK_SUCCESS is a non-zero value so its fine enough */
    return hadal->vulkan.vkCreateWaylandSurfaceKHR(hadal->vulkan.instance, &surface_info, callbacks, out_surface);
}
#endif /* REZNOR_VULKAN */

#endif /* HADAL_WAYLAND */
