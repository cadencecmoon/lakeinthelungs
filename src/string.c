#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE /* for strncasecmp() */
#endif

#include <amw/string.h>

void str_swap(struct str * const s1, struct str * const s2)
{
    const struct str temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

s32 str_cmp(const struct str s1, const struct str s2)
{
    const usize n1 = str_len(s1), n2 = str_len(s2);
    /* either string may be missing a null terminator, hence "memcmp" */
    const s32 res = memcmp(str_ptr(s1), str_ptr(s2), (n1 < n2) ? n1 : n2);

    if (res != 0 || n1 == n2)
        return res;

    return (n1 < n2) ? -1 : 1;
}

s32 str_cmp_ci(const struct str s1, const struct str s2)
{
    const usize n1 = str_len(s1), n2 = str_len(s2);
    /* either string may be missing a null terminator, hence "strNcasecmp" */
    const s32 res = strncasecmp(str_ptr(s1), str_ptr(s2), (n1 < n2) ? n1 : n2);
    
    if (res != 0 || n1 == n2)
        return res;

    return (n1 < n2) ? -1 : 1;
}

b32 str_has_prefix(const struct str s, const struct str prefix)
{
    const usize n = str_len(prefix);

    return (n == 0) || (str_len(s) >= n && memcmp(str_ptr(s), str_ptr(prefix), n) == 0);
}

b32 str_has_suffix(const struct str s, const struct str suffix)
{
    const usize n = str_len(suffix);

    return (n == 0) || (str_len(s) >= n && memcmp(str_end(s) - n, str_ptr(suffix), n) == 0);
}
