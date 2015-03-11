#pragma once

#include <iow/pipeline/aspect/tags.hpp>

namespace iow{ 
  
struct ad_run_line
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "run line" << std::endl;
    t.get_aspect().template gete<_on_line_>()(t);
    t.get_aspect().template get<_read_more_>()(t);
  }
};

}