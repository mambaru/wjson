#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace flow{
  
struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}