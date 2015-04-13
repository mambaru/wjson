#pragma once

#include <iow/io/writer/tags.hpp>

namespace iow{ namespace io{ namespace writer{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto p = t.get_aspect().template get< _next_ >()(t);
    if (t.get_aspect().template get< _can_write_ >()(t, p))
    {
      t.get_aspect().template get< _some_ >()(t, std::move(p) );
    }
  }
};

}}}