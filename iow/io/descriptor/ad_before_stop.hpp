#pragma once

#include <iow/io/descriptor/tags.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_before_stop
{
  // TODO: shutdown_handler
  // TODO: в дескриптор
  template<typename T>
  void operator()(T& t)
  {
    //auto& cntx_basic = t.get_aspect().template get< ::iow::io::basic::_context_ >();
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.shutdown_handler )
    {
      cntx.shutdown_handler( t.get_id_(t) );
    }
    t.descriptor().close();
  }
};


}}}
