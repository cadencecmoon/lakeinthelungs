/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_TIME_H
#define _AMW_TIME_H

#include <lake/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AMW_MS_PER_SECOND       1000
#define AMW_US_PER_SECOND       1000000
#define AMW_NS_PER_SECOND       1000000000LL
#define AMW_NS_PER_MS           1000000
#define AMW_NS_PER_US           1000
#define AMW_SECONDS_TO_NS(S)    (((u64)(S)) * AMW_NS_PER_SECOND)
#define AMW_NS_TO_SECONDS(NS)   ((NS) / AMW_NS_PER_SECOND)
#define AMW_MS_TO_NS(MS)        (((u64)(MS)) * AMW_NS_PER_MS)
#define AMW_NS_TO_MS(NS)        ((NS) / AMW_NS_PER_MS)
#define AMW_US_TO_NS(US)        (((u64)(US)) * AMW_NS_PER_US)
#define AMW_NS_TO_US(NS)        ((NS) / AMW_NS_PER_US)

/** Initialize the game clock. */
AMWAPI void ticks_init(void);

/** Get the time passed since the game initialized in milliseconds. */
AMWAPI u64 ticks_ms(void);

/** Get the time passed since the game initialized in nanoseconds. */
AMWAPI u64 ticks_ns(void);

#ifdef __cplusplus
}
#endif

#endif /* _AMW_TIME_H */
