/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#define A_MOONLIT_WALK_MAIN
#include "../a_moonlit_walk.h"

int amw_main(struct application_config *config, int argc, char **argv)
{
    config->name = "Lake in the Lungs";
    config->version = VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);

    config->window.width = 1200;
    config->window.height = 800;
    config->window.title = config->name;

    (void)argc;
    (void)argv;

    return result_success;
}
