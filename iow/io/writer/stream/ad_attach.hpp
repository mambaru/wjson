#pragma once

#include <iow/io/writer/stream/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace writer{ namespace stream{
  
struct ad_attach
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    auto &buf = t.get_aspect().template get<_write_buffer_>();
    //if ( buf.check(*d) )
    // TODO: проверить на maxsize
    buf.attach(std::move(d));
  }
};


}}}}
