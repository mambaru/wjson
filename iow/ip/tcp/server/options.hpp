#pragma once

#include <iow/ip/tcp/acceptor/options.hpp>
#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{
  
struct options
  : public ::iow::ip::tcp::acceptor::options
{
  //::iow::ip::tcp::acceptor::options acceptor;
  //::iow::ip::tcp::connection::options connection;
  int threads = 0;
};

}}}}

