#pragma once

#include <iow/io/server/server.hpp>
#include <iow/ip/tcp/server/acceptor.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

template<typename Acceptor = acceptor<> >
using server = ::iow::io::server::server<Acceptor>;

}}}}
