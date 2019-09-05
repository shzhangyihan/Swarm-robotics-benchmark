#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber;
unsigned char message[1];

void sent() {
    publisher->send(message, strlen((char*)message));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {

}

void loop() {
}

void setup() {
    message[0] = 0;
    channel = swarmnet->new_channel(2, 4, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(message, 1);
    set_color(RGB(1,0,0));
}

END_USER_PROGRAM