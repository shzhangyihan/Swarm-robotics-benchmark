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
    printf("%d: Recv dist = %d, size = %d, payload = ", rand_id, meta->dist, size);
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
    channel = swarmnet->new_channel(2, 0, true);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(token, strlen((char*)token));
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM