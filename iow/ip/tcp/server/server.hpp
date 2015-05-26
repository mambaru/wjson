#pragma once

#include <iow/ip/tcp/acceptor/acceptor.hpp>
#include <iow/ip/tcp/server/aspect.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

struct acceptor: ::iow::ip::tcp::acceptor::acceptor<>{};

/*
template< typename AcceptorType = acceptor, typename A = fas::aspect<> >
using server = ::iow::io::io_base< typename fas::merge_aspect< A, aspect<AcceptorType> >::type >;
*/

template< typename AcceptorType = acceptor, typename A = fas::aspect<> >
class server
  : public ::iow::io::io_base< typename fas::merge_aspect< A, aspect<AcceptorType> >::type >
{
public:
  
  server(::iow::asio::io_service& io)
    :_io_service(io)
  {
  }
  
  ::iow::asio::io_service& get_io_service() 
  {
    return _io_service;
  }
  
public:
  
  ::iow::asio::io_service& _io_service;
};

}}}}
