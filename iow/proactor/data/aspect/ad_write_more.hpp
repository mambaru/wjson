#pragma once

#include <iow/proactor/data/aspect/tags.hpp>

namespace iow{ namespace proactor{
  
struct ad_write_more
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    /*
    typename T::data_ptr d = t.get_aspect().template get< _prepare_data_ >()(t, std::move(d) );
    t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
    */
  }
};

}}