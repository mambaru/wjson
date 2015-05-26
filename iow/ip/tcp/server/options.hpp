#pragma once

#include <iow/ip/tcp/acceptor/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{
  
struct options:
  ::iow::ip::tcp::acceptor::options
{
  int threads = 0;
};

}}}}

