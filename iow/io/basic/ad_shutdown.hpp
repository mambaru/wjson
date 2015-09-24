#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace basic{

struct ad_shutdown
{
  template<typename T>
  void operator()(T& t, std::function<void(io_id_t)>&& shutdown_complete)
  {
    t.get_aspect().template get<_stop_>()(t);
    if ( auto sc = shutdown_complete )
    {
      t.mutex().unlock();
      try
      {
        
        sc( t.get_id_(t) );
      }
      catch(...)
      {
        IOW_LOG_ERROR("ad_shutdown unhandled exception")
      }
      t.mutex().lock();
    }
  }
};
 
}}}
