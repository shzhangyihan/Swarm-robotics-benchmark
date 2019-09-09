#pragma once

// python callback for get_clock
typedef int (*get_clock_caller)(void *user_func);
get_clock_caller my_get_clock_caller;
void * get_clock_caller_func;

// python callback for set_color
typedef void (*set_color_caller)(void *user_func, int red, int green, int blue);
set_color_caller my_set_color_caller;
void * set_color_caller_func;

// python callback for set_motors
typedef void (*set_motors_caller)(void *user_func, int left, int right);
set_motors_caller my_set_motors_caller;
void * set_motors_caller_func;

void python_register_get_clock(get_clock_caller caller, void * usr_f);
void python_register_set_color(set_color_caller caller, void * usr_f);
void python_register_set_motors(set_motors_caller caller, void * usr_f);

void python_call_driver_loop();
void python_call_driver_setup();

int python_pull_packet(unsigned char * packet);
void python_packet_receive(unsigned char *packet_received, float distance);

unsigned int python_get_clock();
void python_set_color(int red, int green, int blue);
void python_set_motors(int left, int right);
