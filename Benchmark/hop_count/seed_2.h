#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

#define LED_DURATION 200

typedef struct custom_message {
    bool is_seed;
    int hop;
} custom_message_t;

Channel * channel_seed_2;
Publisher * publisher;
custom_message_t my_message;

void sent() {
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
}

void loop() {
    LED_control->turn_on(1, 1, 1, LED_DURATION);
}

void setup() {
    my_message.hop = 0;
    my_message.is_seed = true;
    channel_seed_2 = swarmnet->new_channel(2, 0, false);
    publisher = channel_seed_2->new_publisher(sent);
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
    LED_control->turn_on(1, 1, 1, LED_DURATION);
}

END_USER_PROGRAM
