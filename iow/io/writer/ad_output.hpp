#pragma once

#include <iow/io/writer/tags.hpp>

namespace iow{ namespace io{ namespace writer{

struct ad_output
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    t.get_aspect().template get<_attach_>()(t, std::move(d) );
    t.get_aspect().template get<_more_>()(t);
  }
};

}}}