#pragma once

#include "../../SwarmNet/src/swarmnet.h"

class Control_factory {
    public:
        virtual void update_control() = 0;
        virtual void register_common_sys(Common_system * common_sys) = 0;

    protected:
        Common_system * common_sys;
};

class Control_unit {
    public:
        virtual void update_control() = 0;
        virtual void register_common_sys(Common_system * common_sys) = 0;
};