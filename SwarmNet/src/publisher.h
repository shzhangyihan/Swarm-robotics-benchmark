#pragma once
#include "channel.h"
#include "util.h"

#if FUNC
#include <functional>
#endif

#if PYTHON
typedef void (*send_caller)(void *user_func);
#endif

class Publisher {
    public:
        int send(unsigned char * msg, int msgSize);
        bool available();

        Publisher();
        
        #if PYTHON
        Publisher(Channel * chan, send_caller caller, void * usr_f);
        void init(Channel * chan, send_caller caller, void * usr_f);
        #else
        #if FUNC
        Publisher(Channel * chan, std::function<void()> callback);
        void init(Channel * chan, std::function<void()> callback);
        #else
        Publisher(Channel * chan, void (*callback)());
        void init(Channel * chan, void (*callback)());
        #endif
        #endif

    private:
        Channel * chan;
};
