#ifndef _AMW_IPOMOEA_H
#define _AMW_IPOMOEA_H

#include <lake/defines.h>

/** Sets bits of a memory region of size n as c. */
AMWAPI void *iamemset(void *dest, int32_t c, size_t n);

#define iazero(mem)  iamemset(&(mem), 0, sizeof((mem)))
#define iazerop(mem) iamemset((mem), 0, sizeof(*(mem)))
#define iazeroa(mem) iamemset((mem), 0, sizeof((mem)))

/* TODO tagged heap memory allocator */

#endif /* _AMW_IPOMOEA_H */
