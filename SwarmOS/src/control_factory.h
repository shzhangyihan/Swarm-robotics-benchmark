#pragma once

#include "../../SwarmNet/src/swarmnet.h"

class Control_factory {
    public:
        virtual void update_control();
        void register_common_sys(Common_system * common_sys) {
            this->common_sys = common_sys;
        }

    protected:
        Common_system * common_sys;
};

class Control_unit {
    public:
        virtual void update_control();
};