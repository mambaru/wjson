#pragma once

#include <iow/asio.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{

struct ad_set_reuse_address
{
  template<typename T>
  void operator()(T& t)
  {
    // такая же для local::stream_protocol::acceptor
    t.descriptor().set_option( iow::asio::ip::tcp::acceptor::reuse_address(true) );
  }
};

}}}}
