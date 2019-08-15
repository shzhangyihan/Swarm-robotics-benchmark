#pragma once

#include <stdio.h>
#include "util.h"
#include <stddef.h>

#if FUNC
#include <functional>
#endif

#if PYTHON
typedef void (*recv_caller)(void *user_func, unsigned char * msg, int size, int hop, Meta_t * meta);
#endif

class Subscriber {
    public:
        void receive(unsigned char * msg, int size, int hop, Meta_t * meta);
        
        Subscriber();

        #if PYTHON
        Subscriber(int dist, recv_caller caller, void * usr_f);
        void init(int dist, recv_caller caller, void * usr_f);
        #else
        #if FUNC
        Subscriber(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback);
        void init(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback);
        #else
        Subscriber(int dist, void (*callback)(unsigned char *, int, int, Meta_t *));
        void init(int dist, void (*callback)(unsigned char *, int, int, Meta_t *));
        #endif
        #endif

    private:
        int dist;
        #if PYTHON
        recv_caller caller;
        void * usr_f;
        #else
        #if FUNC
        std::function<void (unsigned char *, int, int, Meta_t *)> callback;
        #else
        void (*callback)(unsigned char *, int, int, Meta_t *);
        #endif
        #endif
};
