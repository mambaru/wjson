#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <iow/asio.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{

struct ad_sync_resolver
{
  template<typename T, typename Opt>
  ::iow::asio::ip::tcp::endpoint operator()(T& t, const Opt& opt)
  {
    ::iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    ::iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      opt.addr, 
      opt.port
    });
    return endpoint;
  }
};

}}}}
