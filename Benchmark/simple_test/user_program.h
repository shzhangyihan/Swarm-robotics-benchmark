START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char token[20];
int rand_id;

void sent() {
    publisher->send(token, strlen((char*)token));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("Recv dist = %d, size = %d, payload = %s, from = %d\n", meta->dist, size, (msg + 1), (int)msg[0]);
} 

void loop() {
}

void setup() {
    rand_id = rand_hard();
    strncpy((char*)token, " Hello world!", 13);
    token[0] = rand_id;
    channel = swarmnet->new_channel(2, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(token, strlen((char*)token));
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM