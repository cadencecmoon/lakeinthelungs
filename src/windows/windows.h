#pragma once

#include <amw/bedrock.h>

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

#include <wctype.h>
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>

#undef near
#undef far
