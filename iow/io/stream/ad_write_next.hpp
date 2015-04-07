#pragma once


#include <iow/io/stream/tags.hpp>
#include <utility>

namespace iow{ namespace io{ namespace stream{
  
struct ad_write_next
{
  template<typename T>
  std::pair<const char*, size_t> operator()(T& t)
  {
    return t.get_aspect().template get<_write_buffer_>().next();
  }
};


}}}
