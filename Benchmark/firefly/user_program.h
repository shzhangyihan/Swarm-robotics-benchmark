#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

#define TIMER_DIVISION_CONSTANT 100
#define TIMER_MAX 500

typedef struct custom_message {
    bool firing;
} custom_message_t;

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
custom_message_t my_message;

void sent() {
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    custom_message_t * received_msg = (custom_message_t *) msg;
    if (received_msg->firing == true){
        timer += (timer/TIMER_DIVISION_CONSTANT) * (timer/TIMER_DIVISION_CONSTANT);
    }
} 

void loop() {
    if(LED_control->current_status() == Off) {
        if (timer >= TIMER_MAX){
            timer = 0;
            my_message.firing = true;
            LED_control->turn_on(1, 1, 1, 25);
        }
        else {
            my_message.firing = false;
            LED_control->turn_on(0, 0, 1, 1);
        }
    }
}

void setup() {
    my_message.firing = false;
    channel = swarmnet->new_channel(1, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
    LED_control->turn_on(0, 0, 1, 1);
}

END_USER_PROGRAM