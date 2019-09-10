#pragma once
#include "../../../Swarm-robotics-benchmark/Driver/Kilobot_simulation/kilobot_driver.h"

START_USER_PROGRAM

#define MAX_HOP 100
#define LED_DURATION 200

typedef struct custom_message {
    bool is_seed;
    int hop;
} custom_message_t;

Channel * channel_seed_1;
Channel * channel_seed_2;

Publisher * publisher_channel_1;
Publisher * publisher_channel_2;

Subscriber * subscriber_channel_1;
Subscriber * subscriber_channel_2;

custom_message_t my_message_channel_1;
custom_message_t my_message_channel_2;

int hop_from_seed_1;
int hop_from_seed_2;

int LED_flag;

void sent_callback_channel_1() {
    publisher_channel_1->send((unsigned char *) &my_message_channel_1, sizeof(my_message_channel_1));
}

void sent_callback_channel_2() {
    publisher_channel_2->send((unsigned char *) &my_message_channel_2, sizeof(my_message_channel_2));
}

void recv_callback_channel_1(unsigned char * msg, int size, int ttl, Meta_t * meta) {
	custom_message_t * received_msg = (custom_message_t *) msg;
    if(received_msg->hop < hop_from_seed_1) {
        hop_from_seed_1 = received_msg->hop + 1;
        my_message_channel_1.hop = hop_from_seed_1;
    }
}

void recv_callback_channel_2(unsigned char * msg, int size, int ttl, Meta_t * meta) {
	custom_message_t * received_msg = (custom_message_t *) msg;
    if(received_msg->hop < hop_from_seed_2) {
        hop_from_seed_2 = received_msg->hop + 1;
        my_message_channel_2.hop = hop_from_seed_2;
    }
}

void update_LED(int hop) {
    switch (hop) {
        case 1:
            LED_control->turn_on(1, 0, 0, LED_DURATION);
            break;
        case 2:
            LED_control->turn_on(1, 1, 0, LED_DURATION);
            break;
        case 3:
            LED_control->turn_on(0, 1, 0, LED_DURATION);
            break;
        default:
            LED_control->turn_on(0, 0, 0, LED_DURATION);
    }
}

void loop() {
    if(LED_control->current_status() == Off) {
        if(LED_flag == 0) {
            LED_flag = 1;
            update_LED(hop_from_seed_1);
        }
        else {
            LED_flag = 0;
            update_LED(hop_from_seed_2);
        }
    }
}

void setup() {
    hop_from_seed_1 = MAX_HOP;
    hop_from_seed_2 = MAX_HOP;

    my_message_channel_1.hop = hop_from_seed_1;
    my_message_channel_1.is_seed = false;
    my_message_channel_2.hop = hop_from_seed_2;
    my_message_channel_2.is_seed = false;

    channel_seed_1 = swarmnet->new_channel(1, 0, false);
    channel_seed_2 = swarmnet->new_channel(2, 0, false);

    publisher_channel_1 = channel_seed_1->new_publisher(sent_callback_channel_1);
    publisher_channel_2 = channel_seed_2->new_publisher(sent_callback_channel_2);

    subscriber_channel_1 = channel_seed_1->new_subscriber(10, recv_callback_channel_1);
    subscriber_channel_2 = channel_seed_2->new_subscriber(10, recv_callback_channel_2);

    publisher_channel_1->send((unsigned char *) &my_message_channel_1, sizeof(my_message_channel_1));
    publisher_channel_2->send((unsigned char *) &my_message_channel_2, sizeof(my_message_channel_2));

    LED_control->turn_on(0, 0, 0, LED_DURATION);
    LED_flag = 0;
}

END_USER_PROGRAM