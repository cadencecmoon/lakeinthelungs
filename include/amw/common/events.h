/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_COMMON_EVENTS_H
#define _AMW_COMMON_EVENTS_H

#include <amw/common/defines.h>
#include <amw/common/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A code to identify the event structure. */
enum event_type {
    event_type_invalid = 0,

    event_type_enum_padding = 0x7FFFFFFF,
};

/** Holds state shared between every event in the union. */
struct event_shared {
    u32 type;       /**< Enum amw_event_type indicates what event structure it is. */
    u32 flags;      /**< Populated by a related systems context state, or 0. */
};

/** Holds every event. */
union event {
    u32 type; /* enum amw_event_type */

    struct event_shared shared;

    u8 padding[32]; /* must be divisible by a byte */
};

static_assertion(sizeof(union event) == sizeof(((union event *)NULL)->padding), "Event union padding not large enough");

#ifdef __cplusplus
}
#endif

#endif /* _AMW_COMMON_EVENTS_H */
