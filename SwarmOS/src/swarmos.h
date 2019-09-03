#pragma once

#include "../../SwarmNet/src/swarmnet.h"

#include <stdlib.h>
#include <stdio.h>

class SwarmOS {
    public:
        Swarmnet * get_swarmnet();
        SwarmOS();
        ~SwarmOS();
    
    private:
        Swarmnet swarmnet;
};