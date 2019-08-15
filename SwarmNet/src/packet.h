#pragma once
#include "util.h"
#include <stdint.h>

class Packet {
    public:
        void init(unsigned char *pkt);
        void init(unsigned char *content, int size, unsigned int nodeId, unsigned int msgId,
                  unsigned int seqNum, bool ifEnd, unsigned int chanNum, unsigned int ttl);
        
        int to_packet(unsigned char * pkt);
        unsigned char * get_content();
        void set_node_id(unsigned int id);
        void print_data();
        void decrease_hop();
        void set_time_bytes(unsigned int time);
        unsigned int get_node_id();
        unsigned int get_msg_id();
        unsigned int get_seq_num();
        unsigned int get_chan_type();
        unsigned int get_ttl();
        unsigned int get_pay_size();
        bool get_if_end();
        bool if_valid();
        unsigned int get_time_bytes();

        Packet();
        Packet(unsigned char *pkt);
        Packet(unsigned char *content, int size, unsigned int nodeId, unsigned int msgId,
               unsigned int seqNum, bool ifEnd, unsigned int chanNum, unsigned int ttl);
        ~Packet();

    private:
        #if WITH_CRC
        unsigned char content[PKT_SIZE - HEADER_BYTE - CRC_BYTE];
        #else
        unsigned char content[PKT_SIZE - HEADER_BYTE];
        #endif
        unsigned char header[HEADER_BYTE];
        bool valid;

        unsigned char crc_check(unsigned char *pkt, int size, int in_crc);
        unsigned char crc(unsigned char *pkt, int size);
};
