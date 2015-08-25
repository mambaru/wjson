#pragma once

#include <iow/ip/tcp/server/aspect.hpp>
#include <iow/io/basic_io.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace mtio{

template< typename AcceptorType = acceptor, typename A = fas::aspect<> >
class mtio
  : private ::iow::io::basic_io< typename fas::merge_aspect< A, aspect1<AcceptorType> >::type >
{
public:
  typedef ::iow::io::basic_io< typename fas::merge_aspect< A, aspect1<AcceptorType> >::type > super;
  typedef typename super::aspect::template advice_cast<_io_service_type_>::type io_service_type;
  
  typedef typename super::aspect aspect;
  using super::get_aspect;

  mtio( io_service_type& io)
    : _io_service(io)
  {
  }

  io_service_type& get_io_service() 
  {
    return _io_service;
  }

  
  template<typename O>
  void start(O opt)
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex());
    //opt.acceptor.connection_options = opt.connection;
    this->start_(*this, opt);
  }

public:
  io_service_type& _io_service;
};

}}}}
