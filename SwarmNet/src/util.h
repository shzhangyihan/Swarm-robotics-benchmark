#pragma once

#include "macro.h"

#if FUNC
#include <functional>
#endif

namespace common {
    unsigned int decode(unsigned char * header, unsigned int offset, unsigned int len);
    void encode(unsigned char * header, unsigned int offset, unsigned int len, unsigned int data);
    unsigned int clock_diff(unsigned int t1, unsigned int t2);
    
    void dummy_lock();
    unsigned int dummy_clock();
}

class Common_system {
    public:
        Common_system();
        #if FUNC
        std::function<unsigned int()> get_clock;
        std::function<void()> lock;
        std::function<void()> unlock;
        #else
        void(*lock)();
        void(*unlock)();
        unsigned int(*get_clock)();
        #endif
};