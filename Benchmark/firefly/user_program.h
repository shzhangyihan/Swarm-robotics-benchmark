#pragma once
#include <math.h>

START_USER_PROGRAM

#define PERIOD 50
#define LED_DURATION 2
#define TIMER_CHANGE_BOUND 20
#define CHANGE_NORMAL 25

typedef struct state {
    int cur_period;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int last_reset;
int timer_change;
int timer_change_raw;

void sent_callback() {
    my_state.cur_period = swarmos.get_clock() - last_reset;
    publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    state_t * recv_state = (state_t *) msg;
    int recv_period = (recv_state->cur_period + meta->msg_delay) % PERIOD;
    int my_period = swarmos.get_clock() - last_reset;
    // printf("period_diff = %d delay = %d\n", my_period - recv_period, meta->msg_delay);
    printf("recv %02d - recv_period = %d delay = %d my_period = %d\n", id, recv_period, meta->msg_delay, my_period);
    int tmp_timer_change_raw = timer_change_raw;
    if((my_period - recv_period) > 0) {
        if((my_period - recv_period) < (PERIOD / 2)) {
            timer_change_raw += my_period - recv_period;
        }
        else {
            timer_change_raw -= (PERIOD + recv_period - my_period) % PERIOD;
        }
    }
    else if((my_period - recv_period) < 0) {
        if((recv_period - my_period) < (PERIOD / 2)) {
            timer_change_raw -= recv_period - my_period;
        }
        else {
            timer_change_raw += (PERIOD + my_period - recv_period) % PERIOD;
        }
    }
    printf("recv %02d - timer_change_raw from %d to %d\n", id, tmp_timer_change_raw, timer_change_raw);
}

void loop() {
    int reset_time = last_reset + PERIOD + timer_change;
    printf("%02d - kilo_ticks = %d last_reset = %d timer_change_raw = %d timer_change = %d reset_time = %d \n", id, swarmos.get_clock(), last_reset, timer_change_raw, timer_change, reset_time);
    if(swarmos.get_clock() >= reset_time) {
        last_reset = swarmos.get_clock();
        timer_change = timer_change_raw / CHANGE_NORMAL;
        if(timer_change > TIMER_CHANGE_BOUND) timer_change = TIMER_CHANGE_BOUND;
        if(timer_change < -TIMER_CHANGE_BOUND) timer_change = -TIMER_CHANGE_BOUND;
        LED_control->turn_on(1, 1, 1, LED_DURATION);
        timer_change_raw = 0;
    }
}

void setup() {
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(200, recv_callback);

    last_reset = 0;
    timer_change = 0;
    my_state.cur_period = swarmos.get_clock() - last_reset;

    publisher->send((unsigned char *) &my_state, sizeof(my_state));
    LED_control->turn_on(0, 0, 0, 0);
}

END_USER_PROGRAM
