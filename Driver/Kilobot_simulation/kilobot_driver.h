#pragma once
#include "../../../Swarm-platforms/simulation/src/kilolib.h"
#include "../../SwarmOS/src/swarmos.h"
#include "std_macro.h"

#define START_USER_PROGRAM class mykilobot : public kilobot_driver {
#define END_USER_PROGRAM   };

/*----------------------------------------------------------------------*/
/*-----------------------------             ----------------------------*/
/*----------------------------- Driver code ----------------------------*/
/*-----------------------------             ----------------------------*/
/*----------------------------------------------------------------------*/

class kilobot_driver : public kilobot {
    public:
        message_t message;
        SwarmOS swarmos;
        Swarmnet * swarmnet;

        //executed on successfull message send
        void message_tx_success() { }
        
        //sends message at fixed rate
        message_t *message_tx() {
            message.type = NORMAL;
            int ret = swarmnet->next_pkt(message.data);
            message.crc = message_crc(&message);
            if(ret == 0) return NULL;
            static int r = rand() % 100;
            if(r <= 90) return &message;
            return NULL;
        }
        
        void message_rx(message_t *message, distance_measurement_t *distance_measurement,float t) {
            // Set the flag on message reception.
            int dist = estimate_distance(distance_measurement);
            //printf("P Recv dist = %d, theta = %f\n", dist, t);
            Meta_t meta;
            meta.dist = dist;
            swarmnet->receive(message->data, PKT_SIZE, &meta);
        }

        kilobot_driver() {
            swarmnet = swarmos.get_swarmnet();
        }
};
