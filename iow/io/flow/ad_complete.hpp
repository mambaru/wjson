#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace flow{
 
struct ad_complete
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    t.get_aspect().template gete<_on_complete_>()(t, std::cref(d) );
    t.get_aspect().template get<_confirm_>()(t, std::move(d) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}