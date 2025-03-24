#pragma once

#include <amw/hadal.h>

/** A joystick implementation shared between all Linux display backends (Wayland, XCB, DRM/KMS) */
struct hadal_joystick {
    struct hadal_joystick_header        header;
};
