#pragma once

#include <amw/bedrock.h>

struct str {
    char *ptr;
    usize len;
};

#define str_init(cstr) (struct str){ .ptr = cstr, .len = sizeof(cstr)-1 }
#define str_init_null  (struct str){ .ptr = NULL, .len = 0 }
