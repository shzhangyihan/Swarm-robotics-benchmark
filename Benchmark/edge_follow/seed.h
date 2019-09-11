#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

typedef struct custom_message {
    int id;
} custom_message_t;

Channel *channel;
Publisher *publisher;
Subscriber *subscriber;
custom_message_t my_message;

void sent() {
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
}


void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
}

void loop() {
    LED_control->turn_on(1, 1, 1, 200);
    my_message.id = id;
}

void setup() {
    my_message.id = id;
    channel = swarmnet->new_channel(1, 0, false);
    publisher = channel->new_publisher(sent);
    publisher->send((unsigned char *) &my_message, sizeof(my_message));
    LED_control->turn_on(1, 1, 1, 200);
}

END_USER_PROGRAM