#pragma once
#include <iow/io/acceptor/options.hpp>
#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{
  
struct options:
  ::iow::io::acceptor::options< ::iow::ip::tcp::connection::options >
{
  //std::string port;
  // int backlog = 1024;
};

}}}}

