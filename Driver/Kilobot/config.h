#pragma once

/* call back meta data struct */
typedef struct Meta {
	int dist;
} Meta_t;

/* functional parameter */
#define DEBUG true
#define FUNC false
#define CLASS mykilobot

/* protocal config */
#define PKT_SIZE       8
#define NODE_ID_BITS   8
#define MSG_ID_BITS    4
#define SEQ_NUM_BITS   3
#define CHAN_TYPE_BITS 4
#define TTL_BITS       4
#define WITH_CRC       false
#define TIMER_BITS     32

/* local memory bounding */
#define TOTAL_CHAN_NUM      1
#define BUFF_SIZE_PER_CHAN  20
#define FORWARD_BUFF_SIZE   10
#define MAX_SUB_PER_CHAN    1
#define MAX_PUB_PER_CHAN    1
#define CACHE_SIZE          5
