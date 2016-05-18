#pragma once

#include <iow/io/client/multi_thread.hpp>
#include <iow/io/client/multi_client.hpp>
#include <iow/io/client/thread.hpp>
#include <iow/io/client/client.hpp>

#include <iow/ip/tcp/client/connection.hpp>
#include <iow/ip/tcp/client/aspect.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

template< typename Connection = connection_base<> >
using client = ::iow::io::client::client< Connection >;
  
template< typename Client = client<> >
using multi_client = ::iow::io::client::multi_client< Client >;

template< typename Client = multi_client<> >
using thread = ::iow::io::client::thread< Client >;

template< typename Client = thread<> >
using multi_thread = ::iow::io::client::multi_thread< Client >;


}}}}
