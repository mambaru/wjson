#pragma once

#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{
  
struct options:
  ::iow::ip::tcp::connection::options
{
  std::string host;
  std::string port;
};

}}}}

