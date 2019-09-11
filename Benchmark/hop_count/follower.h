#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber;
Channel *channel2;
Publisher *publisher2;
Subscriber *subscriber2;
unsigned char message[1];
int hop1;
int hop2;

void sent() {
    publisher->send(message, strlen((char*)message));
}
void sent2() {
    publisher2->send(message, strlen((char*)message));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
	if (hop < hop1) hop1 = hop;
}
void call_back2(unsigned char * msg, int size, int hop, Meta_t * meta) {
	if (hop < hop2) hop2 = hop;
}

void loop() {
	if (hop1 == 0) set_color(RGB(1,0,0));
	else if (hop1 == 1) set_color(RGB(1,1,0));
	else if (hop1 == 2) set_color(RGB(1,0,1));
	else if (hop1 == 3) set_color(RGB(0,1,1));
	else if (hop1 == 4) set_color(RGB(0,0,1));
}

void setup() {
    message[0] = 0;
    channel = swarmnet->new_channel(0, 4, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    channel2 = swarmnet->new_channel(2, 4, false);
    publisher2 = channel2->new_publisher(sent2);
    subscriber2 = channel2->new_subscriber(100, call_back2);
    publisher->send(message, 1);
    publisher2->send(message, 1);
    set_color(RGB(0,0,0));
    hop1 = 10;
    hop2 = 20;
    loop();
}

END_USER_PROGRAM