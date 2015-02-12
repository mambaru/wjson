#pragma once

#include <iow/pipeline/aspect/tags.hpp>

namespace iow{ 
 
struct ad_read_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<_on_read_>()(t, &d);
    t.get_aspect().template get<_read_loop_>()(t, std::move(d) );
  }
};

}