#pragma once
#include <iow/io/acceptor/options.hpp>
#include <iow/ip/tcp/connection/options.hpp>
#include <iow/ip/tcp/acceptor/connection.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{
  
template<
  typename ConnectionType = ::iow::ip::tcp::acceptor::connection, 
  typename ConnectionOptions = ::iow::ip::tcp::connection::options 
>
using options =  ::iow::io::acceptor::options< ConnectionType, ConnectionOptions>;

}}}}

