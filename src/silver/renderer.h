#ifndef _SILVER_RENDERER_H
#define _SILVER_RENDERER_H

#include <lake/silver.h>

/** Internal rendering backend API, for calls from shared code. */
struct renderer_api {
    enum silver_backend id;

};

struct silver {
    struct renderer_api api;

    void *backend;
};

struct silvdevice {

};

#endif /* _SILVER_RENDERER_H */
