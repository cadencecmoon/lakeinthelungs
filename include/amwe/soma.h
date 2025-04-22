#pragma once

#include <amwe/soma/encore.h>
#include <amwe/soma/sink.h>

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

/** Implies different strategies for the audio engine. */
enum soma_strategy {
    /** Allow the initialization process to figure out what strategy is best, not a valid strategy. */
    soma_strategy_auto = 0,
    /** Runs a single primary backend. */
    soma_strategy_optimal,
    /** May run multiple audio backends. */
    soma_strategy_debug,
};

/** An engine structure for audio processing. */
struct soma_audio {
    enum soma_strategy                          strategy;
    atomic_u32                                  flags;
    union soma_encore_view                      soma;

    lake_dynamic_array(union soma_sink_view)    playback_sinks;
    lake_dynamic_array(union soma_sink_view)    record_sinks;
};

struct soma_audio_assembly {
    union soma_encore_view      soma;
    enum soma_strategy          strategy;
};

LAKEAPI lake_nonnull_all lake_nodiscard
enum soma_result LAKECALL soma_audio_init(
    const struct soma_audio_assembly *assembly,
    struct soma_audio                *out_audio);

/** Returns previous reference count of the encore:
 *  - 0 means invalid backend.
 *  - 1 means the backend may be safely destroyed. 
 *  - >1 means that another system holds onto the backend. */
LAKEAPI lake_nonnull_all u32 LAKECALL
soma_audio_fini(struct soma_audio *audio, struct riven_work *out_zero_refcnt);

#ifdef __cplusplus
}
#endif /* __cplusplus */
