#include <amwe/a_moonlit_walk.h>

/** Describes all engine and application runtime settings. Holds all settings 
 *  that can be updated without restarting the application. */
struct amwe_settings {
    u32 todo;
};

#define AMWE_SIMULATION_WORK_INDEX 0
#define AMWE_RENDERING_WORK_INDEX  1
#define AMWE_GPUEXEC_WORK_INDEX    2

extern void LAKECALL a_moonlit_walk_simulation(struct amwe_pipeline_work *work);
extern void LAKECALL a_moonlit_walk_rendering(struct amwe_pipeline_work *work);
extern void LAKECALL a_moonlit_walk_gpuexec(struct amwe_pipeline_work *work);

extern FN_RIVEN_INTERFACE_VALIDATION(hadal);
extern FN_RIVEN_INTERFACE_VALIDATION(soma);
extern FN_RIVEN_INTERFACE_VALIDATION(xaku);
