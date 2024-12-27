#include <lake/bedrock/atomic.h>
#include <lake/bedrock/log.h>

#include <stdio.h>
#include <time.h>

static at_int32_t level = log_level_debug;
static at_bool_t quiet = false;
static at_bool_t verbose = false;
static at_flag_t spinlock = AMW_ATOMIC_FLAG_INIT;

static const char *level_strings[] = {
    "D",
    "I",
    "W",
    "E",
    "F"
};

static const char *level_colors[] = {
#if AMW_LOG_DISABLE_COLOR
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
#else
    "\033[38;5;215m",
    "\033[38;5;209m", 
    "\033[38;5;167m", 
    "\033[38;5;160m",
    "\033[38;5;68m",
#endif
};

struct logdata {
    va_list     ap;
    const char *fmt;
    const char *file;
    struct tm  *time;
    int32_t     line;
    int32_t     level;
};

void log_message(
        enum log_level level, 
        const char *filename, 
        int32_t line, 
        const char *fmt, ...)
{
    if (at_read_relaxed(&quiet) || (level < at_read_relaxed(&level)))
        return;

    assert_release(fmt != NULL);

    struct logdata log = {
        .fmt = fmt,
        .file = filename,
        .line = line,
        .level = level,
    };

    va_start(log.ap, fmt);
    do {} while (at_test_and_set(&spinlock));

    if (at_read_relaxed(&verbose)) {
        time_t t = time(NULL);
        log.time = localtime(&t);

        char timestamp[22];
        timestamp[strftime(timestamp, sizeof(timestamp), "%H:%M:%S", log.time)] = '\0';

        fprintf(stdout, "%s %s%s ", timestamp, level_colors[log.level], level_strings[log.level]);
        fprintf(stdout, "\033[38;5;8m%26s:%-5d%s ", log.file, log.line, level_colors[log.level]);
    } else {
        fprintf(stdout, "%s ", level_colors[log.level]);
    }
    vfprintf(stdout, log.fmt, log.ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    at_clear(&spinlock);
    va_end(log.ap);
}

void log_set_level(enum log_level set_level)
{
    assert_debug(set_level <= log_level_fatal && set_level >= 0);
    at_store_explicit(&level, set_level, memory_model_acq_rel);
}

void log_set_verbose(bool set_verbose)
{
    at_store_explicit(&verbose, set_verbose, memory_model_acq_rel);
}

void log_set_quiet(bool set_quiet)
{
    at_store_explicit(&quiet, set_quiet, memory_model_acq_rel);
}
