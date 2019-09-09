START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
unsigned char token[20];

void sent() {
}

void loop() {
    printf("loop called\n");
    printf("clock = %d\n", swarmos.get_clock());
}

void setup() {
    printf("setup\n");
    motor_control->move_forward(20);
    LED_control->turn_on(1, 0, 0, 10);
    strncpy((char *)token, " Hello world!", 5);
    channel = swarmnet->new_channel(2, 0, true);
    publisher = channel->new_publisher(sent);
    publisher->send(token, strlen((char*)token));
}

END_USER_PROGRAM