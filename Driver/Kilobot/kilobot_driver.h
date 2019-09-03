#pragma once
#define DEBUG

extern "C" {
    #include "../../../Swarm-platforms/kilobot/kilolib.h"
    #include "../../../Swarm-platforms/kilobot/debug.h"
}
#include "../../SwarmOS/src/swarmos.h"

#define START_USER_PROGRAM
#define END_USER_PROGRAM int main() { \
                             kilo_init(); \
                             debug_init(); \
                             kilo_message_tx = message_tx; \
                             kilo_message_tx_success = message_tx_success;\
                             kilo_message_rx = message_rx; \
                             kilo_start(setup, loop); \
                             return 0; \
                         }

message_t message;
SwarmOS swarmos;
Swarmnet * swarmnet = swarmos.get_swarmnet();

void message_tx_success() { }

message_t *message_tx() {
    message.type = NORMAL;
    int ret = swarmnet->next_pkt(message.data);
    message.crc = message_crc(&message);
    if(ret == 0) return NULL;
    else return &message;
}

void message_rx(message_t *message, distance_measurement_t *distance_measurement) {
    // Set the flag on message reception.
    int dist = estimate_distance(distance_measurement);
    //printf("P Recv dist = %d, theta = %f\n", dist, t);
    Meta_t meta;
    meta.dist = dist;
    swarmnet->receive(message->data, PKT_SIZE, &meta);
}