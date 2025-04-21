#pragma once

#include <amwe/audio/encore.h>
#include <amwe/audio/sink.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the audio engine, implemented by a backend 'soma_encore'.
 *
 *  The encore 'userdata' type is 'struct soma_encore_assembly'. */
struct soma_interface {
    struct riven_interface_header           header;
    PFN_soma_sink_query                     sink_query;
    PFN_soma_sink_open                      sink_open;
    PFN_soma_sink_close                     sink_close;
    PFN_soma_sink_wait                      sink_wait;
};

/** A view into the backend. */
union soma_encore_view {
    struct riven_interface_header          *header;
    struct soma_interface                  *interface;
    struct soma_encore                     *encore;
};

/** A view into an audio sink. */
union soma_sink_view {
    struct {SOMA_INTERFACE_SINK_HEADER}    *header;
    struct soma_sink                       *sink;
    union soma_encore_view                 *encore_view;
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
    enum soma_strategy                          strategy;
    atomic_u32                                  flags;
    union soma_encore_view                      backend;

    lake_dynamic_array(union soma_sink_view)    playback_sinks;
    lake_dynamic_array(union soma_sink_view)    record_sinks;
};

LAKEAPI void LAKECALL
soma_fini(struct soma *soma);

#ifdef __cplusplus
}
#endif /* __cplusplus */
