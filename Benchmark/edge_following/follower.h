#pragma once

START_USER_PROGRAM

#define SEED_CHANNEL 0
#define FOLLOWER_CHANNEL 1

#define MOTION_STEP 10
#define LED_DURATION 200
#define MAX_DIST 200
#define SET_DISTANCE 60
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
int motion_state; // 0 for stop, 1 for left, 2 for right, 3 for forward

void sent_callback() {
    follower_publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

void recv_callback(unsigned char * msg, int size, int ttl, Meta_t * meta) {
    seed_state_t * seed_state = (seed_state_t *) msg;
    if(seed_state->seed_id != my_state.following && meta->dist < my_state.dist) {
        my_state.following = seed_state->seed_id;
        if(seed_state->occupied && seed_state->follower_id == my_state.follower_id) {
            my_state.dist = meta->dist;
        }
        else {
            LED_control->turn_on(1, 1, 1, LED_DURATION);
            return;
        }
    }
    else if(seed_state->seed_id == my_state.following) {
        my_state.dist = meta->dist;
    }

    if(seed_state->seed_id == my_state.following && seed_state->follower_id == my_state.follower_id) {
        if(meta->dist > SET_DISTANCE + ALLOW_NOISE) {
            if(motion_state == 0) {
                motion_state = 2;
            }
            // motor_control->turn_right(MOTION_STEP);
            // LED_control->turn_on(1, 1, 0, LED_DURATION);
        }
        else if(meta->dist < SET_DISTANCE - ALLOW_NOISE) {
            if(motion_state == 0) {
                motion_state = 1;
            }
            // motor_control->turn_left(MOTION_STEP);
            // LED_control->turn_on(1, 0, 0, LED_DURATION);
        }
        else {
            if(motion_state == 0) {
                motion_state = 3;
            }
            // motor_control->move_forward(MOTION_STEP);
            // LED_control->turn_on(0, 1, 0, LED_DURATION);
        }
    }
    else if(seed_state->seed_id == my_state.following && seed_state->follower_id != my_state.follower_id) {
        motor_control->stop_motor();
    }
}

void loop() {
    if(motor_control->current_status == Off) {
        if(motion_state == 1) {
            motion_state = 3;
            motor_control->turn_left(MOTION_STEP);
            LED_control->turn_on(1, 0, 0, LED_DURATION);
        }
        if(motion_state == 2) {
            motion_state = 3;
            motor_control->turn_right(MOTION_STEP);
            LED_control->turn_on(1, 1, 0, LED_DURATION);
        }
        if(motion_state == 3) {
            motion_state = 0;
            motor_control->move_forward(MOTION_STEP);
            LED_control->turn_on(0, 1, 0, LED_DURATION);
        }
    }
    if(LED_control->current_status() == Off) {
        LED_control->turn_on(1, 1, 1, LED_DURATION);
    }
}

void setup() {
    my_state.follower_id = swarmos.random_func() % 255;
    my_state.following = -1;
    my_state.dist = MAX_DIST;

    seed_channel = swarmnet->new_channel(SEED_CHANNEL, 0, false);
    follower_channel = swarmnet->new_channel(FOLLOWER_CHANNEL, 0, false);
    follower_publisher = follower_channel->new_publisher(sent_callback);
    seed_subscriber = seed_channel->new_subscriber(MAX_DIST, recv_callback);
    follower_publisher->send((unsigned char *) &my_state, sizeof(my_state));
}

END_USER_PROGRAM