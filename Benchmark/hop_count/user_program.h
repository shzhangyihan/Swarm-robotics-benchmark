#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char msg1[10];

int minhop1 = -1;
int minhop2 = -1;

void sent() {
    publisher->send(msg1, strlen((char*)msg1));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    if (msg[0] == 'A'){
        if (hop > minhop1){
            minhop1 = hop;
        }  
    }
    if (msg[0] == 'B'){
        if (hop > minhop2){
            minhop2 = hop;
        }  
    }
} 

void loop() {
    if (id == 3) {
        strcpy((char*)msg1, "A");
        set_color(RGB(1,1,1));
    }
    else if (id == 4){
        strcpy((char*)msg1, "B");
        set_color(RGB(1,1,1));
    }
    else {
        if (minhop1 == 4 && minhop2 == 1){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 3 && minhop2 == 1){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 2 && minhop2 == 1){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 2 && minhop2 == 2){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 2 && minhop2 == 3){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 2 && minhop2 == 4){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 1 && minhop2 == 4){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 1 && minhop2 == 3){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 0 && minhop2 == 3){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 0 && minhop2 == 2){
            set_color(RGB(1,0,1));
        }
        else if (minhop1 == 0 && minhop2 == 1){
            set_color(RGB(1,0,1));
        }
        else set_color(RGB(0,0,0));
    }
}

void setup() {
    strcpy((char*)msg1, "H");
    channel = swarmnet->new_channel(2, 4, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(msg1, strlen((char*)msg1));
    set_color(RGB(0,0,1));
}

END_USER_PROGRAM
