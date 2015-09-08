#pragma once

#include <iow/ip/tcp/client/connection.hpp>
#include <iow/ip/tcp/client/aspect.hpp>

#include <iow/io/client/client.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

template< typename ConnectionType = connection<> >
using client = ::iow::io::client::client< ConnectionType >;
  
}}}}
