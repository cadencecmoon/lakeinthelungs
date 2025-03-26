#pragma once

#include <amw/bedrock.h>
#include <amw/hadal.h>

FN_HADAL_WINDOW_CREATE(win32);
FN_HADAL_WINDOW_DESTROY(win32);
FN_HADAL_WINDOW_ATTACH_SWAPCHAIN(win32);
FN_HADAL_WINDOW_ACQUIRE_FRAMEBUFFER_EXTENT(win32);
#ifdef REZNOR_VULKAN
FN_HADAL_VULKAN_WRITE_INSTANCE_PROCEDURES(win32);
FN_HADAL_VULKAN_SURFACE_CREATE(win32);
FN_HADAL_VULKAN_PRESENTATION_SUPPORT(win32);
#endif

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

/* HACK: Define macros that some windows.h variants don't */
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif
#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif
#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA 0x0049
#endif
#ifndef WM_UNICHAR
#define WM_UNICHAR 0x0109
#endif
#ifndef UNICODE_NOCHAR
#define UNICODE_NOCHAR 0xFFFF
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif
#ifndef GET_XBUTTON_WPARAM
#define GET_XBUTTON_WPARAM(w) (HIWORD(w))
#endif
#ifndef EDS_ROTATEDMODE
#define EDS_ROTATEDMODE 0x00000004
#endif
#ifndef DISPLAY_DEVICE_ACTIVE
#define DISPLAY_DEVICE_ACTIVE 0x00000001
#endif
#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE 0x0603
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8 0x0602
#endif
#ifndef WM_GETDPISCALEDSIZE
#define WM_GETDPISCALEDSIZE 0x02e4
#endif
#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef OCR_HAND
#define OCR_HAND 32649
#endif

#if WINVER < 0x0601
typedef struct {
    DWORD cbSize;
    DWORD ExtStatus;
} CHANGEFILTERSTRUCT;
#ifndef MSGFLT_ALLOW
#define MSGFLT_ALLOW 1
#endif
#endif /* Windows 7 */

#if WINVER < 0x0600
#define DWM_BB_ENABLE 0x00000001
#define DWM_BB_BLURREGION 0x00000002
typedef struct {
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;
#else
#include <dwmapi.h>
#endif /* Windows Vista */

#ifndef DPI_ENUMS_DECLARED
typedef enum {
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif /* DPI_ENUMS_DECLARED */

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE) -4)
#endif 


/* HACK: Define macros that some xinput.h variants don't */
#ifndef XINPUT_CAPS_WIRELESS
#define XINPUT_CAPS_WIRELESS 0x0002
#endif
#ifndef XINPUT_DEVSUBTYPE_WHEEL
#define XINPUT_DEVSUBTYPE_WHEEL 0x02
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_STICK
#define XINPUT_DEVSUBTYPE_ARCADE_STICK 0x03
#endif
#ifndef XINPUT_DEVSUBTYPE_FLIGHT_STICK
#define XINPUT_DEVSUBTYPE_FLIGHT_STICK 0x04
#endif
#ifndef XINPUT_DEVSUBTYPE_DANCE_PAD
#define XINPUT_DEVSUBTYPE_DANCE_PAD 0x05
#endif
#ifndef XINPUT_DEVSUBTYPE_GUITAR
#define XINPUT_DEVSUBTYPE_GUITAR 0x06
#endif
#ifndef XINPUT_DEVSUBTYPE_DRUM_KIT
#define XINPUT_DEVSUBTYPE_DRUM_KIT 0x08
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_PAD
#define XINPUT_DEVSUBTYPE_ARCADE_PAD 0x13
#endif
#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

/* HACK: Define macros that some dinput.h variants don't */
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

/* xinput.dll */
typedef DWORD (WINAPI *PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES *);
typedef DWORD (WINAPI *PFN_XInputGetState)(DWORD, XINPUT_STATE *);
#define XInputGetCapabilities _glfw.win32.xinput.GetCapabilities
#define XInputGetState _glfw.win32.xinput.GetState

/* dinput8.dll */
typedef HRESULT (WINAPI *PFN_DirectInput8Create)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
#define DirectInput8Create _glfw.win32.dinput8.Create

/* user32.dll */
typedef BOOL (WINAPI *PFN_SetProcessDPIAware)(void);
typedef BOOL (WINAPI *PFN_ChangeWindowMessageFilterEx)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT *);
typedef BOOL (WINAPI *PFN_EnableNonClientDpiScaling)(HWND);
typedef BOOL (WINAPI *PFN_SetProcessDpiAwarenessContext)(HANDLE);
typedef UINT (WINAPI *PFN_GetDpiForWindow)(HWND);
typedef BOOL (WINAPI *PFN_AdjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT);
typedef int  (WINAPI *PFN_GetSystemMetricsForDpi)(int, UINT);
#define SetProcessDPIAware _glfw.win32.user32.SetProcessDPIAware_
#define ChangeWindowMessageFilterEx _glfw.win32.user32.ChangeWindowMessageFilterEx_
#define EnableNonClientDpiScaling _glfw.win32.user32.EnableNonClientDpiScaling_
#define SetProcessDpiAwarenessContext _glfw.win32.user32.SetProcessDpiAwarenessContext_
#define GetDpiForWindow _glfw.win32.user32.GetDpiForWindow_
#define AdjustWindowRectExForDpi _glfw.win32.user32.AdjustWindowRectExForDpi_
#define GetSystemMetricsForDpi _glfw.win32.user32.GetSystemMetricsForDpi_

/* dwmapi.dll */
typedef HRESULT (WINAPI *PFN_DwmIsCompositionEnabled)(BOOL *);
typedef HRESULT (WINAPI *PFN_DwmFlush)(VOID);
typedef HRESULT (WINAPI *PFN_DwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND *);
typedef HRESULT (WINAPI *PFN_DwmGetColorizationColor)(DWORD *, BOOL *);
#define DwmIsCompositionEnabled _glfw.win32.dwmapi.IsCompositionEnabled
#define DwmFlush _glfw.win32.dwmapi.Flush
#define DwmEnableBlurBehindWindow _glfw.win32.dwmapi.EnableBlurBehindWindow
#define DwmGetColorizationColor _glfw.win32.dwmapi.GetColorizationColor

/* shcore.dll */
typedef HRESULT (WINAPI *PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
typedef HRESULT (WINAPI *PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);
#define SetProcessDpiAwareness _glfw.win32.shcore.SetProcessDpiAwareness_
#define GetDpiForMonitor _glfw.win32.shcore.GetDpiForMonitor_

/* ntdll.dll */
typedef LONG (WINAPI *PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW *, ULONG, ULONGLONG);
#define RtlVerifyVersionInfo _glfw.win32.ntdll.RtlVerifyVersionInfo_

#if defined(REZNOR_VULKAN) && !defined(VULKAN_H_)
typedef u32 VkBool32;
typedef u32 VkFlags;
enum VkResult;

enum VkStructureType {
    VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR = 1000009000,
};

typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType                     sType;
    const void                         *pNext;
    VkWin32SurfaceCreateFlagsKHR        flags;
    HINSTANCE                           hinstance;
    HWND                                hwnd;
};
typedef VkResult (APIENTRY *PFN_vkCreateWin32SurfaceKHR)(
    struct VkInstance_T *, 
    const VkWin32SurfaceCreateInfoKHR *,
    const struct VkAllocationCallbacks *,
    struct VkSurfaceKHR_T **);

typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(
    struct VkPhysicalDevice_T *, u32);
#endif /* REZNOR_VULKAN */

struct hadal_pen {
    struct hadal_pen_header             header;
};

struct hadal_touch {
    struct hadal_touch_header           header;
};

struct hadal_mouse {
    struct hadal_mouse_header           header;
};

struct hadal_keyboard {
    struct hadal_keyboard_header        header;
};

struct hadal_joystick {
    struct hadal_joystick_header        header;
};

struct hadal_window {
    struct hadal_window_header          header;
};

struct hadal_output {
    struct hadal_output_header          header;
};

struct hadal {
    struct hadal_interface                      interface;

    struct {
        PFN_XInputGetCapabilities               GetCapabilities;
        PFN_XInputGetState                      GetState;
    } xinput;

    struct {
        PFN_DirectInput8Create                  Create;
    } dinput8;

    struct {
        PFN_SetProcessDPIAware                  SetProcessDPIAware_;
        PFN_ChangeWindowMessageFilterEx         ChangeWindowMessageFilterEx_;
        PFN_EnableNonClientDpiScaling           EnableNonClientDpiScaling_;
        PFN_SetProcessDpiAwarenessContext       SetProcessDpiAwarenessContext_;
        PFN_GetDpiForWindow                     GetDpiForWindow_;
        PFN_AdjustWindowRectExForDpi            AdjustWindowRectExForDpi_;
        PFN_GetSystemMetricsForDpi              GetSystemMetricsForDpi_;
    } user32;

    struct {
        PFN_DwmIsCompositionEnabled             IsCompositionEnabled;
        PFN_DwmFlush                            Flush;
        PFN_DwmEnableBlurBehindWindow           EnableBlurBehindWindow;
        PFN_DwmGetColorizationColor             GetColorizationColor;
    } dwmapi;

    struct {
        PFN_SetProcessDpiAwareness              SetProcessDPIAwareness_;
        PFN_GetDpiForMonitor                    GetDpiForMonitor_;
    } shcore;

    struct {
        PFN_RtlVerifyVersionInfo                RtlVerifyVersionInfo_;
    } ntdll;
#ifdef REZNOR_VULKAN
    struct {
        struct VkInstance_T                                    *instance;
        PFN_vkCreateWin32SurfaceKHR                             vkCreateWin32SurfaceKHR;
        PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR      vkGetPhysicalDeviceWin32PresentationSupportKHR;
    } vulkan;
#endif /* REZNOR_VULKAN */
}

extern struct hadal *g_windows;
