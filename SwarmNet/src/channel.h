#pragma once
#include "subscriber.h"
#include "publisher.h"
#include "packet.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if FUNC
#include <functional>
#endif

#define DUMMY_ID        0
#define MAX_BUFF        8
#define MAX_MSG_ID      16

#define ERROR_NOT_READY 1
#define ERROR_TOO_LONG  2
#define SUCCESS         0
#define ERROR_FULL      1
#define ERROR_EMPTY     1

class Channel: Base {
    public:
        void init(int type, int hops, bool listen);
        int send(unsigned char * msg, int msgSize);
        
        #if FUNC
        Subscriber * new_subscriber(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback);
        Publisher * new_publisher(std::function<void()> callback);
        #else
        Subscriber * new_subscriber(int dist, void (*callback)(unsigned char *, int, int, Meta_t *));
        Publisher * new_publisher(void (*callback)());
        #endif
        
        bool available();
        bool overflow_status();
        int next_pkt(Packet *ret);
        void receive(Packet * newPkt, Meta_t * meta);
        int get_type();
        
        void print_channel_status();

        Channel();
        Channel(int type, int hops, bool time_chan);
        ~Channel();

    private:
        int type;
        int hops;
        bool time_chan;
        int send_index;
        int send_pktNum;
        int recv_pktNum;
        bool ready;
        Publisher publishers[MAX_PUB_PER_CHAN];
        Subscriber subscribers[MAX_SUB_PER_CHAN];
        Packet recvBuffer[BUFF_SIZE_PER_CHAN];
        Packet sendBuffer[MAX_PKT_PER_MSG];
        int assembler[MAX_PKT_PER_MSG];
        int overflowCounter;
        bool overflowFlag;
        void try_merge(int nodeId, int msgId, int hopCount, Meta_t * meta);
};

