/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _A_MOONLIT_WALK_H
#define _A_MOONLIT_WALK_H

#include <amw/common/defines.h>

#include <amw/common/align.h>
#include <amw/common/assert.h>
#include <amw/common/cpu.h>
#include <amw/common/endian.h>
#include <amw/common/events.h>
#include <amw/common/filesystem.h>
#include <amw/common/log.h>
#include <amw/common/parser.h>
#include <amw/common/string.h>
#include <amw/common/time.h>
#include <amw/common/unicode.h>

#include <amw/arch/atomic.h>
#include <amw/arch/bswap.h>
#include <amw/arch/simd.h>

#include <amw/datastructures/arena_allocator.h>

#include <amw/input/gamepad.h>
#include <amw/input/joystick.h>
#include <amw/input/keyboard.h>
#include <amw/input/mouse.h>
#include <amw/input/pen.h>
#include <amw/input/touch.h>

#include <amw/math/types.h>

#include <amw/cynic.h>      /* Audio Engine */
#include <amw/hadal.h>      /* Platform Abstraction Layer */
#include <amw/ipomoea.h>    /* Tagged Heap Allocator */
#include <amw/riven.h>      /* Fiber-based Job System */
#include <amw/srebro.h>     /* Rendering Engine */

AMW_C_DECL_BEGIN

#define AMW_VERSION_MAJOR 0
#define AMW_VERSION_MINOR 1
#define AMW_VERSION_REVISION 1

/** A way for configuring the framework by the application. */
struct application_config {
    const char *name;
    u32         version;

    struct { /* HADAL */
        u32         api;
        u32         width;
        u32         height;
        const char *title;
        u32         allow_headless; /* fallback */
    } hadal;

    struct { /* RIVEN */
        size_t threads; 
        size_t fibers;
        size_t stack_size;
        size_t log2_tears;
    } riven;
};

/** Used to control the framework's gameloop. */
enum amw_flag {
    amw_flag_dont_continue_work = (1u << 0), /**< Don't continue with more work - finish all current frames and exit. */

    amw_flag_run_gameloop = (1u << 30), /**< A flag for the while condition. */
    amw_flag_forced_exit  = (1u << 31), /**< Close immediately, can't continue nor finish the work. */
};

/** Collects handles for individual systems that build up the game engine context. */
struct a_moonlit_walk {
    at_u32_t flags;

    struct ipomoea *ia;
    struct riven   *riven;
    struct hadal   *hadal;
};

AMWAPI int a_moonlit_walk(
        int (*main__)(struct application_config *, int, char **), 
        int argc, char **argv);

/** Holds data for a maximum of frames at a time: calculate N, N-1, N-2 and preserve N-3. */
#define AMW_MAX_FRAMES 4

#define AMW_SIMULATION_TEAR_IDX 0
/** Entrypoint for the simulation stage of the mainloop. This stage will read any seed
 *  like the delta time, input or RNG and necessary data from the simulation stage of the last 
 *  frame. It will write data about models that are visible for drawing, and other variables
 *  like the camera position, render targets, a scene graph, skinning matrices, etc.. Only 
 *  this stage is responsible for simulating the game world - mechanics, animations, physics.
 *  
 *  The argument for this tear is: struct framedata */
AMW_RIVENS_TEARS(a_moonlit_walk_simulation_tear__);

#define AMW_RENDERING_TEAR_IDX 1
/** Entrypoint for the rendering stage of the mainloop. This stage will read data of the 
 *  world scene to visually and auditory render it, so it can be executed on the GPU and 
 *  presented to the player. It writes buffers with recorded commands for the graphics card.
 *
 *  The argument for this tear is: struct framedata */
AMW_RIVENS_TEARS(a_moonlit_walk_rendering_tear__);

#define AMW_GPUEXEC_TEAR_IDX 2
/** Entrypoint for the GPU execution stage of the mainloop. This stage will read the 
 *  recorded command buffers, audio mixer, simulation info like skinning matrices from the 
 *  animations, and do the heavy calculations in a 3D accelerated environment. When this is done, 
 *  the final frame can be presented to the screen, audio to the speakers, and the cycle can repeat. 
 *
 *  The argument for this tear is: struct framedata */
AMW_RIVENS_TEARS(a_moonlit_walk_gpuexec_tear__);

struct framedata {
    u32             frame_idx;          /**< The index of the current frames, incremented every frame. Should be cycled with %AMW_MAX_FRAMES. */
    f64             delta_time;         /**< A seed for simulation, the time it took to complete the last frame. */

    struct a_moonlit_walk *AMW;         /**< The engine context */

    /** Holds a pointer to the previous work, read only. */
    const struct framedata *previous_frame;
};


#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN

/* This function should be implemented by the application. */
extern int amw_main(struct application_config *config, int argc, char **argv);

#if defined(AMW_PLATFORM_WINDOWS)
#include <windows.h>
#include <wchar.h>

static char **command_line_to_utf8_argv(LPWSTR w_command_line, int *o_argc)
{
    int argc = 0;
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

        i32 n;
        for (i32 i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (i32)size, NULL, NULL);
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

    int res, argc_utf8 = 0;
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
    /* TODO */
}

#else
int main(int argc, char **argv) 
{
    return a_moonlit_walk(amw_main, argc, argv);
}
#endif
#endif /* A_MOONLIT_WALK_MAIN */

AMW_C_DECL_END
#endif /* _AMW_INTERNAL_H */
