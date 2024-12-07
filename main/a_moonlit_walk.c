/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include "a_moonlit_walk.h"

static void a_moonlit_walk_cleanup__(struct a_moonlit_walk *AMW)
{
    (void)AMW;
}

static void a_moonlit_walk_main__(
        struct riven *riven,
        thread_t *threads,
        size_t thread_count,
        riven_argument_t argument)
{
    struct a_moonlit_walk AMW;
    iazero(AMW);

    AMW.riven = riven;

    { /* engine initialization */
        struct application_config *config = (struct application_config *)argument;

        (void)config;
    }

    (void)threads;
    (void)thread_count;

    a_moonlit_walk_cleanup__(&AMW);
}

AMWAPI int a_moonlit_walk(
        int (*main__)(struct application_config *, int, char **), 
        int argc, char **argv)
{
    int res = result_success;
    size_t riven_bytes;
    void *riven;

    struct application_config config;
    iazero(config);

    /* parse arguments, configure the engine */
    res = main__(&config, argc, argv);
    if (res != result_success)
        return res;

    { /* validate the configuration, provide defaults */
        if (config.name == NULL)            config.name = "Undefined";
        if (config.version == 0)            config.version = VERSION_NUM(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION);

        if (config.window.width <= 0)       config.window.width = 1200;
        if (config.window.height <= 0)      config.window.height = 800;
        if (config.window.title == NULL)    config.window.title = "A Moonlit Walk Engine";

        if (config.riven.threads == 0)      cpu_count(&config.riven.threads, NULL, NULL);
        if (config.riven.fibers == 0)       config.riven.fibers = 128;
        if (config.riven.stack_size == 0)   config.riven.stack_size = 64 * 1024;
        if (config.riven.log2_tears == 0)   config.riven.log2_tears = 12;
    }

    riven_bytes = riven_unveil_rift(
        NULL,                       /* Null to get the required memory buffer size. */
        config.riven.stack_size,    /* Size of stack per fiber in bytes. */
        config.riven.fibers,        /* How many fibers to instance. */
        config.riven.log2_tears,    /* Job queue will be (1u << log2_tears) entries large. */
        config.riven.threads,       /* How many CPU threads to run, including the main thread. */
        a_moonlit_walk_main__,      /* Main riven tear, serves as the entry point. */
        &config);                   /* Argument holding hints for other systems' configuration */
    riven = malloc(riven_bytes);

    riven_unveil_rift(
        riven,
        config.riven.stack_size,
        config.riven.fibers,
        config.riven.log2_tears,
        config.riven.threads,
        a_moonlit_walk_main__,
        &config);
    free(riven);

    return res;
}
