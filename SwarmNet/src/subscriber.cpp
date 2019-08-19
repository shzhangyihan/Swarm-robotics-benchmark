#include "subscriber.h"






#if PYTHON

void Subscriber::receive(unsigned char * msg, int size, int hop, Meta_t * meta) {
    //if(dist < this->dist && this->caller != NULL) {
    if(this->caller != NULL) {
        #if DEBUG
        printf("Sub callback\r\n");
        #endif
        this->caller(usr_f, msg, size, hop, meta);
    }
}

Subscriber::Subscriber() {
    this->dist = 0;
    this->caller = NULL;
}

Subscriber::Subscriber(int dist, recv_caller caller, void * usr_f) {
    subscriber_init(dist, caller, usr_f);
}

void Subscriber::subscriber_init(int dist, recv_caller caller, void * usr_f) {
    this->dist = dist;
    this->caller = caller;
    this->usr_f = usr_f;
}

#else

void Subscriber::receive(unsigned char * msg, int size, int hop, Meta_t * meta) {
    //if(dist < this->dist && this->callback != NULL) {
    if(this->callback != NULL) {
        callback(msg, size, hop, meta);
    }
}

Subscriber::Subscriber() {
    this->dist = 0;
    this->callback = NULL;
}

#if FUNC
Subscriber::Subscriber(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback) {
    subscriber_init(dist, callback);
}
void Subscriber::subscriber_init(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback) {
    this->dist = dist;
    this->callback = callback;
}

#else
Subscriber::Subscriber(int dist, void (*callback)(unsigned char *, int, int, Meta_t *)) {
    subscriber_init(dist, callback);
}
void Subscriber::subscriber_init(int dist, void (*callback)(unsigned char *, int, int, Meta_t *)) {
    this->dist = dist;
    this->callback = callback;
}
#endif
#endif 

