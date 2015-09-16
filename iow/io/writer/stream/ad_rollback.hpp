#pragma once

#include <iow/io/writer/stream/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace writer{ namespace stream{

struct ad_rollback
{
  template<typename T, typename P>
  void operator()(T& t, P /*p*/)
  {
    t.get_aspect().template get<_write_buffer_>().rollback();
  }
};

}}}}
