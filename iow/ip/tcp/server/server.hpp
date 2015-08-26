#pragma once

#include <iow/io/server/server.hpp>
#include <iow/ip/tcp/acceptor/acceptor.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

typedef ::iow::ip::tcp::acceptor::acceptor<> acceptor;
  
template<typename Acceptor = acceptor >
using server = ::iow::io::server::server<acceptor>;

}}}}
