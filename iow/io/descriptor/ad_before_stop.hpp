#pragma once

#include <iow/io/descriptor/tags.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_before_stop
{
  template<typename T>
  void operator()(T& t)
  {
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.shutdown_handler )
    {
      cntx.shutdown_handler( t.get_id_(t) );
    }
    t.get_aspect().template get<_close_>()(t);
  }
};

}}}
