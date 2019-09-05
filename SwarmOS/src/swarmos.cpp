#include "swarmos.h"

Swarmnet * SwarmOS::get_swarmnet() {
    swarmnet.set_common_sys(&common_sys);
    return &(this->swarmnet);
}

SwarmOS::SwarmOS() {}

SwarmOS::~SwarmOS() {}

#if FUNC
void SwarmOS::set_common_sys_get_clock(std::function<unsigned int()> get_clock) {
    common_sys.get_clock = get_clock;
}

void SwarmOS::set_common_sys_random_func(std::function<unsigned int()> random_func) {
    common_sys.random_func = random_func;
}

void SwarmOS::set_common_sys_lock(std::function<void()> lock) {
    common_sys.lock = lock;
}

void SwarmOS::set_common_sys_unlock(std::function<void()> unlock) {
    common_sys.unlock = unlock;
}
#else
void SwarmOS::set_common_sys_get_clock(unsigned int(*get_clock)()) {
    common_sys.get_clock = get_clock;
}

void SwarmOS::set_common_sys_random_func(unsigned int(*random_func)()) {
    common_sys.random_func = random_func;
}

void SwarmOS::set_common_sys_lock(void(*lock)()) {
    common_sys.lock = lock;
}

void SwarmOS::set_common_sys_unlock(void(*unlock)()) {
    common_sys.unlock = unlock;
}
#endif