#include <amw/bedrock/log.h>
#include <amw/bedrock/time.h>

#include <stdlib.h>

/* how many past frame times are used to compute the median */
#define FRAME_TIME_COUNT (128)

/* a ring buffer of delta times, invalid entries are zero */
static f64 recorded_frame_times[FRAME_TIME_COUNT] = {0.0};
/* the most recently written entry in the ring buffer */
static u32 recorded_frame_time_idx = FRAME_TIME_COUNT - 1;
static f64 last_print_time = 0.0;

static u64 timer_offset = 0;

void record_frame_time(u64 time_now, f64 dt_frequency)
{
    if (!timer_offset)
        timer_offset = time_now;

    recorded_frame_time_idx++;
    if (recorded_frame_time_idx >= FRAME_TIME_COUNT)
        recorded_frame_time_idx -= FRAME_TIME_COUNT;
    recorded_frame_times[recorded_frame_time_idx] = (f64)(time_now - timer_offset) * dt_frequency;
}

static s32 compare_floats(const void *lhs_raw, const void *rhs_raw)
{
    f32 lhs = *((f32 *)lhs_raw);
    f32 rhs = *((f32 *)rhs_raw);
    return (lhs < rhs) ? -1 : ((lhs == rhs) ? 0 : 1);
}

f32 median_frame_time(void)
{
    f32 frame_times[FRAME_TIME_COUNT];
    u32 recorded_count = 0;
    for (s32 i = 0; i < FRAME_TIME_COUNT-1; i++) {
        s32 lhs = (recorded_frame_time_idx + FRAME_TIME_COUNT - i) % FRAME_TIME_COUNT;
        s32 rhs = (recorded_frame_time_idx + FRAME_TIME_COUNT - i - 1) % FRAME_TIME_COUNT;
        if (recorded_frame_times[lhs] != 0.0 && recorded_frame_times[rhs] != 0.0) {
            frame_times[recorded_count] = (f32)(recorded_frame_times[lhs] - recorded_frame_times[rhs]);
            recorded_count++;
        }
    }
    if (recorded_count == 0) return 0.0f;

    /* sort our frame times */
    qsort(frame_times, recorded_count, sizeof(frame_times[0]), compare_floats);
    /* calculate the median */
    return frame_times[recorded_count / 2];
}

void print_frame_time(f32 interval_ms)
{
    f64 current_time = recorded_frame_times[recorded_frame_time_idx];
    if (last_print_time == 0.0f || last_print_time + (f64)interval_ms < current_time) {
        f32 frame_time = median_frame_time();
        if (frame_time > 0.0f) {
            log_info("Frame time: %.3f ms (%.0f FPS)", frame_time, 1000/frame_time);
        }
        last_print_time = current_time;
    }
}
