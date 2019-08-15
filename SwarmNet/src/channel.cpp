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
    
    for(int i = 0; i < MAX_PUB_PER_CHAN; i++) sentCallbacks[i] = NULL;
    for(int i = 0; i < MAX_SUB_PER_CHAN; i++)  subscribers[i] = NULL;
}

Channel::~Channel() {
    //printf("Freeing Channel\r\n");
}

int Channel::subscribe(Subscriber * s) {
    for(int i = 0; i < MAX_SUB_PER_CHAN; i++) {
        if(subscribers[i] == NULL) {
            subscribers[i] = s;
            return SUCCESS;
        }
    }
    return ERROR_FULL;
}

#if PYTHON
int Channel::register_send_success(send_caller caller, void * usr_f) {
    this->caller = caller;
    for(int i = 0; i < MAX_PUB_PER_CHAN; i++) {
        if(sentCallbacks[i] == NULL) {
            sentCallbacks[i] = usr_f;
            return SUCCESS;
        }
    }
    return ERROR_FULL;
}
#else
#if FUNC
int Channel::register_send_success(std::function<void()> callback) {
#else
int Channel::register_send_success(void (*callback)()) {
#endif
    for(int i = 0; i < MAX_PUB_PER_CHAN; i++) {
        if(sentCallbacks[i] == NULL) {
            sentCallbacks[i] = callback;
            return SUCCESS;
        }
    }
    return ERROR_FULL;
}
#endif

bool Channel::available() {
    return ready;
}

bool Channel::overflow_status() {
    return overflowFlag;
}

void Channel::print_channel_status() {
    printf("Channel %d\r\n", get_type());
    printf("Send Buffer Status: (%d / %d)\r\n", (send_pktNum - send_index), MAX_BUFF);
    /*
    for(int i =  send_index; i < send_pktNum; i++) {
        printf("")
    }*/
    printf("Recv Buffer Status: (%d / %d)\r\n", recv_pktNum, BUFF_SIZE_PER_CHAN);
}

int Channel::send(unsigned char * msg, int msgSize) {
    if(!this->ready) {
        // buffer not ready to send
        return ERROR_NOT_READY;
    }
    
    int msgId = rand() % MAX_MSG_ID;
    int payloadSize;
    int header_byte = HEADER_BYTE;
    unsigned int cur_time = get_clock();
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
    this->ready = false;
    this->send_pktNum = totalPkt;
    this->send_index = 0;
    return SUCCESS;
}

int Channel::next_pkt(Packet *ret) {
    // buffer empty
    #if DEBUG
    printf("send_pktNum = %d\r\n", send_pktNum);
    #endif
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
        ready = true;
        // callback on all sents
        for(int i = 0; i < MAX_PUB_PER_CHAN; i++)
            if(sentCallbacks[i] != NULL)
                #if PYTHON
                this->caller(sentCallbacks[i]);
                #else
                sentCallbacks[i]();
                #endif
    }
    if(time_chan) {
        unsigned int cur_time = get_clock();
        unsigned int old_time = ret->get_time_bytes();
        unsigned int diff_time = clock_diff(old_time, cur_time);
        ret->set_time_bytes(diff_time);
    }

    return SUCCESS;
}

void Channel::receive(Packet * newPkt, Meta_t * meta) {
    int nodeId = newPkt->get_node_id();
    int msgId  = newPkt->get_msg_id();
    int seqNum = newPkt->get_seq_num();
    int ttl    = newPkt->get_ttl();
    
    for(int i = recv_pktNum - 1; i > 0; i--) {
        if(recvBuffer[i].get_node_id() == nodeId &&
           recvBuffer[i].get_msg_id()  == msgId  &&
           recvBuffer[i].get_seq_num() == seqNum &&
           recvBuffer[i].get_ttl()     == ttl) {
            // delete duplicate
            #if DEBUG
            printf("Find duplicate at %d\n", i);
            #endif
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
        unsigned int cur_time = get_clock();
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
    }
    recv_pktNum++;
    #if DEBUG
    printf("Pkt num after insert: %d\n", recv_pktNum);
    #endif

    try_merge(nodeId, msgId, ttl, meta);
}

int Channel::get_type() {
    return type;
}

void Channel::try_merge(int nodeId, int msgId, int ttl, Meta_t * meta) {
    #if DEBUG
    printf("Try merge!!\n");
    printf("Buffer:\n");
    #endif
    for(int i = 0; i < MAX_BUFF; i++)
        assembler[i] = -1;
    
    for(int i = 0; i < recv_pktNum; i++) {
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
        #if DEBUG
        printf("%d at %d\n", i, assembler[i]);
        #endif
        final_pkt = i;
    }
    
    #if DEBUG
    printf("Final pkt at %d; if_end? %d\n", final_pkt, recvBuffer[assembler[final_pkt]].get_if_end());
    #endif
    
    unsigned int longest_diff_time = 0;
    unsigned int cur_time = get_clock();
    // if all continuous and is finished, then merge
    if(final_pkt != -1 && recvBuffer[assembler[final_pkt]].get_if_end()) {
        #if DEBUG
        printf("!!! Start merging msg !!!\n");
        printf("Total size: %d\n", final_pkt + 1);
        #endif
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
                unsigned int diff_time = clock_diff(prev_time, cur_time);
                if(diff_time > longest_diff_time) longest_diff_time = diff_time;
            }
        }

        #if DEBUG
        printf("$$$$$$$ Msg formed $$$$$$$\n");
        for(int i = 0; i < msgSize; i++) {
            printf("%d %c\n", i, msg[i]);
        }
        #endif
        if(time_chan) {
            printf("Timer channel msg formed, time diff = %d\n", longest_diff_time);
        }
        overflowCounter = 0;
        overflowFlag = false;
        
        #if DEBUG
        printf("?????   BEFORE REVERSELY DELETE\n");
        // reversely delete the pkts
        for(int i = 0; i < (final_pkt + 1); i++) {
            printf("Assembler[%d] = %d\n", i, assembler[i]);
        }
        #endif
        for(int i = 0; i < (final_pkt + 1); i++) {
            int max_index = -1;
            int ass_index = -1;
            for(int j = 0; j < (final_pkt + 1); j++) {
                if(assembler[j] > max_index) {
                    ass_index = j;
                    max_index = assembler[j];
                }
            }
            #if DEBUG
            printf("Max index = %d, Ass index = %d, pkt Num = %d\n", max_index, ass_index, recv_pktNum);
            #endif
            assembler[ass_index] = -1;
            memmove(recvBuffer + max_index, recvBuffer + max_index + 1, 
                    (recv_pktNum - max_index - 1) * sizeof(Packet));
            recv_pktNum--;
        }
        #if DEBUG
        printf("!!!!!   AFTER REVERSELY DELETE\n");
        #endif
                
        // callback on all recvs
        for(int i = 0; i < MAX_SUB_PER_CHAN; i++) {
            if(subscribers[i] != NULL) {
                // TODO:: assuming /0 terminating
                #if DEBUG
                printf("Callback on sub %d\r\n", i);
                #endif
                subscribers[i]->receive(msg, msgSize, ttl, meta);
            }
        }
    }
}


