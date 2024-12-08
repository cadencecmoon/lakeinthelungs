/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_INTERNAL_HADOPELAGIC_H
#define _AMW_INTERNAL_HADOPELAGIC_H

#include <amw/hadal.h>
#include <amw/riven.h>

#include "linux/wayland.h"

struct monitor {
    char name[128];
    u32  width_mm, height_mm;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct window *window;

    HADOPELAGIC_WAYLAND_MONITOR_STATE
};

struct window {
    char   *title;
    u32     minwidth, minheight;
    u32     maxwidth, maxheight;
    u32     numer, denom;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct monitor *monitor;

    HADOPELAGIC_WAYLAND_WINDOW_STATE
};

struct hadopelagic_api {
    u32 api, last_api_fallback;

    i32     (*init)(void);
    void    (*fini)(void);

    i32     (*window_create)(struct window *window, u32 width, u32 height);
    void    (*window_destroy)(struct window *window);

    void    (*get_window_size)(const struct window *window, u32 *out_width, u32 *out_height);
    void    (*get_framebuffer_size)(const struct window *window, u32 *out_width, u32 *out_height);

    /* Control hadal flags: */
    void    (*visible_show)(struct window *window); /**< flag visible : true */
    void    (*visible_hide)(struct window *window); /**< flag visible : false */

    HADOPELAGIC_WAYLAND_API_WL_STATE
    HADOPELAGIC_WAYLAND_API_XKB_STATE
};

struct hadal {
    at_u32_t                flags;
    struct hadopelagic_api  api;
    struct window          *window;
    struct monitor        **monitors;
    u32                     monitor_count;

    HADOPELAGIC_WAYLAND_PLATFORM_STATE
};

/** The global state of the platform abstraction layer. */
extern struct hadal HADAL;

/** Implemented in AMW_DEBUG only, checks internal API. */
extern bool _hadal_debug_validate_display_api(const struct hadopelagic_api *api);

#endif /* _AMW_INTERNAL_HADOPELAGIC_H */
