#include "swarmos.h"

Swarmnet * SwarmOS::get_swarmnet() {
    swarmnet.set_common_sys(&common_sys);
    return &(this->swarmnet);
}

SwarmOS::SwarmOS() {
    this->control_factory = NULL;
    this->user_loop = NULL;
}

SwarmOS::~SwarmOS() {}

unsigned int SwarmOS::get_clock() {
    return common_sys.get_clock();
}

unsigned int SwarmOS::random_func() {
    return common_sys.random_func();
}

void SwarmOS::lock() {
    common_sys.lock();
}

void SwarmOS::unlock() {
    common_sys.unlock();
}

void SwarmOS::execute_loop() {
    if(control_factory == NULL) return;
    control_factory->update_control();
    if(this->user_loop == NULL) {
    }
    else {
        this->user_loop();
    }
}

void SwarmOS::register_control_factory(Control_factory * control_factory) {
    this->control_factory = control_factory;
    this->control_factory->register_common_sys(&(this->common_sys));
}

#if FUNC
void SwarmOS::register_user_loop(std::function<void()> user_loop) {
    this->user_loop = user_loop;
}

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
void SwarmOS::register_user_loop(void(*user_loop)()) {
    this->user_loop = user_loop;
}

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