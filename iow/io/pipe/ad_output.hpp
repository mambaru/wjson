#pragma once

#include <iow/io/flow/tags.hpp>

namespace iow{ namespace io{ namespace pipe{

struct ad_output
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    t.get_aspect().template get<_entry_>()(t, std::move(d) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}