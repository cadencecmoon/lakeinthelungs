#pragma once

#include <amwe/hadal/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HADAL_INTERFACE_MONITOR_HEADER \
    /**< The encore that owns this output monitor. */ \
    struct hadal_encore *encore;

/** A view into an output monitor. */
union hadal_monitor_view {
    struct {HADAL_INTERFACE_MONITOR_HEADER}    *header;
    struct hadal_monitor                       *monitor; 
    union hadal_encore_view                    *encore_view;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
