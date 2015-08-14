#pragma once
#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{
  
struct options:
  ::iow::io::client::options< ::iow::ip::tcp::connection::options >
{
  std::string port;
  int backlog = 1024;
};

}}}}

