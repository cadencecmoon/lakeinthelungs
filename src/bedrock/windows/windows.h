#pragma once

#include "../defines.h"

/* some defines for including windows.h, and avoid namespace pollution */
#ifdef PLATFORM_WINDOWS
    /* make sure we get the "unicode" (acutally UTF-16) versions of Win32 APIs, it defaults to legacy crippled ones */
    #ifndef UNICODE
        #define UNICODE 1
    #endif
    #ifndef _UNICODE
        #define _UNICODE 1
    #endif

    /* ask windows.h to include a little bit less of the stupid crap we'll never use */
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #define NOMINMAX

    #include <windows.h>

    #undef near
    #undef far
#endif
