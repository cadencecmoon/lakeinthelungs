#pragma once

#include <amw/bedrock/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define const_ms_per_sec    1000
#define const_us_per_sec    1000000
#define const_ns_per_sec    1000000000LL
#define const_ns_per_ms     1000000
#define const_ns_per_us     1000
#define sec_to_ns(s)    (((u64)(s)) * const_ns_per_sec)
#define ns_to_sec(ns)   ((NS) / const_ns_per_sec)
#define ms_to_ns(ms)    (((u64)(ms)) * const_ns_per_ms)
#define ns_to_ms(ns)    ((NS) / const_ns_per_ms)
#define us_to_ns(us)    (((u64)(us)) * const_ns_per_us)
#define ns_to_us(ns)    ((NS) / const_ns_per_us)

/** Returns the counter of the real-time clock. */
AMWAPI u64 AMWCALL rtc_counter(void);

/** Returns the frequency of the real-time clock. */
AMWAPI u64 AMWCALL rtc_frequency(void);

/** Invoke this function exactly once per frame to record the current frame time.
 *  Only when the other functions defined in this header will be available. */
AMWAPI void AMWCALL record_frame_time(u64 time_now, f64 dt_frequency);

/** Retrieves the current estimate of the frame time in seconds. It is the median 
 *  of a certain number of previously recorded frame times. */
AMWAPI f32 AMWCALL median_frame_time(void);

/** Prints the current estimate of the total frame time periodically, namely once 
 *  per given time interval, assuming this function is called once per frame. */
AMWAPI void AMWCALL print_frame_time(f32 interval_ms);

#ifdef __cplusplus
}
#endif
