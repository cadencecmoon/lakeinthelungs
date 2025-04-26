#pragma once

#include <amwe/audio/soma_encore.h>
#include <amwe/audio/soma_sink.h>

#ifndef SOMA_INTERFACE_MAX_DEVICES_COUNT
#define SOMA_INTERFACE_MAX_DEVICES_COUNT 4
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** A public interface of the audio engine, implemented by a backend 'soma_encore'. */
struct soma_interface {
    struct riven_interface              riven;

    union soma_device_view              playback_devices[SOMA_INTERFACE_MAX_DEVICES_COUNT];
    union soma_device_view              record_devices[SOMA_INTERFACE_MAX_DEVICES_COUNT];
    u32                                 playback_device_count;
    u32                                 record_device_count;

    /* procedures, implemented by the backend */
    PFN_soma_device_query               device_query;
    PFN_soma_device_open                device_open;
    PFN_soma_device_close               device_close;
    PFN_soma_device_wait                device_wait;
};
union soma_audio {
    struct riven_interface     *riven;
    struct soma_interface      *interface;
    struct soma_encore         *encore;
    void                       *data;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
