#ifdef PLATFORM_UNIX
#include <amw/bedrock/time.h>

#include <unistd.h>
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
#endif /* PLATFORM_APPLE */

static void check_monotonic(void)
{
#if defined(HAS_CLOCK_GETTIME)
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        has_monotonic = 1;
#elif defined(PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0)
        has_monotonic = 1;
#endif /* HAS_CLOCK_GETTIME */
    checked_monotonic = 0;
}

u64 rtc_counter(void)
{
    u64 ticks = 0;

    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= const_ns_per_sec;
        ticks += now.tv_nsec;
#elif defined(PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        UNREACHABLE;
#endif /* HAS_CLOCK_GETTIME */
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= const_us_per_sec;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 rtc_frequency(void)
{
    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAS_CLOCK_GETTIME
        return const_ns_per_sec;
#elif defined(PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= const_ns_per_sec;
        freq /= mach_base_info.numer;
        return freq;
#endif /* HAS_CLOCK_GETTIME */
    } 
    return const_us_per_sec;
}

#endif /* PLATFORM_UNIX */
