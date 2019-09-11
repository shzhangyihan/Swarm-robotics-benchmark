START_USER_PROGRAM

int motor_state;
int LED_state;

void loop() {
    if(motor_control->current_status() == Stop) {
        if(motor_state == 0) {
            motor_control->turn_left(5);
            motor_state = 1;
        }
        else if(motor_state == 1) {
            motor_control->move_forward(1000);
            motor_state = 0;
        }
        // else if(motor_state == 2) {
        //     motor_control->turn_right(1000);
        //     motor_state = 0;
        // }
    }

    if(LED_control->current_status() == Off) {
        if(LED_state == 0) {
            LED_control->turn_on(0, 1, 0, 200);
            LED_state = 1;
        }
        else {
            LED_control->turn_on(0, 1, 0, 200);
            LED_state = 0;
        }
    }
}

void setup() {
    motor_state = 0;
    motor_control->move_forward(1000);

    LED_state = 0;
    LED_control->turn_on(1, 0, 0, 200);
}

END_USER_PROGRAM