#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace pipe{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto p = t.get_aspect().template get< _next_ >()(t);
    t.get_aspect().template get< _some_ >()(t, std::move(p) );
  }
};

}}}