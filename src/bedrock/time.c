#include <amwe/bedrock.h>

#ifdef LAKE_PLATFORM_UNIX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/time.h>

#ifdef LAKE_HAS_CLOCK_GETTIME
    #include <time.h>
#endif
#ifdef LAKE_PLATFORM_APPLE
    #include <mach/mach_time.h>
#endif

static bool checked_monotonic = 0;
static bool has_monotonic = 0;

static void check_monotonic(void)
{
#if defined(LAKE_HAS_CLOCK_GETTIME)
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0)
        has_monotonic = 1;
#elif defined(LAKE_PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0)
        has_monotonic = 1;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    checked_monotonic = 0;
}

u64 bedrock_rtc_counter(void)
{
    u64 ticks = 0;

    if (lake_unlikely(!checked_monotonic))
        check_monotonic();

    if (lake_likely(has_monotonic)) {
#ifdef LAKE_HAS_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= LAKE_NS_PER_SECOND;
        ticks += now.tv_nsec;
#elif defined(LAKE_PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        lake_unreachable;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    } else {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= LAKE_US_PER_SECOND;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 bedrock_rtc_frequency(void)
{
    if (lake_unlikely(!checked_monotonic))
        check_monotonic();

    if (lake_likely(has_monotonic)) {
#ifdef LAKE_HAS_CLOCK_GETTIME
        return LAKE_NS_PER_SECOND;
#elif defined(LAKE_PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= LAKE_NS_PER_SECOND;
        freq /= mach_base_info.numer;
        return freq;
#endif /* LAKE_HAS_CLOCK_GETTIME */
    } 
    return LAKE_US_PER_SECOND;
}

/* LAKE_PLATFORM_UNIX */
#elif defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/arch/windows.h>

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

/* how many past frame times are used to compute the median */
#define FRAME_TIME_COUNT (128)

/* a ring buffer of delta times, invalid entries are zero */
static f64 g_recorded_frame_times[FRAME_TIME_COUNT] = {0.0};
/* the most recently written entry in the ring buffer */
static u32 g_recorded_frame_time_idx = FRAME_TIME_COUNT - 1;
static f64 g_last_print_time = 0.0;

static u64 g_timer_offset = 0;

void bedrock_frame_time_record(u64 time_now, f64 dt_frequency)
{
    if (!g_timer_offset)
        g_timer_offset = time_now;

    g_recorded_frame_time_idx++;
    if (g_recorded_frame_time_idx >= FRAME_TIME_COUNT)
        g_recorded_frame_time_idx -= FRAME_TIME_COUNT;
    g_recorded_frame_times[g_recorded_frame_time_idx] = (f64)(time_now - g_timer_offset) * dt_frequency;
}

static s32 compare_floats(const void *lhs_raw, const void *rhs_raw)
{
    f32 lhs = *((f32 *)lhs_raw);
    f32 rhs = *((f32 *)rhs_raw);
    return (lhs < rhs) ? -1 : ((lhs == rhs) ? 0 : 1);
}

f32 bedrock_frame_time_median(void)
{
    f32 frame_times[FRAME_TIME_COUNT];
    u32 recorded_count = 0;
    for (s32 i = 0; i < FRAME_TIME_COUNT-1; i++) {
        s32 lhs = (g_recorded_frame_time_idx + FRAME_TIME_COUNT - i) % FRAME_TIME_COUNT;
        s32 rhs = (g_recorded_frame_time_idx + FRAME_TIME_COUNT - i - 1) % FRAME_TIME_COUNT;
        if (g_recorded_frame_times[lhs] != 0.0 && g_recorded_frame_times[rhs] != 0.0) {
            frame_times[recorded_count] = (f32)(g_recorded_frame_times[lhs] - g_recorded_frame_times[rhs]);
            recorded_count++;
        }
    }
    if (recorded_count == 0) return 0.0f;

    /* sort our frame times */
    qsort(frame_times, recorded_count, lake_sizeof(frame_times[0]), compare_floats);
    /* calculate the median */
    return frame_times[recorded_count / 2];
}

void bedrock_frame_time_print(f32 interval_ms)
{
    f64 current_time = g_recorded_frame_times[g_recorded_frame_time_idx];
    if (g_last_print_time == 0.0f || g_last_print_time + (f64)interval_ms < current_time) {
        f32 frame_time = bedrock_frame_time_median();
        if (frame_time > 0.0f) {
            bedrock_log_verbose("Frame time: %.3f ms (%.0f FPS)", frame_time, 1000/frame_time);
        }
        g_last_print_time = current_time;
    }
}
