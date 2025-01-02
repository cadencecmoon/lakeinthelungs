#ifndef _AMW_STRINGS_H
#define _AMW_STRINGS_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define str(s) (str){(str *)s, lengthof(s)}
typedef struct str {
    u8   *data;
    ssize len;
} str;

typedef struct strlist {
    struct strlist *next;
    struct str      item;
} strlist;

typedef struct strpair {
    str head;
    str tail;
} strpair;

// TODO

#ifdef __cplusplus
}
#endif

#endif /* _AMW_STRINGS_H */
