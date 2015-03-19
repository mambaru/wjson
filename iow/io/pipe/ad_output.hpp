#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace pipe{
  
struct ad_output
{
  template<typename T>
  void operator()(T& t, typename T::output_t d)
  {
    t.get_aspect().template get<_more_>()(t, std::move(d), 0);
  }
};

}}}