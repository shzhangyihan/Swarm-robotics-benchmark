#pragma once

#include "../../SwarmOS/src/swarmos.h"
#include "control_driver.h"
#include "python_driver.h"

#define START_USER_PROGRAM
#define END_USER_PROGRAM int main() { \
                             swarmos.set_common_sys_get_clock(get_clock); \
                             swarmos.register_user_loop(loop); \
                             swarmos.register_control_factory(&my_control_factory); \
                             motor_control = (Motor_control_unit *) my_control_factory.get_control_unit(0); \
                             LED_control = (LED_control_unit *) my_control_factory.get_control_unit(1); \
                             swarmnet = swarmos.get_swarmnet(); \
                             setup(); \
                             coach_start(); \
                             return 0; \
                         }

unsigned char packet_level_message[PKT_SIZE];
SwarmOS swarmos;
Swarmnet * swarmnet;
Motor_control_unit * motor_control;
LED_control_unit * LED_control;
My_control_factory my_control_factory;

unsigned int get_clock() {
    return python_get_clock();
}

void driver_loop() {
    swarmos.execute_loop();
}

unsigned char * pull_packet() {
    int ret = swarmnet->next_pkt(packet_level_message);
    if(ret == 0) return NULL;
    else return packet_level_message;
}

void packet_receive(unsigned char * packet_received, float distance) {
    Meta_t meta;
    meta.dist = distance;
    swarmnet->receive(packet_received, PKT_SIZE, &meta);
}

