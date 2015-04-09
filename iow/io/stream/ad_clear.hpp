#pragma once

#include <iow/io/stream/tags.hpp>

namespace iow{ namespace io{ namespace stream{
  
struct ad_clear
{
  template<typename T>
  void operator()(T& t )
  {
    //t.get_aspect().template get<_write_buffer_>().clear();
    //t.get_aspect().template get<_read_buffer_>().clear();
  }
};


}}}
