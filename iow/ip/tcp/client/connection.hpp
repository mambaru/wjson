#pragma once

#include <iow/io/client/connection.hpp>
#include <iow/ip/tcp/client/aspect.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

template<typename A = fas::aspect<> >
using connection =
  ::iow::io::client::connection< typename fas::merge_aspect<A, aspect >::type >;

template<typename A = fas::aspect<> >
using connection_base =
  ::iow::io::client::connection_base< typename fas::merge_aspect<A, aspect >::type >;
  
}}}}
