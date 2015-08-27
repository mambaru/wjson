#pragma once

#include <iow/io/client/connection.hpp>
#include <iow/ip/tcp/client/options.hpp>
#include <iow/ip/tcp/connection/aspect.hpp>
#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>
#include <iow/system.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct ad_sync_resolver
{
  template<typename T, typename Opt>
  ::iow::asio::ip::tcp::endpoint operator()(T& t, const Opt& opt)
  {
    ::iow::system::error_code ec;
    ::iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    ::iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      opt.addr, 
      opt.port
    }, ec);
    IOW_LOG_MESSAGE("Client Reslove: " << opt.addr << ":" << opt.port << " " << ec.message())
    return endpoint;
  }
};


struct aspect : fas::aspect<
  fas::type< ::iow::io::_options_type_, options >,
  fas::advice< ::iow::io::client::_sync_resolver_, ad_sync_resolver>,
  ::iow::ip::tcp::connection::aspect
>{};
  
}}}}
