#pragma once

#include <amw/bedrock.h>

#include <amw/bits.h>
#include <amw/hadal.h>
#include <amw/hash_table.h>
#include <amw/keycodes.h>
#include <amw/log.h>
#include <amw/process.h>
#include <amw/reznor.h>
#include <amw/riven.h>
#include <amw/scancodes.h>
#include <amw/soma.h>
#include <amw/string.h>
#include <amw/threads.h>
#include <amw/time.h>

#define AMW_VERSION_MAJOR 0
#define AMW_VERSION_MINOR 1
#define AMW_VERSION_REVISION 4

#define AMW_VERSION (VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION))

#ifndef AMW_MAX_RENDERING_DEVICE_COUNT
#define AMW_MAX_RENDERING_DEVICE_COUNT 4
#endif
#ifndef AMW_MAX_WINDOW_COUNT
#define AMW_MAX_WINDOW_COUNT REZNOR_MAX_SWAPCHAINS
#endif
#ifndef AMW_MAX_DEFERRED_COUNT
#define AMW_MAX_DEFERRED_COUNT 8
#endif
#define AMW_MAX_FRAMEDATA 16

/** An opaque game context. */
struct moonlit;

/** Information passed to the application at engine initialization. */
struct moonlit_encore {
    struct riven_encore_header      header;
    struct a_moonlit_walk_engine   *amw;
};

/** Procedures implemented by a game application. */
struct moonlit_interface {
    struct riven_interface_header   header;
    struct a_moonlit_walk_engine   *amw;
};

/** Information about the application. */
struct amw_metadata {
    struct str      engine_name;
    u32             engine_build_version;
    struct str      game_name;
    u32             game_build_version;
};

/* forward declarations */
struct amw_deferred_work;

/** Hints to configure the framework. */
struct amw_config {
    struct amw_metadata             metadata;
    u32                             frames_in_flight; // best 3 or 4

    u32                             hadal_main_width, hadal_main_height;
    s32                             reznor_force_primary_device_index;
    u32                             reznor_target_device_count;
    b32                             reznor_prefer_only_discrete_devices;

    usize                           riven_memory_budget_size;
    usize                           riven_fiber_stack_size;
    u32                             riven_fiber_count;
    u32                             riven_thread_count;
    u32                             riven_tagged_heap_count;
    u32                             riven_log2_work_count;
    u32                             riven_log2_memory_count;

    struct {
        PFN_riven_work              hadal;
        PFN_riven_work              reznor;
        PFN_riven_work              soma;
        PFN_riven_work              moonlit;
    } encores;

    /* debug options */
    u32                             reznor_virtual_device_count;
    b32                             debug_utilities;
};

/** Engine state. */
struct a_moonlit_walk_engine {
    /** The game application. */
    struct moonlit                 *moonlit;

    /** The display backend. */
    struct hadal                   *hadal;
    struct hadal_window            *windows[AMW_MAX_WINDOW_COUNT];

    /** The rendering engine. */
    struct reznor                  *reznor;
    struct reznor_device           *render_devices[AMW_MAX_RENDERING_DEVICE_COUNT];

    /** The audio engine. */
    struct soma                    *soma;
    struct soma_device             *playback_device;
    struct soma_device             *recording_device;

    /** Implies the count of swapchains and interface viewports too, can change at any time. */
    at_u32                          window_count;
    u32                             active_window_count;

    /** The number of available rendering devices, 1 or more in a mGPU setup. */
    u32                             render_device_count;
    /** The number of frames in flight that hold frame-to-frame internal data of some systems. */
    u32                             frames_in_flight;

    struct riven                   *riven;
    const thread_t                 *threads;
    const u32                       thread_count;

    /* gameloop flags */
    b32                             exit_game;
    b32                             restart_engine;
    b32                             finalize_gameloop;
};

/** Will execute itself as the frame leaves the pipeline. */
struct amw_deferred_work {
    struct amw_framedata           *frame;
    /** An interpretation of the work. */
    PFN_riven_work                  procedure;
    /** An index value related to the work, e.g. index of the window to be destroyed. */
    u32                             context;
};

/** Describes data for a single frame-worth of work. A frame is the result of such work,
 *  that we can show to the player in auditory and visual format. */
struct amw_framedata {
    s32                             result;
    /** The index of the current frame, always incrementing. */
    u64                             frame_index;
    /** Delta time of the last frame, whatever it is. */
    f64                             delta_time;

    /** Work executed after the framedata leaves the pipeline. */
    struct amw_deferred_work        deferred_work[AMW_MAX_DEFERRED_COUNT];
    u32                             deferred_count;

    struct reznor_device_frame     *render_frames[AMW_MAX_RENDERING_DEVICE_COUNT];
    struct reznor_swapchain        *swapchains[AMW_MAX_WINDOW_COUNT];
    u32                             swapchain_count;

    riven_tag_t                     game_to_rendering_tag;
    riven_tag_t                     game_to_gpuexec_tag;
    riven_tag_t                     rendering_to_gpuexec_tag;

    struct a_moonlit_walk_engine   *amw;
    const struct amw_framedata     *last_work;
    struct amw_framedata           *next_work;
    /** A synchronization primitive, only one stage can process this work at a time. */
    riven_chain_t                   chain;
};

/** Main engine procedure. */
AMWAPI s32 AMWCALL a_moonlit_walk(
    struct riven            *riven,
    thread_t                *threads,
    u32                      thread_count,
    const struct amw_config *config);

#define AMW_GAME_WORK_IDX 0
/** Entrypoint for the simulation stage of the mainloop. This stage will read any seed
 *  like the delta time, input or RNG and necessary data from the simulation stage of the last 
 *  frame. It will write data about models that are visible for drawing, and other variables
 *  like the camera position, render targets, a scene graph, skinning matrices, etc.. Only 
 *  this stage is responsible for simulating the game world - mechanics, animations, physics. */
AMWAPI void AMWCALL a_moonlit_walk_game(struct amw_framedata *restrict work);

#define AMW_RENDERING_WORK_IDX 1
/** Entrypoint for the rendering stage of the mainloop. This stage will read data of the 
 *  world scene to visually and auditory render it, so it can be executed on the GPU and 
 *  presented to the player. It writes buffers with recorded commands for the graphics card. */
AMWAPI void AMWCALL a_moonlit_walk_rendering(struct amw_framedata *restrict work);

#define AMW_GPUEXEC_WORK_IDX 2
/** Entrypoint for the GPU execution stage of the mainloop. This stage will read the 
 *  recorded command buffers, audio mixer, simulation info like skinning matrices from the 
 *  animations, and do the heavy calculations in a 3D accelerated environment. When this is done, 
 *  the final frame can be presented to the screen, audio to the speakers, and the cycle can repeat. */
AMWAPI void AMWCALL a_moonlit_walk_gpuexec(struct amw_framedata *restrict work);

/** Entry point for the framework. */
AMWAPI s32 AMWCALL amw_run_framework(
    PFN_riven_work (AMWCALL *entry)(struct amw_config *, s32, char **), 
    s32 argc, char **argv);

#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN
/** Function for the application to parse the command line and configure the engine accordingly. */
extern PFN_riven_work AMWCALL amw_main(struct amw_config *config, s32 argc, char **argv);

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
    return amw_run_framework(amw_main, 0, NULL);
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
    return amw_run_framework(amw_main, 0, NULL);
}

#else
s32 main(s32 argc, char **argv)
{
    return amw_run_framework(amw_main, argc, argv);
}
#endif
#endif /* A_MOONLIT_WALK_MAIN */
