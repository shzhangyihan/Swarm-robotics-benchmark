#pragma once
#include "publisher.h"
#include "subscriber.h"
#include "packet.h"
#include "channel.h"
#include "util.h"

#if FUNC
#include <functional>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define ID_RANGE    256
//#define HEADER_BYTE 3
//#define MAX_TYPE    8

class Swarmnet {
    public:
        /* ---- user interfaces ---- */

        Channel * new_channel(int type, int hops, bool listen);
        
        /* ---- driver interfaces ---- */   
     
        int  next_pkt(unsigned char * pkt);
        void receive(unsigned char * pkt, int size, Meta_t * meta);

        /* ------- constructor ------- */   

        Swarmnet();
        ~Swarmnet();
        void set_common_sys(Common_system * common_sys);

    private:
        Common_system * common_sys;
        int nodeId;
        int forward_size;
        Packet forwardBuffer[FORWARD_BUFF_SIZE];
        int chan_num;
        Channel chans[TOTAL_CHAN_NUM];
        unsigned char send_cache[CACHE_SIZE][HEADER_BYTE];
        int cache_size;
        int roundRobinIndex;

        void new_id();
        bool check_cache(unsigned char * header);
        void add_cache(unsigned char * header);
};

//Swarmnet s;
