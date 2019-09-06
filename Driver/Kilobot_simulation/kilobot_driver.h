#pragma once
#include "../../../Swarm-platforms/simulation/src/kilolib.h"
#include "../../SwarmOS/src/swarmos.h"
#include "std_macro.h"
#include "control_driver.h"

#define START_USER_PROGRAM class CLASS : public kilobot_driver {
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
        Motor_control_unit * motor_control;
        LED_control_unit * LED_control;
        My_control_factory my_control_factory;

        //executed on successfull message send
        void message_tx_success() { }
        
        //sends message at fixed rate
        message_t *message_tx() {
            message.type = NORMAL;
            int ret = swarmnet->next_pkt(message.data);
            message.crc = message_crc(&message);
            if(ret == 0) return NULL;
            static int r = rand() % 100;
            if(r <= 90) {
                return &message;
            }
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
        
        unsigned int get_clock() {
            return (unsigned int) kilo_ticks;
        }

        unsigned int custom_rand() {
            return rand();
        }

        void driver_loop() {
            swarmos.execute_loop();
        }

        virtual void loop() { }

        kilobot_driver() {
            swarmos.set_common_sys_get_clock(std::bind(&kilobot_driver::get_clock, this));
            swarmos.set_common_sys_random_func(std::bind(&kilobot_driver::custom_rand, this));
            swarmos.register_user_loop(std::bind(&kilobot_driver::loop, this));
            my_control_factory.register_this(this);
            swarmos.register_control_factory(&my_control_factory);
            motor_control = (Motor_control_unit *) my_control_factory.get_control_unit(0);
            LED_control = (LED_control_unit *) my_control_factory.get_control_unit(1);
            swarmnet = swarmos.get_swarmnet();
        }
};
