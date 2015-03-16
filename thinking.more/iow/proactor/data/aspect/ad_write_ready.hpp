#pragma once

#include <iow/proactor/data/aspect/tags.hpp>

namespace iow{ namespace proactor{
 
struct ad_write_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    /*
    t.get_aspect().template gete<_on_write_>()(t, &d);
    if ( d->empty() )
    {
      t.get_aspect().template get<_write_more_>()(t, nullptr );
      t.get_aspect().template get<_free_data_>()(t, nullptr );
    }
    else
    {
      t.get_aspect().template get<_write_some_>()(t, std::move(d) );
    }
    */
  }
};

}}