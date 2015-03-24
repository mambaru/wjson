#pragma once

#include <iow/io/flow/tags.hpp>
#include <functional>

namespace iow{ namespace io{ namespace pipe{
 
struct ad_complete
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    t.get_aspect().template gete<_on_complete_>()(t, std::cref(p) );
    t.get_aspect().template get< _confirm_ >()(t, std::move(p) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}
