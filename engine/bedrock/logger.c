#include <amwe/bedrock.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef BEDROCK_LOG_USE_COLOR
    #ifdef BEDROCK_LOG_DISABLE_COLOR
        #define BEDROCK_LOG_USE_COLOR 0
    #else
        #define BEDROCK_LOG_USE_COLOR 1
    #endif
#endif

static s32 g_log_level = BEDROCK_LOG_VERBOSE;
static bool g_quiet = 0;
static bool g_verbose = 0;
static atomic_spinlock g_log_spinlock = LAKE_ATOMIC_SPINLOCK_INIT;

static const char *level_strings[] = {
    "V",
    "D",
    "I",
    "W",
    "E",
    "F"
};

static const char *level_colors[] = {
#if BEDROCK_LOG_USE_COLOR
    "\033[38;5;8m",
    "\033[38;5;215m", 
    "\033[38;5;216m", 
    "\033[38;5;167m", 
    "\033[38;5;160m",
    "\033[38;5;68m",
#else
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
    "\x1b[0m",
#endif /* BEDROCK_LOG_USE_COLOR */
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

void bedrock_log_message(s32 level, const char *fn, const char *file, s32 line, const char *fmt, ...)
{
    if (g_quiet || (level < g_log_level || fmt == NULL))
        return;

    struct logdata log = {
        .fmt = fmt,
        .func = fn,
        .file = file,
        .line = line,
        .level = level,
    };

    va_start(log.ap, fmt);
    lake_atomic_spinlock_acquire(&g_log_spinlock);

    if (g_verbose) {
        time_t t = time(NULL);
        log.time = localtime(&t);

        char timestamp[22];
        timestamp[strftime(timestamp, lake_sizeof(timestamp), "%H:%M:%S", log.time)] = '\0';

        fprintf(stderr, "%s %s%s ", timestamp, level_colors[log.level], level_strings[log.level]);
        fprintf(stderr, "\033[38;5;8m%22s:%-6d%lx%s \x1b[0m", log.file, log.line, bedrock_thread_current(), level_colors[log.level]);
    } else {
        fprintf(stderr, "%s%s \x1b[0m", level_colors[log.level], level_strings[log.level]);
    }
    vfprintf(stderr, log.fmt, log.ap);
    fprintf(stderr, "\x1b[0m\n");
    fflush(stderr);

    lake_atomic_spinlock_release(&g_log_spinlock);
    va_end(log.ap);
}

void bedrock_log_set_level(s32 level)
{
    bedrock_assert_debug(level <= BEDROCK_LOG_FATAL && level >= 0);
    g_log_level = level;
}
void bedrock_log_set_verbose(bool set) { g_verbose = set; }
void bedrock_log_set_quiet(bool set) { g_quiet = set; }
