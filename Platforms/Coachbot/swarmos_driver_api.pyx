
cdef int cython_get_clock_caller(void *user_func):
    return (<object>user_func)()

cdef void cython_set_color_caller(void *user_func, int red, int green, int blue):
    (<object>user_func)(red, green, blue)

cdef void cython_set_motors_caller(void *user_func, int left, int right):
    (<object>user_func)(left, right)

cdef extern from "../../Driver/Coachbot/coach_driver.h":
    ctypedef int (*get_clock_caller)(void *user_func)
    ctypedef void (*set_color_caller)(void *user_func, int red, int green, int blue)
    ctypedef void (*set_motors_caller)(void *user_func, int left, int right)

    void python_register_get_clock(get_clock_caller caller, void * usr_f)
    void python_register_set_color(set_color_caller caller, void * usr_f)
    void python_register_set_motors(set_motors_caller caller, void * usr_f)

    void python_call_driver_loop()
    void python_call_driver_setup()

    int python_pull_packet(unsigned char * packet)
    void python_packet_receive(unsigned char *packet_received, float distance)

def register_get_clock(func):
    python_register_get_clock(cython_get_clock_caller, <void*>func)

def register_set_color(func):
    python_register_set_color(cython_set_color_caller, <void*>func)

def register_set_motors(func):
    python_register_set_motors(cython_set_motors_caller, <void*>func)

def call_driver_loop():
    python_call_driver_loop()

def call_driver_setup():
    print "python driver setup"
    python_call_driver_setup()

def pull_packet(pkt):
    cdef unsigned char* tmp_buf = pkt
    return python_pull_packet(tmp_buf)

def packet_receive(pkt, dist):
    cdef unsigned char* tmp_arr = pkt
    python_packet_receive(tmp_arr, dist)