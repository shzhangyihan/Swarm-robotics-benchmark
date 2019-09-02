#include "swarmnet.h"

Swarmnet::Swarmnet() {
    new_id();
    roundRobinIndex = 0;
    chan_num = 0;
    forward_size = 0;
}

Swarmnet::~Swarmnet() {
}

Channel * Swarmnet::new_channel(int type, int hops, bool listen) {
    if(type > CHAN_TYPE_MAX) {
        // type invalid
        return NULL;
    }
    
    if(chan_num >= TOTAL_CHAN_NUM) {
        // too many channels
        return NULL;
    }
    
    chans[chan_num].init(type, hops, listen);
    chan_num++;
    return &(chans[chan_num-1]);
}

void Swarmnet::new_id() {
    this->nodeId = rand() % NODE_ID_MAX;
}

int Swarmnet::next_pkt(unsigned char * pkt) {
    // no channel created
    if(chan_num == 0) return 0;
    Packet send;
    int flag;
    for(int i = roundRobinIndex; i < roundRobinIndex + chan_num + 1; i++) {
        if(i == chan_num) {
            #if DEBUG
            printf("Pulling from forward buffer\r\n");
            #endif
            if(forward_size == 0) {
                #if DEBUG
                printf("Forward buff empty\r\n");
                #endif
                flag = SUCCESS + 1;
            }
            else {
                memcpy(&send, forwardBuffer, sizeof(Packet));
                memmove(forwardBuffer, forwardBuffer + 1, (forward_size - 1) * sizeof(Packet));
                forward_size--;
                flag = SUCCESS;
            }
        }
        else {
            int index = i % (chan_num + 1);
            #if DEBUG
            printf("Pulling from channel %d\r\n", chans[index].get_type());
            #endif
            flag = chans[index].next_pkt(&send);
            #if DEBUG
            printf("Flag is %d\r\n", flag);
            #endif
            if(flag == SUCCESS) {
                send.set_node_id(nodeId);
            }
            else {
                #if DEBUG
                printf("Channel %d buff empty\n", chans[index].get_type());
                #endif
            }
        }
        if(flag == SUCCESS) {
            // next packet ready to send
            #if DEBUG
            printf("Pkt ready\n");
            #endif
            int size;
            size = send.to_packet(pkt);
            roundRobinIndex = (i + 1) % (chan_num + 1);
            // cache the outgoing packet
            add_cache(pkt);
            return size;
        }
        
    }
    return 0;
}

void Swarmnet::receive(unsigned char * pkt, int size, Meta_t * meta) {
    if(size == 0) return;

    Packet newPkt(pkt);

    // packet parsing failed, abort
    if(! newPkt.if_valid()) return;

    int chanNum = newPkt.get_chan_type();
    int nodeId  = newPkt.get_node_id();
    int ttl     = newPkt.get_ttl();

    #if DEBUG
    int msgId    = newPkt.get_msg_id();
    int seqNum   = newPkt.get_seq_num();
    printf("++++++++++++++++++++++++++++++++++\n");
    printf("Pkg received: %d, %d, %d, %d, %d\n", nodeId, msgId, seqNum, chanNum, ttl);
    printf("++++++++++++++++++++++++++++++++++\n");
    #endif

    if(check_cache(pkt)) {
        // already in cache, abort
        #if DEBUG
        printf("Pkt cached already, abort\n");
        #endif
        return;
    }

    // id collision, change self id
    if(nodeId == this->nodeId) {
        #if DEBUG
        printf("!!!!! NodeId collision !!!!!\n");
        #endif
        new_id();
    }

    // if hopCount > 0, store in forward
    if(ttl > 0) {
        // only cache if forward buffer is not full
        if(forward_size + 1 <= FORWARD_BUFF_SIZE) {
            memcpy(forwardBuffer + forward_size, & newPkt, sizeof(Packet));
            forwardBuffer[forward_size].decrease_hop();
            forward_size++;
        }
    }
    
    for(int i = 0; i < chan_num; i++) {
        if(chans[i].get_type() == chanNum) {
            chans[i].receive(&newPkt, meta);
        }
    }
    
}

bool Swarmnet::check_cache(unsigned char *header) {
    unsigned int node_id = common::decode(header, NODE_ID_OFFSET, NODE_ID_BITS);
    unsigned int msg_id  = common::decode(header, MSG_ID_OFFSET, MSG_ID_BITS);
    unsigned int seq_num = common::decode(header, SEQ_NUM_OFFSET, SEQ_NUM_BITS);
    unsigned int ttl     = common::decode(header, TTL_OFFSET, TTL_BITS);
    #if DEBUG
    printf("Check cache -- nodeId: %d msg_id: %d seq_num: %d ttl: %d \n", node_id, msg_id, seq_num, ttl);
    #endif
    for(int i = 0; i < cache_size; i++) {
        unsigned int check_node_id = common::decode(send_cache[i], NODE_ID_OFFSET, NODE_ID_BITS);
        unsigned int check_msg_id  = common::decode(send_cache[i], MSG_ID_OFFSET, MSG_ID_BITS);
        unsigned int check_seq_num = common::decode(send_cache[i], SEQ_NUM_OFFSET, SEQ_NUM_BITS);
        unsigned int check_ttl     = common::decode(send_cache[i], TTL_OFFSET, TTL_BITS);
        if(node_id == check_node_id && msg_id == check_msg_id && seq_num == check_seq_num) {
            #if DEBUG
            printf("Cache hit\n");
            #endif
            if(ttl > check_ttl) {
                #if DEBUG
                printf("Need to update ttl\n");
                #endif
                memcpy(send_cache[i], header, HEADER_BYTE * sizeof(unsigned char));
                return false;
            }
            return true;
        }
    }

    return false;
}


void Swarmnet::add_cache(unsigned char * header) {
    if(CACHE_SIZE == 0) return;
    #if DEBUG
    unsigned int node_id = decode(header, NODE_ID_OFFSET, NODE_ID_BITS);
    unsigned int msg_id  = decode(header, MSG_ID_OFFSET, MSG_ID_BITS);
    unsigned int seq_num = decode(header, SEQ_NUM_OFFSET, SEQ_NUM_BITS);
    unsigned int ttl     = decode(header, TTL_OFFSET, TTL_BITS);
    printf("Add cache -- nodeId: %d msg_id: %d seq_num: %d ttl: %d cache_buffer: %d/%d\n", node_id, msg_id, seq_num, ttl, cache_size, CACHE_SIZE);
    #endif
    if(cache_size >= CACHE_SIZE) {
        // cache full
        #if DEBUG
        printf("Cache full, discard oldest\n");
        #endif
        memmove(send_cache, send_cache + 1, (cache_size - 1) * HEADER_BYTE * sizeof(unsigned char));
        cache_size--;
    }
    memcpy(send_cache + cache_size, header, HEADER_BYTE * sizeof(unsigned char));
    cache_size++;
}
