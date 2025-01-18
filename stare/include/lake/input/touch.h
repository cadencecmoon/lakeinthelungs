/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_INPUT_TOUCH_H
#define _AMW_INPUT_TOUCH_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

enum touch_device {
    touch_device_invalid = -1,
    touch_device_direct,            /**< Touch screen with window-relative coordinates. */
    touch_device_indirect_absolute, /**< Trackpad with absolute device coordinates. */
    touch_device_indirect_relative, /**< Trackpad with screen cursor-relative coordinates. */
};

/** Data about a single finger in a multitouch event. */
struct finger {
    u64 id;
    f32 x, y;
    f32 pressure;
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_INPUT_TOUCH_H */
