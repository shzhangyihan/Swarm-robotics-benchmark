#pragma once

#include "config.h"

#if FUNC
#define new_publisher(func) new_publisher(std::bind(&CLASS::func, this))
#define new_subscriber(dist, func) new_subscriber(dist, std::bind(&CLASS::func, this, \
                                                  std::placeholders::_1, \
                                                  std::placeholders::_2, \
                                                  std::placeholders::_3,\
                                                  std::placeholders::_4))
#endif