#pragma once

#include <iow/ip/tcp/acceptor/acceptor.hpp>
#include <iow/ip/tcp/server/options.hpp>
#include <iow/io/basic/tags.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

struct _context_;
struct _io_service_type_;

template<typename AcceptorType>
struct context
{
  typedef AcceptorType acceptor_type;
  typedef std::shared_ptr<acceptor_type> acceptor_ptr;
  
  acceptor_ptr acceptor;
};

struct _server_start_;
struct ad_server_start
{
  template<typename T>
  void operator()( T& )
  {
    
  }
};

struct ad_initialize
{
  template<typename T, typename O>
  void operator()( T& t, O&& opt )
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::acceptor_type acceptor_type;
    typedef typename acceptor_type::descriptor_type descriptor_type;

    auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.acceptor==nullptr )
    {
      cntx.acceptor = std::make_shared<acceptor_type>( std::move( descriptor_type( t.get_io_service() ) ) );
    }
    //cntx.acceptor->initialize(opt); 
    //std::cout << "server::ad_initialize sep=[" << opt.connection_options.reader.sep.size() << "]" << std::endl;
    cntx.acceptor->start(opt);
  }
};


typedef ::iow::ip::tcp::acceptor::acceptor<> acceptor;

template<typename AcceptorType = acceptor>
struct aspect: fas::aspect<
  fas::type< _io_service_type_, ::iow::asio::io_service>,
  fas::type< ::iow::io::_options_type_, options>,
  fas::value< _context_, context<AcceptorType> >,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< _server_start_, ad_server_start>,
  fas::group< ::iow::io::_after_start_, _server_start_ >
  
>{};
  
}}}}
