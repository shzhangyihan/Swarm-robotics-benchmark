#pragma once

#include "coach_driver.h"

int driver_setup() {
    swarmos.set_common_sys_get_clock(get_clock);
    swarmos.register_user_loop(loop);
    swarmos.register_control_factory(&my_control_factory);
    motor_control =
        (Motor_control_unit *)my_control_factory.get_control_unit(0);
    LED_control = (LED_control_unit *)my_control_factory.get_control_unit(1);
    swarmnet = swarmos.get_swarmnet();
    setup();
    return 0;
}

unsigned int get_clock() { return python_get_clock(); }

void driver_loop() { swarmos.execute_loop(); }

int pull_packet(unsigned char *packet) { return swarmnet->next_pkt(packet); }

void packet_receive(unsigned char *packet_received, float distance) {
    Meta_t meta;
    meta.dist = distance;
    swarmnet->receive(packet_received, PKT_SIZE, &meta);
}

void python_register_get_clock(get_clock_caller caller, void *usr_f) {
    my_get_clock_caller = caller;
    get_clock_caller_func = usr_f;
}

void python_register_set_color(set_color_caller caller, void *usr_f) {
    my_set_color_caller = caller;
    set_color_caller_func = usr_f;
}

void python_register_set_motors(set_motors_caller caller, void *usr_f) {
    my_set_motors_caller = caller;
    set_motors_caller_func = usr_f;
}

void python_call_driver_loop() { driver_loop(); }

void python_call_driver_setup() { driver_loop(); }

unsigned int python_get_clock() {
    return (unsigned int)my_get_clock_caller(get_clock_caller_func);
}

void python_set_color(int red, int green, int blue) {
    my_set_color_caller(set_color_caller_func, red, green, blue);
}

void python_set_motors(int left, int right) {
    my_set_motors_caller(set_motors_caller_func, left, right);
}

int python_pull_packet(unsigned char *packet) { return pull_packet(packet); }

void python_packet_receive(unsigned char *packet_received, float distance) {
    packet_receive(packet_received, distance);
}

void Motor_control_unit::update_control() {
    if (this->common_sys->get_clock == NULL) return;
    if (this->status == Stop || this->time_left == 0) return;
    unsigned long cur_time = this->common_sys->get_clock();
    if (cur_time - this->start_time > this->time_left) {
        status = Stop;
        time_left = 0;
        python_set_motors(0, 0);
    }
}

void Motor_control_unit::move_forward(unsigned long time) {
    if (this->common_sys->get_clock == NULL) return;
    this->start_time = (this->common_sys)->get_clock();
    this->status = Move_forward;
    this->time_left = time;
    python_set_motors(1, 1);
}

void Motor_control_unit::turn_left(unsigned long time) {
    if (this->common_sys->get_clock == NULL) return;
    this->start_time = this->common_sys->get_clock();
    this->status = Turn_left;
    this->time_left = time;
    python_set_motors(1, 0);
}

void Motor_control_unit::turn_right(unsigned long time) {
    if (this->common_sys->get_clock == NULL) return;
    this->start_time = this->common_sys->get_clock();
    this->status = Turn_right;
    this->time_left = time;
    python_set_motors(0, 1);
}

void Motor_control_unit::stop_motor() {
    if (this->common_sys->get_clock == NULL) return;
    this->start_time = this->common_sys->get_clock();
    this->status = Stop;
    this->time_left = 0;
    python_set_motors(0, 0);
}

int Motor_control_unit::current_status() { return this->status; }

void Motor_control_unit::register_common_sys(Common_system *common_sys) {
    this->common_sys = common_sys;
}

Motor_control_unit::Motor_control_unit() {
    time_left = 0;
    status = Stop;
}

Motor_control_unit::~Motor_control_unit() {}

void LED_control_unit::update_control() {
    if (this->common_sys->get_clock == NULL) return;
    if (this->status == Stop || this->time_left == 0) return;
    unsigned long cur_time = this->common_sys->get_clock();
    if (cur_time - this->start_time > this->time_left) {
        status = Off;
        time_left = 0;
        python_set_color(0, 0, 0);
        ;
    }
}

void LED_control_unit::turn_on(int red, int green, int blue, int time) {
    if (this->common_sys->get_clock == NULL) return;
    this->start_time = (this->common_sys)->get_clock();
    this->status = On;
    this->time_left = time;
    python_set_color(red, green, blue);
    ;
}

int LED_control_unit::current_status() { return this->status; }

void LED_control_unit::register_common_sys(Common_system *common_sys) {
    this->common_sys = common_sys;
}

LED_control_unit::LED_control_unit() {
    time_left = 0;
    status = Off;
}

LED_control_unit::~LED_control_unit() {}

void My_control_factory::update_control() {
    for (int i = 0; i < NUM_OF_CONTROL_UNITS; i++) {
        if (my_control_unit[i] != NULL) {
            my_control_unit[i]->update_control();
        }
    }
}

Control_unit *My_control_factory::get_control_unit(int index) {
    return my_control_unit[index];
}

void My_control_factory::register_common_sys(Common_system *common_sys) {
    this->common_sys = common_sys;
    for (int i = 0; i < NUM_OF_CONTROL_UNITS; i++) {
        my_control_unit[i]->register_common_sys(this->common_sys);
    }
}

My_control_factory::My_control_factory() {
    my_control_unit[0] = &my_motor_control;
    my_control_unit[1] = &my_LED_control;
}

My_control_factory::~My_control_factory() {}
