#pragma once
#include <math.h>
#include <unistd.h>

START_USER_PROGRAM

#define DEFAULT_PERIOD 600
#define LED_DURATION 60
#define TIMER_CHANGE_BOUND 300
#define NORMALIZE_FACTOR 0.01

typedef struct state {
    int period_length;
} state_t;

Channel * channel;
Publisher * publisher;
Subscriber * subscriber;
state_t my_state;
int last_rest;
FILE *f;
float period;

void sent_callback() {

}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    // subtracting the queueing delay
    // here my_time is the time when the sender called send function
    //int my_time = swarmos.get_clock() - last_rest - meta->msg_delay;
    int my_time = swarmos.get_clock() - last_rest;
    if(my_time < 0) {
        my_time = DEFAULT_PERIOD + my_time;
    }
    //if(my_time < 0 || my_time > my_state.period_length) {return;}
    //fprintf(f, "my_time %d delay %d last reset %d change from %d ", my_time, meta->msg_delay, last_rest, my_state.period_length);
    if(my_time < 0 || my_time > period) {return;}
    float step = NORMALIZE_FACTOR * (1 + (swarmos.random_func() % 3) / 5.0);
    if(my_time < (my_state.period_length / 2))
        //my_state.period_length = my_state.period_length + (int)(my_time * step);
        period = period + (my_time * step);
    else
        //my_state.period_length = my_state.period_length - (int)((my_state.period_length - my_time) * step);
        period = period - ((period - my_time) * step);
    if(period < DEFAULT_PERIOD - TIMER_CHANGE_BOUND) {
        period = period - TIMER_CHANGE_BOUND;
    }
    if(period > DEFAULT_PERIOD + TIMER_CHANGE_BOUND) {
        period = period + TIMER_CHANGE_BOUND;
    }
    /*
    if(my_state.period_length < DEFAULT_PERIOD - TIMER_CHANGE_BOUND)
        my_state.period_length = DEFAULT_PERIOD - TIMER_CHANGE_BOUND;
    if(my_state.period_length > DEFAULT_PERIOD + TIMER_CHANGE_BOUND)
        my_state.period_length = DEFAULT_PERIOD + TIMER_CHANGE_BOUND;
*/
    //fprintf(f, "to %d\n", my_state.period_length);
    //fflush(f);
}

void loop() {
    //fprintf(f, "loop %d\n", swarmos.get_clock());
//    if(swarmos.get_clock() - last_rest > my_state.period_length) {
      if(swarmos.get_clock() - last_rest > period) {
//LED_control->turn_on(1, 1, 1, 1000);
        python_set_color(100, 100, 100);
        publisher->send((unsigned char *) &my_state, sizeof(my_state));
        last_rest = swarmos.get_clock();
	//fprintf(f, "period = %d\n", my_state.period_length);
	//fprintf(f, "FLASHED at %d\n", swarmos.get_clock());
	//fflush(f);
        my_state.period_length = DEFAULT_PERIOD;
	period = DEFAULT_PERIOD;
    }
    else if(swarmos.get_clock() - last_rest > 10) {
        //LED_control->turn_on(0, 0, 0, 1000);
        //fprintf(f, "stop led\n");
	//fflush(f);
        python_set_color(0, 0, 0);
    }
    else {
        //fprintf(f, "led on\n");
        //fflush(f);
    }
    //if(LED_control->current_status() == Off) {
        //LED_control->turn_on(0, 0, 1, 10000);
	//LED_control->turn_off();
    //}
    //sleep(0.5);
}

void setup() {
    //f = fopen("ff_log", "w");
    channel = swarmnet->new_channel(0, 0, true);
    publisher = channel->new_publisher(sent_callback);
    subscriber = channel->new_subscriber(150, recv_callback);

    my_state.period_length = swarmos.random_func() % (DEFAULT_PERIOD + TIMER_CHANGE_BOUND);
    //period = (1.0 * swarmos.random_func()) % (DEFAULT_PERIOD + TIMER_CHANGE_BOUND);
    period = fmod(1.0 * swarmos.random_func(), 1.0 * (DEFAULT_PERIOD));
    //my_state.period_length = DEFAULT_PERIOD;
    last_rest = swarmos.get_clock();
    //LED_control->turn_on(0, 0, 1, 10000);
    //LED_control->turn_off();
    //fprintf(f, "end of setup\n");
    //fflush(f);
}

END_USER_PROGRAM
