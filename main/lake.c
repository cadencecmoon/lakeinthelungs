#define A_MOONLIT_WALK_MAIN
#include "lake.h"

int32_t lake_in_the_lungs_init__(
        struct a_moonlit_walk *AMW, 
        struct amw_workload   *work, 
        uint32_t               workload_count, 
        void                  *context) 
{
    (void)AMW;
    (void)work;
    (void)workload_count;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_simulation__(
        struct a_moonlit_walk *AMW, 
        struct amw_workload   *work, 
        void                  *context)
{
    (void)AMW;
    (void)work;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_rendering__(
        struct a_moonlit_walk *AMW, 
        struct amw_workload   *work, 
        void                  *context)
{
    (void)AMW;
    (void)work;
    (void)context;
    return 0;
}

int32_t lake_in_the_lungs_gpuexec__(
        struct a_moonlit_walk *AMW, 
        struct amw_workload   *work, 
        void                  *context)
{
    (void)AMW;
    (void)work;
    (void)context;
    return 0;
}

void lake_in_the_lungs_cleanup__(
        struct a_moonlit_walk *AMW, 
        void                  *context)
{
    (void)AMW;
    (void)context;
}

int amw_main(struct amw_hints *hints, int argc, char **argv)
{
    hints->app_name = "Lake in the Lungs";
    hints->version = AMW_VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);
    hints->window_title = hints->app_name;
    hints->window_width = 1200;
    hints->window_height = 800;

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
