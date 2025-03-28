#pragma once

#include <amw/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Combines a string/buffer with information about it's length and ownership. */
struct str {
    const char *ptr;
    usize       length;
};

/** NULL string. */
#define str_null ((struct str){ 0, 0 })

/** C string literal. */
#define str_init(cstr) ((struct str){ cstr, lengthof(cstr) })

/** Length of the string. */
attr_inline 
usize str_len(const struct str s) { 
    return s.length;
}

/** Pointer of the string. */
attr_inline
const char *str_ptr(const struct str s) {
    return s.ptr;
}

/** End of the string. */
attr_inline
const char *str_end(const struct str s) { 
    const char *ptr = str_ptr(s);
    return ptr ? ptr + str_len(s) : NULL;
}

/** Test if the string is empty. */
attr_inline
b32 str_is_empty(const struct str s) {
    return s.length == 0;
}

/** Reset the string to str_null. */
attr_inline
void str_clear(struct str * const ps) {
    *ps = str_null;
}

/** Move the string, reset the source to str_null. */
attr_inline
struct str str_move(struct str * const ps) {
    const struct str t = *ps;
    str_clear(ps);
    return t;
}

/** Swap two string objects. */
AMWAPI attr_hot void AMWCALL str_swap(struct str * const s1, struct str * const s2);

/** Compare two strings lexicographically. */
AMWAPI s32 AMWCALL str_cmp(const struct str s1, const struct str s2);

/** Case-insensitive comparison. */
AMWAPI s32 AMWCALL str_cmp_ci(const struct str s1, const struct str s2);

/** Test if two strings are equal. */
attr_inline
b32 str_eq(const struct str s1, const struct str s2) {
    return str_cmp(s1, s2) == 0;
}

/** Case-insensitive match. */
attr_inline
b32 str_eq_ci(const struct str s1, const struct str s2) {
    return str_cmp_ci(s1, s2) == 0;
}

/** Test for prefix. */
AMWAPI b32 AMWCALL str_has_prefix(const struct str s, const struct str prefix);

/** Test for suffix. */
AMWAPI b32 AMWCALL str_has_suffix(const struct str s, const struct str suffix);

#ifdef __cplusplus
}
#endif
