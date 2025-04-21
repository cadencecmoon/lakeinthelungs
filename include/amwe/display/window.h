#pragma once

#include <amwe/display/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Flags that describe a window state. */
enum hadal_window_flags {
    hadal_window_flag_is_valid          = (1u << 0),
    /** Window is visible and the user can interact with it. */
    hadal_window_flag_visible           = (1u << 1),
    /** Window is in exclusive fullscreen mode. When true, some other flags may be ignored. */
    hadal_window_flag_fullscreen        = (1u << 2),
    /** Window is filling the screen space, ignored in fullscreen. */
    hadal_window_flag_maximized         = (1u << 3),
    /** Window is minimized to the task bar. */
    hadal_window_flag_minimized         = (1u << 4),
    /** Window can be resized by the user by dragging it. */
    hadal_window_flag_resizable         = (1u << 5),
    /** Window decorations are enabled, otherwise it is borderless. */
    hadal_window_flag_decorated         = (1u << 6),
    /** Window has input focus. */
    hadal_window_flag_input_focus       = (1u << 7),
    /** Window has a mouse cursor hovered over it. */
    hadal_window_flag_cursor_hovered    = (1u << 8),
    /** Capturing a mouse enables to track input outside of the window. */
    hadal_window_flag_mouse_captured    = (1u << 9),
    /** Window is modal. */
    hadal_window_flag_modal             = (1u << 10),
    /** Window is occluded by another window. */
    hadal_window_flag_occluded          = (1u << 11),
    /** Window should always be above other windows. */
    hadal_window_flag_always_on_top     = (1u << 12),
    /** Window will always minimize when leaving fullscreen. */
    hadal_window_flag_auto_minimize     = (1u << 13),
    /** Window is shell activated, available only on some platforms. */
    hadal_window_flag_shell_activated   = (1u << 14),
    /** Window has a transparent buffer. */
    hadal_window_flag_transparent       = (1u << 15),
    /** Window usable for Vulkan surface. */
    hadal_window_flag_vulkan            = (1u << 16),
    /** Window usable for Metal view. */
    hadal_window_flag_metal             = (1u << 17),
    /** Set on exit events from sources outside of the application.  */
    hadal_window_flag_should_close      = (1u << 18),
};

/** The first member of a 'hadal_window' struct implementation. */
struct hadal_window_header {
    /** The encore used to create this window. Does not modify the refcnt, as it's owned by Riven. */
    struct hadal_encore            *encore;
    /** The window may be destroyed when the reference count reaches zero. */
    atomic_u64                      refcnt;
};

typedef lake_nodiscard enum hadal_result (LAKECALL *PFN_hadal_window_create)(struct hadal_encore *encore, struct hadal_window **out_window);
#define FN_HADAL_WINDOW_CREATE(ENCORE) \
    lake_nodiscard enum hadal_result LAKECALL _hadal_##ENCORE##_window_create(struct hadal_encore *ENCORE, struct hadal_window **out_window)

typedef void (LAKECALL *PFN_hadal_window_destroy)(struct hadal_window *window);
#define FN_HADAL_WINDOW_DESTROY(ENCORE) \
    void LAKECALL _hadal_##ENCORE##_window_destroy(struct hadal_window *window)

typedef u32 (LAKECALL *PFN_hadal_window_visibility)(struct hadal_window *window, bool visible);
#define FN_HADAL_WINDOW_VISIBILITY(ENCORE) \
    u32 LAKECALL _hadal_##ENCORE##_window_visibility(struct hadal_window *window, bool visible)

#ifdef __cplusplus
}
#endif /* __cplusplus */
