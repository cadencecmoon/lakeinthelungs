#ifndef _AMW_PARSER_H
#define _AMW_PARSER_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO find a cool name for the parser module i guess */

AMW_INLINE int32_t parser_atoi(const char *c) {
    int32_t value = 0;
    int32_t sign = 1;
    if (*c == '+' || *c == '-') {
        if (*c == '-') sign = -1;
        c++; /* holy hell */
    }
    while (((*c) >= '0') && ((*c) <= '9')) {
        value *= 10;
        value += (int32_t)(*c - '0');
        c++; /* !! */
    }
    return value * sign;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PARSER_H */
