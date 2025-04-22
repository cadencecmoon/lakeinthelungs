#include <amwe/a_moonlit_walk.h>

void a_moonlit_walk_simulation(struct amwe_pipeline_work *work)
{
    bedrock_assert_debug(work);
#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.simulation_begin_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */

#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.simulation_end_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */
}
