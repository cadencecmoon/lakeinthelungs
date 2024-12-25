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
    uint64_t id;
    float x, y;
    float pressure;
};

#ifdef __cplusplus
}
#endif

#endif /* _AMW_INPUT_TOUCH_H */
