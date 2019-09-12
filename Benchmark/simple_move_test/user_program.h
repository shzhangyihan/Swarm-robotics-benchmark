START_USER_PROGRAM

int motor_state;

void loop() {
    if(motor_control->current_status() == Stop) {
        if(motor_state == 0) {
            motor_control->turn_right(50);
            LED_control->turn_on(0, 1, 0, 50);
            motor_state = 1;
        }
        else if(motor_state == 1) {
            motor_control->move_forward(200);
            LED_control->turn_on(0, 0, 1, 200);
            motor_state = 2;
        }
        else if(motor_state == 2) {
            motor_control->turn_left(50);
            LED_control->turn_on(1, 0, 0, 50);
            motor_state = 3;
        }
        else if(motor_state == 3) {
            motor_control->move_forward(200);
            LED_control->turn_on(1, 1, 1, 200);
            motor_state = 0;
        }
    }
}

void setup() {
    motor_state = 0;
}

END_USER_PROGRAM
