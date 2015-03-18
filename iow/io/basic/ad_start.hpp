#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/io/io_id.hpp>

namespace iow{ namespace io{ namespace basic{
  
struct ad_start
{
  template<typename T, typename O>
  void operator()(T& t, O&& options)
  {
    t.get_aspect().template get<_io_id_>() = create_id< typename T::io_id_t>();
    t.get_aspect().template gete<_initialize_>()(t, options );
    t.get_aspect().template get<_status_>()=true;
    t.get_aspect().template gete<_after_start_>()(t);
  }
};
 
}}}
