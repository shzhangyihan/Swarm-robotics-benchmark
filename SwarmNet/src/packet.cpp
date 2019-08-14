#include "packet.h"




#include <stdio.h>
#include <string.h>

Packet::Packet() {
    this->valid = false;
}

Packet::Packet(unsigned char *pkt) {
    init(pkt);
}

Packet::Packet(unsigned char *content, int size, unsigned int nodeId, unsigned int msgId,
               unsigned int seqNum, bool ifEnd, unsigned int chanNum, unsigned int ttl) {
    init(content, size, nodeId, msgId, seqNum, ifEnd, chanNum, ttl);
}

void Packet::init(unsigned char *pkt) {
    // create packet from raw packet
    #if DEBUG
    printf("== New Packet arrived! ==\r\n");
    // print out raw packet
    printf("*************************\r\n");
    for(int i = 0; i < PKT_SIZE; i++) {
        unsigned char n = pkt[i];
        for(int j = 0; j < 8; j++) {
            if (n & 1)
                printf("1");
            else
                printf("0");
        
            n >>= 1;
        }
        printf("  %c\r\n", n);
    }
    printf("*************************\r\n");
    #endif

    valid = true;
    #if WITH_CRC
    {
        // check if CRC match
        int c = crc(pkt, PKT_SIZE - CRC_BYTE);

        #if DEBUG
        printf("CRC checking ...\r\n");
        printf("CRC_in = %d; CRC_check = %d\r\n", int(pkt[PKT_SIZE - CRC_BYTE]), c);
        #endif

        if(pkt[PKT_SIZE - CRC_BYTE] == c) {
            #if DEBUG
            printf("CRC passed\r\n");
            #endif
            valid = true;
        }
        else {
            #if DEBUG
            printf("CRC failed, abort!\r\n");
            #endif
            valid = false;
            return;
        }
    }
    #endif
    
    memcpy(header, pkt, HEADER_BYTE);
    
    #if WITH_CRC
    memcpy(content, pkt + HEADER_BYTE, PKT_SIZE - HEADER_BYTE - CRC_BYTE);
    #else
    memcpy(content, pkt + HEADER_BYTE, PKT_SIZE - HEADER_BYTE);
    #endif
    
    #if DEBUG
    print_data();
    #endif
}

void Packet::init(unsigned char *content, int size, unsigned int nodeId, unsigned int msgId,
                  unsigned int seqNum, bool ifEnd, unsigned int chanNum, unsigned int ttl) {
    // clear out header and content
    memset(this->content, 0, sizeof(this->content));
    memset(this->header, 0, sizeof(this->header));
    // create packet from meta data
    memcpy(this->content, content, size);\
    encode(header, NODE_ID_OFFSET  , NODE_ID_BITS  , nodeId);
    encode(header, MSG_ID_OFFSET   , MSG_ID_BITS   , msgId);
    encode(header, SEQ_NUM_OFFSET  , SEQ_NUM_BITS  , seqNum);
    encode(header, IF_END_OFFSET   , 1             , ifEnd);
    encode(header, CHAN_TYPE_OFFSET, CHAN_TYPE_BITS, chanNum);
    encode(header, TTL_OFFSET      , TTL_BITS      , ttl);

    #if DEBUG
    printf("== New Packet ccreated! ==\r\n");
    print_data();
    #endif
}

Packet::~Packet() {

}

int Packet::to_packet(unsigned char *pkt) {
    // convert to raw packet
    memcpy(pkt, header, HEADER_BYTE);
    memcpy(pkt + HEADER_BYTE, content, sizeof(content));
    #if WITH_CRC
    pkt[PKT_SIZE - CRC_BYTE] = crc(pkt, PKT_SIZE - CRC_BYTE);
    #endif

    // print out raw packet
    #if DEBUG
    printf("*************************\r\n");
    for(int i = 0; i < PKT_SIZE; i++) {
        unsigned char n = pkt[i];
        for(int j = 0; j < 8; j++) {
            if (n & 1)
                printf("1");
            else
                printf("0");
        
            n >>= 1;
        }
        printf("  %c\r\n", n);
    }
    printf("*************************\r\n");
    #endif

    return PKT_SIZE;
}

void Packet::set_node_id(unsigned int id) {
    encode(header, NODE_ID_OFFSET, NODE_ID_BITS, id);
}

void Packet::print_data() {
    // print out meta data
    printf("*************************\r\n");
    printf("Size    : %d\r\n", PAYLOAD_BYTE);
    printf("Node id : %d\r\n", get_node_id());
    printf("Msg id  : %d\r\n", get_msg_id());
    printf("Seq num : %d\r\n", get_seq_num());
    printf("If end  : %d\r\n", get_if_end());
    printf("Channal : %d\r\n", get_chan_type());
    printf("TTL     : %d\r\n", get_ttl());
    printf("Message : %.*s\r\n", PAYLOAD_BYTE, content);
    printf("*************************\r\n");
}

void Packet::decrease_hop() {
    uint8_t ttl = get_ttl() - 1;
    if(ttl == 0xFF) ttl = 0;
    encode(header, TTL_OFFSET, TTL_BITS, ttl);
}

void Packet::set_time_bytes(unsigned int time) {
    // clear the potential dirty bits
    time = time & (~(~(0U) << TIMER_BITS));
    #if WITH_CRC
    memcpy(this->content + (PKT_SIZE - HEADER_BYTE - CRC_BYTE - TIMER_BYTE), &time, TIMER_BYTE);
    #else
    memcpy(this->content + (PKT_SIZE - HEADER_BYTE - TIMER_BYTE), &time, TIMER_BYTE);
    #endif
}

unsigned char * Packet::get_content() {
    return content;
}

unsigned int Packet::get_node_id() {
    return decode(header, NODE_ID_OFFSET, NODE_ID_BITS);
}

unsigned int Packet::get_msg_id() {
    return decode(header, MSG_ID_OFFSET, MSG_ID_BITS);
}

unsigned int Packet::get_seq_num() {
    return decode(header, SEQ_NUM_OFFSET, SEQ_NUM_BITS);
}

bool Packet::get_if_end() {
    if(decode(header, IF_END_OFFSET, 1) == 0)
        return false;
    else
        return true;
}

unsigned int Packet::get_chan_type() {
    return decode(header, CHAN_TYPE_OFFSET, CHAN_TYPE_BITS);
}

unsigned int Packet::get_ttl() {
    return decode(header, TTL_OFFSET, TTL_BITS);
}

bool Packet::if_valid() {
    return valid;
}

unsigned int Packet::get_time_bytes() {
    unsigned int ret = 0;
    #if WITH_CRC
    memcpy(&ret, this->content + (PKT_SIZE - HEADER_BYTE - CRC_BYTE - TIMER_BYTE), TIMER_BYTE);
    #else
    memcpy(&ret, this->content + (PKT_SIZE - HEADER_BYTE - TIMER_BYTE), TIMER_BYTE);
    #endif
    // clear the potential dirty bits
    ret = ret & (~(~(0U) << TIMER_BITS));
    #if DEBUG
    printf("Get timer: %d\n", ret);
    #endif
    return ret;
}

unsigned char Packet::crc_check(unsigned char *pkt, int size, int in_crc) {
    // check CRC match
    #if DEBUG
    printf("CRC checking ...\r\n");
    #endif
    int c = crc(pkt, size);
    if(in_crc == c) {
        #if DEBUG
        printf("CRC passed\r\n");
        #endif
        return true;
    }
    else {
        #if DEBUG
        printf("CRC failed, abort!\r\n");
        #endif
        return false;
    }
}

unsigned char Packet::crc(unsigned char *pkt, int size) {
    // calculate CRC
    int crc = 0;
    for(int i = 0; i < size; i++)
        crc += pkt[i];
    return crc % 256;
}
