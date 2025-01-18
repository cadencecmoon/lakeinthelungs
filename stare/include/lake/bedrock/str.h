/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_STRINGS_H
#define _AMW_STRINGS_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define string(s) (struct string){(struct string *)s, lengthof(s)}
struct string {
    u8   *data;
    ssize len;
};

struct strlist {
    struct strlist *next;
    struct string   item;
};

struct strpair {
    struct string head;
    struct string tail;
};

// TODO

#ifdef __cplusplus
}
#endif

#endif /* _AMW_STRINGS_H */
