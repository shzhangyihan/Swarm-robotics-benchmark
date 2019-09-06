#pragma once
#include "../../SwarmOS/src/swarmos.h"

#include "../../../Swarm-platforms/simulation/src/kilolib.h"

#define NUM_OF_CONTROL_UNITS 1

enum enumType {Stop, Move_forward, Turn_left, Turn_right};

class Motor_control_unit: public Control_unit {
    public:
        void update_control() {
            if(this->status == Stop || this->time_left == 0) return;
            unsigned long cur_time = common_sys->get_clock();
            if(cur_time - this->start_time > this->time_left) {
                printf("stop\n");
                status = Stop;
                time_left = 0;
                set_motors(0, 0);
            }
        }

        void move_forward(unsigned long time) {
            this->start_time = common_sys->get_clock();
            printf("Start moving forward from status %d to status %d for %d ticks\n", this->status, Move_forward, time);
            if(this->status == Stop) spinup_motors();
            this->status = Move_forward;
            this->time_left = time;
            set_motors(kilo_straight_left, kilo_straight_right);
        }

        void turn_left(unsigned long time) {
            this->start_time = common_sys->get_clock();
            if(this->status == Stop) spinup_motors();
            this->status = Turn_left;
            this->time_left = time;
            set_motors(kilo_turn_left, 0);
        }

        void turn_right(unsigned long time) {
            this->start_time = common_sys->get_clock();
            if(this->status == Stop) spinup_motors();
            this->status = Turn_right;
            this->time_left = time;
            set_motors(0, kilo_turn_right);
        }

        void stop_motor() {
            this->start_time = common_sys->get_clock();
            this->status = Stop;
            this->time_left = 0;
            set_motors(0, 0);
        }

        int current_status() {
            return this->status;
        }

        void register_common_sys(Common_system * common_sys) {
            this->common_sys = common_sys;
        }

        Motor_control_unit() {
            time_left = 0;
            status = Stop;
        }
    
    private:
        int status;
        unsigned long time_left;
        unsigned long start_time;
        Common_system * common_sys;
};

class My_control_factory: public Control_factory {
    public:
        void update_control() {
            for(int i = 0; i < NUM_OF_CONTROL_UNITS; i++) {
                if(my_control_unit[i] != NULL) {
                    my_control_unit[i]->update_control();
                }
            }
        }

        Control_unit * get_control_unit(int index) {
            return my_control_unit[index];
        }

        My_control_factory() {
            my_motor_control.register_common_sys(common_sys);
            my_control_unit[0] = &my_motor_control;
        }
    
    private:
        Control_unit * my_control_unit[NUM_OF_CONTROL_UNITS];
        Motor_control_unit my_motor_control;
};