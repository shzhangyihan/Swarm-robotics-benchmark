START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char token[20];

void sent() {
    publisher->send(token, strlen((char*)token));
    set_color(RGB(1, 0, 0));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("Recv dist = %d, size = %d, payload = %s\n", meta->dist, size, msg);
    set_color(RGB(0, 1, 0));
} 

void loop() {
    
}

void setup() {
    strncpy((char*)token, "Hello world!", 12);
    channel = swarmnet.new_channel(2, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(token, strlen((char*)token));
    set_color(RGB(0, 0, 1));
}

END_USER_PROGRAM