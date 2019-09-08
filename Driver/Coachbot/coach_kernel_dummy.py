import ctypes

def coach_start():
    coach_driver = ctypes.CDLL('./coach_driver.so')
    while True:
        print "loop"

def clock():
    return 0

def set_motor(left, right):
    print "set motor to (" + str(left) + ", " + str(right) + ")"

def set_LED(red, green, blue):
    print "set LED to (" + str(red) + ", " + str(green) + ", " + str(blue) + ")"
