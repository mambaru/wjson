#pragma once

#include <iow/io/client/multi_client.hpp>
#include <iow/io/client/mtclient.hpp>
#include <iow/io/client/client.hpp>

#include <iow/ip/tcp/client/connection.hpp>
#include <iow/ip/tcp/client/aspect.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

template< typename Connection = connection_base<>, typename A = fas::aspect<> >
using client = ::iow::io::client::client< Connection, A >;
  
template< typename Client = client<> >
using mtclient = ::iow::io::client::mtclient< Client >;

template< typename Client = mtclient<> >
using multi_client = ::iow::io::client::multi_client< Client >;

}}}}
