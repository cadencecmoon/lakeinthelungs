#include <amwe/a_moonlit_walk.h>

void a_moonlit_walk_gpuexec(struct amwe_pipeline_work *work)
{
    bedrock_assert_debug(work);
#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.gpuexec_begin_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */

#ifdef RIVEN_ENABLE_PROFILER
    work->profiling.gpuexec_end_time = bedrock_rtc_counter();
#endif /* RIVEN_ENABLE_PROFILER */
}
