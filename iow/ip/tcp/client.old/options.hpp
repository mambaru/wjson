#pragma once

#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{
  
struct options
{
  ::iow::ip::tcp::connection::options connection;
  std::string host;
  std::string port;
  int count_per_thread = 1;
  int threads = 0;
};

}}}}

