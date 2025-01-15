/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _A_MOONLIT_WALK_H
#define _A_MOONLIT_WALK_H

#include <lake/bedrock/defines.h>

#include <lake/bedrock/align.h>
#include <lake/bedrock/assert.h>
#include <lake/bedrock/atomic.h>
#include <lake/bedrock/endian.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/os.h>
#include <lake/bedrock/parser.h>
#include <lake/bedrock/str.h>
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

#include <lake/pelagia.h>
#include <lake/hadopelagic.h>
#include <lake/ipomoeaalba.h>
#include <lake/riven.h>
#include <lake/silver.h>
#include <lake/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

struct a_moonlit_walk; /* forward declaration */

/** Holds per-frame memory, pointer to the engine context and data needed to calculate and present a frame. */
struct amw_workload {
    u32                         idx;
    f64                         dt;
    //struct render_graph         render_graph;
    /* TODO put command buffers here */

    struct a_moonlit_walk      *AMW;
    const struct amw_workload  *last_work;
};

typedef s32  (*PFN_amw_init)(struct a_moonlit_walk *AMW, struct amw_workload *work, u32 frame_count, void *context);
typedef s32  (*PFN_amw_simulation)(struct a_moonlit_walk *AMW, struct amw_workload *work, void *context);
typedef s32  (*PFN_amw_rendering)(struct a_moonlit_walk *AMW, struct amw_workload *work, void *context);
typedef s32  (*PFN_amw_gpuexec)(struct a_moonlit_walk *AMW, struct amw_workload *work, void *context);
typedef void (*PFN_amw_cleanup)(struct a_moonlit_walk *AMW, void *context);

/** Data used for configuring the framework from the application space. */
struct amw_hints {
    const char *app_name;
    u32         version;

    u32         window_width, window_height;
    const char *window_title;

    u32         riven_thread_count;
    u32         riven_fiber_count;
    u32         riven_log_2_tears;
    u32         riven_stack_size;

    s32         pelagia_preferred_device_idx;
    s32         pelagia_workload_buffering;
    b32         pelagia_enable_vsync;

    struct {
        PFN_hadal_entry_point   hadal;
        PFN_silver_entry_point  silv;
    } entry_points;

    struct {
        PFN_amw_init        init;
        PFN_amw_simulation  simulation;
        PFN_amw_rendering   rendering;
        PFN_amw_gpuexec     gpuexec;
        PFN_amw_cleanup     cleanup;
    } callbacks;
};

/** Used to control the framework's gameloop. */
enum amw_flags {
    //amw_flag_parallel_gameloop_execution    = (1u << 0),  /**< Controls if the gameloop stages (simulation, rendering, gpuexec) will run in parallel for different frames, or if false, run in a pipeline one frame at a time. */

    amw_flag_finalize_gameloop              = (1u << 30), /**< Don't continue with more work - finish all current frames and exit. */
    amw_flag_forced_exit                    = (1u << 31), /**< Close immediately, can't continue nor finish the work. */
};

/** Collects handles for individual systems that build up the game engine context. */
struct a_moonlit_walk {
    at_u32       flags;

    struct ipomoeaalba  ia;
    struct hadopelagic  hadal;
    struct pelagia      pelagia;
    struct silver       silv;

    struct riven       *riven;
    thread_id          *threads;
    ssize               thread_count;

    const struct amw_hints *hints;
};

/** Entry point for the framework. */
AMWAPI s32 a_moonlit_walk(
    s32 (*main__)(struct amw_hints *hints, s32 argc, char **argv),
    s32 argc, char **argv);

#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN

/* This function should be implemented by the application. */
extern int32_t amw_main(struct amw_hints *hints, int32_t argc, char **argv);

#if defined(AMW_PLATFORM_WINDOWS)
#include <windows.h>
#include <wchar.h>

static char **command_line_to_utf8_argv(
    LPWSTR w_command_line, 
    int   *o_argc)
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

JNIEXPORT void ANativeActivity_onCreate(
    ANativeActivity *activity, 
    void            *saved_state, 
    size_t           saved_state_size) 
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
