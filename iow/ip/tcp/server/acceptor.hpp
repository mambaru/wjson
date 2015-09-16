#pragma once

#include <iow/ip/tcp/connection/connection.hpp>
#include <iow/ip/tcp/acceptor/acceptor.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

template<
  typename ConnectionType = ::iow::ip::tcp::connection::connection<>,
  typename A = fas::aspect<>
>
using acceptor = ::iow::ip::tcp::acceptor::acceptor< ConnectionType, A>;

}}}}
