#pragma once

#include "../../SwarmNet/src/swarmnet.h"
#include "control_factory.h"
#include <stdlib.h>
#include <stdio.h>

class SwarmOS {
    public:
        Swarmnet * get_swarmnet();

        unsigned int get_clock();
        unsigned int random_func();
        void lock();
        void unlock();
        void execute_loop();
        void register_control_factory(Control_factory * control_factory);

        #if FUNC
        void register_user_loop(std::function<void()> user_loop);
        void set_common_sys_get_clock(std::function<unsigned int()> get_clock);
        void set_common_sys_random_func(std::function<unsigned int()> random_func);
        void set_common_sys_lock(std::function<void()> lock);
        void set_common_sys_unlock(std::function<void()> unlock);
        #else
        void register_user_loop(void(*user_loop)());
        void set_common_sys_get_clock(unsigned int(*get_clock)());
        void set_common_sys_random_func(unsigned int(*random_func)());
        void set_common_sys_lock(void(*lock)());
        void set_common_sys_unlock(void(*unlock)());
        #endif

        SwarmOS();
        ~SwarmOS();
    
    private:
        Common_system common_sys;
        Control_factory * control_factory;
        Swarmnet swarmnet;
        #if FUNC
        std::function<void()> user_loop;
        #else
        void(*user_loop)();
        #endif
};