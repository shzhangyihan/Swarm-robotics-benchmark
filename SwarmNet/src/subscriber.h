#pragma once

#include <stdio.h>
#include "util.h"
#include <stddef.h>

#if FUNC
#include <functional>
#endif

class Subscriber {
    public:
        void receive(unsigned char * msg, int size, int hop, Meta_t * meta);
        bool if_initialized();

        Subscriber();

        #if FUNC
        Subscriber(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback);
        void subscriber_init(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback);
        #else
        Subscriber(int dist, void (*callback)(unsigned char *, int, int, Meta_t *));
        void subscriber_init(int dist, void (*callback)(unsigned char *, int, int, Meta_t *));
        #endif

    private:
        int dist;
        #if FUNC
        std::function<void (unsigned char *, int, int, Meta_t *)> callback;
        #else
        void (*callback)(unsigned char *, int, int, Meta_t *);
        #endif
};
