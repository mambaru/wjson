#pragma once

#include <iow/io/basic/tags.hpp>

namespace iow{ namespace io{ namespace basic{

struct ad_shutdown
{
  template<typename T, typename Handler>
  void operator()(T& t, Handler&& shutdown_complete)
  {
    t.get_aspect().template get<_stop_>()(t);
    if ( shutdown_complete != nullptr )
    {
      shutdown_complete( t.get_id_(t) );
    }
  }
};
 
}}}
