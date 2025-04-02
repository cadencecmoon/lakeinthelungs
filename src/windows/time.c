#include <amw/bedrock/time.h>

#include "win32_hadal.h"

static u64 timer_frequency = 0;

u64 rtc_counter(void)
{
    if (UNLIKELY(!timer_frequency))
        QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frequency);

    u64 counter;
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
    return counter;
}

u64 rtc_frequency(void)
{
    if (UNLIKELY(!timer_frequency))
        QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frequency);
    return timer_frequency;
}
