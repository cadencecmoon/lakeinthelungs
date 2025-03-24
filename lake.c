#define A_MOONLIT_WALK_MAIN
#include <amw/a_moonlit_walk.h>

RIVEN_ENCORE(moonlit, lake)
{
    (void)encore;
}

PFN_riven_work amw_main(struct amw_config *config, s32 argc, char **argv)
{
    config->metadata.game_build_version = AMW_VERSION;
    config->metadata.game_name = str_init("Lake in the Lungs");

    log_set_verbose(argc-1);

    /* TODO parse the command line */
    (void)argv;

    return (PFN_riven_work)moonlit_encore_lake;
}
