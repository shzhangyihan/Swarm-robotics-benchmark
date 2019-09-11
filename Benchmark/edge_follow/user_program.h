START_USER_PROGRAM

Channel *channel;
Publisher *publisher;
Subscriber *subscriber; 
unsigned char token[20];
int rand_id;
int tolerable_max;
int tolerable_min;
int curr_attached;
int distance;
int dont_touch;
bool freeze;

void sent() {
    publisher->send(token, strlen((char*)token));
}

void call_back(unsigned char * msg, int size, int hop, Meta_t * meta) {
    if (id == 5){
        printf("%d\r\n", distance);
        if (msg[0] == 5) {
            dont_touch = msg[1];
            if (dont_touch != curr_attached) freeze = false;
        }
        else{
            if (msg[0] == curr_attached) distance = meta->dist;
            else {
                if (meta->dist < distance) {
                    if (msg[0] == dont_touch) freeze = true;
                    else {
                        distance = meta->dist;
                        curr_attached = msg[0];
                    }
                }
            }
        }
    }
    //printf("Recv dist = %d, size = %d, payload = %s, from = %d\n", meta->dist, size, (msg + 1), (int)msg[0]);
} 

void loop() {
    token[0] = id;
    if (id == 5) {
        token[1] = curr_attached;
        set_color(RGB(0,1,1));
        if (freeze != true){
            if (distance >= tolerable_max){
                set_motors(50,0);
                spinup_motors();
            }
            else if (distance <= tolerable_min){
                set_motors(0,50);
                spinup_motors();
            }
            else {
                set_motors(50,50);
                spinup_motors();
            }
        }
        else {
            set_motors(0,0);
            spinup_motors();
        }
    }
}

void setup() {
    rand_id = rand();
    strncpy((char*)token, " Hello world!", 13);
    token[0] = rand_id;
    tolerable_max = 55;
    tolerable_min = 35;
    curr_attached = 0;
    distance = 10000;
    dont_touch = 0;
    channel = swarmnet->new_channel(2, 0, false);
    publisher = channel->new_publisher(sent);
    subscriber = channel->new_subscriber(100, call_back);
    publisher->send(token, strlen((char*)token));
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM