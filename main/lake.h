#ifndef _LAKE_IN_THE_LUNGS_H
#define _LAKE_IN_THE_LUNGS_H

#include <lake/a_moonlit_walk.h>

extern int32_t  lake_in_the_lungs_init__(struct a_moonlit_walk *AMW, struct amw_workload *work, uint32_t workload_count, void *context);
extern int32_t  lake_in_the_lungs_simulation__(struct a_moonlit_walk *AMW, struct amw_workload *work, void *context);
extern int32_t  lake_in_the_lungs_rendering__(struct a_moonlit_walk *AMW, struct amw_workload *frame, void *context);
extern int32_t  lake_in_the_lungs_gpuexec__(struct a_moonlit_walk *AMW, struct amw_workload *frame, void *context);
extern void     lake_in_the_lungs_cleanup__(struct a_moonlit_walk *AMW, void *context);

#endif /* _LAKE_IN_THE_LUNGS_H */
