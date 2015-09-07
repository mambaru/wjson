#pragma once
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_close
{
  template<typename T>
  void operator()(T& t)
  {
    IOW_LOG_DEBUG("descriptor::ad_close")
    t.descriptor().close();
  }
};

}}}
