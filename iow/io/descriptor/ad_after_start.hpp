#pragma once

#include <memory>

#include <fas/aop.hpp>
#include <fas/integral/bool_.hpp>
#include <iostream>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_after_start
{
  template<typename T>
  void operator()(T& t)
  {
    IOW_LOG_DEBUG("descriptor::ad_after_start ")
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.startup_handler )
    {
      IOW_LOG_DEBUG("descriptor::ad_after_start startup_handler")
      cntx.startup_handler( t.get_id_(t), cntx.outgoing_handler);
    }
  }
};
  
}}}
