#include <lake/silver.h>

AMWAPI int32_t AMWAPIENTRY 
silver_dummy_entry_point(struct silver *silv, struct ipomoeaalba *ia)
{
    (void)silv; (void)ia;
    return result_success;
}
