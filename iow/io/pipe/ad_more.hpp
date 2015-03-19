#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace pipe{

struct ad_more
{
  template<typename T>
  void operator()(T& t, typename T::output_t d, size_t bytes_transferred)
  {
    auto dd = t.get_aspect().template get< _factory_ >()(t, std::move(d), bytes_transferred);
    if ( dd!=nullptr )
    {
      t.get_aspect().template get< _some_ >()(t, std::move(dd) );
    }
    else
    {
      t.get_aspect().template gete< _on_done_ >()(t);
    }
  }
};

}}}