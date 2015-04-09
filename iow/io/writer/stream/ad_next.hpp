#pragma once


#include <iow/io/writer/stream/tags.hpp>
#include <utility>

namespace iow{ namespace io{ namespace writer{ namespace stream{
  
struct ad_next
{
  template<typename T>
  std::pair<const char*, size_t> operator()(T& t)
  {
    return t.get_aspect().template get<_write_buffer_>().next();
  }
};


}}}}
