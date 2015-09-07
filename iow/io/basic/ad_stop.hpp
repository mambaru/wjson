#pragma once

#include <iow/io/basic/tags.hpp>

namespace iow{ namespace io{ namespace basic{
  
struct ad_stop
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template gete<_before_stop_>()(t);
    t.get_aspect().template get<_reset_>()(t);
    t.get_aspect().template get<_context_>().status=false;
    t.get_aspect().template gete<_after_stop_>()(t);
  }
};

  
}}}
