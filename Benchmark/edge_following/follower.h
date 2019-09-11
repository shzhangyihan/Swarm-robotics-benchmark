#pragma once

START_USER_PROGRAM

#define SEED_CHANNEL 0
#define FOLLOWER_CHANNEL 1

#define MOTION_STEP 1
#define LED_DURATION 30
#define MAX_DIST 150
#define SET_DISTANCE 80
#define ALLOW_NOISE 5

typedef struct seed_state {
    int seed_id;
    int follower_id;
    bool occupied;
} seed_state_t;

typedef struct follower_state {
    int follower_id;
    int following;
    int dist;
} follower_state_t;

Channel * seed_channel;
Channel * follower_channel;
Publisher * follower_publisher;
Subscriber * seed_subscriber;
follower_state_t my_state;

void sent_callback() {
    follower_publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
	seed_state_t * seed_state = (seed_state_t *) msg;
    if(seed_state->seed_id != my_state.following && meta->dist < my_state.dist) {
        if(seed_state->occupied) {
            motor_control->stop_motor();
            return;
        }
        my_state.following = seed_state->seed_id;
        my_state.dist = meta->dist;
        printf("%d: start following %d from %d\n", my_state.follower_id, my_state.following, my_state.dist);
    }
    else if(seed_state->seed_id == my_state.following) {
        my_state.dist = meta->dist;
    }
    
    if(seed_state->seed_id == my_state.following) {
        if(meta->dist > SET_DISTANCE + ALLOW_NOISE) {
            motor_control->turn_right(MOTION_STEP);
        }
        else if(meta->dist < SET_DISTANCE - ALLOW_NOISE) {
            motor_control->turn_left(MOTION_STEP);
        }
        else {
            motor_control->move_forward(MOTION_STEP);
        }
    }
}

void loop() {
    LED_control->turn_on(1, 0, 0, LED_DURATION);
}

void setup() {
    my_state.follower_id = swarmos.random_func();
    my_state.following = -1;
    my_state.dist = MAX_DIST;

    seed_channel = swarmnet->new_channel(SEED_CHANNEL, 0, false);
    follower_channel = swarmnet->new_channel(FOLLOWER_CHANNEL, 0, false);
    follower_publisher = follower_channel->new_publisher(sent_callback);
    seed_subscriber = seed_channel->new_subscriber(MAX_DIST, recv_callback);
    follower_publisher->send((unsigned char *) &my_state, sizeof(my_state));

    LED_control->turn_on(1, 0, 0, LED_DURATION);
}

END_USER_PROGRAM