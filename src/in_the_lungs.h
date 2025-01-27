#pragma once

#include "bedrock.h"

#include "memory/allocator.h"
#include "memory/arena_allocator.h"
#include "memory/block_allocator.h"
#include "memory/stack_allocator.h"
#include "memory/tagged_heap_allocator.h"
#include "display/hadopelagic.h"

#define LAKE_VERSION_MAJOR 0
#define LAKE_VERSION_MINOR 1
#define LAKE_VERSION_REVISION 4

#define LAKE_VERSION (VERSION_NUM(LAKE_VERSION_MAJOR, LAKE_VERSION_MINOR, LAKE_VERSION_REVISION))

/** We target a parallelized workflow for no more than 4 frames at a time.
 *  Increasing this limit doesn't really make sense, and it may not be save either
 *  because a few things are designed with this limit in mind. For example, we will 
 *  copy skinning matrices, render targets or other frame-to-frame data this value 
 *  of times, so that every workload in the gameloop can be executed independently. */
#define LAKE_MAX_GAMELOOP_WORK 4

/** A Moonlit Walk Engine context. */
struct a_moonlit_walk {
    struct hadopelagic              hadal;  /**< Connects to the display backend. */
    struct tagged_heap_allocator    th;     /**< It is the only allocator allowed to map RAM memory. */
    struct riven                   *riven;  /**< The fiber-based job system. */
};

/** Represents a single gameloop workload, used within a process that transforms some input data 
 *  into a rendered frame, that can be presented to the player, or used somewhere else in the pipeline. */
struct lake_work {
    WORK_STRUCTURE_HEADER
    u64 idx;    /**< The index of the current frame. */
    f64 dt;     /**< The delta time of the last frame. */

    /** Holds the context of execution, with data that's shared between all stages and is frame-independent. */
    struct a_moonlit_walk *amw;
    /** A read-only pointer to the last frame, . */
    const struct lake_work *last_work;
    /** Allows gameloop stages to feed themselves on later iterations, has different implications depending on stage. */
    struct lake_work       *next_work;
};

#define LAKE_SIMULATION_TEAR_IDX 0
/** Entrypoint for the simulation stage of the mainloop. This stage will read any seed
 *  like the delta time, input or RNG and necessary data from the simulation stage of the last 
 *  frame. It will write data about models that are visible for drawing, and other variables
 *  like the camera position, render targets, a scene graph, skinning matrices, etc.. Only 
 *  this stage is responsible for simulating the game world - mechanics, animations, physics. */
extern void lake_in_the_lungs_simulation_tear__(struct lake_work *work);

#define LAKE_RENDERING_TEAR_IDX 1
/** Entrypoint for the rendering stage of the mainloop. This stage will read data of the 
 *  world scene to visually and auditory render it, so it can be executed on the GPU and 
 *  presented to the player. It writes buffers with recorded commands for the graphics card. */
extern void lake_in_the_lungs_rendering_tear__(struct lake_work *work);

#define LAKE_GPUEXEC_TEAR_IDX 2
/** Entrypoint for the GPU execution stage of the mainloop. This stage will read the 
 *  recorded command buffers, audio mixer, simulation info like skinning matrices from the 
 *  animations, and do the heavy calculations in a 3D accelerated environment. When this is done, 
 *  the final frame can be presented to the screen, audio to the speakers, and the cycle can repeat. */
extern void lake_in_the_lungs_gpuexec_tear__(struct lake_work *work);
