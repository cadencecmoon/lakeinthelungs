#include "in_the_lungs.h"

static void lake_in_the_lungs(
    struct riven          *riven,
    thread_t              *threads,
    ssize                  thread_count,
    struct a_moonlit_walk *amw)
{
    s32 res, frame_idx = 0;
    u32 time_last = 0, time_now = bedrock_rtc_counter();
    f64 dt_frequency = 1000.0/(f64)bedrock_rtc_frequency();
    f64 dt = 0;

    amw->riven = riven;

    amw->tear.threads = threads;
    amw->tear.thread_count = thread_count;
}

static s32 a_moonlit_walk_main__(s32 argc, char **argv)
{
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
