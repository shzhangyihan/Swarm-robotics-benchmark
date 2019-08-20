#include "publisher.h"




int Publisher::send(unsigned char * msg, int msgSize) {
    return chan->send(msg, msgSize);
}

bool Publisher::available() {
    return chan->available();
}

Publisher::Publisher() {
    this->chan = NULL;
}

#if PYTHON
Publisher::Publisher(Channel * chan, send_caller caller, void * usr_f) {
    this->chan = chan;
    this->chan->register_send_success(caller, usr_f);
}
void Publisher::publisher_init(Channel * chan, send_caller caller, void * usr_f) {
    this->chan = chan;
    this->chan->register_send_success(caller, usr_f);
}
#else
#if FUNC
Publisher::Publisher(Channel * chan, std::function<void()> callback) {
    this->chan = chan;
    this->chan->register_send_success(callback);
}
void Publisher::publisher_init(Channel * chan, std::function<void()> callback) {
    this->chan = chan;
    this->chan->register_send_success(callback);
}

#else
Publisher::Publisher(Channel * chan, void (*callback)()) {
    this->chan = chan;
    this->chan->register_send_success(callback);
}
void Publisher::publisher_init(Channel * chan, void (*callback)()) {
    this->chan = chan;
    this->chan->register_send_success(callback);
}
#endif
#endif
