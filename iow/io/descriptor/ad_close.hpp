#pragma once
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_close
{
  template<typename T>
  void operator()(T& t)
  {
    try
    {
      if ( t.descriptor().is_open() )
      {
        t.descriptor().close();
      }
    }
    catch(...)
    {
      abort();
    }
  }
};

}}}
