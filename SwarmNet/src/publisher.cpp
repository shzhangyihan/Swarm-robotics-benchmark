#include "publisher.h"
#include "channel.h"

int Publisher::send(unsigned char * msg, int msgSize) {
    if(chan == NULL) return 0;
    return ((Channel *)chan)->send(msg, msgSize);
}

void Publisher::sent_callback() {
    if(callback != NULL) {
        callback();
    }
}

bool Publisher::available() {
    if(chan == NULL) return false;
    return ((Channel *)chan)->available();
}

bool Publisher::if_initialized() {
    if(callback == NULL) return false;
    else return true;
}

Publisher::Publisher() {
    this->chan = NULL;
    this->callback = NULL;
}

#if FUNC
Publisher::Publisher(void * chan, std::function<void()> callback) {
#else
Publisher::Publisher(void * chan, void (*callback)()) {
#endif
    publisher_init(chan, callback);
}

#if FUNC
void Publisher::publisher_init(void * chan, std::function<void()> callback) {
#else
void Publisher::publisher_init(void * chan, void (*callback)()) {
#endif
    this->chan = chan;
    this->callback = callback;
}

