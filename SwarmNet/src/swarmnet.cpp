#include "swarmnet.h"

Swarmnet::Swarmnet() {
    //new_id();
    roundRobinIndex = 0;
    chan_num = 0;
    forward_size = 0;
}

Swarmnet::~Swarmnet() {
}

void Swarmnet::set_common_sys(Common_system * common_sys) {
    this->common_sys = common_sys;
    new_id();
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
    chans[chan_num].set_common_sys(common_sys);
    chan_num++;
    return &(chans[chan_num-1]);
}

void Swarmnet::new_id() {
    this->nodeId = common_sys->random_func() % NODE_ID_MAX;
}

int Swarmnet::next_pkt(unsigned char * pkt) {
    // no channel created
    if(chan_num == 0) return 0;
    Packet send;
    int flag;
    for(int i = roundRobinIndex; i < roundRobinIndex + chan_num + 1; i++) {
        if(i == chan_num) {
            SWARM_LOG("Pulling from forward buffer");
            if(forward_size == 0) {
                SWARM_LOG("Forward buff empty");
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
            SWARM_LOG("Pulling from channel %d", chans[index].get_type());
            flag = chans[index].next_pkt(&send);
            if(flag == SUCCESS) {
                send.set_node_id(nodeId);
                // printf("send time diff = %u\n", send.get_time_bytes());
                // unsigned char * pkt = send.get_content();
                // for(int i = 0; i < PKT_SIZE - HEADER_BYTE; i++) {
                //     unsigned char n = pkt[i];
                //     for(int j = 0; j < 8; j++) {
                //         if (n & 1)
                //             printf("1");
                //         else
                //             printf("0");
                    
                //         n >>= 1;
                //     }
                //     printf("  %c\r\n", n);
                // }
            }
            else {
                SWARM_LOG("Channel %d buff empty", chans[index].get_type());
            }
        }
        if(flag == SUCCESS) {
            // next packet ready to send
            SWARM_LOG("Next packet ready to send");
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

    int msgId    = newPkt.get_msg_id();
    int seqNum   = newPkt.get_seq_num();
    SWARM_LOG("Pkg received: %d, %d, %d, %d, %d", nodeId, msgId, seqNum, chanNum, ttl);

    if(check_cache(pkt)) {
        // already in cache, abort
        SWARM_LOG("Pkt cached already, abort");
        return;
    }

    // id collision, change self id
    if(nodeId == this->nodeId) {
        new_id();
        SWARM_LOG("NodeId collision, changed to new id: %d", this->nodeId);
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
            //printf("recv diff = %u\n", newPkt.get_time_bytes());
            // printf("recv time diff = %u\n", newPkt.get_time_bytes());
            // unsigned char * pkt = newPkt.get_content();
            // for(int i = 0; i < PKT_SIZE - HEADER_BYTE; i++) {
            //     unsigned char n = pkt[i];
            //     for(int j = 0; j < 8; j++) {
            //         if (n & 1)
            //             printf("1");
            //         else
            //             printf("0");
                
            //         n >>= 1;
            //     }
            //     printf("  %c\r\n", n);
            // }
            chans[i].receive(&newPkt, meta);
        }
    }
    
}

bool Swarmnet::check_cache(unsigned char *header) {
    unsigned int node_id = common::decode(header, NODE_ID_OFFSET, NODE_ID_BITS);
    unsigned int msg_id  = common::decode(header, MSG_ID_OFFSET, MSG_ID_BITS);
    unsigned int seq_num = common::decode(header, SEQ_NUM_OFFSET, SEQ_NUM_BITS);
    unsigned int ttl     = common::decode(header, TTL_OFFSET, TTL_BITS);
    SWARM_LOG("Check cache -- nodeId: %d msg_id: %d seq_num: %d ttl: %d", node_id, msg_id, seq_num, ttl);
    for(int i = 0; i < cache_size; i++) {
        unsigned int check_node_id = common::decode(send_cache[i], NODE_ID_OFFSET, NODE_ID_BITS);
        unsigned int check_msg_id  = common::decode(send_cache[i], MSG_ID_OFFSET, MSG_ID_BITS);
        unsigned int check_seq_num = common::decode(send_cache[i], SEQ_NUM_OFFSET, SEQ_NUM_BITS);
        unsigned int check_ttl     = common::decode(send_cache[i], TTL_OFFSET, TTL_BITS);
        if(node_id == check_node_id && msg_id == check_msg_id && seq_num == check_seq_num) {
            //SWARM_LOG("Cache hit");
            if(ttl > check_ttl) {
                //SWARM_LOG("Need to update ttl\n");
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
    unsigned int node_id = common::decode(header, NODE_ID_OFFSET, NODE_ID_BITS);
    unsigned int msg_id  = common::decode(header, MSG_ID_OFFSET, MSG_ID_BITS);
    unsigned int seq_num = common::decode(header, SEQ_NUM_OFFSET, SEQ_NUM_BITS);
    unsigned int ttl     = common::decode(header, TTL_OFFSET, TTL_BITS);
    SWARM_LOG("Add cache -- nodeId: %d msg_id: %d seq_num: %d ttl: %d cache_buffer: %d/%d", node_id, msg_id, seq_num, ttl, cache_size, CACHE_SIZE);
    if(cache_size >= CACHE_SIZE) {
        // cache full
        //SWARM_LOG("Cache full, discard oldest");
        memmove(send_cache, send_cache + 1, (cache_size - 1) * HEADER_BYTE * sizeof(unsigned char));
        cache_size--;
    }
    memcpy(send_cache + cache_size, header, HEADER_BYTE * sizeof(unsigned char));
    cache_size++;
}
