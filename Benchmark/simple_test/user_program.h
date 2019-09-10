START_USER_PROGRAM

Channel *channel_1;
Publisher *publisher_1;
Subscriber *subscriber_1; 
Channel *channel_2;
Publisher *publisher_2;
Subscriber *subscriber_2; 
unsigned char token[20];
int rand_id;

void sent() {
    publisher_1->send(token, strlen((char*)token));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("1: %d: Recv dist = %d, size = %d, payload = ", rand_id, meta->dist, size);
    for(int i = 1; i < size; i++) {
        printf("%c", msg[i]);
    }
    printf(", from = %d, delay = %d\r\n", (int)msg[0], meta->msg_delay);
} 

void sent_2() {
    publisher_2->send(token, strlen((char*)token));
}

void call_back_2(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("2: %d: Recv dist = %d, size = %d, payload = ", rand_id, meta->dist, size);
    for(int i = 1; i < size; i++) {
        printf("%c", msg[i]);
    }
    printf(", from = %d, delay = %d\r\n", (int)msg[0], meta->msg_delay);
} 

void loop() {
}

void setup() {
    rand_id = rand() % 255;
    strncpy((char*)token, " Hello world!", 5);
    token[0] = rand_id;
    channel_1 = swarmnet->new_channel(1, 0, true);
    publisher_1 = channel_1->new_publisher(sent);
    subscriber_1 = channel_1->new_subscriber(100, call_back);
    channel_2 = swarmnet->new_channel(2, 0, true);
    publisher_2 = channel_2->new_publisher(sent_2);
    subscriber_2 = channel_2->new_subscriber(100, call_back_2);
    publisher_1->send(token, strlen((char*)token));
    publisher_2->send(token, strlen((char*)token));
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM