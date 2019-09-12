#pragma once
#include <math.h>

START_USER_PROGRAM

#define WHOLE_PERIOD 1000
#define FLASH_PERIOD 50
#define NORMALIZE_SCALE WHOLE_PERIOD / 50
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef struct state {
    int period;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int current_period_start;
int total_period_diff;
int total_period_count;

void sent_callback() {
    my_state.period = swarmos.get_clock() - current_period_start;
    publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

int normalize_period_diff(int avg_period_diff) {
    return NORMALIZE_SCALE * (avg_period_diff + WHOLE_PERIOD) / (2 * WHOLE_PERIOD);
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    state_t * recv_state = (state_t *) msg;
    int current_source_period = (recv_state->period + meta->msg_delay) % WHOLE_PERIOD;
    if(current_source_period < 0) {
        // source clock overflow
        return;
    }
    int period_diff = (swarmos.get_clock() - current_period_start) - current_source_period;
    // to consider period rollover
    period_diff = MIN(abs(period_diff), MIN(abs(period_diff + WHOLE_PERIOD), abs(period_diff - WHOLE_PERIOD)));
    
    if(period_diff > 500)
    total_period_diff = total_period_diff + period_diff;
    total_period_count = total_period_count + 1;
}

void loop() {
    if(swarmos.get_clock() - current_period_start > WHOLE_PERIOD) {
        int period_adjustment;
        if(total_period_count == 0) period_adjustment = 0;
        else period_adjustment = normalize_period_diff(total_period_diff / total_period_count);
        LED_control->turn_on(1, 0, 0, 100);
        current_period_start = swarmos.get_clock();
        current_period_start = current_period_start + period_adjustment;
        total_period_diff = 0;
        total_period_count = 0;
    }
    if(swarmos.get_clock() - current_period_start > FLASH_PERIOD) {
        LED_control->turn_on(0, 0, 1, 100);
    }
}

void setup() {
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(200, recv_callback);

    current_period_start = swarmos.get_clock();
    my_state.period = swarmos.get_clock() - current_period_start;

    publisher->send((unsigned char *) &my_state, sizeof(my_state));
    LED_control->turn_on(0, 0, 1, 100);
}

END_USER_PROGRAM
