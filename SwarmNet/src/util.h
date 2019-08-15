#pragma once

#include "macro.h"

unsigned int decode(unsigned char * header, unsigned int offset, unsigned int len);

void encode(unsigned char * header, unsigned int offset, unsigned int len, unsigned int data);

unsigned int clock_diff(unsigned int t1, unsigned int t2);

void dummy_lock();
unsigned int dummy_clock();

class Base {
    public:
        static void(*lock)();
        static void(*unlock)();
        static unsigned int(*get_clock)();
};