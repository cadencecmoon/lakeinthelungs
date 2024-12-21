#ifndef _AMW_TIME_H
#define _AMW_TIME_H

#include <lake/defines.h>

#define AMW_MS_PER_SECOND       1000
#define AMW_US_PER_SECOND       1000000
#define AMW_NS_PER_SECOND       1000000000LL
#define AMW_NS_PER_MS           1000000
#define AMW_NS_PER_US           1000
#define AMW_SECONDS_TO_NS(S)    (((uint64_t)(S)) * AMW_NS_PER_SECOND)
#define AMW_NS_TO_SECONDS(NS)   ((NS) / AMW_NS_PER_SECOND)
#define AMW_MS_TO_NS(MS)        (((uint64_t)(MS)) * AMW_NS_PER_MS)
#define AMW_NS_TO_MS(NS)        ((NS) / AMW_NS_PER_MS)
#define AMW_US_TO_NS(US)        (((uint64_t)(US)) * AMW_NS_PER_US)
#define AMW_NS_TO_US(NS)        ((NS) / AMW_NS_PER_US)

/** Get the time passed since the game initialized in milliseconds. */
AMWAPI uint64_t ticks_ms(void);

/** Get the time passed since the game initialized in nanoseconds. */
AMWAPI uint64_t ticks_ns(void);

#endif /* _AMW_TIME_H */
