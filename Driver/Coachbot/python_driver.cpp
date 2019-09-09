#pragma once

#include "python_driver.h"
#include "coach_driver.h"

void python_register_get_clock(get_clock_caller caller, void * usr_f) {
    my_get_clock_caller = caller;
    get_clock_caller_func = usr_f;
}

void python_register_set_color(set_color_caller caller, void * usr_f) {
    my_set_color_caller = caller;
    set_color_caller_func = usr_f;
}

void python_register_set_motors(set_motors_caller caller, void * usr_f) {
    my_set_motors_caller = caller;
    set_motors_caller_func = usr_f;
}

void python_call_driver_loop() {
    driver_loop();
}

void python_call_driver_setup() {
    driver_loop();
}

unsigned int python_get_clock() {
    return (unsigned int) my_get_clock_caller(get_clock_caller_func);
}

void python_set_color(int red, int green, int blue) {
    my_set_color_caller(set_color_caller_func, red, green, blue);
}

void python_set_motors(int left, int right) {
    my_set_motors_caller(set_motors_caller_func, left, right);
}

int python_pull_packet(unsigned char * packet) {
    return pull_packet(packet);
}

void python_packet_receive(unsigned char *packet_received, float distance) {
    packet_receive(packet_received, distance);
}