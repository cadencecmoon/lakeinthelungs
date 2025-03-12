#pragma once

#include <amw/a_moonlit_walk.h>

#define LAKE_VERSION_MAJOR 0
#define LAKE_VERSION_MINOR 1
#define LAKE_VERSION_REVISION 3

#define LAKE_VERSION (VERSION_NUM(LAKE_VERSION_MAJOR, LAKE_VERSION_MINOR, LAKE_VERSION_REVISION))

/** Crafts a shared context of multiple engine subsystems. */
struct lake {
    /* audio engine */
    struct octavia         *octavia;

    /* renderer */
    struct pelagia         *pelagia;
    struct pelagia_device **devices;
    u32                     device_count;

    /* display */
    struct hadal           *hadal;

    /* framework */
    struct riven           *riven;
    struct riven_metadata  *metadata;
    thread_t               *threads;
    u32                     thread_count;

    /* gameloop flags */
    b32 exit_game;
    b32 restart_engine;
    b32 finalize_gameloop;
};

/** Describes data for a single frame-worth of work. A frame is the result of such work,
 *  that we can show to the player in auditory and visual format. */
struct framedata {
    struct work_header work_header;
    /** A frame index incremented every frame. Can be cycled to access per-frame resources via an array index. */
    u64 frame_index;
    /** Delta time of the last frame, whatever it is. */
    f64 dt;     

    /* TODO instance arrays, command buffers, skinning matrices */

    struct lake        *lake;
    struct framedata   *last_frame;
    /** Next framedata, used to feed forward data like low-priority ray casts, seeds, etc.
     *  The framedata is assumed to may have invalid/outdated state, and is really intended
     *  only for touching long-lived data of the same gameloop stage, none other. */
    struct framedata   *next_frame;
    /** A synchronization primitive, only one stage can work on a framedata at a time. */
    riven_chain_t       chain;
};

#define LAKE_SIMULATION_WORK_IDX 0
/** Entrypoint for the simulation stage of the mainloop. This stage will read any seed
 *  like the delta time, input or RNG and necessary data from the simulation stage of the last 
 *  frame. It will write data about models that are visible for drawing, and other variables
 *  like the camera position, render targets, a scene graph, skinning matrices, etc.. Only 
 *  this stage is responsible for simulating the game world - mechanics, animations, physics. */
extern void lake_in_the_lungs_simulation(struct framedata *work);

#define LAKE_RENDERING_WORK_IDX 1
/** Entrypoint for the rendering stage of the mainloop. This stage will read data of the 
 *  world scene to visually and auditory render it, so it can be executed on the GPU and 
 *  presented to the player. It writes buffers with recorded commands for the graphics card. */
extern void lake_in_the_lungs_rendering(struct framedata *work);

#define LAKE_GPUEXEC_WORK_IDX 2
/** Entrypoint for the GPU execution stage of the mainloop. This stage will read the 
 *  recorded command buffers, audio mixer, simulation info like skinning matrices from the 
 *  animations, and do the heavy calculations in a 3D accelerated environment. When this is done, 
 *  the final frame can be presented to the screen, audio to the speakers, and the cycle can repeat. */
extern void lake_in_the_lungs_gpuexec(struct framedata *work);
