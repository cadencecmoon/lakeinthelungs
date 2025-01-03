#include <lake/bedrock/time.h>
#include <lake/bedrock/os.h>

#include <sys/time.h>
#ifdef AMW_HAS_CLOCK_GETTIME
#include <time.h>
#endif

#ifdef AMW_PLATFORM_APPLE
#include <mach/mach_time.h>
#endif

static b32 checked_monotonic = false;
static b32 has_monotonic = false;

#if !defined(AMW_HAS_CLOCK_GETTIME) && defined(AMW_PLATFORM_APPLE)
static mach_timebase_info_data_t mach_base_info;
#endif

static void check_monotonic(void)
{
#if defined(AMW_HAS_CLOCK_GETTIME)
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        has_monotonic = true;
#elif defined(AMW_PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0)
        has_monotonic = true;
#endif
    checked_monotonic = true;
}

AMWAPI u64 bedrock_rtc_counter(void)
{
    u64 ticks = 0;

    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef AMW_HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= AMW_NS_PER_SECOND;
        ticks += now.tv_nsec;
#elif defined(AMW_PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        AMW_UNREACHABLE;
#endif
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= AMW_US_PER_SECOND;
        ticks += now.tv_usec;
    }
    return ticks;
}

AMWAPI u64 bedrock_rtc_frequency(void)
{
    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef AMW_HAS_CLOCK_GETTIME
        return AMW_NS_PER_SECOND;
#elif defined(AMW_PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= AMW_NS_PER_SECOND;
        freq /= mach_base_info.numer;
        return freq;
#endif
    } 
    return AMW_US_PER_SECOND;
}
