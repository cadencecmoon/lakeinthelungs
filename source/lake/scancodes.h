#pragma once

/** @file lake/scancodes.h
 *  @brief A scancode is the physical representation of an input key, independent of language and keyboard mapping.
 */
#include "types.h"

/** Values are based on the USB usage page standard:
 *  https://usb.org/sites/default/files/hut1_5.pdf */ 
typedef enum : s16 {
    lake_scancode_unknown = 0,

    /* These values are from usage page 0x07 (USB keyboard page) */
    lake_scancode_a = 4,
    lake_scancode_b = 5,
    lake_scancode_c = 6,
    lake_scancode_d = 7,
    lake_scancode_e = 8,
    lake_scancode_f = 9,
    lake_scancode_g = 10,
    lake_scancode_h = 11,
    lake_scancode_i = 12,
    lake_scancode_j = 13,
    lake_scancode_k = 14,
    lake_scancode_l = 15,
    lake_scancode_m = 16,
    lake_scancode_n = 17,
    lake_scancode_o = 18,
    lake_scancode_p = 19,
    lake_scancode_q = 20,
    lake_scancode_r = 21,
    lake_scancode_s = 22,
    lake_scancode_t = 23,
    lake_scancode_u = 24,
    lake_scancode_v = 25,
    lake_scancode_w = 26,
    lake_scancode_x = 27,
    lake_scancode_y = 28,
    lake_scancode_z = 29,

    lake_scancode_1 = 30,
    lake_scancode_2 = 31,
    lake_scancode_3 = 32,
    lake_scancode_4 = 33,
    lake_scancode_5 = 34,
    lake_scancode_6 = 35,
    lake_scancode_7 = 36,
    lake_scancode_8 = 37,
    lake_scancode_9 = 38,
    lake_scancode_0 = 39,
 
    lake_scancode_return = 40,
    lake_scancode_escape = 41,
    lake_scancode_backspace = 42,
    lake_scancode_tab = 43,
    lake_scancode_space = 44,
 
    lake_scancode_minus = 45,
    lake_scancode_equals = 46,
    lake_scancode_leftbracket = 47,
    lake_scancode_rightbracket = 48,
    lake_scancode_backslash = 49, /**< Located at the lower left of the return
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
    lake_scancode_nonushash = 50, /**< ISO USB keyboards actually use this code
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
    lake_scancode_semicolon = 51,
    lake_scancode_apostrophe = 52,
    lake_scancode_grave = 53, /**< Located in the top left corner (on both ANSI
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
    lake_scancode_comma = 54,
    lake_scancode_period = 55,
    lake_scancode_slash = 56,

    lake_scancode_capslock = 57,

    lake_scancode_f1 = 58,
    lake_scancode_f2 = 59,
    lake_scancode_f3 = 60,
    lake_scancode_f4 = 61,
    lake_scancode_f5 = 62,
    lake_scancode_f6 = 63,
    lake_scancode_f7 = 64,
    lake_scancode_f8 = 65,
    lake_scancode_f9 = 66,
    lake_scancode_f10 = 67,
    lake_scancode_f11 = 68,
    lake_scancode_f12 = 69,

    lake_scancode_printscreen = 70,
    lake_scancode_scrolllock = 71,
    lake_scancode_pause = 72,
    lake_scancode_insert = 73, /**< insert on PC, help on some Mac keyboards (but does send code 73, not 117) */
    lake_scancode_home = 74,
    lake_scancode_pageup = 75,
    lake_scancode_delete = 76,
    lake_scancode_end = 77,
    lake_scancode_pagedown = 78,
    lake_scancode_right = 79,
    lake_scancode_left = 80,
    lake_scancode_down = 81,
    lake_scancode_up = 82,

    lake_scancode_numlockclear = 83, /**< num lock on PC, clear on Mac keyboards */
    lake_scancode_kp_divide = 84,
    lake_scancode_kp_multiply = 85,
    lake_scancode_kp_minus = 86,
    lake_scancode_kp_plus = 87,
    lake_scancode_kp_enter = 88,
    lake_scancode_kp_1 = 89,
    lake_scancode_kp_2 = 90,
    lake_scancode_kp_3 = 91,
    lake_scancode_kp_4 = 92,
    lake_scancode_kp_5 = 93,
    lake_scancode_kp_6 = 94,
    lake_scancode_kp_7 = 95,
    lake_scancode_kp_8 = 96,
    lake_scancode_kp_9 = 97,
    lake_scancode_kp_0 = 98,
    lake_scancode_kp_period = 99,

    lake_scancode_nonusbackslash = 100, /**< This is the additional key that ISO
                                         *   keyboards have over ANSI ones,
                                         *   located between left shift and Y.
                                         *   Produces GRAVE ACCENT and TILDE in a
                                         *   US or UK Mac layout, REVERSE SOLIDUS
                                         *   (backslash) and VERTICAL LINE in a
                                         *   US or UK Windows layout, and
                                         *   LESS-THAN SIGN and GREATER-THAN SIGN
                                         *   in a Swiss German, German, or French
                                         *   layout. */
    lake_scancode_application = 101, /**< windows contextual menu, compose */
    lake_scancode_power = 102, /**< The USB document says this is a status flag,
                                *   not a physical key - but some Mac keyboards
                                *   do have a power key. */
    lake_scancode_kp_equals = 103,
    lake_scancode_f13 = 104,
    lake_scancode_f14 = 105,
    lake_scancode_f15 = 106,
    lake_scancode_f16 = 107,
    lake_scancode_f17 = 108,
    lake_scancode_f18 = 109,
    lake_scancode_f19 = 110,
    lake_scancode_f20 = 111,
    lake_scancode_f21 = 112,
    lake_scancode_f22 = 113,
    lake_scancode_f23 = 114,
    lake_scancode_f24 = 115,
    lake_scancode_execute = 116,
    lake_scancode_help = 117,    /**< AL Integrated Help Center */
    lake_scancode_menu = 118,    /**< Menu (show menu) */
    lake_scancode_select = 119,
    lake_scancode_stop = 120,    /**< AC Stop */
    lake_scancode_again = 121,   /**< AC Redo/Repeat */
    lake_scancode_undo = 122,    /**< AC Undo */
    lake_scancode_cut = 123,     /**< AC Cut */
    lake_scancode_copy = 124,    /**< AC Copy */
    lake_scancode_paste = 125,   /**< AC Paste */
    lake_scancode_find = 126,    /**< AC Find */
    lake_scancode_mute = 127,
    lake_scancode_volumeup = 128,
    lake_scancode_volumedown = 129,
/* not sure whether there's a reason to enable these */
/*     lake_scancode_lockingcapslock = 130, */
/*     lake_scancode_lockingnumlock = 131, */
/*     lake_scancode_lockingscrolllock = 132, */
    lake_scancode_kp_comma = 133,
    lake_scancode_kp_equalsas400 = 134,

    lake_scancode_international1 = 135, /**< used on Asian keyboards, see footnotes in USB doc */
    lake_scancode_international2 = 136,
    lake_scancode_international3 = 137, /**< Yen */
    lake_scancode_international4 = 138,
    lake_scancode_international5 = 139,
    lake_scancode_international6 = 140,
    lake_scancode_international7 = 141,
    lake_scancode_international8 = 142,
    lake_scancode_international9 = 143,
    lake_scancode_lang1 = 144, /**< Hangul/English toggle */
    lake_scancode_lang2 = 145, /**< Hanja conversion */
    lake_scancode_lang3 = 146, /**< Katakana */
    lake_scancode_lang4 = 147, /**< Hiragana */
    lake_scancode_lang5 = 148, /**< Zenkaku/Hankaku */
    lake_scancode_lang6 = 149, /**< reserved */
    lake_scancode_lang7 = 150, /**< reserved */
    lake_scancode_lang8 = 151, /**< reserved */
    lake_scancode_lang9 = 152, /**< reserved */

    lake_scancode_alterase = 153,    /**< Erase-Eaze */
    lake_scancode_sysreq = 154,
    lake_scancode_cancel = 155,      /**< AC Cancel */
    lake_scancode_clear = 156,
    lake_scancode_prior = 157,
    lake_scancode_return2 = 158,
    lake_scancode_separator = 159,
    lake_scancode_out = 160,
    lake_scancode_oper = 161,
    lake_scancode_clearagain = 162,
    lake_scancode_crsel = 163,
    lake_scancode_exsel = 164,

    lake_scancode_kp_00 = 176,
    lake_scancode_kp_000 = 177,
    lake_scancode_thousandsseparator = 178,
    lake_scancode_decimalseparator = 179,
    lake_scancode_currencyunit = 180,
    lake_scancode_currencysubunit = 181,
    lake_scancode_kp_leftparen = 182,
    lake_scancode_kp_rightparen = 183,
    lake_scancode_kp_leftbrace = 184,
    lake_scancode_kp_rightbrace = 185,
    lake_scancode_kp_tab = 186,
    lake_scancode_kp_backspace = 187,
    lake_scancode_kp_a = 188,
    lake_scancode_kp_b = 189,
    lake_scancode_kp_c = 190,
    lake_scancode_kp_d = 191,
    lake_scancode_kp_e = 192,
    lake_scancode_kp_f = 193,
    lake_scancode_kp_xor = 194,
    lake_scancode_kp_power = 195,
    lake_scancode_kp_percent = 196,
    lake_scancode_kp_less = 197,
    lake_scancode_kp_greater = 198,
    lake_scancode_kp_ampersand = 199,
    lake_scancode_kp_dblampersand = 200,
    lake_scancode_kp_verticalbar = 201,
    lake_scancode_kp_dblverticalbar = 202,
    lake_scancode_kp_colon = 203,
    lake_scancode_kp_hash = 204,
    lake_scancode_kp_space = 205,
    lake_scancode_kp_at = 206,
    lake_scancode_kp_exclam = 207,
    lake_scancode_kp_memstore = 208,
    lake_scancode_kp_memrecall = 209,
    lake_scancode_kp_memclear = 210,
    lake_scancode_kp_memadd = 211,
    lake_scancode_kp_memsubtract = 212,
    lake_scancode_kp_memmultiply = 213,
    lake_scancode_kp_memdivide = 214,
    lake_scancode_kp_plusminus = 215,
    lake_scancode_kp_clear = 216,
    lake_scancode_kp_clearentry = 217,
    lake_scancode_kp_binary = 218,
    lake_scancode_kp_octal = 219,
    lake_scancode_kp_decimal = 220,
    lake_scancode_kp_hexadecimal = 221,
 
    lake_scancode_lctrl = 224,
    lake_scancode_lshift = 225,
    lake_scancode_lalt = 226, /**< alt, option */
    lake_scancode_lgui = 227, /**< windows, command (apple), meta */
    lake_scancode_rctrl = 228,
    lake_scancode_rshift = 229,
    lake_scancode_ralt = 230, /**< alt gr, option */
    lake_scancode_rgui = 231, /**< windows, command (apple), meta */

    lake_scancode_mode = 257, /**< I'm not sure if this is really not covered
                               *   by any of the above, but since there's a
                               *   special SDL_KMOD_MODE for it I'm adding it here */

    /* These values are mapped from usage page 0x0C (USB consumer page).
     *
     * There are way more keys in the spec than we can represent in the
     * current scancode range, so pick the ones that commonly come up in
     * real world usage. */
    lake_scancode_sleep = 258,               /**< Sleep */
    lake_scancode_wake = 259,                /**< Wake */

    lake_scancode_channel_increment = 260,   /**< Channel Increment */
    lake_scancode_channel_decrement = 261,   /**< Channel Decrement */

    lake_scancode_media_play = 262,          /**< Play */
    lake_scancode_media_pause = 263,         /**< Pause */
    lake_scancode_media_record = 264,        /**< Record */
    lake_scancode_media_fast_forward = 265,  /**< Fast Forward */
    lake_scancode_media_rewind = 266,        /**< Rewind */
    lake_scancode_media_next_track = 267,    /**< Next Track */
    lake_scancode_media_previous_track = 268, /**< Previous Track */
    lake_scancode_media_stop = 269,          /**< Stop */
    lake_scancode_media_eject = 270,         /**< Eject */
    lake_scancode_media_play_pause = 271,    /**< Play / Pause */
    lake_scancode_media_select = 272,        /* Media Select */

    lake_scancode_ac_new = 273,              /**< AC New */
    lake_scancode_ac_open = 274,             /**< AC Open */
    lake_scancode_ac_close = 275,            /**< AC Close */
    lake_scancode_ac_exit = 276,             /**< AC Exit */
    lake_scancode_ac_save = 277,             /**< AC Save */
    lake_scancode_ac_print = 278,            /**< AC Print */
    lake_scancode_ac_properties = 279,       /**< AC Properties */

    lake_scancode_ac_search = 280,           /**< AC Search */
    lake_scancode_ac_home = 281,             /**< AC Home */
    lake_scancode_ac_back = 282,             /**< AC Back */
    lake_scancode_ac_forward = 283,          /**< AC Forward */
    lake_scancode_ac_stop = 284,             /**< AC Stop */
    lake_scancode_ac_refresh = 285,          /**< AC Refresh */
    lake_scancode_ac_bookmarks = 286,        /**< AC Bookmarks */

    /* these are values that are often used on mobile phones */
    lake_scancode_softleft = 287, /**< usually situated below the display on phones and
                                   *   used as a multi-function feature key for selecting
                                   *   a software defined function shown on the bottom left
                                   *   of the display. */
    lake_scancode_softright = 288, /**< usually situated below the display on phones and
                                    *   used as a multi-function feature key for selecting
                                    *    a software defined function shown on the bottom right
                                    *    of the display. */
    lake_scancode_call = 289,    /**< used for accepting phone calls. */
    lake_scancode_endcall = 290, /**< used for rejecting phone calls. */

    /* add any other keys here. */
    lake_scancode_reserved = 400,  /**< 400-500 reserved for dynamic keycodes */
    lake_scancode_count = 512      /**< not a key, just marks the number of scancodes for array bounds */
} lake_scancode;

