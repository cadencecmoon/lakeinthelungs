#pragma once

#include "../../bedrock/types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum pen_input {
    pen_input_down       = (1u << 0),  /**< Pen is pressed down. */
    pen_input_button_1   = (1u << 1),  /**< Button 1 is pressed. */
    pen_input_button_2   = (1u << 2),  /**< Button 2 is pressed. */
    pen_input_button_3   = (1u << 3),  /**< Button 3 is pressed. */
    pen_input_button_4   = (1u << 4),  /**< Button 4 is pressed. */
    pen_input_button_5   = (1u << 5),  /**< Button 5 is pressed. */
    pen_input_eraser_tip = (1u << 30), /**< Eraser tip is used. */
};

enum pen_axis {
    pen_axis_pressure = 0,          /**< Pen pressure. Unidirectional: 0.0 to 1.0. */
    pen_axis_xtilt,                 /**< Pen horizontal tilt angle. Bidirectional: -90.0 to 90.0 (left-to-right). */
    pen_axis_ytilt,                 /**< Pen vertical tilt angle. Bidirectional: -90.0 to 90.0 (top-to-down). */
    pen_axis_distance,              /**< Pen distance to drawing surface. Unidirectional: 0.0 to 1.0. */
    pen_axis_rotation,              /**< Pen barrel rotation. Bidirectional: -180.0 to 179.9 (clockwise, 0 is facing up, -180 is facing down). */
    pen_axis_slider,                /**< Pen finger wheel or slider. Unidirectional: 0.0 to 1.0. */
    pen_axis_tangential_pressure,   /**< Pressure from squeezing the pen ("barrel pressure"). */
    pen_axis_count,                 /**< Total known pen axis types. */
};

#ifdef __cplusplus
}
#endif
