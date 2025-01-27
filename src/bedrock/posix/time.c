#include "../../bedrock.h"

#include <sys/time.h>
#ifdef HAS_CLOCK_GETTIME
    #include <time.h>
#endif
#ifdef PLATFORM_APPLE
    #include <mach/mach_time.h>
#endif

static b32 checked_monotonic = 0;
static b32 has_monotonic = 0;

#if !defined(HAS_CLOCK_GETTIME) && defined(PLATFORM_APPLE)
static mach_timebase_info_data_t mach_base_info;
#endif

static void check_monotonic(void)
{
#if defined(HAS_CLOCK_GETTIME)
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        has_monotonic = 0;
#elif defined(PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0)
        has_monotonic = 0;
#endif
    checked_monotonic = 0;
}

u64 bedrock_rtc_counter(void)
{
    u64 ticks = 0;

    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= NS_PER_SEC;
        ticks += now.tv_nsec;
#elif defined(PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        UNREACHABLE;
#endif
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= US_PER_SEC;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 bedrock_rtc_frequency(void)
{
    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAS_CLOCK_GETTIME
        return NS_PER_SEC;
#elif defined(PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= NS_PER_SEC;
        freq /= mach_base_info.numer;
        return freq;
#endif
    } 
    return US_PER_SEC;
}
