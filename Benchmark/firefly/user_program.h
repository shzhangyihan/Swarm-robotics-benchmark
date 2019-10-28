#pragma once
#include <math.h>

START_USER_PROGRAM

#define DEFAULT_PERIOD 150
#define LED_DURATION 2
#define TIMER_CHANGE_BOUND 10
#define NORMALIZE_FACTOR 5

typedef struct state {
    int period_length;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int last_rest;

void sent_callback() {

}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    // subtracting the queueing delay
    // here my_time is the time when the sender called send function
    int my_time = swarmos.get_clock() - last_rest - meta->msg_delay;
    if(my_time < 0) {
        my_time = DEFAULT_PERIOD + my_time;
    }
    if(my_time < (my_state.period_length / 2))
        my_state.period_length = my_state.period_length + my_time / NORMALIZE_FACTOR;
    else
        my_state.period_length = my_state.period_length - (my_state.period_length - my_time) / NORMALIZE_FACTOR;
    
    if(my_state.period_length < DEFAULT_PERIOD - TIMER_CHANGE_BOUND)
        my_state.period_length = DEFAULT_PERIOD - TIMER_CHANGE_BOUND;
    if(my_state.period_length > DEFAULT_PERIOD + TIMER_CHANGE_BOUND)
        my_state.period_length = DEFAULT_PERIOD + TIMER_CHANGE_BOUND;
}

void loop() {
    if(swarmos.get_clock() - last_rest > my_state.period_length) {
        LED_control->turn_on(1, 1, 1, LED_DURATION);
        publisher->send((unsigned char *) &my_state, sizeof(my_state));
        last_rest = swarmos.get_clock();
        my_state.period_length = DEFAULT_PERIOD;
    }
    if(LED_control->current_status() == Off) {
        LED_control->turn_on(0, 0, 1, 10000);
    }
}

void setup() {
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(200, recv_callback);

    my_state.period_length = DEFAULT_PERIOD;
    last_rest = swarmos.get_clock();
    LED_control->turn_on(0, 0, 1, 10000);
}

END_USER_PROGRAM
