#ifndef _A_MOONLIT_WALK_H
#define _A_MOONLIT_WALK_H

#include <lake/bedrock/defines.h>

#include <lake/hadal.h>         /* windowing system */
#include <lake/ipomoea.h>       /* memory, tagged heap allocator */
#include <lake/moth.h>          /* audio engine */
#include <lake/platynova.h>     /* renderer */
#include <lake/riven.h>         /* fiber-based job system */

#include <lake/bedrock/align.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/endian.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/parser.h>
#include <lake/bedrock/time.h>

#include <lake/compute/math_types.h>
#include <lake/compute/simd.h>

#include <lake/datastructures/arena_allocator.h>

#include <lake/input/gamepad.h>
#include <lake/input/joystick.h>
#include <lake/input/keyboard.h>
#include <lake/input/mouse.h>
#include <lake/input/pen.h>
#include <lake/input/touch.h>

#ifdef __cplusplus
extern "C" {
#endif

struct a_moonlit_walk; /* forward declaration */

/** Holds per-frame memory, pointer to the engine context and data needed to calculate and present a frame. */
struct amw_framedata {
    uint32_t idx;
    double   dt;

    struct a_moonlit_walk *AMW;
    const struct amw_framedata *previous;
};

/** Application defined procedures. It's guaranteed that a single mainloop stage will not run 
 *  their callback in parallel with other internal procedures, needing no special synchronization
 *  work from the application's code. The individual stages still run in parallel, working on 
 *  different frames, unless the parallel gameloop execution has been disabled. */
typedef int32_t (*PFN_amw_init)(struct a_moonlit_walk *AMW, struct amw_framedata *frames, uint32_t frame_count, void *context);
typedef int32_t (*PFN_amw_simulation)(struct a_moonlit_walk *AMW, struct amw_framedata *frame, void *context);
typedef int32_t (*PFN_amw_rendering)(struct a_moonlit_walk *AMW, struct amw_framedata *frame, void *context);
typedef int32_t (*PFN_amw_gpuexec)(struct a_moonlit_walk *AMW, struct amw_framedata *frame, void *context);
typedef void    (*PFN_amw_cleanup)(struct a_moonlit_walk *AMW, void *context);

struct amw_callbacks {
    PFN_amw_init        init;
    PFN_amw_simulation  simulation;
    PFN_amw_rendering   rendering;
    PFN_amw_gpuexec     gpuexec;
    PFN_amw_cleanup     cleanup;
};

/** Data used for configuring the framework from the application space. */
struct amw_hints {
    const char *app_name;
    uint32_t    version;

    struct {
        uint32_t    width, height;
        const char *title;
    } window;

    struct {
        uint32_t thread_count;
        uint32_t fiber_count;
        uint32_t log_2_tears;
        uint32_t stack_size;
    } riven;

    struct {
        uint32_t hadal_backend;
        uint32_t moth_backend;

        bool allow_headless_display;
    } init;

    struct amw_callbacks callbacks;
};

/** Used to control the framework's gameloop. */
enum amw_flag {
    //amw_flag_parallel_gameloop_execution    = (1u << 0),  /**< Controls if the gameloop stages (simulation, rendering, gpuexec) will run in parallel for different frames, or if false, run in a pipeline one frame at a time. */

    amw_flag_finalize_gameloop              = (1u << 30), /**< Don't continue with more work - finish all current frames and exit. */
    amw_flag_forced_exit                    = (1u << 31), /**< Close immediately, can't continue nor finish the work. */
};

/** Collects handles for individual systems that build up the game engine context. */
struct a_moonlit_walk {
    at_uint32_t     flags;

    /* backends */
    struct hadal   *hadal; // TODO should not be opaque, only internally
    struct moth    *moth; // TODO should not be opaque, only internally
    struct platinum plat;

    /* memory allocators */
    struct ipomoea *ipomoea;

    /* job system */
    struct riven   *riven;
    thread_t       *threads;
    size_t          thread_count;

    /* application */
    struct amw_callbacks callbacks;
};

/** Entry point for the framework. */
AMWAPI int32_t a_moonlit_walk(
        int32_t (*main__)(struct amw_hints *hints, int32_t, char **),
        int32_t argc, char **argv);

#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN

/* This function should be implemented by the application. */
extern int32_t amw_main(struct amw_hints *hints, int32_t argc, char **argv);

#if defined(AMW_PLATFORM_WINDOWS)
#include <windows.h>
#include <wchar.h>

static char **command_line_to_utf8_argv(LPWSTR w_command_line, int *o_argc)
{
    int    argc = 0;
    char **argv = 0;
    char  *args;

    LPWSTR *w_argv = CommandLineToArgvW(w_command_line, &argc);
    if (w_argv == NULL) {
        log_error("Win32 couldn't fetch command line arguments");
    } else {
        size_t size = wcslen(w_command_line) * 4;
        void *ptr_argv = malloc(((size_t)argc + 1) * sizeof(char *) + size);
        iazerop(ptr_argv);
        argv = (char **)ptr_argv;
        assert_release(argv);
        args = (char *)&argv[argc + 1];

        int32_t n;
        for (int32_t i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (int32_t)size, NULL, NULL);
            if (n == 0) {
                log_error("Win32 got a 0 length argument");
                break;
            }
            argv[i] = args;
            size -= (size_t)n;
            args += n;
        }
        LocalFree(w_argv);
    }
    *o_argc = argc;
    return argv;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
                   _In_opt_ HINSTANCE hPrevInstance, 
                   _In_ LPSTR lpCmdLine, 
                   _In_ int nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    int32_t res, argc_utf8 = 0;
    char **argv_utf8 = command_line_to_utf8_argv(GetCommandLineW(), &argc_utf8);

    res = a_moonlit_walk(amw_main, argc_utf8, argv_utf8);
    free(argv_utf8);
    return res;
}

#elif defined(AMW_PLATFORM_MACOS)
#elif defined(AMW_PLATFORM_IOS)
#elif defined(AMW_PLATFORM_ANDROID)

#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, 
                                        void* saved_state, 
                                        size_t saved_state_size) 
{
    (void)activity;
    (void)saved_state;
    (void)saved_state_size;
}

#else
int main(int argc, char **argv) 
{
    return a_moonlit_walk(amw_main, argc, argv);
}
#endif
#endif /* A_MOONLIT_WALK_MAIN */

#ifdef __cplusplus
}
#endif

#endif /* _A_MOONLIT_WALK_H */
