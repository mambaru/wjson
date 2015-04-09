#pragma once

#include <iow/io/reader/tags.hpp>

namespace iow{ namespace io{ namespace reader{

struct ad_some
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    auto handler = t.get_aspect().template get<_make_handler_>()(t, p);
    t.get_aspect().template get<_read_some_>()(t, std::move(p), std::move(handler));
  }
};

}}}
