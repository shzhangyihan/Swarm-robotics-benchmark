#include "_config.h"

#if FUNC
#define publisher_init(chan, func) publisher_init(chan, std::bind(&CLASS::func, this));
#define subscriber_init(dist, func) subscriber_init(dist, std::bind(&CLASS::func, this, \
                                             std::placeholders::_1, \
                                             std::placeholders::_2, \
                                             std::placeholders::_3,\
                                             std::placeholders::_4));
#endif
