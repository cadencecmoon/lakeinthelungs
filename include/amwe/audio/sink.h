#pragma once

#include <amwe/audio/encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ARGS_SOMA_SINK_QUERY(ENCORE) \
    struct soma_encore *ENCORE, \
    struct soma_sink  **default_playback, \
    struct soma_sink  **default_recording
typedef lake_nodiscard enum soma_result (LAKECALL *PFN_soma_sink_query)(ARGS_SOMA_SINK_QUERY(encore));
#define FN_SOMA_SINK_QUERY(ENCORE) \
    lake_nodiscard enum soma_result LAKECALL _soma_##ENCORE##_sink_query(ARGS_SOMA_SINK_QUERY(ENCORE))

typedef lake_nodiscard enum soma_result (LAKECALL *PFN_soma_sink_open)(struct soma_encore *encore);
#define FN_SOMA_SINK_OPEN(ENCORE) \
    lake_nodiscard enum soma_result LAKECALL _soma_##ENCORE##_sink_open(struct soma_encore *ENCORE)

typedef void (LAKECALL *PFN_soma_sink_close)(struct soma_sink *sink);
#define FN_SOMA_SINK_CLOSE(ENCORE) \
    void LAKECALL _soma_##ENCORE##_sink_close(struct soma_sink *sink)

typedef bool (LAKECALL *PFN_soma_sink_wait)(struct soma_sink *sink);
#define FN_SOMA_SINK_WAIT(ENCORE) \
    bool LAKECALL _soma_##ENCORE##_sink_wait(struct soma_sink *sink)

#ifdef __cplusplus
}
#endif /* __cplusplus */
