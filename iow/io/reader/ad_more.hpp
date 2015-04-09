#pragma once

#include <iow/io/reader/tags.hpp>

namespace iow{ namespace io{ namespace reader{
  
struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< _next_ >()(t);
    t.get_aspect().template get< _some_ >()(t, std::move(d) );
  }
};

}}}