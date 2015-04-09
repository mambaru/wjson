#pragma once

#include <iow/io/stream/tags.hpp>
#include <utility>


namespace iow{ namespace io{ namespace stream{
  
/*
struct ad_write_confirm
{
  template<typename T>
  void operator()(T& t, std::pair<const char*, size_t> p)
  {
    bool result = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( !result )
    {
      // Все закртыть, и выдать в log
    }
  }
};
*/

}}}
