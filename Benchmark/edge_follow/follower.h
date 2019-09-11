#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

#define ACCEPTABLE_MAX 55
#define ACCEPTABLE_MIN 35

typedef struct custom_message {
    int id;
} custom_message_t;

Channel *channel;
Publisher *publisher;
Subscriber *subscriber;
custom_message_t my_message;

int nearest_distance;
int attached_robot;

void sent() {
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    custom_message_t * received_msg = (custom_message_t *) msg;
    int robot_id = received_msg->id;
    if (robot_id == attached_robot){
        nearest_distance = meta->dist;
    }
    else {
        if (meta->dist < nearest_distance){
            attached_robot = robot_id;
            nearest_distance = meta->dist;
        }
    }
}

void loop() {
    LED_control->turn_on(0, 1, 0, 200);
    if(motor_control->current_status() == Stop) {
        if (nearest_distance < ACCEPTABLE_MIN){
            motor_control->turn_left(1);
        }
        else if (nearest_distance > ACCEPTABLE_MAX){
            motor_control->turn_right(1);
        }
        else {
            motor_control->move_forward(1);
        }
    }
}

void setup() {
    nearest_distance = 255;
    attached_robot = 0;

    my_message.id = 0;
    channel = swarmnet->new_channel(1, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
    LED_control->turn_on(0, 1, 0, 200);
}

END_USER_PROGRAM