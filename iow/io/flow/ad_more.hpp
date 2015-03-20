#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace flow{
  
struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< _create_ >()(t);
    t.get_aspect().template get< _some_ >()(t, std::move(d) );
    /*
    if ( d!=nullptr )
    {
      t.get_aspect().template get< _some_ >()(t, std::move(d) );
    }
    else
    {
      t.get_aspect().template gete< _on_done_ >()(t);
    }*/
  }
};

}}}