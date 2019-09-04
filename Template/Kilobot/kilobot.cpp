#pragma once
#include "../../Swarm-robotics-benchmark/Driver/Kilobot/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char msg1[10];

void sent() {
    publisher->send(msg1, strlen((char*)msg1));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("M Recv dist = %d\n", meta->dist);
} 

void loop() {
    
}

void setup() {
    strcpy((char*)msg1, "H");
    channel = swarmnet.new_channel(2, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(msg1, strlen((char*)msg1));
    set_color(RGB(0,0,1));
}

END_USER_PROGRAM
