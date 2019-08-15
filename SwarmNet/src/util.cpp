#include "util.h"

unsigned int decode(unsigned char * header, unsigned int offset, unsigned int len) {
    unsigned int ret = 0;
    unsigned int end_bit = offset + len - 1;
    unsigned int cur_index = offset / BYTE;
    unsigned int end_index = end_bit / BYTE;
    unsigned char mask;
    unsigned int front, rear;
    //printf("start bit: %d; end bit: %d; start index: %d; end index: %d\n", offset, end_bit, cur_index, end_index);
    while(cur_index <= end_index) {
        front = offset - cur_index * BYTE;
        if(cur_index == end_index) {
            ret = ret << (end_bit - offset + 1);
            rear = (cur_index + 1) * BYTE - 1 - end_bit;
        }
        else {
            ret = ret << ((cur_index + 1) * BYTE - offset);
            rear = 0;
        }
        mask = ((0xFF >> front) >> rear) << rear;
        ret = ret + ((header[cur_index] & mask) >> rear);
        cur_index++;
        offset = cur_index * BYTE;
        //printf("ret = %d\n", ret);
    }
    
    return ret;
}

void encode(unsigned char * header, unsigned int offset, unsigned int len, unsigned int data) {
    int end_bit = offset + len - 1;
    int start_index = offset / BYTE;
    int cur_index = end_bit / BYTE;
    unsigned char mask;
    int front, rear;
    while(cur_index >= start_index) {
        rear = (cur_index + 1) * BYTE - 1 - end_bit;
        if(cur_index == start_index) {
            front = offset - start_index * BYTE;
        }
        else {
            front = 0;
        }
        mask = ((0xFF >> front) >> rear) << rear;
        header[cur_index] = header[cur_index] & ((unsigned char)~mask);
        header[cur_index] = header[cur_index] + ((data & (~(0xFF << (BYTE - front - rear)))) << rear);
        data = data >> (BYTE - front - rear);
        cur_index--;
        end_bit = (cur_index + 1) * BYTE - 1;
    }
}

unsigned int clock_diff(unsigned int start, unsigned int end) {
    if(start > end) {
        // based on assumption that if start overflows, end won't overflow
        // assuming the msg processing won't take as long as entire clock overflow 
        end = end + TIMER_MAX;
    }
    return end - start;
}

void dummy_lock() {}

unsigned int dummy_clock() { return 0; }

void (*Base::lock)() = dummy_lock;

void (*Base::unlock)() = dummy_lock;

unsigned int (*Base::get_clock)() = dummy_clock;
