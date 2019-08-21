#include "subscriber.h"

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

