#ifndef _A_MOONLIT_WALK_FRAMEWORK_H
#define _A_MOONLIT_WALK_FRAMEWORK_H

#include <lake/a_moonlit_walk.h>

/** Holds memory for a maximum of 16 frames at a time. */
#define AMW_MAX_FRAMES 16

#define AMW_SIMULATION_TEAR_IDX 0
/** Entrypoint for the simulation stage of the mainloop. This stage will read any seed
 *  like the delta time, input or RNG and necessary data from the simulation stage of the last 
 *  frame. It will write data about models that are visible for drawing, and other variables
 *  like the camera position, render targets, a scene graph, skinning matrices, etc.. Only 
 *  this stage is responsible for simulating the game world - mechanics, animations, physics.
 *  
 *  @param[in,out]  struct amw_framedata */
RIVENS_TEAR(a_moonlit_walk_simulation_tear__);

#define AMW_RENDERING_TEAR_IDX 1
/** Entrypoint for the rendering stage of the mainloop. This stage will read data of the 
 *  world scene to visually and auditory render it, so it can be executed on the GPU and 
 *  presented to the player. It writes buffers with recorded commands for the graphics card.
 *
 *  @param[in,out]  struct amw_framedata */
RIVENS_TEAR(a_moonlit_walk_rendering_tear__);

#define AMW_GPUEXEC_TEAR_IDX 2
/** Entrypoint for the GPU execution stage of the mainloop. This stage will read the 
 *  recorded command buffers, audio mixer, simulation info like skinning matrices from the 
 *  animations, and do the heavy calculations in a 3D accelerated environment. When this is done, 
 *  the final frame can be presented to the screen, audio to the speakers, and the cycle can repeat. 
 *
 *  @param[in,out]  struct amw_framedata */
RIVENS_TEAR(a_moonlit_walk_gpuexec_tear__);

#endif /* _A_MOONLIT_WALK_FRAMEWORK_H */
