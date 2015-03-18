#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace flow{
 
struct ad_complete
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    t.get_aspect().template gete<_on_complete_>()(t, std::ref(*d) );
    t.get_aspect().template get<_handler_>()(t, std::move(d) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}