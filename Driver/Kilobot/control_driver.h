#pragma once
#include "../../SwarmOS/src/swarmos.h"

extern "C" {
    #include "../../../Swarm-platforms/kilobot/kilolib.h"
}

#define NUM_OF_CONTROL_UNITS 2

enum enumType {Stop, Move_forward, Turn_left, Turn_right, On, Off};

class Motor_control_unit: public Control_unit {
    public:
        void update_control() {
            if(this->common_sys->get_clock == NULL) return;
            if(this->status == Stop || this->time_left == 0) return;
            unsigned long cur_time = this->common_sys->get_clock();
            if(cur_time - this->start_time > this->time_left) {
                status = Stop;
                time_left = 0;
                set_motors(0, 0);
            }
        }

        void move_forward(unsigned long time) {
            if(this->common_sys->get_clock == NULL) return;
            this->start_time = (this->common_sys)->get_clock();
            if(this->status == Stop) spinup_motors();
            this->status = Move_forward;
            this->time_left = time;
            set_motors(kilo_straight_left, kilo_straight_right);
        }

        void turn_left(unsigned long time) {
            if(this->common_sys->get_clock == NULL) return;
            this->start_time = this->common_sys->get_clock();
            if(this->status == Stop) spinup_motors();
            this->status = Turn_left;
            this->time_left = time;
            set_motors(0, kilo_turn_left);
        }

        void turn_right(unsigned long time) {
            if(this->common_sys->get_clock == NULL) return;
            this->start_time = this->common_sys->get_clock();
            if(this->status == Stop) spinup_motors();
            this->status = Turn_right;
            this->time_left = time;
            set_motors(kilo_turn_right, 0);
        }

        void stop_motor() {
            if(this->common_sys->get_clock == NULL) return;
            this->start_time = this->common_sys->get_clock();
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

        ~Motor_control_unit() { }
    
    private:
        int status;
        unsigned long time_left;
        unsigned long start_time;
        Common_system * common_sys;
};

class LED_control_unit: public Control_unit {
    public:
        void update_control() {
            if(this->common_sys->get_clock == NULL) return;
            if(this->status == Stop || this->time_left == 0) return;
            unsigned long cur_time = this->common_sys->get_clock();
            if(cur_time - this->start_time > this->time_left) {
                status = Off;
                time_left = 0;
                set_color(RGB(0, 0, 0));;
            }
        }

        void turn_on(int red, int green, int blue, int time) {
            if(this->common_sys->get_clock == NULL) return;
            this->start_time = (this->common_sys)->get_clock();
            this->status = On;
            this->time_left = time;
            set_color(RGB(red, green, blue));;
        }

        int current_status() {
            return this->status;
        }

        void register_common_sys(Common_system * common_sys) {
            this->common_sys = common_sys;
        }

        LED_control_unit() {
            time_left = 0;
            status = Off;
        }

        ~LED_control_unit() { }
    
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

        void register_common_sys(Common_system * common_sys) {
            this->common_sys = common_sys;
            for(int i = 0; i < NUM_OF_CONTROL_UNITS; i++) {
                my_control_unit[i]->register_common_sys(this->common_sys);
            }
        }

        My_control_factory() {
            my_control_unit[0] = &my_motor_control;
            my_control_unit[1] = &my_LED_control;
        }

        ~My_control_factory() { }
    
    private:
        Control_unit * my_control_unit[NUM_OF_CONTROL_UNITS];
        Motor_control_unit my_motor_control;
        LED_control_unit my_LED_control;
};