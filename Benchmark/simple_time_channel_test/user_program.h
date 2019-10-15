START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char token[1];
unsigned char rand_id;

void sent() {
    publisher->send(token, 1);
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("%d: Recv dist = %d, size = %d, from = %d, delay = %d \r\n", (int)rand_id, meta->dist, size, (int)msg[0], meta->msg_delay);
} 

void loop() {
    //printf("%d\n", (int)rand_id);
    LED_control->turn_on(0, 0, 1, 200);
}

void setup() {
    rand_id = swarmos.random_func() % 255;
    token[0] = rand_id;
    channel = swarmnet->new_channel(1, 0, true);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(token, 1);
    LED_control->turn_on(0, 0, 1, 200);
}

END_USER_PROGRAM