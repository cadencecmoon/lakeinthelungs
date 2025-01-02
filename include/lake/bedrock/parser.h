#ifndef _AMW_PARSER_H
#define _AMW_PARSER_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

AMW_INLINE s32 parser_atoi(const char *c) {
    s32 value = 0;
    s32 sign = 1;
    if (*c == '+' || *c == '-') {
        if (*c == '-') sign = -1;
        c++; /* holy hell */
    }
    while (((*c) >= '0') && ((*c) <= '9')) {
        value *= 10;
        value += (s32)(*c - '0');
        c++; /* !! */
    }
    return value * sign;
}

/* TODO adapt to windows */
AMW_INLINE ssize parser_strnlen(const char *s, ssize n) {
    const char *p = (char *)__builtin_memchr(s, 0, n);
    return p ? (ssize)(p - s) : n;
}

#ifdef __cplusplus
}
#endif

#endif /* _AMW_PARSER_H */
