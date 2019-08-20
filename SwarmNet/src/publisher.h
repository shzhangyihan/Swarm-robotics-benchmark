#pragma once
#include "channel.h"
#include "util.h"

#if FUNC
#include <functional>
#endif

class Publisher {
    public:
        int send(unsigned char * msg, int msgSize);
        bool available();

        Publisher();
        
        #if FUNC
        Publisher(Channel * chan, std::function<void()> callback);
        void publisher_init(Channel * chan, std::function<void()> callback);
        #else
        Publisher(Channel * chan, void (*callback)());
        void publisher_init(Channel * chan, void (*callback)());
        #endif

    private:
        Channel * chan;
};
