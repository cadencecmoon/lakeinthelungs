#include <amw/soma.h>
#include <amw/log.h>
#include <amw/process.h>

FN_SOMA_DEVICE_QUERY(pipewire);
FN_SOMA_DEVICE_OPEN(pipewire);
FN_SOMA_DEVICE_CLOSE(pipewire);
FN_SOMA_DEVICE_WAIT(pipewire);

#include <pipewire/pipewire.h>
#include <pipewire/loop.h>
#include <pipewire/extensions/metadata.h>
#include <spa/param/audio/format-utils.h>
#include <spa/utils/json.h>

#ifndef PW_KEY_CONFIG_NAME
#define PW_KEY_CONFIG_NAME "config.name"
#endif

#ifndef PW_KEY_NODE_RATE
#define PW_KEY_NODE_RATE "node.rate"
#endif

#ifndef PW_KEY_TARGET_OBJECT
#define PW_KEY_TARGET_OBJECT "target.object"
#endif

#define PW_MIN_SAMPLES                  32 /* about 0.67ms at 48kHz */
#define PW_BASE_CLOCK_RATE              48000

#define PW_POD_BUFFER_LENGTH            1024
#define PW_THREAD_NAME_BUFFER_LENGTH    128
#define PW_MAX_IDENTIFIER_LENGTH        256

enum PW_READY_FLAGS {
    PW_READY_FLAG_BUFFER_ADDED = 0x1,
    PW_READY_FLAG_STREAM_READY = 0x2,
    PW_READY_FLAG_ALL_BITS = 0x3,
};

#define PW_ID_TO_HANDLE(x) (void *)((uptr)x)
#define PW_HANDLE_TO_ID(x) (u32)((uptr)x)

typedef const char *(*PFN_pw_get_library_version)(void);
typedef void (*PFN_pw_init)(s32 *, char ***);
typedef void (*PFN_pw_deinit)(void);
typedef struct pw_loop *(*PFN_pw_loop_new)(const struct spa_dict *props);
typedef void (*PFN_pw_loop_destroy)(struct pw_loop *loop);
typedef s32 (*PFN_pw_loop_set_name)(struct pw_loop *loop, const char *name);
typedef struct pw_context *(*PFN_pw_context_new)(struct pw_loop *, struct pw_properties *, usize);
typedef void (*PFN_pw_context_destroy)(struct pw_context *);
typedef struct pw_core *(*PFN_pw_context_connect)(struct pw_context *, struct pw_properties *, usize);
typedef void (*PFN_pw_proxy_add_object_listener)(struct pw_proxy *, struct spa_hook *, const void *, void *);
typedef void *(*PFN_pw_proxy_get_user_data)(struct pw_proxy *);
typedef void (*PFN_pw_proxy_destroy)(struct pw_proxy *);
typedef s32 (*PFN_pw_core_disconnect)(struct pw_core *);
typedef struct pw_stream *(*PFN_pw_stream_new_simple)(struct pw_loop *, const char *, struct pw_properties *, const struct pw_stream_events *, void *);
typedef void (*PFN_pw_stream_destroy)(struct pw_stream *);
typedef s32 (*PFN_pw_stream_connect)(struct pw_stream *, enum pw_direction, u32, enum pw_stream_flags, const struct spa_pod **, u32);
typedef enum pw_stream_state (*PFN_pw_stream_get_state)(struct pw_stream *, const char **);
typedef struct pw_buffer *(*PFN_pw_stream_dequeue_buffer)(struct pw_stream *);
typedef s32 (*PFN_pw_stream_queue_buffer)(struct pw_stream *, struct pw_buffer *);
typedef struct pw_properties *(*PFN_pw_properties_new)(const char *, ...) SPA_SENTINEL;
typedef s32 (*PFN_pw_properties_set)(struct pw_properties *, const char *, const char *);
typedef s32 (*PFN_pw_properties_setf)(struct pw_properties *, const char *, const char *, ...) SPA_PRINTF_FUNC(3, 4);

struct soma_device {
    struct soma_device_header           header;
};

struct soma {
    struct soma_interface               interface;

    void                               *module_pipewire;
    PFN_pw_get_library_version          pw_get_library_version;
    PFN_pw_init                         pw_init;
    PFN_pw_deinit                       pw_deinit;
    PFN_pw_loop_new                     pw_loop_new;           
    PFN_pw_loop_destroy                 pw_loop_destroy;
    PFN_pw_loop_set_name                pw_loop_set_name;
    PFN_pw_context_new                  pw_context_new;
    PFN_pw_context_destroy              pw_context_destroy;
    PFN_pw_context_connect              pw_context_connect;
    PFN_pw_proxy_add_object_listener    pw_proxy_add_object_listener;
    PFN_pw_proxy_get_user_data          pw_proxy_get_user_data;
    PFN_pw_proxy_destroy                pw_proxy_destroy;
    PFN_pw_core_disconnect              pw_core_disconnect;
    PFN_pw_stream_new_simple            pw_stream_new_simple;
    PFN_pw_stream_destroy               pw_stream_destroy;
    PFN_pw_stream_connect               pw_stream_connect;
    PFN_pw_stream_get_state             pw_stream_get_state;
    PFN_pw_stream_dequeue_buffer        pw_stream_dequeue_buffer;
    PFN_pw_stream_queue_buffer          pw_stream_queue_buffer;
    PFN_pw_properties_new               pw_properties_new;
    PFN_pw_properties_set               pw_properties_set;
    PFN_pw_properties_setf              pw_properties_setf;

    s32                                 pipewire_version_major;
    s32                                 pipewire_version_minor;
    s32                                 pipewire_version_patch;
};

static void *pipewire_dlsym(void *module, const char *fn)
{
    void *addr = process_get_address(module, fn);
    if (!addr)
        log_error("Can't load PipeWire procedure '%s'.", fn);
    return addr;
}

FN_SOMA_DEVICE_QUERY(pipewire)
{
    (void)soma;
    (void)default_playback;
    (void)default_recording;
}

FN_SOMA_DEVICE_OPEN(pipewire)
{
    (void)soma;
    (void)memory;
    (void)out_device;
    return result_error;
}

FN_SOMA_DEVICE_CLOSE(pipewire)
{
    (void)device;
}

FN_SOMA_DEVICE_WAIT(pipewire)
{
    (void)device;
    return false;
}

static struct soma *g_pipewire = NULL;

static void _soma_pipewire_fini(struct soma *soma)
{
    soma->pw_deinit();
    zerop(soma);
    g_pipewire = NULL;
}

RIVEN_ENCORE(soma, pipewire)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);

    const char *fn = "'soma_encore_pipewire'";

    /* we allow only one pipewire backend at a time, so the interface will be shared */
    if (UNLIKELY(g_pipewire != NULL)) {
        *encore->header.interface = (riven_argument_t)g_pipewire;
        return;
    }

    void *module_pipewire = process_load_dll("libpipewire-0.3.so");
    if (!module_pipewire) {
        log_error("%s can't open pipewire-0.3 library.", fn);
        return;
    }

    PFN_pw_get_library_version _pw_get_library_version = (PFN_pw_get_library_version)pipewire_dlsym(module_pipewire, "pw_get_library_version");
    PFN_pw_init _pw_init = (PFN_pw_init)pipewire_dlsym(module_pipewire, "pw_init");
    if (!_pw_get_library_version || !_pw_init) return;

    const char *pipewire_version = _pw_get_library_version();
    s32 pipewire_version_major, pipewire_version_minor, pipewire_version_patch;
    s32 nargs = sscanf(pipewire_version, "%d.%d.%d", &pipewire_version_major, &pipewire_version_minor, &pipewire_version_patch);
    if (nargs < 3) {
        log_error("%s an unsupported PipeWire version of %s.", fn, pipewire_version);
        return;
    }
    _pw_init(NULL, NULL);

    struct riven *riven = encore->header.riven;
    riven_tag_t tag = encore->header.tag;

    struct soma *soma = (struct soma *)riven_alloc(riven, tag, sizeof(struct soma), _Alignof(struct soma));
    zerop(soma);

    soma->pipewire_version_major = pipewire_version_major;
    soma->pipewire_version_minor = pipewire_version_minor;
    soma->pipewire_version_patch = pipewire_version_patch;

    soma->module_pipewire = module_pipewire;
    soma->pw_get_library_version = _pw_get_library_version;
    soma->pw_init = _pw_init;
    soma->pw_deinit = (PFN_pw_deinit)pipewire_dlsym(module_pipewire, "pw_deinit");
    soma->pw_loop_new = (PFN_pw_loop_new)pipewire_dlsym(module_pipewire, "pw_loop_new");           
    soma->pw_loop_destroy = (PFN_pw_loop_destroy)pipewire_dlsym(module_pipewire, "pw_loop_destroy");
    soma->pw_loop_set_name = (PFN_pw_loop_set_name)pipewire_dlsym(module_pipewire, "pw_loop_set_name");
    soma->pw_context_new = (PFN_pw_context_new)pipewire_dlsym(module_pipewire, "pw_context_new");
    soma->pw_context_destroy = (PFN_pw_context_destroy)pipewire_dlsym(module_pipewire, "pw_context_destroy");
    soma->pw_context_connect = (PFN_pw_context_connect)pipewire_dlsym(module_pipewire, "pw_context_connect");
    soma->pw_proxy_add_object_listener = (PFN_pw_proxy_add_object_listener)pipewire_dlsym(module_pipewire, "pw_proxy_add_object_listener");
    soma->pw_proxy_get_user_data = (PFN_pw_proxy_get_user_data)pipewire_dlsym(module_pipewire, "pw_proxy_get_user_data");
    soma->pw_proxy_destroy = (PFN_pw_proxy_destroy)pipewire_dlsym(module_pipewire, "pw_proxy_destroy");
    soma->pw_core_disconnect = (PFN_pw_core_disconnect)pipewire_dlsym(module_pipewire, "pw_core_disconnect");
    soma->pw_stream_new_simple = (PFN_pw_stream_new_simple)pipewire_dlsym(module_pipewire, "pw_stream_new_simple");
    soma->pw_stream_destroy = (PFN_pw_stream_destroy)pipewire_dlsym(module_pipewire, "pw_stream_destroy");
    soma->pw_stream_connect = (PFN_pw_stream_connect)pipewire_dlsym(module_pipewire, "pw_stream_connect");
    soma->pw_stream_get_state = (PFN_pw_stream_get_state)pipewire_dlsym(module_pipewire, "pw_stream_get_state");
    soma->pw_stream_dequeue_buffer = (PFN_pw_stream_dequeue_buffer)pipewire_dlsym(module_pipewire, "pw_stream_dequeue_buffer");
    soma->pw_stream_queue_buffer = (PFN_pw_stream_queue_buffer)pipewire_dlsym(module_pipewire, "pw_stream_queue_buffer");
    soma->pw_properties_new = (PFN_pw_properties_new)pipewire_dlsym(module_pipewire, "pw_properties_new");
    soma->pw_properties_set = (PFN_pw_properties_set)pipewire_dlsym(module_pipewire, "pw_properties_set");
    soma->pw_properties_setf = (PFN_pw_properties_setf)pipewire_dlsym(module_pipewire, "pw_properties_setf");

    soma->interface = (struct soma_interface){
        .header = {
            .name = str_init("soma_pipewire"),
            .riven = riven,
            .tag = tag,
            .fini = (PFN_riven_work)_soma_pipewire_fini,
        },
#define WRITE_PFN(fun) \
        .fun = _soma_pipewire_##fun,
        WRITE_PFN(device_query)
        WRITE_PFN(device_open)
        WRITE_PFN(device_close)
        WRITE_PFN(device_wait)
#undef WRITE_PFN
    };
    *encore->header.interface = (riven_argument_t)soma;
    log_verbose("'%s' interface write.", soma->interface.header.name.ptr);
}
