#pragma once

#include <iow/io/stream/tags.hpp>

namespace iow{ namespace io{ namespace stream{


struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O opt)
  {
    //t.get_aspect().template get<_buffer_pool_>() = opt.buffer_pool;
    t.get_aspect().template get<_write_buffer_>().set_options(opt.writer);
    t.get_aspect().template get<_read_buffer_>().set_options(opt.reader);
  }
};


}}}
