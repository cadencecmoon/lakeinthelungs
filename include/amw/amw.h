/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#ifndef _AMW_H
#define _AMW_H

#include <amw/common/defines.h>

#include <amw/common/align.h>
#include <amw/common/assert.h>
#include <amw/common/endian.h>
#include <amw/common/events.h>
#include <amw/common/filesystem.h>
#include <amw/common/log.h>
#include <amw/common/string.h>
#include <amw/common/unicode.h>

#include <amw/arch/atomic.h>
#include <amw/arch/bswap.h>
#include <amw/arch/simd.h>

#include <amw/math/types.h>

#include <amw/coda.h>
#include <amw/ipomoeaalba.h>
#include <amw/rana.h>
#include <amw/riven.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AMW_VERSION_MAJOR 0
#define AMW_VERSION_MINOR 1
#define AMW_VERSION_REVISION 1

/** Holds shared state for different engine systems. */
struct amw_engine;

typedef i32  (*PFN_amw_app_init)(struct amw_engine *amw);
typedef i32  (*PFN_amw_app_frame)(struct amw_engine *amw, double dt);
typedef void (*PFN_amw_app_event)(struct amw_engine *amw, union event *event);
typedef void (*PFN_amw_app_close)(struct amw_engine *amw);

/** Describes application metadata, user defined state and callbacks. */
struct amw_app {
    PFN_amw_app_init    init;
    PFN_amw_app_frame   frame; 
    PFN_amw_app_event   event; 
    PFN_amw_app_close   close; 
    void               *userdata;
    const char         *name;
    u32                 version;
};

/** Controls the configurable parts of the framework initialization. */
struct amw_engine_hints {
    struct amw_app      app;
};

/** Populates the hints struct with some default values, that can be used.
 *  Will use the host system configuration to determine most sane defaults. */
AMWAPI void AMWCALL amw_default_hints(struct amw_engine_hints *hints);

struct amw_engine {
    struct amw_engine_hints hints;

    struct amw_app          app;
};

/** Initializes an engine instance, from the provided configuration. */
AMWAPI int AMWCALL a_moonlit_walk(struct amw_engine *amw);

/** Define A_MOONLIT_WALK_MAIN in your main file, if you wish to make use of a
 *  predefined engine instance and game loop for your application. If you do, 
 *  then include this header with the define below, and create an amw_main() 
 *  function to serve as the entry point. It's not a must, you can define your 
 *  own entry points for different platforms, and use the engine systems in 
 *  isolation, creating just the parts that you need, and controling the 
 *  game loop by yourself. */
#ifdef A_MOONLIT_WALK_MAIN
#undef A_MOONLIT_WALK_MAIN

/** Entry point for the application, it should prepare hints for engine configuration
 *  that must be known before initializing the core systems. Most of the stuff can be changed
 *  later during the mainloop. Engine calls that don't depend on a context state can be 
 *  called from here without issues - so it's not really a place to initialize your assets,
 *  use the renderer, read window state, etc. etc.. Just read your configuration files,
 *  read the command line input and populate the hints struct given in the argument. This 
 *  hints struct is already populated with sane? default values, so just change what you need.
 *  You should return an enum amw_result to indicate any errors (amw_result_success == 0). */
int amw_main(struct amw_engine_hints *hints, int argc, char **argv);

#if defined(AMW_PLATFORM_WINDOWS)
#elif defined(AMW_PLATFORM_MACOS)
#elif defined(AMW_PLATFORM_IOS)
#elif defined(AMW_PLATFORM_ANDROID)
#else

int main(int argc, char **argv)
{
    struct amw_engine amw;
    amw_default_hints(&amw.hints);

    int code = amw_main(&amw.hints, argc, argv);
    if (code != 0)
        return code; /* TODO sys_exit */

    code = a_moonlit_walk(&amw);
    return code; /* TODO sys_exit */
}

#endif /* AMW_PLATFORM_LINUX */
#endif /* A_MOONLIT_WALK_MAIN */

#ifdef __cplusplus
}
#endif

#endif /* _AMW_H */
