#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_listen
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    const auto endpoint = t.get_aspect().template get<_sync_resover_>()(t, opt);

    // такая же последовательность для local::stream_protocol::acceptor
    t.descriptor().open(endpoint.protocol());
    t.get_aspect().template gete<_set_acceptor_options_>()(t);
    t.descriptor().bind(endpoint);
    t.descriptor().listen( opt.backlog );
  }
};

}}}
