#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_acceptor_start
{
  template<typename T, typename Endpoint>
  void operator()(T& t, Endpoint endpoint)
  {
    const auto& context = t.get_aspect().template get<_context_>();
    
    // такая же последовательность для local::stream_protocol::acceptor
    t.descriptor().open(endpoint.protocol());
    t.get_aspect().template gete<_set_acceptor_options_>()(t);
    t.descriptor().bind(endpoint);
    t.descriptor().listen( context.backlog );
  }
};

}}}
