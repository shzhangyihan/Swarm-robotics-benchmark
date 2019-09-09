import swarmos_driver as so

clock = 0
def get_clock():
    global clock
    clock = clock + 1
    return clock

def set_LED(red, green, blue):
    print "set LED to (" + str(red) + ", " + str(green) + ", " + str(blue) + ")"

def set_motors(left, right):
    print "set motor to (" + str(left) + ", " + str(right) + ")"

so.register_get_clock(get_clock)
so.register_set_color(set_LED)
so.register_set_motors(set_motors)
so.call_driver_setup()
pkt = "12345678910"
for i in range(20):
    so.call_driver_loop()
    size = so.pull_packet(pkt)
    print "pkt sent size " + str(size) + " payload: " + ''.join(str(ord(c)) + " " for c in pkt[:size])
    so.packet_receive(pkt[:size], 10)