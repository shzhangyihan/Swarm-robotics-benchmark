#pragma once

#include "coach_driver.h"
#include "python_driver.h"

int driver_setup() {
    swarmos.set_common_sys_get_clock(get_clock);
    swarmos.register_user_loop(loop);
    swarmos.register_control_factory(&my_control_factory);
    motor_control = (Motor_control_unit *)my_control_factory.get_control_unit(0);
    LED_control = (LED_control_unit *)my_control_factory.get_control_unit(1);
    swarmnet = swarmos.get_swarmnet();
    setup();
    return 0;
}

unsigned int get_clock() {
    return python_get_clock();
}

void driver_loop() {
    swarmos.execute_loop();
}

int pull_packet(unsigned char * packet) {
    return swarmnet->next_pkt(packet);
}

void packet_receive(unsigned char *packet_received, float distance) {
    Meta_t meta;
    meta.dist = distance;
    swarmnet->receive(packet_received, PKT_SIZE, &meta);
}
