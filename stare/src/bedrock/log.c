/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/assert.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/log.h>

#include <stdio.h>
#include <time.h>

static at_s32 level = log_level_trace;
static at_b32 quiet = false;
static at_b32 verbose = false;
static at_flag spinlock = AMW_ATOMIC_FLAG_INIT;

static const char *level_strings[] = {
    "T",
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
    "\x1b[0m",
#else
    "\033[38;5;8m", 
    "\033[38;5;215m", //"\033[38;5;209m", 
    "\x1b[0m",
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

AMWAPI void log_message(s32 level, const char *filename, s32 line, const char *fmt, ...)
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

        fprintf(stderr, "%s %s%s ", timestamp, level_colors[log.level], level_strings[log.level]);
        fprintf(stderr, "\033[38;5;8m%26s:%-5d%s ", log.file, log.line, level_colors[log.level]);
    } else {
        fprintf(stderr, "%s", level_colors[log.level]);
    }
    vfprintf(stderr, log.fmt, log.ap);
    fprintf(stderr, "\x1b[0m\n");
    fflush(stderr);

    at_clear(&spinlock);
    va_end(log.ap);
}

AMWAPI void log_set_level(s32 set_log_level)
{
    assert_debug(set_log_level <= log_level_fatal && set_log_level >= 0);
    at_store_explicit(&level, set_log_level, memory_model_acq_rel);
}

AMWAPI void log_set_verbose(b32 set_verbose)
{
    at_store_explicit(&verbose, set_verbose, memory_model_acq_rel);
}

AMWAPI void log_set_quiet(b32 set_quiet)
{
    at_store_explicit(&quiet, set_quiet, memory_model_acq_rel);
}
