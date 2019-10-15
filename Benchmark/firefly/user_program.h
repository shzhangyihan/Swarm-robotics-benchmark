#pragma once
#include <math.h>

START_USER_PROGRAM

#define WHOLE_PERIOD 1000
#define FLASH_PERIOD 5
#define NORMALIZE_SCALE 0.1
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef struct state {
    int period;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int current_period_start;
int current_period_adjustment;
int total_period_diff;
int total_period_count;

void sent_callback() {
    my_state.period = swarmos.get_clock() - current_period_start + current_period_adjustment;
    publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

int normalize_period_diff(int avg_period_diff) {
    if(avg_period_diff > 0) {
        return NORMALIZE_SCALE * (WHOLE_PERIOD - avg_period_diff);
    }
    else {
        return NORMALIZE_SCALE * ((-1) * avg_period_diff);
    }
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    state_t * recv_state = (state_t *) msg;
    int current_source_period = (recv_state->period + meta->msg_delay) % WHOLE_PERIOD;
    if(current_source_period < 0) {
        // source clock overflow
        return;
    }
    int period_diff = (swarmos.get_clock() - current_period_start + current_period_adjustment) - current_source_period;
    // to consider period rollover
    period_diff = MIN(abs(period_diff), MIN(abs(period_diff + WHOLE_PERIOD), abs(period_diff - WHOLE_PERIOD)));
    //printf("%d: period_diff: %d\n", id, period_diff);
    total_period_diff = total_period_diff + period_diff;
    total_period_count = total_period_count + 1;
}

void loop() {
    //printf("loop\n");
    my_state.period = swarmos.get_clock() - current_period_start + current_period_adjustment;
    if(my_state.period > WHOLE_PERIOD) {
        if(total_period_count == 0) current_period_adjustment = normalize_period_diff(0);
        else current_period_adjustment = normalize_period_diff(total_period_diff / total_period_count);
        LED_control->turn_on(1, 1, 1, FLASH_PERIOD);
        current_period_start = swarmos.get_clock();
        my_state.period = current_period_adjustment;
        printf("%d: flash at %d, avg error %d adjustment %d\n", id, current_period_start, (total_period_count == 0) ? 0: (total_period_diff / total_period_count), current_period_adjustment);
        total_period_diff = 0;
        total_period_count = 0;
    }
}

void setup() {
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(200, recv_callback);

    current_period_start = swarmos.get_clock();
    current_period_adjustment = swarmos.random_func() % WHOLE_PERIOD;
    my_state.period = swarmos.get_clock() - current_period_start + current_period_adjustment;

    publisher->send((unsigned char *) &my_state, sizeof(my_state));
    LED_control->turn_on(0, 0, 0, 0);
}

END_USER_PROGRAM
