#include "channel.h"


Channel::Channel() {
    init(0, 0, false);
}


Channel::Channel(int type, int hops, bool time_chan) {
    init(type, hops, time_chan);
}

void Channel::init(int type, int hops, bool time_chan) {
    this->type = type;
    this->hops = hops;
    this->time_chan = time_chan;
    this->overflowCounter = 0;
    this->overflowFlag = false;
    this->send_index = 0;
    this->send_pktNum = 0;
    this->recv_pktNum = 0;
    this->ready = true;
}

Channel::~Channel() {
}

void Channel::set_common_sys(Common_system * common_sys) {
    this->common_sys = common_sys;
}

#if FUNC
Subscriber * Channel::new_subscriber(int dist, std::function<void(unsigned char *, int, int, Meta_t *)> callback) {
#else
Subscriber * Channel::new_subscriber(int dist, void (*callback)(unsigned char *, int, int, Meta_t *)) {
#endif
    for(int i = 0; i < MAX_SUB_PER_CHAN; i++) {
        if(subscribers[i].if_initialized() == false) {
            subscribers[i].subscriber_init(dist, callback);
            return &(subscribers[i]);
        }
    }
    return NULL;
}

#if FUNC
Publisher * Channel::new_publisher(std::function<void()> callback) {
#else
Publisher * Channel::new_publisher(void (*callback)()) {
#endif
    for(int i = 0; i < MAX_PUB_PER_CHAN; i++) {
        if(publishers[i].if_initialized() == false) {
            publishers[i].publisher_init(this, callback);
            return &(publishers[i]);
        }
    }
    return NULL;
}

bool Channel::available() {
    return ready;
}

bool Channel::overflow_status() {
    return overflowFlag;
}

void Channel::print_channel_status() {
    SWARM_LOG("Channel %d", get_type());
    SWARM_LOG("Send Buffer Status: (%d / %d)", (send_pktNum - send_index), MAX_BUFF);
    SWARM_LOG("Recv Buffer Status: (%d / %d)", recv_pktNum, BUFF_SIZE_PER_CHAN);
}

int Channel::send(unsigned char * msg, int msgSize) {
    if(!this->ready) {
        // buffer not ready to send
        SWARM_LOG("Send failed, due to channel not ready");
        return ERROR_NOT_READY;
    }
    
    int msgId = common_sys->random_func() % MAX_MSG_ID;
    int payloadSize;
    int header_byte = HEADER_BYTE;
    unsigned int cur_time = common_sys->get_clock();
    if(time_chan) {
        header_byte = header_byte + TIMER_BYTE;
    }
    
    #if WITH_CRC
    payloadSize = PKT_SIZE - header_byte - CRC_BYTE;
    #else
    payloadSize = PKT_SIZE - header_byte;
    #endif
    
    // round up msgSize / payloadSize
    int totalPkt = (msgSize + payloadSize - 1) / payloadSize;
    if(totalPkt > MAX_BUFF) {
        // msg too long
        SWARM_LOG("Send failed, due to message too long");
        return ERROR_TOO_LONG;
    }
    
    for(int i = 0; i < totalPkt; i++) {
        if(i != totalPkt - 1) {
            // not last pkt
            sendBuffer[i].init(msg + i * payloadSize, payloadSize, DUMMY_ID,
                               msgId, i , false, type, hops);
        }
        else {
            // last pkt
            sendBuffer[i].init(msg + i * payloadSize, msgSize - i * payloadSize,
                               DUMMY_ID, msgId, i, true, type, hops);
        }
        if(time_chan) {
            sendBuffer[i].set_time_bytes(cur_time);
        }
    }
    SWARM_LOG("Set ready to false");
    this->ready = false;
    this->send_pktNum = totalPkt;
    this->send_index = 0;
    return SUCCESS;
}

int Channel::next_pkt(Packet *ret) {
    // buffer empty
    SWARM_LOG("Start fetching next packet %d from channel %d", send_pktNum, this->get_type());
    if(send_pktNum == 0) return ERROR_EMPTY;
    
    memcpy(ret, &sendBuffer[send_index], sizeof(Packet));
    if(send_index + 1 < send_pktNum) {
        //sendBuffer[send_index] = NULL;
        send_index++;
    }
    else {
        // finish sending the entire msg
        send_index = 0;
        send_pktNum = 0;
        SWARM_LOG("Set ready to true");
        ready = true;
        // callback on all sents
        for(int i = 0; i < MAX_PUB_PER_CHAN; i++)
            if(publishers[i].if_initialized() == true)
                publishers[i].sent_callback();
    }
    if(time_chan) {
        unsigned int cur_time = common_sys->get_clock();
        unsigned int old_time = ret->get_time_bytes();
        unsigned int diff_time = common::clock_diff(old_time, cur_time);
        ret->set_time_bytes(diff_time);
        //printf("send diff time: %u\n", ret->get_time_bytes());
    }

    return SUCCESS;
}

void Channel::receive(Packet * newPkt, Meta_t * meta) {
    int nodeId = newPkt->get_node_id();
    int msgId  = newPkt->get_msg_id();
    int seqNum = newPkt->get_seq_num();
    int ttl    = newPkt->get_ttl();
    printf("recv diff = %u\n", newPkt->get_time_bytes());
    SWARM_LOG("Received message with node_id %d msg_id %d seq_num %d", nodeId, msgId, seqNum);
    for(int i = recv_pktNum - 1; i > 0; i--) {
        if(recvBuffer[i].get_node_id() == nodeId &&
           recvBuffer[i].get_msg_id()  == msgId  &&
           recvBuffer[i].get_seq_num() == seqNum &&
           recvBuffer[i].get_ttl()     == ttl) {
            // delete duplicate
            SWARM_LOG("Find duplicate at %d", i);
            if(i != recv_pktNum - 1)
                // dont need to move if its the last pkt
                memmove(recvBuffer + i, recvBuffer + i + 1, 
                        (recv_pktNum - i - 1) * sizeof(Packet));
            recv_pktNum--;
        }
    }
    
    if(recv_pktNum >= BUFF_SIZE_PER_CHAN) {
        memmove(recvBuffer, recvBuffer + 1, (recv_pktNum - 1) * sizeof(Packet));
        overflowCounter++;
        if(overflowCounter >= BUFF_SIZE_PER_CHAN) {
            // entire buffer has been refreshed
            overflowFlag = true;
        }
        recv_pktNum--;
    }
    
    // add new packet to buffer
    memcpy(recvBuffer + recv_pktNum, newPkt, sizeof(Packet));
    // update timer if needed
    if(time_chan) {
        unsigned int cur_time = common_sys->get_clock();
        unsigned int prev_time = recvBuffer[recv_pktNum].get_time_bytes();
        unsigned int diff_time = 0;
        if(cur_time < prev_time) {
            // overflow detection for (cur_time - read_time)
            diff_time = TIMER_MAX - (prev_time - cur_time);
        }
        else {
            diff_time = cur_time - prev_time;
        }
        recvBuffer[recv_pktNum].set_time_bytes(diff_time);
        //printf("recv pkt + diff time: %u, diff time = %u\n", recvBuffer[recv_pktNum].get_time_bytes(), prev_time);
    }
    recv_pktNum++;
    SWARM_LOG("Pkt num after insert: %d", recv_pktNum);

    try_merge(nodeId, msgId, ttl, meta);
}

int Channel::get_type() {
    return type;
}

void Channel::try_merge(int nodeId, int msgId, int ttl, Meta_t * meta) {
    SWARM_LOG("Try merge!!");
    for(int i = 0; i < MAX_BUFF; i++)
        assembler[i] = -1;
    
    for(int i = 0; i < recv_pktNum; i++) {
        SWARM_LOG("Inspecting packet node_id %d msg_id %d seq_num %d if_end %d", recvBuffer[i].get_node_id() ,recvBuffer[i].get_msg_id(), recvBuffer[i].get_seq_num(), recvBuffer[i].get_if_end())
        if(recvBuffer[i].get_node_id() == nodeId &&
           recvBuffer[i].get_msg_id()  == msgId  &&
           recvBuffer[i].get_ttl()     == ttl) {
            if(assembler[recvBuffer[i].get_seq_num()] == -1)
               assembler[recvBuffer[i].get_seq_num()] = i;
        }
    }
    
    int final_pkt = -1;
    for(int i = 0; i < MAX_BUFF; i++) {
        if(assembler[i] == -1) {
            break;
        }
        SWARM_LOG("%d at %d", i, assembler[i]);
        final_pkt = i;
    }
    
    SWARM_LOG("Final pkt at %d; if_end? %d", final_pkt, recvBuffer[assembler[final_pkt]].get_if_end());
    
    unsigned int longest_diff_time = 0;
    unsigned int cur_time = common_sys->get_clock();
    // if all continuous and is finished, then merge
    if(final_pkt != -1 && recvBuffer[assembler[final_pkt]].get_if_end()) {
        SWARM_LOG("!!! Start merging msg !!!");
        SWARM_LOG("Total size: %d", final_pkt + 1);
        int payload_byte = PAYLOAD_BYTE;
        if(time_chan) {
            payload_byte = payload_byte - TIMER_BYTE;
        }
        int msgSize = (final_pkt + 1) * payload_byte;
        unsigned char msg[msgSize];
        for(int i = 0; i < final_pkt + 1; i++) {
            memcpy(msg + i * payload_byte, recvBuffer[assembler[i]].get_content(), payload_byte);
            if(time_chan) {
                // measure the time diff from the time bytes
                unsigned int prev_time = recvBuffer[assembler[i]].get_time_bytes();
                unsigned int diff_time = common::clock_diff(prev_time, cur_time);
                printf("when merge, %u - %u = %u\n", cur_time, prev_time, diff_time);
                if(diff_time > longest_diff_time) {
                    printf("new longest diff time: %d\n", diff_time);
                    longest_diff_time = diff_time;
                }
            }
        }

        SWARM_LOG("Msg formed: %.*s", msgSize, msg);
        /*
        for(int i = 0; i < msgSize; i++) {
            printf("%d %c\n", i, msg[i]);
        }
        */
        if(time_chan) {
            SWARM_LOG("Timer channel msg formed, time diff = %d", longest_diff_time);
            meta->msg_delay = longest_diff_time;
        }
        else {
            meta->msg_delay = 0;
        }
        overflowCounter = 0;
        overflowFlag = false;
        /*
        printf("?????   BEFORE REVERSELY DELETE\n");
        // reversely delete the pkts
        for(int i = 0; i < (final_pkt + 1); i++) {
            printf("Assembler[%d] = %d\n", i, assembler[i]);
        }
        */
        for(int i = 0; i < (final_pkt + 1); i++) {
            int max_index = -1;
            int ass_index = -1;
            for(int j = 0; j < (final_pkt + 1); j++) {
                if(assembler[j] > max_index) {
                    ass_index = j;
                    max_index = assembler[j];
                }
            }
            /*
            printf("Max index = %d, Ass index = %d, pkt Num = %d\n", max_index, ass_index, recv_pktNum);
            */
            assembler[ass_index] = -1;
            memmove(recvBuffer + max_index, recvBuffer + max_index + 1, 
                    (recv_pktNum - max_index - 1) * sizeof(Packet));
            recv_pktNum--;
        }
                
        // callback on all recvs
        for(int i = 0; i < MAX_SUB_PER_CHAN; i++) {
            if(subscribers[i].if_initialized() == true) {
                // TODO:: assuming /0 terminating
                SWARM_LOG("Callback on sub %d", i);
                subscribers[i].receive(msg, msgSize, ttl, meta);
            }
        }
    }
}


