#pragma once

#include <iow/io/reader/stream/tags.hpp>

namespace iow{ namespace io{ namespace reader{ namespace stream{


struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    t.get_aspect().template get<_read_buffer_>().set_options(opt);
  }
};


}}}}
