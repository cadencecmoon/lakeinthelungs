#include <amwe/soma.h>
#ifdef SOMA_PIPEWIRE

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

struct soma_device {
    struct soma_encore *encore;
};

struct soma_encore {
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

    s32                                 version_major;
    s32                                 version_minor;
    s32                                 version_patch;
};

static FN_SOMA_SINK_QUERY(pipewire)
{
    (void)pipewire;
    (void)default_playback;
    (void)default_recording;
}

static FN_SOMA_SINK_OPEN(pipewire)
{
    (void)pipewire;
    return NULL;
}

static FN_SOMA_SINK_CLOSE(pipewire)
{
    (void)sink;
}

static FN_SOMA_SINK_WAIT(pipewire)
{
    (void)sink;
    return false;
}

static bool load_pipewire_symbols(struct soma_encore *pipewire, const char *msg)
{
    /* should be already loaded */
    bedrock_assert_debug(pipewire->pw_get_library_version && pipewire->pw_init);

    void *module = pipewire->module_pipewire;

    pipewire->pw_deinit = (PFN_pw_deinit)
        bedrock_get_proc_address(module, "pw_deinit");
    pipewire->pw_loop_new = (PFN_pw_loop_new)
        bedrock_get_proc_address(module, "pw_loop_new");           
    pipewire->pw_loop_destroy = (PFN_pw_loop_destroy)
        bedrock_get_proc_address(module, "pw_loop_destroy");
    pipewire->pw_loop_set_name = (PFN_pw_loop_set_name)
        bedrock_get_proc_address(module, "pw_loop_set_name");
    pipewire->pw_context_new = (PFN_pw_context_new)
        bedrock_get_proc_address(module, "pw_context_new");
    pipewire->pw_context_destroy = (PFN_pw_context_destroy)
        bedrock_get_proc_address(module, "pw_context_destroy");
    pipewire->pw_context_connect = (PFN_pw_context_connect)
        bedrock_get_proc_address(module, "pw_context_connect");
    pipewire->pw_proxy_add_object_listener = (PFN_pw_proxy_add_object_listener)
        bedrock_get_proc_address(module, "pw_proxy_add_object_listener");
    pipewire->pw_proxy_get_user_data = (PFN_pw_proxy_get_user_data)
        bedrock_get_proc_address(module, "pw_proxy_get_user_data");
    pipewire->pw_proxy_destroy = (PFN_pw_proxy_destroy)
        bedrock_get_proc_address(module, "pw_proxy_destroy");
    pipewire->pw_core_disconnect = (PFN_pw_core_disconnect)
        bedrock_get_proc_address(module, "pw_core_disconnect");
    pipewire->pw_stream_new_simple = (PFN_pw_stream_new_simple)
        bedrock_get_proc_address(module, "pw_stream_new_simple");
    pipewire->pw_stream_destroy = (PFN_pw_stream_destroy)
        bedrock_get_proc_address(module, "pw_stream_destroy");
    pipewire->pw_stream_connect = (PFN_pw_stream_connect)
        bedrock_get_proc_address(module, "pw_stream_connect");
    pipewire->pw_stream_get_state = (PFN_pw_stream_get_state)
        bedrock_get_proc_address(module, "pw_stream_get_state");
    pipewire->pw_stream_dequeue_buffer = (PFN_pw_stream_dequeue_buffer)
        bedrock_get_proc_address(module, "pw_stream_dequeue_buffer");
    pipewire->pw_stream_queue_buffer = (PFN_pw_stream_queue_buffer)
        bedrock_get_proc_address(module, "pw_stream_queue_buffer");
    pipewire->pw_properties_new = (PFN_pw_properties_new)
        bedrock_get_proc_address(module, "pw_properties_new");
    pipewire->pw_properties_set = (PFN_pw_properties_set)
        bedrock_get_proc_address(module, "pw_properties_set");
    pipewire->pw_properties_setf = (PFN_pw_properties_setf)
        bedrock_get_proc_address(module, "pw_properties_setf");

    if (!pipewire->pw_deinit ||
        !pipewire->pw_loop_new ||
        !pipewire->pw_loop_destroy ||
        !pipewire->pw_loop_set_name ||
        !pipewire->pw_context_new ||
        !pipewire->pw_context_destroy ||
        !pipewire->pw_context_connect ||
        !pipewire->pw_proxy_add_object_listener ||
        !pipewire->pw_proxy_get_user_data ||
        !pipewire->pw_proxy_destroy ||
        !pipewire->pw_core_disconnect ||
        !pipewire->pw_stream_new_simple ||
        !pipewire->pw_stream_destroy ||
        !pipewire->pw_stream_connect ||
        !pipewire->pw_stream_get_state ||
        !pipewire->pw_stream_dequeue_buffer ||
        !pipewire->pw_stream_queue_buffer ||
        !pipewire->pw_properties_new ||
        !pipewire->pw_properties_set ||
        !pipewire->pw_properties_setf)
    {
        bedrock_log_debug("%s can't load PipeWire procedures.", msg);
        return false;
    }
    return true;
}

static struct soma_encore *g_pipewire = NULL;

static void pipewire_encore_fini(struct soma_encore *encore)
{
    if (!encore)
        return;

    if (encore->pw_deinit)
        encore->pw_deinit();
    if (encore->module_pipewire)
        bedrock_module_close(encore->module_pipewire);

    bedrock_zerop(encore);
    g_pipewire = NULL;
}

FN_RIVEN_ENCORE(soma, pipewire)
{
    const char *msg = "'soma: pipewire'";

    (void)metadata;
    (void)riven_hints;
    (void)userdata;

    /* we allow only one PipeWire backend instance to exist at a time */
    if (lake_unlikely(g_pipewire != NULL))
        return g_pipewire;

    void *module_pipewire = bedrock_module_open("libpipewire-0.3.so");
    if (!module_pipewire) {
        bedrock_log_debug("%s libpipewire-0.3.so is missing.", msg);
        return NULL;
    }

    PFN_pw_get_library_version get_library_version = (PFN_pw_get_library_version)
        bedrock_get_proc_address(module_pipewire, "pw_get_library_version");
    PFN_pw_init init = (PFN_pw_init)
        bedrock_get_proc_address(module_pipewire, "pw_init");
    if (!get_library_version || !init) {
        bedrock_log_debug("%s can't load PipeWire entry point procedures.", msg);
        bedrock_module_close(module_pipewire);
        return NULL;
    }

    const char *pipewire_version = get_library_version();
    s32 version_major, version_minor, version_patch;
    s32 nargs = sscanf(pipewire_version, "%d.%d.%d", &version_major, &version_minor, &version_patch);
    if (nargs < 3) {
        bedrock_log_debug("%s an unsupported PipeWire version of %s.", msg, pipewire_version);
        bedrock_module_close(module_pipewire);
        return NULL;
    }
    init(NULL, NULL);

    struct soma_encore *pipewire = (struct soma_encore *)
        riven_thalloc(riven, tag, lake_sizeof(struct soma_encore), lake_alignof(struct soma_encore));
    bedrock_zerop(pipewire);

    pipewire->module_pipewire = module_pipewire;
    pipewire->version_major = version_major;
    pipewire->version_minor = version_minor;
    pipewire->version_patch = version_patch;
    pipewire->pw_get_library_version = get_library_version;
    pipewire->pw_init = init;

    /* write the interface header */
    pipewire->interface.header.riven = riven;
    pipewire->interface.header.tag = tag;
    pipewire->interface.header.name = "soma";
    pipewire->interface.header.backend = "pipewire";
    pipewire->interface.header.encore_fini = (PFN_riven_work)pipewire_encore_fini;
    g_pipewire = pipewire;

    if (!load_pipewire_symbols(pipewire, msg)) {
        pipewire_encore_fini(pipewire);
        return NULL;
    }

    /* write the interface */
    pipewire->interface.sink_query = _soma_pipewire_sink_query;
    pipewire->interface.sink_open = _soma_pipewire_sink_open;
    pipewire->interface.sink_close = _soma_pipewire_sink_close;
    pipewire->interface.sink_wait = _soma_pipewire_sink_wait;
    return pipewire;
}

#endif /* SOMA_PIPEWIRE */
