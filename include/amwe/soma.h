#pragma once

#include <amwe/soma/encore.h>
#include <amwe/soma/sink.h>

#ifndef SOMA_MAX_ENCORE_COUNT
    #define SOMA_MAX_ENCORE_COUNT 1
#endif /* SOMA_MAX_ENCORE_COUNT */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the audio engine, implemented by a backend 'soma_encore'.
 *
 *  The encore 'userdata' type is 'NULL' (no custom parameters are expected for now). */
struct soma_interface {
    struct riven_interface_header           header;
    struct soma_interface_sink              sink;
};

/** A view into the backend. */
union soma_encore_view {
    struct riven_interface_header          *header;
    struct soma_interface                  *interface;
    struct soma_encore                     *encore;
};

/** Implies different strategies for the audio engine. */
enum soma_strategy {
    /** Allow the initialization process to figure out what strategy is best. */
    soma_strategy_auto = 0,
    /** Runs a single primary backend. */
    soma_strategy_optimal,
    /** May run multiple audio backends. */
    soma_strategy_debug,
};

/** An engine structure for audio processing. */
struct soma {
    enum soma_strategy                      strategy;
    atomic_u32                              flags;
    union soma_encore_view                  backends[SOMA_MAX_ENCORE_COUNT];

    union soma_sink_view                   *playback_sinks;
    u32                                     playback_sink_count;
    u32                                     playback_sink_capacity;

    union soma_sink_view                   *recording_sinks;
    u32                                     recording_sink_count;
    u32                                     recording_sink_capacity;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
