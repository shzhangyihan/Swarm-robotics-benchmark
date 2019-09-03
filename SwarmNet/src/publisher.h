#pragma once
//#include "channel.h"
#include "util.h"

#if FUNC
#include <functional>
#endif

class Publisher {
    public:
        int send(unsigned char * msg, int msgSize);
        void sent_callback();
        bool available();
        bool if_initialized();

        Publisher();
        
        #if FUNC
        Publisher(void * chan, std::function<void()> callback);
        void publisher_init(void * chan, std::function<void()> callback);
        #else
        Publisher(void * chan, void (*callback)());
        void publisher_init(void * chan, void (*callback)());
        #endif

    private:
        void * chan;
        #if FUNC
        std::function<void()> callback;
        #else
        void (*callback)();
        #endif
};
