#include "in_the_lungs.h"

struct amw_hints {
    const char *amw_name;
    const char *lake_name;
    u32 version;
    u32 window_width;
    u32 window_height;
    const char *window_title;
    ssize riven_thread_count;
    ssize riven_fiber_count;
    ssize riven_stack_size;
    ssize riven_log2_tears;
    PFN_hadal_entry_point hadal_entry;
    PFN_hadal_entry_point hadal_fallback;
    struct a_moonlit_walk amw;
};

static void lake_in_the_lungs(
    struct riven     *riven,
    thread_t         *threads,
    ssize             thread_count,
    struct amw_hints *hints)
{
    s32 res, frame_idx = 0;
    u32 time_last = 0, time_now = bedrock_rtc_counter();
    f64 dt_frequency = 1000.0/(f64)bedrock_rtc_frequency();
    f64 dt = 0;

    struct a_moonlit_walk *amw = &hints->amw;

    amw->riven = riven;
    thinitlate(&amw->th, riven, threads, thread_count);
}

static b32 parse_command_line(struct amw_hints *hints, s32 argc, char **argv)
{
    (void)hints; (void)argc; (void)argv;
    return 1;
}

static s32 a_moonlit_walk_main__(s32 argc, char **argv)
{
    log_set_verbose(0);
    bedrock_apptimer_init();

    struct amw_hints hints = {
        .amw_name = "A Moonlit Walk Engine",
        .lake_name = "Lake in the Lungs",
        .version = LAKE_VERSION,
        .window_width = 1200,
        .window_height = 900,
        .window_title = hints.lake_name,
        .riven_thread_count = 0,
        .riven_fiber_count = 128,
        .riven_stack_size = 64 * 1024,
        .riven_log2_tears = 12,
        .hadal_entry = hadal_entry_point,
        .hadal_fallback = hadal_headless_entry_point,
        .amw = {0},
    };
    bedrock_cpuinfo(&hints.riven_thread_count, NULL, NULL);

    /* Unless a non-zero value is returned, we may not continue to the main gameloop. */
    if (!parse_command_line(&hints, argc, argv)) return result_success;

    /* estimate the core memory requirements */
    usize riven_bytes = riven_unveil_rift(
        NULL,                               /* Null to get the required memory buffer size. */
        hints.riven_stack_size,             /* Size of stack per fiber in bytes. */
        hints.riven_fiber_count,            /* How many fibers to instance. */
        hints.riven_log2_tears,             /* Job queue will be (1u << log2_tears) entries large. */
        hints.riven_thread_count,           /* How many CPU threads to run, including the main thread. */
        (PFN_riven_main)lake_in_the_lungs,  /* Main riven tear, serves as the entry point. */
        &hints);                            /* The engine context as the argument. */
    struct tagged_heap_allocator *th = &hints.amw.th;

    log_debug("Constructing Riven for a parallel workflow:");
    log_debug("      memory buffer - %lu bytes (%luKiB)", riven_bytes, riven_bytes/1024);
    log_debug("    stack per fiber - %lu bytes (%luKiB)", hints.riven_stack_size, hints.riven_stack_size/1024);
    log_debug("    tear queue size - %lu tears, %lu log2", (1lu << hints.riven_log2_tears), hints.riven_log2_tears);
    log_debug("        fiber count - %lu fibers", hints.riven_fiber_count);
    log_debug("       thread count - %lu threads", hints.riven_thread_count);

    /* riven is internally aligned to 16 bytes */
    void *riven_memory = throots(th, riven_bytes, 16);

    riven_unveil_rift(
        riven_memory,
        hints.riven_stack_size,
        hints.riven_fiber_count,
        hints.riven_log2_tears,
        hints.riven_thread_count,
        (PFN_riven_main)lake_in_the_lungs,
        &hints);
    thfini(th);

    return 0;
}

#if defined(PLATFORM_WINDOWS)
#include "bedrock/windows/windows.h"
#include <wchar.h>
#include <stdlib.h>

static char **command_line_to_utf8_argv(
    LPWSTR w_command_line, 
    s32   *o_argc)
{
    s32    argc = 0;
    char **argv = 0;
    char  *args;

    LPWSTR *w_argv = CommandLineToArgvW(w_command_line, &argc);
    if (w_argv == NULL) {
        log_error("Win32 couldn't fetch command line arguments");
    } else {
        usize size = wcslen(w_command_line) * 4;
        void *ptr_argv = malloc(((usize)argc + 1) * sizeof(char *) + size);
        zerop(ptr_argv);
        argv = (char **)ptr_argv;
        assert_release(argv);
        args = (char *)&argv[argc + 1];

        s32 n;
        for (s32 i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (s32)size, NULL, NULL);
            if (n == 0) {
                log_error("Win32 got a 0 length argument");
                break;
            }
            argv[i] = args;
            size -= (usize)n;
            args += n;
        }
        LocalFree(w_argv);
    }
    *o_argc = argc;
    return argv;
}

int WINAPI WinMain(
    _In_ HINSTANCE     hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPSTR         lpCmdLine, 
    _In_ int           nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    s32 res, argc_utf8 = 0;
    char **argv_utf8 = command_line_to_utf8_argv(GetCommandLineW(), &argc_utf8);

    res = a_moonlit_walk_main__(argc_utf8, argv_utf8);
    free(argv_utf8);
    return res;
}

#elif defined(PLATFORM_MACOS)
#elif defined(PLATFORM_IOS)
#elif defined(PLATFORM_ANDROID)

#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

JNIEXPORT void ANativeActivity_onCreate(
    ANativeActivity *activity, 
    void            *saved_state, 
    usize            saved_state_size) 
{
    (void)activity;
    (void)saved_state;
    (void)saved_state_size;
}

#else /* LINUX / UNIX */
s32 main(s32 argc, char **argv) 
{
    return a_moonlit_walk_main__(argc, argv);
}
#endif
