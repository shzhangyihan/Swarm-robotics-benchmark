START_USER_PROGRAM

int previous_time;
int current_time;
int state;

void loop() {
    current_time = swarmos.get_clock();
    printf("current time is %d\n", current_time);
    if(current_time > previous_time && current_time - previous_time > 200) {
        if(state == 0) {
            state = 1;
            set_color(RGB(0, 1, 0));
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
        }
        else {
            state = 0;
            set_color(RGB(0, 0, 1));
            set_motors(0, 0);
        }
        previous_time = current_time;
    }
}

void setup() {
    state = 0;
    previous_time = swarmos.get_clock();
    set_color(RGB(1, 0, 0));
}

END_USER_PROGRAM