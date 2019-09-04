#pragma once

#include "../../SwarmNet/src/swarmnet.h"

#include <stdlib.h>
#include <stdio.h>

class SwarmOS {
    public:
        Swarmnet * get_swarmnet();

        #if FUNC
        void set_common_sys_get_clock(std::function<unsigned int()> get_clock);
        void set_common_sys_lock(std::function<void()> lock);
        void set_common_sys_unlock(std::function<void()> unlock);
        #else
        void set_common_sys_get_clock(unsigned int(*get_clock)());
        void set_common_sys_lock(void(*lock)());
        void set_common_sys_unlock(void(*unlock)());
        #endif

        SwarmOS();
        ~SwarmOS();
    
    private:
        Common_system common_sys;
        Swarmnet swarmnet;
};