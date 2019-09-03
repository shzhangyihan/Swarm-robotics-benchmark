#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char msg1[20];

void sent() {
    publisher->send(msg1, strlen((char*)msg1));
    set_color(RGB(1, 0, 0));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("M Recv dist = %d, size = %d\n", meta->dist, size);
    for(int i = 0; i < size; i++) printf("%c", msg+i);
    printf("\n");
    set_color(RGB(0, 1, 0));
} 

void loop() {
    
}

void setup() {
    strcpy((char*)msg1, "Hello world!!!");
    channel = swarmnet.new_channel(2, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(msg1, strlen((char*)msg1));
    set_color(RGB(0, 0, 1));
}

END_USER_PROGRAM