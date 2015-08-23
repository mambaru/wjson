#pragma once

#include <iow/io/connection/connection.hpp>
#include <iow/ip/tcp/connection/aspect.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace connection{

template<typename A = fas::aspect<> >
using connection =
  ::iow::io::connection::connection< typename fas::merge_aspect<A, aspect >::type >;

}}}}
