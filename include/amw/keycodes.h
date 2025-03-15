#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

/** Represents an unique key. */
enum hadal_keycode {
    hadal_keycode_space         = 32,
    hadal_keycode_apostrophe    = 39,  /* ' */
    hadal_keycode_comma         = 44,  /* , */
    hadal_keycode_minus         = 45,  /* - */
    hadal_keycode_period        = 46,  /* . */
    hadal_keycode_slash         = 47,  /* / */
    hadal_keycode_0             = 48,
    hadal_keycode_1             = 49,
    hadal_keycode_2             = 50,
    hadal_keycode_3             = 51,
    hadal_keycode_4             = 52,
    hadal_keycode_5             = 53,
    hadal_keycode_6             = 54,
    hadal_keycode_7             = 55,
    hadal_keycode_8             = 56,
    hadal_keycode_9             = 57,
    hadal_keycode_semicolon     = 59,  /* ; */
    hadal_keycode_equal         = 61,  /* = */
    hadal_keycode_a             = 65,
    hadal_keycode_b             = 66,
    hadal_keycode_c             = 67,
    hadal_keycode_d             = 68,
    hadal_keycode_e             = 69,
    hadal_keycode_f             = 70,
    hadal_keycode_g             = 71,
    hadal_keycode_h             = 72,
    hadal_keycode_i             = 73,
    hadal_keycode_j             = 74,
    hadal_keycode_k             = 75,
    hadal_keycode_l             = 76,
    hadal_keycode_m             = 77,
    hadal_keycode_n             = 78,
    hadal_keycode_o             = 79,
    hadal_keycode_p             = 80,
    hadal_keycode_q             = 81,
    hadal_keycode_r             = 82,
    hadal_keycode_s             = 83,
    hadal_keycode_t             = 84,
    hadal_keycode_u             = 85,
    hadal_keycode_v             = 86,
    hadal_keycode_w             = 87,
    hadal_keycode_x             = 88,
    hadal_keycode_y             = 89,
    hadal_keycode_z             = 90,
    hadal_keycode_left_bracket  = 91,  /* [ */
    hadal_keycode_backslash     = 92,  /* \ */
    hadal_keycode_right_bracket = 93,  /* ] */
    hadal_keycode_grave_accent  = 96,  /* ` */
    hadal_keycode_world_1       = 161, /* non-US #1 */
    hadal_keycode_world_2       = 162, /* non-US #2 */
    hadal_keycode_escape        = 256,
    hadal_keycode_enter         = 257,
    hadal_keycode_tab           = 258,
    hadal_keycode_backspace     = 259,
    hadal_keycode_insert        = 260,
    hadal_keycode_delete        = 261,
    hadal_keycode_right         = 262,
    hadal_keycode_left          = 263,
    hadal_keycode_down          = 264,
    hadal_keycode_up            = 265,
    hadal_keycode_page_up       = 266,
    hadal_keycode_page_down     = 267,
    hadal_keycode_home          = 268,
    hadal_keycode_end           = 269,
    hadal_keycode_caps_lock     = 280,
    hadal_keycode_scroll_lock   = 281,
    hadal_keycode_num_lock      = 282,
    hadal_keycode_print_screen  = 283,
    hadal_keycode_pause         = 284,
    hadal_keycode_f1            = 290,
    hadal_keycode_f2            = 291,
    hadal_keycode_f3            = 292,
    hadal_keycode_f4            = 293,
    hadal_keycode_f5            = 294,
    hadal_keycode_f6            = 295,
    hadal_keycode_f7            = 296,
    hadal_keycode_f8            = 297,
    hadal_keycode_f9            = 298,
    hadal_keycode_f10           = 299,
    hadal_keycode_f11           = 300,
    hadal_keycode_f12           = 301,
    hadal_keycode_f13           = 302,
    hadal_keycode_f14           = 303,
    hadal_keycode_f15           = 304,
    hadal_keycode_f16           = 305,
    hadal_keycode_f17           = 306,
    hadal_keycode_f18           = 307,
    hadal_keycode_f19           = 308,
    hadal_keycode_f20           = 309,
    hadal_keycode_f21           = 310,
    hadal_keycode_f22           = 311,
    hadal_keycode_f23           = 312,
    hadal_keycode_f24           = 313,
    hadal_keycode_f25           = 314,
    hadal_keycode_kp_0          = 320,
    hadal_keycode_kp_1          = 321,
    hadal_keycode_kp_2          = 322,
    hadal_keycode_kp_3          = 323,
    hadal_keycode_kp_4          = 324,
    hadal_keycode_kp_5          = 325,
    hadal_keycode_kp_6          = 326,
    hadal_keycode_kp_7          = 327,
    hadal_keycode_kp_8          = 328,
    hadal_keycode_kp_9          = 329,
    hadal_keycode_kp_decimal    = 330,
    hadal_keycode_kp_divide     = 331,
    hadal_keycode_kp_multiply   = 332,
    hadal_keycode_kp_subtract   = 333,
    hadal_keycode_kp_add        = 334,
    hadal_keycode_kp_enter      = 335,
    hadal_keycode_kp_equal      = 336,
    hadal_keycode_left_shift    = 340,
    hadal_keycode_left_control  = 341,
    hadal_keycode_left_alt      = 342,
    hadal_keycode_left_super    = 343,
    hadal_keycode_right_shift   = 344,
    hadal_keycode_right_control = 345,
    hadal_keycode_right_alt     = 346,
    hadal_keycode_right_super   = 347,
    hadal_keycode_menu          = 348,
    hadal_keycode_invalid       = 0,
};
#define hadal_keycode_last hadal_keycode_menu

#ifdef __cplusplus
}
#endif
