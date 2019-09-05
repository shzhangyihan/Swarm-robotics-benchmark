START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char token[20];
int rand_id;
int flag;

void sent() {
    publisher->send(token, strlen((char*)token));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    printf("Recv dist = %d, size = %d, payload = %s, from = %d, delay = %d\n", meta->dist, size, (msg + 1), (int)msg[0], meta->msg_delay);
} 

void loop() {
    //if(flag = 0) {
    int ret = publisher->send(token, strlen((char*)token));
    //flag = 1;}
    //printf("%d\n", ret);
    //printf("kilo_ticks = %d\n", kilo_ticks);
}

void setup() {
    rand_id = rand();
    strncpy((char*)token, " Hello world!", 5);
    token[0] = rand_id;
    channel = swarmnet->new_channel(2, 0, true);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    //publisher->send(token, strlen((char*)token));
    flag = 0;
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM