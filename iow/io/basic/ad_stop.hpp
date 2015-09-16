#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace basic{
  
struct ad_stop
{
  template<typename T>
  void operator()(T& t)
  {
    IOW_LOG_DEBUG("basic::ad_stop -1-...")
    t.get_aspect().template gete<_before_stop_>()(t);
    IOW_LOG_DEBUG("basic::ad_stop -2-...")
    t.get_aspect().template get<_reset_>()(t);
    IOW_LOG_DEBUG("basic::ad_stop -3-...")
    t.get_aspect().template get<_context_>().status=false;
    IOW_LOG_DEBUG("basic::ad_stop -4-...")
    t.get_aspect().template gete<_after_stop_>()(t);
    IOW_LOG_DEBUG("basic::ad_stop -5- Done")
  }
};

  
}}}
