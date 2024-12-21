#define A_MOONLIT_WALK_MAIN
#include "lake.h"

int32_t lake_in_the_lungs_init__(
        struct a_moonlit_walk *AMW, 
        struct amw_framedata *frames, 
        uint32_t frame_count, 
        void *context) 
{
    (void)AMW;
    (void)frames;
    (void)frame_count;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_simulation__(
        struct a_moonlit_walk *AMW, 
        struct amw_framedata *frame, 
        void *context)
{
    (void)AMW;
    (void)frame;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_rendering__(
        struct a_moonlit_walk *AMW, 
        struct amw_framedata *frame, 
        void *context)
{
    (void)AMW;
    (void)frame;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_gpuexec__(
        struct a_moonlit_walk *AMW, 
        struct amw_framedata *frame, 
        void *context)
{
    (void)AMW;
    (void)frame;
    (void)context;
    return 0;
}

void lake_in_the_lungs_cleanup__(
        struct a_moonlit_walk *AMW, 
        void *context)
{
    (void)AMW;
    (void)context;
}

int amw_main(struct amw_hints *hints, int argc, char **argv)
{
    hints->app_name = "Jezioro w Plucach";
    hints->version = AMW_VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);
    hints->window.title = "Lake in the Lungs";
    hints->window.width = 1200;
    hints->window.height = 800;

    hints->riven.thread_count = 0;
    hints->riven.fiber_count = 0;
    hints->riven.log_2_tears = 0;
    hints->riven.stack_size = 0;

    hints->callbacks.init = lake_in_the_lungs_init__;
    hints->callbacks.simulation = lake_in_the_lungs_simulation__;
    hints->callbacks.rendering = lake_in_the_lungs_rendering__;
    hints->callbacks.gpuexec = lake_in_the_lungs_gpuexec__;
    hints->callbacks.cleanup = lake_in_the_lungs_cleanup__;

    /* for now we won't read the command line arguments */
    (void)argc;
    (void)argv;

    return 0;
}
