#pragma once

#include <iow/io/stream/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace stream{
  
struct ad_write_attach
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    auto &buf = t.get_aspect().template get<_write_buffer_>();
    //if ( buf.check(*d) )
    // TODO: проверить на maxbuf
    buf.attach(std::move(d));
  }
};


}}}
