#pragma once

#include <iow/io/descriptor/tags.hpp>
#include <iow/logger/logger.hpp>
namespace iow{ namespace io{ namespace descriptor{ 

struct ad_before_stop
{
  template<typename T>
  void operator()(T& t)
  {
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.shutdown_handler )
    {
      IOW_LOG_DEBUG("ad_before_stop shutdown_handler =1=")
      cntx.shutdown_handler( t.get_id_(t) );
      IOW_LOG_DEBUG("ad_before_stop shutdown_handler =2=")
    }
    t.get_aspect().template get<_close_>()(t);
  }
};

}}}
