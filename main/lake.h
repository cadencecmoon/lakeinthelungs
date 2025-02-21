#pragma once

#include <amw/a_moonlit_walk.h>

#define LAKE_VERSION_MAJOR 0
#define LAKE_VERSION_MINOR 1
#define LAKE_VERSION_REVISION 1

#define LAKE_VERSION (VERSION_NUM(LAKE_VERSION_MAJOR, LAKE_VERSION_MINOR, LAKE_VERSION_REVISION))

struct lake {
    struct hadopelagic  hadal;

    struct rivens      *riven;
    u32                 thread_count;

    b32 exit_game;
    b32 restart_engine;
    b32 finalize_gameloop;
};

struct framedata {
    WORK_STRUCTURE_HEADER
    u64 index;
    f64 dt;

    struct lake            *lake;
    const struct framedata *last_frame;
    struct framedata       *next_frame;
    rivens_chain_t          chain;
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
