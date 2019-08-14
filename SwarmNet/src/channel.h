#pragma once
#include "subscriber.h"
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


#if PYTHON
typedef void (*send_caller)(void *user_func);
#endif

class Channel: Base {
    public:
        void init(int type, int hops, bool listen);
        int send(unsigned char * msg, int msgSize);
        
        int subscribe(Subscriber * s);

        #if PYTHON
        int register_send_success(send_caller caller, void * usr_f);
        #else
        #if FUNC
        int register_send_success(std::function<void()> callback);
        #else
        int register_send_success(void (*callback)());
        #endif
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
        #if PYTHON
        send_caller caller;
        void * sentCallbacks[MAX_PUB_PER_CHAN];
        #else
        #if FUNC
        std::function<void()> sentCallbacks[MAX_PUB_PER_CHAN];
        #else
        void (*sentCallbacks[MAX_PUB_PER_CHAN])();
        #endif
        #endif
        Subscriber * subscribers[MAX_SUB_PER_CHAN];
        Packet recvBuffer[BUFF_SIZE_PER_CHAN];
        Packet sendBuffer[MAX_PKT_PER_MSG];
        int assembler[MAX_PKT_PER_MSG];
        int overflowCounter;
        bool overflowFlag;
        void try_merge(int nodeId, int msgId, int hopCount, Meta_t * meta);
};

