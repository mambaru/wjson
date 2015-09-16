#pragma once
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_close
{
  template<typename T>
  void operator()(T& t)
  {
    IOW_LOG_DEBUG("BEGIN descriptor::ad_close ")
    try
    {
      if ( t.descriptor().is_open() )
      {
        t.descriptor().close();
      }
    }
    catch(...)
    {
      IOW_LOG_DEBUG("END descriptor::ad_close try!")
      abort();
    }
    IOW_LOG_DEBUG("END descriptor::ad_close")
  }
};

}}}
