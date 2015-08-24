#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <iow/asio.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{

struct ad_sync_resover
{
  template<typename T>
  ::iow::asio::ip::tcp::endpoint operator()(T& t)
  {
    using ::iow::io::acceptor::_context_;
    const auto& context = t.get_aspect().template get<_context_>();
    ::iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    ::iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      context.addr, 
      context.port
    });
    return endpoint;
  }
};

}}}}
