#pragma once

#include <iow/io/stream/tags.hpp>

namespace iow{ namespace io{ namespace stream{
  
struct ad_read_handler
{
  template<typename T>
  void operator()(T& t)
  {
    auto& buf = t.get_aspect().template get<_read_buffer_>();
    while (auto d = buf.detach() )
    {
      if (d->empty() )
      {
        // Кривая ошибка
        // t.get_aspect().template get<_aaaa_>()(t, "");
      }
      t.get_aspect().template get<_handler_>()(t, std::move(d) );
    }
  }
};


}}}
