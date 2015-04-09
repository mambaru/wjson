#pragma once

#include <iow/io/reader/tags.hpp>

namespace iow{ namespace io{ namespace reader{
  
struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}