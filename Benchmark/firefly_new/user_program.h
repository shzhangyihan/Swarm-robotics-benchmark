#pragma once
#include <math.h>

START_USER_PROGRAM

#define DEFAULT_PERIOD 200
#define LED_DURATION 10
#define TIMER_CHANGE_BOUND 50
#define NORMALIZE_FACTOR 380

typedef struct state {
    int time;
    int period_length;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int timer_change, timer_change_raw;
int last_reset;
FILE * f;

void sent_callback() {
    my_state.time = swarmos.get_clock() - last_reset;
    my_state.period_length = DEFAULT_PERIOD + timer_change;
    publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    state_t * recv_state = (state_t *) msg;
    int recv_time = (recv_state->time + meta->msg_delay) % recv_state->period_length;
    int my_time = swarmos.get_clock() - last_reset;
    //fprintf(f, "time %d delay %d period %d result %d\n", recv_state->time, meta->msg_delay, recv_state->period_length, recv_time);
    //fprintf(f, "recv my_time %d recv_time %d\n", my_time, recv_time);
    //fflush(f);
    if((my_time - recv_time) > 0) {
        if((my_time - recv_time) < (DEFAULT_PERIOD / 2)) {
            timer_change_raw += my_time - recv_time;
        }
        else {
            timer_change_raw -= (recv_time + DEFAULT_PERIOD - my_time) % DEFAULT_PERIOD;
        }
    }
    else if((my_time - recv_time) < 0) {
        if((recv_time - my_time) < (DEFAULT_PERIOD / 2)) {
            timer_change_raw -= recv_time - my_time;
        }
        else {
            timer_change_raw += (DEFAULT_PERIOD + my_time - recv_time) % DEFAULT_PERIOD;
        }
    }
}

void loop() {
    int reset_time = last_reset + DEFAULT_PERIOD + timer_change;
    fprintf(f, "cur period = %d\n", swarmos.get_clock() - last_reset);
    if(swarmos.get_clock() >= reset_time) {
        last_reset = swarmos.get_clock();
        timer_change = timer_change_raw / NORMALIZE_FACTOR;
        if(timer_change > TIMER_CHANGE_BOUND)
            timer_change = TIMER_CHANGE_BOUND;
        else if(timer_change < -TIMER_CHANGE_BOUND)
            timer_change = -TIMER_CHANGE_BOUND;
        fprintf(f, "timer_change = %d raw = %d\n", timer_change, timer_change_raw);
        fflush(f);
        timer_change_raw = 0;
        python_set_color(100, 100, 100);
        //LED_control->turn_on(100, 100, 100, LED_DURATION);
        // motor_control->move_forward(1);
        // publisher->send((unsigned char *) &my_state, sizeof(my_state));
    }
    else if(swarmos.get_clock() >= last_reset + LED_DURATION) {
        //LED_control->turn_on(0, 0, 100, 1000);
        python_set_color(0, 0, 0);
        fprintf(f, "off\n");
	fflush(f);
    }
}

void setup() {
    f = fopen("ff_log", "w");
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(300, recv_callback);

    // timer_change = swarmos.random_func() % DEFAULT_PERIOD;
    timer_change = 0;
    last_reset = swarmos.get_clock() - (swarmos.random_func() % DEFAULT_PERIOD);
    //LED_control->turn_on(0, 0, 100, 10000);
    python_set_color(0, 0, 0);
    my_state.time = swarmos.get_clock() - last_reset;
    my_state.period_length = DEFAULT_PERIOD;
    publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

END_USER_PROGRAM
