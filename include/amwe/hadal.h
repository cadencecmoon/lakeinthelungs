#pragma once

#include <amwe/display/hadal_encore.h>
#include <amwe/display/hadal_monitor.h>
#include <amwe/display/hadal_window.h>
#include <amwe/display/keycodes.h>
#include <amwe/display/scancodes.h>

#ifndef HADAL_INTERFACE_MAX_MONITOR_COUNT
#define HADAL_INTERFACE_MAX_MONITOR_COUNT 8
#endif
#ifndef HADAL_INTERFACE_MAX_WINDOW_COUNT
#define HADAL_INTERFACE_MAX_WINDOW_COUNT 8
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the display engine, implemented by a backend 'hadal_encore'. */
struct hadal_interface {
    struct riven_interface                      riven;

    union hadal_monitor_view                    monitors[HADAL_INTERFACE_MAX_MONITOR_COUNT];
    u32                                         monitor_count;

    union hadal_window_view                     windows[HADAL_INTERFACE_MAX_WINDOW_COUNT];
    atomic_u32                                  active_window_count;

    s16                                         keycodes[256];
    s16                                         scancodes[hadal_keycode_last + 1];
    s8                                          keynames[hadal_keycode_last + 1][5];
    s8                                          keys[hadal_keycode_last + 1];

    /* procedures, implemented by the backend */
    PFN_hadal_window_create                     window_create;
    PFN_hadal_window_destroy                    window_destroy;
    PFN_hadal_window_visibility                 window_visibility;
#ifdef XAKU_VULKAN
    PFN_hadal_vulkan_write_instance             vulkan_write_instance;
    PFN_hadal_vulkan_presentation_support       vulkan_presentation_support;
    PFN_hadal_vulkan_surface_create             vulkan_surface_create;
#endif /* XAKU_VULKAN */
};
union hadal_display {
    struct riven_interface     *riven;
    struct hadal_interface     *interface;
    struct hadal_encore        *encore;
    void                       *data;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
