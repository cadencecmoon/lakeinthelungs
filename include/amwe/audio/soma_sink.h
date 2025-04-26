#pragma once

#include <amwe/audio/soma_encore.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ARGS_SOMA_DEVICE_QUERY(ENCORE) \
    struct soma_encore  *ENCORE, \
    struct soma_device **default_playback, \
    struct soma_device **default_recording
typedef lake_nodiscard enum soma_result (LAKECALL *PFN_soma_device_query)(ARGS_SOMA_DEVICE_QUERY(encore));
#define FN_SOMA_DEVICE_QUERY(ENCORE) \
    lake_nodiscard enum soma_result LAKECALL _soma_##ENCORE##_device_query(ARGS_SOMA_DEVICE_QUERY(ENCORE))

typedef lake_nodiscard enum soma_result (LAKECALL *PFN_soma_device_open)(struct soma_encore *encore);
#define FN_SOMA_DEVICE_OPEN(ENCORE) \
    lake_nodiscard enum soma_result LAKECALL _soma_##ENCORE##_device_open(struct soma_encore *ENCORE)

typedef void (LAKECALL *PFN_soma_device_close)(struct soma_device *device);
#define FN_SOMA_DEVICE_CLOSE(ENCORE) \
    void LAKECALL _soma_##ENCORE##_device_close(struct soma_device *device)

typedef bool (LAKECALL *PFN_soma_device_wait)(struct soma_device *device);
#define FN_SOMA_DEVICE_WAIT(ENCORE) \
    bool LAKECALL _soma_##ENCORE##_device_wait(struct soma_device *device)

/** A view into an audio sink. */
union soma_device_view {
    struct {SOMA_INTERFACE_DEVICE_HEADER}  *header;
    struct soma_device                     *device;
    union soma_encore_view                 *encore_view;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
