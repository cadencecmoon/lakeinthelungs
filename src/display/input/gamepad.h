#pragma once

#include "../../bedrock/types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum gamepad_button {
    gamepad_button_a            = 0x0001,
    gamepad_button_b            = 0x0002,
    gamepad_button_x            = 0x0004,
    gamepad_button_y            = 0x0008,
    gamepad_button_left_bumper  = 0x0010,
    gamepad_button_right_bumper = 0x0020,
    gamepad_button_back         = 0x0040,
    gamepad_button_start        = 0x0080,
    gamepad_button_guide        = 0x0100,
    gamepad_button_left_thumb   = 0x0200,
    gamepad_button_right_thumb  = 0x0400,
    gamepad_button_dpad_up      = 0x0800,
    gamepad_button_dpad_right   = 0x1000,
    gamepad_button_dpad_down    = 0x2000,
    gamepad_button_dpad_left    = 0x4000,
    gamepad_button_cross        = gamepad_button_a,
    gamepad_button_circle       = gamepad_button_b,
    gamepad_button_square       = gamepad_button_x,
    gamepad_button_triangle     = gamepad_button_y,
    gamepad_button_invalid      = 0,
};
#define gamepad_button_last gamepad_button_dpad_left

enum gamepad_axis {
    gamepad_axis_left_x         = 0x0001,
    gamepad_axis_left_y         = 0x0002,
    gamepad_axis_right_x        = 0x0004,
    gamepad_axis_right_y        = 0x0008,
    gamepad_axis_left_trigger   = 0x0010,
    gamepad_axis_right_trigger  = 0x0020,
};
#define gamepad_axis_last gamepad_axis_right_trigger

enum gamepad_hat {
    gamepad_hat_centered    = 0,
    gamepad_hat_up          = 0x0001,
    gamepad_hat_right       = 0x0002,
    gamepad_hat_down        = 0x0004,
    gamepad_hat_left        = 0x0008,
    gamepad_hat_right_up    = (gamepad_hat_right | gamepad_hat_up),
    gamepad_hat_right_down  = (gamepad_hat_right | gamepad_hat_down),
    gamepad_hat_left_up     = (gamepad_hat_left  | gamepad_hat_up),
    gamepad_hat_left_down   = (gamepad_hat_left  | gamepad_hat_down),
};

struct gamepad_state {
    u8  buttons[15];
    f32 axes[6];
};

#ifdef __cplusplus
}
#endif
