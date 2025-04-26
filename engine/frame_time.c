#include <amwe/bedrock.h>

/* how many past frame times are used to compute the median */
#define FRAME_TIME_COUNT (128)

/* a ring buffer of delta times, invalid entries are zero */
static f64 g_recorded_frame_times[FRAME_TIME_COUNT] = {0.0};
/* the most recently written entry in the ring buffer */
static u32 g_recorded_frame_time_idx = FRAME_TIME_COUNT - 1;
static f64 g_last_print_time = 0.0;

void bedrock_frame_time_record(u64 time_app_start, u64 time_now, f64 dt_frequency_reciprocal)
{
    g_recorded_frame_time_idx++;
    if (g_recorded_frame_time_idx >= FRAME_TIME_COUNT)
        g_recorded_frame_time_idx -= FRAME_TIME_COUNT;
    g_recorded_frame_times[g_recorded_frame_time_idx] = ((f64)(time_now - time_app_start) * dt_frequency_reciprocal);
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
