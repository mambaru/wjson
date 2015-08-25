#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_resolve_and_start
{
  template<typename T>
  void operator()(T& t)
  {
    auto endpoint = t.get_aspect().template get<_sync_resover_>()(t);
    t.get_aspect().template get<_acceptor_start_>()(t, endpoint);
  }
};

}}}
