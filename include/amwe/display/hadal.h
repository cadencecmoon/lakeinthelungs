#pragma once

#include <amwe/display/encore.h>
#include <amwe/display/keycodes.h>
#include <amwe/display/scancodes.h>
#include <amwe/display/window.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the display engine, implemented by a backend 'hadal_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct hadal_interface {
    struct riven_interface_header               header;

    s16                                         keycodes[256];
    s16                                         scancodes[hadal_keycode_last + 1];
    s8                                          keynames[hadal_keycode_last + 1][5];
    s8                                          keys[hadal_keycode_last + 1];

    PFN_hadal_window_create                     window_create;
    PFN_hadal_window_destroy                    window_destroy;
    PFN_hadal_window_visibility                 window_visibility;
#ifdef XAKU_VULKAN
    PFN_hadal_vulkan_write_instance             vulkan_write_instance;
    PFN_hadal_vulkan_presentation_support       vulkan_presentation_support;
    PFN_hadal_vulkan_surface_create             vulkan_surface_create;
#endif /* XAKU_VULKAN */
};

/** A view into the backend. */
union hadal_encore_view {
    struct riven_interface_header              *header;
    struct hadal_interface                     *interface;
    struct hadal_encore                        *encore;
};

/** A view into an output monitor. */
union hadal_monitor_view {
    struct {HADAL_INTERFACE_MONITOR_HEADER}    *header;
    struct hadal_monitor                       *monitor; 
    union hadal_encore_view                    *encore_view;
};

/** A view into a window. */
union hadal_window_view {
    struct {HADAL_INTERFACE_WINDOW_HEADER}     *header;
    struct hadal_window                        *window; 
    union hadal_encore_view                    *encore_view;
};

/** Implies different strategies for the display engine. */
enum hadal_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    hadal_strategy_auto = 0,
    /** Only the main window may be present, no modals, popups or multiple windows allowed. */
    hadal_strategy_optimal_one_viewport = 0,
    /** One backend, but it is allowed and expected to run more than one system window */
    hadal_strategy_optimal_multi_viewport,
    /** Allows to run multiple backends, may be used for testing. */
    hadal_strategy_debug,
};

/** An engine structure for display communication and windowing. */
struct hadal {
    enum hadal_strategy                         strategy;
    atomic_u32                                  flags;
    union hadal_encore_view                     backend;

    lake_dynamic_array(union hadal_window_view) windows;
};

LAKEAPI void LAKECALL
hadal_fini(struct hadal *hadal);

#ifdef __cplusplus
}
#endif /* __cplusplus */
