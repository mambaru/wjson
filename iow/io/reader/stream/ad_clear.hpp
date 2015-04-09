#pragma once

#include <iow/io/reader/stream/tags.hpp>

namespace iow{ namespace io{ namespace reader{ namespace stream{
  
struct ad_clear
{
  template<typename T>
  void operator()(T& t )
  {
    t.get_aspect().template get<_read_buffer_>().clear();
  }
};


}}}}
