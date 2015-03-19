#pragma once

#include <iow/io/flow/tags.hpp>
#include <functional>

namespace iow{ namespace io{ namespace pipe{
 
struct ad_complete
{
  template<typename T>
  void operator()(T& t, typename T::output_t d, size_t bytes_transferred)
  {
    t.get_aspect().template gete<_on_complete_>()(t, std::ref(*d), bytes_transferred );
    t.get_aspect().template get<_more_>()(t, std::move(d), bytes_transferred);
  }
};

}}}