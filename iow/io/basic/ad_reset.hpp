#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/logger/logger.hpp>
namespace iow{ namespace io{ namespace basic{
  
struct ad_reset
{
  template<typename T>
  void operator()(T& t)
  {
    IOW_LOG_DEBUG("basic::ad_reset -1-... " << size_t(&t))
    t.get_aspect().template get<_context_>().holder.reset();
    IOW_LOG_DEBUG("basic::ad_reset -2-...")
    t.get_aspect().template gete<_after_reset_>()(t);
    IOW_LOG_DEBUG("basic::ad_reset -3- Done")
  }
};
  
}}}
