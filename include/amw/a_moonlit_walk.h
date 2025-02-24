#pragma once

#include <amw/bedrock.h>

#include <amw/bits.h>
#include <amw/endian.h>
#include <amw/hadal.h>
#include <amw/harridan.h>
#include <amw/hash_table.h>
#include <amw/input.h>
#include <amw/log.h>
#include <amw/octavia.h>
#include <amw/plugin.h>
#include <amw/process.h>
#include <amw/riven.h>
#include <amw/random.h>
#include <amw/string.h>
#include <amw/threads.h>
#include <amw/time.h>

#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN
extern s32 amw_main(s32 argc, char **argv);

#if defined(PLATFORM_WINDOWS)
/* some defines for including windows.h, and avoid namespace pollution */
/* make sure we get the "unicode" (acutally UTF-16) versions of Win32 APIs, it defaults to legacy crippled ones */
#ifndef UNICODE
    #define UNICODE 1
#endif
#ifndef _UNICODE
    #define _UNICODE 1
#endif

/* ask windows.h to include a little bit less of the stupid crap we'll never use */
#ifndef VC_EXTRALEAN
    #define VC_EXTRALEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOGDI
    #define NOGDI
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif

#define DIRECTINPUT_VERSION 0x0800
#define OEMRESOURCE

#include <windows.h>

#undef near
#undef far

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
    /* TODO */

    return amw_main(0, NULL);
}

#elif defined(PLATFORM_APPLE)
    /* TODO */

#elif defined(PLATFORM_EMSCRIPTEN)
    /* TODO */

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
    /* TODO */

    return amw_main(0, NULL);
}

#else
s32 main(s32 argc, char **argv)
{
    return amw_main(argc, argv);
}
#endif
#endif /* A_MOONLIT_WALK_MAIN */
