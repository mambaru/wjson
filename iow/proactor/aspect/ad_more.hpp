#pragma once

#include <iow/proactor/aspect/tags.hpp>

namespace iow{ namespace proactor{
  
struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "ad_more" << std::endl;
    auto d = t.get_aspect().template get< _make_data_ >()(t);
    t.get_aspect().template get< _some_ >()(t, std::move(d) );
  }
};

}}