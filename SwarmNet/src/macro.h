#pragma once

#include "config.h"

#define BYTE           8U
#define HEADER_BYTE    ((NODE_ID_BITS + MSG_ID_BITS + SEQ_NUM_BITS + CHAN_TYPE_BITS + TTL_BITS + 1 + BYTE - 1) / BYTE)
#define PAYLOAD_BYTE   (PKT_SIZE - HEADER_BYTE - (WITH_CRC? 1: 0))
#define CRC_BYTE       1U
#define TIMER_BYTE     (TIMER_BITS % BYTE == 0? TIMER_BITS / BYTE: TIMER_BITS / BYTE + 1)

#define NODE_ID_MAX    ((1U << NODE_ID_BITS) - 1)
#define MSG_ID_MAX     ((1U << MSG_ID_BITS) - 1)
#define SEQ_NUM_MAX    ((1U << SEQ_NUM_BITS) - 1)
#define CHAN_TYPE_MAX  ((1U << CHAN_TYPE_BITS) - 1)
#define TTL_MAX        ((1U << TTL_BITS) - 1)
#define TIMER_MAX      ((1U << TIMER_BITS) - 1)

#define NODE_ID_OFFSET   0
#define MSG_ID_OFFSET    (NODE_ID_OFFSET + NODE_ID_BITS)
#define SEQ_NUM_OFFSET   (MSG_ID_OFFSET + MSG_ID_BITS)
#define IF_END_OFFSET    (SEQ_NUM_OFFSET + SEQ_NUM_BITS)
#define CHAN_TYPE_OFFSET (IF_END_OFFSET + 1)
#define TTL_OFFSET       (CHAN_TYPE_OFFSET + CHAN_TYPE_BITS)

#define MAX_PKT_PER_MSG  (SEQ_NUM_MAX + 1)

#define SINGLE_PKT_MEM   (HEADER_BYTE + PAYLOAD_BYTE + 1)
#define BUFF_MEM_USAGE   ((BUFF_SIZE_PER_CHAN * TOTAL_CHAN_NUM + FORWARD_BUFF_SIZE * PKT) * PKT_MEM_USAGE)
#define CACHE_MEM_USAGE  (CACHE_SIZE * HEADER_BYTE)
#define TOTAL_MEM_USAGE  (BUFF_MEM_USAGE + CACHE_MEM_USAGE)

#if DEBUG
#define SWARM_LOG(...) printf("(%s) ", __PRETTY_FUNCTION__); printf(__VA_ARGS__); printf("\r\n");
#else
#define SWARM_LOG(...)
#endif