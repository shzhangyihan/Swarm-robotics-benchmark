#pragma once

#include "../../SwarmOS/src/swarmos.h"
#include "control_driver.h"

#define START_USER_PROGRAM
#define END_USER_PROGRAM

//unsigned char packet_level_message[PKT_SIZE];
SwarmOS swarmos;
Swarmnet *swarmnet;
Motor_control_unit *motor_control;
LED_control_unit *LED_control;
My_control_factory my_control_factory;

int driver_setup();
unsigned int get_clock();
void driver_loop();
int pull_packet(unsigned char * packet);
void packet_receive(unsigned char *packet_received, float distance);

#include "user_program.h"