#pragma once

#include <amw/bedrock.h>

struct string {
    char *ptr;
    usize len;
};

#define string_init(cstr) (struct string){ .ptr = cstr, .len = sizeof(cstr)-1 }
#define string_init_null  (struct string){ .ptr = NULL, .len = 0 }
