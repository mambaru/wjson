#pragma once

#include <iow/proactor/aspect/tags.hpp>

namespace iow{ 
  
struct ad_read_more
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "ad_read_more" << std::endl;
    auto d = t.get_aspect().template get< _make_data_ >()(t);
    t.get_aspect().template get< _read_some_ >()(t, std::move(d) );
  }
};

}