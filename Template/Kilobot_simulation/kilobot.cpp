#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

class mykilobot : public kilobot_driver {
    Channel *channel;
    Publisher *publisher;
    Subscriber *subscriber; 
    unsigned char msg1[10];

    void sent() {
        publisher->send(msg1, strlen((char*)msg1));
    }
    
    void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
        printf("M Recv dist = %d, theta = %f\n", meta->dist, meta->theta);
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
};
