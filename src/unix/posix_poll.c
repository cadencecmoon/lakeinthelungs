#ifdef PLATFORM_UNIX
#include <amw/bedrock/time.h>
#include <amw/hadal.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <poll.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

extern b32 _hadal_posix_poll(struct pollfd *fds, nfds_t count, f64 *timeout)
{
    for (;;) {
        if (timeout) {
            const u64 base = rtc_counter();
#if defined(PLATFORM_LINUX) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__CYGWIN__)
            const time_t seconds = (time_t)*timeout;
            const long nanoseconds = (long)((*timeout - seconds) * 1e9);
            const struct timespec ts = { seconds, nanoseconds };
            const s32 result = ppoll(fds, count, &ts, NULL);
#elif defined(__NetBSD__)
            const time_t seconds = (time_t)*timeout;
            const long nanoseconds = (long)((*timeout - seconds) * 1e9);
            const struct timespec ts = { seconds, nanoseconds };
            const s32 result = pollts(fds, count, &ts, NULL);
#else
            const s32 milliseconds = (s32)(*timeout * 1e3);
            const s32 result = poll(fds, count, milliseconds);
#endif
            const s32 error = errno; /* clock_gettime may overwrite an error */

            *timeout -= (rtc_counter() - base) / (f64)rtc_frequency();

            if (result > 0)
                return true;
            else if (result == -1 && error != EINTR && error != EAGAIN)
                return false;
            else if (*timeout <= 0.0)
                return false;
        } else {
            const s32 result = poll(fds, count, -1);
            if (result > 0)
                return true;
            else if (result == -1 && errno != EINTR && errno != EAGAIN)
                return false;
        }
    }
}

#endif /* PLATFORM_UNIX */
