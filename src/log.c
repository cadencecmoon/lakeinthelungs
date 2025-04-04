#include <amw/bedrock/log.h>
#include <amw/bedrock/threads.h>

#include <stdio.h>
#include <time.h>

static s32 log_level = log_level_verbose;
static b32 quiet = 0;
static b32 verbose = 0;
static atomic_flag spinlock = ATOMIC_FLAG_INIT;

static const char *level_strings[] = {
    "V",
    "D",
    "I",
    "W",
    "E",
    "F"
};

static const char *level_colors[] = {
#if LOG_DISABLE_COLOR
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
#else
    "\033[38;5;8m",
    "\033[38;5;215m", 
    "\033[38;5;216m", 
    "\033[38;5;167m", 
    "\033[38;5;160m",
    "\033[38;5;68m",
#endif
};

struct logdata {
    va_list     ap;
    const char *fmt;
    const char *func;
    const char *file;
    struct tm  *time;
    s32         line;
    s32         level;
};

void log_message(
    s32         level,
    const char *func,
    const char *filename,
    s32         line,
    const char *fmt, ...)
{
    if (quiet || (level < log_level || fmt == NULL))
        return;

    struct logdata log = {
        .fmt = fmt,
        .func = func,
        .file = filename,
        .line = line,
        .level = level,
    };

    va_start(log.ap, fmt);
    do {} while (atomic_flag_test_and_set_explicit(&spinlock, memory_order_acquire));

    if (verbose) {
        time_t t = time(NULL);
        log.time = localtime(&t);

        char timestamp[22];
        timestamp[strftime(timestamp, sizeof(timestamp), "%H:%M:%S", log.time)] = '\0';

        fprintf(stderr, "%s %s%s ", timestamp, level_colors[log.level], level_strings[log.level]);
        fprintf(stderr, "\033[38;5;8m%22s:%-6d%lx%s \x1b[0m", log.file, log.line, thread_current(), level_colors[log.level]);
    } else {
        fprintf(stderr, "%s%s \x1b[0m", level_colors[log.level], level_strings[log.level]);
    }
    vfprintf(stderr, log.fmt, log.ap);
    fprintf(stderr, "\x1b[0m\n");
    fflush(stderr);

    atomic_flag_clear_explicit(&spinlock, memory_order_release);
    va_end(log.ap);
}

void log_set_level(s32 set_log_level)
{
    if (set_log_level > log_level_fatal && set_log_level < 0) return;
    log_level = set_log_level;
}

void log_set_verbose(b32 set_verbose)
{
    verbose = set_verbose;
}

void log_set_quiet(b32 set_quiet)
{
    quiet = set_quiet;
}
