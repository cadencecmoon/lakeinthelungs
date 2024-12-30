#include <lake/silver.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

AMWAPI int32_t AMWAPIENTRY 
silver_pipewire_entry_point(struct silver *silv, struct ipomoeaalba *ia) 
{ 
    (void)silv; (void)ia; 
    return result_success;
}
