#pragma once

#include <iow/proactor/aspect/tags.hpp>

namespace iow{ namespace proactor{
  
struct ad_run
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "run line" << std::endl;
    t.get_aspect().template gete<_on_run_>()(t);
    t.get_aspect().template get<_more_>()(t);
  }
};

}}