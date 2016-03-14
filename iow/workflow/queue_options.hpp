#pragma once 

#include <cstddef>
#include <functional>

namespace iow {
  
struct queue_options
{
  typedef std::function< void(size_t, size_t) > handler_type;
  size_t maxsize = 1024*1024*128;
  size_t wrnsize = 1024*1024;
  handler_type wrn_handler = nullptr;
  handler_type max_handler = nullptr;
};

}