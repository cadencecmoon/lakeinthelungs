#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>

static u64 timer_frequency = 0;

u64 bedrock_rtc_counter(void)
{
    if (lake_unlikely(!timer_frequency))
        QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frequency);

    u64 counter;
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
    return counter;
}

u64 bedrock_rtc_frequency(void)
{
    if (lake_unlikely(!timer_frequency))
        QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frequency);
    return timer_frequency;
}
#endif /* LAKE_PLATFORM_WINDOWS */
