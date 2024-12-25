#ifndef _HADOPELAGIC_H
#define _HADOPELAGIC_H

#include <lake/hadal.h>
#include <lake/riven.h>

#include "linux/wl_hadal.h"

struct monitor {
    char        name[128];
    uint32_t    width_mm, height_mm;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct window *window;

    _HADAL_WAYLAND_MONITOR_STATE
};

struct window {
    char       *title;
    uint32_t    minwidth, minheight;
    uint32_t    maxwidth, maxheight;
    uint32_t    numer, denom;

    /* A window is binded to a monitor if in fullscreen mode. */
    struct monitor *monitor;

    _HADAL_WAYLAND_WINDOW_STATE
};

struct hadopelagic_api {
    uint32_t api, last_api_fallback;

    int32_t (*init)(struct hadal *hadal);
    void    (*fini)(struct hadal *hadal);

    int32_t (*window_create)(struct hadal *hadal, uint32_t width, uint32_t height);
    void    (*window_destroy)(struct hadal *hadal);

    void    (*get_window_size)(const struct hadal *hadal, uint32_t *out_width, uint32_t *out_height);
    void    (*get_framebuffer_size)(const struct hadal *hadal, uint32_t *out_width, uint32_t *out_height);

    /* Control hadal flags: */
    void    (*visible_show)(struct hadal *hadal); /**< flag visible : true */
    void    (*visible_hide)(struct hadal *hadal); /**< flag visible : false */
};

struct hadal {
    at_uint32_t             flags;
    struct hadopelagic_api  api;
    struct window           window;
    struct monitor        **monitors;
    uint32_t                monitor_count;

    _HADAL_WAYLAND_PLATFORM_STATE
};

#ifndef AMW_NDEBUG
extern bool _hadal_debug_validate_display_api(const struct hadopelagic_api *api);
#endif

#endif /* _HADOPELAGIC_H */
