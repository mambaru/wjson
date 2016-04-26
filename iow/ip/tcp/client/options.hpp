#pragma once
#include <iow/io/client/options.hpp>
#include <iow/ip/tcp/connection/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct options:
  public ::iow::io::client::options< ::iow::ip::tcp::connection::options >
{
  
};

}}}}

