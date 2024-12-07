/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_PARSER_H
#define _AMW_PARSER_H

#include <amw/common/defines.h>

AMW_C_DECL_BEGIN

AMW_INLINE i32 parser_atoi(const char *c) {
    i32 value = 0;
    i32 sign = 1;
    if (*c == '+' || *c == '-') {
        if (*c == '-') sign = -1;
        c++; /* holy hell */
    }
    while (((*c) >= '0') && ((*c) <= '9')) {
        value *= 10;
        value += (i32)(*c - '0');
        c++; /* !! */
    }
    return value * sign;
}

AMW_C_DECL_END
#endif /* _AMW_PARSER_H */
