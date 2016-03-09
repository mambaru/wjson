#pragma once 

#include <cstddef>

namespace iow {
  
struct queue_options
{
  size_t maxsize = 1024*1024*128;
  size_t wrnsize = 1024*1024;
};

}