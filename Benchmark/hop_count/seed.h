#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
unsigned char message[1];

void sent() {
    publisher->send(message, strlen((char*)message));
}

void loop() {
}

void setup() {
    message[0] = 0;
    channel = swarmnet->new_channel(2, 4, false);
    publisher = channel->new_publisher(sent);
    publisher->send(message, 1);
    set_color(RGB(0,0,1));
}

END_USER_PROGRAM
