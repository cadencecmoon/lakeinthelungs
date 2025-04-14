#pragma once

#include <amwe/hadal/encore.h>
#include <amwe/hadal/keycodes.h>
#include <amwe/hadal/scancodes.h>
#include <amwe/hadal/window.h>

#ifndef HADAL_MAX_ENCORE_COUNT
    #define HADAL_MAX_ENCORE_COUNT 1
#endif /* HADAL_MAX_ENCORE_COUNT */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the display engine, implemented by a backend 'hadal_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct hadal_interface {
    struct riven_interface_header           header;

    s16                                     keycodes[256];
    s16                                     scancodes[hadal_keycode_last + 1];
    s8                                      keynames[hadal_keycode_last + 1][5];
    s8                                      keys[hadal_keycode_last + 1];

    struct hadal_interface_window           window;
#ifdef XAKU_VULKAN
    struct hadal_interface_vulkan           vulkan;
#endif /* XAKU_VULKAN */
};

/** A view into the backend. */
union hadal_encore_view {
    struct riven_interface_header          *header;
    struct hadal_interface                 *interface;
    struct hadal_encore                    *encore;
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
    enum hadal_strategy                     strategy;
    atomic_u32                              flags;
    union hadal_encore_view                 backends[HADAL_MAX_ENCORE_COUNT];

    union hadal_window_view                *windows;
    u32                                     window_count;
    u32                                     window_capacity;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
