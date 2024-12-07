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

struct application_config {
    const char *name;
    u32         version;

    struct {
        u32         width;
        u32         height;
        const char *title;
    } window;

    struct {
        size_t threads; 
        size_t fibers;
        size_t stack_size;
        size_t log2_tears;
    } riven;
};

struct a_moonlit_walk {
    struct ipomoea *ia;
    struct riven   *riven;
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
