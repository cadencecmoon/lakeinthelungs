/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_INPUT_MOUSE_H
#define _AMW_INPUT_MOUSE_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

/** Opaque structure used to identify a cursor object. */
struct cursor;

enum mouse_wheel {
    mouse_wheel_normal,
    mouse_wheel_flipped,
};

enum mouse_button {
    mouse_button_1       = 0x0001,
    mouse_button_2       = 0x0002,
    mouse_button_3       = 0x0004,
    mouse_button_4       = 0x0008,
    mouse_button_5       = 0x0010,
    mouse_button_6       = 0x0020,
    mouse_button_7       = 0x0040,
    mouse_button_8       = 0x0080,
    mouse_button_left    = mouse_button_1,
    mouse_button_right   = mouse_button_2,
    mouse_button_middle  = mouse_button_3,
    mouse_button_invalid = 0,
};
#define mouse_button_last mouse_button_8

AMW_C_DECL_END
#endif /* _AMW_INPUT_MOUSE_H */
