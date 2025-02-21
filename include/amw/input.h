#pragma once

#include <amw/bedrock.h>

enum keycode {
    keycode_space         = 32,
    keycode_apostrophe    = 39,  /* ' */
    keycode_comma         = 44,  /* , */
    keycode_minus         = 45,  /* - */
    keycode_period        = 46,  /* . */
    keycode_slash         = 47,  /* / */
    keycode_0             = 48,
    keycode_1             = 49,
    keycode_2             = 50,
    keycode_3             = 51,
    keycode_4             = 52,
    keycode_5             = 53,
    keycode_6             = 54,
    keycode_7             = 55,
    keycode_8             = 56,
    keycode_9             = 57,
    keycode_semicolon     = 59,  /* ; */
    keycode_equal         = 61,  /* = */
    keycode_a             = 65,
    keycode_b             = 66,
    keycode_c             = 67,
    keycode_d             = 68,
    keycode_e             = 69,
    keycode_f             = 70,
    keycode_g             = 71,
    keycode_h             = 72,
    keycode_i             = 73,
    keycode_j             = 74,
    keycode_k             = 75,
    keycode_l             = 76,
    keycode_m             = 77,
    keycode_n             = 78,
    keycode_o             = 79,
    keycode_p             = 80,
    keycode_q             = 81,
    keycode_r             = 82,
    keycode_s             = 83,
    keycode_t             = 84,
    keycode_u             = 85,
    keycode_v             = 86,
    keycode_w             = 87,
    keycode_x             = 88,
    keycode_y             = 89,
    keycode_z             = 90,
    keycode_left_bracket  = 91,  /* [ */
    keycode_backslash     = 92,  /* \ */
    keycode_right_bracket = 93,  /* ] */
    keycode_grave_accent  = 96,  /* ` */
    keycode_world_1       = 161, /* non-US #1 */
    keycode_world_2       = 162, /* non-US #2 */
    keycode_escape        = 256,
    keycode_enter         = 257,
    keycode_tab           = 258,
    keycode_backspace     = 259,
    keycode_insert        = 260,
    keycode_delete        = 261,
    keycode_right         = 262,
    keycode_left          = 263,
    keycode_down          = 264,
    keycode_up            = 265,
    keycode_page_up       = 266,
    keycode_page_down     = 267,
    keycode_home          = 268,
    keycode_end           = 269,
    keycode_caps_lock     = 280,
    keycode_scroll_lock   = 281,
    keycode_num_lock      = 282,
    keycode_print_screen  = 283,
    keycode_pause         = 284,
    keycode_f1            = 290,
    keycode_f2            = 291,
    keycode_f3            = 292,
    keycode_f4            = 293,
    keycode_f5            = 294,
    keycode_f6            = 295,
    keycode_f7            = 296,
    keycode_f8            = 297,
    keycode_f9            = 298,
    keycode_f10           = 299,
    keycode_f11           = 300,
    keycode_f12           = 301,
    keycode_f13           = 302,
    keycode_f14           = 303,
    keycode_f15           = 304,
    keycode_f16           = 305,
    keycode_f17           = 306,
    keycode_f18           = 307,
    keycode_f19           = 308,
    keycode_f20           = 309,
    keycode_f21           = 310,
    keycode_f22           = 311,
    keycode_f23           = 312,
    keycode_f24           = 313,
    keycode_f25           = 314,
    keycode_kp_0          = 320,
    keycode_kp_1          = 321,
    keycode_kp_2          = 322,
    keycode_kp_3          = 323,
    keycode_kp_4          = 324,
    keycode_kp_5          = 325,
    keycode_kp_6          = 326,
    keycode_kp_7          = 327,
    keycode_kp_8          = 328,
    keycode_kp_9          = 329,
    keycode_kp_decimal    = 330,
    keycode_kp_divide     = 331,
    keycode_kp_multiply   = 332,
    keycode_kp_subtract   = 333,
    keycode_kp_add        = 334,
    keycode_kp_enter      = 335,
    keycode_kp_equal      = 336,
    keycode_left_shift    = 340,
    keycode_left_control  = 341,
    keycode_left_alt      = 342,
    keycode_left_super    = 343,
    keycode_right_shift   = 344,
    keycode_right_control = 345,
    keycode_right_alt     = 346,
    keycode_right_super   = 347,
    keycode_menu          = 348,
    keycode_invalid       = 0,
};
#define keycode_last keycode_menu

/** Key modifiers. */
enum keymod {
    keymod_shift     = 0x0001, 
    keymod_ctrl      = 0x0002, 
    keymod_alt       = 0x0004, 
    keymod_super     = 0x0008, 
    keymod_caps_lock = 0x0010,
    keymod_num_lock  = 0x0020,
    keymod_invalid   = 0,
};

/** A scancode is the physical representation of a key on the keyboard,
 *  independent of language and keyboard mapping.
 *
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://usb.org/sites/default/files/hut1_5.pdf */
enum scancode {
    scancode_unknown = 0,

    /* These values are from usage page 0x07 (USB keyboard page) */
    scancode_a = 4,
    scancode_b = 5,
    scancode_c = 6,
    scancode_d = 7,
    scancode_e = 8,
    scancode_f = 9,
    scancode_g = 10,
    scancode_h = 11,
    scancode_i = 12,
    scancode_j = 13,
    scancode_k = 14,
    scancode_l = 15,
    scancode_m = 16,
    scancode_n = 17,
    scancode_o = 18,
    scancode_p = 19,
    scancode_q = 20,
    scancode_r = 21,
    scancode_s = 22,
    scancode_t = 23,
    scancode_u = 24,
    scancode_v = 25,
    scancode_w = 26,
    scancode_x = 27,
    scancode_y = 28,
    scancode_z = 29,

    scancode_1 = 30,
    scancode_2 = 31,
    scancode_3 = 32,
    scancode_4 = 33,
    scancode_5 = 34,
    scancode_6 = 35,
    scancode_7 = 36,
    scancode_8 = 37,
    scancode_9 = 38,
    scancode_0 = 39,

    scancode_return = 40,
    scancode_escape = 41,
    scancode_backspace = 42,
    scancode_tab = 43,
    scancode_space = 44,

    scancode_minus = 45,
    scancode_equals = 46,
    scancode_leftbracket = 47,
    scancode_rightbracket = 48,
    scancode_backslash = 49, /**< Located at the lower left of the return
                              *   key on ISO keyboards and at the right end
                              *   of the QWERTY row on ANSI keyboards.
                              *   Produces REVERSE SOLIDUS (backslash) and
                              *   VERTICAL LINE in a US layout, REVERSE
                              *   SOLIDUS and VERTICAL LINE in a UK Mac
                              *   layout, NUMBER SIGN and TILDE in a UK
                              *   Windows layout, DOLLAR SIGN and POUND SIGN
                              *   in a Swiss German layout, NUMBER SIGN and
                              *   APOSTROPHE in a German layout, GRAVE
                              *   ACCENT and POUND SIGN in a French Mac
                              *   layout, and ASTERISK and MICRO SIGN in a
                              *   French Windows layout. */
    scancode_nonushash = 50, /**< ISO USB keyboards actually use this code
                              *   instead of 49 for the same key, but all
                              *   OSes I've seen treat the two codes
                              *   identically. So, as an implementor, unless
                              *   your keyboard generates both of those
                              *   codes and your OS treats them differently,
                              *   you should generate SDL_SCANCODE_BACKSLASH
                              *   instead of this code. As a user, you
                              *   should not rely on this code because SDL
                              *   will never generate it with most (all?)
                              *   keyboards. */
    scancode_semicolon = 51,
    scancode_apostrophe = 52,
    scancode_grave = 53, /**< Located in the top left corner (on both ANSI
                          *   and ISO keyboards). Produces GRAVE ACCENT and
                          *   TILDE in a US Windows layout and in US and UK
                          *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                          *   and NOT SIGN in a UK Windows layout, SECTION
                          *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                          *   layouts on ISO keyboards, SECTION SIGN and
                          *   DEGREE SIGN in a Swiss German layout (Mac:
                          *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                          *   DEGREE SIGN in a German layout (Mac: only on
                          *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                          *   French Windows layout, COMMERCIAL AT and
                          *   NUMBER SIGN in a French Mac layout on ISO
                          *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                          *   SIGN in a Swiss German, German, or French Mac
                          *   layout on ANSI keyboards. */
    scancode_comma = 54,
    scancode_period = 55,
    scancode_slash = 56,

    scancode_capslock = 57,

    scancode_f1 = 58,
    scancode_f2 = 59,
    scancode_f3 = 60,
    scancode_f4 = 61,
    scancode_f5 = 62,
    scancode_f6 = 63,
    scancode_f7 = 64,
    scancode_f8 = 65,
    scancode_f9 = 66,
    scancode_f10 = 67,
    scancode_f11 = 68,
    scancode_f12 = 69,

    scancode_printscreen = 70,
    scancode_scrolllock = 71,
    scancode_pause = 72,
    scancode_insert = 73, /**< insert on PC, help on some Mac keyboards (but
                               does send code 73, not 117) */
    scancode_home = 74,
    scancode_pageup = 75,
    scancode_delete = 76,
    scancode_end = 77,
    scancode_pagedown = 78,
    scancode_right = 79,
    scancode_left = 80,
    scancode_down = 81,
    scancode_up = 82,

    scancode_numlockclear = 83, /**< num lock on PC, clear on Mac keyboards */
    scancode_kp_divide = 84,
    scancode_kp_multiply = 85,
    scancode_kp_minus = 86,
    scancode_kp_plus = 87,
    scancode_kp_enter = 88,
    scancode_kp_1 = 89,
    scancode_kp_2 = 90,
    scancode_kp_3 = 91,
    scancode_kp_4 = 92,
    scancode_kp_5 = 93,
    scancode_kp_6 = 94,
    scancode_kp_7 = 95,
    scancode_kp_8 = 96,
    scancode_kp_9 = 97,
    scancode_kp_0 = 98,
    scancode_kp_period = 99,

    scancode_nonusbackslash = 100, /**< This is the additional key that ISO
                                    *   keyboards have over ANSI ones,
                                    *   located between left shift and Y.
                                    *   Produces GRAVE ACCENT and TILDE in a
                                    *   US or UK Mac layout, REVERSE SOLIDUS
                                    *   (backslash) and VERTICAL LINE in a
                                    *   US or UK Windows layout, and
                                    *   LESS-THAN SIGN and GREATER-THAN SIGN
                                    *   in a Swiss German, German, or French
                                    *   layout. */
    scancode_application = 101, /**< windows contextual menu, compose */
    scancode_power = 102, /**< The USB document says this is a status flag,
                           *   not a physical key - but some Mac keyboards
                           *   do have a power key. */
    scancode_kp_equals = 103,
    scancode_f13 = 104,
    scancode_f14 = 105,
    scancode_f15 = 106,
    scancode_f16 = 107,
    scancode_f17 = 108,
    scancode_f18 = 109,
    scancode_f19 = 110,
    scancode_f20 = 111,
    scancode_f21 = 112,
    scancode_f22 = 113,
    scancode_f23 = 114,
    scancode_f24 = 115,
    scancode_execute = 116,
    scancode_help = 117,    /**< AL Integrated Help Center */
    scancode_menu = 118,    /**< Menu (show menu) */
    scancode_select = 119,
    scancode_stop = 120,    /**< AC Stop */
    scancode_again = 121,   /**< AC Redo/Repeat */
    scancode_undo = 122,    /**< AC Undo */
    scancode_cut = 123,     /**< AC Cut */
    scancode_copy = 124,    /**< AC Copy */
    scancode_paste = 125,   /**< AC Paste */
    scancode_find = 126,    /**< AC Find */
    scancode_mute = 127,
    scancode_volumeup = 128,
    scancode_volumedown = 129,
/* not sure whether there's a reason to enable these */
/*     scancode_lockingcapslock = 130,  */
/*     scancode_lockingnumlock = 131, */
/*     scancode_lockingscrolllock = 132, */
    scancode_kp_comma = 133,
    scancode_kp_equalsas400 = 134,

    scancode_international1 = 135, /**< used on Asian keyboards, see
                                        footnotes in USB doc */
    scancode_international2 = 136,
    scancode_international3 = 137, /**< Yen */
    scancode_international4 = 138,
    scancode_international5 = 139,
    scancode_international6 = 140,
    scancode_international7 = 141,
    scancode_international8 = 142,
    scancode_international9 = 143,
    scancode_lang1 = 144, /**< Hangul/English toggle */
    scancode_lang2 = 145, /**< Hanja conversion */
    scancode_lang3 = 146, /**< Katakana */
    scancode_lang4 = 147, /**< Hiragana */
    scancode_lang5 = 148, /**< Zenkaku/Hankaku */
    scancode_lang6 = 149, /**< reserved */
    scancode_lang7 = 150, /**< reserved */
    scancode_lang8 = 151, /**< reserved */
    scancode_lang9 = 152, /**< reserved */

    scancode_alterase = 153,    /**< Erase-Eaze */
    scancode_sysreq = 154,
    scancode_cancel = 155,      /**< AC Cancel */
    scancode_clear = 156,
    scancode_prior = 157,
    scancode_return2 = 158,
    scancode_separator = 159,
    scancode_out = 160,
    scancode_oper = 161,
    scancode_clearagain = 162,
    scancode_crsel = 163,
    scancode_exsel = 164,

    scancode_kp_00 = 176,
    scancode_kp_000 = 177,
    scancode_thousandsseparator = 178,
    scancode_decimalseparator = 179,
    scancode_currencyunit = 180,
    scancode_currencysubunit = 181,
    scancode_kp_leftparen = 182,
    scancode_kp_rightparen = 183,
    scancode_kp_leftbrace = 184,
    scancode_kp_rightbrace = 185,
    scancode_kp_tab = 186,
    scancode_kp_backspace = 187,
    scancode_kp_a = 188,
    scancode_kp_b = 189,
    scancode_kp_c = 190,
    scancode_kp_d = 191,
    scancode_kp_e = 192,
    scancode_kp_f = 193,
    scancode_kp_xor = 194,
    scancode_kp_power = 195,
    scancode_kp_percent = 196,
    scancode_kp_less = 197,
    scancode_kp_greater = 198,
    scancode_kp_ampersand = 199,
    scancode_kp_dblampersand = 200,
    scancode_kp_verticalbar = 201,
    scancode_kp_dblverticalbar = 202,
    scancode_kp_colon = 203,
    scancode_kp_hash = 204,
    scancode_kp_space = 205,
    scancode_kp_at = 206,
    scancode_kp_exclam = 207,
    scancode_kp_memstore = 208,
    scancode_kp_memrecall = 209,
    scancode_kp_memclear = 210,
    scancode_kp_memadd = 211,
    scancode_kp_memsubtract = 212,
    scancode_kp_memmultiply = 213,
    scancode_kp_memdivide = 214,
    scancode_kp_plusminus = 215,
    scancode_kp_clear = 216,
    scancode_kp_clearentry = 217,
    scancode_kp_binary = 218,
    scancode_kp_octal = 219,
    scancode_kp_decimal = 220,
    scancode_kp_hexadecimal = 221,

    scancode_lctrl = 224,
    scancode_lshift = 225,
    scancode_lalt = 226, /**< alt, option */
    scancode_lgui = 227, /**< windows, command (apple), meta */
    scancode_rctrl = 228,
    scancode_rshift = 229,
    scancode_ralt = 230, /**< alt gr, option */
    scancode_rgui = 231, /**< windows, command (apple), meta */

    scancode_mode = 257, /**< I'm not sure if this is really not covered
                          *   by any of the above, but since there's a
                          *   special SDL_KMOD_MODE for it I'm adding it here */

    /**
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage. */

    scancode_sleep = 258,               /**< Sleep */
    scancode_wake = 259,                /**< Wake */

    scancode_channel_increment = 260,   /**< Channel Increment */
    scancode_channel_decrement = 261,   /**< Channel Decrement */

    scancode_media_play = 262,          /**< Play */
    scancode_media_pause = 263,         /**< Pause */
    scancode_media_record = 264,        /**< Record */
    scancode_media_fast_forward = 265,  /**< Fast Forward */
    scancode_media_rewind = 266,        /**< Rewind */
    scancode_media_next_track = 267,    /**< Next Track */
    scancode_media_previous_track = 268, /**< Previous Track */
    scancode_media_stop = 269,          /**< Stop */
    scancode_media_eject = 270,         /**< Eject */
    scancode_media_play_pause = 271,    /**< Play / Pause */
    scancode_media_select = 272,        /* Media Select */

    scancode_ac_new = 273,              /**< AC New */
    scancode_ac_open = 274,             /**< AC Open */
    scancode_ac_close = 275,            /**< AC Close */
    scancode_ac_exit = 276,             /**< AC Exit */
    scancode_ac_save = 277,             /**< AC Save */
    scancode_ac_print = 278,            /**< AC Print */
    scancode_ac_properties = 279,       /**< AC Properties */

    scancode_ac_search = 280,           /**< AC Search */
    scancode_ac_home = 281,             /**< AC Home */
    scancode_ac_back = 282,             /**< AC Back */
    scancode_ac_forward = 283,          /**< AC Forward */
    scancode_ac_stop = 284,             /**< AC Stop */
    scancode_ac_refresh = 285,          /**< AC Refresh */
    scancode_ac_bookmarks = 286,        /**< AC Bookmarks */

    /** these are values that are often used on mobile phones. */

    scancode_softleft = 287, /**< usually situated below the display on phones and
                                  used as a multi-function feature key for selecting
                                  a software defined function shown on the bottom left
                                  of the display. */
    scancode_softright = 288, /**< usually situated below the display on phones and
                                   used as a multi-function feature key for selecting
                                   a software defined function shown on the bottom right
                                   of the display. */
    scancode_call = 289, /**< used for accepting phone calls. */
    scancode_endcall = 290, /**< used for rejecting phone calls. */

    /* add any other keys here. */
    scancode_reserved = 400,    /**< 400-500 reserved for dynamic keycodes */

    scancode_count = 512 /**< not a key, just marks the number of scancodes for array bounds */
};

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

struct gamepad_map {
    u8 type;
    u8 index;
    s8 axis_scale;
    s8 axis_offset;
};

struct gamepad {
    char                name[128];
    char                guid[33];
    struct gamepad_map  buttons[15];
    struct gamepad_map  axes[6];
};

struct gamepad_state {
    u8  buttons[15];
    f32 axes[6];
};

struct joystick {
    b32                 connected;
    f32                *axes;
    u32                 axis_count;
    u8                 *buttons;
    u32                 button_count;
    u8                 *hats;
    u32                 hat_count;
    struct gamepad     *mapping;
    void               *internal;
};

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
