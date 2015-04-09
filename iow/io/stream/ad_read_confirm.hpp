#pragma once

#include <iow/io/stream/tags.hpp>

namespace iow{ namespace io{ namespace stream{

  /*
struct ad_read_confirm
{
  template<typename T>
  void operator()(T& t, std::pair<char*, size_t> p)
  {
    bool result = t.get_aspect().template get<_read_buffer_>().confirm(p);
    if ( !result )
    {
      // read error
      // t.get_aspect().template get<_error_>(t, "")
    }
  }
};
*/
}}}
