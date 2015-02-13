#pragma once

#include <iow/pipeline/aspect/tags.hpp>

namespace iow{ 
  
struct ad_write_more
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto d = t.get_aspect().template get< _prepare_buffer_ >()(t, d);
    t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
  }
};

}