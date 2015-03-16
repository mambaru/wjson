#pragma once

#include <iow/proactor/aspect/tags.hpp>

namespace iow{ namespace proactor{
 
struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<_on_ready_>()(t, std::ref(*d) );
    t.get_aspect().template get<_handler_>()(t, std::move(d) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}